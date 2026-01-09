#include "io/parsing/parser_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Flights ---
struct ParsedFields
{
    char *fields[12];
    gboolean ok;
};

ParsedFieldsF *parseFlightLineRaw(char *line)
{
    ParsedFieldsF *pf = malloc(sizeof(ParsedFieldsF));
    pf->ok = FALSE;

    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n')
        line[len - 1] = '\0';

    char *curr = line;
    int idx = 0;

    while (*curr && idx < 12)
    {
        if (*curr == '"')
            curr++;

        pf->fields[idx++] = curr;

        char *end = strchr(curr, '"');
        if (!end)
            return pf;

        *end = '\0';
        curr = end + 1;
        if (*curr == ',')
            curr++;
    }

    if (idx == 12)
        pf->ok = TRUE;
    return pf;
}

gboolean parsed_fields_ok(const ParsedFieldsF *pf) { return pf ? pf->ok : FALSE; }

const char *parsed_fields_get(const ParsedFieldsF *pf, int index)
{
    if (!pf || index < 0 || index >= 12)
        return NULL;
    return pf->fields[index];
}

void parsed_fields_free(ParsedFieldsF *pf) { free(pf); }

// --- Airports ---
struct ParsedAirport
{
    gchar *fields[8];
    gboolean ok;
    gchar *line;
};

ParsedAirportF *parseAirportLineRaw(gchar *line)
{
    ParsedAirportF *pf = g_malloc0(sizeof(ParsedAirportF));
    pf->ok = FALSE;
    if (!line || *line == '\0')
        return pf;

    pf->line = g_strdup(line);
    gchar *copy = g_strdup(line);
    gchar *curr = copy;
    int idx = 0;
    int isCode = 1;

    while (*curr && idx < 8)
    {
        if (*curr == '"')
            curr++;

        gchar *end = strchr(curr, '"');
        if (!end)
            break;

        *end = '\0';
        pf->fields[idx++] = g_strdup(curr);

        if (isCode)
            isCode = !isCode;

        curr = end + 1;
        if (*curr == ',')
            curr++;
    }

    g_free(copy);
    if (idx == 8)
        pf->ok = TRUE;
    return pf;
}

gboolean parsed_airport_ok(const ParsedAirportF *pf) { return pf ? pf->ok : FALSE; }

const gchar *parsed_airport_get(const ParsedAirportF *pf, int index)
{
    if (!pf || index < 0 || index >= 8)
        return NULL;
    return pf->fields[index];
}

void parsed_airport_free(ParsedAirportF *pf)
{
    if (!pf)
        return;
    for (int i = 0; i < 8; i++)
        g_free(pf->fields[i]);
    g_free(pf->line);
    g_free(pf);
}
const gchar *parsed_airport_line(const ParsedAirportF *pf) { return pf ? pf->line : NULL; }

// --- Aircrafts ---
struct ParsedAircraft
{
    gchar *fields[6];
    gboolean ok;
    gchar *line;
};

ParsedAircraftF *parseAircraftLineRaw(const gchar *line)
{
    ParsedAircraftF *pf = g_malloc0(sizeof(ParsedAircraftF));
    pf->ok = FALSE;
    if (!line || *line == '\0')
        return pf;

    pf->line = g_strdup(line);
    gchar *copy = g_strdup(line);
    gchar *curr = copy;
    int idx = 0;

    while (*curr && idx < 6)
    {
        if (*curr == '"')
            curr++;

        gchar *end = strchr(curr, '"');
        if (!end)
            break;
        *end = '\0';

        pf->fields[idx++] = g_strdup(curr);

        curr = end + 1;
        if (*curr == ',')
            curr++;
    }

    g_free(copy);
    if (idx == 6)
        pf->ok = TRUE;
    return pf;
}

gboolean parsed_aircraft_ok(const ParsedAircraftF *pf) { return pf ? pf->ok : FALSE; }

const gchar *parsed_aircraft_get(const ParsedAircraftF *pf, int index)
{
    if (!pf || index < 0 || index >= 6)
        return NULL;
    return pf->fields[index];
}

void parsed_aircraft_free(ParsedAircraftF *pf)
{
    if (!pf)
        return;
    for (int i = 0; i < 6; i++)
        g_free(pf->fields[i]);
    g_free(pf->line);
    g_free(pf);
}

const gchar *parsed_aircraft_line(const ParsedAircraftF *pf) { return pf ? pf->line : NULL; }

// --- Passengers ---
struct ParsedPassenger
{
    gchar *fields[10];
    gboolean ok;
    gchar *line;
};

ParsedPassengerF *parsePassengerLineRaw(const gchar *line)
{
    ParsedPassengerF *pf = g_malloc0(sizeof(ParsedPassengerF));
    pf->ok = FALSE;

    if (!line || *line == '\0')
        return pf;

    pf->line = g_strdup(line);
    gchar *copy = g_strdup(line);
    gchar *curr = copy;
    int idx = 0;

    while (*curr && idx < 10)
    {
        if (*curr != '"')
            break;
        curr++;

        gchar *start = curr;
        gchar *end = strchr(curr, '"');
        if (!end)
            break;

        *end = '\0';
        pf->fields[idx++] = g_strdup(start);

        curr = end + 1;
        if (*curr == ',')
            curr++;
    }

    g_free(copy);
    if (idx == 10)
        pf->ok = TRUE;
    return pf;
}

