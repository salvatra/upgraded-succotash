/**
 * @file queries.h
 * @brief Query execution orchestration and context management.
 *
 * This module serves as the engine room for the application's query processing.
 * It has two primary responsibilities:
 * 1. **Context Construction:** Pre-calculating auxiliary data structures (indexes, trees, sorted arrays)
 * needed to answer complex queries efficiently (O(1) or O(log N)).
 * 2. **Batch Execution:** Parsing a command file and dispatching execution to specific query logic.
 *
 * @note This module handles the "Business Logic" flow but does not manage the User Interface.
 */

#ifndef QUERIES_H
#define QUERIES_H

#include <core/dataset.h>
#include <queries/query4.h>
#include <stdio.h>

/**
 * @brief Callback function signature for query performance monitoring.
 *
 * This function pointer type allows `runAllQueries` to report execution metrics
 * back to the caller without coupling the query logic to a specific output format.
 *
 * @param queryNum The ID of the query executed (e.g., 1 for Query 1).
 * @param lineNum The line number in the input file where this query command was found.
 * @param elapsed The execution time in seconds.
 * @param ctx User-provided context pointer (passed through from runAllQueries).
 */
typedef void (*QueryStatsCallback)(int queryNum, int lineNum, double elapsed, void *ctx);

/**
 * @brief Pre-calculates and allocates all auxiliary structures required for Query optimization.
 *
 * This function analyzes the loaded Dataset and constructs specialized indexes
 * to ensure specific queries meet performance requirements:
 * - **Query 2:** Builds sorted arrays of aircrafts and flight counts.
 * - **Query 3:** Constructs Fenwick Trees for temporal range queries.
 * - **Query 4:** Initializes the Q4Struct optimization.
 * - **Query 5:** Pre-sorts airline delay statistics.
 * - **Query 6:** Indexes passenger nationalities.
 *
 * @note The function allocates memory for these structures. The caller (usually `interactive_mode` or `main`)
 * becomes responsible for freeing them when they are no longer needed.
 *
 * @param ds The source Dataset.
 * @param aircraftsArray [Output] Handle to the sorted array of aircrafts.
 * @param flightCounts [Output] Handle to the array of flight counts per aircraft.
 * @param airportFtrees [Output] Handle to the Hash Table of Fenwick Trees (for Q3).
 * @param q4_data [Output] Handle to the specialized structure for Q4 optimization.
 * @param airlineDelays [Output] Handle to the sorted list of airline delays (for Q5).
 * @param natTable [Output] Handle to the Hash Table of nationality stats (for Q6).
 *
 * @return 0 on success, non-zero on failure.
 */
int build_query_context(Dataset *ds, GPtrArray **aircraftsArray,
                        int **flightCounts, GHashTable **airportFtrees,
                        Q4Struct **q4_data,
                        GList **airlineDelays, GHashTable **natTable);

/**
 * @brief Executes a batch of queries defined in a text file.
 *
 * This function opens the specified `filePath`, parses it line-by-line,
 * identifies the requested query ID (1-10), and dispatches the execution
 * to the appropriate logic handler.
 *
 * It manages the creation of separate output files for each query command
 * (e.g., `command1_output.txt`).
 *
 * @param ds The Dataset containing the loaded data.
 * @param filePath The full path to the input file containing the query commands.
 * @param callback An optional function pointer to receive execution statistics (useful for benchmarking).
 * Can be NULL if no stats are required.
 * @param ctx An optional generic pointer passed to the callback (user context). Can be NULL.
 */
void runAllQueries(Dataset *ds, const char *filePath,
                   QueryStatsCallback callback, void *ctx);

#endif // QUERIES_H