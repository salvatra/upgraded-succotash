/**
 * @file parser_utils.h
 * @brief Low-level utilities for CSV tokenization and raw field extraction.
 *
 * This header defines a set of opaque structures and helper functions responsible
 * for the initial "raw" parsing of CSV lines. It handles the splitting of strings
 * by delimiters (semicolons), managing memory for tokens, and providing safe access
 * to fields by index.
 *
 * This layer separates the *syntactic* parsing (splitting strings) from the
 * *semantic* parsing (validating data and creating entities).
 */

#ifndef PARSER_UTILS_H
#define PARSER_UTILS_H

#include <glib.h>

/* --- Opaque Structures for Intermediate Parsed Data --- */

/**
 * @typedef ParsedFieldsF
 * @brief Opaque wrapper for parsed Flight fields.
 * Validates column count for flights.csv.
 */
typedef struct ParsedFields ParsedFieldsF;

/**
 * @typedef ParsedAirportF
 * @brief Opaque wrapper for parsed Airport fields.
 * Validates column count for airports.csv.
 */
typedef struct ParsedAirport ParsedAirportF;

/**
 * @typedef ParsedAircraftF
 * @brief Opaque wrapper for parsed Aircraft fields.
 * Validates column count for aircrafts.csv.
 */
typedef struct ParsedAircraft ParsedAircraftF;

/**
 * @typedef ParsedPassengerF
 * @brief Opaque wrapper for parsed Passenger fields.
 * Validates column count for passengers.csv.
 */
typedef struct ParsedPassenger ParsedPassengerF;

/**
 * @typedef ParsedReservationF
 * @brief Opaque wrapper for parsed Reservation fields.
 * Validates column count for reservations.csv.
 */
typedef struct ParsedReservation ParsedReservationF;

/* --- Flight Parsing --- */

/**
 * @brief Tokenizes a raw line from flights.csv.
 *
 * Copies the input string and splits it by the standard delimiter (';').
 *
 * @param line The raw CSV line string.
 * @return A pointer to a `ParsedFieldsF` object containing the tokens.
 * Must be freed with `parsed_fields_free`.
 */
ParsedFieldsF *parseFlightLineRaw(char *line);

/**
 * @brief Checks if the flight line was parsed successfully.
 *
 * @param pf The parsed fields object.
 * @return `TRUE` if the number of tokens matches the expected column count (12), `FALSE` otherwise.
 */
gboolean parsed_fields_ok(const ParsedFieldsF *pf);

/**
 * @brief Retrieves a specific field token by index.
 *
 * @param pf The parsed fields object.
 * @param index The 0-based column index.
 * @return The string token at that index, or NULL if out of bounds.
 */
const char *parsed_fields_get(const ParsedFieldsF *pf, int index);

/**
 * @brief Frees the parsed fields object and its internal data.
 * @param pf The object to free.
 */
void parsed_fields_free(ParsedFieldsF *pf);

/* --- Airport Parsing --- */

/**
 * @brief Tokenizes a raw line from airports.csv.
 * @param line The raw CSV line string.
 * @return A pointer to the opaque parsed object.
 */
ParsedAirportF *parseAirportLineRaw(gchar *line);

/**
 * @brief Checks if the airport line has the correct number of columns.
 * @param pf The parsed airport object.
 * @return `TRUE` if valid, `FALSE` otherwise.
 */
gboolean parsed_airport_ok(const ParsedAirportF *pf);

/**
 * @brief Gets a field token by index.
 * @param pf The parsed airport object.
 * @param index The column index.
 * @return The token string.
 */
const gchar *parsed_airport_get(const ParsedAirportF *pf, int index);

/**
 * @brief Retrieves the original full line (useful for error logging).
 * @param pf The parsed airport object.
 * @return The original CSV line string.
 */
const gchar *parsed_airport_line(const ParsedAirportF *pf);

