/**
 * @file airports.h
 * @brief Definition and management logic for Airport entities.
 *
 * This module handles the data encapsulation for Airport records (IATA Code, Location, Name)
 * and provides functionality to parse them from CSV files into efficient in-memory structures.
 *
 * @note This header strictly enforces encapsulation by providing an opaque structure
 * and read-only accessors. Memory ownership is retained by the dataset/container,
 * preventing consumers from modifying or freeing internal data directly.
 */

#ifndef AIRPORTS_H
#define AIRPORTS_H

#include <glib.h>

/**
 * @brief Opaque structure representing an Airport entity.
 *
 * Holds details such as the IATA Code, Name, City, Country, and Type.
 * The internal memory layout is hidden to preserve ABI compatibility and data integrity.
 */
typedef struct airport Airport;

/**
 * @brief Memory cleanup function for an Airport structure.
 *
 * Designed to be used as a @c GDestroyNotify callback for GHashTables or other GLib containers.
 * It performs a deep free, releasing all internal strings and the structure pointer itself.
 *
 * @param data A generic pointer to the Airport structure to be freed.
 * If @p data is NULL, the function does nothing.
 */
void freeAirport(gpointer data);

/**
 * @brief Parses the Airports CSV file and populates a Hash Table.
 *
 * This function handles file I/O, CSV parsing, and data validation.
 * - Ignores the header line.
 * - Validates essential fields (e.g., Code must be present).
 * - Logs invalid entries to the global error file.
 * - Optionally collects valid airport codes into the provided @p codes array.
 *
 * @param filename The full path to the `airports.csv` file.
 * @param errorsFlag Pointer to an integer that will be set to 1 if any invalid lines are encountered.
 * @param codes Optional pointer to a @c GPtrArray. If provided (not NULL), the function
 * adds a copy of each valid airport code to this array (useful for sorting/indexing).
 *
 * @return A new @c GHashTable where:
 * - Key: Airport Code (string).
 * - Value: Pointer to @c Airport structure.
 * Returns @c NULL if the file cannot be opened.
 */
GHashTable *readAirports(const gchar *filename, gint *errorsFlag,
                         GPtrArray *codes);

/**
 * @brief Retrieves a read-only reference to an airport from the repository.
 *
 * Performs a fast O(1) lookup in the provided hash table using the airport code.
 *
 * @warning The returned pointer is owned by the Dataset container. The caller
 * **MUST NOT** free it, modify it, or attempt to free its internal strings.
 *
 * @param code The IATA code of the airport to retrieve (e.g., "LIS").
 * @param airportsTable The hash table containing the airport dataset.
 *
 * @return A @c const pointer to the Airport structure if found; @c NULL otherwise.
 */
const Airport *getAirport(const gchar *code, const GHashTable *airportsTable);

/**
 * @brief Accessor for the Airport IATA Code.
 *
 * @param airport A pointer to the constant Airport structure.
 * @return A constant string representing the airport code.
 * Returns @c NULL if @p airport is NULL.
 */
const gchar *getAirportCode(const Airport *airport);

/**
 * @brief Accessor for the Airport Name.
 *
 * @param airport A pointer to the constant Airport structure.
 * @return A constant string representing the full name of the airport.
 * Returns @c NULL if @p airport is NULL.
 */
const gchar *getAirportName(const Airport *airport);

/**
 * @brief Accessor for the City location.
 *
 * @param airport A pointer to the constant Airport structure.
 * @return A constant string representing the city name.
 * Returns @c NULL if @p airport is NULL.
 */
const gchar *getAirportCity(const Airport *airport);

/**
 * @brief Accessor for the Country location.
 *
 * @param airport A pointer to the constant Airport structure.
 * @return A constant string representing the country name.
 * Returns @c NULL if @p airport is NULL.
 */
const gchar *getAirportCountry(const Airport *airport);

/**
 * @brief Accessor for the Airport Type.
 *
 * @param airport A pointer to the constant Airport structure.
 * @return A constant string representing the type (e.g., "medium_airport", "large_airport").
 * Returns @c NULL if @p airport is NULL.
 */
const gchar *getAirportType(const Airport *airport);

#endif // AIRPORTS_H