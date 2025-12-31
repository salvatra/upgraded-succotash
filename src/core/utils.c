#include "core/time_utils.h"
#include <ctype.h>
#include <sys/stat.h>
#include <time.h>
#define _XOPEN_SOURCE 700
#include <core/utils.h>
#include <entities/aircrafts.h>
#include <entities/flights.h>
#include <entities/passengers.h>
#include <entities/reservations.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <validation.h>

gboolean checkRegex(const gchar *pattern, const gchar *string) {
  if (!string || !*string)
    return FALSE;

  GError *error = NULL;
  GRegex *regex = g_regex_new(pattern, 0, 0, &error);

  if (!regex) {
    // Check for regex compilation failure
    g_printerr("Regex compilation error: %s\n", error->message);
    g_error_free(error);
    return -1;
  }

  gboolean result = g_regex_match(regex, string, 0, 0);
  g_regex_unref(regex);

  return result;
}

void trim(gchar *s) {
  gchar *start = s;
  while (*start == '\"')
    start++;
  memmove(s, start, strlen(start) + 1);
  gchar *end = s + strlen(s) - 1;
  while (end > s && *end == '\"')
    end--;
  *(end + 1) = '\0';
}

void trim_whitespace(gchar *s) {
  if (!s) return;
  gchar *start = s;
  while (*start && isspace((unsigned char)*start))
    start++;
  memmove(s, start, strlen(start) + 1);
  gchar *end = s + strlen(s) - 1;
  while (end > s && isspace((unsigned char)*end))
    end--;
  *(end + 1) = '\0';
}
/*
gint checkAircraftAsKey(GHashTable *flightsTable, GHashTable *aircraftsTable,
                        const gchar *header, const gchar *errorsFile) {
  GHashTableIter iter;
  gpointer key, value;
  GPtrArray *toRemove = g_ptr_array_new(); // keys to remove later

  g_hash_table_iter_init(&iter, flightsTable);
  while (g_hash_table_iter_next(&iter, &key, &value)) {
    Flight *flight = (Flight *)value;

    const gchar *aircraft = getFlightAircraft(flight);

    // If aircraft is NULL or not in aircraftsTable, mark as invalid
    if (!aircraft || !g_hash_table_contains(aircraftsTable, aircraft)) {

      // Safely log the flight without touching the original struct
      char dep[20], act_dep[20], arr[20], act_arr[20];
      int dep_err = format_time_t(getFlightDeparture(flight), dep);
      int act_dep_err =
          format_time_t(getFlightActualDeparture(flight), act_dep);
      int arr_err = format_time_t(getFlightArrival(flight), arr);
      int act_arr_err = format_time_t(getFlightActualArrival(flight), act_arr);

      gchar line[1024];
      snprintf(
          line, sizeof(line),
          "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%d\",\"%s\",\"%s\",\"%s\","
          "\"%s\",\"%s\",\"%s\"",
          getFlightId(flight) ? getFlightId(flight) : "",
          (dep_err < (time_t)0) ? "N/A" : dep,
          (act_dep_err < (time_t)0) ? "N/A" : act_dep,
          (arr_err < (time_t)0) ? "N/A" : arr,
          (act_arr_err < (time_t)0) ? "N/A" : act_arr, getFlightGate(flight),
          getFlightStatus(flight) ? getFlightStatus(flight) : "",
          getFlightOrigin(flight) ? getFlightOrigin(flight) : "",
          getFlightDestination(flight) ? getFlightDestination(flight) : "",
          aircraft ? aircraft : "",
          getFlightAirline(flight) ? getFlightAirline(flight) : "",
          getFlightTrackingUrl(flight) ? getFlightTrackingUrl(flight) : "");

      // Log invalid flight safely
      logInvalidLine(errorsFile, header, line);

      printf("To remove - \t%s\n", line);

      // Mark key to remove
      g_ptr_array_add(toRemove, key);
    }
  }

  // Remove invalid flights outside the iterator
  for (guint i = 0; i < toRemove->len; i++) {
    gpointer k = g_ptr_array_index(toRemove, i);
    g_hash_table_remove(flightsTable, k);
  }

  gboolean anyRemoved = (toRemove->len > 0);
  g_ptr_array_free(toRemove, TRUE);

  return anyRemoved ? 0 : 1;
}

gint checkDocumentAsKey(GHashTable *reservationsTable,
                        GHashTable *passengersTable, const gchar *header,
                        const gchar *errorsFile) {
  GHashTableIter iter;
  gpointer key, value;
  GPtrArray *toRemove = g_ptr_array_new();

  g_hash_table_iter_init(&iter, reservationsTable);

  while (g_hash_table_iter_next(&iter, &key, &value)) {
    Reservation *reservation = (Reservation *)value;

    const gchar *doc = getReservationDocumentNo(reservation);
    // If passenger exists → skip

    if (doc && g_hash_table_contains(passengersTable, doc))
      continue;

    // printf("Reservation - \t%s   DocNo - \t%s\n",
    // getReservationId(reservation),
    //      doc);

    gchar **flights = getReservationFlightIds(reservation);
    gchar *flight_list = g_strdup("[");

    if (flights) {
      for (gint i = 0; flights[i] != NULL; i++) {
        gchar *tmp = g_strdup_printf("%s%s'%s'", flight_list, i > 0 ? ", " : "",
                                     flights[i]);
        g_free(flight_list);
        flight_list = tmp;
      }
    }
    gchar *final_list = g_strdup_printf("%s]", flight_list);
    g_free(flight_list);
    flight_list = final_list;

    gchar line[1024];
    snprintf(
        line, sizeof(line),
        "\"%s\",\"%s\",\"%s\",\"%s\",\"%.2f\",\"%s\",\"%s\",\"%s\"",
        getReservationId(reservation) ? getReservationId(reservation) : "",
        flight_list,
        getReservationDocumentNo(reservation)
            ? getReservationDocumentNo(reservation)
            : "",
        getReservationSeat(reservation) ? getReservationSeat(reservation) : "",
        getReservationPrice(reservation),
        getReservationExtraLuggage(reservation),
        getReservationPriorityBoarding(reservation),
        getReservationQrCode(reservation) ? getReservationQrCode(reservation)
                                          : "");

    // log dof the invalid line
    logInvalidLine(errorsFile, header, line);

    g_free(flight_list);

    // Save reservation key to remove later
    g_ptr_array_add(toRemove, key);
  }

  // ------------ Remove invalid reservations ------------
  for (guint i = 0; i < toRemove->len; i++)
    g_hash_table_remove(reservationsTable, g_ptr_array_index(toRemove, i));

  gboolean removed = (toRemove->len > 0);
  g_ptr_array_free(toRemove, TRUE);

  return removed ? 0 : 1;
}
  */

int parse_price_cents(const char *price_str) {
  int price = 0;
  int i = 0;

  while (isdigit(price_str[i])) {
    price = (price * 10) + (price_str[i] - '0');
    i++;
  }

  price *= 100;

  if (price_str[i] == '.') {
    i++;

    if (isdigit(price_str[i])) {
      price += (price_str[i] - '0') * 10;
      i++;
    }

    if (isdigit(price_str[i])) {
      price += (price_str[i] - '0');
    }
  }

  return price;
}

float cents_to_euro(int cents) {
  float euro = 0.0;

  euro += cents / 100.0;

  return euro;
}
/*
char *euro_to_string(float euros) {
 char *string = NULL;

 // TODO:

 return string;
}
*/

gboolean checkPath(const gchar *path) {
  struct stat buffer;
  // stat returns 0 on success (path exists)
  return (stat(path, &buffer) == 0);
}
