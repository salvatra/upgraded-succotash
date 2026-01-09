/**
 * @file dataset.h
 * @brief Public API for the central Dataset module.
 * @date 2026
 *
 * This header defines the opaque interface for the Dataset, which acts as the
 * main in-memory database for the application. It aggregates all entity collections
 * (Flights, Passengers, Airports, Aircrafts, Reservations) and auxiliary indices
 * required for statistics and search operations.
 *
 * @note **Design Pattern**: This module implements the Opaque Pointer (Pimpl) idiom.
 * The internal structure definition of `Dataset` is hidden in the source file to
 * enforce encapsulation. Consumers interact with the data solely through the
 * handle `Dataset*` and the provided accessor/iterator functions.
 */

#ifndef DATASET_H
#define DATASET_H

#include <glib.h>

// --- Forward Declarations ---

/*
 * Forward declarations for Entity types.
 * These pointers are returned by accessors, but their internal layout is
 * defined in their respective entity headers/source files.
 */
typedef struct flight Flight;                                 /**< Represents a single flight record. */
typedef struct passenger Passenger;                           /**< Represents a passenger's personal data. */
typedef struct airport Airport;                               /**< Represents an airport facility. */
typedef struct aircraft Aircraft;                             /**< Represents an aircraft model and specs. */
typedef struct reservation Reservation;                       /**< Represents a booking made by a passenger. */
typedef struct airport_passenger_stats AirportPassengerStats; /**< Aggregated statistics for an airport. */

// --- Opaque Types ---

/**
 * @typedef Dataset
 * @brief Opaque handle for the main Dataset object.
 *
 * The `Dataset` structure serves as the root container for all application data.
 * It manages the lifecycle (allocation and deallocation) of all entities loaded
 * from the input files.
 *
 * Usage:
 * - Create with `initDataset()`.
 * - Populate using the Loader API (see `dataset_loader.h`).
 * - Query using the accessors and iterators below.
 * - Destroy with `cleanupDataset()`.
 */
typedef struct dataset Dataset;

/**
 * @typedef DatasetIterator
 * @brief Opaque handle for Entity Iterators.
 *
 * Provides a uniform mechanism to traverse collections of entities (Flights,
 * Passengers, etc.) without exposing the underlying data structure (e.g., Hash Table).
 *
 * Usage pattern:
 * @code
 * DatasetIterator *it = dataset_flight_iterator_new(ds);
 * const Flight *flight;
 * while ((flight = (const Flight*)dataset_iterator_next(it)) != NULL) {
 * // Process flight
 * }
 * dataset_iterator_free(it);
 * @endcode
 */
typedef struct dataset_iter DatasetIterator;

/**
 * @typedef DatasetStringIterator
 * @brief Opaque handle for String List Iterators.
 *
 * Specialized iterator for traversing auxiliary lists of strings, such as
 * unique Airport Codes, Aircraft Manufacturers, or Nationalities. These lists
 * are typically used for validation or autocomplete suggestions in the UI.
 */
typedef struct dataset_string_iter DatasetStringIterator;

// --- Life Cycle ---

/**
 * @brief Initializes a new Dataset instance.
 *
 * Allocates memory for the `Dataset` container and initializes all internal
 * collections to an empty state.
 *
 * @return A pointer to the newly allocated `Dataset` instance.
 * @warning The returned pointer must be freed using `cleanupDataset()` to avoid memory leaks.
 */
Dataset *initDataset(void);

/**
 * @brief Destroys the Dataset and frees all associated memory.
 *
 * This function performs a deep clean:
 * 1. Destroys all internal hash tables.
 * 2. Frees every Entity (Flight, Passenger, etc.) stored within.
 * 3. Frees all auxiliary indices and caches.
 * 4. Frees the `Dataset` struct itself.
 *
 * @param ds Pointer to the Dataset to destroy. If `NULL`, the function does nothing.
 */
void cleanupDataset(Dataset *ds);

// --- Counters ---

/**
 * @brief Gets the total number of flights stored in the dataset.
 *
 * @param ds The dataset instance.
 * @return The count of flight records, or 0 if `ds` is NULL.
 */
int dataset_get_flight_count(const Dataset *ds);

/**
 * @brief Gets the total number of aircraft stored in the dataset.
 *
 * @param ds The dataset instance.
 * @return The count of aircraft records, or 0 if `ds` is NULL.
 */
int dataset_get_aircraft_count(const Dataset *ds);

/**
 * @brief Gets the total number of passengers stored in the dataset.
 *
 * @param ds The dataset instance.
 * @return The count of passenger records, or 0 if `ds` is NULL.
 */
int dataset_get_passenger_count(const Dataset *ds);

/**
 * @brief Gets the total number of reservations stored in the dataset.
 *
 * @param ds The dataset instance.
 * @return The count of reservation records, or 0 if `ds` is NULL.
 */
int dataset_get_reservation_count(const Dataset *ds);

// --- Accessors ---

/**
 * @brief Retrieves a read-only reference to a specific Flight.
 *
 * Performs an O(1) lookup based on the Flight ID.
 *
 * @param ds The dataset instance.
 * @param id The unique Flight ID string (e.g., "0000000001").
 * @return A `const` pointer to the Flight entity, or `NULL` if not found.
 */
const Flight *dataset_get_flight(const Dataset *ds, const char *id);

/**
 * @brief Retrieves a read-only reference to a specific Passenger.
 *
 * Performs an O(1) lookup based on the Passenger ID.
 *
 * @param ds The dataset instance.
 * @param id The Passenger ID (Document Number) as an integer.
 * @return A `const` pointer to the Passenger entity, or `NULL` if not found.
 */
