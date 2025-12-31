#include <ctype.h>
#include <entities/reservations.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <validation.h>
#include "entities/parser.h"
#include "entities/flights.h"

struct reservation {
  gchar *reservation_id;
  gchar **flight_ids;
  int document_no;
 // gchar *seat;
  double price;
  //gchar *extra_luggage;
  //gchar *priority_boarding;
 // gchar *qr_code;
};

void freeReservations(gpointer data) {
  Reservation *reserv = data;
  g_free(reserv->reservation_id);
  g_strfreev(reserv->flight_ids);
  //g_free(reserv->seat);
  //g_free(reserv->qr_code);
  //g_free(reserv->extra_luggage);
  //g_free(reserv->priority_boarding);
  g_free(reserv);
}

void cleanupReservations(Reservation *data, gchar **fields) {
  if (data)
    freeReservations(data);
  if (fields)
    g_strfreev(fields);
}

gboolean strToBool(const gchar *str) {
  if (!str)
    return FALSE;
  return g_ascii_strcasecmp(str, "true") == 0 ? TRUE : FALSE;
}

/*
GHashTable *readReservations1(const gchar *filename, gint *errorsFlag) {
  GHashTable *reservationTable =
      g_hash_table_new_full(g_str_hash, g_str_equal, g_free, freeReservations);

  FILE *file = fopen(filename, "r");
  if (!file) {
    g_hash_table_destroy(reservationTable);
    return NULL;
  }

  gchar *line = NULL;
  size_t len = 0;
  ssize_t read;
  gchar *headerLine = NULL;

  // Read header line
  if ((read = getline(&line, &len, file)) != -1) {
    g_strchomp(line);
    headerLine = g_strdup(line);
  } else {
    fclose(file);
    g_hash_table_destroy(reservationTable);
    return NULL;
  }

  while ((read = getline(&line, &len, file)) != -1) {
    // Keep original line for logging
    g_strchomp(line);
    gchar *originalLine = g_strdup(line); // remove newline for processing

    // Split CSV line safely
    gchar **fields = g_strsplit(line, "\",\"", 8);
    if (!fields) {
      g_free(originalLine);
      continue;
    }
    trim(fields[0]);
    trim(fields[2]);
    trim(fields[7]);

    Reservation *data = g_new0(Reservation, 1);
    gboolean invalid = FALSE;

    // --- Validate Reservation ID ---
    if (!checkReservationId(fields[0]))
      invalid = TRUE;
    else
      data->reservation_id = g_strdup(fields[0]);

    if (!invalid) {
      data->flight_ids = parseFlightIds(fields[1]);
      if (!data->flight_ids || data->flight_ids[0] == NULL) {
        invalid = TRUE;
      } else {
        for (gint i = 0; data->flight_ids[i]; i++) {
          if (!checkFlightId(data->flight_ids[i])) {
            invalid = TRUE;
            break;
          }
        }
      }
    }

    // --- Validate Document Number ---
    if (!invalid && !checkDocumentNo(fields[2])) {
      invalid = TRUE;
    } else {
      data->document_no = atoi(fields[2]);
    }

    if (!invalid) {
      data->seat = g_strdup(fields[3]);
      data->price = atof(fields[4]);
      data->extra_luggage = g_strdup(fields[5]);
      data->priority_boarding = g_strdup(fields[6]);
      data->qr_code = g_strdup(fields[7]);
    }

    // --- Handle invalid lines ---
    if (invalid) {
      logInvalidLine("resultados/reservations_errors.csv", headerLine,
                     originalLine);
      if (!(*errorsFlag))
        *errorsFlag = 1;
      cleanupReservations(data, fields);
      g_free(originalLine);
      continue;
    }

    // --- Insert valid reservation ---
    g_hash_table_insert(reservationTable, g_strdup(fields[0]), data);

    g_strfreev(fields);
    g_free(originalLine);
  }

  g_free(headerLine);
  free(line);
  fclose(file);

  return reservationTable;
} 
  */

