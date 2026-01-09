/**
 * @file dataset_loader.h
 * @brief Loader API for populating the central Dataset.
 * @date 2026
 *
 * This header exposes the "Write/Set" interface for the Dataset, allowing external
 * modules (specifically the IO Manager) to inject populated data structures.
 *
 * @warning **Restricted API**: This header exposes GLib types (`GHashTable`, `GPtrArray`)
 * which are otherwise hidden from the public `dataset.h` API. It should **ONLY** be
 * included by `src/io/manager.c` or unit tests responsible for data loading.
 *
 * @note **Ownership Transfer**: All functions in this file assume that the caller
 * is handing over ownership of the memory (Hash Tables, Arrays) to the `Dataset`.
 * The `Dataset` becomes responsible for freeing these structures via `cleanupDataset()`.
 */

#ifndef DATASET_LOADER_H
#define DATASET_LOADER_H

#include <glib.h>
#include "dataset.h"


/**
 * @brief Injects the Flights hash table into the Dataset.
 *
 * @param ds The dataset instance.
 * @param flights A `GHashTable*` containing `Flight*` entities.
 * - Key: Flight ID string.
 * - Value: `Flight*` struct.
 * - Ownership: Transferred to `ds`.
 */
void dataset_set_flights(Dataset *ds, GHashTable *flights);

/**
 * @brief Injects the Passengers hash table into the Dataset.
 *
 * @param ds The dataset instance.
 * @param passengers A `GHashTable*` containing `Passenger*` entities.
 * - Key: Passenger ID (int cast to pointer).
 * - Value: `Passenger*` struct.
 * - Ownership: Transferred to `ds`.
 */
void dataset_set_passengers(Dataset *ds, GHashTable *passengers);

/**
 * @brief Injects the Airports hash table into the Dataset.
 *
 * @param ds The dataset instance.
 * @param airports A `GHashTable*` containing `Airport*` entities.
 * - Key: Airport Code string (IATA).
 * - Value: `Airport*` struct.
 * - Ownership: Transferred to `ds`.
 */
void dataset_set_airports(Dataset *ds, GHashTable *airports);

/**
 * @brief Injects the Aircrafts hash table into the Dataset.
 *
 * @param ds The dataset instance.
 * @param aircrafts A `GHashTable*` containing `Aircraft*` entities.
 * - Key: Aircraft ID string.
 * - Value: `Aircraft*` struct.
 * - Ownership: Transferred to `ds`.
 */
void dataset_set_aircrafts(Dataset *ds, GHashTable *aircrafts);

/**
 * @brief Injects the Reservations hash table into the Dataset.
 *
 * @param ds The dataset instance.
 * @param reservations A `GHashTable*` containing `Reservation*` entities.
 * - Key: Reservation ID string.
 * - Value: `Reservation*` struct.
 * - Ownership: Transferred to `ds`.
 */
void dataset_set_reservations(Dataset *ds, GHashTable *reservations);

/**
 * @brief Injects pre-calculated airport statistics into the Dataset.
 *
 * These statistics (arrivals/departures counts) are typically computed
 * by the loader after all entities have been parsed.
 *
 * @param ds The dataset instance.
 * @param stats A `GHashTable*` mapping airport codes to `AirportPassengerStats*`.
 * - Ownership: Transferred to `ds`.
 */
void dataset_set_airport_stats(Dataset *ds, GHashTable *stats);

/**
 * @brief Injects the sorted list of unique Airport Codes.
 *
 * Used for validation and autocomplete features.
 *
 * @param ds The dataset instance.
 * @param codes A `GPtrArray*` of strings (airport codes).
 * - Ownership: Transferred to `ds`.
 */
void dataset_set_airport_codes(Dataset *ds, GPtrArray *codes);

/**
 * @brief Injects the sorted list of unique Aircraft Manufacturers.
 *
 * Used for filtering queries (e.g., Query 2) and autocomplete.
 *
 * @param ds The dataset instance.
 * @param manufacturers A `GPtrArray*` of strings.
 * - Ownership: Transferred to `ds`.
 */
void dataset_set_aircraft_manufacturers(Dataset *ds, GPtrArray *manufacturers);

/**
 * @brief Injects the sorted list of unique Nationalities.
 *
 * Used for autocomplete in interactive mode.
 *
 * @param ds The dataset instance.
 * @param nationalities A `GPtrArray*` of strings.
 * - Ownership: Transferred to `ds`.
 */
void dataset_set_nationalities(Dataset *ds, GPtrArray *nationalities);

#endif // DATASET_LOADER_H