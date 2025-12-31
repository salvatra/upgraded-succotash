#include "core/time_utils.h"
#include <stdlib.h>
#define _XOPEN_SOURCE 700
#include <entities/flights.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <validation.h>
#include "entities/parser.h"

struct flight {
  gchar *id;
  time_t departure;
  time_t actual_departure;
  time_t arrival;
  time_t actual_arrival;
  //gchar *gate;
  gchar *status;
  gchar *origin;
  gchar *destination;
  gchar *aircraft;
  gchar *airline;
  //gchar *tracking_url;
};

void freeFlight(gpointer data) {
  Flight *flight = data;
  g_free(flight->id);
  g_free(flight->origin);
  g_free(flight->destination);
  g_free(flight->aircraft);
  g_free(flight->status);
  //g_free(flight->gate);
  g_free(flight->airline);
  //g_free(flight->tracking_url);
  g_free(flight);
}

void cleanupFlight(Flight *data, gchar **fields) {
  if (data)
    freeFlight(data);
  if (fields)
    g_strfreev(fields);
}

/*
GHashTable *readFlights2(const char *filename, int *errorsFlag) {

  FILE *flights = fopen(filename, "r");
  if (!flights) {
    return NULL;
  }

  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  // Read and store header for logging
  if ((read = getline(&line, &len, flights)) == -1) {
    fclose(flights);
    return NULL;
  }

  if (read > 0 && line[read - 1] == '\n')
    line[--read] = '\0';
  char *headerLine = g_strdup(line);

  // --- Preallocate reusable buffer ---
  gchar **fields = g_new0(gchar *, 12);

  GHashTable *flightsTable =
      g_hash_table_new_full(g_str_hash, g_str_equal, g_free, freeFlight);

  while ((read = getline(&line, &len, flights)) != -1) {
    g_strchomp(line);

    gchar *originalLine = strdup(line);
    // --- Fast manual CSV splitting (avoids g_strsplit mallocs) ---
    int fieldCount = 0;
    gchar *start = line;
    gchar *ptr = line;
    while (*ptr && fieldCount < 12) {
      if (*ptr == '"' && *(ptr + 1) == ',' && *(ptr + 2) == '"') {
        *ptr = '\0';
        fields[fieldCount++] = start;
        start = ptr + 3;
        ptr += 2;
      }
      ptr++;
    }
    fields[fieldCount++] = start;
    if (fieldCount < 9)
      continue;

    trim(fields[0]);
    trim(fields[11]);

    gboolean invalid = FALSE;

    if (!checkFlightId(fields[0])) {
      invalid = TRUE;
    }

    if (!invalid) {
      if (!checkDatetime(fields[1]) || !checkDatetime(fields[3])) {
        invalid = TRUE;
      }

      if (strcmp(fields[2], "N/A") != 0 && !checkDatetime(fields[2])) {
        invalid = TRUE;
      }
      if (strcmp(fields[4], "N/A") != 0 && !checkDatetime(fields[4])) {
        invalid = TRUE;
      }
    }

    if (!invalid) {
      if (!checkDelayed(fields[6], fields[1], fields[3], fields[2],
                        fields[4]) ||
          !checkCancellation(fields[6], fields[2], fields[4]) ||
          !checkDestinationOrigin(fields[8], fields[7])) {
        invalid = TRUE;
      }
    }

    if (!invalid) {
      if (compare_unix_datetime(fields[1], fields[2]) > 0)
        invalid = TRUE;
      else if (compare_unix_datetime(fields[3], fields[4]) > 0)
        invalid = TRUE;
      else if (compare_unix_datetime(fields[1], fields[3]) >= 0)
        invalid = TRUE;
      else if (compare_unix_datetime(fields[2], fields[4]) >= 0)
        invalid = TRUE;
    }

    if (!invalid) {
      if (!checkAirportCode(fields[7]) || !checkAirportCode(fields[8]))
        invalid = TRUE;
    }

    if (!invalid) {
      if (!checkAircraftId(fields[9]))
        invalid = TRUE;
    }

    if (invalid) {
      logInvalidLine("resultados/flights_errors.csv", headerLine, originalLine);
      if (!(*errorsFlag))
        *errorsFlag = 1;
      free(originalLine);
      continue;
    }

    gint ad = 0, aa = 0;

    Flight *data = g_new0(Flight, 1);

    data->id = g_strdup(fields[0]);

    data->departure = parse_unix_datetime(fields[1], NULL);
    data->actual_departure = parse_unix_datetime(fields[2], &ad);
    data->arrival = parse_unix_datetime(fields[3], NULL);
    data->actual_arrival = parse_unix_datetime(fields[4], &aa);

    data->gate = g_strdup(fields[5]);
    data->status = g_strdup(fields[6]);
    data->origin = g_strdup(fields[7]);
    data->destination = g_strdup(fields[8]);
    data->aircraft = g_strdup(fields[9]);
    data->airline = g_strdup(fields[10]);
    data->tracking_url = g_strdup(fields[11]);

    // --- Insert ---
    g_hash_table_insert(flightsTable, g_strdup(data->id), data);
    free(originalLine);
  }

  g_free(fields);
  g_free(headerLine);
  free(line);
  fclose(flights);
  return flightsTable;
}
*/
/*
GHashTable *readFlights(const char *filename, int *errorsFlag) {
  FILE *flights = fopen(filename, "r");
  if (!flights) {
    return NULL;
  }

  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  // Read and store header for logging
  if ((read = getline(&line, &len, flights)) == -1) {
    fclose(flights);
    return NULL;
  }

  if (read > 0 && line[read - 1] == '\n')
    line[--read] = '\0';
  char *headerLine = g_strdup(line);

  // Define array of pointers to each field in line
  char *fields[12];

  GHashTable *flightsTable =
      g_hash_table_new_full(g_str_hash, g_str_equal, NULL, freeFlight);

  while ((read = getline(&line, &len, flights)) != -1) {
    // Remove '\n' manually
    if (read > 0 && line[read - 1] == '\n')
      line[--read] = '\0';

    // Optimized parser: return the pointer to the field in the original line
    // No need to dupe strings or use many allocations

    char *curr = line;
    int fieldIdx = 0;

    while (*curr && fieldIdx < 12) {
      if (*curr == '"')
        curr++;

      fields[fieldIdx++] = curr;

      // Find the next quotation marks
      char *end = strchr(curr, '"');

      if (end) {
        *end = '\0';
        curr = end + 1;
        if (*curr == ',')
          curr++;
      } else {
        break;
      }
    }

    if (fieldIdx < 12) {
      continue;
    }

    gboolean invalid = FALSE;

    if (!checkFlightId(fields[0]))
      invalid = TRUE;

    time_t sched_dep = 0, act_dep = 0, sched_arr = 0, act_arr = 0;
    int cancelled = 0;

    if (!invalid) {

      sched_dep = parse_unix_datetime(fields[1], NULL);
      if (sched_dep < (time_t)0)
        invalid = TRUE;

      if (!invalid) {
        sched_arr = parse_unix_datetime(fields[3], NULL);
        if (sched_arr < (time_t)0)
          invalid = TRUE;

        if (!invalid) {
          act_dep = parse_unix_datetime(fields[2], &cancelled);
          if (act_dep < (time_t)0 && cancelled == 0)
            invalid = TRUE;

          if (!invalid) {
            act_arr = parse_unix_datetime(fields[4], &cancelled);
            if (act_arr < (time_t)0 && cancelled == 0)
              invalid = TRUE;
          }
        }
      }
    }

    if (!invalid) {
      if (!checkDelayed(fields[6], sched_dep, sched_arr, act_dep, act_arr,
                        cancelled) ||
          !checkCancellation(fields[6], act_dep, act_arr) ||
          !checkDestinationOrigin(fields[8], fields[7])) {
        invalid = TRUE;
      }
    }

    if (!invalid) {
      if (compare_unix_datetime(fields[1], fields[2]) > 0)
        invalid = TRUE;
      else if (compare_unix_datetime(fields[3], fields[4]) > 0)
        invalid = TRUE;
      else if (compare_unix_datetime(fields[1], fields[3]) >= 0)
        invalid = TRUE;
      else if (compare_unix_datetime(fields[2], fields[4]) >= 0)
        invalid = TRUE;
    }

    if (!invalid) {
      if (!checkAirportCode(fields[7]) || !checkAirportCode(fields[8]))
        invalid = TRUE;
    }

    if (!invalid) {
      if (!checkAircraftId(fields[9]))
        invalid = TRUE;
    }

    if (invalid) {
      char tempBuffer[4096];
      tempBuffer[0] = '\0';

      for (int i = 0; i < 12; i++) {
        if (i > 0)
          strcat(tempBuffer, ",");
        strcat(tempBuffer, "\"");
        strcat(tempBuffer, fields[i]);
        strcat(tempBuffer, "\"");
      }

      logInvalidLine("resultados/flights_errors.csv", headerLine, tempBuffer);

      if (!(*errorsFlag))
        *errorsFlag = 1;
      continue;
    }

    Flight *data = g_slice_new(Flight);

    data->id = g_strdup(fields[0]);

    data->departure = sched_dep;
    data->actual_departure = act_dep;
    data->arrival = sched_arr;
    data->actual_arrival = act_arr;

    data->gate = g_strdup(fields[5]);

    data->status = g_strdup(fields[6]);

    data->origin = g_strdup(fields[7]);

    data->destination = g_strdup(fields[8]);

    data->aircraft = g_strdup(fields[9]);

    data->airline = g_strdup(fields[10]);
    data->tracking_url = g_strdup(fields[11]);

    // --- Insert ---
    g_hash_table_insert(flightsTable, data->id, data);
  }

  g_free(headerLine);
  free(line);
  fclose(flights);
  return flightsTable;
}
*/

