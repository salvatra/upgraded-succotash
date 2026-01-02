/**
 * @file parser.h
 * @brief Raw CSV parsing, tokenization, and syntactic validation.
 *
 * This module acts as the low-level translation layer between raw text files and
 * the application's Entity logic. It provides functions to:
 * 1. Tokenize CSV lines into accessible fields.
 * 2. Validate basic syntax (e.g., non-empty mandatory fields, correct column counts).
 * 3. Provide safe accessors to string tokens.
 *
 * @note The structures defined here (@c ParsedFieldsF, @c ParsedAirportF, etc.) are
 * **intermediate and temporary**. They are created solely to assist in the construction
 * of the actual entities (Flight, Airport, etc.) and should be freed immediately after use.
 */

#ifndef PARSER_H
#define PARSER_H

#include <glib.h>
#include <time.h>
#include <string.h>

/* --- Opaque Structures for Intermediate Parsed Data --- */

/** @brief Opaque structure holding tokenized fields for a Flight record. */
typedef struct ParsedFields ParsedFieldsF;

/** @brief Opaque structure holding tokenized fields for an Airport record. */
typedef struct ParsedAirport ParsedAirportF;

/** @brief Opaque structure holding tokenized fields for an Aircraft record. */
typedef struct ParsedAircraft ParsedAircraftF;

/** @brief Opaque structure holding tokenized fields for a Passenger record. */
typedef struct ParsedPassenger ParsedPassengerF;

/** @brief Opaque structure holding tokenized fields for a Reservation record. */
typedef struct ParsedReservation ParsedReservationF;

/* --- Flight Parsing --- */

/**
 * @brief Tokenizes a raw line from the `flights.csv` file.
 *
 * Breaks the CSV line into individual string tokens.
 *
 * @param line The raw string from the file.
 * @return A pointer to a new @c ParsedFieldsF structure containing the tokens.
 * Returns @c NULL if memory allocation fails.
 */
ParsedFieldsF *parseFlightLineRaw(char *line);

/**
 * @brief Validates the syntax of the parsed flight data.
 *
 * Checks if the record has the correct number of columns and if mandatory
 * fields are not empty. Does not perform logical validation (e.g., date checks).
 *
 * @param pf Pointer to the parsed flight structure.
 * @return @c TRUE if the syntax is valid, @c FALSE otherwise.
 */
gboolean parsed_fields_ok(const ParsedFieldsF *pf);

/**
 * @brief Retrieves a token at a specific index from the parsed flight.
 *
 * @param pf Pointer to the parsed flight structure.
 * @param index The 0-based column index (e.g., 0 for ID, 1 for Airline).
 * @return A @c const string representing the field value.
 * Returns @c NULL or empty string if index is out of bounds.
 */
const char *parsed_fields_get(const ParsedFieldsF *pf, int index);

/**
 * @brief Destroys the intermediate flight structure.
 *
 * Frees the memory allocated for the structure and its internal tokens.
 *
 * @param pf Pointer to the structure to free.
 */
void parsed_fields_free(ParsedFieldsF *pf);

/* --- Airport Parsing --- */

/**
 * @brief Tokenizes a raw line from the `airports.csv` file.
 *
 * @param line The raw CSV line.
 * @return A pointer to a new @c ParsedAirportF structure.
 */
ParsedAirportF *parseAirportLineRaw(gchar *line);

/**
 * @brief Validates the syntax of the parsed airport data.
 *
 * Checks for mandatory fields such as IATA Code, Name, and Country.
 *
 * @param pf Pointer to the parsed airport structure.
 * @return @c TRUE if valid, @c FALSE otherwise.
 */
gboolean parsed_airport_ok(const ParsedAirportF *pf);

/**
 * @brief Retrieves a token at a specific index from the parsed airport.
 *
 * @param pf Pointer to the parsed airport structure.
 * @param index The column index.
 * @return The field string value.
 */
const gchar *parsed_airport_get(const ParsedAirportF *pf, int index);

/**
 * @brief Retrieves the original raw line used to create this structure.
 *
 * Useful for logging exact errors to the error file when validation fails.
 *
 * @param pf Pointer to the parsed airport structure.
 * @return The original raw CSV line string.
 */
const gchar *parsed_airport_line(const ParsedAirportF *pf);

/**
 * @brief Destroys the intermediate airport structure.
 *
 * @param pf Pointer to the structure to free.
 */
void parsed_airport_free(ParsedAirportF *pf);

