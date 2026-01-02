#include "queries/query6.h"
#include <core/dataset.h>
#include <entities/reservations.h>
#include <entities/passengers.h>
#include <entities/flights.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

typedef struct
{
    GHashTable *airportCounts;
} NationalityData;

static void freeNationalityData(gpointer data)
{
    NationalityData *nd = data;
    if (!nd)
        return;
    g_hash_table_destroy(nd->airportCounts);
    g_free(nd);
}

GHashTable *prepareNationalityData(const Dataset *ds)
{
    GHashTable *natTable = g_hash_table_new_full(
        g_str_hash, g_str_equal, g_free, freeNationalityData);

    DatasetIterator *it = dataset_reservation_iterator_new(ds);
    const Reservation *r;

    while ((r = (const Reservation *)dataset_iterator_next(it)) != NULL)
    {
        int doc = getReservationDocumentNo(r);

        const Passenger *p = dataset_get_passenger(ds, doc);
        if (!p)
            continue;

        const char *nat = getPassengerNationality(p);
        if (!nat)
            continue;

        NationalityData *nd = g_hash_table_lookup(natTable, nat);
        if (!nd)
        {
            nd = g_new0(NationalityData, 1);
            nd->airportCounts = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
            g_hash_table_insert(natTable, g_strdup(nat), nd);
        }

        gchar **flightIds = getReservationFlightIds(r);
        if (!flightIds)
            continue;

        for (int i = 0; flightIds[i]; i++)
        {
            const Flight *f = dataset_get_flight(ds, flightIds[i]);
            if (!f)
                continue;

            if (strcmp(getFlightStatus(f), "Cancelled") == 0)
                continue;

            const char *dest = getFlightDestination(f);
            if (!dest)
                continue;

            gpointer countPtr = g_hash_table_lookup(nd->airportCounts, dest);
            int count = countPtr ? GPOINTER_TO_INT(countPtr) : 0;
            g_hash_table_replace(nd->airportCounts, g_strdup(dest), GINT_TO_POINTER(count + 1));
        }
    }
    dataset_iterator_free(it);

    return natTable;
}

int query_Q6(GHashTable *natTable, const char *nationality, FILE *output, int isSpecial)
{
    char sep = isSpecial ? '=' : ';';
    NationalityData *nd = g_hash_table_lookup(natTable, nationality);
    if (!nd)
        return 0;

    char *bestAirport = NULL;
    int bestCount = 0;
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, nd->airportCounts);
    while (g_hash_table_iter_next(&iter, &key, &value))
    {
        char *airport = key;
        int count = GPOINTER_TO_INT(value);
        if (count > bestCount || (count == bestCount && (!bestAirport || strcmp(airport, bestAirport) < 0)))
        {
            bestAirport = airport;
            bestCount = count;
        }
    }
    if (bestAirport)
    {
        fprintf(output, "%s%c%d\n", bestAirport, sep, bestCount);
        return 1;
    }
    return 0;
}