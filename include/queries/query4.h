/**
 * @file query4.h
 * @brief Logic for Query 4: Weekly Top Passenger Statistics.
 *
 * This query identifies the passenger who most frequently appears in the "Top 10 Spenders"
 * list across all weeks in a given date range.
 */

#ifndef QUERY4_H
#define QUERY4_H
#include <core/dataset.h>
#include <glib.h>
#include <stdio.h>
#include "queries/query_module.h"

/**
 * @typedef Q4Struct
 * @brief Opaque handle for Query 4's internal index (Weekly Top 10s).
 */
typedef struct q4_struct Q4Struct;

/**
 * @brief Builds the index of Weekly Top 10 Spenders.
 */
Q4Struct *init_Q4_structure(const Dataset *ds);

/**
 * @brief Frees the internal Q4 structure.
 */
void destroy_Q4_structure(Q4Struct *data);

/**
 * @brief Executes Query 4.
 * Finds the most frequent "Top 10" passenger within the date range [date_begin, date_end].
 */
void query4(Q4Struct *q4_data, const Dataset *ds,
            const char *date_begin, const char *date_end,
            FILE *output, int isSpecial);


/**
 * @brief Returns the module definition for the Query Manager.
 */
QueryModule get_query4_module(void);

#endif // QUERY4_H