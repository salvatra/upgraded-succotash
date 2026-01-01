#ifndef QUERIES_H
#define QUERIES_H
#include <core/dataset.h>
#include <queries/query4.h>
#include <stdio.h>

/**
 * @file queries.h
 * @brief Orchestration of query execution and context management.
 * * This module handles the "heavy lifting" of preparing data structures
 * for query execution (Context) and running the batch mode (runAllQueries).
 * It does NOT handle interactive output or user input parsing.
 */

/**
 * @brief Callback function type for performance statistics.
 * Used by runAllQueries to report execution time per query.
 */
typedef void (*QueryStatsCallback)(int queryNum, int lineNum, double elapsed, void *ctx);
/**
 * @brief Builds the context data structures required for executing queries.
 *
 * @param ds Dataset pointer.
 * @param aircraftsArray Pointer to return the array of aircrafts.
 * @param flightCounts Pointer to return the array of flight counts.
 * @param airportFtrees Pointer to return the Fenwick trees hash table.
 * @param q4_data Pointer to return the Query 4 data structure.
 * @param airlineDelays Pointer to return the airline delays list.
 * @param natTable Pointer to return the nationality data hash table.
 * @return 0 on success, 1 on failure.
 */
int build_query_context(Dataset *ds, GPtrArray **aircraftsArray,
                        int **flightCounts, GHashTable **airportFtrees,
                        Q4Struct **q4_data,
                        GList **airlineDelays, GHashTable **natTable);

/**
 * @brief Callback function type for reporting query execution statistics.
 *
 * @param queryNum The identifier of the query type (e.g., 1 for Query 1).
 * @param lineNum The line number in the input file triggering this query.
 * @param elapsed The time taken to execute the query in seconds.
 * @param ctx User-provided context pointer.
 */
typedef void (*QueryStatsCallback)(int queryNum, int lineNum, double elapsed,
                                   void *ctx);

/**
 * @brief Executes a batch of queries from an input file.
 *
 * Reads query commands from the specified file and executes them against the
 * dataset.
 *
 * @param ds A pointer to the Dataset containing loaded data.
 * @param filePath The path to the input file containing query commands.
 * @param callback An optional callback function to receive execution statistics
 * (can be NULL).
 * @param ctx An optional context pointer passed to the callback (can be NULL).
 */
void runAllQueries(Dataset *ds, const char *filePath,
                   QueryStatsCallback callback, void *ctx);

#endif
