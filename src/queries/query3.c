#include <core/dataset.h>
#include <core/fenwick.h>
#include <core/time_utils.h>
#include <entities/airports.h>
#include <core/indexer.h>
#include <glib.h>
#include <queries/query3.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <validation.h>

gchar *query3Aux(const gchar *code, const Dataset *ds)
{
  if (!code || !ds)
    return NULL;

  const Airport *airport = dataset_get_airport(ds, code);
  if (!airport)
    return NULL;

  const gchar *name = getAirportName(airport);
  const gchar *city = getAirportCity(airport);
  const gchar *country = getAirportCountry(airport);

  name = name ? name : "";
  city = city ? city : "";
  country = country ? country : "";

  return g_strconcat(code, ";", name, ";", city, ";", country, NULL);
}

gchar *query3(GHashTable *airportFtrees, const Dataset *ds,
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

  gchar *airportName = query3Aux(bestAirport, ds);
  gchar *result = g_strdup_printf("%s;%d", airportName, bestCount);
  g_free(airportName);

  return result;
}