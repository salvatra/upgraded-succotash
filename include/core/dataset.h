/**
 * @file dataset.h
 * @brief Central data repository and abstraction layer for the application.
 *
 * This header defines the @c Dataset structure and its public interface.
 * It serves as a facade over the internal data structures (Hash Tables, Arrays),
 * allowing the rest of the application (Queries, Statistics) to access data
 * without knowledge of the underlying implementation details.
 *
 * @note To reduce compilation dependencies and enforce encapsulation,
 * this file uses forward declarations for all entity types.
 */

#ifndef DATASET_H
#define DATASET_H

#include <glib.h>

/* --- Forward Declarations (Opaque Types) --- */

typedef struct aircraft Aircraft;
typedef struct airport Airport;
typedef struct flight Flight;
typedef struct passenger Passenger;
typedef struct reservation Reservation;
typedef struct AiportPassengStats AirportPassengerStats;

/**
 * @brief Opaque structure representing the entire dataset.
 * * Holds references to all collections (Flights, Passengers, etc.) and
 * auxiliary indices needed for the application's operation.
 * Internal layout is hidden from consumers.
 */
typedef struct dataset Dataset;

/**
 * @brief Opaque structure for Date indexing used in temporal queries.
 */
typedef struct datesinfo DatesInfo;

/**
 * @brief Opaque structure for iterating over dataset elements.
 * * Allows sequential traversal of entities without exposing the
 * container type (e.g., GHashTable, GList).
 */
typedef struct dataset_iter DatasetIterator;

/* --- Dataset Statistics / Counts --- */

/**
 * @brief Retrieves the total number of valid flights loaded in the dataset.
 * * @param ds The dataset instance.
 * @return The count of flights. Returns 0 if @p ds is NULL.
 */
int dataset_get_flight_count(const Dataset *ds);

/**
 * @brief Retrieves the total number of valid aircrafts loaded in the dataset.
 * * @param ds The dataset instance.
 * @return The count of aircrafts. Returns 0 if @p ds is NULL.
 */
int dataset_get_aircraft_count(const Dataset *ds);

/**
 * @brief Retrieves the total number of valid passengers loaded in the dataset.
 * * @param ds The dataset instance.
 * @return The count of passengers. Returns 0 if @p ds is NULL.
 */
int dataset_get_passenger_count(const Dataset *ds);

/**
 * @brief Retrieves the total number of valid reservations loaded in the dataset.
 * * @param ds The dataset instance.
 * @return The count of reservations. Returns 0 if @p ds is NULL.
 */
int dataset_get_reservation_count(const Dataset *ds);

/* --- Iterators --- */

/**
 * @brief Creates a new iterator to traverse the collection of Flights.
 * * The iteration order is not guaranteed.
 * * @param ds The dataset instance.
 * @return A pointer to a new @c DatasetIterator.
 * @warning The caller is responsible for freeing the iterator using @ref dataset_iterator_free.
 */
DatasetIterator *dataset_flight_iterator_new(const Dataset *ds);

/**
 * @brief Creates a new iterator to traverse the collection of Aircrafts.
 * * The iteration order is not guaranteed.
 * * @param ds The dataset instance.
 * @return A pointer to a new @c DatasetIterator.
 * @warning The caller is responsible for freeing the iterator using @ref dataset_iterator_free.
 */
DatasetIterator *dataset_aircraft_iterator_new(const Dataset *ds);

/**
 * @brief Creates a new iterator to traverse the collection of Reservations.
 * * The iteration order is not guaranteed.
 * * @param ds The dataset instance.
 * @return A pointer to a new @c DatasetIterator.
 * @warning The caller is responsible for freeing the iterator using @ref dataset_iterator_free.
 */
DatasetIterator *dataset_reservation_iterator_new(const Dataset *ds);

/**
 * @brief Creates a new iterator to traverse the collection of Passengers.
 * * The iteration order is not guaranteed.
 * * @param ds The dataset instance.
 * @return A pointer to a new @c DatasetIterator.
 * @warning The caller is responsible for freeing the iterator using @ref dataset_iterator_free.
 */
DatasetIterator *dataset_passenger_iterator_new(const Dataset *ds);

/**
 * @brief Advances the iterator and retrieves the current item.
 * * @param it The iterator instance.
 * @return A @c const @c void* pointer to the current entity (needs casting to Flight*, Aircraft*, etc.),
 * or @c NULL if the iteration has finished.
 */
