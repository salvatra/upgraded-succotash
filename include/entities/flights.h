/**
 * @file flights.h
 * @brief Flight entity definition and high-performance management.
 *
 * This header defines the interface for the Flight entity. It is optimized for
 * memory efficiency and execution speed. Accessors provide direct, read-only
 * access to internal data, eliminating the overhead of copying structures
 * during queries.
 */

#ifndef FLIGHTS_H
#define FLIGHTS_H

#include <glib.h>
#include <time.h>

/**
 * @brief Opaque structure representing a flight.
 *
 * The internal memory layout is hidden to preserve encapsulation.
 * Access is strictly controlled via const-correct getters.
 */
typedef struct flight Flight;

/**
 * @brief Frees the memory allocated for a Flight structure.
 *
 * Intended for use with GHashTable as a GDestroyNotify callback.
 * It frees all internal dynamic strings and the structure itself.
 *
 * @param data A generic pointer to the Flight structure to be freed.
 */
void freeFlight(gpointer data);

/**
 * @brief Reads flight data from a CSV file and populates a hash table.
 *
 * Parses the flights CSV, validates data integrity (including aircraft existence),
 * and stores valid flights.
 *
 * @param filename The path to the CSV file containing flight data.
 * @param errorsFlag A pointer to an integer updated to 1 if invalid lines are found.
 * @param aircrafts A hash table of existing aircraft (const gchar* key -> const Aircraft* val)
 * used to validate that the flight is assigned to a real aircraft.
 *
 * @return A GHashTable where keys are flight IDs (strings) and values are
 * pointers to Flight structures. Returns NULL on failure.
 */
GHashTable *readFlights(const char *filename, int *errorsFlag, GHashTable *aircrafts);

/**
 * @brief Retrieves a read-only reference to a flight from the hash table.
 *
 * **PERFORMANCE CRITICAL:** This function returns a direct pointer to the
 * data stored in the hash table. It involves NO memory allocation and NO copying.
 *
 * @warning The returned pointer is owned by the dataset. The caller MUST NOT
 * free it, modify it, or store it beyond the lifespan of the dataset.
 *
 * @param id The unique ID of the flight to retrieve.
 * @param flightsTable The hash table containing flight data.
 * @return A const pointer to the Flight structure, or NULL if not found.
 */
const Flight *getFlight(const gchar *id, const GHashTable *flightsTable);

/**
 * @brief Gets the unique ID of a flight.
 *
 * @param flight A pointer to the constant Flight structure.
 * @return A constant string representing the flight ID. Owned by the Flight structure.
 */
const gchar *getFlightId(const Flight *flight);

/**
 * @brief Gets the scheduled departure time.
 *
 * @param flight A pointer to the constant Flight structure.
 * @return The scheduled departure timestamp (time_t).
 */
time_t getFlightDeparture(const Flight *flight);

/**
 * @brief Gets the actual departure time.
 *
 * @param flight A pointer to the constant Flight structure.
 * @return The actual departure timestamp (time_t).
 */
time_t getFlightActualDeparture(const Flight *flight);

/**
 * @brief Gets the scheduled arrival time.
 *
 * @param flight A pointer to the constant Flight structure.
 * @return The scheduled arrival timestamp (time_t).
 */
time_t getFlightArrival(const Flight *flight);

/**
 * @brief Gets the actual arrival time.
 *
 * @param flight A pointer to the constant Flight structure.
 * @return The actual arrival timestamp (time_t).
 */
time_t getFlightActualArrival(const Flight *flight);

/**
 * @brief Gets the flight status.
 *
 * @param flight A pointer to the constant Flight structure.
 * @return A constant string representing status (e.g., "Arrived", "Cancelled").
 */
const gchar *getFlightStatus(const Flight *flight);

/**
 * @brief Gets the origin airport code.
 *
 * @param flight A pointer to the constant Flight structure.
 * @return A constant string representing the origin IATA code.
 */
const gchar *getFlightOrigin(const Flight *flight);

/**
 * @brief Gets the destination airport code.
 *
 * @param flight A pointer to the constant Flight structure.
 * @return A constant string representing the destination IATA code.
 */
const gchar *getFlightDestination(const Flight *flight);

/**
 * @brief Gets the aircraft model/ID associated with the flight.
 *
 * @param flight A pointer to the constant Flight structure.
 * @return A constant string representing the aircraft.
 */
const gchar *getFlightAircraft(const Flight *flight);

/**
 * @brief Gets the airline name.
 *
 * @param flight A pointer to the constant Flight structure.
 * @return A constant string representing the airline.
 */
const gchar *getFlightAirline(const Flight *flight);

// Gate and Tracking URL are currently unused in the struct definition to save memory.
// int getFlightGate(const Flight *f);
// const gchar *getFlightTrackingUrl(const Flight *f);

#endif //! FLIGHTS_H