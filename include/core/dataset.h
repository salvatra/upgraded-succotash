/**
 * @file dataset.h
 * @brief Public API for the central Dataset module.
 *
 * This header defines the opaque interface for the Dataset, which acts as the
 * main in-memory database for the application. It provides functions to:
 * - Initialize and destroy the dataset.
 * - Retrieve read-only references to specific entities (Flights, Passengers, etc.).
 * - Iterate over collections of entities.
 * - Access auxiliary data for statistics and autocomplete.
 *
 * @note This is the **Public Interface**. The internal structure definition
 * is hidden in `dataset_internal.h` to enforce encapsulation. Consumers of this
 * API (like queries and the main loop) work solely with the opaque `Dataset*` handle.
 */

#ifndef DATASET_H
#define DATASET_H

#include <glib.h>

// Forward declarations for Entity types to avoid circular includes in headers
typedef struct flight Flight;
typedef struct passenger Passenger;
typedef struct airport Airport;
typedef struct aircraft Aircraft;
typedef struct reservation Reservation;
typedef struct airport_passenger_stats AirportPassengerStats;

/**
 * @typedef Dataset
 * @brief Opaque handle for the Dataset.
 *
 * Represents the main data container. The internal layout is hidden from
 * the public API. Use `initDataset()` to create an instance and
 * `cleanupDataset()` to destroy it.
 */
typedef struct dataset Dataset;

/**
 * @typedef DatasetIterator
 * @brief Opaque handle for Iterators.
 *
 * Represents an iterator object used to traverse collections (hash tables)
 * within the dataset without exposing the underlying GLib implementation.
 */
typedef struct dataset_iter DatasetIterator;

// --- Life Cycle ---

/**
 * @brief Initializes a new Dataset instance.
 *
 * Allocates memory for the `Dataset` structure and initializes all internal
 * hash tables and auxiliary arrays to NULL or empty states.
 *
 * @return A pointer to the newly allocated `Dataset`.
 */
Dataset *initDataset(void);

/**
 * @brief Frees all memory associated with the Dataset.
 *
 * Destroys all internal hash tables (Flights, Passengers, etc.), frees the
 * associated entity structures using their specific destructors, and finally
 * frees the Dataset struct itself.
 *
 * @param ds Pointer to the Dataset to clean up. If NULL, does nothing.
 */
void cleanupDataset(Dataset *ds);

// --- Counters ---

/**
 * @brief Gets the total number of flights in the dataset.
 * @param ds The dataset instance.
 * @return The count of flights, or 0 if the dataset is NULL/empty.
 */
int dataset_get_flight_count(const Dataset *ds);

/**
 * @brief Gets the total number of aircrafts in the dataset.
 * @param ds The dataset instance.
 * @return The count of aircrafts, or 0 if the dataset is NULL/empty.
 */
int dataset_get_aircraft_count(const Dataset *ds);

/**
 * @brief Gets the total number of passengers in the dataset.
 * @param ds The dataset instance.
 * @return The count of passengers, or 0 if the dataset is NULL/empty.
 */
int dataset_get_passenger_count(const Dataset *ds);

/**
 * @brief Gets the total number of reservations in the dataset.
 * @param ds The dataset instance.
 * @return The count of reservations, or 0 if the dataset is NULL/empty.
 */
int dataset_get_reservation_count(const Dataset *ds);

// --- Accessors ---

/**
 * @brief Retrieves a read-only reference to a Flight by its ID.
 * @param ds The dataset instance.
 * @param id The Flight ID string (e.g., "0000000001").
 * @return A const pointer to the Flight entity, or NULL if not found.
 */
const Flight *dataset_get_flight(const Dataset *ds, const char *id);

/**
 * @brief Retrieves a read-only reference to a Passenger by their ID.
 * @param ds The dataset instance.
 * @param id The Passenger ID (document number) as an integer.
 * @return A const pointer to the Passenger entity, or NULL if not found.
 */
const Passenger *dataset_get_passenger(const Dataset *ds, int id);

