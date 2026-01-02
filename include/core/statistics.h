/**
 * @file statistics.h
 * @brief Logic for calculating global aggregated statistics (e.g., Airport Traffic).
 *
 * This module is responsible for performing heavy data aggregation tasks during the
 * dataset loading phase. Specifically, it cross-references Reservations with Flights
 * to calculate the total number of passengers arriving and departing from each airport.
 *
 * @note The results generated here are cached in the Dataset to allow O(1) access
 * during Query 1 execution, avoiding expensive runtime iteration.
 */

#ifndef STATISTICS_H
#define STATISTICS_H

#include <glib.h>

/**
 * @brief Opaque structure holding passenger traffic statistics for a specific airport.
 *
 * Encapsulates the counters for:
 * - Total passengers arriving.
 * - Total passengers departing.
 */
typedef struct AiportPassengStats AirportPassengerStats;

/**
 * @brief Memory cleanup function for AirportPassengerStats.
 *
 * Compatible with @c GDestroyNotify. Frees the allocated memory for the statistics structure.
 *
 * @param data Pointer to the @c AirportPassengerStats to be freed.
 */
void freeAirportPassengerStats(gpointer data);

/**
 * @brief Generates the Airport Traffic statistics table.
 *
 * This function iterates through all reservations, links them to their corresponding flights,
 * and aggregates passenger counts for both origin and destination airports.
 *
 * @note This is a computationally intensive operation (O(R), where R is the number of reservations).
 * It handles logic such as ignoring cancelled flights or invalid reservations.
 *
 * @param reservations The raw Hash Table of reservations (from Dataset).
 * @param flights The raw Hash Table of flights (from Dataset).
 * @return A new @c GHashTable mapping Airport Codes (char*) -> @c AirportPassengerStats*.
 * Returns @c NULL if input tables are missing.
 */
GHashTable *calculate_airport_traffic(const GHashTable *reservations,
                                      const GHashTable *flights);

/**
 * @brief Accessor for the total number of arriving passengers.
 *
 * @param s Pointer to the statistics structure.
 * @return The number of arrivals. Returns 0 if @p s is NULL.
 */
long getAirportArrivals(const AirportPassengerStats *s);

/**
 * @brief Accessor for the total number of departing passengers.
 *
 * @param s Pointer to the statistics structure.
 * @return The number of departures. Returns 0 if @p s is NULL.
 */
long getAirportDepartures(const AirportPassengerStats *s);

#endif // STATISTICS_H