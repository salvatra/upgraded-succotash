/**
 * @file flight_parser.h
 * @brief Parser specific for flights.csv.
 *
 * This header defines the function responsible for reading the flights CSV file.
 * It involves complex validation logic, including date comparisons and
 * referential integrity checks against previously loaded entities (specifically aircrafts).
 */

#ifndef FLIGHT_PARSER_H
#define FLIGHT_PARSER_H

#include "../../core/dataset.h"

/**
 * @brief Reads the flights file, validates the data, and populates the dataset.
 *
 * This function parses `flights.csv` line by line. Unlike simple parsers, this
 * function requires the `Dataset` to already contain loaded Aircrafts to perform
 * referential integrity checks.
 *
 * **Validation Rules:**
 * - **CSV Syntax & Data Types**: Ensures correct column count and data types (integers for timestamps, strings for IDs).
 * - **Date Logic**: Validates that the Scheduled Arrival timestamp is strictly greater than
 * the Scheduled Departure timestamp.
 * - **Referential Integrity**: The Flight's Aircraft ID **must** exist in the `aircrafts` hash table
 * currently loaded in the Dataset. If the aircraft is unknown, the flight is considered invalid.
 * - **Airport Validation**: Origin and Destination must be valid 3-letter IATA codes (uppercase).
 * Furthermore, the Origin and Destination **must not** be the same airport.
 *
 * @param filepath [in] Full path to the `flights.csv` file.
 * @param ds [in,out] Pointer to the central Dataset. It is used as a lookup source for validation
 * (checking if the aircraft exists) and as the destination for valid `Flight` entities.
 * @param global_errors [in,out] Pointer to an integer flag. The function sets this to 1
 * if at least one invalid line is detected during parsing.
 */
void io_parse_flights(const char *filepath, Dataset *ds, int *global_errors);

#endif