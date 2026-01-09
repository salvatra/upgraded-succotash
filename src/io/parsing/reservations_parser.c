#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <glib.h>

#include "entities/access/reservations_access.h"
#include "entities/internal/reservations_internal.h"
#include "entities/access/flights_access.h"
#include "io/parsing/parser_utils.h"
#include "validation.h"

GHashTable *readReservations(const char *filename,
                             GHashTable *passengersTable,
                             GHashTable *flightsTable,
                             int *errorsFlag)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        return NULL;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    if ((read = getline(&line, &len, f)) == -1)
    {
        free(line);
        fclose(f);
        return NULL;
    }
    g_strchomp(line);
    char *headerLine = g_strdup(line);

    GHashTable *table =
        g_hash_table_new_full(g_str_hash, g_str_equal, g_free, freeReservations);

    while ((read = getline(&line, &len, f)) != -1)
    {
        g_strchomp(line);

        ParsedReservationF *pr = parseReservationLineRaw(line);
        if (!parsed_reservation_ok(pr))
        {
            parsed_reservation_free(pr);
            continue;
        }

        const gchar *fields[8];
        for (int i = 0; i < 8; i++)
            fields[i] = parsed_reservation_get(pr, i);

        gboolean invalid = FALSE;

        /* --- Step 1: Reservation ID --- */
        if (!checkReservationId(fields[0]))
            invalid = TRUE;

        /* --- Step 2: Document number --- */
        int docNo = 0;
        // Uses checkDocumentNo from passengers_validation.h
        if (!invalid && !checkDocumentNo(fields[2]))
            invalid = TRUE;
        else
        {
            docNo = atoi(fields[2]);
        }

        /* --- Step 3: Passenger exists --- */
        if (!invalid &&
            !g_hash_table_contains(passengersTable, GINT_TO_POINTER(docNo)))
            invalid = TRUE;

        /* --- Step 4: Flight IDs (list validation) --- */
        gchar **flights = NULL;

        if (!invalid)
        {
            const char *s = fields[1];
            size_t l = strlen(s);
            if (l < 2 || s[0] != '[' || s[l - 1] != ']')
            {
                invalid = TRUE;
            }
        }

        if (!invalid)
        {
            flights = parseFlightIds(fields[1]);
            if (!flights || !flights[0])
            {
                invalid = TRUE;
            }
            else
            {
                int count = 0;
                while (flights[count])
                    count++;

                if (count == 0 || count > 2)
                {
                    invalid = TRUE;
                }
                else if (count == 1)
                {
                    if (!g_hash_table_contains(flightsTable, flights[0]))
                        invalid = TRUE;
                }
                else
                {
                    const Flight *f1 = getFlight(flights[0], flightsTable);
                    const Flight *f2 = getFlight(flights[1], flightsTable);

                    if (!f1 || !f2 ||
                        g_strcmp0(getFlightDestination(f1),
                                  getFlightOrigin(f2)) != 0)
                        invalid = TRUE;
                }

                if (invalid)
                {
                    g_strfreev(flights);
                    flights = NULL;
                }
            }
        }

        if (invalid)
        {
            logInvalidLine("resultados/reservations_errors.csv",
                           headerLine,
                           parsed_reservation_line(pr));
            *errorsFlag = 1;
            parsed_reservation_free(pr);
            continue;
        }

        /* --- Create reservation --- */
        Reservation *data = g_new0(Reservation, 1);
        data->reservation_id = g_strdup(fields[0]);
        data->flight_ids = flights;
        data->document_no = docNo;
        data->price = atof(fields[4]);

        g_hash_table_insert(table, g_strdup(fields[0]), data);
        parsed_reservation_free(pr);
    }

    g_free(headerLine);
    free(line);
    fclose(f);
    return table;
}