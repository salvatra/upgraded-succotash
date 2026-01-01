#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "queries/queries.h"
#include "core/dataset.h"
#include "core/fenwick.h"
#include "queries/query1.h"
#include "queries/query2.h"
#include "queries/query3.h"
#include "queries/query4.h"
#include "queries/query5.h"
#include "queries/query6.h"



int build_query_context(Dataset *ds, GPtrArray **aircraftsArray,
                        int **flightCounts, GHashTable **airportFtrees,
                        Q4Struct **q4_data, GList **airlineDelays,
                        GHashTable **natTable)
{
  if (!ds)
    return 1;

  *aircraftsArray = g_ptr_array_new();
  GPtrArray *flightsArray = g_ptr_array_new(); // Needed for Q5 context
  GHashTableIter iter;
  gpointer key, value;

  g_hash_table_iter_init(&iter, (GHashTable *)getDatasetAircrafts(ds));
  while (g_hash_table_iter_next(&iter, &key, &value))
    g_ptr_array_add(*aircraftsArray, value);

  g_hash_table_iter_init(&iter, (GHashTable *)getDatasetFlights(ds));
  while (g_hash_table_iter_next(&iter, &key, &value))
    g_ptr_array_add(flightsArray, value);

  int numAircrafts = (*aircraftsArray)->len;
  *flightCounts = calloc(numAircrafts, sizeof(int));

  GHashTable *idToIndex = g_hash_table_new(g_str_hash, g_str_equal);
  for (int i = 0; i < numAircrafts; i++)
  {
    Aircraft *ac = g_ptr_array_index(*aircraftsArray, i);
    g_hash_table_insert(idToIndex, (gpointer)getAircraftId(ac), GINT_TO_POINTER(i + 1));
  }

  for (guint i = 0; i < flightsArray->len; i++)
  {
    Flight *f = g_ptr_array_index(flightsArray, i);
    if (!f || strcmp(getFlightStatus(f), "Cancelled") == 0)
      continue;

    const char *acId = getFlightAircraft(f);
    if (!acId)
      continue;

    gpointer idxPtr = g_hash_table_lookup(idToIndex, acId);
    if (idxPtr)
    {
      int idx = GPOINTER_TO_INT(idxPtr) - 1;
      (*flightCounts)[idx]++;
    }
  }
  g_hash_table_destroy(idToIndex);

  GHashTable *dates = getDatesTable(ds);
  *airportFtrees = getFTrees(dates, (GHashTable *)getDatasetFlights(ds));
  g_hash_table_destroy(dates);

  *q4_data = init_Q4_structure((GHashTable *)getDatasetReservations(ds),
                               (GHashTable *)getDatasetFlights(ds));

  *airlineDelays = prepareAirlineDelays(flightsArray);

  *natTable = prepareNationalityData(getDatasetPassengers(ds),
                                     getDatasetReservations(ds),
                                     (GHashTable *)getDatasetFlights(ds));

  g_ptr_array_free(flightsArray, TRUE);
  return 0;
}

