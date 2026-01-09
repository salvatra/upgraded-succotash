#ifndef FLIGHTS_VALIDATOR_H
#define FLIGHTS_VALIDATOR_H

#include <glib.h>
#include <time.h>

/**
 * @brief Checks if a flight ID is valid.
 */
gboolean checkFlightId(const gchar *id);

/**
 * @brief Checks if destination and origin are different.
 */
gboolean checkDestinationOrigin(const gchar *destination, const gchar *origin);

/**
 * @brief Checks if a flight is delayed correctly.
 */
gboolean checkDelayed(const gchar *status, time_t scheduledDep,
                      time_t scheduledArr, time_t actualDep, time_t actualArr,
                      int cancel_flag);

/**
 * @brief Checks consistency of cancellation status.
 */
gboolean checkCancellation(const gchar *status, time_t act_dep,
                           time_t act_arr);

#endif