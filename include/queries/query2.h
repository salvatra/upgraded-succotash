/**
 * @file query2.h
 * @brief Logic for Query 2: Aircraft Flight Statistics.
 *
 * This module implements the logic to identify the top N aircraft with the highest
 * number of flights. It supports optional filtering by aircraft model.
 *
 * @section q2_algo Algorithm Overview
 * Unlike simple lookups, this query requires a global ranking of aircraft based on
 * flight frequency. To answer this efficiently (O(N log N) or better per query):
 * 1. **Pre-Calculation Phase (Init):** We iterate the entire dataset once to count
 * flights per aircraft, storing the results in a direct-mapped array (`flightCounts`).
 * 2. **Query Phase (Run):** We filter the aircraft list (if a model argument is provided),
 * create a temporary list of candidates, sort them by flight count, and return the top N.
 */

#ifndef QUERY2_H
#define QUERY2_H

#include <glib.h>
#include <core/dataset.h>
#include "queries/query_module.h"

// Forward declaration of the opaque stats structure used in the results
typedef struct aircraft_stats AircraftStats;

/**
 * @brief Retrieves the Aircraft ID from a stats object.
 * @param stats The stats object.
 * @return The unique Aircraft ID string.
 */
char *get_aircraftstats_id(AircraftStats *stats);

/**
 * @brief Retrieves the Manufacturer from a stats object.
 * @param stats The stats object.
 * @return The manufacturer name.
 */
char *get_aircraftstats_manufacturer(AircraftStats *stats);

/**
 * @brief Retrieves the Model from a stats object.
 * @param stats The stats object.
 * @return The model name.
 */
char *get_aircraftstats_model(AircraftStats *stats);

/**
 * @brief Retrieves the flight count from a stats object.
 * @param stats The stats object.
 * @return The total number of valid flights.
 */
int get_aircraftstats_count(AircraftStats *stats);

/**
 * @brief Frees an array of AircraftStats objects.
 * @param array The array of pointers to AircraftStats.
 * @param size The number of elements in the array.
 */
void free_aircraftstats_array(AircraftStats **array, int size);

/**
 * @brief Core logic for Query 2.
 *
 * Identifies the top N aircraft with the most flights.
 *
 * @param N            The number of top results to return.
 * @param aircrafts    A list of all valid Aircraft entities (pre-sorted or indexed).
 * @param filter_model Optional string to filter by Model (can be NULL).
 * @param size         [Output] Stores the number of results returned.
 * @param flightCounts An array of integers where index `i` corresponds to the flight
 * count of the aircraft at `aircrafts[i]`.
 *
 * @return An allocated array of `AircraftStats*` pointers. Returns NULL if no results found.
 */
AircraftStats **query2(int N, GPtrArray *aircrafts, const char *filter_model,
                       int *size, int *flightCounts);

/**
 * @brief Factory function to retrieve the Module definition for Query 2.
 * Used by the Query Manager to register this query.
 */
QueryModule get_query2_module(void);

#endif // QUERY2_H