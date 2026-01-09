#include "io/validation/flights_validator.h"
#include "core/time_utils.h"
#include <ctype.h>
#include <string.h>

gboolean checkFlightId(const gchar *id)
{
    int i = 0;
    for (; i < 2; i++)
    {
        if (!isupper(id[i]))
            return FALSE;
    }

    for (; i < 7; i++)
    {
        if (!isdigit(id[i]))
            return FALSE;
    }

    return TRUE;
}

gboolean checkDestinationOrigin(const gchar *destination, const gchar *origin)
{
    return strcmp(destination, origin);
}

gboolean checkDelayed(const gchar *status, time_t scheduledDep,
                      time_t scheduledArr, time_t actualDep, time_t actualArr,
                      int cancel_flag)
{
    if (!status)
        return FALSE;

    // Only check if status is "Delayed"
    if (strcmp(status, "Delayed") != 0)
        return TRUE;

    // If actual times are "N/A", the check fails
    if (cancel_flag == 1)
        return FALSE;

    // Compare actual vs scheduled
    gint cmpDep = compare_time_t(actualDep, scheduledDep);
    gint cmpArr = compare_time_t(actualArr, scheduledArr);

    return (cmpDep >= 0 && cmpArr >= 0);
}

gboolean checkCancellation(const gchar *status, time_t act_dep,
                           time_t act_arr)
{
    if (g_strcmp0(status, "Cancelled") != 0)
        return TRUE;

    return (act_dep == (time_t)-1 && act_arr == (time_t)-1);
}