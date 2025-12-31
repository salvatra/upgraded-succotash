#ifndef AIRPORTS_H
#define AIRPORTS_H

#include <glib.h>

/**
 * @brief Opaque structure representing an airport.
 */
typedef struct airport Airport;

/**
 * @brief Frees the memory allocated for an Airport structure.
 *
 * This function is intended to be used as a GDestroyNotify callback for
 * GHashTable or other GLib data structures.
 *
 * @param data A pointer to the Airport structure to be freed.
 */
void freeAirport(gpointer data);

/**
 * @brief Reads airport data from a CSV file and populates a hash table.
 *
 * @param filename The path to the CSV file containing airport data.
 * @param errorsFlag A pointer to an integer that will be set to 1 if any
 * invalid lines are encountered.
 * @param codes An array to hold all distinct airport codes, allowing greater
 * flexibility of the code.
 * @return A GHashTable where keys are airport codes (gchar*) and values are
 * Airport pointers. Returns NULL on failure.
 */
GHashTable *readAirports(const gchar *filename, gint *errorsFlag,
                         GPtrArray *codes);

/**
 * @brief Retrieves a copy of an airport from the hash table by its code.
 *
 * This function looks up the airport in the provided hash table and returns a
 * *newly allocated* copy of the Airport structure. Users are responsible for
 * freeing the returned Airport.
 *
 * @param code The IATA code of the airport to retrieve.
 * @param airportsTable The hash table containing airport data.
 * @return A pointer to a newly allocated Airport structure, or NULL if not
 * found.
 */
Airport *getAirport(const gchar *code, const GHashTable *airportsTable);

/**
 * @brief Gets the IATA code of an airport.
 *
 * @param airport A pointer to the Airport structure.
 * @return A string representing the airport code.
 */
gchar *getAirportCode(Airport *airport);

/**
 * @brief Gets the name of an airport.
 *
 * @param airport A pointer to the Airport structure.
 * @return A string representing the airport name.
 */
gchar *getAirportName(Airport *airport);

/**
 * @brief Gets the city where the airport is located.
 *
 * @param airport A pointer to the Airport structure.
 * @return A string representing the airport city.
 */
gchar *getAirportCity(Airport *airport);

/**
 * @brief Gets the country where the airport is located.
 *
 * @param airport A pointer to the Airport structure.
 * @return A string representing the airport country.
 */
gchar *getAirportCountry(Airport *airport);

// gdouble getAirportLatitude(const Airport *airport);
// gdouble getAirportLongitude(const Airport *airport);
// const gchar *getAirportICAO(const Airport *airport);

/**
 * @brief Gets the type of the airport.
 *
 * @param airport A pointer to the Airport structure.
 * @return A string representing the airport type.
 */
gchar *getAirportType(Airport *airport);

#endif // !AIRPORTS_H
