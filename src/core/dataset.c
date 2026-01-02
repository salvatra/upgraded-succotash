#include "core/time_utils.h"
#include <core/dataset.h>
#include <core/report.h>
#include <core/utils.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <validation.h>
#include <entities/aircrafts.h>
#include <entities/airports.h>
#include <entities/flights.h>
#include <entities/passengers.h>
#include <entities/reservations.h>

struct dataset
{
  GHashTable *flights;
  GHashTable *passengers;
  GHashTable *airports;
  GHashTable *aircrafts;
  GHashTable *reservations;
  GHashTable *airportStats;
  GPtrArray *airportCodes;
  GPtrArray *aircraftManufacturers;
  GPtrArray *nationalities;
};


Dataset *initDataset()
{
  Dataset *ds = g_new(Dataset, 1);
  ds->flights = NULL;
  ds->passengers = NULL;
  ds->airports = NULL;
  ds->aircrafts = NULL;
  ds->reservations = NULL;
  ds->airportCodes = NULL;
  ds->aircraftManufacturers = NULL;
  ds->nationalities = NULL;
  return ds;
}

void loadAllDatasets(Dataset *ds, gint *errorsFlag, const char *filePath,
                     gboolean enable_timing)
{
  GTimer *timer = NULL;
  gdouble elapsed;
  gboolean success;
  guint size;

  ds->airportCodes = g_ptr_array_new_with_free_func(g_free);
  ds->aircraftManufacturers = g_ptr_array_new_with_free_func(g_free);
  ds->nationalities = g_ptr_array_new_with_free_func(g_free);

  if (enable_timing)
    timer = startTimer();
  char pathAC[128];
  snprintf(pathAC, sizeof(pathAC), "%s/aircrafts.csv", filePath);
  ds->aircrafts = readAircrafts(pathAC, errorsFlag, ds->aircraftManufacturers);
  if (enable_timing)
  {
    elapsed = stopTimer(timer);
    success = (ds->aircrafts != NULL);
    size = success ? g_hash_table_size(ds->aircrafts) : 0;
    if (success)
      printf("Aircrafts loaded: %u (%.3f seconds)\n", size, elapsed);
    else
      printf("Failed to load aircrafts.csv (%.3f seconds)\n", elapsed);
  }

  if (enable_timing)
    timer = startTimer();
  char pathF[128];
  snprintf(pathF, sizeof(pathF), "%s/flights.csv", filePath);
  ds->flights = readFlights(pathF, errorsFlag, ds->aircrafts);
  if (enable_timing)
  {
    elapsed = stopTimer(timer);
    success = (ds->flights != NULL);
    size = success ? g_hash_table_size(ds->flights) : 0;
    if (success)
      printf("Flights loaded: %u (%.3f seconds)\n", size, elapsed);
    else
      printf("Failed to load flights.csv (%.3f seconds)\n", elapsed);
  }

  if (enable_timing)
    timer = startTimer();
  char pathP[128];
  snprintf(pathP, sizeof(pathP), "%s/passengers.csv", filePath);
  ds->passengers = readPassengers(pathP, errorsFlag, ds->nationalities);
  if (enable_timing)
  {
    elapsed = stopTimer(timer);
    success = (ds->passengers != NULL);
    size = success ? g_hash_table_size(ds->passengers) : 0;
    if (success)
      printf("Passengers loaded: %u (%.3f seconds)\n", size, elapsed);
    else
      printf("Failed to load passengers.csv (%.3f seconds)\n", elapsed);
  }

  if (enable_timing)
    timer = startTimer();
  char pathAP[128];
  snprintf(pathAP, sizeof(pathAP), "%s/airports.csv", filePath);
  ds->airports = readAirports(pathAP, errorsFlag, ds->airportCodes);
  if (enable_timing)
  {
    elapsed = stopTimer(timer);
    success = (ds->airports != NULL);
    size = success ? g_hash_table_size(ds->airports) : 0;
    if (success)
      printf("Airports loaded: %u (%.3f seconds)\n", size, elapsed);
    else
      printf("Failed to load airports.csv (%.3f seconds)\n", elapsed);
  }

  if (enable_timing)
    timer = startTimer();
  char pathR[128];
  snprintf(pathR, sizeof(pathR), "%s/reservations.csv", filePath);
  ds->reservations =
      readReservations(pathR, ds->passengers, ds->flights, errorsFlag);
  if (enable_timing)
  {
    elapsed = stopTimer(timer);
    success = (ds->reservations != NULL);
    size = success ? g_hash_table_size(ds->reservations) : 0;
    if (success)
      printf("Reservations loaded: %u (%.3f seconds)\n", size, elapsed);
    else
      printf("Failed to load reservations.csv (%.3f seconds)\n", elapsed);
  }
  ds->airportStats = buildAirportPassengerStats(
      ds->reservations, ds->flights);

  if (ds->airportCodes)
  {
    g_ptr_array_sort(ds->airportCodes, (GCompareFunc)strcmp);
  }
  if (ds->nationalities)
  {
    g_ptr_array_sort(ds->nationalities, (GCompareFunc)strcmp);
  }
}

