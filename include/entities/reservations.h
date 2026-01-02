/**
 * @file reservations.h
 * @brief Reservation entity definition and management.
 *
 * This header defines the interface for the Reservation entity.
 * It strictly enforces encapsulation and zero-copy access for maximum performance.
 */

#ifndef RESERVATIONS_H
#define RESERVATIONS_H

#include <glib.h>

/**
 * @brief Opaque structure representing a flight reservation.
 */
typedef struct reservation Reservation;

/**
 * @brief Opaque structure for holding airport passenger statistics.
 * (Note: Ideally this should move to a statistics module in the future,
 * but kept here for now to maintain compilation).
 */
typedef struct AiportPassengStats AirportPassengerStats;

/**
 * @brief Frees the memory allocated for a Reservation structure.
 *
 * Intended as a GDestroyNotify callback.
 *
 * @param data A generic pointer to the Reservation structure.
 */
void freeReservations(gpointer data);

/**
 * @brief Reads reservation data from a CSV file and populates a hash table.
 *
 * Parses reservations, validates them against passengers and flights, and checks
 * logical consistency (e.g., flight connections).
 *
 * @param filename Path to the CSV file.
 * @param passengersTable Hash table of existing passengers (for validation).
 * @param flightsTable Hash table of existing flights (for validation).
 * @param errorsFlag Pointer to integer, set to 1 if invalid lines are found.
 * @return A GHashTable (Key: Reservation ID, Value: Reservation*). Returns NULL on failure.
 */
GHashTable *readReservations(const char *filename, GHashTable *passengersTable,
                             GHashTable *flightsTable, int *errorsFlag);

/**
 * @brief Retrieves a read-only reference to a reservation.
 *
 * **PERFORMANCE CRITICAL:** Returns a direct pointer to the internal data.
 * Zero copies, zero allocations.
 *
 * @warning The returned pointer is owned by the dataset. DO NOT free or modify.
 *
 * @param id The reservation ID.
 * @param reservationsTable The hash table containing reservations.
 * @return A const pointer to the Reservation, or NULL if not found.
 */
const Reservation *getReservation(const gchar *id, const GHashTable *reservationsTable);

/**
 * @brief Gets the Reservation ID.
 *
 * @param r Pointer to the constant Reservation structure.
 * @return Constant string of the ID.
 */
const gchar *getReservationId(const Reservation *r);

/**
 * @brief Gets the list of Flight IDs in the reservation.
 *
 * @param r Pointer to the constant Reservation structure.
 * @return A NULL-terminated array of strings. The array belongs to the struct.
 */
gchar **getReservationFlightIds(const Reservation *r);

/**
 * @brief Gets the document number of the passenger.
 *
 * @param r Pointer to the constant Reservation structure.
 * @return Document number as integer.
 */
int getReservationDocumentNo(const Reservation *r);

/**
 * @brief Gets the price of the reservation.
 *
 * @param r Pointer to the constant Reservation structure.
 * @return Price as float.
 */
gfloat getReservationPrice(const Reservation *r);

// Unused getters kept commented out for memory optimization
// const gchar *getReservationSeat(const Reservation *r);
// const gchar *getReservationExtraLuggage(const Reservation *r);
// const gchar *getReservationPriorityBoarding(const Reservation *r);
// const gchar *getReservationQrCode(const Reservation *r);

/**
 * @brief Builds airport passenger statistics from reservations and flights.
 *
 * Iterates through all reservations and calculates arrivals/departures per airport.
 *
 * @param reservations Hash table of reservations.
 * @param flights Hash table of flights.
 * @return A hash table mapping airport codes to AirportPassengerStats structures.
 */
GHashTable *buildAirportPassengerStats(GHashTable *reservations,
                                       GHashTable *flights);

/**
 * @brief Gets the total number of arrivals for an airport from statistics.
 */
long getAirportArrivals(const AirportPassengerStats *s);

/**
 * @brief Gets the total number of departures for an airport from statistics.
 */
long getAirportDepartures(const AirportPassengerStats *s);

#endif // !RESERVATIONS_H