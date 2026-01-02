/**
 * @file airports.h
 * @brief Airport entity definition and management.
 *
 * This header defines the interface for the Airport entity. It strictly enforces
 * encapsulation by providing an opaque structure and read-only accessors.
 * Memory ownership is retained by the dataset/container, meaning users get
 * direct references to internal data but cannot modify or free it.
 */

#ifndef AIRPORTS_H
#define AIRPORTS_H

#include <glib.h>

/**
 * @brief Opaque structure representing an airport.
 *
 * The internal layout of this structure is hidden from the public API.
 * Users interact with it solely through the provided accessor functions,
 * which ensure data integrity is maintained.
 */
typedef struct airport Airport;

/**
 * @brief Frees the memory allocated for an Airport structure.
 *
 * This function is designed to be used as a GDestroyNotify callback for
 * GHashTables or other GLib containers that hold Airport pointers.
 * It frees all internal strings and the structure itself.
 *
 * @param data A generic pointer to the Airport structure to be freed.
 * If data is NULL, the function does nothing.
 */
void freeAirport(gpointer data);

/**
 * @brief Reads airport data from a CSV file and populates a hash table.
 *
 * This function handles the parsing of the airports CSV file. It validates
 * each record (checking codes, coordinates, etc.) and stores valid airports
 * in a GHashTable. Invalid lines are logged to an error file.
 *
 * @param filename The path to the CSV file containing airport data.
 * @param errorsFlag A pointer to an integer that will be updated to 1 if any
 * invalid lines are encountered during parsing.
 * @param codes A pointer to a GPtrArray. If provided (not NULL), this function
 * will add a copy of each valid airport code to this array.
 * This is typically used for validation or statistical purposes.
 *
 * @return A GHashTable pointer where keys are airport codes (strings) and values
 * are pointers to Airport structures. Returns NULL if the file cannot
 * be opened or if a critical error occurs.
 */
GHashTable *readAirports(const gchar *filename, gint *errorsFlag,
                         GPtrArray *codes);

/**
 * @brief Retrieves a read-only reference to an airport from the hash table.
 *
 * This function performs a lookup in the provided hash table using the airport code.
 * It returns a direct pointer to the stored Airport structure, granting fast
 * access without memory allocation overhead.
 *
 * @warning The returned pointer belongs to the dataset container. The caller
 * MUST NOT free it, modify it, or attempt to free its internal strings.
 *
 * @param code The IATA code of the airport to retrieve.
 * @param airportsTable The hash table containing the airport data.
 *
 * @return A const pointer to the Airport structure if found; NULL otherwise.
 */
const Airport *getAirport(const gchar *code, const GHashTable *airportsTable);

/**
 * @brief Gets the IATA code of an airport.
 *
 * @param airport A pointer to the constant Airport structure.
 * @return A constant string representing the airport code (e.g., "OPO").
 * The string is owned by the Airport structure and must not be freed.
 * Returns NULL if the input pointer is NULL.
 */
const gchar *getAirportCode(const Airport *airport);

/**
 * @brief Gets the full name of an airport.
 *
 * @param airport A pointer to the constant Airport structure.
 * @return A constant string representing the airport name.
 * The string is owned by the Airport structure and must not be freed.
 * Returns NULL if the input pointer is NULL.
 */
const gchar *getAirportName(const Airport *airport);

/**
 * @brief Gets the city where the airport is located.
 *
 * @param airport A pointer to the constant Airport structure.
 * @return A constant string representing the city name.
 * The string is owned by the Airport structure and must not be freed.
 * Returns NULL if the input pointer is NULL.
 */
const gchar *getAirportCity(const Airport *airport);

/**
 * @brief Gets the country where the airport is located.
 *
 * @param airport A pointer to the constant Airport structure.
 * @return A constant string representing the country name.
 * The string is owned by the Airport structure and must not be freed.
 * Returns NULL if the input pointer is NULL.
 */
const gchar *getAirportCountry(const Airport *airport);

/**
 * @brief Gets the type of the airport.
 *
 * @param airport A pointer to the constant Airport structure.
 * @return A constant string representing the airport type (e.g., "medium_airport").
 * The string is owned by the Airport structure and must not be freed.
 * Returns NULL if the input pointer is NULL.
 */
const gchar *getAirportType(const Airport *airport);

#endif // AIRPORTS_H