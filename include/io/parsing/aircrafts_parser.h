/**
 * @file aircraft_parser.h
 * @brief Parser specific for aircrafts.csv.
 *
 * This header defines the function responsible for reading the aircrafts CSV file,
 * parsing each line, validating the data format according to business rules,
 * and populating the central Dataset structure with valid Aircraft entities.
 */

#ifndef AIRCRAFT_PARSER_H
#define AIRCRAFT_PARSER_H

#include "../../core/dataset.h"

/**
 * @brief Reads the aircrafts file and populates the dataset.
 *
 * This function opens the specified file path and iterates through the CSV lines.
 * It applies specific parsing logic for aircraft data.
 *
 * **Validation Rules:**
 * - **CSV Syntax**: The line must contain the expected number of tokens separated by semicolons.
 * - **Aircraft ID**: Must be a valid alphanumeric string, typically following a registration format (e.g., "AB-12345").
 * - **Numeric Constraints**: Capacity and Range (if applicable) must be valid positive integers.
 *
 * Any line failing these checks is logged to an error file, and the entry is skipped.
 *
 * @param filepath [in] Full path to the `aircrafts.csv` file to be read.
 * @param ds [in,out] Pointer to the central Dataset structure where valid aircraft entities will be inserted.
 * @param global_errors [in,out] Pointer to an integer flag. The function sets this to 1
 * if at least one error is encountered during parsing; otherwise, it remains untouched.
 */
void io_parse_aircrafts(const char *filepath, Dataset *ds, int *global_errors);

#endif