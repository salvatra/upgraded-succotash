#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "queries/queries.h"
#include "core/dataset.h"
#include "core/fenwick.h"
#include "core/indexer.h"
#include "entities/access/aircrafts_access.h"
#include "entities/access/flights_access.h"
#include "entities/access/passengers_access.h"
#include "entities/access/reservations_access.h"
#include "queries/query1.h"
#include "queries/query2.h"
#include "queries/query3.h"
#include "queries/query4.h"
#include "queries/query5.h"
#include "queries/query6.h"

static void build_arrays(const Dataset *ds, GPtrArray **aircrafts, GPtrArray **flights)
{
  *aircrafts = g_ptr_array_new();
  *flights = g_ptr_array_new();

  DatasetIterator *it = dataset_aircraft_iterator_new(ds);
  const Aircraft *ac;
  while ((ac = dataset_iterator_next(it)) != NULL)
  {
    g_ptr_array_add(*aircrafts, (gpointer)ac);
  }
  dataset_iterator_free(it);

  it = dataset_flight_iterator_new(ds);
  const Flight *f;
  while ((f = dataset_iterator_next(it)) != NULL)
  {
    g_ptr_array_add(*flights, (gpointer)f);
  }
  dataset_iterator_free(it);
}

int build_query_context(Dataset *ds, GPtrArray **aircraftsArray,
                        int **flightCounts, GHashTable **airportFtrees,
                        Q4Struct **q4_data, GList **airlineDelays,
                        GHashTable **natTable)
{
  if (!ds)
    return 1;

  GPtrArray *flightsArray = NULL;

  // 1. Build arrays
  build_arrays(ds, aircraftsArray, &flightsArray);

  int numAircrafts = (*aircraftsArray)->len;
  *flightCounts = calloc(numAircrafts, sizeof(int));

  GHashTable *idToIndex = g_hash_table_new(g_str_hash, g_str_equal);
  for (int i = 0; i < numAircrafts; i++)
  {
    const Aircraft *ac = g_ptr_array_index(*aircraftsArray, i);
    g_hash_table_insert(idToIndex, (gpointer)getAircraftId(ac), GINT_TO_POINTER(i + 1));
  }

  for (guint i = 0; i < flightsArray->len; i++)
  {
    const Flight *f = g_ptr_array_index(flightsArray, i);
    if (!f || strcmp(getFlightStatus(f), "Cancelled") == 0)
      continue;

    const char *acId = getFlightAircraft(f);
    if (!acId)
      continue;

    gpointer idxPtr = g_hash_table_lookup(idToIndex, acId);
    if (idxPtr)
    {
      (*flightCounts)[GPOINTER_TO_INT(idxPtr) - 1]++;
    }
  }
  g_hash_table_destroy(idToIndex);

  // 2. Indexing and preload
  GHashTable *dates = create_date_index(ds);
  *airportFtrees = getFTrees(dates, ds);
  g_hash_table_destroy(dates);

  *q4_data = init_Q4_structure(ds);
  *airlineDelays = prepareAirlineDelays(flightsArray);
  *natTable = prepareNationalityData(ds);

  g_ptr_array_free(flightsArray, TRUE);
  return 0;
}

