#include "core/fenwick.h"
#include "core/time_utils.h"
#include "entities/flights.h"
#include <core/dataset.h>
#include <core/indexer.h>
#include <glib.h>
#include <stdio.h>
#include <time.h>

struct ftree
{
  int n;
  time_t *dates;
  int *bit;
};

void freeFTree(gpointer data)
{
  FTree *tree = (FTree *)data;
  if (tree)
  {
    g_free(tree->dates);
    g_free(tree->bit);
    g_free(tree);
  }
}

GHashTable *getFTrees(GHashTable *airportDepartures, const Dataset *ds)
{

  GHashTable *airportTrees = g_hash_table_new_full(
      g_str_hash, g_str_equal, g_free, (GDestroyNotify)freeFTree);

  // 1. Configure trees (based on indexer)
  GHashTableIter diter;
  gpointer dkey, dval;
  g_hash_table_iter_init(&diter, airportDepartures);

  while (g_hash_table_iter_next(&diter, &dkey, &dval))
  {
    gchar *airportCode = (gchar *)dkey;
    DatesInfo *di = (DatesInfo *)dval;

    int nDates = getDiDates(di)->len;

    FTree *tree = g_new0(FTree, 1);
    tree->n = nDates;
    tree->dates = g_new0(time_t, nDates);

    for (int i = 0; i < nDates; i++)
    {
      time_t dt = g_array_index(getDiDates(di), time_t, i);
      tree->dates[i] = dt;
    }

    // index 1..n
    tree->bit = g_new0(int, nDates + 1);

    g_hash_table_insert(airportTrees, g_strdup(airportCode), tree);
  }

  // 2. Iterate through Flights using dataset iterator
  DatasetIterator *it = dataset_flight_iterator_new(ds);
  const Flight *flight;

  while ((flight = (const Flight *)dataset_iterator_next(it)) != NULL)
  {

    if (strcmp(getFlightStatus(flight), "Cancelled") == 0)
    {
      continue;
    }
    const gchar *airportCode = getFlightOrigin(flight);
    if (!airportCode)
    {
      continue;
    }

    FTree *tree = g_hash_table_lookup(airportTrees, airportCode);
    if (!tree)
    {
      continue;
    }

    time_t date = getFlightActualDeparture(flight);
    if (date < 0)
    {
      continue;
    }

    time_t date_trunc = date - (date % 86400);

    // Binary search
    int lower = 0, upper = tree->n - 1, idx = -1;
    while (lower <= upper)
    {
      int mid = (lower + upper) / 2;
      time_t dt = tree->dates[mid];
      if (dt == -1)
      {
        lower = mid + 1;
        continue;
      }
      gint cmp = compare_time_t(dt, date_trunc);
      if (cmp < 0)
      {
        lower = mid + 1;
      }
      else
      {
        idx = mid + 1;
        upper = mid - 1;
      }
    }
    if (idx > 0)
    {
      int pos = idx;
      while (pos <= tree->n)
      {
        tree->bit[pos] += 1;
        pos += (pos & -pos);
      }
    }
  }

  dataset_iterator_free(it);

  return airportTrees;
}

// Standard fenwick tree functions

int ftree_prefix_sum(FTree *t, int idx)
{
  int result = 0;
  while (idx > 0)
  {
    result += t->bit[idx];
    idx -= (idx & -idx);
  }
  return result;
}

int ftree_range_sum(FTree *t, int left_idx, int right_idx)
{
  if (right_idx < left_idx)
    return 0;
  return ftree_prefix_sum(t, right_idx) - ftree_prefix_sum(t, left_idx - 1);
}

// Fenwick tree getters

FTree *getFTree(gpointer data) { return (FTree *)data; }

int getFtreeN(FTree *tree) { return tree->n; }

time_t *getFtreeDates(FTree *tree) { return tree->dates; }

int *getFTreeBit(FTree *tree) { return tree->bit; }