/**
 * @brief Frees the parsed airport object.
 * @param pf The object to free.
 */
void parsed_airport_free(ParsedAirportF *pf);

/* --- Aircraft Parsing --- */

/**
 * @brief Tokenizes a raw line from aircrafts.csv.
 * @param line The raw CSV line string.
 * @return A pointer to the opaque parsed object.
 */
ParsedAircraftF *parseAircraftLineRaw(const gchar *line);

/**
 * @brief Checks if the aircraft line has the correct number of columns.
 * @param pf The parsed aircraft object.
 * @return `TRUE` if valid.
 */
gboolean parsed_aircraft_ok(const ParsedAircraftF *pf);

/**
 * @brief Gets a field token by index.
 * @param pf The parsed aircraft object.
 * @param index The column index.
 * @return The token string.
 */
const gchar *parsed_aircraft_get(const ParsedAircraftF *pf, int index);

/**
 * @brief Retrieves the original full line.
 * @param pf The parsed aircraft object.
 * @return The original CSV line string.
 */
const gchar *parsed_aircraft_line(const ParsedAircraftF *pf);

/**
 * @brief Frees the parsed aircraft object.
 * @param pf The object to free.
 */
void parsed_aircraft_free(ParsedAircraftF *pf);

/* --- Passenger Parsing --- */

/**
 * @brief Tokenizes a raw line from passengers.csv.
 * @param line The raw CSV line string.
 * @return A pointer to the opaque parsed object.
 */
ParsedPassengerF *parsePassengerLineRaw(const gchar *line);

/**
 * @brief Checks if the passenger line has the correct number of columns.
 * @param pf The parsed passenger object.
 * @return `TRUE` if valid.
 */
gboolean parsed_passenger_ok(const ParsedPassengerF *pf);

/**
 * @brief Gets a field token by index.
 * @param pf The parsed passenger object.
 * @param idx The column index.
 * @return The token string.
 */
const gchar *parsed_passenger_get(const ParsedPassengerF *pf, int idx);

/**
 * @brief Retrieves the original full line.
 * @param pf The parsed passenger object.
 * @return The original CSV line string.
 */
const gchar *parsed_passenger_line(const ParsedPassengerF *pf);

/**
 * @brief Frees the parsed passenger object.
 * @param pf The object to free.
 */
void parsed_passenger_free(ParsedPassengerF *pf);

/* --- Reservation Parsing --- */

/**
 * @brief Helper function to parse a formatted list of Flight IDs.
 *
 * Handles the specific format found in reservations: `['ID1', 'ID2', ...]`.
 * It strips brackets and quotes, splitting the content into a string array.
 *
 * @param field The raw string field containing the list (e.g., "['A', 'B']").
 * @return A NULL-terminated array of strings (char**), or NULL if parsing fails.
 * The caller must free this array using `g_strfreev`.
 */
gchar **parseFlightIds(const gchar *field);

/**
 * @brief Tokenizes a raw line from reservations.csv.
 * @param line The raw CSV line string.
 * @return A pointer to the opaque parsed object.
 */
ParsedReservationF *parseReservationLineRaw(const gchar *line);

/**
 * @brief Checks if the reservation line has the correct number of columns.
 * @param pr The parsed reservation object.
 * @return `TRUE` if valid.
 */
gboolean parsed_reservation_ok(const ParsedReservationF *pr);

/**
 * @brief Gets a field token by index.
 * @param pr The parsed reservation object.
 * @param idx The column index.
 * @return The token string.
 */
const gchar *parsed_reservation_get(const ParsedReservationF *pr, int idx);

/**
 * @brief Retrieves the original full line.
 * @param pr The parsed reservation object.
 * @return The original CSV line string.
 */
const gchar *parsed_reservation_line(const ParsedReservationF *pr);

/**
 * @brief Frees the parsed reservation object.
 * @param pr The object to free.
 */
void parsed_reservation_free(ParsedReservationF *pr);

#endif