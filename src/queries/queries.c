#include <core/dataset.h>
#include <core/fenwick.h>
#include <glib.h>
#include <queries/queries.h>
#include <queries/query1.h>
#include <queries/query2.h>
#include <queries/query3.h>
#include <queries/query4.h>
#include <queries/query5.h>
#include <queries/query6.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void specialize(int isSpecial, char *str) {
  if (isSpecial) {
    for (char *p = str; *p; p++)
      if (*p == ';')
        *p = '=';
  }
}

int query1wrapper(char *code, int isSpecial, FILE *stream, Dataset *ds) {
  gchar *res = query1(code, getDatasetAirports(ds), getDatasetAiportStats(ds));

  if (res) {
    specialize(isSpecial, res);

    fprintf(stream, "%s\n", res);
    g_free(res);
  } else {
    fprintf(stream, "\n");
    return 1;
  }

  return 0;
}

int query2wrapper(char *number, char *manufacturer, FILE *stream,
                  GPtrArray *aircraftsArray, int *flightCounts, int isSpecial) {
  int N = atoi(number);
  if (N == 0)
    return 1;
  int size;
  int r = 0;

  AircraftStats **top =
      query2(N, aircraftsArray, strlen(manufacturer) > 0 ? manufacturer : NULL,
             &size, flightCounts);

  char separator = (isSpecial) ? '=' : ';';

  if (top && size > 0) {
    for (int i = 0; i < size; i++) {
      fprintf(stream, "%s%c%s%c%s%c%d\n", get_aircraftstats_id(top[i]),
              separator, get_aircraftstats_manufacturer(top[i]), separator,
              get_aircraftstats_model(top[i]), separator,
              get_aircraftstats_count(top[i]));
    }
    free_aircraftstats_array(top, size);
  } else {
    fprintf(stream, "\n");
    r = 2;
  }

  return r;
}

int query3wrapper(char *arg1, char *arg2, FILE *stream, Dataset *ds,
                  int isSpecial, GHashTable *airportFtrees) {
  GHashTable *airports = (GHashTable *)getDatasetAirports(ds);

  gchar *result = query3(airportFtrees, airports, arg1, arg2);

  if (result) {
    if (isSpecial) {
      for (char *p = result; *p; p++)
        if (*p == ';')
          *p = '=';
    }
    fprintf(stream, "%s\n", result);
    g_free(result);
  } else {
    fprintf(stream, "\n");
    return 1;
  }

  return 0;
}

int query5wrapper(char *arg1, FILE *stream, GList *airlineDelays,
                  int isSpecial) {
  if (strlen(arg1) == 0) {
    fprintf(stream, "\n");
    return 1;
  } else {
    int N = atoi(arg1);
    if (N <= 0) {
      fprintf(stream, "\n");
      return 2;
    } else {
      if (query5(airlineDelays, N, stream, isSpecial) == 0)
        return 3;
    }
  }

  return 0;
}

int query6wrapper(char *arg1, FILE *stream, GHashTable *natTable,
                  int isSpecial) {
  if (strlen(arg1) == 0) {
    fprintf(stream, "\n");
    return 1;
  } else {
    if (query_Q6(natTable, arg1, stream, isSpecial) == 0)
      return 2;
  }

  return 0;
}

