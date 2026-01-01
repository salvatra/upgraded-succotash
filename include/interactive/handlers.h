#ifndef HANDLERS_H
#define HANDLERS_H

#include <stdio.h>
#include <glib.h>
#include "core/dataset.h"
#include "queries/query4.h"

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

#endif