GHashTable *readReservations(const char *filename,
                             GHashTable *passengersTable,
                             GHashTable *flightsTable,
                             int *errorsFlag) {
    FILE *f = fopen(filename, "r");
    if (!f) return NULL;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    /* Header */
    if ((read = getline(&line, &len, f)) == -1) {
        fclose(f);
        return NULL;
    }
    g_strchomp(line);
    char *headerLine = g_strdup(line);

    GHashTable *table =
        g_hash_table_new_full(g_str_hash, g_str_equal, g_free, freeReservations);

    while ((read = getline(&line, &len, f)) != -1) {
        g_strchomp(line);

        ParsedReservationF *pr = parseReservationLineRaw(line);
        if (!parsed_reservation_ok(pr)) {
            parsed_reservation_free(pr);
            continue;
        }

        const gchar *fields[8];
        for (int i = 0; i < 8; i++)
            fields[i] = parsed_reservation_get(pr, i);

        gboolean invalid = FALSE;

        /* --- Step 1: Reservation ID --- */
        if (!checkReservationId(fields[0]))
            invalid = TRUE;

        /* --- Step 2: Document number --- */
        int docNo = 0; 
        if (!invalid && !checkDocumentNo(fields[2]))
            invalid = TRUE;
        else {
          docNo = atoi(fields[2]);
        }

        /* --- Step 3: Passenger exists --- */
        if (!invalid &&
            !g_hash_table_contains(passengersTable, GINT_TO_POINTER(docNo)))
            invalid = TRUE;

        /* --- Step 4: Flight IDs (list validation) --- */
        gchar **flights = NULL;

        if (!invalid) {
            const char *s = fields[1];
            size_t l = strlen(s);

            /* list must start with '[' and end with ']' */
            if (l < 2 || s[0] != '[' || s[l - 1] != ']') {
                invalid = TRUE;
            }
        }

        if (!invalid) {
            flights = parseFlightIds(fields[1]);
            if (!flights || !flights[0]) {
                invalid = TRUE;
            } else {
                int count = 0;
                while (flights[count]) count++;

                if (count == 0 || count > 2) {
                    invalid = TRUE;

                } else if (count == 1) {
                    if (!g_hash_table_contains(flightsTable, flights[0]))
                        invalid = TRUE;

                } else { /* count == 2 */
                    Flight *f1 = getFlight(flights[0], flightsTable);
                    Flight *f2 = getFlight(flights[1], flightsTable);

                    if (!f1 || !f2 ||
                        g_strcmp0(getFlightDestination(f1),
                                  getFlightOrigin(f2)) != 0)
                        invalid = TRUE;

                    if (f1) freeFlight(f1);
                    if (f2) freeFlight(f2);
                }

                if (invalid) {
                    g_strfreev(flights);
                    flights = NULL;
                }
            }
        }

        if (invalid) {
            logInvalidLine("resultados/reservations_errors.csv",
                           headerLine,
                           parsed_reservation_line(pr));
            *errorsFlag = 1;
            parsed_reservation_free(pr);
            continue;
        }

        /* --- Create reservation --- */
        Reservation *data = g_new0(Reservation, 1);
        data->reservation_id     = g_strdup(fields[0]);
        data->flight_ids         = flights;
        data->document_no        = docNo;
        //data->seat               = g_strdup(fields[3]);
        data->price              = atof(fields[4]);
        //data->extra_luggage      = g_strdup(fields[5]);
        //data->priority_boarding  = g_strdup(fields[6]);
        //data->qr_code            = g_strdup(fields[7]);

        g_hash_table_insert(table, g_strdup(fields[0]), data);
        parsed_reservation_free(pr);
    }

    g_free(headerLine);
    free(line);
    fclose(f);
    return table;
}






/*
GHashTable *readReservations2(const char *filename, int *errorsFlag) {
    FILE *reservations = fopen(filename, "r");
    if (!reservations)
        return NULL;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // Read header
    if ((read = getline(&line, &len, reservations)) == -1) {
        fclose(reservations);
        return NULL;
    }

    if (read > 0 && line[read - 1] == '\n')
        line[--read] = '\0';

    char *headerLine = g_strdup(line);

    char *fields[8];

    GHashTable *reservationsTable =
        g_hash_table_new_full(g_str_hash, g_str_equal, NULL, freeReservations);

    // ---- MAIN LOOP ----
    while ((read = getline(&line, &len, reservations)) != -1) {

        if (read > 0 && line[read - 1] == '\n')
            line[--read] = '\0';

        char *curr = line;
        int fieldIdx = 0;

        while (*curr && fieldIdx < 8) {
            if (*curr == '"')
                curr++;

            fields[fieldIdx++] = curr;

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

        if (fieldIdx < 8)
            continue;

        gboolean invalid = FALSE;
        gchar **flights = parseFlightIds(fields[1]);

        if (!checkReservationId(fields[0]) || !flights || !flights[0])
            invalid = TRUE;
        else {
            for (int i = 0; flights[i]; i++) {
                if (!checkFlightId(flights[i])) {
                    invalid = TRUE;
                    break;
                }
            }
        }

        if (!invalid && !checkDocumentNo(fields[2]))
            invalid = TRUE;

        if (invalid) {
            char tempBuffer[4096] = {0};

            for (int i = 0; i < 8; i++) {      // FIXED (was 12)
                if (i > 0) strcat(tempBuffer, ",");
                strcat(tempBuffer, "\"");
                strcat(tempBuffer, fields[i]);
                strcat(tempBuffer, "\"");
            }

            logInvalidLine("resultados/reservations_errors.csv",
                           headerLine, tempBuffer);

            *errorsFlag = 1;
            continue;
        }

        // ----- VALID ENTRY -----
        Reservation *data = g_slice_new(Reservation);

        data->reservation_id = g_strdup(fields[0]);
        data->flight_ids     = flights;
        data->document_no    = atoi(fields[2]);
        data->seat           = g_strdup(fields[3]);

        g_hash_table_insert(reservationsTable,
                            data->reservation_id,   // key
                            data);                  // value
    }

    // Cleanup
    g_free(headerLine);
    free(line);
    fclose(reservations);

    return reservationsTable;    // FIX: final return
}
*/