int build_query_context(Dataset *ds, GPtrArray **aircraftsArray,
                        int **flightCounts, GHashTable **airportFtrees,
                        GList **airlineDelays, GHashTable **natTable) {
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
  for (int i = 0; i < numAircrafts; i++) {
    Aircraft *ac = g_ptr_array_index(*aircraftsArray, i);
    g_hash_table_insert(idToIndex, (gpointer)getAircraftId(ac),
                        GINT_TO_POINTER(i + 1));
  }

  // Calculate flight counts using the array we just created
  for (guint i = 0; i < flightsArray->len; i++) {
    Flight *f = g_ptr_array_index(flightsArray, i);

    if (!f || strcmp(getFlightStatus(f), "Cancelled") == 0)
      continue;

    const char *acId = getFlightAircraft(f);
    if (!acId)
      continue;

    gpointer idxPtr = g_hash_table_lookup(idToIndex, acId);
    if (idxPtr) {
      int idx = GPOINTER_TO_INT(idxPtr) - 1;
      (*flightCounts)[idx]++;
    }
  }

  g_hash_table_destroy(idToIndex);

  // Query 3 Context
  GHashTable *dates = getDatesTable(ds);
  *airportFtrees = getFTrees(dates, (GHashTable *)getDatasetFlights(ds));
  g_hash_table_destroy(dates);

  // Query 5 Context
  *airlineDelays = prepareAirlineDelays(flightsArray);

  // Query 6 Context
  *natTable = prepareNationalityData(getDatasetPassengers(ds),
                                     getDatasetReservations(ds),
                                     (GHashTable *)getDatasetFlights(ds));

  g_ptr_array_free(flightsArray, TRUE);

  return 0;
}

