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
#include <core/statistics.h>

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

struct dataset_iter
{
  GHashTableIter iter;
};

Dataset *initDataset()
{
  Dataset *ds = g_new(Dataset, 1);
  ds->flights = NULL;
  ds->passengers = NULL;
  ds->airports = NULL;
  ds->aircrafts = NULL;
  ds->reservations = NULL;
  ds->airportStats = NULL;
  ds->airportCodes = NULL;
  ds->aircraftManufacturers = NULL;
  ds->nationalities = NULL;
  return ds;
}

// --- Counters ---
int dataset_get_flight_count(const Dataset *ds)
{
  return ds && ds->flights ? g_hash_table_size(ds->flights) : 0;
}
int dataset_get_aircraft_count(const Dataset *ds)
{
  return ds && ds->aircrafts ? g_hash_table_size(ds->aircrafts) : 0;
}
int dataset_get_passenger_count(const Dataset *ds)
{
  return ds && ds->passengers ? g_hash_table_size(ds->passengers) : 0;
}
int dataset_get_reservation_count(const Dataset *ds)
{
  return ds && ds->reservations ? g_hash_table_size(ds->reservations) : 0;
}

// --- Iterators ---
static DatasetIterator *iterator_new_from_table(GHashTable *table)
{
  if (!table)
    return NULL;
  DatasetIterator *it = g_new0(DatasetIterator, 1);
  g_hash_table_iter_init(&it->iter, table);
  return it;
}

DatasetIterator *dataset_flight_iterator_new(const Dataset *ds)
{
  return iterator_new_from_table(ds->flights);
}

DatasetIterator *dataset_aircraft_iterator_new(const Dataset *ds)
{
  return iterator_new_from_table(ds->aircrafts);
}

DatasetIterator *dataset_reservation_iterator_new(const Dataset *ds)
{
  return iterator_new_from_table(ds->reservations);
}

DatasetIterator *dataset_passenger_iterator_new(const Dataset *ds)
{
  return iterator_new_from_table(ds->passengers);
}

const void *dataset_iterator_next(DatasetIterator *it)
{
  if (!it)
    return NULL;
  gpointer key, value;
  if (g_hash_table_iter_next(&it->iter, &key, &value))
  {
    return (const void *)value;
  }
  return NULL;
}

void dataset_iterator_free(DatasetIterator *it)
{
  if (it)
    g_free(it);
}

// --- Loading stuff ---
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

void cleanupDataset(Dataset *ds)
{
  if (!ds)
    return;

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

// --- Accessors ---

const AirportPassengerStats *dataset_get_airport_stats(const Dataset *ds, const char *code)
{
  if (!ds || !ds->airportStats || !code)
    return NULL;
  return (const AirportPassengerStats *)g_hash_table_lookup(ds->airportStats, code);
}

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

// --- Other aux accessors ---
GPtrArray *get_dataset_airport_codes(Dataset *ds) { return ds->airportCodes; }
GPtrArray *get_dataset_aircraft_manufacturers(Dataset *ds) { return ds->aircraftManufacturers; }
GPtrArray *get_dataset_nationalities(Dataset *ds) { return ds->nationalities; }