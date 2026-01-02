/**
 * @file query6.h
 * @brief Logic for Query 6: Passenger Traffic Analysis by Nationality.
 *
 * This module answers the question: "For a given nationality, which airports processed
 * the most passengers?".
 *
 * **Optimization Strategy:**
 * Since filtering passengers by nationality at runtime would require a full scan
 * of the passenger/flight dataset (slow), this module constructs a pre-calculated
 * **Inverted Index** during the loading phase. This maps each unique nationality
 * string directly to a list of airports and their respective counters.
 */

#ifndef QUERY6_H
#define QUERY6_H

#include <glib.h>
#include <stdio.h>
#include <core/dataset.h>

/**
 * @brief Constructs the Nationality Inverted Index (Context Builder).
 *
 * This function iterates through the entire dataset to build an aggregation table.
 * It cross-references Passengers with their Flight history to count movements.
 *
 * **Process:**
 * 1. Iterates over all flights/passengers.
 * 2. For each passenger, identifies their nationality.
 * 3. Updates the passenger count for the corresponding airports (Origin/Destination)
 * associated with that nationality key.
 *
 * @param ds The source Dataset containing Passengers and Flights.
 * @return A @c GHashTable where:
 * - Key: Nationality (string).
 * - Value: An opaque internal structure holding Airport counters (e.g., a map of AirportCode -> Count).
 * Returns @c NULL if the dataset is empty or allocation fails.
 */
GHashTable *prepareNationalityData(const Dataset *ds);

/**
 * @brief Executes Query 6: Top Airports by Nationality.
 *
 * Retrieves the statistics for the specific @p nationality from the pre-built index
 * and prints the top airports sorted by passenger count (descending).
 *
 * **Time Complexity:** O(A log A), where A is the number of airports visited by
 * nationals of that country (usually small), plus the O(1) hash lookup.
 *
 * @param natTable The pre-calculated Inverted Index (Context).
 * @param nationality The target nationality string (e.g., "Portuguese").
 * @param output The output stream to write results to.
 * @param isSpecial Formatting flag (Standard vs Custom/CSV).
 *
 * @return The number of lines/records printed.
 * Returns 0 if the nationality is not found in the index.
 */
int query_Q6(GHashTable *natTable, const char *nationality, FILE *output, int isSpecial);

#endif // QUERY6_H