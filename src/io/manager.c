#include "io/manager.h"
#include <stdio.h>
#include <glib.h>
#include "core/time_utils.h"
#include "core/utils.h"
#include "core/statistics.h"
#include "core/dataset_loader.h" // Uses the new Loader API
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

    // Create auxiliary arrays locally
    GPtrArray *airportCodes = g_ptr_array_new_with_free_func(g_free);
    GPtrArray *aircraftManufacturers = g_ptr_array_new_with_free_func(g_free);
    GPtrArray *nationalities = g_ptr_array_new_with_free_func(g_free);

    if (enable_timing)
        timer = g_timer_new();

    char pathAC[256];
    snprintf(pathAC, sizeof(pathAC), "%s/aircrafts.csv", filePath);
    // Pass local 'aircraftManufacturers' to the parser
    GHashTable *aircrafts = readAircrafts(pathAC, errorsFlag, aircraftManufacturers);

    // Transfer ownership to Dataset
    dataset_set_aircrafts(ds, aircrafts);
    dataset_set_aircraft_manufacturers(ds, aircraftManufacturers);

    if (enable_timing)
    {
        elapsed = g_timer_elapsed(timer, NULL);
        g_timer_destroy(timer);
        success = (aircrafts != NULL);
        size = success ? g_hash_table_size(aircrafts) : 0;
        if (success)
            printf("Aircrafts loaded: %u (%.3f seconds)\n", size, elapsed);
        else
            printf("Failed to load aircrafts.csv (%.3f seconds)\n", elapsed);
    }

    if (enable_timing)
        timer = g_timer_new();

    char pathF[256];
    snprintf(pathF, sizeof(pathF), "%s/flights.csv", filePath);
    // Pass local 'aircrafts' table
    GHashTable *flights = readFlights(pathF, errorsFlag, aircrafts);
    dataset_set_flights(ds, flights);

    if (enable_timing)
    {
        elapsed = g_timer_elapsed(timer, NULL);
        g_timer_destroy(timer);
        success = (flights != NULL);
        size = success ? g_hash_table_size(flights) : 0;
        if (success)
            printf("Flights loaded: %u (%.3f seconds)\n", size, elapsed);
        else
            printf("Failed to load flights.csv (%.3f seconds)\n", elapsed);
    }

    if (enable_timing)
        timer = g_timer_new();

    char pathP[256];
    snprintf(pathP, sizeof(pathP), "%s/passengers.csv", filePath);
    // Pass local 'nationalities'
    GHashTable *passengers = readPassengers(pathP, errorsFlag, nationalities);
    dataset_set_passengers(ds, passengers);
    dataset_set_nationalities(ds, nationalities);

    if (enable_timing)
    {
        elapsed = g_timer_elapsed(timer, NULL);
        g_timer_destroy(timer);
        success = (passengers != NULL);
        size = success ? g_hash_table_size(passengers) : 0;
        if (success)
            printf("Passengers loaded: %u (%.3f seconds)\n", size, elapsed);
        else
            printf("Failed to load passengers.csv (%.3f seconds)\n", elapsed);
    }

    if (enable_timing)
        timer = g_timer_new();

    char pathAP[256];
    snprintf(pathAP, sizeof(pathAP), "%s/airports.csv", filePath);
    // Pass local 'airportCodes'
    GHashTable *airports = readAirports(pathAP, errorsFlag, airportCodes);
    dataset_set_airports(ds, airports);
    dataset_set_airport_codes(ds, airportCodes);

    if (enable_timing)
    {
        elapsed = g_timer_elapsed(timer, NULL);
        g_timer_destroy(timer);
        success = (airports != NULL);
        size = success ? g_hash_table_size(airports) : 0;
        if (success)
            printf("Airports loaded: %u (%.3f seconds)\n", size, elapsed);
        else
            printf("Failed to load airports.csv (%.3f seconds)\n", elapsed);
    }

    if (enable_timing)
        timer = g_timer_new();

    char pathR[256];
    snprintf(pathR, sizeof(pathR), "%s/reservations.csv", filePath);
    // Pass local 'passengers' and 'flights' tables
    GHashTable *reservations = readReservations(pathR, passengers, flights, errorsFlag);
    dataset_set_reservations(ds, reservations);

    if (enable_timing)
    {
        elapsed = g_timer_elapsed(timer, NULL);
        g_timer_destroy(timer);
        success = (reservations != NULL);
        size = success ? g_hash_table_size(reservations) : 0;
        if (success)
            printf("Reservations loaded: %u (%.3f seconds)\n", size, elapsed);
        else
            printf("Failed to load reservations.csv (%.3f seconds)\n", elapsed);
    }

    // Calculate stats using local variables before setting
    GHashTable *airportStats = calculate_airport_traffic(reservations, flights);
    if (!airportStats)
    {
        airportStats = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, freeAirportPassengerStats);
    }
    dataset_set_airport_stats(ds, airportStats);

    if (airportCodes)
    {
        g_ptr_array_sort(airportCodes, (GCompareFunc)strcmp);
    }
    if (nationalities)
    {
        g_ptr_array_sort(nationalities, (GCompareFunc)strcmp);
    }
}