void runAllQueries(Dataset *ds, const char *filePath,
                   QueryStatsCallback callback, void *ctx)
{
  GPtrArray *aircraftsArray = NULL;
  int *flightCounts = NULL;
  GHashTable *airportFtrees = NULL;
  Q4Struct *q4_data = NULL;
  GList *airlineDelays = NULL;
  GHashTable *natTable = NULL;

  if (build_query_context(ds, &aircraftsArray, &flightCounts, &airportFtrees,
                          &q4_data, &airlineDelays, &natTable) != 0)
  {
    perror("Failed to build query context");
    return;
  }

  FILE *inputFile = fopen(filePath, "r");
  if (!inputFile)
  {
    perror("Failed to open inputs file");
    destroy_Q4_structure(q4_data);
    freeAirlineDelays(airlineDelays);
    g_hash_table_destroy(airportFtrees);
    g_ptr_array_free(aircraftsArray, TRUE);
    g_hash_table_destroy(natTable);
    free(flightCounts);
    return;
  }

  GTimer *totalTimer = g_timer_new();
  char line[512];
  int lineNumber = 1;

  while (fgets(line, sizeof(line), inputFile))
  {
    line[strcspn(line, "\n")] = '\0';

    char queryStr[8];
    int lenRead = 0;
    if (sscanf(line, "%7s%n", queryStr, &lenRead) < 1)
    {
      lineNumber++;
      continue;
    }

    int isSpecial = 0;
    size_t len = strlen(queryStr);
    if (len > 0 && queryStr[len - 1] == 'S')
    {
      isSpecial = 1;
      queryStr[len - 1] = '\0';
    }
    int queryNumber = atoi(queryStr);
    char *arg1 = line + lenRead;
    g_strstrip(arg1);
    char *arg2 = NULL;
    if (queryNumber >= 2 && queryNumber <= 4)
    {
      char *space = strchr(arg1, ' ');
      if (space)
      {
        *space = '\0';
        arg2 = space + 1;
        g_strstrip(arg2);
      }
    }

    char outputFileName[128];
    snprintf(outputFileName, sizeof(outputFileName), "resultados/command%d_output.txt", lineNumber);
    FILE *output = fopen(outputFileName, "w");
    if (!output)
    {
      lineNumber++;
      continue;
    }

    GTimer *queryTimer = g_timer_new();

    if (queryNumber == 1)
    {
      gchar *res = query1(arg1, ds);
      if (res)
      {
        if (isSpecial)
          for (char *p = res; *p; p++)
            if (*p == ';')
              *p = '=';
        fprintf(output, "%s\n", res);
        g_free(res);
      }
      else
        fprintf(output, "\n");
    }
    else if (queryNumber == 2)
    {
      int N = atoi(arg1);
      int size;
      char sep = isSpecial ? '=' : ';';
      AircraftStats **top = query2(N, aircraftsArray, (arg2 && *arg2) ? arg2 : NULL, &size, flightCounts);

      if (size > 0 && top)
      {
        for (int i = 0; i < size; i++)
        {
          fprintf(output, "%s%c%s%c%s%c%d\n",
                  get_aircraftstats_id(top[i]), sep,
                  get_aircraftstats_manufacturer(top[i]), sep,
                  get_aircraftstats_model(top[i]), sep,
                  get_aircraftstats_count(top[i]));
        }
      }
      else
      {
        fprintf(output, "\n");
      }

      free_aircraftstats_array(top, size);
    }
    else if (queryNumber == 3)
    {
      gchar *res = query3(airportFtrees, ds, arg1, arg2);
      if (res)
      {
        if (isSpecial)
          for (char *p = res; *p; p++)
            if (*p == ';')
              *p = '=';
        fprintf(output, "%s\n", res);
        g_free(res);
      }
      else
        fprintf(output, "\n");
    }
    else if (queryNumber == 4)
    {
      query4(q4_data, ds, arg1, arg2, output, isSpecial);
    }
    else if (queryNumber == 5)
    {
      if (!*arg1)
        fprintf(output, "\n");
      else
      {
        int N = atoi(arg1);
        if (N <= 0)
          fprintf(output, "\n");
        else if (query5(airlineDelays, N, output, isSpecial) == 0)
          fprintf(output, "\n");
      }
    }
    else if (queryNumber == 6)
    {
      if (!*arg1)
        fprintf(output, "\n");
      else if (query_Q6(natTable, arg1, output, isSpecial) == 0)
        fprintf(output, "\n");
    }
    else
      fprintf(output, "\n");

    fclose(output);
    gdouble elapsed = g_timer_elapsed(queryTimer, NULL);
    g_timer_destroy(queryTimer);
    if (callback)
      callback(queryNumber, lineNumber, elapsed, ctx);
    lineNumber++;
  }

  // Cleanup
  g_timer_destroy(totalTimer);
  fclose(inputFile);

  destroy_Q4_structure(q4_data);
  freeAirlineDelays(airlineDelays);
  g_hash_table_destroy(airportFtrees);
  g_ptr_array_free(aircraftsArray, TRUE);
  g_hash_table_destroy(natTable);
  free(flightCounts);
}