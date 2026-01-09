/**
 * @file query2.h
 * @brief Logic for Query 2: Global Flight Ranking by Aircraft.
 *
 * This module implements the logic to identify and rank aircraft based on their
 * total flight volume. It supports filtering by manufacturer and utilizes
 * pre-computed indices to deliver high-performance results (sorting logic).
 */

#ifndef QUERY2_H
#define QUERY2_H

#include <entities/access/aircrafts_access.h>
#include <entities/access/flights_access.h>
#include <glib.h>

/**
 * @brief Opaque structure holding aggregated statistics for a specific Aircraft.
 *
 * Encapsulates the pairing of an Aircraft entity with its calculated flight count.
 * Used as the return element for the query result list.
 * Internal layout is hidden from the public API.
 */
typedef struct aircraftstats AircraftStats;

/**
 * @brief Executes Query 2: Retrieve Top N Aircraft by Flight Count.
 *
 * This function ranks aircrafts descendingly by the number of flights they have performed.
 * It allows for optional filtering by manufacturer.
 *
 * @note **Optimization:** If @p precomputedCounts is provided, the function skips the
 * expensive iteration over the Flights table and uses the cached counters, significantly
 * reducing execution time.
 *
 * @param N The maximum number of results to return (The "N" in "Top N").
 * @param aircrafts A pointer array (`GPtrArray*`) containing all available Aircraft entities.
 * @param manufacturerFilter An optional string to filter results.
 * If provided (not NULL), only aircrafts from this manufacturer are considered.
 * If NULL, all aircrafts are considered.
 * @param outSize [Output] Pointer to an integer where the actual number of elements
 * returned in the array will be stored (may be less than N).
 * @param precomputedCounts (Optional) An array of integers representing flight counts,
 * indexed strictly to match the order of the @p aircrafts array.
 *
 * @return A dynamically allocated array of pointers to @c AircraftStats structures,
 * sorted by flight count (descending).
 * @warning The caller owns this array and is responsible for freeing it using
 * @ref free_aircraftstats_array.
 */
AircraftStats **query2(int N, GPtrArray *aircrafts,
                       const char *manufacturerFilter, int *outSize,
                       int *precomputedCounts);

/**
 * @brief Frees the array of results returned by Query 2.
 *
 * Iterates through the array, freeing each @c AircraftStats element, and finally
 * frees the array pointer itself.
 *
 * @param array The array of pointers to @c AircraftStats.
 * @param size The number of elements in the array (value of @p outSize from query2).
 */
void free_aircraftstats_array(AircraftStats **array, int size);

/**
 * @brief Frees a single AircraftStats structure.
 *
 * @param a Pointer to the AircraftStats structure to be freed.
 */
void free_aircraftstats(AircraftStats *a);

/**
 * @brief Accessor for the calculated Flight Count.
 *
 * @param a Pointer to the AircraftStats structure.
 * @return The total number of flights performed by this aircraft.
 */
int get_aircraftstats_count(const AircraftStats *a);

/**
 * @brief Accessor for the Aircraft ID (Registration).
 *
 * @param a Pointer to the AircraftStats structure.
 * @return A constant string representing the aircraft ID.
 */
const gchar *get_aircraftstats_id(const AircraftStats *a);

/**
 * @brief Accessor for the Aircraft Model.
 *
 * @param a Pointer to the AircraftStats structure.
 * @return A constant string representing the model name.
 */
const gchar *get_aircraftstats_model(const AircraftStats *a);

/**
 * @brief Accessor for the Aircraft Manufacturer.
 *
 * @param a Pointer to the AircraftStats structure.
 * @return A constant string representing the manufacturer name.
 */
const gchar *get_aircraftstats_manufacturer(const AircraftStats *a);

#endif // QUERY2_H