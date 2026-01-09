#define _XOPEN_SOURCE 700
#include "glib.h"
#include <core/time_utils.h>
#include <ctype.h>
#include <entities/access/flights_access.h>
#include <entities/access/reservations_access.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <validation.h>

// NOTE: ########## AUX VALIDATIONS ##########

#define NUM_REGEX "^[0-9]+$"
#define AIRCRAFTID_REGEX "^[0-9A-Z]{2}-[0-9A-Z]{5}$"
gboolean checkInt(const gchar *str) { return checkRegex(NUM_REGEX, str); }

gboolean checkAircraftId(const char *id)
{
    if (!id)
        return FALSE;

    int i = 0;
    const char *ptr = id;

    for (; i < 2; i++, ptr++)
    {
        if (!isupper(*ptr) && !isdigit(*ptr))
            return FALSE;
    }

    if (*ptr != '-')
        return FALSE;
    ptr++;

    for (i = 0; i < 5; i++, ptr++)
    {
        if (!isupper(*ptr) && !isdigit(*ptr))
            return FALSE;
    }
    if (*ptr != '\0')
        return FALSE;

    return TRUE;
}

gboolean checkAirportCode(const char *code)
{
    if (!code)
        return FALSE;

    const char *ptr = code;
    for (int i = 0; i < 3; i++, ptr++)
    {
        if (!isupper(*ptr))
            return FALSE;
    }
    if (*ptr != '\0')
        return FALSE;

    return TRUE;
}

// NOTE: ########### DATE VALIDATION ##########

// as defined in the guidelines (subsec. 5.2)
#define TODAY "2025-09-30"
#define TODAY_T ((time_t)1759190400)

gboolean checkDatetime(const time_t dt)
{

    // Check for any error codes (-1 to -4), as "negative dates" (before 1970)
    // will never result in these codes
    if (dt == (time_t)-1 || dt == (time_t)-2 || dt == (time_t)-3 ||
        dt == (time_t)-4)
    {
        return FALSE;
    }

    if (dt > TODAY_T)
    {
        return FALSE;
    }

    return TRUE;
}

gboolean checkDate(const time_t dt)
{

    // Check for any error codes (-1 to -4), as "negative dates" (before 1970)
    // will never result in these codes
    if (dt == (time_t)-1 || dt == (time_t)-2 || dt == (time_t)-3 ||
        dt == (time_t)-4)
    {
        return FALSE;
    }

    time_t today = parse_unix_date(TODAY, NULL);

    if (dt > today)
    {
        return FALSE;
    }

    return TRUE;
}

gboolean checkYear(const gchar *yearChar)
{
    if (!yearChar)
        return 0;

    const gchar *p = yearChar;

    for (int i = 0; i < 4; i++)
    {
        if (!isdigit((unsigned char)*p))
            return FALSE;
        p++;
    }
    if (*p != '\0')
        return FALSE;

    gint year = atoi(yearChar);
    return g_date_valid_year(year);
}

// NOTE: ########## EMAIL VALIDATION ##########

gboolean checkEmail(const char *email)
{
    if (!email)
        return FALSE;

    const char *ptr = email;

    // Invalid if username's first character is '.'
    if (!(isdigit(*ptr) || islower(*ptr)))
        return FALSE;
    ptr++;

    // Valid if username only contains a-z0-9 and . but not consecutive ..
    while (*ptr != '@')
    {
        // Found the end of the string without finding '@'
        if (*ptr == '\0')
            return FALSE;

        // After a dot, the character must be a digit or a lowercase letter
        if (*ptr == '.')
        {
            ptr++;

            if (!(isdigit(*ptr) || islower(*ptr)))
                return FALSE;
        }
        else if (!(isdigit(*ptr) || islower(*ptr)))
            // If it's not [a-z0-9] or a dot, it's already invalid
            return FALSE;

        ptr++;
    }

    // If ptr exited the loop, *ptr == '@' - skip
    ptr++;

    // Validate domain
    int left = 0, right = 0;

    // Iterate thorugh the lstring, while *ptr != '.'
    while (*ptr != '.')
    {
        // Inavlid if found end of string
        if (*ptr == '\0')
            return FALSE;

        // If not [a-z] it's invalid
        if (!islower(*ptr))
            return FALSE;

        // Increase lstring size
        left++;
        ptr++;
    }

    // Validate lstring length
    if (left < 1)
        return FALSE;

    // Skip '.'
    ptr++;

    // Iterate over the rest of the domain
    while (*ptr != '\0')
    {
        if (!islower(*ptr))
            return FALSE;

        right++;
        ptr++;
    }

    // rstring has 2 or 3 characters
    if (right != 2 && right != 3)
        return FALSE;

    return TRUE;
}

