#include "core/dataset.h"
#include "core/dataset_loader.h"
#include "core/statistics.h"
#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include "entities/access/aircrafts_access.h"
#include "entities/access/airports_access.h"
#include "entities/access/flights_access.h"
#include "entities/access/passengers_access.h"
#include "entities/access/reservations_access.h"

// --- Private Internal Structure ---
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

// --- Iterator Structures ---
struct dataset_iter
{
  GHashTableIter iter;
};

struct dataset_string_iter
{
  GPtrArray *array;
  guint index;
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

// --- Loader API Implementation ---

void dataset_set_flights(Dataset *ds, GHashTable *flights)
{
  if (ds)
    ds->flights = flights;
}
void dataset_set_passengers(Dataset *ds, GHashTable *passengers)
{
  if (ds)
    ds->passengers = passengers;
}
void dataset_set_airports(Dataset *ds, GHashTable *airports)
{
  if (ds)
    ds->airports = airports;
}
void dataset_set_aircrafts(Dataset *ds, GHashTable *aircrafts)
{
  if (ds)
    ds->aircrafts = aircrafts;
}
void dataset_set_reservations(Dataset *ds, GHashTable *reservations)
{
  if (ds)
    ds->reservations = reservations;
}
void dataset_set_airport_stats(Dataset *ds, GHashTable *stats)
{
  if (ds)
    ds->airportStats = stats;
}
void dataset_set_airport_codes(Dataset *ds, GPtrArray *codes)
{
  if (ds)
    ds->airportCodes = codes;
}
void dataset_set_aircraft_manufacturers(Dataset *ds, GPtrArray *manufacturers)
{
  if (ds)
    ds->aircraftManufacturers = manufacturers;
}
void dataset_set_nationalities(Dataset *ds, GPtrArray *nationalities)
{
  if (ds)
    ds->nationalities = nationalities;
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

// --- Entity Iterators ---
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

// --- String Iterators ---

static DatasetStringIterator *string_iterator_new(GPtrArray *array)
{
  if (!array)
    return NULL;
  DatasetStringIterator *it = g_new0(DatasetStringIterator, 1);
  it->array = array;
  it->index = 0;
  return it;
}

DatasetStringIterator *dataset_airport_codes_iter_new(Dataset *ds) { return string_iterator_new(ds->airportCodes); }
DatasetStringIterator *dataset_aircraft_manufacturers_iter_new(Dataset *ds) { return string_iterator_new(ds->aircraftManufacturers); }
DatasetStringIterator *dataset_nationalities_iter_new(Dataset *ds) { return string_iterator_new(ds->nationalities); }

const char *dataset_string_iter_next(DatasetStringIterator *it)
{
  if (it && it->array && it->index < it->array->len)
  {
    return (const char *)g_ptr_array_index(it->array, it->index++);
  }
  return NULL;
}

void dataset_string_iter_free(DatasetStringIterator *it)
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