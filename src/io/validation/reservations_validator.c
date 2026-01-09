#include "io/validation/reservations_validator.h"
#include "core/utils.h"
#include <string.h>

#define RESERVATION_REGEX "^R[0-9]{9}$"

gboolean checkReservationId(const gchar *id)
{
    return checkRegex(RESERVATION_REGEX, id);
}

gint compareReservations(const void *a, const void *b)
{
    return g_strcmp0((const gchar *)a, (const gchar *)b);
}

gboolean checkReservationIdList(const gchar *flight, GArray *reservations)
{
    return g_array_binary_search(reservations, flight, compareReservations, NULL);
}

gboolean checkReservationFlightDestOrig(const gchar *DestinationFirstFlight,
                                        const gchar *OriginSecondFlight)
{
    return (strcmp(DestinationFirstFlight, OriginSecondFlight) == 0);
}