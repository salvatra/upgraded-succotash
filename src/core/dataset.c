#include "core/dataset.h"
#include "core/internal/dataset_internal.h"
#include "core/statistics.h"

#include <glib.h>
#include <stdlib.h>
#include <string.h>

// --- EXPLICIT INCLUDES FOR ACCESSORS ---
// CRITICAL: These headers declare functions like getAirport, getFlight, etc.
#include "entities/access/aircrafts_access.h"
#include "entities/access/airports_access.h"
#include "entities/access/flights_access.h"
#include "entities/access/passengers_access.h"
#include "entities/access/reservations_access.h"

struct dataset_iter
{
  GHashTableIter iter;
};

Dataset *initDataset()
{
  Dataset *ds = g_new0(Dataset, 1);
  return ds;
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

// --- Counters ---
int dataset_get_flight_count(const Dataset *ds)
{
  return (ds && ds->flights) ? g_hash_table_size(ds->flights) : 0;
}
int dataset_get_aircraft_count(const Dataset *ds)
{
  return (ds && ds->aircrafts) ? g_hash_table_size(ds->aircrafts) : 0;
}
int dataset_get_passenger_count(const Dataset *ds)
{
  return (ds && ds->passengers) ? g_hash_table_size(ds->passengers) : 0;
}
int dataset_get_reservation_count(const Dataset *ds)
{
  return (ds && ds->reservations) ? g_hash_table_size(ds->reservations) : 0;
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
  // This line caused the error implicitly if airports_access.h was missing
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