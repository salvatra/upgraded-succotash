/**
 * @file query6.h
 * @brief Logic for Query 6: Passenger Nationality Statistics.
 *
 * This module allows for the analysis of the most frequent destination airport
 * for passengers of a specific nationality.
 *
 * @section q6_algo Algorithm Overview
 * 1. **Pre-Calculation (Init):** We iterate through all reservations. For each reservation,
 * we resolve the Passenger's nationality and the flight's destination.
 * 2. **Storage:** We build a nested Hash Table structure:
 * `Nationality -> { DestinationAirport -> Count }`
 * 3. **Query Phase (Run):** When a nationality is requested, we look up its airport histogram
 * and iterate to find the airport with the maximum count.
 */

#ifndef QUERY6_H
#define QUERY6_H

#include <core/dataset.h>
#include <glib.h>
#include <stdio.h>
#include "queries/query_module.h"


/**
 * @brief Initializes the data structure for Nationality analysis.
 *
 * @param ds The dataset.
 * @return A GHashTable mapping Nationality (string) -> internal NationalityData struct.
 */
GHashTable *prepareNationalityData(const Dataset *ds);

/**
 * @brief Executes Query 6.
 * Finds the most visited airport for the given nationality.
 *
 * @param natTable    The pre-calculated hash table.
 * @param nationality The nationality to query (e.g., "PORTUGAL").
 * @param output      The output file stream.
 * @param isSpecial   Flag for 'S' variant (separator formatting).
 * @return 1 if a result was found and printed, 0 otherwise.
 */
int query_Q6(GHashTable *natTable, const char *nationality, FILE *output, int isSpecial);

/**
 * @brief Factory function to retrieve the Module definition for Query 6.
 */
QueryModule get_query6_module(void);

#endif // QUERY6_H