// NOTE: ########## AIRPORT VALIDATION ##########

#define LTTD_REGEX "^-?[0-9]{2}\\.[0-9]{1,8}$"
#define LGTD_REGEX "^-?[0-9]{3}\\.[0-9]{1,8}$"

gboolean checkType(const gchar *type)
{
    gchar *types[] = {"small_airport", "medium_airport", "large_airport",
                      "heliport", "seaplane_base"};

    for (int i = 0; i < 5; i++)
    {
        if (!strcmp(type, types[i]))
            return 1;
    }

    return 0;
}

/*
gboolean checkCoords(const gchar *latitude, const gchar *longitude) {
  if (!checkRegex(LTTD_REGEX, latitude) || !checkRegex(LGTD_REGEX, longitude))
    return 0;

  gdouble lttd = g_ascii_strtod(latitude, NULL);
  gdouble lgtd = g_ascii_strtod(longitude, NULL);
  if (!lttd || !lgtd)
    return 0;

  if (lttd < -90 || lttd > 90 || lgtd < -180 || lgtd > 180)
    return 0;

  return 1;
}
  */
gboolean checkCoords(const gchar *latitude, const gchar *longitude)
{
    if (!latitude || !longitude)
        return FALSE;

    // ######### Latitude ##########
    const gchar *p = latitude;
    gint digits = 0;

    if (*p == '-' || *p == '+')
        p++;
    while (*p != '.' && isdigit((unsigned char)*p))
    {
        digits++;
        p++;
    }
    if (*p != '.')
        return FALSE;
    if (digits == 0 || digits > 2)
        return FALSE;

    p++;
    digits = 0;
    while (*p != '\0' && isdigit((unsigned char)*p))
    {
        digits++;
        p++;
    }
    if (*p != '\0')
        return FALSE;
    if (digits < 1 || digits > 8)
        return FALSE;

    gdouble lattd = g_ascii_strtod(latitude, NULL);
    if (lattd < -90 || lattd > 90)
        return FALSE;

    // ########## Longitude ##########
    p = longitude;
    digits = 0;

    if (*p == '-' || *p == '+')
        p++;
    while (*p != '.' && isdigit((unsigned char)*p))
    {
        digits++;
        p++;
    }
    if (*p != '.')
        return FALSE;
    if (digits == 0 || digits > 3)
        return FALSE;

    p++;
    digits = 0;
    while (*p != '\0' && isdigit((unsigned char)*p))
    {
        digits++;
        p++;
    }
    if (*p != '\0')
        return FALSE;
    if (digits < 1 || digits > 8)
        return FALSE;

    gdouble lngtd = g_ascii_strtod(longitude, NULL);
    if (lngtd < -180 || lngtd > 180)
        return FALSE;

    return TRUE;
}

// NOTE: ########## FLIGHT VALIDATION ##########
gboolean checkFlightId(const gchar *id)
{
    int i = 0;
    for (; i < 2; i++)
    {
        if (!isupper(id[i]))
            return FALSE;
    }

    for (; i < 7; i++)
    {
        if (!isdigit(id[i]))
            return FALSE;
    }

    return TRUE;
}