gboolean parsed_passenger_ok(const ParsedPassengerF *pf) { return pf ? pf->ok : FALSE; }

const gchar *parsed_passenger_get(const ParsedPassengerF *pf, int idx)
{
    if (!pf || idx < 0 || idx >= 10)
        return NULL;
    return pf->fields[idx];
}

const gchar *parsed_passenger_line(const ParsedPassengerF *pf) { return pf ? pf->line : NULL; }

void parsed_passenger_free(ParsedPassengerF *pf)
{
    if (!pf)
        return;
    for (int i = 0; i < 10; i++)
        g_free(pf->fields[i]);
    g_free(pf->line);
    g_free(pf);
}

struct ParsedReservation
{
    gchar *fields[8];
    gboolean ok;
    gchar *line;
};

gchar **parseFlightIds(const gchar *field)
{
    if (!field || !*field)
        return NULL;

    gchar *copy = g_strdup(field);
    g_strstrip(copy);
    size_t len = strlen(copy);

    if (len >= 2 && copy[0] == '[' && copy[len - 1] == ']')
    {
        copy[len - 1] = '\0';
        memmove(copy, copy + 1, len - 1);
    }

    gchar **raw_ids = g_strsplit(copy, ", ", -1);
    GPtrArray *arr = g_ptr_array_new_with_free_func(g_free);

    for (int i = 0; raw_ids[i]; i++)
    {
        gchar *t = g_strdup(raw_ids[i]);
        g_strstrip(t);

        size_t tlen = strlen(t);
        if (tlen >= 2 && t[0] == '\'' && t[tlen - 1] == '\'')
        {
            t[tlen - 1] = '\0';
            memmove(t, t + 1, tlen - 1);
            g_strstrip(t);
        }

        if (*t == '\0')
        {
            g_free(t);
            g_ptr_array_free(arr, TRUE);
            g_strfreev(raw_ids);
            g_free(copy);
            return NULL;
        }

        g_ptr_array_add(arr, t);
    }

    g_ptr_array_add(arr, NULL);
    g_strfreev(raw_ids);
    g_free(copy);

    return (gchar **)g_ptr_array_free(arr, FALSE);
}

ParsedReservationF *parseReservationLineRaw(const gchar *line)
{
    if (!line)
        return NULL;

    ParsedReservationF *pr = g_malloc0(sizeof(*pr));
    pr->ok = FALSE;
    pr->line = g_strdup(line);

    int idx = 0;
    gboolean in_quotes = FALSE;
    GString *field = g_string_new(NULL);

    for (const char *p = line; *p; p++)
    {
        if (*p == '"')
        {
            if (in_quotes && *(p + 1) == '"')
            {
                g_string_append_c(field, '"');
                p++;
            }
            else
            {
                in_quotes = !in_quotes;
            }
        }
        else if (*p == ',' && !in_quotes)
        {
            if (idx >= 8)
            {
                g_string_free(field, TRUE);
                parsed_reservation_free(pr);
                return NULL;
            }
            gchar *tmp = g_strdup(field->str);
            g_strstrip(tmp);
            if (*tmp == '\0')
            {
                g_free(tmp);
                g_string_free(field, TRUE);
                parsed_reservation_free(pr);
                return NULL;
            }
            pr->fields[idx++] = tmp;
            g_string_truncate(field, 0);
        }
        else
        {
            g_string_append_c(field, *p);
        }
    }

    if (in_quotes || idx >= 8)
    {
        g_string_free(field, TRUE);
        parsed_reservation_free(pr);
        return NULL;
    }

    gchar *tmp = g_strdup(field->str);
    g_strstrip(tmp);
    if (*tmp == '\0')
    {
        g_free(tmp);
        g_string_free(field, TRUE);
        parsed_reservation_free(pr);
        return NULL;
    }
    pr->fields[idx++] = tmp;

    g_string_free(field, TRUE);

    if (idx != 8)
    {
        parsed_reservation_free(pr);
        return NULL;
    }

    pr->ok = TRUE;
    return pr;
}

gboolean parsed_reservation_ok(const ParsedReservationF *pr) { return pr ? pr->ok : FALSE; }

const gchar *parsed_reservation_get(const ParsedReservationF *pr, int idx)
{
    if (!pr || idx < 0 || idx >= 8)
        return NULL;
    return pr->fields[idx];
}

const gchar *parsed_reservation_line(const ParsedReservationF *pr) { return pr ? pr->line : NULL; }

void parsed_reservation_free(ParsedReservationF *pr)
{
    if (!pr)
        return;
    for (int i = 0; i < 8; i++)
        g_free(pr->fields[i]);
    g_free(pr->line);
    g_free(pr);
}