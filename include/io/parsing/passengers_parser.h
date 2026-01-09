/**
 * @file passenger_parser.h
 * @brief Parser specific for passengers.csv (Users).
 *
 * This header defines the function responsible for reading the passengers CSV file.
 * It handles the ingestion of user data, enforcing strict format validation on personal
 * details such as emails and birth dates, and populating the central Dataset.
 */

#ifndef PASSENGER_PARSER_H
#define PASSENGER_PARSER_H

#include "../../core/dataset.h"

/**
 * @brief Reads the passengers file, validates the data, and populates the dataset.
 *
 * This function parses `passengers.csv` line by line. It is responsible for creating
 * `Passenger` entities and handling the logic for collecting unique nationalities
 * (which are required for specific queries later).
 *
 * **Validation Rules:**
 * - **CSV Syntax**: Checks for the correct number of fields per line.
 * - **Email Format**: Validates the email string structure. It must follow the
 * format `username@domain.tld`, where the TLD has at least 2 characters.
 * - **Date of Birth**: Dates must be valid calendar dates and cannot be in the future
 * relative to the current date (or a specific reference date defined in the system).
 * - **Mandatory Fields**: Nationality, First Name, Last Name, and ID must not be empty or null.
 *
 * **Side Effects:**
 * - Inserts valid `Passenger` objects into the Dataset's passenger hash table.
 * - Updates the Dataset's internal list of unique nationalities.
 *
 * @param filepath [in] Full path to the `passengers.csv` file.
 * @param ds [in,out] Pointer to the central Dataset structure for insertion.
 * @param global_errors [in,out] Pointer to an integer flag. The function sets this to 1
 * if at least one error is encountered during parsing (e.g., malformed email).
 */
void io_parse_passengers(const char *filepath, Dataset *ds, int *global_errors);

#endif