#ifndef QUERIES_H
#define QUERIES_H

#include <core/dataset.h>
#include <stdio.h>

/**
 * @brief Opaque handle for the Query Manager.
 * Defines the type so it can be used in queries.c.
 */
typedef struct QueryManager QueryManager;

/**
 * @brief Callback for performance monitoring (stats).
 */
typedef void (*QueryStatsCallback)(int queryNum, int lineNum, double elapsed, void *ctx);

/**
 * @brief Executes all queries listed in the commands file.
 */
void runAllQueries(Dataset *ds, const char *filePath,
                   QueryStatsCallback callback, void *ctx);

#endif // QUERIES_H