gboolean checkDestinationOrigin(const gchar *destination, const gchar *origin)
{
    return strcmp(destination, origin);
}

gint compareAircraft(const void *a, const void *b)
{
    // 'a' and 'b' are *pointers to pointers of gchar*
    const gchar *idA = *(const gchar **)a;
    const gchar *idB = *(const gchar **)b;
    return g_strcmp0(idA, idB);
}

gboolean checkAircraft(const gchar *flightField, GArray *aircrafts)
{
    gchar *key = (gchar *)flightField;
    guint index;
    return g_array_binary_search(aircrafts, &key, compareAircraft, &index);
}

// Returns TRUE if the delayed status is consistent with the actual vs scheduled
// times
gboolean checkDelayed(const gchar *status, time_t scheduledDep,
                      time_t scheduledArr, time_t actualDep, time_t actualArr,
                      int cancel_flag)
{
    if (!status)
        return FALSE;

    // Only check if status is "Delayed"
    if (strcmp(status, "Delayed") != 0)
        return TRUE;

    // If actual times are "N/A", the check fails
    if (cancel_flag == 1)
        return FALSE;

    // Compare actual vs scheduled
    gint cmpDep = compare_time_t(actualDep, scheduledDep);
    gint cmpArr = compare_time_t(actualArr, scheduledArr);

    return (cmpDep >= 0 && cmpArr >= 0);
}

// Returns TRUE if the cancellation status is consistent with actual times
gboolean checkCancellation(const gchar *status, time_t act_dep,
                           time_t act_arr)
{
    if (g_strcmp0(status, "Cancelled") != 0)
        return TRUE;

    return (act_dep == (time_t)-1 && act_arr == (time_t)-1);
}

// NOTE: ########## RESERVATION VALIDATION #########

#define RESERVATION_REGEX "^R[0-9]{9}$"

gboolean checkReservationId(const gchar *id)
{
    return checkRegex(RESERVATION_REGEX, id);
}

gint compareReservations(const void *a, const void *b)
{
    return g_strcmp0((const gchar *)a, (const gchar *)b);
}

gboolean checkReservationIdList(const gchar *flight, GArray *reservations)
{
    return g_array_binary_search(reservations, flight, compareReservations, NULL);
}

