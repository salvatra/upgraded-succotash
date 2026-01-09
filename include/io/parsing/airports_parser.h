/**
 * @file airport_parser.h
 * @brief Parser specific for airports.csv.
 *
 * This header defines the function responsible for reading the airports CSV file,
 * parsing each line, performing rigorous validation on airport codes and coordinates,
 * and populating the central Dataset structure with valid Airport entities.
 */

#ifndef AIRPORT_PARSER_H
#define AIRPORT_PARSER_H

#include "../../core/dataset.h"

/**
 * @brief Reads the airports file and populates the dataset.
 *
 * This function handles the ingestion of airport data. It opens the file at the specified
 * path and iterates through every line, applying the validation logic described below.
 * Valid airports are stored in the Dataset's airport hash table.
 *
 * **Validation Rules:**
 * - **CSV Syntax**: Ensures the line has the correct number of tokens.
 * - **IATA Code**: Must be exactly 3 uppercase letters (e.g., "LIS", "JFK").
 * - **Coordinates**: Validates that Latitude (-90 to 90) and Longitude (-180 to 180) are valid floating-point numbers.
 * - **Mandatory Fields**: Checks that essential fields like City and Country are not empty.
 * - **Airport Type**: Validates the type field (e.g., "small_airport", "heliport", "closed").
 * *Note*: Even if specific fields (like specific metadata) are not stored in the final struct for queries,
 * they are validated to ensure the integrity of the input line.
 *
 * @param filepath [in] Full path to the `airports.csv` file.
 * @param ds [in,out] Pointer to the central Dataset structure where valid airport entities will be inserted.
 * @param global_errors [in,out] Pointer to an integer flag. The function sets this to 1
 * if at least one error is encountered during parsing (used to report "Dataset loaded with errors").
 */
void io_parse_airports(const char *filepath, Dataset *ds, int *global_errors);

#endif