GHashTable *readFlights(const char *filename, int *errorsFlag, GHashTable *aircrafts) {
    FILE *flights = fopen(filename, "r");
    if (!flights) return NULL;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    if ((read = getline(&line, &len, flights)) == -1) {
        fclose(flights);
        return NULL;
    }
    if (read > 0 && line[read - 1] == '\n')
        line[--read] = '\0';
    char *headerLine = g_strdup(line);

    GHashTable *flightsTable =
        g_hash_table_new_full(g_str_hash, g_str_equal, NULL, freeFlight);

    while ((read = getline(&line, &len, flights)) != -1) {
        ParsedFieldsF *pf = parseFlightLineRaw(line);
        if (!parsed_fields_ok(pf)) {
            parsed_fields_free(pf);
            continue;
        }

        gboolean invalid = FALSE;

        const char *fields[12];
        for (int i = 0; i < 12; i++)
            fields[i] = parsed_fields_get(pf, i);

        if (!checkFlightId(fields[0])) invalid = TRUE;

        time_t sched_dep = 0, act_dep = 0, sched_arr = 0, act_arr = 0;
        int cancelled = 0;

        if (!invalid) {
            sched_dep = parse_unix_datetime(fields[1], NULL);
            if (sched_dep < 0) invalid = TRUE;

            sched_arr = parse_unix_datetime(fields[3], NULL);
            if (sched_arr < 0) invalid = TRUE;

            act_dep = parse_unix_datetime(fields[2], &cancelled);
            if (act_dep < 0 && cancelled == 0) invalid = TRUE;

            act_arr = parse_unix_datetime(fields[4], &cancelled);
            if (act_arr < 0 && cancelled == 0) invalid = TRUE;
        }

        if (!invalid) {
            if (!checkDelayed(fields[6], sched_dep, sched_arr, act_dep, act_arr, cancelled) ||
                !checkCancellation(fields[6], act_dep, act_arr) ||
                !checkDestinationOrigin(fields[8], fields[7])) {
                invalid = TRUE;
            }
        }

        if (!invalid) {
            if (compare_unix_datetime(fields[1], fields[2]) > 0 ||
                compare_unix_datetime(fields[3], fields[4]) > 0 ||
                compare_unix_datetime(fields[1], fields[3]) >= 0 ||
                compare_unix_datetime(fields[2], fields[4]) >= 0) {
                invalid = TRUE;
            }
        }

        if (!invalid) {
            if (!checkAirportCode(fields[7]) || !checkAirportCode(fields[8]))
                invalid = TRUE;
        }

        if (!invalid && (!fields[9] || !g_hash_table_contains(aircrafts, fields[9]))) {
            invalid = TRUE;
        }

        if (!invalid) {
            if (!checkAircraftId(fields[9]))
                invalid = TRUE;
        }

        if (invalid) {
            char tempBuffer[4096] = "";
            for (int i = 0; i < 12; i++) {
                if (i > 0) strcat(tempBuffer, ",");
                strcat(tempBuffer, "\"");
                strcat(tempBuffer, fields[i]);
                strcat(tempBuffer, "\"");
            }
            logInvalidLine("resultados/flights_errors.csv", headerLine, tempBuffer);
            *errorsFlag = 1;
            parsed_fields_free(pf);
            continue;
        }

        Flight *data = g_new0(Flight, 1);
        data->id = g_strdup(fields[0]);
        data->departure = sched_dep;
        data->actual_departure = act_dep;
        data->arrival = sched_arr;
        data->actual_arrival = act_arr;

        //data->gate = g_strdup(fields[5]);
        data->status = g_strdup(fields[6]);
        data->origin = g_strdup(fields[7]);
        data->destination = g_strdup(fields[8]);
        data->aircraft = g_strdup(fields[9]);
        data->airline = g_strdup(fields[10]);
        //data->tracking_url = g_strdup(fields[11]);

        g_hash_table_insert(flightsTable, data->id, data);

        parsed_fields_free(pf); 
    }

    g_free(headerLine);
    free(line);
    fclose(flights);
    return flightsTable;
}


