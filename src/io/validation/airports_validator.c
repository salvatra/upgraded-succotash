#include "io/validation/airports_validator.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

gboolean checkAirportCode(const char *code)
{
    if (!code)
        return FALSE;

    const char *ptr = code;
    for (int i = 0; i < 3; i++, ptr++)
    {
        if (!isupper(*ptr))
            return FALSE;
    }
    if (*ptr != '\0')
        return FALSE;

    return TRUE;
}

gboolean checkType(const gchar *type)
{
    gchar *types[] = {"small_airport", "medium_airport", "large_airport",
                      "heliport", "seaplane_base"};

    for (int i = 0; i < 5; i++)
    {
        if (!strcmp(type, types[i]))
            return 1;
    }

    return 0;
}

gboolean checkCoords(const gchar *latitude, const gchar *longitude)
{
    if (!latitude || !longitude)
        return FALSE;

    // ######### Latitude ##########
    const gchar *p = latitude;
    gint digits = 0;

    if (*p == '-' || *p == '+')
        p++;
    while (*p != '.' && isdigit((unsigned char)*p))
    {
        digits++;
        p++;
    }
    if (*p != '.')
        return FALSE;
    if (digits == 0 || digits > 2)
        return FALSE;

    p++;
    digits = 0;
    while (*p != '\0' && isdigit((unsigned char)*p))
    {
        digits++;
        p++;
    }
    if (*p != '\0')
        return FALSE;
    if (digits < 1 || digits > 8)
        return FALSE;

    gdouble lattd = g_ascii_strtod(latitude, NULL);
    if (lattd < -90 || lattd > 90)
        return FALSE;

    // ########## Longitude ##########
    p = longitude;
    digits = 0;

    if (*p == '-' || *p == '+')
        p++;
    while (*p != '.' && isdigit((unsigned char)*p))
    {
        digits++;
        p++;
    }
    if (*p != '.')
        return FALSE;
    if (digits == 0 || digits > 3)
        return FALSE;

    p++;
    digits = 0;
    while (*p != '\0' && isdigit((unsigned char)*p))
    {
        digits++;
        p++;
    }
    if (*p != '\0')
        return FALSE;
    if (digits < 1 || digits > 8)
        return FALSE;

    gdouble lngtd = g_ascii_strtod(longitude, NULL);
    if (lngtd < -180 || lngtd > 180)
        return FALSE;

    return TRUE;
}