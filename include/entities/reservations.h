/**
 * @file reservations.h
 * @brief Definition and management logic for Reservation entities.
 *
 * This module defines the Reservation entity, which serves as the associative link
 * between Passengers (Users) and Flights. It handles parsing, validation, and storage logic.
 *
 * @note This header strictly enforces encapsulation and zero-copy access to ensure
 * maximum performance, especially given that reservations often contain complex
 * lists of flight references.
 */

#ifndef RESERVATIONS_H
#define RESERVATIONS_H

#include <glib.h>

/**
 * @brief Opaque structure representing a Flight Reservation.
 *
 * Encapsulates the Booking ID, the Passenger (User ID), the total Price,
 * and the list of flight segments associated with this booking.
 * The internal memory layout is hidden to preserve encapsulation.
 */
typedef struct reservation Reservation;

/**
 * @brief Memory cleanup function for a Reservation structure.
 *
 * Designed to be used as a @c GDestroyNotify callback for @c GHashTable.
 * It performs a deep free, releasing the internal string array of flight IDs
 * and the structure itself.
 *
 * @param data A generic pointer to the Reservation structure to be freed.
 * If @p data is NULL, does nothing.
 */
void freeReservations(gpointer data);

/**
 * @brief Parses the Reservations CSV file and populates a Hash Table.
 *
 * This function handles file I/O, CSV parsing, and **Cross-Entity Validation**.
 * It ensures data consistency by verifying relationships:
 * - **User Existence:** Checks if `user_id` exists in @p passengersTable.
 * - **Flight Existence:** Checks if every `flight_id` in the reservation exists in @p flightsTable.
 * - **Date Consistency:** Validates if flight dates follow a logical chronological order (if implemented).
 *
 * @param filename The full path to the `reservations.csv` file.
 * @param passengersTable A Hash Table of existing passengers (Key: ID -> Value: Passenger*).
 * Used to validate the `user_id` field.
 * @param flightsTable A Hash Table of existing flights (Key: ID -> Value: Flight*).
 * Used to validate the list of `flight_ids`.
 * @param errorsFlag Pointer to an integer that will be set to 1 if any invalid or inconsistent lines are found.
 *
 * @return A new @c GHashTable where:
 * - Key: Reservation ID (string).
 * - Value: Pointer to @c Reservation structure.
 * Returns @c NULL if the file cannot be opened.
 */
GHashTable *readReservations(const char *filename, GHashTable *passengersTable,
                             GHashTable *flightsTable, int *errorsFlag);

/**
 * @brief Retrieves a read-only reference to a reservation from the repository.
 *
 * **PERFORMANCE CRITICAL:** This function returns a direct pointer to the
 * data stored in the hash table. It involves NO memory allocation and NO copying.
 * Complexity: O(1).
 *
 * @warning The returned pointer is owned by the Dataset. The caller **MUST NOT**
 * free it, modify it, or store it beyond the lifespan of the dataset.
 *
 * @param id The unique Reservation ID to retrieve (e.g., "Book001").
 * @param reservationsTable The hash table containing the reservation dataset.
 * @return A @c const pointer to the Reservation structure, or @c NULL if not found.
 */
const Reservation *getReservation(const gchar *id, const GHashTable *reservationsTable);

/**
 * @brief Accessor for the Reservation ID.
 *
 * @param r Pointer to the constant Reservation structure.
 * @return A constant string representing the ID.
 * Returns @c NULL if @p r is NULL.
 */
const gchar *getReservationId(const Reservation *r);

/**
 * @brief Accessor for the list of Flight IDs associated with the reservation.
 *
 * @param r Pointer to the constant Reservation structure.
 * @return A @c NULL-terminated array of strings (@c gchar**).
 * Each string is a Flight ID. The array is owned by the struct and must not be freed.
 * Returns @c NULL if @p r is NULL.
 */
gchar **getReservationFlightIds(const Reservation *r);

/**
 * @brief Accessor for the Passenger/User Document Number.
 *
 * @param r Pointer to the constant Reservation structure.
 * @return The document number as an integer.
 * Returns 0 if @p r is NULL.
 */
int getReservationDocumentNo(const Reservation *r);

/**
 * @brief Accessor for the Total Price of the reservation.
 *
 * @param r Pointer to the constant Reservation structure.
 * @return The price as a float (including tax).
 * Returns 0.0 if @p r is NULL.
 */
gfloat getReservationPrice(const Reservation *r);

// Unused getters kept commented out for memory optimization to reduce struct size.
// const gchar *getReservationSeat(const Reservation *r);
// const gchar *getReservationExtraLuggage(const Reservation *r);
// const gchar *getReservationPriorityBoarding(const Reservation *r);
// const gchar *getReservationQrCode(const Reservation *r);

#endif // !RESERVATIONS_H