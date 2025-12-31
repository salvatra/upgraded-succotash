#include "core/fenwick.h"
#include "core/time_utils.h"
#include "entities/flights.h"
#include <core/dataset.h>
#include <glib.h>
#include <stdio.h>
#include <time.h>

//
// AN IMPLEMENTATION OF FENWICK TREES / BIT FOR THIS USE CASE
// https://cp-algorithms.com/data_structures/fenwick.html
//

struct ftree {
  int n;         // number of distinct dates
  time_t *dates; // ordered array of all departures
  int *bit;      // array to implemet Fenwick Tree logic
};

void freeFTree(gpointer data) {
  FTree *tree = (FTree *)data;
  if (tree) {
    g_free(tree->dates);
    g_free(tree->bit);
    g_free(tree);
  }
}

GHashTable *getFTrees(GHashTable *airportDepartures, GHashTable *flights) {
  // 1. Create new hashtable to hold each airport's date information Fenwick
  // Tree (FTree)
  GHashTable *airportTrees = g_hash_table_new_full(
      g_str_hash, g_str_equal, g_free, (GDestroyNotify)freeFTree);

  // 2. Iterate over date info of each airport
  GHashTableIter diter;
  gpointer dkey, dval;
  g_hash_table_iter_init(&diter, airportDepartures);
  while (g_hash_table_iter_next(&diter, &dkey, &dval)) {
    // FIXME: please, I don't want to deep copy a hash table to 'encapsulate'
    // DatesInfo...

    // 3. Get the aiport's code and information about its dates
    gchar *airportCode = (gchar *)dkey;
    DatesInfo *di = (DatesInfo *)dval;

    int nDates = getDiDates(di)->len;

    // 4. Create and populate each airport's FTree
    FTree *tree = g_new0(FTree, 1);
    tree->n = nDates;
    // The max size of dates[] is known but not at compile time;
    // it's also more efficient and easier to implement than a dynamic array
    tree->dates = g_new0(time_t, nDates);
    for (int i = 0; i < nDates; i++) {
      time_t dt = g_array_index(getDiDates(di), time_t, i);
      tree->dates[i] = dt;
    }
    // Intialize the bit array with 0s
    tree->bit = g_new0(int, nDates + 1); // index 1..n
    for (int i = 1; i <= nDates; i++) {
      tree->bit[i] = 0;
    }

    g_hash_table_insert(airportTrees, g_strdup(airportCode), tree);
  }

  // 5. Iterate over all flights to populate the bit array
  GHashTableIter fIter;
  gpointer fkey;
  g_hash_table_iter_init(&fIter, flights);
  while (g_hash_table_iter_next(&fIter, &fkey, NULL)) {
    Flight *flight = getFlight(fkey, flights);

    if (strcmp(getFlightStatus(flight), "Cancelled") == 0) {
      freeFlight(flight);
      continue;
    }
    const gchar *airportCode = getFlightOrigin(flight);
    if (!airportCode) {
      freeFlight(flight);
      continue;
    }

    FTree *tree = g_hash_table_lookup(airportTrees, airportCode);
    if (!tree) {
      freeFlight(flight);
      // free(airportCode);
      continue;
    }

    time_t date = getFlightActualDeparture(flight);
    if (date < 0) {
      freeFlight(flight);
      // free(airportCode);
      // g_free(tree);
      continue;
    }

    freeFlight(flight);

    time_t date_trunc = date - (date % 86400);

    // binary search for the min index i such tree->dates[i] >= date
    int lower = 0, upper = tree->n - 1, idx = -1;
    while (lower <= upper) {
      int mid = (lower + upper) / 2;
      time_t dt = tree->dates[mid];
      if (dt == -1) {
        lower = mid + 1;
        continue;
      }
      gint cmp = compare_time_t(dt, date_trunc);
      if (cmp < 0) {
        lower = mid + 1;
      } else {
        idx = mid + 1;
        upper = mid - 1;
      }
    }
    // standard fenwick tree bit population
    if (idx > 0) {
      int pos = idx;
      while (pos <= tree->n) {
        tree->bit[pos] += 1;
        pos += (pos & -pos);
      }
    }
  }

  // g_hash_table_destroy(airportDepartures);

  return airportTrees;
}

// Standard fenwick tree functions

int ftree_prefix_sum(FTree *t, int idx) {
  int result = 0;
  while (idx > 0) {
    result += t->bit[idx];
    idx -= (idx & -idx);
  }
  return result;
}

int ftree_range_sum(FTree *t, int left_idx, int right_idx) {
  if (right_idx < left_idx)
    return 0;
  return ftree_prefix_sum(t, right_idx) - ftree_prefix_sum(t, left_idx - 1);
}

// Fenwick tree getters

FTree *getFTree(gpointer data) { return (FTree *)data; }

int getFtreeN(FTree *tree) { return tree->n; }

time_t *getFtreeDates(FTree *tree) { return tree->dates; }

int *getFTreeBit(FTree *tree) { return tree->bit; }
