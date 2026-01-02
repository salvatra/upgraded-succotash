/**
 * @file query3.h
 * @brief Logic for Query 3: Temporal Airport Traffic Analysis.
 *
 * This module implements the logic to calculate flight statistics for all airports
 * within a specific date range. It leverages **Fenwick Trees (Binary Indexed Trees)**
 * to perform Range Sum Queries efficiently.
 */

#ifndef QUERY3_H
#define QUERY3_H

#include <glib.h>
#include <core/dataset.h>

/**
 * @brief Helper structure to hold an Airport Code and its aggregated flight count.
 *
 * Used primarily for sorting the results (by Count DESC, then Code ASC)
 * before generating the final output string.
 */
typedef struct dateCount DateCount;

/**
 * @brief Executes Query 3: Total Flights per Airport in a Date Range.
 *
 * This function calculates the total number of flights (movements) for each airport
 * between @p startStr and @p endStr (inclusive).
 *
 * **Algorithmic Strategy:**
 * Instead of iterating through millions of flights to check dates, it uses
 * pre-calculated Fenwick Trees stored in @p airportFtrees.
 * 1. Parses the start/end dates into internal indices.
 * 2. Performs a Range Sum Query on each airport's tree: $Sum(L, R) = PrefixSum(R) - PrefixSum(L-1)$.
 * 3. Time Complexity per Airport: $O(\log N)$, where N is the number of days.
 *
 * @param airportFtrees A Hash Table mapping Airport Codes (char*) to Fenwick Tree structures (FTree*).
 * Created via `build_query_context`.
 * @param ds The main Dataset (used for validating codes or accessing auxiliary data).
 * @param startStr The start date string in format "YYYY-MM-DD".
 * @param endStr The end date string in format "YYYY-MM-DD".
 *
 * @return A newly allocated string containing the formatted results (e.g., "OPO;150\nLIS;120\n").
 * @warning The caller is responsible for freeing this string using `g_free()`.
 * Returns NULL/Empty string if dates are invalid or no data is found.
 */
gchar *query3(GHashTable *airportFtrees, const Dataset *ds, const char *startStr, const char *endStr);

#endif // QUERY3_H