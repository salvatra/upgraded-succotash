/**
 * @file aircrafts.h
 * @brief Aircraft entity definition and management.
 *
 * This header defines the interface for the Aircraft entity. It strictly enforces
 * encapsulation by providing an opaque structure and read-only accessors.
 * Memory ownership is retained by the dataset/container, preventing users from
 * modifying or freeing internal data directly.
 */

#ifndef AIRCRAFTS_H
#define AIRCRAFTS_H

#include <glib.h>

/**
 * @brief Opaque structure representing an aircraft.
 *
 * The internal layout is hidden. Users interact with this structure solely
 * through the provided accessor functions, ensuring data integrity.
 */
typedef struct aircraft Aircraft;

/**
 * @brief Frees the memory allocated for an Aircraft structure.
 *
 * Designed to be used as a GDestroyNotify callback for GHashTable.
 * It frees the internal strings (id, manufacturer, model) and the structure itself.
 *
 * @param data A generic pointer to the Aircraft structure to be freed.
 */
void freeAircraft(gpointer data);

/**
 * @brief Reads aircraft data from a CSV file and populates a hash table.
 *
 * Parses the aircraft CSV file, validates each record, and stores valid
 * aircraft in a GHashTable. Invalid lines are logged to an error file.
 *
 * @param filename The path to the CSV file containing aircraft data.
 * @param errorsFlag A pointer to an integer updated to 1 if invalid lines are found.
 * @param manufacturers An optional pointer to a GPtrArray. If provided, the function
 * adds unique manufacturer names to this list during parsing.
 *
 * @return A GHashTable where keys are aircraft IDs (strings) and values are
 * pointers to Aircraft structures. Returns NULL on failure.
 */
GHashTable *readAircrafts(const gchar *filename, gint *errorsFlag,
                          GPtrArray *manufacturers);

/**
 * @brief Retrieves a read-only reference to an aircraft from the hash table.
 *
 * Performs a fast O(1) lookup. Returns a direct pointer to the stored data.
 *
 * @warning The returned pointer is owned by the dataset. The caller MUST NOT
 * free it or modify it.
 *
 * @param id The ID of the aircraft to retrieve (e.g., "A380-800").
 * @param aircraftsTable The hash table containing aircraft data.
 * @return A const pointer to the Aircraft structure, or NULL if not found.
 */
const Aircraft *getAircraft(const gchar *id, const GHashTable *aircraftsTable);

/**
 * @brief Gets the unique ID of an aircraft.
 *
 * @param aircraft A pointer to the constant Aircraft structure.
 * @return A constant string representing the ID. Owned by the Aircraft structure.
 */
const gchar *getAircraftId(const Aircraft *aircraft);

/**
 * @brief Gets the manufacturer of an aircraft.
 *
 * @param aircraft A pointer to the constant Aircraft structure.
 * @return A constant string representing the manufacturer (e.g., "Boeing").
 * Owned by the Aircraft structure.
 */
const gchar *getAircraftManufacturer(const Aircraft *aircraft);

/**
 * @brief Gets the model of an aircraft.
 *
 * @param aircraft A pointer to the constant Aircraft structure.
 * @return A constant string representing the model. Owned by the Aircraft structure.
 */
const gchar *getAircraftModel(const Aircraft *aircraft);

#endif // !AIRCRAFTS_H