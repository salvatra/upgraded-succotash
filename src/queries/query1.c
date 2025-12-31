#include <queries/query1.h>
#include <string.h>
#include <stdio.h>

gchar *query1(const gchar *code,
              const GHashTable *airports,
              const GHashTable *airportStats) {

  Airport *airport = getAirport(code, airports);
  if (!airport)
    return NULL;

  AirportPassengerStats *s =
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
    freeAirport(airport);
  return result;
}

// Query 1 antiga
/*
gchar *query1(gchar *code, const GHashTable *airportsTable) {
  if (!code || !airportsTable)
    return NULL;

  Airport *airport = getAirport(code, airportsTable);
  if (!airport)
    return NULL;

  gchar *name = getAirportName(airport);
  gchar *city = getAirportCity(airport);
  gchar *country = getAirportCountry(airport);
  gchar *type = getAirportType(airport);

  name = name ? name : "";
  city = city ? city : "";
  country = country ? country : "";
  type = type ? type : "";

  gchar *result =
      g_strconcat(code, ",", name, ",", city, ",", country, ",", type, NULL);
  freeAirport((gpointer)airport);
  return result;
}
  */
