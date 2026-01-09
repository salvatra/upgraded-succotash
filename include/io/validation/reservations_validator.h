#ifndef RESERVATIONS_VALIDATOR_H
#define RESERVATIONS_VALIDATOR_H

#include <glib.h>

/**
 * @brief Checks if a reservation ID is valid.
 */
gboolean checkReservationId(const gchar *id);

/**
 * @brief Comparison function for reservations.
 */
gint compareReservations(const void *a, const void *b);

/**
 * @brief Checks if reservation flight IDs exist in the reservations list.
 */
gboolean checkReservationIdList(const gchar *flight, GArray *reservations);

/**
 * @brief Checks logic for connecting flights in a reservation.
 */
gboolean checkReservationFlightDestOrig(const gchar *DestinationFirstFlight,
                                        const gchar *OriginSecondFlight);

#endif