void cleanupDataset(Dataset *ds)
{
  if (ds->flights)
    g_hash_table_destroy(ds->flights);
  if (ds->passengers)
    g_hash_table_destroy(ds->passengers);
  if (ds->airports)
    g_hash_table_destroy(ds->airports);
  if (ds->aircrafts)
    g_hash_table_destroy(ds->aircrafts);
  if (ds->reservations)
    g_hash_table_destroy(ds->reservations);
  if (ds->airportStats)
    g_hash_table_destroy(ds->airportStats);
  if (ds->airportCodes)
    g_ptr_array_free(ds->airportCodes, TRUE);
  if (ds->aircraftManufacturers)
    g_ptr_array_free(ds->aircraftManufacturers, TRUE);
  if (ds->nationalities)
    g_ptr_array_free(ds->nationalities, TRUE);
  g_free(ds);
}

const GHashTable *getDatasetFlights(const Dataset *ds) { return ds->flights; }

const GHashTable *getDatasetPassengers(const Dataset *ds) { return ds->passengers; }

const GHashTable *getDatasetAirports(const Dataset *ds) { return ds->airports; }

const GHashTable *getDatasetAircrafts(const Dataset *ds) { return ds->aircrafts; }

const GHashTable *getDatasetReservations(const Dataset *ds)
{
  return ds->reservations;
}

const GHashTable *getDatasetAiportStats(const Dataset *ds)
{
  return ds->airportStats;
}

// --------------------------------------------------

const Flight *dataset_get_flight(const Dataset *ds, const char *id)
{
  if (!ds || !ds->flights || !id)
    return NULL;
  return getFlight(id, ds->flights);
}

const Airport *dataset_get_airport(const Dataset *ds, const char *code)
{
  if (!ds || !ds->airports || !code)
    return NULL;
  return getAirport(code, ds->airports);
}

const Aircraft *dataset_get_aircraft(const Dataset *ds, const char *id)
{
  if (!ds || !ds->aircrafts || !id)
    return NULL;
  return getAircraft(id, ds->aircrafts);
}

const Passenger *dataset_get_passenger(const Dataset *ds, int id)
{
  if (!ds || !ds->passengers)
    return NULL;
  return getPassenger(id, ds->passengers);
}

const Reservation *dataset_get_reservation(const Dataset *ds, const char *id)
{
  if (!ds || !ds->reservations || !id)
    return NULL;
  return getReservation(id, ds->reservations);
}

// --------------------------------------------------


GPtrArray *get_dataset_airport_codes(Dataset *ds) { return ds->airportCodes; }

GPtrArray *get_dataset_aircraft_manufacturers(Dataset *ds)
{
  return ds->aircraftManufacturers;
}

GPtrArray *get_dataset_nationalities(Dataset *ds) { return ds->nationalities; }