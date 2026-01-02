#include <queries/query1.h>
#include <string.h>
#include <stdio.h>

gchar *query1(const gchar *code,
              const GHashTable *airports,
              const GHashTable *airportStats)
{

  const Airport *airport = getAirport(code, airports);
  if (!airport)
    return NULL;

  const AirportPassengerStats *s =
      g_hash_table_lookup((GHashTable *)airportStats, code);

  long arrivals = getAirportArrivals(s);
  long departures = getAirportDepartures(s);

  gchar bufArr[32], bufDep[32];
  snprintf(bufArr, sizeof(bufArr), "%ld", arrivals);
  snprintf(bufDep, sizeof(bufDep), "%ld", departures);

  gchar *result = g_strconcat(
      code, ";",
      getAirportName(airport), ";",
      getAirportCity(airport), ";",
      getAirportCountry(airport), ";",
      getAirportType(airport), ";",
      bufArr, ";",
      bufDep,
      NULL);
  return result;
}