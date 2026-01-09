#ifndef AIRPORTS_VALIDATOR_H
#define AIRPORTS_VALIDATOR_H

#include <glib.h>

/**
 * @brief Checks if an airport code is valid.
 */
gboolean checkAirportCode(const char *code);

/**
 * @brief Checks if an airport type is valid.
 */
gboolean checkType(const gchar *type);

/**
 * @brief Checks if latitude and longitude coordinates are valid.
 */
gboolean checkCoords(const gchar *latitude, const gchar *longitude);

#endif