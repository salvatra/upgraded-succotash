/**
 * @file reservation_parser.h
 * @brief Parser specific for the reservations.csv file.
 *
 * This header defines the function responsible for reading the reservations CSV file.
 * This is typically the most complex parsing stage as it validates relationships
 * between the reservation, the passenger, and multiple flights.
 */

#ifndef RESERVATION_PARSER_H
#define RESERVATION_PARSER_H

#include "../../core/dataset.h"

/**
 * @brief Reads the reservations file, validates logical dependencies, and populates the dataset.
 *
 * This function processes `reservations.csv`. Since reservations link Passengers to Flights,
 * this parser **must** be executed after `io_parse_passengers` and `io_parse_flights`
 * to perform successful referential integrity checks.
 *
 * **Validation Rules:**
 * - **CSV Syntax**: Ensures the correct number of columns and data types (e.g., price as float).
 * - **Passenger Existence**: The User ID provided in the reservation must correspond to a
 * valid `Passenger` already stored in the dataset.
 * - **Flight Existence**: Every Flight ID listed in the reservation must correspond to a
 * valid `Flight` already stored in the dataset.
 * - **Flight Connectivity**: For reservations with multiple flights (layovers), the logic checks
 * that the **Destination** of Flight *N* matches the **Origin** of Flight *N+1*.
 *
 * @param filepath [in] Full path to the `reservations.csv` file.
 * @param ds [in,out] Pointer to the central Dataset. It is used to look up existing Passengers
 * and Flights for validation, and to store the new `Reservation` entities.
 * @param global_errors [in,out] Pointer to an integer flag. The function sets this to 1
 * if at least one error (syntax or logical) is encountered during parsing.
 */
void io_parse_reservations(const char *filepath, Dataset *ds, int *global_errors);

#endif