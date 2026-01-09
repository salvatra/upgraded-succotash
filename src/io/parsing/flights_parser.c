#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <glib.h>

#include "entities/access/flights_access.h"
#include "entities/internal/flights_internal.h"
#include "io/parsing/parser_utils.h"
#include "core/time_utils.h"
#include "validation.h"

static FlightStatus parse_status_string(const gchar *status_str)
{
    if (!status_str)
        return FLIGHT_UNKNOWN;

    if (g_ascii_strcasecmp(status_str, "On Time") == 0)
        return FLIGHT_ON_TIME;
    if (g_ascii_strcasecmp(status_str, "Delayed") == 0)
        return FLIGHT_DELAYED;
    if (g_ascii_strcasecmp(status_str, "Cancelled") == 0)
        return FLIGHT_CANCELLED;

    return FLIGHT_UNKNOWN;
}

GHashTable *readFlights(const char *filename, int *errorsFlag, GHashTable *aircrafts)
{
    FILE *flights = fopen(filename, "r");
    if (!flights)
        return NULL;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    if ((read = getline(&line, &len, flights)) == -1)
    {
        free(line);
        fclose(flights);
        return NULL;
    }
    if (read > 0 && line[read - 1] == '\n')
        line[--read] = '\0';
    char *headerLine = g_strdup(line);

    GHashTable *flightsTable =
        g_hash_table_new_full(g_str_hash, g_str_equal, NULL, freeFlight);

    while ((read = getline(&line, &len, flights)) != -1)
    {
        ParsedFieldsF *pf = parseFlightLineRaw(line);
        if (!parsed_fields_ok(pf))
        {
            parsed_fields_free(pf);
            continue;
        }

        gboolean invalid = FALSE;
        const char *fields[12];
        for (int i = 0; i < 12; i++)
            fields[i] = parsed_fields_get(pf, i);

        // --- VALIDATION LOGIC ---
        if (!checkFlightId(fields[0]))
            invalid = TRUE;

        time_t sched_dep = 0, act_dep = 0, sched_arr = 0, act_arr = 0;
        int cancelled_flag = 0;

        if (!invalid)
        {
            sched_dep = parse_unix_datetime(fields[1], NULL);
            if (sched_dep < 0)
                invalid = TRUE;

            sched_arr = parse_unix_datetime(fields[3], NULL);
            if (sched_arr < 0)
                invalid = TRUE;

            act_dep = parse_unix_datetime(fields[2], &cancelled_flag);
            if (act_dep < 0 && cancelled_flag == 0)
                invalid = TRUE;

            act_arr = parse_unix_datetime(fields[4], &cancelled_flag);
            if (act_arr < 0 && cancelled_flag == 0)
                invalid = TRUE;
        }

        if (!invalid)
        {
            if (!checkDelayed(fields[6], sched_dep, sched_arr, act_dep, act_arr, cancelled_flag) ||
                !checkCancellation(fields[6], act_dep, act_arr) ||
                !checkDestinationOrigin(fields[8], fields[7]))
            {
                invalid = TRUE;
            }
        }

        if (!invalid)
        {
            if (compare_unix_datetime(fields[1], fields[2]) > 0 ||
                compare_unix_datetime(fields[3], fields[4]) > 0 ||
                compare_unix_datetime(fields[1], fields[3]) >= 0 ||
                compare_unix_datetime(fields[2], fields[4]) >= 0)
            {
                invalid = TRUE;
            }
        }

        if (!invalid)
        {
            // These functions are now visible via their respective headers
            if (!checkAirportCode(fields[7]) || !checkAirportCode(fields[8]))
                invalid = TRUE;
        }

        if (!invalid && (!fields[9] || !g_hash_table_contains(aircrafts, fields[9])))
        {
            invalid = TRUE;
        }

        if (!invalid)
        {
            if (!checkAircraftId(fields[9]))
                invalid = TRUE;
        }

        if (invalid)
        {
            char tempBuffer[4096] = "";
            for (int i = 0; i < 12; i++)
            {
                if (i > 0)
                    strcat(tempBuffer, ",");
                strcat(tempBuffer, "\"");
                strcat(tempBuffer, fields[i]);
                strcat(tempBuffer, "\"");
            }
            logInvalidLine("resultados/flights_errors.csv", headerLine, tempBuffer);
            *errorsFlag = 1;
            parsed_fields_free(pf);
            continue;
        }

        // --- STRUCT CREATION ---
        Flight *data = g_new0(Flight, 1);

        data->id = g_strdup(fields[0]);
        data->departure = sched_dep;
        data->actual_departure = act_dep;
        data->arrival = sched_arr;
        data->actual_arrival = act_arr;
        data->status = parse_status_string(fields[6]);
        data->origin = g_strdup(fields[7]);
        data->destination = g_strdup(fields[8]);
        data->aircraft = g_strdup(fields[9]);
        data->airline = g_strdup(fields[10]);

        g_hash_table_insert(flightsTable, data->id, data);

        parsed_fields_free(pf);
    }

    g_free(headerLine);
    free(line);
    fclose(flights);
    return flightsTable;
}