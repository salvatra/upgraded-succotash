/**
 * @file query_module.h
 * @brief Defines the polymorphic interface for Query Modules.
 *
 * This header establishes the contract (interface) that all specific query implementations
 * (e.g., Query 1, Query 2) must adhere to. It implements a plugin-style architecture
 * where the central orchestrator (queries.c) interacts with individual queries strictly
 * through this opaque interface, ensuring loose coupling and adherence to the
 * Open/Closed Principle.
 *
 * @note This file contains only type definitions and function pointer signatures.
 * It acts purely as a contract definition with no executable logic.
 */

#ifndef QUERY_MODULE_H
#define QUERY_MODULE_H

#include <core/dataset.h>
#include <stdio.h>

/**
 * @typedef QueryInitFunc
 * @brief Function pointer signature for Module Initialization.
 *
 * This function is called exactly once during the application startup (or context build phase).
 * Its purpose is to prepare any specific data structures, indexes, or caches that the
 * query needs to execute efficiently (e.g., sorting arrays, building Fenwick trees).
 *
 * @param ds The main Dataset containing all loaded data (Flights, Passengers, etc.).
 * The module should read from this but NOT modify it.
 *
 * @return A generic pointer (`void*`) to the module's private context data.
 * This context will be passed back to the module during `run` and `destroy`.
 * If the module is stateless (needs no setup), it may return NULL.
 */
typedef void *(*QueryInitFunc)(Dataset *ds);

/**
 * @typedef QueryRunFunc
 * @brief Function pointer signature for Query Execution.
 *
 * This function is called whenever the command interpreter encounters a line referring
 * to this specific query ID. It encapsulates the core business logic of the query.
 *
 * @param ctx       The private context pointer returned by `QueryInitFunc`.
 * The module must cast this back to its specific internal structure type.
 * @param ds        The main Dataset (passed for access to raw data if needed).
 * @param arg1      The first argument string parsed from the input command (e.g., "LIS").
 * @param arg2      The second argument string parsed from the input command (optional, may be NULL).
 * @param isSpecial A boolean flag (0 or 1) indicating if the 'S' variant was requested
 * (e.g., "10F" vs "10"). This usually dictates output formatting.
 * @param output    The open file stream where the query results must be written.
 */
typedef void (*QueryRunFunc)(void *ctx, Dataset *ds, char *arg1, char *arg2, int isSpecial, FILE *output);

/**
 * @typedef QueryDestroyFunc
 * @brief Function pointer signature for Module Cleanup.
 *
 * This function is called exactly once during the application shutdown phase.
 * It is responsible for freeing any memory allocated during `QueryInitFunc`.
 *
 * @param ctx The private context pointer to be freed. If NULL, this function should safely do nothing.
 */
typedef void (*QueryDestroyFunc)(void *ctx);

/**
 * @struct QueryModule
 * @brief Represents a self-contained Query Plugin.
 *
 * This structure acts as a "Virtual Method Table" (vtable) for a generic query.
 * By exposing an instance of this structure, a `.c` file registers itself as a
 * valid query handler without exposing its internal implementation details.
 */
typedef struct
{
    /**
     * @brief The unique numeric identifier of the query (e.g., 1 for Query 1).
     * Used by the manager to map input commands (e.g., "1 ...") to this module.
     */
    int id;

    /**
     * @brief Pointer to the initialization logic.
     * Can be NULL if the query requires no pre-calculation.
     */
    QueryInitFunc init;

    /**
     * @brief Pointer to the execution logic.
     * MUST NOT be NULL.
     */
    QueryRunFunc run;

    /**
     * @brief Pointer to the cleanup logic.
     * Can be NULL if `init` was NULL or if no dynamic memory is held in the context.
     */
    QueryDestroyFunc destroy;
} QueryModule;

#endif // QUERY_MODULE_H