/**
 * @file flights.h
 * @brief Definition and high-performance management for Flight entities.
 *
 * This module defines the Flight entity and handles its parsing, validation, and storage.
 * It is optimized for memory efficiency and execution speed, as Flights typically
 * constitute the largest portion of the dataset.
 *
 * @note Accessors provide direct, read-only access to internal data, eliminating
 * the overhead of copying structures during high-frequency queries.
 */

#ifndef FLIGHTS_H
#define FLIGHTS_H

#include <glib.h>
#include <time.h>

/**
 * @brief Opaque structure representing a Flight entity.
 *
 * Encapsulates data such as ID, Schedule (Departure/Arrival), Status, Airline,
 * and associated Aircraft. The internal memory layout is hidden to preserve encapsulation.
 */
typedef struct flight Flight;

/**
 * @brief Memory cleanup function for a Flight structure.
 *
 * Intended for use with @c GHashTable as a @c GDestroyNotify callback.
 * It performs a deep free, releasing all internal dynamic strings and the structure itself.
 *
 * @param data A generic pointer to the Flight structure to be freed.
 * If @p data is NULL, does nothing.
 */
void freeFlight(gpointer data);

/**
 * @brief Parses the Flights CSV file and populates a Hash Table.
 *
 * This function handles file I/O, CSV parsing, and rigorous data validation.
 * Validation steps include:
 * - Syntax checking (non-empty mandatory fields).
 * - Logical time validation (Arrival time > Departure time).
 * - **Referential Integrity**: Verifies if the `aircraft_id` exists in the provided @p aircrafts table.
 *
 * @param filename The full path to the `flights.csv` file.
 * @param errorsFlag Pointer to an integer that will be set to 1 if any invalid lines are encountered.
 * @param aircrafts A hash table of existing aircraft (Key: ID string -> Value: Aircraft*).
 * Used to validate that the flight is assigned to a real aircraft.
 *
 * @return A new @c GHashTable where:
 * - Key: Flight ID (string).
 * - Value: Pointer to @c Flight structure.
 * Returns @c NULL if the file cannot be opened.
 */
GHashTable *readFlights(const char *filename, int *errorsFlag, GHashTable *aircrafts);

/**
 * @brief Retrieves a read-only reference to a flight from the repository.
 *
 * **PERFORMANCE CRITICAL:** This function returns a direct pointer to the
 * data stored in the hash table. It involves NO memory allocation and NO copying.
 * Complexity: O(1).
 *
 * @warning The returned pointer is owned by the Dataset. The caller **MUST NOT**
 * free it, modify it, or store it beyond the lifespan of the dataset.
 *
 * @param id The unique ID of the flight to retrieve.
 * @param flightsTable The hash table containing the flight dataset.
 * @return A @c const pointer to the Flight structure, or @c NULL if not found.
 */
const Flight *getFlight(const gchar *id, const GHashTable *flightsTable);

/**
 * @brief Accessor for the Flight ID.
 *
 * @param flight A pointer to the constant Flight structure.
 * @return A constant string representing the flight ID.
 * Returns @c NULL if @p flight is NULL.
 */
const gchar *getFlightId(const Flight *flight);

/**
 * @brief Accessor for the Scheduled Departure Timestamp.
 *
 * @param flight A pointer to the constant Flight structure.
 * @return The scheduled departure time as a @c time_t (Unix timestamp).
 * Returns 0 if @p flight is NULL.
 */
time_t getFlightDeparture(const Flight *flight);

/**
 * @brief Accessor for the Actual Departure Timestamp.
 *
 * @param flight A pointer to the constant Flight structure.
 * @return The actual departure time as a @c time_t (Unix timestamp).
 * Returns 0 if @p flight is NULL.
 */
time_t getFlightActualDeparture(const Flight *flight);

/**
 * @brief Accessor for the Scheduled Arrival Timestamp.
 *
 * @param flight A pointer to the constant Flight structure.
 * @return The scheduled arrival time as a @c time_t (Unix timestamp).
 * Returns 0 if @p flight is NULL.
 */
time_t getFlightArrival(const Flight *flight);

/**
 * @brief Accessor for the Actual Arrival Timestamp.
 *
 * @param flight A pointer to the constant Flight structure.
 * @return The actual arrival time as a @c time_t (Unix timestamp).
 * Returns 0 if @p flight is NULL.
 */
time_t getFlightActualArrival(const Flight *flight);

/**
 * @brief Accessor for the Flight Status.
 *
 * @param flight A pointer to the constant Flight structure.
 * @return A constant string representing status (e.g., "Arrived", "Cancelled").
 * Returns @c NULL if @p flight is NULL.
 */
const gchar *getFlightStatus(const Flight *flight);

/**
 * @brief Accessor for the Origin Airport Code.
 *
 * @param flight A pointer to the constant Flight structure.
 * @return A constant string representing the origin IATA code.
 * Returns @c NULL if @p flight is NULL.
 */
const gchar *getFlightOrigin(const Flight *flight);

/**
 * @brief Accessor for the Destination Airport Code.
 *
 * @param flight A pointer to the constant Flight structure.
 * @return A constant string representing the destination IATA code.
 * Returns @c NULL if @p flight is NULL.
 */
const gchar *getFlightDestination(const Flight *flight);

/**
 * @brief Accessor for the Associated Aircraft ID.
 *
 * @param flight A pointer to the constant Flight structure.
 * @return A constant string representing the aircraft tail number/ID.
 * Returns @c NULL if @p flight is NULL.
 */
const gchar *getFlightAircraft(const Flight *flight);

/**
 * @brief Accessor for the Airline Name.
 *
 * @param flight A pointer to the constant Flight structure.
 * @return A constant string representing the airline.
 * Returns @c NULL if @p flight is NULL.
 */
const gchar *getFlightAirline(const Flight *flight);

// Gate and Tracking URL are currently unused in the struct definition to conserve memory usage.
// int getFlightGate(const Flight *f);
// const gchar *getFlightTrackingUrl(const Flight *f);

#endif //! FLIGHTS_H