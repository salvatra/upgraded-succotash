#include <queries/query1.h>
#include <core/dataset.h>
#include <core/statistics.h>
#include <entities/airports.h>
#include <string.h>
#include <stdio.h>

gchar *query1(const gchar *code, const Dataset *ds) {

  const Airport *airport = dataset_get_airport(ds, code);
  if (!airport)
    return NULL;

  const AirportPassengerStats *s = dataset_get_airport_stats(ds, code);

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