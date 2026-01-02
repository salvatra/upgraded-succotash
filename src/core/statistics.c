#include <core/statistics.h>
#include <entities/reservations.h>
#include <entities/flights.h>
#include <string.h>
#include <stdio.h>

struct AiportPassengStats
{
    long arrivals;
    long departures;
};

void freeAirportPassengerStats(gpointer data)
{
    g_free(data);
}

static AirportPassengerStats *getOrCreateStats(GHashTable *stats, const char *code)
{
    AirportPassengerStats *s = g_hash_table_lookup(stats, code);
    if (!s)
    {
        s = g_new0(AirportPassengerStats, 1);
        g_hash_table_insert(stats, g_strdup(code), s);
    }
    return s;
}

GHashTable *calculate_airport_traffic(const GHashTable *reservations,
                                      const GHashTable *flights)
{

    if (!reservations || !flights)
    {
        return NULL;
    }

    GHashTable *stats = g_hash_table_new_full(g_str_hash, g_str_equal,
                                              g_free, freeAirportPassengerStats);

    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, (GHashTable *)reservations);

    while (g_hash_table_iter_next(&iter, &key, &value))
    {
        const Reservation *res = value;
        if (!res)
            continue;

        gchar **flightIds = getReservationFlightIds(res);
        if (!flightIds)
            continue;

        for (int i = 0; flightIds[i] != NULL; i++)
        {

            if (!flightIds[i] || strlen(flightIds[i]) == 0)
            {
                continue;
            }

            const Flight *flight = getFlight(flightIds[i], flights);
            if (!flight)
                continue;

            if (strcmp(getFlightStatus(flight), "Cancelled") == 0)
            {
                continue;
            }

            const char *orig = getFlightOrigin(flight);
            const char *dest = getFlightDestination(flight);

            if (orig)
                getOrCreateStats(stats, orig)->departures++;

            if (dest)
                getOrCreateStats(stats, dest)->arrivals++;
        }
    }

    return stats;
}

long getAirportArrivals(const AirportPassengerStats *s)
{
    return s ? s->arrivals : 0;
}

long getAirportDepartures(const AirportPassengerStats *s)
{
    return s ? s->departures : 0;
}