/**
 * @file aircrafts_access.h
 * @brief Public interface for accessing and managing Aircraft entities.
 *
 * This header defines the opaque `Aircraft` structure and provides functions for:
 * - Lifecycle management (freeing memory).
 * - Parsing (reading from CSV).
 * - Data retrieval (getters for entity properties).
 *
 * This design decouples the internal representation of an Aircraft from its consumers.
 */

#ifndef AIRCRAFTS_ACCESS_H
#define AIRCRAFTS_ACCESS_H

#include <glib.h>

/**
 * @typedef Aircraft
 * @brief Opaque structure representing a single Aircraft entity.
 *
 * The internal layout is defined in `src/entities/internal/aircrafts_internal.h`
 * and is hidden from external modules to enforce encapsulation.
 */
typedef struct aircraft Aircraft;

/**
 * @brief Memory cleanup function for an Aircraft structure.
 *
 * Compatible with `GDestroyNotify`. Frees all dynamically allocated memory associated
 * with the Aircraft instance (strings, etc.) and the structure itself.
 *
 * @param data A pointer to the `Aircraft` structure to free.
 */
void freeAircraft(gpointer data);

/**
 * @brief Parses the Aircrafts CSV file and populates a hash table.
 *
 * Reads `aircrafts.csv`, validates each line, and stores valid records in a hash table.
 * It also collects unique aircraft manufacturers into a sorted array for index-based queries.
 *
 * @param filename The full path to the `aircrafts.csv` file.
 * @param errorsFlag Pointer to an integer flag. Set to 1 if any invalid lines are encountered/logged.
 * @param manufacturers A pointer to a `GPtrArray`. If provided, unique manufacturer names
 * will be added to this array.
 * @return A `GHashTable*` containing `Aircraft*` values indexed by Aircraft ID strings.
 * Returns NULL on file error.
 */
GHashTable *readAircrafts(const gchar *filename, gint *errorsFlag, GPtrArray *manufacturers);

/**
 * @brief Retrieves a read-only reference to an aircraft from the lookup table.
 *
 * @param id The Aircraft ID string (e.g., "A380-800").
 * @param aircraftsTable The hash table containing all aircraft entities.
 * @return A const pointer to the `Aircraft` entity, or NULL if not found.
 */
const Aircraft *getAircraft(const gchar *id, const GHashTable *aircraftsTable);

// --- Getters ---

/**
 * @brief Gets the unique identifier of the aircraft.
 * @param aircraft The aircraft entity.
 * @return The Aircraft ID string (e.g., "A380-800").
 */
const gchar *getAircraftId(const Aircraft *aircraft);

/**
 * @brief Gets the manufacturer name of the aircraft.
 * @param aircraft The aircraft entity.
 * @return The manufacturer string (e.g., "Airbus", "Boeing").
 */
const gchar *getAircraftManufacturer(const Aircraft *aircraft);

/**
 * @brief Gets the model name of the aircraft.
 * @param aircraft The aircraft entity.
 * @return The model string (e.g., "A380", "747").
 */
const gchar *getAircraftModel(const Aircraft *aircraft);

#endif