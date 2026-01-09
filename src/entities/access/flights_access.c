#include "entities/access/flights_access.h"
#include "entities/internal/flights_internal.h"
#include <glib.h>
#include <string.h>

void freeFlight(gpointer data)
{
  Flight *f = (Flight *)data;
  if (!f)
    return;

  g_free(f->id);
  g_free(f->origin);
  g_free(f->destination);
  g_free(f->aircraft);
  g_free(f->airline);

  g_free(f);
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

const gchar *getFlightStatus(const Flight *flight)
{
  if (!flight)
    return NULL;

  switch (flight->status)
  {
  case FLIGHT_ON_TIME:
    return "On Time";
  case FLIGHT_DELAYED:
    return "Delayed";
  case FLIGHT_CANCELLED:
    return "Cancelled";
  default:
    return "";
  }
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