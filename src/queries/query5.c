#include "queries/query5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// first test (non efficient)
/*
struct airline_delay {
    gchar *airline;
    guint delayed_count;
    double total_delay; 
};

static gint compare_airline_delay(gconstpointer a, gconstpointer b) {
    const struct airline_delay *pa = a;
    const struct airline_delay *pb = b;

    double avg_a = round((pa->total_delay / pa->delayed_count) * 1000.0) / 1000.0;
    double avg_b = round((pb->total_delay / pb->delayed_count) * 1000.0) / 1000.0;

    if (avg_a > avg_b) return -1;
    if (avg_a < avg_b) return 1;

    return g_strcmp0(pa->airline, pb->airline); // alphabetical tie-breaker
}

void query_Q5(GPtrArray *flightsArray, int N, FILE *output, int isSpecial) {
    char separator = isSpecial ? '=' : ';';

    GHashTable *table = g_hash_table_new_full(
        g_str_hash, g_str_equal, g_free, g_free
    );

    for (guint i = 0; i < flightsArray->len; i++) {
        Flight *f = g_ptr_array_index(flightsArray, i);
        if (!f) continue;

        if (strcmp(getFlightStatus(f), "Delayed") != 0) continue;

        const gchar *airline = getFlightAirline(f);
        if (!airline) continue;

        double delay_min = (double)(getFlightActualDeparture(f) - getFlightDeparture(f)) / 60.0;

        struct airline_delay *entry = g_hash_table_lookup(table, airline);
        if (!entry) {
            entry = g_new0(struct airline_delay, 1);
            entry->airline = g_strdup(airline);
            entry->delayed_count = 1;
            entry->total_delay = delay_min;
            g_hash_table_insert(table, g_strdup(airline), entry);
        } else {
            entry->delayed_count++;
            entry->total_delay += delay_min;
        }
    }

    GList *list = NULL;
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, table);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        list = g_list_prepend(list, value);
    }
    list = g_list_reverse(list); // maintains original order

    list = g_list_sort(list, compare_airline_delay);

    GList *l = list;
    guint count = 0;
    while (l && count < (guint)N) {
        struct airline_delay *entry = l->data;
        double avg_delay = round((entry->total_delay / entry->delayed_count) * 1000.0) / 1000.0;
        fprintf(output, "%s%c%u%c%.3f\n",
                entry->airline,
                separator,
                entry->delayed_count,
                separator,
                avg_delay);
        l = l->next;
        count++;
    }

    g_list_free(list);
    g_hash_table_destroy(table);
}
    */

// Auxiliary struct
typedef struct {
    gchar *airline;
    guint delayed_count;
    double total_delay;
    double avg_delay_rounded;
} AirlineDelayPrepared;

// Function to prepare airline delays to use on query 5
GList* prepareAirlineDelays(GPtrArray *flightsArray) {
    GHashTable *table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
    GList *list = NULL;

    for (guint i = 0; i < flightsArray->len; i++) {
        Flight *f = g_ptr_array_index(flightsArray, i);
        if (!f || strcmp(getFlightStatus(f), "Delayed") != 0) continue;

        const gchar *airline = getFlightAirline(f);
        if (!airline) continue;

        double delay_min = (double)(getFlightActualDeparture(f) - getFlightDeparture(f)) / 60.0;

        AirlineDelayPrepared *entry = g_hash_table_lookup(table, airline);
        if (!entry) {
            entry = g_new0(AirlineDelayPrepared, 1);
            entry->airline = g_strdup(airline);  
            entry->delayed_count = 1;
            entry->total_delay = delay_min;
            g_hash_table_insert(table, g_strdup(airline), entry); // duplicates keys
        } else {
            entry->delayed_count++;
            entry->total_delay += delay_min;
        }
    }

    // Moves the values to the list and calculates the average
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, table);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        AirlineDelayPrepared *entry = value;
        entry->avg_delay_rounded = round((entry->total_delay / entry->delayed_count) * 1000.0) / 1000.0;
        list = g_list_prepend(list, entry);
    }

    list = g_list_reverse(list);
    g_hash_table_destroy(table); 
    return list;
}

static gint compare_airline_delay(gconstpointer a, gconstpointer b) {
    const AirlineDelayPrepared *pa = a;
    const AirlineDelayPrepared *pb = b;

    if (pa->avg_delay_rounded > pb->avg_delay_rounded) return -1;
    if (pa->avg_delay_rounded < pb->avg_delay_rounded) return 1;

    return g_strcmp0(pa->airline, pb->airline);
}

int query5(GList *airlineDelays, int N, FILE *output, int isSpecial) {
    char separator = isSpecial ? '=' : ';';  // New syntax for queries

    // Copy of the list because process changes
    GList *listCopy = g_list_copy(airlineDelays);

    listCopy = g_list_sort(listCopy, (GCompareFunc)compare_airline_delay);

    guint count = 0;
    int printed = 0;
    for (GList *l = listCopy; l != NULL && count < (guint)N; l = l->next, count++) {
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


void freeAirlineDelays(GList *airlineDelays) {
    for (GList *l = airlineDelays; l != NULL; l = l->next) {
        AirlineDelayPrepared *entry = l->data;
        g_free(entry->airline);
        g_free(entry);
    }
    g_list_free(airlineDelays);
}