/* --- Aircraft Parsing --- */

/**
 * @brief Tokenizes a raw line from the `aircrafts.csv` file.
 *
 * @param line The raw CSV line.
 * @return A pointer to a new @c ParsedAircraftF structure.
 */
ParsedAircraftF *parseAircraftLineRaw(const gchar *line);

/**
 * @brief Validates the syntax of the parsed aircraft data.
 *
 * @param pf Pointer to the parsed aircraft structure.
 * @return @c TRUE if valid, @c FALSE otherwise.
 */
gboolean parsed_aircraft_ok(const ParsedAircraftF *pf);

/**
 * @brief Retrieves a token at a specific index from the parsed aircraft.
 *
 * @param pf Pointer to the parsed aircraft structure.
 * @param index The column index.
 * @return The field string value.
 */
const gchar *parsed_aircraft_get(const ParsedAircraftF *pf, int index);

/**
 * @brief Retrieves the original raw line for the aircraft.
 *
 * @param pf Pointer to the parsed aircraft structure.
 * @return The raw CSV line string.
 */
const gchar *parsed_aircraft_line(const ParsedAircraftF *pf);

/**
 * @brief Destroys the intermediate aircraft structure.
 *
 * @param pf Pointer to the structure to free.
 */
void parsed_aircraft_free(ParsedAircraftF *pf);

/* --- Passenger Parsing --- */

/**
 * @brief Tokenizes a raw line from the `passengers.csv` file.
 *
 * @param line The raw CSV line.
 * @return A pointer to a new @c ParsedPassengerF structure.
 */
ParsedPassengerF *parsePassengerLineRaw(const gchar *line);

/**
 * @brief Validates the syntax of the parsed passenger data.
 *
 * @param pf Pointer to the parsed passenger structure.
 * @return @c TRUE if valid, @c FALSE otherwise.
 */
gboolean parsed_passenger_ok(const ParsedPassengerF *pf);

/**
 * @brief Retrieves a token at a specific index from the parsed passenger.
 *
 * @param pf Pointer to the parsed passenger structure.
 * @param idx The column index.
 * @return The field string value.
 */
const gchar *parsed_passenger_get(const ParsedPassengerF *pf, int idx);

/**
 * @brief Retrieves the original raw line for the passenger.
 *
 * @param pf Pointer to the parsed passenger structure.
 * @return The raw CSV line string.
 */
const gchar *parsed_passenger_line(const ParsedPassengerF *pf);

/**
 * @brief Destroys the intermediate passenger structure.
 *
 * @param pf Pointer to the structure to free.
 */
void parsed_passenger_free(ParsedPassengerF *pf);

/* --- Reservation Parsing --- */

/**
 * @brief Helper function to parse multiple flight IDs from a single field.
 *
 * Reservations may contain multiple flight IDs separated by a delimiter (e.g., spaces).
 * This function splits them into an array.
 *
 * @param field The string containing one or more flight IDs.
 * @return A @c NULL-terminated array of strings (char**).
 * The caller is responsible for freeing this array and its strings.
 */
gchar **parseFlightIds(const gchar *field);

/**
 * @brief Tokenizes a raw line from the `reservations.csv` file.
 *
 * @param line The raw CSV line.
 * @return A pointer to a new @c ParsedReservationF structure.
 */
ParsedReservationF *parseReservationLineRaw(const gchar *line);

/**
 * @brief Validates the syntax of the parsed reservation data.
 *
 * Includes checks for positive prices, valid tax values, and non-empty IDs.
 *
 * @param pr Pointer to the parsed reservation structure.
 * @return @c TRUE if valid, @c FALSE otherwise.
 */
gboolean parsed_reservation_ok(const ParsedReservationF *pr);

/**
 * @brief Retrieves a token at a specific index from the parsed reservation.
 *
 * @param pr Pointer to the parsed reservation structure.
 * @param idx The column index.
 * @return The field string value.
 */
const gchar *parsed_reservation_get(const ParsedReservationF *pr, int idx);

/**
 * @brief Retrieves the original raw line for the reservation.
 *
 * @param pr Pointer to the parsed reservation structure.
 * @return The raw CSV line string.
 */
const gchar *parsed_reservation_line(const ParsedReservationF *pr);

/**
 * @brief Destroys the intermediate reservation structure.
 *
 * @param pr Pointer to the structure to free.
 */
void parsed_reservation_free(ParsedReservationF *pr);

#endif // PARSER_H