Reservation *getReservation(const gchar *id, GHashTable *reservationsTable) {
  Reservation *original = g_hash_table_lookup(reservationsTable, id);
  if (!original)
    return NULL;

  Reservation *reservation = g_new0(Reservation, 1); // initialize to zero
  reservation->reservation_id = g_strdup(original->reservation_id);
  reservation->flight_ids = g_strdupv(original->flight_ids);
  reservation->document_no = original->document_no;
  //reservation->seat = g_strdup(original->seat);
  //reservation->qr_code = g_strdup(original->qr_code);
  reservation->price = original->price;
  //reservation->extra_luggage = g_strdup(original->extra_luggage);
  //reservation->priority_boarding = g_strdup(original->priority_boarding);

  return reservation;
}

const gchar *getReservationId(const Reservation *r) {
  if (!r)
    return NULL;
  return r->reservation_id;
}

gchar **getReservationFlightIds(const Reservation *r) {
  if (!r)
    return NULL;
  return r->flight_ids;
}

int getReservationDocumentNo(const Reservation *r) {
  if (!r)
    return -1;
  return r->document_no;
}
/*
const gchar *getReservationSeat(const Reservation *r) {
  if (!r)
    return NULL;
  return r->seat;
}
*/

gfloat getReservationPrice(const Reservation *r) {
  if (!r)
    return -1.0f;
  return r->price;
}
/*
const gchar *getReservationExtraLuggage(const Reservation *r) {
  if (!r)
    return FALSE;
  return r->extra_luggage;
}

const gchar *getReservationPriorityBoarding(const Reservation *r) {
  if (!r)
    return FALSE;
  return r->priority_boarding;
}
  */
/*
const gchar *getReservationQrCode(const Reservation *r) {
  if (!r)
    return NULL;
  return r->qr_code;
}
  */

struct AiportPassengStats {
  long arrivals;
  long departures;
};

static AirportPassengerStats *getOrCreateStats(GHashTable *stats, const char *code) {
    AirportPassengerStats *s = g_hash_table_lookup(stats, code);
    if (!s) {
        s = g_new0(AirportPassengerStats, 1);
        g_hash_table_insert(stats, g_strdup(code), s);
    }
    return s;
}


GHashTable *buildAirportPassengerStats(GHashTable *reservations,
                                       GHashTable *flights) {

    GHashTable *stats = g_hash_table_new_full(g_str_hash, g_str_equal,
                                              g_free, g_free);

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, reservations);

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Reservation *res = value;
        if (!res) continue;

        gchar **flightIds = res->flight_ids;
        if (!flightIds) continue;

        for (int i = 0; flightIds[i] != NULL; i++) {

            if (!flightIds[i] || strlen(flightIds[i]) == 0) {
                continue;
            }

            Flight *flight = getFlight(flightIds[i], flights);
            if (!flight) continue;

            if (strcmp(getFlightStatus(flight), "Cancelled") == 0) {
                freeFlight(flight);
                continue;
            }

            const char *orig = getFlightOrigin(flight);
            const char *dest = getFlightDestination(flight);

            if (orig)
                getOrCreateStats(stats, orig)->departures++;

            if (dest)
                getOrCreateStats(stats, dest)->arrivals++;

            freeFlight(flight);
        }
    }

    return stats;
}

long getAirportArrivals(const AirportPassengerStats *s) {
    return s ? s->arrivals : 0;
}

long getAirportDepartures(const AirportPassengerStats *s) {
    return s ? s->departures : 0;
}
