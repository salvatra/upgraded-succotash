/**
 * @file reservations_internal.h
 * @brief Internal structure definition for the Reservation entity.
 *
 * This header defines the actual memory layout of the `struct reservation`.
 * It is intended for internal use only by the Reservation Access module
 * (`src/entities/access/reservations_access.c`) to implement getters and parsing logic.
 *
 * @warning **Internal Header**: Do not include this file in public API headers or
 * consumer modules. Use the opaque `Reservation` typedef from `reservations_access.h` instead.
 */

#ifndef RESERVATIONS_INTERNAL_H
#define RESERVATIONS_INTERNAL_H

#include <glib.h>

/**
 * @struct reservation
 * @brief Internal representation of a Reservation.
 *
 * Stores the details of a booking as parsed from the dataset.
 * A reservation links a passenger to one or more flights.
 */
struct reservation
{
    /**
     * @brief The unique identifier for the reservation.
     * Example: "Book0000000001"
     */
    gchar *reservation_id;

    /**
     * @brief List of flight IDs included in this reservation.
     *
     * A NULL-terminated array of strings. Typically contains 1 (one-way)
     * or 2 (round-trip) flight IDs.
     */
    gchar **flight_ids;

    /**
     * @brief The document number of the passenger who made the booking.
     * Acts as a foreign key to the Passenger entity.
     */
    int document_no;

    /**
     * @brief The total price paid for the reservation.
     * Includes taxes and fees.
     */
    double price;

    // Fields present in CSV but currently unused in business logic:
    // gchar *seat;
    // gchar *extra_luggage;
    // gchar *priority_boarding;
    // gchar *qr_code;
};

#endif