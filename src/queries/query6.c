#include "queries/query6.h"

#include <ctype.h>
#include <string.h>
#include <stdio.h>


// --------------------- Structs for Query 6 ---------------------
typedef struct {
    GHashTable *airportCounts; // airport -> number of passengers of that nationality
} NationalityData;

static void freeNationalityData(gpointer data) {
    NationalityData *nd = data;
    if (!nd) return;

    g_hash_table_destroy(nd->airportCounts);
    g_free(nd);
}

// --------------------- Prepare data for Query 6 ---------------------
// Precomputes nationality -> (airport -> passenger count)
GHashTable* prepareNationalityData(const GHashTable *passengers,
                                   const GHashTable *reservations,
                                   const GHashTable *flights) {
    // nationality -> NationalityData
    GHashTable *natTable = g_hash_table_new_full(
        g_str_hash, g_str_equal, g_free, freeNationalityData
    );

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, (GHashTable *)reservations);

    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Reservation *r = value;
        if (!r) continue;

        int doc = getReservationDocumentNo(r);
        Passenger *p = g_hash_table_lookup((GHashTable *)passengers, GINT_TO_POINTER(doc));
        if (!p) continue;

        const char *nat = getPassengerNationality(p);
        if (!nat) continue;

        // Get or create NationalityData
        NationalityData *nd = g_hash_table_lookup(natTable, nat);
        if (!nd) {
            nd = g_new0(NationalityData, 1);
            nd->airportCounts = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
            g_hash_table_insert(natTable, g_strdup(nat), nd);
        }

        gchar **flightIds = getReservationFlightIds(r);
        if (!flightIds) continue;

        for (int i = 0; flightIds[i]; i++) {
            Flight *f = g_hash_table_lookup((GHashTable *)flights, flightIds[i]);
            if (!f) continue;
            if (strcmp(getFlightStatus(f), "Cancelled") == 0) continue;

            const char *dest = getFlightDestination(f);
            if (!dest) continue;

            // Incrementa contagem do aeroporto
            gpointer countPtr = g_hash_table_lookup(nd->airportCounts, dest);
            int count = countPtr ? GPOINTER_TO_INT(countPtr) : 0;
            g_hash_table_replace(nd->airportCounts, g_strdup(dest), GINT_TO_POINTER(count + 1));
        }
    }

    return natTable;
}


// --------------------- Query 6 using precomputed data ---------------------
int query_Q6(GHashTable *natTable, const char *nationality, FILE *output, int isSpecial) {
    char sep = isSpecial ? '=' : ';';

    NationalityData *nd = g_hash_table_lookup(natTable, nationality);
    if (!nd) {
        return 0;
    }

    char *bestAirport = NULL;
    int bestCount = 0;

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, nd->airportCounts);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        char *airport = key;
        int count = GPOINTER_TO_INT(value);

        if (count > bestCount || (count == bestCount && (!bestAirport || strcmp(airport, bestAirport) < 0))) {
            bestAirport = airport;
            bestCount = count;
        }
    }

    if (bestAirport) {
        fprintf(output, "%s%c%d\n", bestAirport, sep, bestCount);
        return 1;
    } else {
        return 0;
    }
}


/* A Less eficient query 6
// --------------------- Query Q6 ---------------------
void query_Q6(const GHashTable *passengers,
              const GHashTable *reservations,
              const GHashTable *flights,
              const char *nationality,
              FILE *output,
              int isSpecial) {

    char sep = isSpecial ? '=' : ';';

    if (!nationality || nationality[0] == '\0') {
        fprintf(output, "\n");
        return;
    }

    // Hash table airport -> counts
    GHashTable *airportCounts = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

    // Hash table passagers+destination because duplicates
    GHashTable *seen = g_hash_table_new_full(hash_passenger_airport, equal_passenger_airport, g_free, NULL);

    GHashTableIter iter;
    gpointer key, value;

    // Iterates on reservations
    g_hash_table_iter_init(&iter, (GHashTable *)reservations);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        Reservation *r = value;
        if (!r) continue;

        int doc = getReservationDocumentNo(r);
        Passenger *p = g_hash_table_lookup((GHashTable *)passengers, GINT_TO_POINTER(doc));
        if (!p) continue;

        const char *nat = getPassengerNationality(p);
        if (!nat || strcmp(nat, nationality) != 0)
            continue;

        gchar **flightIds = getReservationFlightIds(r);
        if (!flightIds) continue;

        for (int i = 0; flightIds[i]; i++) {
            Flight *f = g_hash_table_lookup((GHashTable *)flights, flightIds[i]);
            if (!f) continue;
            if (strcmp(getFlightStatus(f), "Cancelled") == 0) continue;

            const char *dest = getFlightDestination(f);
            if (!dest) continue;

            // Checks if passenger already counted for this airport
            PassengerAirportKey *k = g_new(PassengerAirportKey, 1);
            k->doc = doc;
            k->airport = dest; 

            if (!g_hash_table_lookup(seen, k)) {
                g_hash_table_insert(seen, k, GINT_TO_POINTER(1));

                // Increments counts of each airport
                gpointer countPtr = g_hash_table_lookup(airportCounts, dest);
                int count = countPtr ? GPOINTER_TO_INT(countPtr) : 0;
                g_hash_table_replace(airportCounts, g_strdup(dest), GINT_TO_POINTER(count + 1));
            } else {
                g_free(k); 
            }
        }
    }

    g_hash_table_destroy(seen);

    //Find the airport with the most passengers
    char *bestAirport = NULL;
    int bestCount = 0;

    g_hash_table_iter_init(&iter, airportCounts);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        char *airport = key;
        int count = GPOINTER_TO_INT(value);

        if (count > bestCount || (count == bestCount && (!bestAirport || strcmp(airport, bestAirport) < 0))) {
            bestAirport = airport;
            bestCount = count;
        }
    }

    if (bestAirport)
        fprintf(output, "%s%c%d\n", bestAirport, sep, bestCount);
    else
        fprintf(output, "\n");

    g_hash_table_destroy(airportCounts);
}

*/
