/**
 * @file query3.h
 * @brief Logic for Query 3: Airport Delays Optimization.
 *
 * This module allows for the efficient retrieval of the airport with the highest
 * delay rating within a specific time range.
 *
 * @section q3_algo Algorithm Overview
 * This query typically faces "Range Sum" problems which are slow (O(N)) if iterating
 * over raw flights every time. To optimize this to O(log N):
 *
 * 1. **Pre-Calculation (Init):** We map every airport to a **Fenwick Tree (Binary Indexed Tree)**.
 * The tree indexes time (dates), allowing us to calculate the cumulative delay rating
 * between any two dates in logarithmic time.
 * 2. **Query Phase (Run):** For a given date range [A, B], we query the Fenwick Tree
 * of every airport to find the range sum. We compare these sums to find the maximum.
 */

#ifndef QUERY3_H
#define QUERY3_H

#include <glib.h>
#include <core/dataset.h>
#include "queries/query_module.h"

/**
 * @brief Core logic for Query 3.
 *
 * Iterates through the pre-computed Fenwick Trees to find the airport with the
 * highest delay rating in the specified date range.
 *
 * @param airportFtrees A Hash Table mapping Airport Code (string) -> Fenwick Tree (FTree*).
 * This structure must be built during the module initialization.
 * @param ds            The dataset (used to retrieve Airport names for the final output).
 * @param startStr      The start date string (e.g., "2023/01/01").
 * @param endStr        The end date string (e.g., "2023/01/31").
 *
 * @return A newly allocated string (`gchar*`) formatted as "AirportName;Rating".
 * Returns NULL if no airport matches the criteria or if input is invalid.
 */
gchar *query3(GHashTable *airportFtrees, const Dataset *ds,
              const char *startStr, const char *endStr);

/**
 * @brief Factory function to retrieve the Module definition for Query 3.
 * Used by the Query Manager to register this query.
 */
QueryModule get_query3_module(void);

#endif // QUERY3_H