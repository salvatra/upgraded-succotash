#include "io/validation/aircrafts_validator.h"
#include <ctype.h>
#include <string.h>


gboolean checkAircraftId(const char *id)
{
    if (!id)
        return FALSE;

    int i = 0;
    const char *ptr = id;

    for (; i < 2; i++, ptr++)
    {
        if (!isupper(*ptr) && !isdigit(*ptr))
            return FALSE;
    }

    if (*ptr != '-')
        return FALSE;
    ptr++;

    for (i = 0; i < 5; i++, ptr++)
    {
        if (!isupper(*ptr) && !isdigit(*ptr))
            return FALSE;
    }
    if (*ptr != '\0')
        return FALSE;

    return TRUE;
}

gint compareAircraft(const void *a, const void *b)
{
    // 'a' and 'b' are *pointers to pointers of gchar*
    const gchar *idA = *(const gchar **)a;
    const gchar *idB = *(const gchar **)b;
    return g_strcmp0(idA, idB);
}

gboolean checkAircraft(const gchar *flightField, GArray *aircrafts)
{
    gchar *key = (gchar *)flightField;
    guint index;
    return g_array_binary_search(aircrafts, &key, compareAircraft, &index);
}