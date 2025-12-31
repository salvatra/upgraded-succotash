/**
 * @file dataset.h
 * @brief Dataset management module
 */

#ifndef DATASET_H
#define DATASET_H

#include <entities/aircrafts.h>
#include <entities/airports.h>
#include <entities/flights.h>
#include <entities/passengers.h>
#include <entities/reservations.h>
#include <glib.h>

/**
 * @brief Opaque structure representing the main dataset containing all
 * application data.
 *
 * It stores all information about flights, passengers, airports, aicrafts and
 * reservations.
 */
typedef struct dataset Dataset;

/**
 * @brief Opaque structure representing date-related information used for
 * statistical queries.
 *
 * It's used to hold information on distinct flight dates as well as data which
 * allows to check such dates.
 */
typedef struct datesinfo DatesInfo;

/**
 * @brief Initializes a new, empty Dataset.
 *
 * Creates, allocates and initializes with NULL a new Dataset structure.
 *
 * @return A pointer to the newly allocated dataset.
 */
Dataset *initDataset(void);

/**
 * @brief Loads all data from CSV files into the dataset.
 *
 * This function coordinates the parsing and loading of flights, passengers,
 * airports, aircrafts and reservations from the specified directory by calling
 * lower-level parsing functions and populating the provided Dataset structure
 * with the data contained therein. It also allows the logging of execution time
 * for testing purposes.
 *
 * @param ds A pointer to the Dataset structure to populate.
 * @param errorsFlag A pointer to an integer that will be updated to indicate if
 * any errors occurred during loading.
 * @param filePath The path to the directory containing the CSV data files.
 * @param enable_timing If TRUE, performance timing information may be logged.
 */
void loadAllDatasets(Dataset *ds, gint *errorsFlag, const char *filePath,
                     gboolean enable_timing);

/**
 * @brief Frees all memory associated with a Dataset.
 *
 * This includes freeing all hash tables and their contained data as well as the
 * Dataset structure.
 *
 * @param ds A pointer to the Dataset to be cleaned up.
 */
void cleanupDataset(Dataset *ds);

/**
 * @brief Retrieves the hash table containing flight data.
 *
 * @param ds A pointer to the Dataset.
 * @return A const pointer to the GHashTable containing flights.
 */
const GHashTable *getDatasetFlights(Dataset *ds);

/**
 * @brief Retrieves the hash table containing passenger data.
 *
 * @param ds A pointer to the Dataset.
 * @return A const pointer to the GHashTable containing passengers.
 */
const GHashTable *getDatasetPassengers(Dataset *ds);

/**
 * @brief Retrieves the hash table containing airport data.
 *
 * @param ds A pointer to the Dataset.
 * @return A const pointer to the GHashTable containing airports.
 */
const GHashTable *getDatasetAirports(Dataset *ds);

/**
 * @brief Retrieves the hash table containing aircraft data.
 *
 * @param ds A pointer to the Dataset.
 * @return A const pointer to the GHashTable containing aircrafts.
 */
const GHashTable *getDatasetAircrafts(Dataset *ds);

/**
 * @brief Retrieves the hash table containing reservation data.
 *
 * @param ds A pointer to the Dataset.
 * @return A const pointer to the GHashTable containing reservations.
 */
const GHashTable *getDatasetReservations(Dataset *ds);

/**
 * @brief Retrieves the hash table containing aiports data used for the new
 * query1.
 *
 * @param ds A pointer to the Dataset.
 * @return A const pointer to the GHashTable containing aiportsStats.
 */
const GHashTable *getDatasetAiportStats(Dataset *ds);

/**
 * @brief Retrieves the hash table storing date information.
 *
 * This table is used for optimizing queries that involve date filtering or
 * sorting as well as the construction of the Fenwick Trees.
 *
 * @param ds A pointer to the Dataset.
 * @return A pointer to the GHashTable containing date info.
 */
GHashTable *getDatesTable(Dataset *ds);

/**
 * @brief Frees memory associated with DatesInfo structure.
 *
 * @param data A pointer to the DatesInfo structure to be freed.
 */
void freeDatesInfo(gpointer data);

/**
 * @brief Retrieves a DatesInfo strcuture from a generic pointer.
 *
 * @param data A generic pointer, typically from a hash table value.
 * @return A const pointer to the DatesInfo structure.
 */
const DatesInfo *getDatesInfo(gpointer *data);

/**
 * @brief Retrieves the array of dates associated with a DatesInfo structure.
 *
 * @param di A pointer to the DatesInfo structure.
 * @return A const pointer to a GArray containing dates.
 */
const GArray *getDiDates(DatesInfo *di);

/**
 * @brief Retrieves the set of dates (as a hash table) associated with a
 * DatesInfo structure.
 *
 * @param di A pointer to the DatesInfo structure.
 * @return A const pointer to a GHashTable representing the set of dates.
 */
const GHashTable *getDiSet(DatesInfo *di);

/**
 * @brief Retrieves the array of distinct airport codes.
 *
 * @param ds A pointer to the Dataset.
 * @return A GPtrArray pointer containing airport code strings.
 */
GPtrArray *get_dataset_airport_codes(Dataset *ds);

/**
 * @brief Retrieves the array of distinct aircraft manufacturers.
 *
 * @param ds A pointer to the Dataset.
 * @return A GPtrArray pointer containing manufacturer strings.
 */
GPtrArray *get_dataset_aircraft_manufacturers(Dataset *ds);

/**
 * @brief Retrieves the array of distinct passenger nationalities.
 *
 * @param ds A pointer to the Dataset.
 * @return A GPtrArray pointer containing nationality strings.
 */
GPtrArray *get_dataset_nationalities(Dataset *ds);

// gboolean validateDataset(Dataset *ds);

#endif
