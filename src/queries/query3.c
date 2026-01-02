#include <core/dataset.h>
#include <core/fenwick.h>
#include <core/time_utils.h>
#include <entities/airports.h>
#include <entities/flights.h>
#include <glib.h>
#include <queries/query1.h>
#include <queries/query3.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <validation.h>

// Used only because it only needs name, city and country (airport)
gchar *query3Aux(const gchar *code, const GHashTable *airportsTable)
{
  if (!code || !airportsTable)
    return NULL;

  const Airport *airport = getAirport(code, airportsTable);
  if (!airport)
    return NULL;

  const gchar *name = getAirportName(airport);
  const gchar *city = getAirportCity(airport);
  const gchar *country = getAirportCountry(airport);

  name = name ? name : "";
  city = city ? city : "";
  country = country ? country : "";

  gchar *result = g_strconcat(code, ";", name, ";", city, ";", country, NULL);
  return result;
}

gchar *query3(GHashTable *airportFtrees, GHashTable *airports,
              const char *startStr, const char *endStr)
{
  time_t start_date = parse_unix_date(startStr, NULL);
  time_t end_date = parse_unix_date(endStr, NULL);

  const gchar *bestAirport = NULL;
  int bestCount = 0;

  GHashTableIter iter;
  gpointer key, val;
  g_hash_table_iter_init(&iter, airportFtrees);
  while (g_hash_table_iter_next(&iter, &key, &val))
  {
    const gchar *code = (const gchar *)key;
    FTree *tree = getFTree(val);
    int n = getFtreeN(tree);
    time_t *dates = getFtreeDates(tree);

    // Binary search for start index (1-based)
    int lower = 0, upper = n - 1, start = n + 1;
    while (lower <= upper)
    {
      int mid = (lower + upper) / 2;
      if (compare_time_t(dates[mid], start_date) >= 0)
      {
        start = mid + 1;
        upper = mid - 1;
      }
      else
      {
        lower = mid + 1;
      }
    }

    // Binary search for end index
    lower = 0;
    upper = n - 1;
    int end = 0;
    while (lower <= upper)
    {
      int mid = (lower + upper) / 2;
      if (compare_time_t(dates[mid], end_date) <= 0)
      {
        end = mid + 1;
        lower = mid + 1;
      }
      else
      {
        upper = mid - 1;
      }
    }

    int count = ftree_range_sum(tree, start, end);

    if (count > bestCount ||
        (count == bestCount &&
         (bestAirport == NULL || strcmp(code, bestAirport) < 0)))
    {
      bestAirport = code;
      bestCount = count;
    }
  }

  if (bestCount == 0 || !bestAirport)
    return NULL;

  gchar *airportName = query3Aux(bestAirport, airports);
  gchar *result = g_strdup_printf("%s;%d", airportName, bestCount);
  g_free(airportName);

  return result; // caller must g_free()
}