/**
 * @brief Retrieves a read-only reference to an Airport by its code.
 * @param ds The dataset instance.
 * @param code The 3-letter IATA airport code (e.g., "LIS").
 * @return A const pointer to the Airport entity, or NULL if not found.
 */
const Airport *dataset_get_airport(const Dataset *ds, const char *code);

/**
 * @brief Retrieves a read-only reference to an Aircraft by its ID.
 * @param ds The dataset instance.
 * @param id The Aircraft ID string.
 * @return A const pointer to the Aircraft entity, or NULL if not found.
 */
const Aircraft *dataset_get_aircraft(const Dataset *ds, const char *id);

/**
 * @brief Retrieves a read-only reference to a Reservation by its ID.
 * @param ds The dataset instance.
 * @param id The Reservation ID string (e.g., "Book0000000001").
 * @return A const pointer to the Reservation entity, or NULL if not found.
 */
const Reservation *dataset_get_reservation(const Dataset *ds, const char *id);

/**
 * @brief Retrieves precomputed passenger statistics for a specific airport.
 * @param ds The dataset instance.
 * @param code The 3-letter IATA airport code.
 * @return A const pointer to the stats structure, or NULL if not found.
 */
const AirportPassengerStats *dataset_get_airport_stats(const Dataset *ds, const char *code);

// --- Aux Accessors ---

/**
 * @brief Retrieves the list of all unique Airport Codes.
 * Used for autocomplete and validation.
 * @param ds The dataset instance.
 * @return A pointer to the internal GPtrArray of strings. Do NOT free this array.
 */
GPtrArray *get_dataset_airport_codes(Dataset *ds);

/**
 * @brief Retrieves the list of all unique Aircraft Manufacturers.
 * Used for autocomplete and filtering in Query 2.
 * @param ds The dataset instance.
 * @return A pointer to the internal GPtrArray of strings. Do NOT free this array.
 */
GPtrArray *get_dataset_aircraft_manufacturers(Dataset *ds);

/**
 * @brief Retrieves the list of all unique Passenger Nationalities.
 * Used for autocomplete in Query 6.
 * @param ds The dataset instance.
 * @return A pointer to the internal GPtrArray of strings. Do NOT free this array.
 */
GPtrArray *get_dataset_nationalities(Dataset *ds);

// --- Iterators ---

/**
 * @brief Creates a new iterator for traversing the Flights collection.
 * @param ds The dataset instance.
 * @return A pointer to a new `DatasetIterator`. Must be freed with `dataset_iterator_free()`.
 */
DatasetIterator *dataset_flight_iterator_new(const Dataset *ds);

/**
 * @brief Creates a new iterator for traversing the Aircrafts collection.
 * @param ds The dataset instance.
 * @return A pointer to a new `DatasetIterator`. Must be freed with `dataset_iterator_free()`.
 */
DatasetIterator *dataset_aircraft_iterator_new(const Dataset *ds);

/**
 * @brief Creates a new iterator for traversing the Reservations collection.
 * @param ds The dataset instance.
 * @return A pointer to a new `DatasetIterator`. Must be freed with `dataset_iterator_free()`.
 */
DatasetIterator *dataset_reservation_iterator_new(const Dataset *ds);

/**
 * @brief Creates a new iterator for traversing the Passengers collection.
 * @param ds The dataset instance.
 * @return A pointer to a new `DatasetIterator`. Must be freed with `dataset_iterator_free()`.
 */
DatasetIterator *dataset_passenger_iterator_new(const Dataset *ds);

/**
 * @brief Advances the iterator and retrieves the next entity.
 *
 * @param it The iterator instance.
 * @return A const void pointer to the next entity (cast to specific type required),
 * or NULL if the iteration is complete.
 */
const void *dataset_iterator_next(DatasetIterator *it);

/**
 * @brief Frees the memory allocated for a DatasetIterator.
 * @param it The iterator to free.
 */
void dataset_iterator_free(DatasetIterator *it);

#endif // DATASET_H