const Passenger *dataset_get_passenger(const Dataset *ds, int id);

/**
 * @brief Retrieves a read-only reference to a specific Airport.
 *
 * Performs an O(1) lookup based on the IATA Airport Code.
 *
 * @param ds The dataset instance.
 * @param code The 3-letter IATA code (e.g., "LIS", "JFK").
 * @return A `const` pointer to the Airport entity, or `NULL` if not found.
 */
const Airport *dataset_get_airport(const Dataset *ds, const char *code);

/**
 * @brief Retrieves a read-only reference to a specific Aircraft.
 *
 * Performs an O(1) lookup based on the Aircraft ID.
 *
 * @param ds The dataset instance.
 * @param id The unique Aircraft ID string.
 * @return A `const` pointer to the Aircraft entity, or `NULL` if not found.
 */
const Aircraft *dataset_get_aircraft(const Dataset *ds, const char *id);

/**
 * @brief Retrieves a read-only reference to a specific Reservation.
 *
 * Performs an O(1) lookup based on the Reservation ID.
 *
 * @param ds The dataset instance.
 * @param id The unique Reservation ID string (e.g., "Book0000000001").
 * @return A `const` pointer to the Reservation entity, or `NULL` if not found.
 */
const Reservation *dataset_get_reservation(const Dataset *ds, const char *id);

/**
 * @brief Retrieves pre-computed passenger statistics for an airport.
 *
 * @param ds The dataset instance.
 * @param code The 3-letter IATA Airport Code.
 * @return A `const` pointer to the `AirportPassengerStats` struct, or `NULL` if no stats exist for this code.
 */
const AirportPassengerStats *dataset_get_airport_stats(const Dataset *ds, const char *code);

/**
 * @brief Creates a new iterator for the list of unique Airport Codes.
 *
 * This iterator traverses the sorted list of all known airport codes.
 * Useful for validating user input or providing autocomplete suggestions.
 *
 * @param ds The dataset instance.
 * @return A pointer to a new `DatasetStringIterator`. Must be freed with `dataset_string_iter_free()`.
 */
DatasetStringIterator *dataset_airport_codes_iter_new(Dataset *ds);

/**
 * @brief Creates a new iterator for the list of unique Aircraft Manufacturers.
 *
 * This iterator traverses the sorted list of all unique manufacturer names found in the dataset.
 *
 * @param ds The dataset instance.
 * @return A pointer to a new `DatasetStringIterator`. Must be freed with `dataset_string_iter_free()`.
 */
DatasetStringIterator *dataset_aircraft_manufacturers_iter_new(Dataset *ds);

/**
 * @brief Creates a new iterator for the list of unique Passenger Nationalities.
 *
 * This iterator traverses the sorted list of all unique nationalities found in the dataset.
 *
 * @param ds The dataset instance.
 * @return A pointer to a new `DatasetStringIterator`. Must be freed with `dataset_string_iter_free()`.
 */
DatasetStringIterator *dataset_nationalities_iter_new(Dataset *ds);

/**
 * @brief Advances the string iterator and retrieves the next string.
 *
 * @param it The iterator instance.
 * @return A `const char*` to the next string in the sequence, or `NULL` if the end of the list is reached.
 * @warning The returned string is owned by the Dataset. Do not free or modify it.
 */
const char *dataset_string_iter_next(DatasetStringIterator *it);

/**
 * @brief Frees the memory allocated for a String Iterator.
 *
 * Does not affect the underlying data in the Dataset.
 *
 * @param it The iterator to free. If `NULL`, does nothing.
 */
void dataset_string_iter_free(DatasetStringIterator *it);

/**
 * @brief Creates a new general iterator for traversing all Flights.
 *
 * @param ds The dataset instance.
 * @return A pointer to a new `DatasetIterator`. Must be freed with `dataset_iterator_free()`.
 */
DatasetIterator *dataset_flight_iterator_new(const Dataset *ds);

/**
 * @brief Creates a new general iterator for traversing all Aircraft.
 *
 * @param ds The dataset instance.
 * @return A pointer to a new `DatasetIterator`. Must be freed with `dataset_iterator_free()`.
 */
DatasetIterator *dataset_aircraft_iterator_new(const Dataset *ds);

/**
 * @brief Creates a new general iterator for traversing all Reservations.
 *
 * @param ds The dataset instance.
 * @return A pointer to a new `DatasetIterator`. Must be freed with `dataset_iterator_free()`.
 */
DatasetIterator *dataset_reservation_iterator_new(const Dataset *ds);

/**
 * @brief Creates a new general iterator for traversing all Passengers.
 *
 * @param ds The dataset instance.
 * @return A pointer to a new `DatasetIterator`. Must be freed with `dataset_iterator_free()`.
 */
DatasetIterator *dataset_passenger_iterator_new(const Dataset *ds);

/**
 * @brief Advances the entity iterator and retrieves the next entity.
 *
 * @param it The iterator instance.
 * @return A `const void*` pointer to the next entity.
 * - The caller must cast this pointer to the appropriate type (e.g., `const Flight*`).
 * - Returns `NULL` when the iteration is complete.
 */
const void *dataset_iterator_next(DatasetIterator *it);

/**
 * @brief Frees the memory allocated for an Entity Iterator.
 *
 * @param it The iterator to free. If `NULL`, does nothing.
 */
void dataset_iterator_free(DatasetIterator *it);

#endif // DATASET_H