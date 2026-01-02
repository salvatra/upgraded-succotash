/**
 * @file indexer.h
 * @brief Logic for creating auxiliary indices and optimized structures.
 *
 * Separates complex indexing logic (like date sorting for Query 3)
 * from the main data storage (Dataset).
 */

#ifndef INDEXER_H
#define INDEXER_H

#include <glib.h>
#include <core/dataset.h>

/**
 * @brief Opaque structure for Date indexing (Query 3).
 */
typedef struct datesinfo DatesInfo;

/**
 * @brief Generates or retrieves the optimized Dates Table for Query 3.
 *
 * Creates a mapping of Airport Code -> Sorted List of Departure Dates.
 *
 * @param ds The dataset containing flights.
 * @return A GHashTable mapping Airport Code (char*) -> DatesInfo*.
 */
GHashTable *create_date_index(const Dataset *ds);

/**
 * @brief Helper to access dates array from the opaque DatesInfo structure.
 */
const GArray *getDiDates(const DatesInfo *di);

/**
 * @brief Helper to access the dates set from the opaque DatesInfo structure.
 */
const GHashTable *getDiSet(const DatesInfo *di);

#endif // INDEXER_H