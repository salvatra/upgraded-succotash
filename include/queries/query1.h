#ifndef QUERY1_H
#define QUERY1_H

#include <glib.h>
#include <core/dataset.h>
#include "queries/query_module.h"

/**
 * @file query1.h
 * @brief Logic for Query 1: Airport Summary Statistics.
 */

/**
 * @brief Executes the core logic for Query 1.
 * Kept exposed for unit testing purposes.
 */
gchar *query1(const gchar *code, const Dataset *ds);

/**
 * @brief Factory function to retrieve the Module definition for Query 1.
 * Used by the Query Manager to register this query.
 */
QueryModule get_query1_module(void);

#endif // QUERY1_H