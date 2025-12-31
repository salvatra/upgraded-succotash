#ifndef QUERIES_H
#define QUERIES_H
#include <core/dataset.h>
#include <stdio.h>

/**
 * @brief Applies specific output formatting to a string based on the special flag.
 *
 * @param isSpecial The formatting flag (0 for normal, 1 for special).
 * @param str The string to format/modify in place.
 */
void specialize(int isSpecial, char *str);

/**
 * @brief Wrapper for Query 1 execution.
 *
 * @param code Airport code.
 * @param isSpecial Formatting flag.
 * @param stream Output stream.
 * @param ds Dataset pointer.
 * @return 0 on success, 1 on failure.
 */
int query1wrapper(char *code, int isSpecial, FILE *stream, Dataset *ds);

/**
 * @brief Wrapper for Query 2 execution.
 *
 * @param number Number of aircrafts (as string).
 * @param manufacturer Manufacturer filter (optional).
 * @param stream Output stream.
 * @param aircraftsArray Array of aircrafts.
 * @param flightCounts Array of flight counts per aircraft.
 * @param isSpecial Formatting flag.
 * @return 0 on success, error code otherwise.
 */
int query2wrapper(char *number, char *manufacturer, FILE *stream,
                  GPtrArray *aircraftsArray, int *flightCounts, int isSpecial);

/**
 * @brief Wrapper for Query 3 execution.
 *
 * @param arg1 Start date.
 * @param arg2 End date.
 * @param stream Output stream.
 * @param ds Dataset pointer.
 * @param isSpecial Formatting flag.
 * @param airportFtrees Hash table of Fenwick trees.
 * @return 0 on success, 1 on failure.
 */
int query3wrapper(char *arg1, char *arg2, FILE *stream, Dataset *ds,
                  int isSpecial, GHashTable *airportFtrees);

/**
 * @brief Wrapper for Query 5 execution.
 *
 * @param arg1 Number of top airlines (as string).
 * @param stream Output stream.
 * @param airlineDelays List of airline delays.
 * @param isSpecial Formatting flag.
 * @return 0 on success, error code otherwise.
 */
int query5wrapper(char *arg1, FILE *stream, GList *airlineDelays,
                  int isSpecial);

/**
 * @brief Wrapper for Query 6 execution.
 *
 * @param arg1 Nationality string.
 * @param stream Output stream.
 * @param natTable Hash table of nationality data.
 * @param isSpecial Formatting flag.
 * @return 0 on success, error code otherwise.
 */
int query6wrapper(char *arg1, FILE *stream, GHashTable *natTable,
                  int isSpecial);

/**
 * @brief Builds the context data structures required for executing queries.
 *
 * @param ds Dataset pointer.
 * @param aircraftsArray Pointer to return the array of aircrafts.
 * @param flightCounts Pointer to return the array of flight counts.
 * @param airportFtrees Pointer to return the Fenwick trees hash table.
 * @param airlineDelays Pointer to return the airline delays list.
 * @param natTable Pointer to return the nationality data hash table.
 * @return 0 on success, 1 on failure.
 */
int build_query_context(Dataset *ds, GPtrArray **aircraftsArray,
                        int **flightCounts, GHashTable **airportFtrees,
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
