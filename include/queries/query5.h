#ifndef QUERY5_H
#define QUERY5_H

#include <glib.h>
#include <stdio.h>
#include "entities/flights.h"

/**
 * @brief Executes Query 5: Top N airlines with the highest average delay.
 *
 * @param airlineDelays List of precomputed airline delays.
 * @param N Number of airlines to list.
 * @param output Output stream.
 * @param isSpecial Formatting flag.
 * @return Number of lines printed.
 */
int query5(GList *airlineDelays, int N, FILE *output, int isSpecial);

/**
 * @brief Prepares the airline delay data for Query 5.
 *
 * @param flightsArray Array of all flights.
 * @return A list of prepared airline delay structures.
 */
GList* prepareAirlineDelays(GPtrArray *flightsArray);

/**
 * @brief Frees the airline delay list.
 *
 * @param airlineDelays The list to free.
 */
void freeAirlineDelays(GList *airlineDelays);

#endif