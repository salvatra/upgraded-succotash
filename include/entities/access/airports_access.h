/**
 * @file airports_access.h
 * @brief Public interface for accessing and managing Airport entities.
 *
 * This header defines the opaque `Airport` structure and provides functions for:
 * - Lifecycle management (freeing memory).
 * - Parsing (reading from CSV).
 * - Data retrieval (getters for entity properties).
 *
 * This abstraction ensures that consumer modules interact with airports only through
 * defined accessors, hiding the internal memory layout.
 */

#ifndef AIRPORTS_ACCESS_H
#define AIRPORTS_ACCESS_H

#include <glib.h>

/**
 * @typedef Airport
 * @brief Opaque structure representing a single Airport entity.
 *
 * The internal layout is defined in `src/entities/internal/airports_internal.h`
 * and is hidden from external modules to enforce encapsulation.
 */
typedef struct airport Airport;

/**
 * @brief Memory cleanup function for an Airport structure.
 *
 * Compatible with `GDestroyNotify`. Frees all dynamically allocated memory associated
 * with the Airport instance (strings, etc.) and the structure itself.
 *
 * @param data A pointer to the `Airport` structure to free.
 */
void freeAirport(gpointer data);

/**
 * @brief Parses the Airports CSV file and populates a hash table.
 *
 * Reads `airports.csv`, validates each line (checking 3-letter IATA codes,
 * valid coordinates, etc.), and stores valid records in a hash table.
 *
 * @param filename The full path to the `airports.csv` file.
 * @param errorsFlag Pointer to an integer flag. Set to 1 if any invalid lines are encountered/logged.
 * @param codes A pointer to a `GPtrArray`. If provided, unique valid airport codes
 * will be added to this array (useful for autocomplete/validation later).
 * @return A `GHashTable*` containing `Airport*` values indexed by the Airport Code.
 * Returns NULL on file error.
 */
GHashTable *readAirports(const gchar *filename, gint *errorsFlag, GPtrArray *codes);

/**
 * @brief Retrieves a read-only reference to an airport from the lookup table.
 *
 * @param code The 3-letter IATA Airport Code (e.g., "LIS").
 * @param airportsTable The hash table containing all airport entities.
 * @return A const pointer to the `Airport` entity, or NULL if not found.
 */
const Airport *getAirport(const gchar *code, const GHashTable *airportsTable);

// --- Getters ---

/**
 * @brief Gets the IATA code of the airport.
 * @param airport The airport entity.
 * @return The 3-letter code string (e.g., "LIS").
 */
const gchar *getAirportCode(const Airport *airport);

/**
 * @brief Gets the full name of the airport.
 * @param airport The airport entity.
 * @return The name string (e.g., "Humberto Delgado Airport").
 */
const gchar *getAirportName(const Airport *airport);

/**
 * @brief Gets the city where the airport is located.
 * @param airport The airport entity.
 * @return The city name string (e.g., "Lisbon").
 */
const gchar *getAirportCity(const Airport *airport);

/**
 * @brief Gets the country where the airport is located.
 * @param airport The airport entity.
 * @return The country name string (e.g., "Portugal").
 */
const gchar *getAirportCountry(const Airport *airport);

/**
 * @brief Gets the type/category of the airport.
 * @param airport The airport entity.
 * @return The type string (e.g., "large_airport", "heliport").
 */
const gchar *getAirportType(const Airport *airport);

#endif