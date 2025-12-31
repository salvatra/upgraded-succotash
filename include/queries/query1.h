#ifndef QUERY1_H
#define QUERY1_H

#include <entities/airports.h>
#include <entities/flights.h>
#include <entities/passengers.h>
#include <entities/reservations.h>
#include <glib.h>

/**
 * @brief Executes Query 1: Retrieve airport summary.
 *
 * Given an airport code, returns a summary string containing the airport name,
 * city, country, etc.
 *
 * @param code The IATA code of the airport.
 * @param airportsTable The hash table containing airport data.
 * @return A newly allocated string containing the summary, or NULL/empty string
 * if not found.
 */
gchar *query1(const gchar *code,
              const GHashTable *airports,
              const GHashTable *airportStats);

#endif
