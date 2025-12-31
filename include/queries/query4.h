#ifndef QUERY4_H
#define QUERY4_H

#include <glib.h>
#include <stdio.h>

/**
 * @brief Opaque structure holding the pre-computed timeline of weekly top 10 passengers.
 * * This structure acts as an index/cheat-sheet, storing only the top 10 winners
 * for every week in the dataset to allow for O(1) query performance.
 */
typedef struct q4_struct Q4Struct;

/**
 * @brief Initializes the Query 4 data structure (The Timeline of Champions).
 * * Pre-computes the top 10 passengers for every week in the dataset by aggregating
 * reservation prices. This is intended to be called once during the dataset loading phase.
 * * @param reservations The hash table of all reservations.
 * @param flights The hash table of all flights (needed to determine dates/weeks).
 * @return A pointer to the initialized Q4Struct.
 */
Q4Struct *init_Q4_structure(GHashTable *reservations, GHashTable *flights);

/**
 * @brief Frees the memory associated with the Query 4 structure.
 * * @param q4 Pointer to the Q4Struct to be destroyed.
 */
void destroy_Q4_structure(Q4Struct *q4);

/**
 * @brief Executes Query 4: Find the passenger with the most appearances in the weekly top 10.
 * * Filters the pre-computed weekly top 10 lists by the given date range (inclusive),
 * counts the frequency of each passenger's appearance, and outputs the details of the
 * passenger with the highest frequency.
 * * @param q4_data Pointer to the pre-computed Q4Struct.
 * @param passengers The hash table of passengers (for retrieving details of the winner).
 * @param date_begin The start date filter (YYYY-MM-DD). If NULL/empty, defaults to the first week.
 * @param date_end The end date filter (YYYY-MM-DD). If NULL/empty, defaults to the last week.
 * @param output The file pointer to write the output to.
 * @param isSpecial Flag indicating if the output format should use '=' instead of ';'.
 */
void query4(Q4Struct *q4_data, GHashTable *passengers,
            const char *date_begin, const char *date_end,
            FILE *output, int isSpecial);

#endif