#ifndef AIRCRAFTS_H
#define AIRCRAFTS_H

#include <glib.h>

/**
 * @brief Opaque structure representing an aircraft.
 */
typedef struct aircraft Aircraft;

/**
 * @brief Frees the memory allocated for an Aircraft structure.
 *
 * This function is intended to be used as a GDestroyNotify callback for
 * GHashTable or other GLib data structures.
 *
 * @param data A pointer to the Aircraft structure to be freed.
 */
void freeAircraft(gpointer data);

/**
 * @brief Reads aircraft data from a CSV file and populates a hash table.
 *
 * @param filename The path to the CSV file containing aircraft data.
 * @param errorsFlag A pointer to an integer that will be set to 1 if any
 * invalid lines are encountered during parsing.
 * @param manufacturers An array of aircraft manufacturers to populate
 * @return A GHashTable where keys are aircraft IDs (gchar*) and values are
 * Aircraft pointers. Returns NULL on failure.
 */
GHashTable *readAircrafts(const gchar *filename, gint *errorsFlag,
                          GPtrArray *manufacturers);

/**
 * @brief Retrieves a copy of an aircraft from the hash table by its ID.
 *
 * This function looks up the aircraft in the provided hash table and returns a
 * *newly allocated* copy of the Aircraft structure. Users are responsible for
 * freeing the returned Aircraft.
 *
 * @param id The ID of the aircraft to retrieve.
 * @param aircraftsTable The hash table containing aircraft data.
 * @return A pointer to a newly allocated Aircraft structure, or NULL if not
 * found.
 */
Aircraft *getAircraft(const gchar *id, const GHashTable *aircraftsTable);

/**
 * @brief Gets the ID of an aircraft.
 *
 * @param aircraft A pointer to the Aircraft structure.
 * @return A string representing the aircraft ID, or NULL if the input is NULL.
 */
gchar *getAircraftId(Aircraft *aircraft);

/**
 * @brief Gets the manufacturer of an aircraft.
 *
 * @param aircraft A pointer to the Aircraft structure.
 * @return A string representing the aircraft manufacturer, or NULL if the input
 * is NULL.
 */
gchar *getAircraftManufacturer(Aircraft *aircraft);

/**
 * @brief Gets the model of an aircraft.
 *
 * @param aircraft A pointer to the Aircraft structure.
 * @return A string representing the aircraft model, or NULL if the input is
 * NULL.
 */
gchar *getAircraftModel(Aircraft *aircraft);

// gint getAircraftYear(const Aircraft *aircraft);
// gint getAircraftCapacity(const Aircraft *aircraft);
// gint getAircraftRange(const Aircraft *aircraft);

#endif // !AIRCRAFTS_H