void runAllQueries(Dataset *ds, const char *filePath,
                   QueryStatsCallback callback, void *ctx) {

  // ===================== Data Preparation =====================
  GPtrArray *flightsArray = g_ptr_array_new();
  GPtrArray *aircraftsArray = g_ptr_array_new();

  GHashTableIter iter;
  gpointer key, value;

  // Copy flights to array
  g_hash_table_iter_init(&iter, (GHashTable *)getDatasetFlights(ds));
  while (g_hash_table_iter_next(&iter, &key, &value))
    g_ptr_array_add(flightsArray, value);

  // Copy aircrafts to array
  g_hash_table_iter_init(&iter, (GHashTable *)getDatasetAircrafts(ds));
  while (g_hash_table_iter_next(&iter, &key, &value))
    g_ptr_array_add(aircraftsArray, value);

  int numAircrafts = aircraftsArray->len;
  int *flightCounts = calloc(numAircrafts, sizeof(int));

  // Map to count flights per aircraft
  GHashTable *idToIndex = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
  for (int i = 0; i < numAircrafts; i++) {
    Aircraft *ac = g_ptr_array_index(aircraftsArray, i);
    g_hash_table_insert(idToIndex, g_strdup(getAircraftId(ac)),
                        GINT_TO_POINTER(i));
  }

  for (size_t i = 0; i < flightsArray->len; i++) {
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
  GHashTable *airportFtrees =
      getFTrees(airportDates, (GHashTable *)getDatasetFlights(ds));

  GList *airlineDelays = prepareAirlineDelays(flightsArray); // Query 5

  // Pre-processing for query 6
  GHashTable *natTable =
      prepareNationalityData(getDatasetPassengers(ds),
                             getDatasetReservations(ds), getDatasetFlights(ds));

  // Pre-processing for query 4
  Q4Struct *q4_data = init_Q4_structure((GHashTable *)getDatasetReservations(ds), 
                                        (GHashTable *)getDatasetFlights(ds));

  // ===================== Input reading =====================
  FILE *inputFile = fopen(filePath, "r");
  if (!inputFile) {
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

  while (fgets(line, sizeof(line), inputFile)) {
    line[strcspn(line, "\n")] = '\0'; // remove \n

    char queryStr[8];
    int lenRead = 0;

    if (sscanf(line, "%7s%n", queryStr, &lenRead) < 1) {
      lineNumber++;
      continue;
    }

    int isSpecial = 0;
    char separator = ';';
    size_t len = strlen(queryStr);
    if (len > 0 && queryStr[len - 1] == 'S') {
      isSpecial = 1;
      separator = '=';
      queryStr[len - 1] = '\0';
    }

    int queryNumber = atoi(queryStr);

    // ===================== ARGS =====================
    char *arg1 = line + lenRead;
    g_strstrip(arg1); // remove leading/trailing spaces

    char *arg2 = NULL;
    if (queryNumber == 2 || queryNumber == 3 || queryNumber == 4) {
      char *space = strchr(arg1, ' ');
      if (space) {
        *space = '\0';
        arg2 = space + 1;
        g_strstrip(arg2);
      }
    }
    // For query 6, arg1 keeps spaces (e.g. "Burkina Faso")

    // ===================== Output file =====================
    char outputFileName[128];
    snprintf(outputFileName, sizeof(outputFileName),
             "resultados/command%d_output.txt", lineNumber);

    FILE *output = fopen(outputFileName, "w");
    if (!output) {
      perror("Failed to open output file");
      lineNumber++;
      continue;
    }
    char buffer[65536];
    setvbuf(output, buffer, _IOFBF, sizeof(buffer));

    GTimer *queryTimer = g_timer_new();

    // ===================== QUERY 1 =====================
    if (queryNumber == 1) {
      gchar *res =
          query1(arg1, getDatasetAirports(ds), getDatasetAiportStats(ds));
      if (res) {
        if (isSpecial)
          for (char *p = res; *p; p++)
            if (*p == ';')
              *p = '=';
        fprintf(output, "%s\n", res);
        g_free(res);
      } else
        fprintf(output, "\n");

      // ===================== QUERY 2 =====================
    } else if (queryNumber == 2) {
      int N = atoi(arg1);
      int size;
      AircraftStats **top =
          query2(N, aircraftsArray, arg2 && strlen(arg2) > 0 ? arg2 : NULL,
                 &size, flightCounts);

      if (top && size > 0) {
        for (int i = 0; i < size; i++)
          fprintf(output, "%s%c%s%c%s%c%d\n", get_aircraftstats_id(top[i]),
                  separator, get_aircraftstats_manufacturer(top[i]), separator,
                  get_aircraftstats_model(top[i]), separator,
                  get_aircraftstats_count(top[i]));
        free_aircraftstats_array(top, size);
      } else
        fprintf(output, "\n");

      // ===================== QUERY 3 =====================
    } else if (queryNumber == 3) {
      GHashTable *airports = (GHashTable *)getDatasetAirports(ds);
      gchar *result = query3(airportFtrees, airports, arg1, arg2);
      if (result) {
        if (isSpecial)
          for (char *p = result; *p; p++)
            if (*p == ';')
              *p = '=';
        fprintf(output, "%s\n", result);
        g_free(result);
      } else
        fprintf(output, "\n");

      // ===================== QUERY 4 =====================
    } else if (queryNumber == 4) {
      // arg1 is Start Date, arg2 is End Date
      query4(q4_data, (GHashTable *)getDatasetPassengers(ds), 
               arg1, arg2, output, isSpecial);

      // ===================== QUERY 5 =====================
    } else if (queryNumber == 5) {
      if (strlen(arg1) == 0)
        fprintf(output, "\n");
      else {
        int N = atoi(arg1);
        if (N <= 0)
          fprintf(output, "\n");
        else {
          if (query5(airlineDelays, N, output, isSpecial) == 0)
            fprintf(output, "\n");
        }
      }

      // ===================== QUERY 6 =====================
    } else if (queryNumber == 6) {
      if (strlen(arg1) == 0)
        fprintf(output, "\n");
      else {
        if (query_Q6(natTable, arg1, output, isSpecial) == 0)
          fprintf(output, "\n");
      }

      // ===================== DEFAULT =====================
    } else
      fprintf(output, "\n");

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

  // ===================== Cleanup =====================
  destroy_Q4_structure(q4_data);
  freeAirlineDelays(airlineDelays);
  g_hash_table_destroy(airportFtrees);
  g_hash_table_destroy(airportDates);
  g_ptr_array_free(flightsArray, TRUE);
  g_ptr_array_free(aircraftsArray, TRUE);
  g_hash_table_destroy(natTable);
  free(flightCounts);
}

/*
void runAllQueries(Dataset *ds, const char *filePath,
                   QueryStatsCallback callback, void *ctx) {

  // ===================== Data Preparation =====================

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

  GHashTable *idToIndex = g_hash_table_new(g_str_hash, g_str_equal);
  for (int i = 0; i < numAircrafts; i++) {
    Aircraft *ac = g_ptr_array_index(aircraftsArray, i);
    g_hash_table_insert(idToIndex,
                        (gpointer)getAircraftId(ac),
                        GINT_TO_POINTER(i));
  }

  for (size_t i = 0; i < flightsArray->len; i++) {
    Flight *f = g_ptr_array_index(flightsArray, i);

    if (!f || strcmp(getFlightStatus(f), "Cancelled") == 0)
      continue;

    const char *acId = getFlightAircraft(f);
    if (!acId) continue;

    gpointer idxPtr = g_hash_table_lookup(idToIndex, acId);
    if (idxPtr)
      flightCounts[GPOINTER_TO_INT(idxPtr)]++;
  }

  g_hash_table_destroy(idToIndex);

  GHashTable *airportDates = getDatesTable(ds);
  GHashTable *airportFtrees =
      getFTrees(airportDates, (GHashTable *)getDatasetFlights(ds));

  GList *airlineDelays = prepareAirlineDelays(flightsArray); // Query 5

  // ===================== Input reading =====================

  FILE *inputFile = fopen(filePath, "r");
  if (!inputFile) {
    perror("Failed to open inputs file");
    return;
  }

  GTimer *totalTimer = g_timer_new();

  char line[256];
  int lineNumber = 1;

  while (fgets(line, sizeof(line), inputFile)) {

    line[strcspn(line, "\n")] = '\0';

    char queryStr[8];
    char arg1[64] = "";
    char arg2[64] = "";

    int fields = sscanf(line, "%7s %63s %63s", queryStr, arg1, arg2);

    // ===== format S =====
    int isSpecial = 0;
    char separator = ';';

    size_t len = strlen(queryStr);
    if (len > 0 && queryStr[len - 1] == 'S') {
      isSpecial = 1;
      separator = '=';
      queryStr[len - 1] = '\0';
    }

    int queryNumber = atoi(queryStr);

    char outputFileName[128];
    snprintf(outputFileName, sizeof(outputFileName),
             "resultados/command%d_output.txt", lineNumber);

    FILE *output = fopen(outputFileName, "w");
    if (!output) {
      perror("Failed to open output file");
      lineNumber++;
      continue;
    }

    char buffer[65536];
    setvbuf(output, buffer, _IOFBF, sizeof(buffer));

    GTimer *queryTimer = g_timer_new();

    // ===================== QUERY 1 =====================
    if (queryNumber == 1) {

      gchar *res = query1(arg1,
       getDatasetAirports(ds),
       getDatasetAiportStats(ds));

      if (res) {
        if (isSpecial) {
          for (char *p = res; *p; p++)
            if (*p == ';') *p = '=';
        }
        fprintf(output, "%s\n", res);
        g_free(res);
      } else {
        fprintf(output, "\n");
      }

    // ===================== QUERY 2 =====================
    } else if (queryNumber == 2) {

      int N = atoi(arg1);
      int size;

      AircraftStats **top =
          query2(N, aircraftsArray,
                 strlen(arg2) > 0 ? arg2 : NULL,
                 &size,
                 flightCounts);

      if (top && size > 0) {
        for (int i = 0; i < size; i++) {
          fprintf(output, "%s%c%s%c%s%c%d\n",
                  get_aircraftstats_id(top[i]),
                  separator,
                  get_aircraftstats_manufacturer(top[i]),
                  separator,
                  get_aircraftstats_model(top[i]),
                  separator,
                  get_aircraftstats_count(top[i]));
        }
        free_aircraftstats_array(top, size);
      } else {
        fprintf(output, "\n");
      }

    // ===================== QUERY 3 =====================
    } else if (queryNumber == 3) {

      if (fields < 3) {
        fprintf(output, "\n");
      } else {
        GHashTable *airports =
            (GHashTable *)getDatasetAirports(ds);

        gchar *result =
            query3(airportFtrees, airports, arg1, arg2);

        if (result) {
          if (isSpecial) {
            for (char *p = result; *p; p++)
              if (*p == ';') *p = '=';
          }
          fprintf(output, "%s\n", result);
          g_free(result);
        } else {
          fprintf(output, "\n");
        }
      }

    }
    // ===================== QUERY 4 =====================
 else if (queryNumber == 4) {
    time_t begin = 0, end = 0;

    if (fields >= 2 && strlen(arg1) > 0) {
        struct tm tm_begin;
        strptime(arg1, "%Y-%m-%d", &tm_begin);
        begin = timegm(&tm_begin);
    }
    if (fields >= 3 && strlen(arg2) > 0) {
        struct tm tm_end;
        strptime(arg2, "%Y-%m-%d", &tm_end);
        end = timegm(&tm_end);
    }

    // chama query_Q4 com os getters do dataset
    query_Q4(
        (GHashTable *)getDatasetReservations(ds),
        (GHashTable *)getDatasetFlights(ds),
        (GHashTable *)getDatasetPassengers(ds),
        begin,
        end,
        output
    );
}
// ===================== QUERY 5 =====================
  else if (queryNumber == 5) {
      if (fields < 2 || strlen(arg1) == 0) {
          fprintf(output, "\n");
      } else {
          int N = atoi(arg1);
          if (N <= 0) {
              fprintf(output, "\n");
          } else {
              query5(airlineDelays, N, output, isSpecial);
          }
      }
  }
  else if (queryNumber == 6) {
    if (fields < 2 || strlen(arg1) == 0) {
        fprintf(output, "\n");
    } else {
        query_Q6(getDatasetPassengers(ds), getDatasetReservations(ds),
getDatasetFlights(ds), arg1, output, isSpecial);
    }
}
  else {
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

  // ===================== Cleanup =====================
  freeAirlineDelays(airlineDelays);
  g_hash_table_destroy(airportFtrees);
  g_hash_table_destroy(airportDates);
  g_ptr_array_free(flightsArray, TRUE);
  g_ptr_array_free(aircraftsArray, TRUE);
  free(flightCounts);
}
*/
/*
void runAllQueries(Dataset *ds, const char *filePath, QueryStatsCallback
callback, void *ctx) {
  // Create arrays from hash tables (flights and aircrafts)
  GPtrArray *flightsArray = g_ptr_array_new();
  GHashTableIter iter;
  gpointer key, value;

  g_hash_table_iter_init(&iter, (GHashTable *)getDatasetFlights(ds));
  while (g_hash_table_iter_next(&iter, &key, &value))
    g_ptr_array_add(flightsArray, value);

  GPtrArray *aircraftsArray = g_ptr_array_new();
  g_hash_table_iter_init(&iter, (GHashTable *)getDatasetAircrafts(ds));
  while (g_hash_table_iter_next(&iter, &key, &value))
    g_ptr_array_add(aircraftsArray, value);

  // Allocate array for flight counts, initially zero
  int numAircrafts = aircraftsArray->len;
  int *flightCounts = calloc(numAircrafts, sizeof(int));

  GHashTable *idToIndex = g_hash_table_new(g_str_hash, g_str_equal);
  for (int i = 0; i < numAircrafts; i++) {
      Aircraft *ac = g_ptr_array_index(aircraftsArray, i);
      g_hash_table_insert(idToIndex, (gpointer)getAircraftId(ac),
GINT_TO_POINTER(i));
  }

  // Count flights per aircraft
  for (size_t i = 0; i < flightsArray->len; i++) {
      Flight *f = g_ptr_array_index(flightsArray, i);
      if (!f || strcmp(getFlightStatus(f), "Cancelled") == 0)
          continue;

      const char *acId = getFlightAircraft(f);
      if (!acId) continue;

      gpointer idxPtr = g_hash_table_lookup(idToIndex, acId);
      if (idxPtr) {
          int idx = GPOINTER_TO_INT(idxPtr);
          flightCounts[idx]++;
      }
  }

  // Done with hash table
  g_hash_table_destroy(idToIndex);


  GHashTable *airportDates = getDatesTable(ds);
  GHashTable *airportFtrees = getFTrees(airportDates, (GHashTable
*)getDatasetFlights(ds));

  FILE *inputFile = fopen(filePath, "r");
  if (!inputFile) {
    perror("Failed to open inputs file");
    return;
  }

  GTimer *totalTimer = g_timer_new();

  char line[256];
  int lineNumber = 1;

  while (fgets(line, sizeof(line), inputFile)) {
    line[strcspn(line, "\n")] = '\0';

    int queryNumber;
    char arg1[64] = "";
    char arg2[64] = "";

    int fields = sscanf(line, "%d %63s %63s", &queryNumber, arg1, arg2);

    char outputFileName[128];
    snprintf(outputFileName, sizeof(outputFileName),
             "resultados/command%d_output.txt", lineNumber);
    FILE *output = fopen(outputFileName, "w");
    if (!output) {
      perror("Failed to open output file");
      lineNumber++;
      continue;
    }
    char buf[65536];
    setvbuf(output, buf, _IOFBF, sizeof(buf));

    GTimer *queryTimer = g_timer_new();

    if (queryNumber == 1) {
      // Query 1: airport info
      gchar *res = query1(arg1, (GHashTable *)getDatasetAirports(ds));
      fprintf(output, "%s\n", res ? res : "");
      g_free(res);
    } else if (queryNumber == 2) {
      // Query 2: top N aircrafts (optionally by manufacturer)
      int N = atoi(arg1);
      int size;

      AircraftStats **top =
          query2(N, aircraftsArray,
                strlen(arg2) > 0 ? arg2 : NULL, &size,
                flightCounts);

      if (top && size > 0) {
        for (int i = 0; i < size; i++) {
          fprintf(output, "%s,%s,%s,%d\n",
                  get_aircraftstats_id(top[i]),
                  get_aircraftstats_manufacturer(top[i]),
                  get_aircraftstats_model(top[i]),
                  get_aircraftstats_count(top[i]));
        }
        free_aircraftstats_array(top, size);
      } else {
        fprintf(output, "\n");
      }
    } else if (queryNumber == 3) {
      // Query 3: best airport between start and end date
      if (fields < 3) {
        fprintf(output, "\n");
      } else {
        GHashTable *airports = (GHashTable *)getDatasetAirports(ds);
        gchar *result = query3(airportFtrees, airports, arg1, arg2);
        if (!result) {
          fprintf(output, "\n");
        }
        else {
          fprintf(output, "%s", result ? result : "");
          fprintf(output, "\n");
        }
        g_free(result);
      }
    } else {
      fprintf(output, "\n");
    }

    fclose(output);

    gdouble elapsed = g_timer_elapsed(queryTimer, NULL);
    g_timer_destroy(queryTimer);

    if (callback) {
        callback(queryNumber, lineNumber, elapsed, ctx);
    } else {
        printf("Query %d (line %d) executed in %.6f seconds\n", queryNumber,
                lineNumber, elapsed);
    }

    lineNumber++;
  }

  // Total time of all queries
  gdouble totalElapsed = g_timer_elapsed(totalTimer, NULL);
  if (!callback) {
    printf("Total time of all queries: %.6f seconds\n", totalElapsed);
  }
  g_timer_destroy(totalTimer);

  fclose(inputFile);
  g_hash_table_destroy(airportFtrees);
  g_hash_table_destroy(airportDates);
  g_ptr_array_free(flightsArray, TRUE);
  g_ptr_array_free(aircraftsArray, TRUE);
  free(flightCounts);
}
  */
