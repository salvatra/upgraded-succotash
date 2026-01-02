/**
 * @file indexer.h
 * @brief Indexing module responsible for pre-processing and organizing temporal data.
 *
 * This module separates complex indexing logic from the main data storage.
 * It is primarily responsible for scanning the dataset to identify, collect, and sort
 * distinct operation dates for each airport. This pre-processing step is essential
 * for Coordinate Compression required by the Fenwick Trees (Query 3).
 */

#ifndef INDEXER_H
#define INDEXER_H

#include <glib.h>
#include <core/dataset.h>

/**
 * @brief Opaque structure holding temporal metadata for a specific airport.
 *
 * This structure encapsulates:
 * - A sorted array of distinct dates (time_t) on which flights occurred.
 * - A hash set for O(1) duplicate detection during the collection phase.
 *
 * It serves as an intermediate data carrier between the raw Dataset and the
 * Fenwick Tree construction.
 */
typedef struct datesinfo DatesInfo;

/**
 * @brief Generates the temporal index for all airports in the dataset.
 *
 * This function iterates through the entire Flights collection in the Dataset.
 * For each valid (non-cancelled) flight, it extracts the origin airport and the departure date.
 * It accumulates unique dates for each airport and sorts them chronologically.
 *
 * @note This is a resource-intensive operation (O(N log N) due to sorting) and should
 * be performed only once during the loading phase.
 *
 * @param ds The source dataset containing the flights to index.
 * @return A new @c GHashTable mapping Airport Codes (char*) -> @c DatesInfo*.
 * The caller is responsible for destroying this table.
 */
GHashTable *create_date_index(const Dataset *ds);

/**
 * @brief Accessor for the sorted array of distinct dates.
 *
 * @param di The DatesInfo structure.
 * @return A pointer to the internal @c GArray containing sorted @c time_t values.
 * Returns @c NULL if @p di is NULL.
 */
const GArray *getDiDates(const DatesInfo *di);

/**
 * @brief Accessor for the hash set of dates.
 *
 * Used primarily during the construction phase to check for existence of dates
 * without iterating the array.
 *
 * @param di The DatesInfo structure.
 * @return A pointer to the internal @c GHashTable used as a Set.
 * Returns @c NULL if @p di is NULL.
 */
const GHashTable *getDiSet(const DatesInfo *di);

#endif // INDEXER_H