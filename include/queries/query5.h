/**
 * @file query5.h
 * @brief Logic for Query 5: Airline Delays Statistics.
 *
 * This module identifies the top N airlines with the highest average flight delays.
 *
 * @section q5_algo Algorithm Overview
 * 1. **Pre-Calculation (Init):** The module iterates over all flights to compute the
 * accumulated delay for each airline. It filters only "Delayed" flights.
 * 2. **Metric:** The metric used is the average delay (Total Delay / Count), rounded to
 * 3 decimal places.
 * 3. **Query Phase (Run):** The pre-calculated list is sorted by this average delay
 * (descending) to find the top N airlines.
 */

#ifndef QUERY5_H
#define QUERY5_H

#include <core/dataset.h>
#include <glib.h>
#include <stdio.h>
#include "queries/query_module.h"

// --- Legacy API (Preserved) ---

/**
 * @brief Computes aggregated delay statistics for all airlines.
 *
 * @param flightsArray A GPtrArray containing Flight* pointers.
 * @return A GList* of internal AirlineDelayPrepared structures.
 */
GList *prepareAirlineDelays(GPtrArray *flightsArray);

/**
 * @brief Executes Query 5.
 * Sorts the delay list and prints the top N airlines.
 *
 * @param airlineDelays The pre-calculated list of airline stats.
 * @param N             The number of airlines to show.
 * @param output        The output file stream.
 * @param isSpecial     Flag for 'S' variant (separator formatting).
 * @return The number of lines printed.
 */
int query5(GList *airlineDelays, int N, FILE *output, int isSpecial);

/**
 * @brief Frees the airline delay list and its contents.
 */
void freeAirlineDelays(GList *airlineDelays);

/**
 * @brief Factory function to retrieve the Module definition for Query 5.
 */
QueryModule get_query5_module(void);

#endif // QUERY5_H