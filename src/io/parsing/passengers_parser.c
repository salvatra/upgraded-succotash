#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "entities/access/passengers_access.h"
#include "entities/internal/passengers_internal.h"
#include "io/parsing/parser_utils.h"
#include "io/validation/validation_utils.h"
#include "io/validation/passengers_validator.h"
#include "core/time_utils.h"

GHashTable *readPassengers(const char *filename, int *errorsFlag, GPtrArray *nationalities_list)
{
    FILE *passengers = fopen(filename, "r");
    if (!passengers)
        return NULL;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    if ((read = getline(&line, &len, passengers)) == -1)
    {
        free(line);
        fclose(passengers);
        return NULL;
    }
    g_strchomp(line);
    char *headerLine = g_strdup(line);

    GHashTable *passengersTable =
        g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freePassenger);

    GHashTable *seen_nationalities = NULL;
    if (nationalities_list)
    {
        seen_nationalities = g_hash_table_new(g_str_hash, g_str_equal);
    }

    while ((read = getline(&line, &len, passengers)) != -1)
    {
        g_strchomp(line);

        ParsedPassengerF *pf = parsePassengerLineRaw(line);

        if (!parsed_passenger_ok(pf))
        {
            parsed_passenger_free(pf);
            continue;
        }

        const gchar *fields[10];
        for (int i = 0; i < 10; i++)
            fields[i] = parsed_passenger_get(pf, i);

        gboolean invalid = FALSE;
        time_t dob_t = 0;

        if (!checkDocumentNo(fields[0]))
            invalid = TRUE;

        if (!invalid && !checkPassangerGender(fields[5]))
            invalid = TRUE;

        if (!invalid)
        {
            dob_t = parse_unix_date(fields[3], NULL);
            if (!checkDate(dob_t))
                invalid = TRUE;
        }

        if (!invalid && !checkEmail(fields[6]))
            invalid = TRUE;

        if (invalid)
        {
            logInvalidLine("resultados/passengers_errors.csv",
                           headerLine,
                           parsed_passenger_line(pf));
            *errorsFlag = 1;
            parsed_passenger_free(pf);
            continue;
        }


        Passenger *data = g_new0(Passenger, 1);

        data->document_number = atoi(fields[0]);
        data->first_name = g_strdup(fields[1]);
        data->last_name = g_strdup(fields[2]);
        data->dob = dob_t;
        data->nationality = g_strdup(fields[4]);
        data->gender = fields[5][0];

        // --- Insert ---
        g_hash_table_insert(passengersTable,
                            GINT_TO_POINTER(data->document_number), data);

        // --- Nationality Autocomplete Logic ---
        if (seen_nationalities && !g_hash_table_contains(seen_nationalities, data->nationality))
        {
            g_ptr_array_add(nationalities_list, g_strdup(data->nationality));
            g_hash_table_add(seen_nationalities, data->nationality);
        }

        parsed_passenger_free(pf);
    }

    if (seen_nationalities)
    {
        g_hash_table_destroy(seen_nationalities);
    }

    g_free(headerLine);
    free(line);
    fclose(passengers);

    return passengersTable;
}