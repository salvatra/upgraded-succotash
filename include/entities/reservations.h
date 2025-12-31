#ifndef RESERVATIONS_H
#define RESERVATIONS_H

#include <glib.h>

/**
 * @brief Opaque structure representing a flight reservation.
 */
typedef struct reservation Reservation;

/**
 * @brief Opaque structure for holding airport passenger statistics.
 */
typedef struct AiportPassengStats AirportPassengerStats;

/**
 * @brief Frees the memory allocated for a Reservation structure.
 *
 * This function is intended to be used as a GDestroyNotify callback for GHashTable or other GLib data structures.
 *
 * @param data A pointer to the Reservation structure to be freed.
 */
void freeReservations(gpointer data);

/**
 * @brief Reads reservation data from a CSV file and populates a hash table.
 *
 * Validates the data against existing passengers and flights.
 *
 * @param filename The path to the CSV file containing reservation data.
 * @param passengersTable A hash table of existing passengers for validation.
 * @param flightsTable A hash table of existing flights for validation.
 * @param errorsFlag A pointer to an integer that will be set to 1 if any invalid lines are encountered.
 * @return A GHashTable where keys are reservation IDs and values are Reservation pointers. Returns NULL on failure.
 */
GHashTable *readReservations(const char *filename, GHashTable *passengersTable, GHashTable *flightsTable, int *errorsFlag);

/**
 * @brief Retrieves a copy of a reservation from the hash table by its ID.
 *
 * This function looks up the reservation in the provided hash table and returns a pointer to it.
 * Note: Check implementation to confirm if it returns a copy or a direct pointer. The 'getAircraft' implementation returned a copy.
 *
 * @param id The ID of the reservation to retrieve.
 * @param reservationsTable The hash table containing reservation data.
 * @return A pointer to the Reservation structure, or NULL if not found.
 */
Reservation *getReservation(const gchar *id, GHashTable *reservationsTable);

/**
 * @brief Gets the ID of a reservation.
 *
 * @param r A pointer to the Reservation structure.
 * @return A const string representing the reservation ID.
 */
const gchar *getReservationId(const Reservation *r);

/**
 * @brief Gets the flight IDs associated with a reservation.
 *
 * @param r A pointer to the Reservation structure.
 * @return A NULL-terminated array of strings representing flight IDs.
 */
gchar **getReservationFlightIds(const Reservation *r);

/**
 * @brief Gets the document number of the passenger who made the reservation.
 *
 * @param r A pointer to the Reservation structure.
 * @return The passenger's document number as an integer.
 */
int getReservationDocumentNo(const Reservation *r);

/**
 * @brief Gets the seat number associated with a reservation.
 *
 * @param r A pointer to the Reservation structure.
 * @return A const string representing the seat number.
 */
const gchar *getReservationSeat(const Reservation *r);

/**
 * @brief Gets the price of a reservation.
 *
 * @param r A pointer to the Reservation structure.
 * @return The price as a float.
 */
gfloat getReservationPrice(const Reservation *r);

/**
 * @brief Checks if extra luggage is included in the reservation.
 *
 * @param r A pointer to the Reservation structure.
 * @return A const string (likely "TRUE" or "FALSE") indicating extra luggage status.
 */
const gchar *getReservationExtraLuggage(const Reservation *r);

/**
 * @brief Checks if priority boarding is included in the reservation.
 *
 * @param r A pointer to the Reservation structure.
 * @return A const string (likely "TRUE" or "FALSE") indicating priority boarding status.
 */
const gchar *getReservationPriorityBoarding(const Reservation *r);

/**
 * @brief Gets the QR code string associated with a reservation.
 *
 * @param r A pointer to the Reservation structure.
 * @return A const string representing the QR code.
 */
const gchar *getReservationQrCode(const Reservation *r);

/**
 * @brief Builds airport passenger statistics from reservations and flights.
 *
 * @param reservations Hash table of reservations.
 * @param flights Hash table of flights.
 * @return A hash table mapping airport codes to AirportPassengerStats structures.
 */
GHashTable *buildAirportPassengerStats(GHashTable *reservations,
                                       GHashTable *flights);

/**
 * @brief Gets the total number of arrivals for an airport from statistics.
 *
 * @param s Pointer to the AirportPassengerStats structure.
 * @return The total number of arrivals.
 */
long getAirportArrivals(const AirportPassengerStats *s);

/**
 * @brief Gets the total number of departures for an airport from statistics.
 *
 * @param s Pointer to the AirportPassengerStats structure.
 * @return The total number of departures.
 */
long getAirportDepartures(const AirportPassengerStats *s);

#endif // !RESERVATIONS_H
