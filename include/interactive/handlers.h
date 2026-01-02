/**
 * @file handlers.h
 * @brief Command wrappers and input adapters for Query execution.
 *
 * This module acts as the "Controller" layer in the interactive application.
 * It bridges the gap between raw string inputs (from the shell or command line)
 * and the core query logic.
 *
 * Its responsibilities include:
 * 1. Parsing and validating string arguments (e.g., converting "10" to integer 10).
 * 2. Invoking the appropriate core query function.
 * 3. Handling output formatting logic (Standard vs. "Special"/CSV).
 * 4. Error reporting to the output stream.
 */

#ifndef HANDLERS_H
#define HANDLERS_H

#include <stdio.h>
#include <glib.h>
#include "core/dataset.h"
#include "queries/query4.h"

/**
 * @brief Wrapper for executing Query 1 (Airport Details).
 *
 * Validates the airport code provided as a string and retrieves the
 * corresponding summary statistics.
 *
 * @param code The Airport IATA code (string).
 * @param isSpecial Formatting flag. If 1, output may be formatted differently (e.g., "Special" requirements).
 * @param stream The output stream to write results to (stdout or file).
 * @param ds Pointer to the main Dataset.
 *
 * @return 0 on success (airport found and printed), 1 on failure (not found).
 */
int query1wrapper(char *code, int isSpecial, FILE *stream, Dataset *ds);

/**
 * @brief Wrapper for executing Query 2 (Global Flights/Passangers by Aircraft Type).
 *
 * Parses the integer N and the optional manufacturer name, then retrieves the top elements.
 *
 * @param number The number of top elements to retrieve (N) as a string.
 * @param manufacturer Optional manufacturer filter. Can be NULL or empty string.
 * @param stream The output stream to write results to.
 * @param aircraftsArray Pre-sorted array of Aircraft IDs (optimization context).
 * @param flightCounts Array of flight counts indexed to correspond with aircraftsArray.
 * @param isSpecial Formatting flag.
 *
 * @return 0 on success.
 * @return 1 if 'number' is 0 or invalid.
 * @return 2 if the specified manufacturer is not found.
 */
int query2wrapper(char *number, char *manufacturer, FILE *stream,
                  GPtrArray *aircraftsArray, int *flightCounts, int isSpecial);

/**
 * @brief Wrapper for executing Query 3 (Airport Traffic by Date Range).
 *
 * Parses the date strings into time structures and utilizes the Fenwick Tree
 * index to quickly calculate flight counts within the interval.
 *
 * @param arg1 Start date string (YYYY-MM-DD).
 * @param arg2 End date string (YYYY-MM-DD).
 * @param stream The output stream to write results to.
 * @param ds Pointer to the main Dataset.
 * @param isSpecial Formatting flag.
 * @param airportFtrees Hash Table containing the pre-calculated Fenwick Trees for each airport.
 *
 * @return 0 on success.
 * @return 1 if no data is found for the given range or arguments are invalid.
 */
int query3wrapper(char *arg1, char *arg2, FILE *stream, Dataset *ds,
                  int isSpecial, GHashTable *airportFtrees);

/**
 * @brief Wrapper for executing Query 5 (Top N Airlines by Delay).
 *
 * Parses the number N and prints the top airlines with the highest accumulated delays.
 * Uses a pre-calculated list to ensure fast response times.
 *
 * @param arg1 The number of top airlines to retrieve (N) as a string.
 * @param stream The output stream to write results to.
 * @param airlineDelays A pre-sorted GList of airline statistics.
 * @param isSpecial Formatting flag.
 *
 * @return 0 on success.
 * @return Error code if N is invalid or list is empty.
 */
int query5wrapper(char *arg1, FILE *stream, GList *airlineDelays,
                  int isSpecial);

/**
 * @brief Wrapper for executing Query 6 (Airport traffic for specific Nationality).
 *
 * Calculates the number of passengers of a given nationality that passed through
 * each airport.
 *
 * @param arg1 The Nationality string (e.g., "Portuguese").
 * @param stream The output stream to write results to.
 * @param natTable Hash Table mapping nationalities to specific airport count structures.
 * @param isSpecial Formatting flag.
 *
 * @return 0 on success (data found and printed).
 * @return Non-zero if the nationality does not exist in the dataset.
 */
int query6wrapper(char *arg1, FILE *stream, GHashTable *natTable,
                  int isSpecial);

#endif // HANDLERS_H