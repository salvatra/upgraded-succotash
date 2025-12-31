#ifndef UTILS_H
#define UTILS_H

#include <glib.h>

/**
 * @brief Checks if a string matches a given regular expression pattern.
 *
 * @param pattern The regular expression pattern string.
 * @param string The string to check against the pattern.
 * @return TRUE if the string matches the pattern, FALSE otherwise.
 */
gboolean checkRegex(const gchar *pattern, const gchar *string);

/**
 * @brief Trims leading and trailing quotes (") from a string in place.
 *
 * @param s The string to trim.
 */
void trim(gchar *s);

/**
 * @brief Trims leading and trailing whitespace from a string in place.
 * 
 * @param s The string to trim.
 */
void trim_whitespace(gchar *s);

/**
 * @brief Parses a price string (e.g., "12.34") into cents.
 *
 * @param price_str The price string.
 * @return The price in cents.
 */
int parse_price_cents(const char *price_str);

/**
 * @brief Converts a price in cents to a euro float value.
 *
 * @param cents The price in cents.
 * @return The price in euros.
 */
float cents_to_euro(int cents);

//GDateTime *parseDatetimeNull(const gchar *str, gint *cancelFlag);
//GDateTime *parseDatetime(const gchar *str);

/**
 * @brief Frees a GDateTime structure.
 *
 * Used as a GDestroyNotify callback.
 *
 * @param data Pointer to the GDateTime structure.
 */
void freeGDateTime(gpointer data);

/**
 * @brief Compares two GDateTime pointers.
 *
 * @param a First GDateTime pointer.
 * @param b Second GDateTime pointer.
 * @return A negative integer if a < b, 0 if a == b, or a positive integer if a > b.
 */
gint cmpGDateTime(gconstpointer a, gconstpointer b);

//gint checkAircraftAsKey(GHashTable *flightsTable, GHashTable *aircraftsTable,
 //                       const gchar *header, const gchar *errorsFile);

/**
 * @brief Checks if reservation keys exist in the passenger table and logs errors.
 *
 * @param reservationsTable Hash table of reservations.
 * @param passengersTable Hash table of passengers.
 * @param header CSV header string for error logging.
 * @param errorsFile Path to the error log file.
 * @return 0 on success, or an error code.
 */
gint checkDocumentAsKey(GHashTable *reservationsTable,
                        GHashTable *passengersTable, const gchar *header,
                        const gchar *errorsFile);

/**
 * @brief Checks if a file or directory path exists.
 * @param path The path string to check.
 * @return TRUE if the path exists, FALSE otherwise.
 */
gboolean checkPath(const gchar *path);

#endif
