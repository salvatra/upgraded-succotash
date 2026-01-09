/**
 * @file aircrafts_internal.h
 * @brief Internal structure definition for the Aircraft entity.
 *
 * This header defines the actual memory layout of the `struct aircraft`.
 * It is intended for internal use only by the Aircraft Access module
 * (`src/entities/access/aircrafts_access.c`) to implement getters and parsing logic.
 *
 * @warning **Internal Header**: Do not include this file in public API headers or
 * consumer modules. Use the opaque `Aircraft` typedef from `aircrafts_access.h` instead.
 */

#ifndef AIRCRAFTS_INTERNAL_H
#define AIRCRAFTS_INTERNAL_H

#include <glib.h>

/**
 * @struct aircraft
 * @brief Internal representation of an Aircraft.
 *
 * Stores the specific attributes of an aircraft as parsed from the dataset.
 */
struct aircraft
{
    /**
     * @brief The unique identifier for the aircraft.
     * Example: "A380-800"
     */
    gchar *id;

    /**
     * @brief The name of the aircraft manufacturer.
     * Example: "Airbus", "Boeing"
     */
    gchar *manufacturer;

    /**
     * @brief The specific model name of the aircraft.
     * Example: "A380", "747"
     */
    gchar *model;
};

#endif