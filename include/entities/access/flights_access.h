/**
 * @file flights_access.h
 * @brief Public interface for accessing and managing Flight entities.
 *
 * This header defines the opaque `Flight` structure and provides functions for:
 * - Lifecycle management (freeing memory).
 * - Parsing (reading from CSV).
 * - Data retrieval (getters for entity properties).
 *
 * It serves as the bridge between the internal storage of flight data and
 * the higher-level modules (queries, statistics) that need to consume it.
 */

#ifndef FLIGHTS_ACCESS_H
#define FLIGHTS_ACCESS_H

#include <glib.h>
#include <time.h>

/**
 * @typedef Flight
 * @brief Opaque structure representing a single Flight entity.
 *
 * The internal layout is defined in `src/entities/internal/flights_internal.h`
 * and is hidden from external modules to enforce encapsulation.
 */
typedef struct flight Flight;

/**
 * @brief Memory cleanup function for a Flight structure.
 *
 * Compatible with `GDestroyNotify`. Frees all dynamically allocated memory associated
 * with the Flight instance (strings, etc.) and the structure itself.
 *
 * @param data A pointer to the `Flight` structure to free.
 */
void freeFlight(gpointer data);

/**
 * @brief Parses the Flights CSV file and populates a hash table.
 *
 * Reads `flights.csv`, performs rigorous validation on timestamps, status,
 * airport codes, and aircraft IDs. Valid records are stored in a hash table.
 *
 * @param filename The full path to the `flights.csv` file.
 * @param errorsFlag Pointer to an integer flag. Set to 1 if any invalid lines are encountered/logged.
 * @param aircrafts A hash table of known Aircraft entities. Used to validate that
 * the aircraft assigned to the flight actually exists in the database.
 * @return A `GHashTable*` containing `Flight*` values indexed by Flight ID strings.
 * Returns NULL on file error.
 */
GHashTable *readFlights(const char *filename, int *errorsFlag, GHashTable *aircrafts);

/**
 * @brief Retrieves a read-only reference to a flight from the lookup table.
 *
 * @param id The Flight ID string (e.g., "0000000001").
 * @param flightsTable The hash table containing all flight entities.
 * @return A const pointer to the `Flight` entity, or NULL if not found.
 */
const Flight *getFlight(const gchar *id, const GHashTable *flightsTable);

// --- Getters ---

/**
 * @brief Gets the unique identifier of the flight.
 * @param flight The flight entity.
 * @return The Flight ID string.
 */
const gchar *getFlightId(const Flight *flight);

/**
 * @brief Gets the scheduled departure time.
 * @param flight The flight entity.
 * @return The scheduled departure timestamp (Unix epoch).
 */
time_t getFlightDeparture(const Flight *flight);

/**
 * @brief Gets the actual departure time.
 * @param flight The flight entity.
 * @return The actual departure timestamp (Unix epoch), or -1 if cancelled/invalid.
 */
time_t getFlightActualDeparture(const Flight *flight);

/**
 * @brief Gets the scheduled arrival time.
 * @param flight The flight entity.
 * @return The scheduled arrival timestamp (Unix epoch).
 */
time_t getFlightArrival(const Flight *flight);

/**
 * @brief Gets the actual arrival time.
 * @param flight The flight entity.
 * @return The actual arrival timestamp (Unix epoch), or -1 if cancelled/invalid.
 */
time_t getFlightActualArrival(const Flight *flight);

/**
 * @brief Gets the current status of the flight.
 * @param flight The flight entity.
 * @return The status string (e.g., "On Time", "Delayed", "Cancelled").
 */
const gchar *getFlightStatus(const Flight *flight);

/**
 * @brief Gets the origin airport code.
 * @param flight The flight entity.
 * @return The 3-letter IATA code of the origin airport.
 */
const gchar *getFlightOrigin(const Flight *flight);

/**
 * @brief Gets the destination airport code.
 * @param flight The flight entity.
 * @return The 3-letter IATA code of the destination airport.
 */
const gchar *getFlightDestination(const Flight *flight);

/**
 * @brief Gets the ID of the aircraft assigned to this flight.
 * @param flight The flight entity.
 * @return The aircraft ID string.
 */
const gchar *getFlightAircraft(const Flight *flight);

/**
 * @brief Gets the airline operating the flight.
 * @param flight The flight entity.
 * @return The airline name string.
 */
const gchar *getFlightAirline(const Flight *flight);

#endif