Flight *copyFlight(const Flight *src) {
  if (!src)
    return NULL;

  Flight *copy = g_new0(Flight, 1);

  copy->id = g_strdup(src->id);

  copy->status = g_strdup(src->status);

  copy->origin = g_strdup(src->origin);

  copy->destination = g_strdup(src->destination);

  copy->aircraft = g_strdup(src->aircraft);

  copy->airline = g_strdup(src->airline);
  //copy->gate = g_strdup(src->gate);
  //copy->tracking_url = g_strdup(src->tracking_url);
  copy->departure = src->departure;
  copy->actual_departure = src->departure;
  copy->arrival = src->arrival;
  copy->actual_arrival = src->arrival;

  return copy;
}

Flight *getFlight(const gchar *id, const GHashTable *flightsTable) {
  Flight *flight = g_hash_table_lookup((GHashTable *)flightsTable, id);

  if (!flight)
    return NULL;

  return copyFlight(flight);
}

// NOTE: As the getFlight function returns a deep copy, the aditional getters
// don't need to be concerned with encapsulation.
// Note that they should also ONLY be called on the flight returned by the
// getter function
const gchar *getFlightId(const Flight *f) {
  if (!f)
    return NULL;
  return f->id;
}

time_t getFlightDeparture(const Flight *f) {
  if (!f)
    return -1;
  return f->departure;
}

time_t getFlightActualDeparture(const Flight *f) {
  if (!f)
    return -1;
  return f->actual_departure;
}

time_t getFlightArrival(const Flight *f) {
  if (!f)
    return -1;
  return f->arrival;
}

time_t getFlightActualArrival(const Flight *f) {
  if (!f)
    return -1;
  return f->actual_arrival;
}

const gchar *getFlightStatus(const Flight *f) {
  if (!f)
    return NULL;
  return f->status;
}

const gchar *getFlightOrigin(const Flight *f) {
  if (!f)
    return NULL;
  return f->origin;
}

const gchar *getFlightDestination(const Flight *f) {
  if (!f)
    return NULL;
  return f->destination;
}

const gchar *getFlightAircraft(const Flight *f) {
  if (!f)
    return NULL;
  return f->aircraft;
}
/*
gint getFlightGate(const Flight *f) {
  if (!f || !f->gate)
    return -1;
  return atoi(f->gate);
}
*/
const gchar *getFlightAirline(const Flight *f) {
  if (!f)
    return NULL;
  return f->airline;
}
/*
const gchar *getFlightTrackingUrl(const Flight *f) {
  if (!f)
    return NULL;
  return f->tracking_url;
}
  */
