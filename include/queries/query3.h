#ifndef QUERY3_H
#define QUERY3_H
#include <glib.h>

/**
 * @brief Structure to hold date and its associated count.
 */
typedef struct dateCount DateCount;

/**
 * @brief Executes Query 3: Flight statistics by airport for a date range.
 *
 * @param airportFtrees Hash table mapping airport codes to Fenwick Trees (or similar structures) for date queries.
 * @param airports Hash table of airport data.
 * @param startStr Start date string (YYYY-MM-DD).
 * @param endStr End date string (YYYY-MM-DD).
 * @return A string containing the query results (e.g., lines of "AirportCode;Count").
 */
gchar *query3(GHashTable *airportFtrees, GHashTable *airports,
              const char *startStr, const char *endStr);

#endif
