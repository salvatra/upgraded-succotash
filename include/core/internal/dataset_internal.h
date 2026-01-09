/**
 * @file dataset_internal.h
 * @brief Internal definition of the central Dataset structure.
 *
 * This header defines the layout of the `struct dataset`, which acts as the
 * central in-memory database for the application. It aggregates all entity
 * hash tables and auxiliary indexing structures.
 *
 * @warning **Internal Header**: This file should ONLY be included by:
 * - `src/core/dataset.c`: For memory management and public accessor implementation.
 * - `src/io/manager.c`: For direct population of the tables during the loading phase.
 *
 * It must NOT be included by queries, the main program, or other consumer modules,
 * which should instead rely on the opaque `Dataset` typedef and public API functions
 * defined in `include/core/dataset.h`.
 */

#ifndef DATASET_INTERNAL_H
#define DATASET_INTERNAL_H

#include <glib.h>

/**
 * @struct dataset
 * @brief The main data container structure.
 *
 * Holds pointers to the hash tables containing the parsed entity data, as well
 * as auxiliary data structures used for statistics, autocomplete, and validation.
 * The memory for this structure is managed by `initDataset()` and `cleanupDataset()`
 * in `src/core/dataset.c`.
 */
struct dataset
{
  /**
   * @brief Hash Table of Flights.
   *
   * Stores all parsed flight records.
   * - **Key**: `gchar*` - The Flight ID (e.g., "0000000001").
   * - **Value**: `Flight*` - Pointer to the Flight entity structure.
   */
  GHashTable *flights;

  /**
   * @brief Hash Table of Passengers.
   *
   * Stores all parsed passenger records.
   * - **Key**: `gpointer` (casted `int`) - The Passenger ID / Document Number.
   * - **Value**: `Passenger*` - Pointer to the Passenger entity structure.
   */
  GHashTable *passengers;

  /**
   * @brief Hash Table of Airports.
   *
   * Stores all parsed airport records.
   * - **Key**: `gchar*` - The IATA Airport Code (e.g., "LIS").
   * - **Value**: `Airport*` - Pointer to the Airport entity structure.
   */
  GHashTable *airports;

  /**
   * @brief Hash Table of Aircrafts.
   *
   * Stores all parsed aircraft records.
   * - **Key**: `gchar*` - The Aircraft ID (e.g., "A380-800").
   * - **Value**: `Aircraft*` - Pointer to the Aircraft entity structure.
   */
  GHashTable *aircrafts;

  /**
   * @brief Hash Table of Reservations.
   *
   * Stores all parsed reservation records.
   * - **Key**: `gchar*` - The Reservation ID (e.g., "Book0000000001").
   * - **Value**: `Reservation*` - Pointer to the Reservation entity structure.
   */
  GHashTable *reservations;

  // Statistics & Aux Structures

  /**
   * @brief Hash Table for Airport Traffic Statistics.
   *
   * Stores precomputed passenger statistics (arrivals/departures) per airport.
   * This is calculated after the initial loading of entities.
   * - **Key**: `gchar*` - The Airport Code.
   * - **Value**: `AirportPassengerStats*` - Struct containing arrival/departure counts.
   */
  GHashTable *airportStats;

  /**
   * @brief Sorted array of unique Airport Codes.
   *
   * Contains `gchar*` strings. Used primarily for autocomplete functionality
   * in the interactive mode and for validating airport codes efficiently.
   */
  GPtrArray *airportCodes;

  /**
   * @brief Sorted array of unique Aircraft Manufacturers.
   *
   * Contains `gchar*` strings. Used for autocomplete and filtering queries
   * (e.g., Query 2).
   */
  GPtrArray *aircraftManufacturers;

  /**
   * @brief Sorted array of unique Nationalities.
   *
   * Contains `gchar*` strings. Used for autocomplete functionality in the
   * interactive mode (e.g., Query 6).
   */
  GPtrArray *nationalities;
};

#endif