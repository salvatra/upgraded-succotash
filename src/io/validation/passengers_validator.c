#include "io/validation/passengers_validator.h"
#include <ctype.h>
#include <string.h>

gboolean checkDocumentNo(const gchar *no)
{

    const char *ptr = no;
    for (int i = 0; i < 9; i++)
    {
        if (!isdigit(*ptr))
            return FALSE;
        ptr++;
    }
    if (*ptr != '\0')
        return FALSE;

    return TRUE;
}

gint compareDocuments(const void *a, const void *b)
{
    return g_strcmp0((const gchar *)a, (const gchar *)b);
}

gboolean checkDocumentNumber(const gchar *docNumber, GArray *passengers)
{
    return g_array_binary_search(passengers, docNumber, compareDocuments, NULL);
}

gboolean checkPassangerGender(const gchar *gender)
{
    return (gender[1] == '\0' &&
            (gender[0] == 'F' || gender[0] == 'M' || gender[0] == 'O'));
}