/*
gint checkFlightConnection(GHashTable *reservationsTable,
                           const GHashTable *flightsTable, const gchar *header,
                           const gchar *errorsFile) {
  gboolean noFlights, invalidFlight, tooManyFlights;
  tooManyFlights = noFlights = invalidFlight = FALSE;
  GHashTableIter iter;
  gpointer key, value;
  GPtrArray *toRemove = g_ptr_array_new();

  g_hash_table_iter_init(&iter, reservationsTable);

  while (g_hash_table_iter_next(&iter, &key, &value)) {
    Reservation *reservation = (Reservation *)value;

    gchar **flightIds = getReservationFlightIds(reservation);
    int count = 0;
    while (flightIds && flightIds[count] != NULL) {
      count++;
    }

    // If the current reservation has no flights, return an error
    if (count == 0) {
      noFlights = TRUE;
      // printf("Reservation - %s - has no flights\n",
      //        getReservationId(reservation));
    } else if (count == 1) {

      Flight *flight1 = getFlight(flightIds[0], flightsTable);
      // Flight doesn't exist
      if (!flight1) {
        // printf("Flight1 - %s - doesn't exist\n", flightIds[0]);
        invalidFlight = TRUE;
      }
      if (flight1)
        freeFlight(flight1);
    } else if (count == 2) {
      Flight *flight1 = getFlight(flightIds[0], flightsTable);
      Flight *flight2 = getFlight(flightIds[1], flightsTable);

      if (!flight1 || !flight2 ||
          g_strcmp0(getFlightDestination(flight1), getFlightOrigin(flight2)) !=
              0) {
        // if (!flight1)
        //   printf("Flight1 - %s - doesn't exist\n", flightIds[0]);
        // if (!flight2)
        //   printf("Flight2 - %s - doesn't exist\n", flightIds[1]);
        // if (flight1 && flight2)
        //   printf("Reservation -\t%s   F1 - \t%s   F2 - \t%s\n",
        //          getReservationId(reservation),
        //          getFlightDestination(flight1), getFlightOrigin(flight2));

        invalidFlight = TRUE;
      }
      if (flight1)
        freeFlight(flight1);
      if (flight2)
        freeFlight(flight2);
    } else if (count > 2) {
      tooManyFlights = TRUE;
      // printf("Reservation - %s - has - %d - flighst\n",
      //       getReservationId(reservation), count);
    }

    if (noFlights || invalidFlight || tooManyFlights) {

      // Reconstruct CSV reservation line using header(reservations)
      gchar *flight_list = g_strdup("["); // concat flights IDs
      for (gint i = 0; flightIds[i] != NULL; i++) {
        gchar *tmp = g_strdup_printf("%s%s'%s'", flight_list, i > 0 ? ", " : "",
                                     flightIds[i]);
        g_free(flight_list);
        flight_list = tmp;
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
          getReservationSeat(reservation) ? getReservationSeat(reservation)
                                          : "",
          getReservationPrice(reservation),
          getReservationExtraLuggage(reservation),
          getReservationPriorityBoarding(reservation),
          getReservationQrCode(reservation) ? getReservationQrCode(reservation)
                                            : "");

      logInvalidLine(errorsFile, header, line);

      g_free(flight_list);

      // Save reservation key to remove later
      g_ptr_array_add(toRemove, key);

      return 0;
    }
  }

  // remove out of the iter
  for (guint i = 0; i < toRemove->len; i++) {
    gpointer k = g_ptr_array_index(toRemove, i);
    // printf("To remove - \t%s\n", (gchar *)k);
    g_hash_table_remove(reservationsTable, k);
  }

  gboolean anyRemoved = (toRemove->len > 0);
  g_ptr_array_free(toRemove, TRUE);

  return anyRemoved ? 0 : 1;
}
*/

gboolean checkReservationFlightDestOrig(const gchar *DestinationFirstFlight,
                                        const gchar *OriginSecondFlight)
{
    return (strcmp(DestinationFirstFlight, OriginSecondFlight) == 0);
}

// NOTE: ########## DOCUMENT VALIDATION #########

gboolean checkDocumentNo(const gchar *no)
{

    const char *ptr = no;
    for (int i = 0; i < 9; i++)
    {
        if (!isdigit(*ptr))
            return FALSE;
        ptr++;
    }
    if (*ptr != '\0')
        return FALSE;

    return TRUE;
}

gint compareDocuments(const void *a, const void *b)
{
    return g_strcmp0((const gchar *)a, (const gchar *)b);
}

gboolean checkDocumentNumber(const gchar *docNumber, GArray *passengers)
{
    return g_array_binary_search(passengers, docNumber, compareDocuments, NULL);
}

// NOTE: ########## PASSENGER VALIDATION #########

gboolean checkPassangerGender(const gchar *gender)
{

    return (gender[1] == '\0' &&
            (gender[0] == 'F' || gender[0] == 'M' || gender[0] == 'O'));
}

// NOTE: ########## CSV VALIDATION #########

#define LIST_REGEX "^\\[.*\\]$"

gboolean checkCsvList(const gchar *list)
{
    return checkRegex(LIST_REGEX, list);
}

// NOTE: ########## LogInvalidData ##########

void logInvalidLine(const gchar *filename, const gchar *header,
                    const gchar *line)
{
    FILE *file = fopen(filename, "a"); // open in append mode
    if (!file)
    {
        return;
    }
    if (ftell(file) == 0)
    { // file is empty, write header first
        fprintf(file, "%s\n", header);
    }
    gchar *safe_line = g_strdup(line);
    fprintf(file, "%s\n", safe_line);
    g_free(safe_line);
    fclose(file);
}