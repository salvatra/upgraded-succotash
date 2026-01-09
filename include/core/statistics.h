/**
 * @file statistics.h
 * @brief Logic for calculating and managing airport traffic statistics.
 *
 * This header defines the interface for the statistics module, which is responsible
 * for aggregating passenger movements (arrivals and departures) across the entire
 * network. It defines the opaque `AirportPassengerStats` structure and functions
 * to compute these stats from the raw Reservations and Flights data.
 */

#ifndef STATISTICS_H
#define STATISTICS_H

#include <glib.h>

/**
 * @typedef AirportPassengerStats
 * @brief Opaque structure holding traffic statistics for a single airport.
 *
 * This structure accumulates the total number of passengers arriving at and
 * departing from a specific airport. The internal definition is hidden in
 * `src/core/statistics.c`.
 *
 * It is typically stored as a value in a GHashTable where the key is the airport code.
 */
// Use the same struct tag as defined in dataset.h to avoid conflicts
typedef struct airport_passenger_stats AirportPassengerStats;

/**
 * @brief Memory cleanup function for AirportPassengerStats.
 *
 * Compatible with `GDestroyNotify`. Frees the memory allocated for a single
 * `AirportPassengerStats` instance.
 *
 * @param data A pointer to the `AirportPassengerStats` structure to free.
 * If NULL, the function does nothing.
 */
void freeAirportPassengerStats(gpointer data);

/**
 * @brief Calculates traffic statistics for all airports based on reservations.
 *
 * Iterates through all reservations, resolves the associated flights, and updates
 * the arrival/departure counts for the corresponding origin and destination airports.
 *
 * @note This is a computationally intensive operation typically performed once during
 * the dataset loading phase.
 *
 * @param reservations The hash table containing all `Reservation` entities.
 * @param flights The hash table containing all `Flight` entities.
 * @return A new `GHashTable` where:
 * - **Key**: `gchar*` - The Airport Code (e.g., "LIS").
 * - **Value**: `AirportPassengerStats*` - The computed statistics.
 * The caller is responsible for destroying this table using `g_hash_table_destroy()`.
 */
GHashTable *calculate_airport_traffic(const GHashTable *reservations, const GHashTable *flights);

/**
 * @brief Getter for the total number of arriving passengers.
 *
 * @param s Pointer to the statistics structure.
 * @return The total count of passengers who arrived at this airport.
 * Returns 0 if `s` is NULL.
 */
long getAirportArrivals(const AirportPassengerStats *s);

/**
 * @brief Getter for the total number of departing passengers.
 *
 * @param s Pointer to the statistics structure.
 * @return The total count of passengers who departed from this airport.
 * Returns 0 if `s` is NULL.
 */
long getAirportDepartures(const AirportPassengerStats *s);

#endif