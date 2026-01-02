/**
 * @file aircrafts.h
 * @brief Definition and management logic for Aircraft entities.
 *
 * This module handles the data encapsulation for Aircraft records (ID, Model, Manufacturer)
 * and provides functionality to parse them from CSV files into efficient in-memory structures.
 *
 * @note This header strictly enforces encapsulation by providing an opaque structure
 * and read-only accessors. Memory ownership is retained by the dataset, preventing
 * consumers from modifying or freeing internal data directly.
 */

#ifndef AIRCRAFTS_H
#define AIRCRAFTS_H

#include <glib.h>

/**
 * @brief Opaque structure representing an Aircraft entity.
 *
 * Holds details such as the Aircraft ID (Tail Number), Model, and Manufacturer.
 * The internal memory layout is hidden to preserve ABI compatibility and data integrity.
 */
typedef struct aircraft Aircraft;

/**
 * @brief Memory cleanup function for an Aircraft structure.
 *
 * Designed to be used as a @c GDestroyNotify callback for GHashTables.
 * It performs a deep free, releasing the internal strings (id, manufacturer, model)
 * and the structure pointer itself.
 *
 * @param data A generic pointer to the Aircraft structure to be freed.
 */
void freeAircraft(gpointer data);

/**
 * @brief Parses the Aircrafts CSV file and populates a Hash Table.
 *
 * This function handles file I/O, CSV parsing, and data validation.
 * - It ignores the header line.
 * - It validates the syntax of each field (e.g., non-empty strings).
 * - It logs invalid entries to the global error file (if applicable).
 * - It collects unique manufacturer names into the optional @p manufacturers array.
 *
 * @param filename The full path to the `aircrafts.csv` file.
 * @param errorsFlag Pointer to an integer that will be set to 1 if any invalid lines are encountered.
 * @param manufacturers Optional pointer to a @c GPtrArray. If provided (not NULL),
 * the function appends unique manufacturer names encountered during parsing.
 *
 * @return A new @c GHashTable where:
 * - Key: Aircraft ID (string).
 * - Value: Pointer to @c Aircraft structure.
 * Returns @c NULL if the file cannot be opened.
 */
GHashTable *readAircrafts(const gchar *filename, gint *errorsFlag,
                          GPtrArray *manufacturers);

/**
 * @brief Retrieves a read-only reference to an aircraft from the repository.
 *
 * Performs a fast O(1) lookup in the hash table.
 *
 * @warning The returned pointer is owned by the Dataset. The caller **MUST NOT** * free it or modify its contents.
 *
 * @param id The unique ID of the aircraft to retrieve (e.g., "A380-800").
 * @param aircraftsTable The hash table containing the aircraft dataset.
 * @return A @c const pointer to the Aircraft entity, or @c NULL if the ID is not found.
 */
const Aircraft *getAircraft(const gchar *id, const GHashTable *aircraftsTable);

/**
 * @brief Accessor for the Aircraft ID.
 *
 * @param aircraft A pointer to the constant Aircraft structure.
 * @return A constant string representing the ID. Returns @c NULL if @p aircraft is NULL.
 */
const gchar *getAircraftId(const Aircraft *aircraft);

/**
 * @brief Accessor for the Aircraft Manufacturer.
 *
 * @param aircraft A pointer to the constant Aircraft structure.
 * @return A constant string representing the manufacturer (e.g., "Boeing", "Airbus").
 * Returns @c NULL if @p aircraft is NULL.
 */
const gchar *getAircraftManufacturer(const Aircraft *aircraft);

/**
 * @brief Accessor for the Aircraft Model.
 *
 * @param aircraft A pointer to the constant Aircraft structure.
 * @return A constant string representing the model (e.g., "747-400").
 * Returns @c NULL if @p aircraft is NULL.
 */
const gchar *getAircraftModel(const Aircraft *aircraft);

#endif // !AIRCRAFTS_H