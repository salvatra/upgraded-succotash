#ifndef QUERY2_H
#define QUERY2_H

#include <entities/aircrafts.h>
#include <entities/flights.h>
#include <glib.h>

/**
 * @brief Opaque structure holding statistics for a specific aircraft (or
 * model/manufacturer).
 */
typedef struct aircraftstats AircraftStats;

/**
 * @brief Executes Query 2: Retrieve top N aircraft by flight count.
 *
 * @param N The number of top aircraft to retrieve.
 * @param aircrafts A pointer array of aircraft data.
 * @param manufacturerFilter Optional manufacturer name to filter by (can be
 * NULL).
 * @param outSize Pointer to an integer where the size of the returned array
 * will be stored.
 * @param precomputedCounts (Optional) Precomputed flight counts to speed up the
 * query.
 * @return An array of pointers to AircraftStats structures.
 */
AircraftStats **query2(int N, GPtrArray *aircrafts,
                       const char *manufacturerFilter, int *outSize,
                       int *precomputedCounts);

/**
 * @brief Frees an array of AircraftStats pointers.
 *
 * @param array The array to free.
 * @param size The number of elements in the array.
 */
void free_aircraftstats_array(AircraftStats **array, int size);

/**
 * @brief Frees a single AircraftStats structure.
 *
 * @param a Pointer to the AircraftStats structure.
 */
void free_aircraftstats(AircraftStats *a);

/**
 * @brief Gets the flight count from AircraftStats.
 *
 * @param a Pointer to the AircraftStats structure.
 * @return The number of flights.
 */
int get_aircraftstats_count(const AircraftStats *a);

/**
 * @brief Gets the aircraft ID (registration) from AircraftStats.
 *
 * @param a Pointer to the AircraftStats structure.
 * @return The aircraft ID string.
 */
const gchar *get_aircraftstats_id(const AircraftStats *a);

/**
 * @brief Gets the aircraft model from AircraftStats.
 *
 * @param a Pointer to the AircraftStats structure.
 * @return The aircraft model string.
 */
const gchar *get_aircraftstats_model(const AircraftStats *a);

/**
 * @brief Gets the aircraft manufacturer from AircraftStats.
 *
 * @param a Pointer to the AircraftStats structure.
 * @return The aircraft manufacturer string.
 */
const gchar *get_aircraftstats_manufacturer(const AircraftStats *a);

#endif
