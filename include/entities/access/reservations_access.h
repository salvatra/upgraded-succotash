/**
 * @file reservations_access.h
 * @brief Public interface for accessing and managing Reservation entities.
 *
 * This header defines the opaque `Reservation` structure and provides functions for:
 * - Lifecycle management (freeing memory).
 * - Parsing (reading from CSV).
 * - Data retrieval (getters for entity properties).
 *
 * This module manages the complex relationships between reservations, passengers,
 * and flights.
 */

#ifndef RESERVATIONS_ACCESS_H
#define RESERVATIONS_ACCESS_H

#include <glib.h>

/**
 * @typedef Reservation
 * @brief Opaque structure representing a single Reservation entity.
 *
 * The internal layout is defined in `src/entities/internal/reservations_internal.h`
 * and is hidden from external modules to enforce encapsulation.
 */
typedef struct reservation Reservation;

/**
 * @brief Memory cleanup function for a Reservation structure.
 *
 * Compatible with `GDestroyNotify`. Frees all dynamically allocated memory associated
 * with the Reservation instance (id string, flight ID list, etc.) and the structure itself.
 *
 * @param data A pointer to the `Reservation` structure to free.
 */
void freeReservations(gpointer data);

/**
 * @brief Parses the Reservations CSV file and populates a hash table.
 *
 * Reads `reservations.csv`, performs extensive validation including:
 * - Verifying the Passenger ID exists in the `passengersTable`.
 * - Verifying all Flight IDs exist in the `flightsTable`.
 * - Checking logical consistency (e.g., flight sequence).
 *
 * @param filename The full path to the `reservations.csv` file.
 * @param passengersTable A hash table of known Passengers (for foreign key validation).
 * @param flightsTable A hash table of known Flights (for foreign key validation).
 * @param errorsFlag Pointer to an integer flag. Set to 1 if any invalid lines are encountered/logged.
 * @return A `GHashTable*` containing `Reservation*` values indexed by Reservation ID strings.
 * Returns NULL on file error.
 */
GHashTable *readReservations(const char *filename, GHashTable *passengersTable,
                             GHashTable *flightsTable, int *errorsFlag);

/**
 * @brief Retrieves a read-only reference to a reservation from the lookup table.
 *
 * @param id The Reservation ID string (e.g., "Book0000000001").
 * @param reservationsTable The hash table containing all reservation entities.
 * @return A const pointer to the `Reservation` entity, or NULL if not found.
 */
const Reservation *getReservation(const gchar *id, const GHashTable *reservationsTable);

// --- Getters ---

/**
 * @brief Gets the unique identifier of the reservation.
 * @param r The reservation entity.
 * @return The Reservation ID string.
 */
const gchar *getReservationId(const Reservation *r);

/**
 * @brief Gets the list of Flight IDs associated with this reservation.
 *
 * @param r The reservation entity.
 * @return A NULL-terminated array of strings, where each string is a Flight ID.
 * Do NOT free the returned array or strings.
 */
gchar **getReservationFlightIds(const Reservation *r);

/**
 * @brief Gets the document number of the passenger who made the reservation.
 * @param r The reservation entity.
 * @return The passenger's document number as an integer.
 */
int getReservationDocumentNo(const Reservation *r);

/**
 * @brief Gets the price of the reservation.
 * @param r The reservation entity.
 * @return The price as a float value (includes tax/fees).
 */
gfloat getReservationPrice(const Reservation *r);

#endif