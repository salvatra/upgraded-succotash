#include "io/manager.h"
#include <stdio.h>
#include <glib.h>
#include "core/time_utils.h"
#include "core/utils.h"
#include "core/statistics.h"
#include "core/internal/dataset_internal.h"
#include "entities/access/aircrafts_access.h"
#include "entities/access/airports_access.h"
#include "entities/access/flights_access.h"
#include "entities/access/passengers_access.h"
#include "entities/access/reservations_access.h"

void loadAllDatasets(Dataset *ds, int *errorsFlag, const char *filePath, gboolean enable_timing)
{
    GTimer *timer = NULL;
    gdouble elapsed = 0;
    gboolean success;
    guint size;

    ds->airportCodes = g_ptr_array_new_with_free_func(g_free);
    ds->aircraftManufacturers = g_ptr_array_new_with_free_func(g_free);
    ds->nationalities = g_ptr_array_new_with_free_func(g_free);

    if (enable_timing)
        timer = g_timer_new();

    char pathAC[256];
    snprintf(pathAC, sizeof(pathAC), "%s/aircrafts.csv", filePath);
    ds->aircrafts = readAircrafts(pathAC, errorsFlag, ds->aircraftManufacturers);

    if (enable_timing)
    {
        elapsed = g_timer_elapsed(timer, NULL);
        g_timer_destroy(timer);
        success = (ds->aircrafts != NULL);
        size = success ? g_hash_table_size(ds->aircrafts) : 0;
        if (success)
            printf("Aircrafts loaded: %u (%.3f seconds)\n", size, elapsed);
        else
            printf("Failed to load aircrafts.csv (%.3f seconds)\n", elapsed);
    }

    if (enable_timing)
        timer = g_timer_new();

    char pathF[256];
    snprintf(pathF, sizeof(pathF), "%s/flights.csv", filePath);
    ds->flights = readFlights(pathF, errorsFlag, ds->aircrafts);

    if (enable_timing)
    {
        elapsed = g_timer_elapsed(timer, NULL);
        g_timer_destroy(timer);
        success = (ds->flights != NULL);
        size = success ? g_hash_table_size(ds->flights) : 0;
        if (success)
            printf("Flights loaded: %u (%.3f seconds)\n", size, elapsed);
        else
            printf("Failed to load flights.csv (%.3f seconds)\n", elapsed);
    }

    if (enable_timing)
        timer = g_timer_new();

    char pathP[256];
    snprintf(pathP, sizeof(pathP), "%s/passengers.csv", filePath);
    ds->passengers = readPassengers(pathP, errorsFlag, ds->nationalities);

    if (enable_timing)
    {
        elapsed = g_timer_elapsed(timer, NULL);
        g_timer_destroy(timer);
        success = (ds->passengers != NULL);
        size = success ? g_hash_table_size(ds->passengers) : 0;
        if (success)
            printf("Passengers loaded: %u (%.3f seconds)\n", size, elapsed);
        else
            printf("Failed to load passengers.csv (%.3f seconds)\n", elapsed);
    }

    if (enable_timing)
        timer = g_timer_new();

    char pathAP[256];
    snprintf(pathAP, sizeof(pathAP), "%s/airports.csv", filePath);
    ds->airports = readAirports(pathAP, errorsFlag, ds->airportCodes);

    if (enable_timing)
    {
        elapsed = g_timer_elapsed(timer, NULL);
        g_timer_destroy(timer);
        success = (ds->airports != NULL);
        size = success ? g_hash_table_size(ds->airports) : 0;
        if (success)
            printf("Airports loaded: %u (%.3f seconds)\n", size, elapsed);
        else
            printf("Failed to load airports.csv (%.3f seconds)\n", elapsed);
    }

    if (enable_timing)
        timer = g_timer_new();

    char pathR[256];
    snprintf(pathR, sizeof(pathR), "%s/reservations.csv", filePath);
    ds->reservations = readReservations(pathR, ds->passengers, ds->flights, errorsFlag);

    if (enable_timing)
    {
        elapsed = g_timer_elapsed(timer, NULL);
        g_timer_destroy(timer);
        success = (ds->reservations != NULL);
        size = success ? g_hash_table_size(ds->reservations) : 0;
        if (success)
            printf("Reservations loaded: %u (%.3f seconds)\n", size, elapsed);
        else
            printf("Failed to load reservations.csv (%.3f seconds)\n", elapsed);
    }

    ds->airportStats = calculate_airport_traffic(ds->reservations, ds->flights);
    if (!ds->airportStats)
    {
        ds->airportStats = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, freeAirportPassengerStats);
    }

    if (ds->airportCodes)
    {
        g_ptr_array_sort(ds->airportCodes, (GCompareFunc)strcmp);
    }
    if (ds->nationalities)
    {
        g_ptr_array_sort(ds->nationalities, (GCompareFunc)strcmp);
    }
}