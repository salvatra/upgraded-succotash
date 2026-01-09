#ifndef AIRCRAFTS_VALIDATOR_H
#define AIRCRAFTS_VALIDATOR_H

#include <glib.h>

/**
 * @brief Checks if an aircraft ID is valid.
 */
gboolean checkAircraftId(const char *id);

/**
 * @brief Comparison function for aircraft sorting/searching.
 */
gint compareAircraft(const void *a, const void *b);

/**
 * @brief Checks if an aircraft exists in a list of aircraft.
 */
gboolean checkAircraft(const gchar *flightField, GArray *aircrafts);

#endif