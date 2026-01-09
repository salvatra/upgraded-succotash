/**
 * @file airports_internal.h
 * @brief Internal structure definition for the Airport entity.
 *
 * This header defines the actual memory layout of the `struct airport`.
 * It is intended for internal use only by the Airport Access module
 * (`src/entities/access/airports_access.c`) to implement getters and parsing logic.
 *
 * @warning **Internal Header**: Do not include this file in public API headers or
 * consumer modules. Use the opaque `Airport` typedef from `airports_access.h` instead.
 */

#ifndef AIRPORTS_INTERNAL_H
#define AIRPORTS_INTERNAL_H

#include <glib.h>

/**
 * @struct airport
 * @brief Internal representation of an Airport.
 *
 * Stores the specific attributes of an airport facility as parsed from the dataset.
 */
struct airport
{
    /**
     * @brief The unique 3-letter IATA code for the airport.
     * Example: "LIS" (Lisbon), "JFK" (New York).
     */
    gchar *code;

    /**
     * @brief The official name of the airport.
     * Example: "Humberto Delgado Airport".
     */
    gchar *name;

    /**
     * @brief The city associated with the airport.
     * Example: "Lisbon".
     */
    gchar *city;

    /**
     * @brief The country where the airport is located.
     * Example: "Portugal".
     */
    gchar *country;

    /**
     * @brief The classification type of the airport.
     * Example: "large_airport", "medium_airport", "heliport".
     */
    gchar *type;
};

#endif