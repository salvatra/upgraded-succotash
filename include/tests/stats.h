#ifndef STATS_H
#define STATS_H

#include <glib.h>

/**
 * @brief Opaque structure for collecting test statistics.
 */
typedef struct test_stats TestStats;

/**
 * @brief Initializes a new TestStats structure.
 *
 * @return Pointer to the allocated TestStats.
 */
TestStats *stats_init(void);

/**
 * @brief Records the result of a single test case.
 *
 * @param stats Pointer to the TestStats.
 * @param query_type The type/number of the query (e.g., 1, 2, 3).
 * @param command_number The line number of the command in the input file.
 * @param diff_line The line number where difference occurred (0 if passed).
 */
void stats_add_result(TestStats *stats, int query_type, int command_number, int diff_line);

/**
 * @brief Records the execution time of a query type.
 *
 * @param stats Pointer to the TestStats.
 * @param query_type The type/number of the query.
 * @param time_seconds Execution time in seconds.
 */
void stats_add_timing(TestStats *stats, int query_type, double time_seconds);

/**
 * @brief Prints a detailed statistics report.
 *
 * @param stats Pointer to the TestStats.
 * @param total_time_seconds Total execution time for all tests.
 */
void stats_print_report(TestStats *stats, double total_time_seconds);

/**
 * @brief Frees the TestStats structure.
 *
 * @param stats Pointer to the TestStats.
 */
void stats_free(TestStats *stats);

#endif