void runAllQueries(Dataset *ds, const char *filePath,
                   QueryStatsCallback callback, void *ctx)
{

  GPtrArray *flightsArray = g_ptr_array_new();
  GPtrArray *aircraftsArray = g_ptr_array_new();
  GHashTableIter iter;
  gpointer key, value;

  g_hash_table_iter_init(&iter, (GHashTable *)getDatasetFlights(ds));
  while (g_hash_table_iter_next(&iter, &key, &value))
    g_ptr_array_add(flightsArray, value);

  g_hash_table_iter_init(&iter, (GHashTable *)getDatasetAircrafts(ds));
  while (g_hash_table_iter_next(&iter, &key, &value))
    g_ptr_array_add(aircraftsArray, value);

  int numAircrafts = aircraftsArray->len;
  int *flightCounts = calloc(numAircrafts, sizeof(int));

  GHashTable *idToIndex = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
  for (int i = 0; i < numAircrafts; i++)
  {
    Aircraft *ac = g_ptr_array_index(aircraftsArray, i);
    g_hash_table_insert(idToIndex, g_strdup(getAircraftId(ac)), GINT_TO_POINTER(i));
  }
  for (size_t i = 0; i < flightsArray->len; i++)
  {
    Flight *f = g_ptr_array_index(flightsArray, i);
    if (!f || strcmp(getFlightStatus(f), "Cancelled") == 0)
      continue;
    const char *acId = getFlightAircraft(f);
    if (!acId)
      continue;
    gpointer idxPtr = g_hash_table_lookup(idToIndex, acId);
    if (idxPtr)
      flightCounts[GPOINTER_TO_INT(idxPtr)]++;
  }
  g_hash_table_destroy(idToIndex);

  GHashTable *airportDates = getDatesTable(ds);
  GHashTable *airportFtrees = getFTrees(airportDates, (GHashTable *)getDatasetFlights(ds));
  GList *airlineDelays = prepareAirlineDelays(flightsArray);
  GHashTable *natTable = prepareNationalityData(getDatasetPassengers(ds),
                                                getDatasetReservations(ds), getDatasetFlights(ds));
  Q4Struct *q4_data = init_Q4_structure((GHashTable *)getDatasetReservations(ds),
                                        (GHashTable *)getDatasetFlights(ds));

  FILE *inputFile = fopen(filePath, "r");
  if (!inputFile)
  {
    perror("Failed to open inputs file");
    destroy_Q4_structure(q4_data);
    freeAirlineDelays(airlineDelays);
    g_hash_table_destroy(airportFtrees);
    g_hash_table_destroy(airportDates);
    g_ptr_array_free(flightsArray, TRUE);
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
    line[strcspn(line, "\n")] = '\0'; // remove \n

    char queryStr[8];
    int lenRead = 0;
    if (sscanf(line, "%7s%n", queryStr, &lenRead) < 1)
    {
      lineNumber++;
      continue;
    }

    int isSpecial = 0;
    char separator = ';';
    size_t len = strlen(queryStr);
    if (len > 0 && queryStr[len - 1] == 'S')
    {
      isSpecial = 1;
      separator = '=';
      queryStr[len - 1] = '\0';
    }
    int queryNumber = atoi(queryStr);

    char *arg1 = line + lenRead;
    g_strstrip(arg1);
    char *arg2 = NULL;
    if (queryNumber == 2 || queryNumber == 3 || queryNumber == 4)
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
      gchar *res = query1(arg1, getDatasetAirports(ds), getDatasetAiportStats(ds));
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
      AircraftStats **top = query2(N, aircraftsArray, arg2 && strlen(arg2) > 0 ? arg2 : NULL, &size, flightCounts);
      if (top && size > 0)
      {
        for (int i = 0; i < size; i++)
          fprintf(output, "%s%c%s%c%s%c%d\n",
                  get_aircraftstats_id(top[i]), separator,
                  get_aircraftstats_manufacturer(top[i]), separator,
                  get_aircraftstats_model(top[i]), separator,
                  get_aircraftstats_count(top[i]));
        free_aircraftstats_array(top, size);
      }
      else
        fprintf(output, "\n");
    }
    else if (queryNumber == 3)
    {
      gchar *result = query3(airportFtrees, (GHashTable *)getDatasetAirports(ds), arg1, arg2);
      if (result)
      {
        if (isSpecial)
          for (char *p = result; *p; p++)
            if (*p == ';')
              *p = '=';
        fprintf(output, "%s\n", result);
        g_free(result);
      }
      else
        fprintf(output, "\n");
    }
    else if (queryNumber == 4)
    {
      query4(q4_data, (GHashTable *)getDatasetPassengers(ds), arg1, arg2, output, isSpecial);
    }
    else if (queryNumber == 5)
    {
      if (strlen(arg1) == 0)
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
      if (strlen(arg1) == 0)
        fprintf(output, "\n");
      else if (query_Q6(natTable, arg1, output, isSpecial) == 0)
        fprintf(output, "\n");
    }
    else
    {
      fprintf(output, "\n");
    }

    fclose(output);
    gdouble elapsed = g_timer_elapsed(queryTimer, NULL);
    g_timer_destroy(queryTimer);
    if (callback)
      callback(queryNumber, lineNumber, elapsed, ctx);
    lineNumber++;
  }

  gdouble totalElapsed = g_timer_elapsed(totalTimer, NULL);
  if (!callback)
    printf("Total time of all queries: %.6f seconds\n", totalElapsed);
  g_timer_destroy(totalTimer);
  fclose(inputFile);

  destroy_Q4_structure(q4_data);
  freeAirlineDelays(airlineDelays);
  g_hash_table_destroy(airportFtrees);
  g_hash_table_destroy(airportDates);
  g_ptr_array_free(flightsArray, TRUE);
  g_ptr_array_free(aircraftsArray, TRUE);
  g_hash_table_destroy(natTable);
  free(flightCounts);
}