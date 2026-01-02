/**
 * @file dataset.h
 * @brief Central data repository for the application.
 *
 * This header defines the Dataset structure and its public interface.
 * To reduce compilation dependencies, it uses forward declarations for entities.
 * It strictly hides the internal implementation (GHashTables) from consumers.
 */

#ifndef DATASET_H
#define DATASET_H

#include <glib.h>

typedef struct aircraft Aircraft;
typedef struct airport Airport;
typedef struct flight Flight;
typedef struct passenger Passenger;
typedef struct reservation Reservation;
// Note: AirportPassengerStats is defined in reservations.h, treating as opaque here if needed.

/**
 * @brief Opaque structure representing the entire dataset.
 */
typedef struct dataset Dataset;

/**
 * @brief Opaque structure for Date indexing (Query 3).
 */
typedef struct datesinfo DatesInfo;

/**
 * @brief Initializes a new, empty Dataset.
 * @return A pointer to the allocated Dataset.
 */
Dataset *initDataset(void);

/**
 * @brief Loads all data from CSV files into the Dataset.
 *
 * @param ds The dataset to populate.
 * @param errorsFlag Pointer to an integer to track if errors occurred during parsing.
 * @param filePath The directory path containing the CSV files.
 * @param enable_timing If TRUE, prints loading times to stdout.
 */
void loadAllDatasets(Dataset *ds, gint *errorsFlag, const char *filePath,
                     gboolean enable_timing);

/**
 * @brief Frees all memory associated with the Dataset and its entities.
 * @param ds The dataset to destroy.
 */
void cleanupDataset(Dataset *ds);

// All of thesegetDatasetEntity are going away real soon

/**
 * @brief Retrieves the underlying Aircraft hash table (Read-Only).
 */
const GHashTable *getDatasetAircrafts(const Dataset *ds);

/**
 * @brief Retrieves the underlying Airport hash table (Read-Only).
 */
const GHashTable *getDatasetAirports(const Dataset *ds);

/**
 * @brief Retrieves the underlying Flight hash table (Read-Only).
 */
const GHashTable *getDatasetFlights(const Dataset *ds);

/**
 * @brief Retrieves the underlying Passenger hash table (Read-Only).
 */
const GHashTable *getDatasetPassengers(const Dataset *ds);

/**
 * @brief Retrieves the underlying Reservation hash table (Read-Only).
 */
const GHashTable *getDatasetReservations(const Dataset *ds);

/**
 * @brief Retrieves the underlying Airport Statistics hash table (Read-Only).
 */
const GHashTable *getDatasetAiportStats(const Dataset *ds);


const Flight *dataset_get_flight(const Dataset *ds, const char *id);
const Airport *dataset_get_airport(const Dataset *ds, const char *code);
const Aircraft *dataset_get_aircraft(const Dataset *ds, const char *id);
const Passenger *dataset_get_passenger(const Dataset *ds, int id);
const Reservation *dataset_get_reservation(const Dataset *ds, const char *id);

// aux
GPtrArray *get_dataset_airport_codes(Dataset *ds);
GPtrArray *get_dataset_aircraft_manufacturers(Dataset *ds);
GPtrArray *get_dataset_nationalities(Dataset *ds);

#endif // DATASET_H