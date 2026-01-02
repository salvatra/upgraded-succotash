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

struct flight
{
  gchar *id;
  time_t departure;
  time_t actual_departure;
  time_t arrival;
  time_t actual_arrival;
  gchar *status;
  gchar *origin;
  gchar *destination;
  gchar *aircraft;
  gchar *airline;
};

void freeFlight(gpointer data)
{
  if (!data)
    return;
  Flight *flight = data;
  g_free(flight->id);
  g_free(flight->origin);
  g_free(flight->destination);
  g_free(flight->aircraft);
  g_free(flight->status);
  g_free(flight->airline);
  g_free(flight);
}

GHashTable *readFlights(const char *filename, int *errorsFlag, GHashTable *aircrafts)
{
  FILE *flights = fopen(filename, "r");
  if (!flights)
    return NULL;

  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  if ((read = getline(&line, &len, flights)) == -1)
  {
    free(line);
    fclose(flights);
    return NULL;
  }
  if (read > 0 && line[read - 1] == '\n')
    line[--read] = '\0';
  char *headerLine = g_strdup(line);

  GHashTable *flightsTable =
      g_hash_table_new_full(g_str_hash, g_str_equal, NULL, freeFlight);

  while ((read = getline(&line, &len, flights)) != -1)
  {
    ParsedFieldsF *pf = parseFlightLineRaw(line);
    if (!parsed_fields_ok(pf))
    {
      parsed_fields_free(pf);
      continue;
    }

    gboolean invalid = FALSE;
    const char *fields[12];
    for (int i = 0; i < 12; i++)
      fields[i] = parsed_fields_get(pf, i);

    // --- VALIDATION  ---
    if (!checkFlightId(fields[0]))
      invalid = TRUE;

    time_t sched_dep = 0, act_dep = 0, sched_arr = 0, act_arr = 0;
    int cancelled = 0;

    if (!invalid)
    {
      sched_dep = parse_unix_datetime(fields[1], NULL);
      if (sched_dep < 0)
        invalid = TRUE;

      sched_arr = parse_unix_datetime(fields[3], NULL);
      if (sched_arr < 0)
        invalid = TRUE;

      act_dep = parse_unix_datetime(fields[2], &cancelled);
      if (act_dep < 0 && cancelled == 0)
        invalid = TRUE;

      act_arr = parse_unix_datetime(fields[4], &cancelled);
      if (act_arr < 0 && cancelled == 0)
        invalid = TRUE;
    }

    if (!invalid)
    {
      if (!checkDelayed(fields[6], sched_dep, sched_arr, act_dep, act_arr, cancelled) ||
          !checkCancellation(fields[6], act_dep, act_arr) ||
          !checkDestinationOrigin(fields[8], fields[7]))
      {
        invalid = TRUE;
      }
    }

    if (!invalid)
    {
      if (compare_unix_datetime(fields[1], fields[2]) > 0 ||
          compare_unix_datetime(fields[3], fields[4]) > 0 ||
          compare_unix_datetime(fields[1], fields[3]) >= 0 ||
          compare_unix_datetime(fields[2], fields[4]) >= 0)
      {
        invalid = TRUE;
      }
    }

    if (!invalid)
    {
      if (!checkAirportCode(fields[7]) || !checkAirportCode(fields[8]))
        invalid = TRUE;
    }

    if (!invalid && (!fields[9] || !g_hash_table_contains(aircrafts, fields[9])))
    {
      invalid = TRUE;
    }

    if (!invalid)
    {
      if (!checkAircraftId(fields[9]))
        invalid = TRUE;
    }

    if (invalid)
    {
      char tempBuffer[4096] = "";
      for (int i = 0; i < 12; i++)
      {
        if (i > 0)
          strcat(tempBuffer, ",");
        strcat(tempBuffer, "\"");
        strcat(tempBuffer, fields[i]);
        strcat(tempBuffer, "\"");
      }
      logInvalidLine("resultados/flights_errors.csv", headerLine, tempBuffer);
      *errorsFlag = 1;
      parsed_fields_free(pf);
      continue;
    }

    // --- STRUCT CREATION ---
    Flight *data = g_new0(Flight, 1);

    data->id = g_strdup(fields[0]);
    data->departure = sched_dep;
    data->actual_departure = act_dep;
    data->arrival = sched_arr;
    data->actual_arrival = act_arr;
    data->status = g_strdup(fields[6]);
    data->origin = g_strdup(fields[7]);
    data->destination = g_strdup(fields[8]);
    data->aircraft = g_strdup(fields[9]);
    data->airline = g_strdup(fields[10]);

    g_hash_table_insert(flightsTable, data->id, data);

    parsed_fields_free(pf);
  }

  g_free(headerLine);
  free(line);
  fclose(flights);
  return flightsTable;
}

const Flight *getFlight(const gchar *id, const GHashTable *flightsTable)
{
  if (!id || !flightsTable)
    return NULL;
  return (const Flight *)g_hash_table_lookup((GHashTable *)flightsTable, id);
}

const gchar *getFlightId(const Flight *f)
{
  return f ? f->id : NULL;
}

time_t getFlightDeparture(const Flight *f)
{
  return f ? f->departure : (time_t)-1;
}

time_t getFlightActualDeparture(const Flight *f)
{
  return f ? f->actual_departure : (time_t)-1;
}

time_t getFlightArrival(const Flight *f)
{
  return f ? f->arrival : (time_t)-1;
}

time_t getFlightActualArrival(const Flight *f)
{
  return f ? f->actual_arrival : (time_t)-1;
}

const gchar *getFlightStatus(const Flight *f)
{
  return f ? f->status : NULL;
}

const gchar *getFlightOrigin(const Flight *f)
{
  return f ? f->origin : NULL;
}

const gchar *getFlightDestination(const Flight *f)
{
  return f ? f->destination : NULL;
}

const gchar *getFlightAircraft(const Flight *f)
{
  return f ? f->aircraft : NULL;
}

const gchar *getFlightAirline(const Flight *f)
{
  return f ? f->airline : NULL;
}