#include <queries/query1.h>
#include <queries/query_module.h> // Include the interface
#include <core/dataset.h>
#include <core/statistics.h>
#include "entities/access/airports_access.h"
#include <string.h>
#include <stdio.h>

// --- Core Logic (Unchanged) ---

gchar *query1(const gchar *code, const Dataset *ds)
{
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
static void q1_run_wrapper(void *ctx, Dataset *ds, char *arg1, char *arg2, int isSpecial, FILE *output)
{

  (void)ctx;
  (void)arg2;

  gchar *res = query1(arg1, ds);

  if (res)
  {
    if (isSpecial)
    {
      for (char *p = res; *p; p++)
      {
        if (*p == ';')
          *p = '=';
      }
    }
    fprintf(output, "%s\n", res);
    g_free(res);
  }
  else
  {
    fprintf(output, "\n");
  }
}


QueryModule get_query1_module(void)
{
  QueryModule mod = {
      .id = 1,
      .init = NULL,
      .run = q1_run_wrapper,
      .destroy = NULL
  };
  return mod;
}