/**
 * @file query5.h
 * @brief Logic for Query 5: Airline Punctuality Ranking (Average Delay).
 *
 * This module is responsible for ranking airlines based on their weighted flight delays.
 *
 * **Optimization Strategy:**
 * Calculating the average delay for every airline requires iterating over the entire
 * flights dataset. To avoid this cost during runtime execution, this module uses a
 * **Pre-Aggregation** approach. The statistics are calculated and sorted during the
 * dataset loading phase, making the actual query execution extremely fast (O(N)).
 */

#ifndef QUERY5_H
#define QUERY5_H

#include <glib.h>
#include <stdio.h>
#include "entities/flights.h"

/**
 * @brief Pre-calculates and sorts airline delay statistics.
 *
 * This function serves as the **Context Builder** for Query 5.
 * It iterates through the entire array of flights to:
 * 1. Group flights by Airline.
 * 2. Accumulate the total delay time (Scheduled Departure vs Actual Departure).
 * 3. Count the total flights per airline.
 * 4. Calculate the average delay.
 * 5. **Sort** the resulting list of airlines in descending order of average delay.
 *
 * @note This function is computationally expensive (O(F), where F is total flights)
 * and should only be called once during the application startup.
 *
 * @param flightsArray The master array containing all loaded flights.
 * @return A sorted @c GList* containing internal structures with airline statistics.
 * The caller is responsible for managing this list's lifecycle using @ref freeAirlineDelays.
 */
GList *prepareAirlineDelays(GPtrArray *flightsArray);

/**
 * @brief Executes Query 5: Retrieve Top N Airlines by Average Delay.
 *
 * Since the @p airlineDelays list is already sorted by the builder function,
 * this function simply iterates the first N elements of the list and prints them.
 *
 * **Time Complexity:** O(N)
 *
 * @param airlineDelays The pre-sorted list of airline statistics (Context).
 * @param N The number of top airlines to display.
 * @param output The output stream to write results to.
 * @param isSpecial Formatting flag (Standard vs Custom/CSV).
 *
 * @return The number of lines actually printed (may be less than N if the list is smaller).
 */
int query5(GList *airlineDelays, int N, FILE *output, int isSpecial);

/**
 * @brief Frees the memory associated with the airline delays list.
 *
 * Iterates through the @c GList, freeing the custom airline statistic structures
 * stored in each node, and finally frees the list container itself.
 *
 * @param airlineDelays The list to be destroyed.
 */
void freeAirlineDelays(GList *airlineDelays);

#endif // QUERY5_H