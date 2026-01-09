/**
 * @file passengers_internal.h
 * @brief Internal structure definition for the Passenger entity.
 *
 * This header defines the actual memory layout of the `struct passenger`.
 * It is intended for internal use only by the Passenger Access module
 * (`src/entities/access/passengers_access.c`) to implement getters and parsing logic.
 *
 * @warning **Internal Header**: Do not include this file in public API headers or
 * consumer modules. Use the opaque `Passenger` typedef from `passengers_access.h` instead.
 */

#ifndef PASSENGERS_INTERNAL_H
#define PASSENGERS_INTERNAL_H

#include <glib.h>
#include <time.h>

/**
 * @struct passenger
 * @brief Internal representation of a Passenger.
 *
 * Stores the personal details of a passenger as parsed from the dataset.
 * Unused fields from the raw CSV (like email or phone) are currently commented out
 * to save memory, but the structure remains extensible.
 */
struct passenger
{
    /**
     * @brief The unique document identifier for the passenger.
     * Used as the key in the hash table.
     */
    int document_number;

    /**
     * @brief The passenger's first name.
     */
    gchar *first_name;

    /**
     * @brief The passenger's last name.
     */
    gchar *last_name;

    /**
     * @brief Date of birth stored as a Unix timestamp.
     * Used for age calculation and date comparison.
     */
    time_t dob;

    /**
     * @brief The nationality of the passenger.
     * Example: "Portuguese", "Spanish".
     */
    gchar *nationality;

    /**
     * @brief The gender of the passenger.
     * Typically 'M' or 'F'.
     */
    char gender;

    // Fields present in CSV but currently unused in business logic:
    // gchar *email;
    // gchar *phone;
    // gchar *address;
    // gchar *photo;
};

#endif