#include "queries/query5.h"
#include "queries/query_module.h"
#include "core/dataset.h"
#include "entities/access/flights_access.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

typedef struct
{
    gchar *airline;
    guint delayed_count;
    double total_delay;
    double avg_delay_rounded;
} AirlineDelayPrepared;

GList *prepareAirlineDelays(GPtrArray *flightsArray)
{
    GHashTable *table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
    GList *list = NULL;

    for (guint i = 0; i < flightsArray->len; i++)
    {
        const Flight *f = g_ptr_array_index(flightsArray, i);

        if (!f || strcmp(getFlightStatus(f), "Delayed") != 0)
            continue;

        const gchar *airline = getFlightAirline(f);
        if (!airline)
            continue;

        double delay_min = (double)(getFlightActualDeparture(f) - getFlightDeparture(f)) / 60.0;

        AirlineDelayPrepared *entry = g_hash_table_lookup(table, airline);
        if (!entry)
        {
            entry = g_new0(AirlineDelayPrepared, 1);
            entry->airline = g_strdup(airline);
            entry->delayed_count = 1;
            entry->total_delay = delay_min;
            g_hash_table_insert(table, g_strdup(airline), entry);
        }
        else
        {
            entry->delayed_count++;
            entry->total_delay += delay_min;
        }
    }

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, table);
    while (g_hash_table_iter_next(&iter, &key, &value))
    {
        AirlineDelayPrepared *entry = value;
        entry->avg_delay_rounded = round((entry->total_delay / entry->delayed_count) * 1000.0) / 1000.0;
        list = g_list_prepend(list, entry);
    }

    list = g_list_reverse(list);
    g_hash_table_destroy(table);
    return list;
}

static gint compare_airline_delay(gconstpointer a, gconstpointer b)
{
    const AirlineDelayPrepared *pa = a;
    const AirlineDelayPrepared *pb = b;

    if (pa->avg_delay_rounded > pb->avg_delay_rounded)
        return -1;
    if (pa->avg_delay_rounded < pb->avg_delay_rounded)
        return 1;

    return g_strcmp0(pa->airline, pb->airline);
}

int query5(GList *airlineDelays, int N, FILE *output, int isSpecial)
{
    char separator = isSpecial ? '=' : ';';

    GList *listCopy = g_list_copy(airlineDelays);
    listCopy = g_list_sort(listCopy, (GCompareFunc)compare_airline_delay);

    guint count = 0;
    int printed = 0;
    for (GList *l = listCopy; l != NULL && count < (guint)N; l = l->next, count++)
    {
        AirlineDelayPrepared *entry = l->data;
        fprintf(output, "%s%c%u%c%.3f\n",
                entry->airline,
                separator,
                entry->delayed_count,
                separator,
                entry->avg_delay_rounded);
        printed++;
    }

    g_list_free(listCopy);
    return printed;
}

void freeAirlineDelays(GList *airlineDelays)
{
    for (GList *l = airlineDelays; l != NULL; l = l->next)
    {
        AirlineDelayPrepared *entry = l->data;
        g_free(entry->airline);
        g_free(entry);
    }
    g_list_free(airlineDelays);
}

static void *q5_init_wrapper(Dataset *ds)
{
    if (!ds)
        return NULL;

    GPtrArray *flights = g_ptr_array_new();
    DatasetIterator *it = dataset_flight_iterator_new(ds);
    const Flight *f;
    while ((f = dataset_iterator_next(it)) != NULL)
    {
        g_ptr_array_add(flights, (gpointer)f);
    }
    dataset_iterator_free(it);

    GList *delays_list = prepareAirlineDelays(flights);

    g_ptr_array_free(flights, TRUE);

    return (void *)delays_list;
}

static void q5_run_wrapper(void *ctx, Dataset *ds, char *arg1, char *arg2, int isSpecial, FILE *output)
{
    GList *delays_list = (GList *)ctx;
    (void)ds;
    (void)arg2;

    if (!arg1 || !*arg1)
    {
        fprintf(output, "\n");
        return;
    }

    int N = atoi(arg1);
    if (query5(delays_list, N, output, isSpecial) == 0)
    {
        fprintf(output, "\n");
    }
}

static void q5_destroy_wrapper(void *ctx)
{
    freeAirlineDelays((GList *)ctx);
}

QueryModule get_query5_module(void)
{
    QueryModule mod = {
        .id = 5,
        .init = q5_init_wrapper,
        .run = q5_run_wrapper,
        .destroy = q5_destroy_wrapper};
    return mod;
}