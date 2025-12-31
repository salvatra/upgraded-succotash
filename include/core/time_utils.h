
/**
 * @file time_utils.h
 * @brief Time management, conversion and comparison utilities
 */

#ifndef TIME_UTILS_H
#define TIME_UTILS_H
#include <glib.h>
#include <time.h>

/**
 * @brief Parses a datetime string into a time_t integer.
 *
 * This function uses native mathematical operations to convert a string in
 * datetime format ("yyyy-mm-dd HH:MM") into a time_t struct counting the
 * corresponding seconds since the epoch. The implementation also allows an
 * integer flag to be passed as pointer to mark cases where the input string
 * might be cancelled ("N/A").
 *
 * @param dt The datetime string, in "yyyy-mm-dd HH:MM" format.
 * @param cancelFlag A flag to check for cancelled input.
 *
 * @return The seconds since the epoch corresponding to the inputted datetime
 * string.
 */
time_t parse_unix_datetime(const char *dt, int *cancelFlag);

/**
 * @brief Compares two datetime strings.
 *
 * This function compares two strings in "yyyy-mm-dd HH:MM" format, using native
 * mathematical operations to convert them into time_t structures. It returns:
 * > -1 if t1 < t2
 * > 0 if t1 == t2
 * > 1 if t1 > t2
 * It will also return error code -2 if there's any conversion error.
 *
 * @param t1 The first datetime string, in the specified format.
 * @param t2 The second datetime string, in the specified format
 *
 * @return The comparison result or error code, as specified.
 */
int compare_unix_datetime(const char *t1, const char *t2);

/**
 * @brief Compares the value of two time_t pointers
 *
 * This function casts two generic pointers into time_t ones and compares their
 * value.
 *
 * @param a A generic pointer to a time_t value.
 * @param b Another generic pointer to a time_t value.
 *
 * @return -1 if *a < *b, 1 if *a > *b or 0 if *a == *b.
 */
int compare_time_pointers(gconstpointer a, gconstpointer b);

/**
 * @brief Converts a time_t strucutue into a string.
 *
 * This function reformats a time_t value into the corresponding datetime string
 * in "yyyy-mm-dd HH:MM" format.
 *
 * @param t The time_t value to format as string.
 * @param buffer The buffer/string where to put the formatted output.
 *
 * @return An error code.
 */
int format_time_t(time_t t, char *buffer);

/**
 * @brief Directly compares time_t values.
 *
 * @param t1 A time_t value to compare.
 * @param t2 Another time_t value to comapare.
 *
 * @return -1 if t1 < t2, 1 if t1 > t2 or 0 if t1 == t2.
 */
int compare_time_t(time_t t1, time_t t2);

/**
 * @brief Parses a date string into a time_t value.
 *
 * This function is in all similar to @ref parse_unix_datetime but for strings
 * in "yyyy-mm-dd" format instead.
 *
 * @param dt The string to parse, in the specified format.
 * @param cancelFlag A flag for checking cancellation status and other errors
 * during parsing.
 *
 * @return The seconds since the epoch corresponding to the inputted date
 * string.
 */
time_t parse_unix_date(const char *dt, int *cancelFlag);

#endif
