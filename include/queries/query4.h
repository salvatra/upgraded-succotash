/**
 * @file query4.h
 * @brief Logic for Query 4: "Who booked the most reservations?"
 *
 * This header defines the interface for Query 4, which identifies the passenger
 * who made the highest number of reservations within a specific time interval.
 * To handle the large volume of data efficiently, this query utilizes a specific
 * pre-computation structure (`Q4Struct`).
 */

#ifndef QUERY4_H
#define QUERY4_H

#include <glib.h>
#include <stdio.h>
#include <core/dataset.h>

/**
 * @typedef Q4Struct
 * @brief Opaque structure for Query 4 optimization.
 *
 * This structure holds pre-aggregated reservation counts (e.g., grouped by week or
 * total counts) to allow for faster execution when filtering by date ranges.
 * The internal layout is defined in `src/queries/query4.c`.
 */
typedef struct q4_struct Q4Struct;

/**
 * @brief Initializes the optimization structure for Query 4.
 *
 * Scans the entire dataset (reservations) and pre-calculates necessary statistics
 * (e.g., mapping reservation counts per passenger per week).
 * This allows the actual query execution to be significantly faster than iterating
 * through raw reservations every time.
 *
 * @param ds The central Dataset containing all loaded reservations.
 * @return A pointer to the allocated `Q4Struct`. Must be freed with `destroy_Q4_structure`.
 */
// Updated: Receives only the Dataset
Q4Struct *init_Q4_structure(const Dataset *ds);

/**
 * @brief Frees the memory associated with the Query 4 structure.
 * @param q4 The structure to destroy.
 */
void destroy_Q4_structure(Q4Struct *q4);

/**
 * @brief Executes Query 4.
 *
 * Finds the passenger with the most reservations in the specified date range.
 * If multiple passengers have the same maximum count, the one with the
 * lexicographically smaller Document ID is chosen.
 *
 * @param q4_data The pre-computed optimization structure.
 * @param ds The central Dataset (used to retrieve Passenger details like Name/Nationality once the winner is found).
 * @param date_begin The start date string (YYYY/MM/DD) for the interval (inclusive). Can be NULL/Empty for "beginning of time".
 * @param date_end The end date string (YYYY/MM/DD) for the interval (inclusive). Can be NULL/Empty for "end of time".
 * @param output The file stream to write the result to.
 * @param isSpecial Formatting flag (0 for standard output with ';', 1 for special output with '=').
 */
// Updated: Receives Dataset instead of just the passengers hash table
void query4(Q4Struct *q4_data, const Dataset *ds,
            const char *date_begin, const char *date_end,
            FILE *output, int isSpecial);

#endif