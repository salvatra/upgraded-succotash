/**
 * @file manager.h
 * @brief High-level orchestration module for dataset loading.
 *
 * This header defines the entry point for the I/O subsystem. It is responsible for
 * taking a directory path, locating the required CSV files (passengers, flights, etc.),
 * and dispatching them to their respective parsers in the correct order to ensure
 * referential integrity (e.g., loading Passengers before Reservations).
 */

#ifndef IO_MANAGER_H
#define IO_MANAGER_H

#include <glib.h>
#include "core/dataset.h"

/**
 * @brief Orchestrates the loading of all CSV files into the dataset.
 *
 * This function serves as the "Director" of the loading process. It coordinates
 * the execution of specific entity parsers (Passengers, Airports, Aircrafts, Flights, Reservations).
 *
 * **Loading Order:**
 * 1. **Independent Entities**: Users (Passengers), Airports, Aircrafts.
 * 2. **Dependent Entities**: Flights (depends on Airports and Aircrafts).
 * 3. **Highly Dependent Entities**: Reservations (depends on Users and Flights).
 *
 * @param ds [in,out] The dataset instance to populate. Must be initialized via `initDataset()`.
 * @param errorsFlag [out] Pointer to an integer. The function sets this to 1 if *any* parser
 * encounters invalid lines or file errors.
 * @param filePath [in] The root directory path containing the CSV files (e.g., "data/").
 * @param enable_timing [in] If `TRUE`, prints performance metrics (execution time per file) to stdout.
 * Useful for profiling the parsing bottleneck.
 */
void loadAllDatasets(Dataset *ds, int *errorsFlag, const char *filePath, gboolean enable_timing);

#endif