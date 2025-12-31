#ifndef QUERY6_H
#define QUERY6_H

#include <glib.h>
#include "entities/passengers.h"
#include "entities/flights.h"
#include "entities/reservations.h"
#include <stdio.h>
#include "core/dataset.h"

/**
 * @brief Executes Query 6: Airport with most passengers for a given nationality.
 *
 * @param natTable Precomputed hash table of nationality data.
 * @param nationality The nationality to query.
 * @param output Output stream.
 * @param isSpecial Formatting flag.
 * @return 1 if found and printed, 0 otherwise.
 */
int query_Q6(GHashTable *natTable, const char *nationality, FILE *output, int isSpecial);

/**
 * @brief Prepares the nationality data for Query 6.
 *
 * @param passengers Hash table of passengers.
 * @param reservations Hash table of reservations.
 * @param flights Hash table of flights.
 * @return A hash table mapping nationality strings to NationalityData structures.
 */
GHashTable* prepareNationalityData(const GHashTable *passengers,
                                   const GHashTable *reservations,
                                   const GHashTable *flights);


#endif