#define _XOPEN_SOURCE 700
#include "io/validation/validation_utils.h"
#include "core/time_utils.h"
#include "core/utils.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_REGEX "^[0-9]+$"
#define TODAY "2025-09-30"
#define TODAY_T ((time_t)1759190400)
#define LIST_REGEX "^\\[.*\\]$"

gboolean checkInt(const gchar *str) { return checkRegex(NUM_REGEX, str); }

gboolean checkDatetime(const time_t dt)
{
    if (dt == (time_t)-1 || dt == (time_t)-2 || dt == (time_t)-3 ||
        dt == (time_t)-4)
    {
        return FALSE;
    }

    if (dt > TODAY_T)
    {
        return FALSE;
    }

    return TRUE;
}

gboolean checkDate(const time_t dt)
{
    if (dt == (time_t)-1 || dt == (time_t)-2 || dt == (time_t)-3 ||
        dt == (time_t)-4)
    {
        return FALSE;
    }

    time_t today = parse_unix_date(TODAY, NULL);

    if (dt > today)
    {
        return FALSE;
    }

    return TRUE;
}

gboolean checkYear(const gchar *yearChar)
{
    if (!yearChar)
        return 0;

    const gchar *p = yearChar;

    for (int i = 0; i < 4; i++)
    {
        if (!isdigit((unsigned char)*p))
            return FALSE;
        p++;
    }
    if (*p != '\0')
        return FALSE;

    gint year = atoi(yearChar);
    return g_date_valid_year(year);
}

gboolean checkEmail(const char *email)
{
    if (!email)
        return FALSE;

    const char *ptr = email;

    if (!(isdigit(*ptr) || islower(*ptr)))
        return FALSE;
    ptr++;

    while (*ptr != '@')
    {
        if (*ptr == '\0')
            return FALSE;

        if (*ptr == '.')
        {
            ptr++;

            if (!(isdigit(*ptr) || islower(*ptr)))
                return FALSE;
        }
        else if (!(isdigit(*ptr) || islower(*ptr)))
            return FALSE;

        ptr++;
    }

    ptr++;

    int left = 0, right = 0;

    while (*ptr != '.')
    {
        if (*ptr == '\0')
            return FALSE;

        if (!islower(*ptr))
            return FALSE;

        left++;
        ptr++;
    }

    if (left < 1)
        return FALSE;

    ptr++;

    while (*ptr != '\0')
    {
        if (!islower(*ptr))
            return FALSE;

        right++;
        ptr++;
    }

    if (right != 2 && right != 3)
        return FALSE;

    return TRUE;
}

gboolean checkCsvList(const gchar *list)
{
    return checkRegex(LIST_REGEX, list);
}

void logInvalidLine(const gchar *filename, const gchar *header,
                    const gchar *line)
{
    FILE *file = fopen(filename, "a");
    if (!file)
    {
        return;
    }
    if (ftell(file) == 0)
    {
        fprintf(file, "%s\n", header);
    }
    gchar *safe_line = g_strdup(line);
    fprintf(file, "%s\n", safe_line);
    g_free(safe_line);
    fclose(file);
}