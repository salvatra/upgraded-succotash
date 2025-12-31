#ifndef FLIGHTS_H
#define FLIGHTS_H

#include <glib.h>
#include <time.h>

/**
 * @brief Opaque structure representing a flight.
 */
typedef struct flight Flight;

/**
 * @brief Frees the memory allocated for a Flight structure.
 *
 * This function is intended to be used as a GDestroyNotify callback for GHashTable or other GLib data structures.
 *
 * @param data A pointer to the Flight structure to be freed.
 */
void freeFlight(gpointer data);

/**
 * @brief Reads flight data from a CSV file and populates a hash table.
 *
 * Validates the data against existing aircraft.
 *
 * @param filename The path to the CSV file containing flight data.
 * @param errorsFlag A pointer to an integer that will be set to 1 if any invalid lines are encountered.
 * @param aircrafts A hash table of existing aircraft for validation.
 * @return A GHashTable where keys are flight IDs (gchar*) and values are Flight pointers. Returns NULL on failure.
 */
GHashTable *readFlights(const char *filename, int *errorsFlag, GHashTable *aircrafts);

/**
 * @brief Retrieves a copy of a flight from the hash table by its ID.
 *
 * This function looks up the flight in the provided hash table and returns a pointer to it (or a copy, check implementation).
 *
 * @param id The ID of the flight to retrieve.
 * @param flightsTable The hash table containing flight data.
 * @return A pointer to the Flight structure, or NULL if not found.
 */
Flight *getFlight(const gchar *id, const GHashTable *flightsTable);

/**
 * @brief Gets the unique ID of a flight.
 *
 * @param flight A pointer to the Flight structure.
 * @return A const string representing the flight ID.
 */
const gchar *getFlightId(const Flight *flight);

/**
 * @brief Gets the scheduled departure time of a flight.
 *
 * @param flight A pointer to the Flight structure.
 * @return The scheduled departure time as a time_t value.
 */
time_t getFlightDeparture(const Flight *flight);

/**
 * @brief Gets the actual departure time of a flight.
 *
 * @param flight A pointer to the Flight structure.
 * @return The actual departure time as a time_t value.
 */
time_t getFlightActualDeparture(const Flight *flight);

/**
 * @brief Gets the scheduled arrival time of a flight.
 *
 * @param flight A pointer to the Flight structure.
 * @return The scheduled arrival time as a time_t value.
 */
time_t getFlightArrival(const Flight *flight);

/**
 * @brief Gets the actual arrival time of a flight.
 *
 * @param flight A pointer to the Flight structure.
 * @return The actual arrival time as a time_t value.
 */
time_t getFlightActualArrival(const Flight *flight);

/**
 * @brief Gets the current status of a flight.
 *
 * @param flight A pointer to the Flight structure.
 * @return A const string representing the flight status (e.g., "Arrived", "Delayed").
 */
const gchar *getFlightStatus(const Flight *flight);

/**
 * @brief Gets the origin airport code of a flight.
 *
 * @param flight A pointer to the Flight structure.
 * @return A const string representing the origin airport code.
 */
const gchar *getFlightOrigin(const Flight *flight);

/**
 * @brief Gets the destination airport code of a flight.
 *
 * @param flight A pointer to the Flight structure.
 * @return A const string representing the destination airport code.
 */
const gchar *getFlightDestination(const Flight *flight);

/**
 * @brief Gets the aircraft model associated with a flight.
 *
 * @param flight A pointer to the Flight structure.
 * @return A const string representing the aircraft model.
 */
const gchar *getFlightAircraft(const Flight *flight);

/**
 * @brief Gets the tracking URL associated with a flight.
 *
 * @param f A pointer to the Flight structure.
 * @return A const string representing the tracking URL.
 */
const gchar *getFlightTrackingUrl(const Flight *f);

/**
 * @brief Gets the airline associated with a flight.
 *
 * @param f A pointer to the Flight structure.
 * @return A const string representing the airline name or code.
 */
const gchar *getFlightAirline(const Flight *f);

/**
 * @brief Gets the gate number associated with a flight.
 *
 * @param f A pointer to the Flight structure.
 * @return The gate number as an integer.
 */
gint getFlightGate(const Flight *f);

#endif //! FLIGHTS_H
