#ifndef VALIDATION_UTILS_H
#define VALIDATION_UTILS_H

#include <glib.h>
#include <time.h>

/**
 * @brief Checks if a string represents a valid integer.
 */
gboolean checkInt(const gchar *str);

/**
 * @brief Checks if a date (time_t) is valid.
 */
gboolean checkDate(const time_t dt);

/**
 * @brief Checks if a datetime (time_t) is valid.
 */
gboolean checkDatetime(const time_t dt);

/**
 * @brief Checks if a year string is valid.
 */
gboolean checkYear(const gchar *yearChar);

/**
 * @brief Checks if an email address is valid.
 */
gboolean checkEmail(const char *email);

/**
 * @brief Checks if a CSV list string is valid.
 */
gboolean checkCsvList(const gchar *list);

/**
 * @brief Logs an invalid CSV line to an error file.
 */
void logInvalidLine(const gchar *filename, const gchar *header,
                    const gchar *line);

#endif