const void *dataset_iterator_next(DatasetIterator *it);

/**
 * @brief Frees the memory allocated for the iterator.
 * * @param it The iterator to destroy. If @p it is NULL, does nothing.
 */
void dataset_iterator_free(DatasetIterator *it);

/* --- Lifecycle --- */

/**
 * @brief Initializes a new, empty Dataset structure.
 * * Allocates memory for the dataset and initializes internal containers to NULL.
 * * @return A pointer to the newly allocated Dataset.
 */
Dataset *initDataset(void);

/**
 * @brief Loads and parses all data from CSV files into the Dataset.
 * * This function orchestrates the reading of users, flights, passengers, and reservations.
 * It handles file opening, parsing, validation, and storage.
 * * @param ds The dataset instance to populate.
 * @param errorsFlag Pointer to an integer that will be set to 1 if parsing errors occur (invalid lines), 0 otherwise.
 * @param filePath The directory path containing the CSV files (e.g., "data/").
 * @param enable_timing If @c TRUE, execution time for loading each file is printed to stdout.
 */
void loadAllDatasets(Dataset *ds, gint *errorsFlag, const char *filePath,
                     gboolean enable_timing);

/**
 * @brief Frees all memory associated with the Dataset.
 * * Destroys all internal collections, frees all entities (Flights, Users, etc.),
 * and frees the Dataset structure itself.
 * * @param ds The dataset to destroy. If @p ds is NULL, does nothing.
 */
void cleanupDataset(Dataset *ds);

/* --- Random Access / Lookups (Getters) --- */

/**
 * @brief Retrieves a Flight by its ID.
 * * @param ds The dataset instance.
 * @param id The Flight ID to search for.
 * @return A @c const pointer to the Flight, or @c NULL if not found.
 */
const Flight *dataset_get_flight(const Dataset *ds, const char *id);

/**
 * @brief Retrieves an Airport by its code (IATA).
 * * @param ds The dataset instance.
 * @param code The Airport code (e.g., "LIS").
 * @return A @c const pointer to the Airport, or @c NULL if not found.
 */
const Airport *dataset_get_airport(const Dataset *ds, const char *code);

/**
 * @brief Retrieves an Aircraft by its ID.
 * * @param ds The dataset instance.
 * @param id The Aircraft ID (e.g., tail number).
 * @return A @c const pointer to the Aircraft, or @c NULL if not found.
 */
const Aircraft *dataset_get_aircraft(const Dataset *ds, const char *id);

/**
 * @brief Retrieves a Passenger by their User ID.
 * * @param ds The dataset instance.
 * @param id The User ID (integer).
 * @return A @c const pointer to the Passenger, or @c NULL if not found.
 */
const Passenger *dataset_get_passenger(const Dataset *ds, int id);

/**
 * @brief Retrieves a Reservation by its ID.
 * * @param ds The dataset instance.
 * @param id The Reservation ID string.
 * @return A @c const pointer to the Reservation, or @c NULL if not found.
 */
const Reservation *dataset_get_reservation(const Dataset *ds, const char *id);

/**
 * @brief Retrieves pre-calculated passenger statistics for a specific airport.
 * * @param ds The dataset instance.
 * @param code The Airport code.
 * @return A @c const pointer to the AirportPassengerStats structure, or @c NULL if no data exists.
 */
const struct AiportPassengStats *dataset_get_airport_stats(const Dataset *ds, const char *code);

/* --- Auxiliary Data Access --- */

/**
 * @brief Retrieves a sorted list of all unique Airport codes.
 * * @param ds The dataset instance.
 * @return A @c GPtrArray* containing strings (airport codes).
 * @note Do not free the array's content, it belongs to the dataset.
 */
GPtrArray *get_dataset_airport_codes(Dataset *ds);

/**
 * @brief Retrieves a sorted list of all unique Aircraft manufacturers.
 * * @param ds The dataset instance.
 * @return A @c GPtrArray* containing strings (manufacturer names).
 */
GPtrArray *get_dataset_aircraft_manufacturers(Dataset *ds);

/**
 * @brief Retrieves a sorted list of all unique Passenger nationalities.
 * * @param ds The dataset instance.
 * @return A @c GPtrArray* containing strings (nationality names).
 */
GPtrArray *get_dataset_nationalities(Dataset *ds);

#endif // DATASET_H