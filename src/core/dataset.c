#include "core/time_utils.h"
#include <core/dataset.h>
#include <core/report.h>
#include <core/utils.h>
#include <entities/flights.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <validation.h>

struct dataset {
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

Dataset *initDataset() {
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
                     gboolean enable_timing) {
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
  if (enable_timing) {
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
  if (enable_timing) {
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
  if (enable_timing) {
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
  if (enable_timing) {
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
  if (enable_timing) {
    elapsed = stopTimer(timer);
    success = (ds->reservations != NULL);
    size = success ? g_hash_table_size(ds->reservations) : 0;
    if (success)
      printf("Reservations loaded: %u (%.3f seconds)\n", size, elapsed);
    else
      printf("Failed to load reservations.csv (%.3f seconds)\n", elapsed);
  }
  ds->airportStats = buildAirportPassengerStats(
      ds->reservations, ds->flights); // Used because Query 1

  if (ds->airportCodes) {
    g_ptr_array_sort(ds->airportCodes, (GCompareFunc)strcmp);
  }
  if (ds->nationalities) {
    g_ptr_array_sort(ds->nationalities, (GCompareFunc)strcmp);
  }
}

void cleanupDataset(Dataset *ds) {
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

const GHashTable *getDatasetFlights(Dataset *ds) { return ds->flights; }

const GHashTable *getDatasetPassengers(Dataset *ds) { return ds->passengers; }

const GHashTable *getDatasetAirports(Dataset *ds) { return ds->airports; }

const GHashTable *getDatasetAircrafts(Dataset *ds) { return ds->aircrafts; }

const GHashTable *getDatasetReservations(Dataset *ds) {
  return ds->reservations;
}

const GHashTable *getDatasetAiportStats(Dataset *ds) {
  return ds->airportStats;
}
/*
gboolean validateDataset(Dataset *ds) {
  // flight.aircraft => aicraft.id
  int ok = TRUE;
  const gchar *flightsHeader =
      "\"flight id\",\"departure\",\"actual departure\",\"arrival\",\"actual "
      "arrival\",\"gate\",\"status\",\"origin\",\"destination\",\"aircraft\","
      "\"airline\",\"tracking url\"";
  if (!checkAircraftAsKey(ds->flights, ds->aircrafts, flightsHeader,
                          "resultados/flights_errors.csv")) {
    printf("1\n");
    ok = FALSE;
  }

  // reservations.docNo => passenger.docNo
  const gchar *reservationsHeader =
      "\"reservation id\",\"flight ids\",\"document "
      "number\",\"seat\",\"price\",\"extra luggage\",\"priority "
      "boarding\",\"qr code\"";
  if (!checkDocumentAsKey(ds->reservations, ds->passengers, reservationsHeader,
                          "resultados/reservations_errors.csv")) {
    printf("2\n");
    ok = FALSE;
  }

  // reservations.flightIds => flight1.destination == flight2.origin
  if (!checkFlightConnection(ds->reservations, ds->flights, reservationsHeader,
                             "resultados/reservations_errors.csv")) {
    printf("3\n");
    ok = FALSE;
  }

  return ok;
}
  */

struct datesinfo {
  GArray *distinctDates;
  GHashTable *dateSet; // to check in O(1) if date already exists in array
};

void freeDatesInfo(gpointer data) {
  DatesInfo *di = (DatesInfo *)data;
  g_array_free(di->distinctDates, TRUE);
  g_hash_table_destroy(di->dateSet);
  g_free(di);
}

GHashTable *getDatesTable(Dataset *ds) {
  // NOTE: Useful for Query 3
  //  Initialzing the FT once and using the same one for each Q3 iteration
  //  allows for reducing time spent on each individual Q3

  // 1. Load existing tables
  GHashTable *flights = ds->flights;

  // 2. Create an hash table to hold DatesInfo -> an array of all different
  // dates of a given airport and a set to quickly check if a new date is
  // already present in the array
  GHashTable *airportsDepartures = g_hash_table_new_full(
      g_str_hash, g_str_equal, g_free, (GDestroyNotify)freeDatesInfo);

  // 3. Iterate over all flights
  GHashTableIter iter;
  gpointer key;
  g_hash_table_iter_init(&iter, flights);
  while (g_hash_table_iter_next(&iter, &key, NULL)) {
    // Reminder to keep using encapsulation!
    Flight *flight = getFlight(key, flights);

    // 4. If the flight was cancelled, ignore it
    const char *status = getFlightStatus(flight);
    if (status && strcmp(status, "Cancelled") == 0) {
      freeFlight(flight);
      continue;
    }

    // 5. Get the airport code and insert its flight data into a new hash table
    const gchar *airportCode = getFlightOrigin(flight);

    DatesInfo *di = g_hash_table_lookup(airportsDepartures, airportCode);
    if (!di) {
      di = g_new0(DatesInfo, 1);
      // Array to hold (time_t) elements -> all the distinct dates of an
      // aiports's flights
      di->distinctDates = g_array_new(FALSE, FALSE, sizeof(time_t));
      // Set for existing date verification
      di->dateSet = g_hash_table_new(g_direct_hash, g_direct_equal);
      // Insert date information for the airport into the table
      g_hash_table_insert(airportsDepartures, g_strdup(airportCode), di);
    }

    time_t actualDep = getFlightActualDeparture(flight);
    if (actualDep < 0) {
      freeFlight(flight);
      continue;
    }

    freeFlight(flight);

    // 6. Truncate datetime format to date, as Q3 arguments are in the latter
    // Subtracts the seconds past since midnight
    time_t dayDep = actualDep - (actualDep % 86400);

    // 7. If the new date is not already in the array, add it there
    if (!g_hash_table_contains(di->dateSet, (gpointer)dayDep)) {
      g_hash_table_add(di->dateSet, (gpointer)dayDep);
      g_array_append_val(di->distinctDates, dayDep);
    }
  }

  // 8. Iterate over the hash table which holds the date information for each
  // airport
  GHashTableIter datesIter;
  gpointer datesKey, datesVal;
  g_hash_table_iter_init(&datesIter, airportsDepartures);
  while (g_hash_table_iter_next(&datesIter, &datesKey, &datesVal)) {
    DatesInfo *di = (DatesInfo *)datesVal;
    g_array_sort(di->distinctDates, compare_time_pointers);
  }

  return airportsDepartures;
}

// Getters for DatesInfo

const DatesInfo *getDatesInfo(gpointer *data) {
  DatesInfo *di = (DatesInfo *)data;
  return di;
}

const GArray *getDiDates(DatesInfo *di) { return di->distinctDates; }

const GHashTable *getDiSet(DatesInfo *di) { return di->dateSet; }

GPtrArray *get_dataset_airport_codes(Dataset *ds) { return ds->airportCodes; }

GPtrArray *get_dataset_aircraft_manufacturers(Dataset *ds) {
  return ds->aircraftManufacturers;
}

GPtrArray *get_dataset_nationalities(Dataset *ds) { return ds->nationalities; }
