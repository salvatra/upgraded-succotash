#ifndef QUERY1_H
#define QUERY1_H

#include <glib.h>
#include <core/dataset.h>

/**
 * @file query1.h
 * @brief Logic for Query 1: Airport Summary Statistics.
 *
 * This module implements the logic to retrieve a detailed profile for a specific airport.
 * It combines static entity data (Name, Location) with dynamic aggregated statistics
 * (Total Movements, Total Passengers) calculated during the dataset loading phase.
 */

/**
 * @brief Executes Query 1: Retrieval of Airport Summary.
 *
 * This function performs a lookup for the provided Airport IATA code and generates
 * a formatted output string containing:
 * - Airport Name
 * - Municipality and Country
 * - Total number of Flights (Departures + Arrivals)
 * - Total number of Passengers
 *
 * @note **Performance:** This function executes in **O(1)** time complexity.
 * It relies on Hash Table lookups for the Airport entity and the pre-calculated
 * statistics stored in the Dataset.
 *
 * @param code The IATA code of the airport to query (e.g., "LIS").
 * @param ds The constant Dataset instance containing the loaded data.
 *
 * @return A newly allocated string (`gchar*`) containing the formatted result.
 * @warning The caller is responsible for freeing this memory using `g_free()`.
 * Returns @c NULL if the airport code does not exist in the dataset.
 */
gchar *query1(const gchar *code, const Dataset *ds);

#endif // QUERY1_H