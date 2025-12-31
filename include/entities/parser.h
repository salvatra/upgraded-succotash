#ifndef PARSER_H
#define PARSER_H

#include <glib.h>
#include <time.h>
#include <string.h>

/** @brief Opaque structure for parsed generic fields. */
typedef struct ParsedFields ParsedFieldsF;
/** @brief Opaque structure for parsed airport data. */
typedef struct ParsedAirport ParsedAirportF;
/** @brief Opaque structure for parsed aircraft data. */
typedef struct ParsedAircraft ParsedAircraftF;
/** @brief Opaque structure for parsed passenger data. */
typedef struct ParsedPassenger ParsedPassengerF;
/** @brief Opaque structure for parsed reservation data. */
typedef struct ParsedReservation ParsedReservationF;

/**
 * @brief Parses a raw line of flight data.
 * @param line The raw CSV line string.
 * @return A pointer to a ParsedFieldsF structure.
 */
ParsedFieldsF *parseFlightLineRaw(char *line);

/**
 * @brief Checks if the parsed flight fields are valid.
 * @param pf Pointer to the ParsedFieldsF structure.
 * @return TRUE if valid, FALSE otherwise.
 */
gboolean parsed_fields_ok(const ParsedFieldsF *pf);

/**
 * @brief Retrieves a specific field from parsed flight data.
 * @param pf Pointer to the ParsedFieldsF structure.
 * @param index The index of the field to retrieve.
 * @return The field string.
 */
const char *parsed_fields_get(const ParsedFieldsF *pf, int index);

/**
 * @brief Frees the ParsedFieldsF structure.
 * @param pf Pointer to the structure to free.
 */
void parsed_fields_free(ParsedFieldsF *pf);

/**
 * @brief Parses a raw line of airport data.
 * @param line The raw CSV line string.
 * @return A pointer to a ParsedAirportF structure.
 */
ParsedAirportF *parseAirportLineRaw(gchar *line);

/**
 * @brief Checks if the parsed airport data is valid.
 * @param pf Pointer to the ParsedAirportF structure.
 * @return TRUE if valid, FALSE otherwise.
 */
gboolean parsed_airport_ok(const ParsedAirportF *pf);

/**
 * @brief Retrieves a specific field from parsed airport data.
 * @param pf Pointer to the ParsedAirportF structure.
 * @param index The index of the field to retrieve.
 * @return The field string.
 */
const gchar *parsed_airport_get(const ParsedAirportF *pf, int index);

/**
 * @brief Retrieves the original raw line for the airport.
 * @param pf Pointer to the ParsedAirportF structure.
 * @return The raw line string.
 */
const gchar *parsed_airport_line(const ParsedAirportF *pf);

/**
 * @brief Frees the ParsedAirportF structure.
 * @param pf Pointer to the structure to free.
 */
void parsed_airport_free(ParsedAirportF *pf);

/**
 * @brief Parses a raw line of aircraft data.
 * @param line The raw CSV line string.
 * @return A pointer to a ParsedAircraftF structure.
 */
ParsedAircraftF *parseAircraftLineRaw(const gchar *line);

/**
 * @brief Checks if the parsed aircraft data is valid.
 * @param pf Pointer to the ParsedAircraftF structure.
 * @return TRUE if valid, FALSE otherwise.
 */
gboolean parsed_aircraft_ok(const ParsedAircraftF *pf);

/**
 * @brief Retrieves a specific field from parsed aircraft data.
 * @param pf Pointer to the ParsedAircraftF structure.
 * @param index The index of the field to retrieve.
 * @return The field string.
 */
const gchar *parsed_aircraft_get(const ParsedAircraftF *pf, int index);

/**
 * @brief Retrieves the original raw line for the aircraft.
 * @param pf Pointer to the ParsedAircraftF structure.
 * @return The raw line string.
 */
const gchar *parsed_aircraft_line(const ParsedAircraftF *pf);

/**
 * @brief Frees the ParsedAircraftF structure.
 * @param pf Pointer to the structure to free.
 */
void parsed_aircraft_free(ParsedAircraftF *pf);

/**
 * @brief Parses a raw line of passenger data.
 * @param line The raw CSV line string.
 * @return A pointer to a ParsedPassengerF structure.
 */
ParsedPassengerF *parsePassengerLineRaw(const gchar *line);

/**
 * @brief Checks if the parsed passenger data is valid.
 * @param pf Pointer to the ParsedPassengerF structure.
 * @return TRUE if valid, FALSE otherwise.
 */
gboolean parsed_passenger_ok(const ParsedPassengerF *pf);

/**
 * @brief Retrieves a specific field from parsed passenger data.
 * @param pf Pointer to the ParsedPassengerF structure.
 * @param idx The index of the field to retrieve.
 * @return The field string.
 */
const gchar *parsed_passenger_get(const ParsedPassengerF *pf, int idx);

/**
 * @brief Retrieves the original raw line for the passenger.
 * @param pf Pointer to the ParsedPassengerF structure.
 * @return The raw line string.
 */
const gchar *parsed_passenger_line(const ParsedPassengerF *pf);

/**
 * @brief Frees the ParsedPassengerF structure.
 * @param pf Pointer to the structure to free.
 */
void parsed_passenger_free(ParsedPassengerF *pf);

/**
 * @brief Parses a string of flight IDs into an array of strings.
 * @param field The string containing flight IDs (e.g., space-separated).
 * @return A NULL-terminated array of strings.
 */
gchar **parseFlightIds(const gchar *field);

/**
 * @brief Parses a raw line of reservation data.
 * @param line The raw CSV line string.
 * @return A pointer to a ParsedReservationF structure.
 */
ParsedReservationF *parseReservationLineRaw(const gchar *line);

/**
 * @brief Checks if the parsed reservation data is valid.
 * @param pr Pointer to the ParsedReservationF structure.
 * @return TRUE if valid, FALSE otherwise.
 */
gboolean parsed_reservation_ok(const ParsedReservationF *pr);

/**
 * @brief Retrieves a specific field from parsed reservation data.
 * @param pr Pointer to the ParsedReservationF structure.
 * @param idx The index of the field to retrieve.
 * @return The field string.
 */
const gchar *parsed_reservation_get(const ParsedReservationF *pr, int idx);

/**
 * @brief Retrieves the original raw line for the reservation.
 * @param pr Pointer to the ParsedReservationF structure.
 * @return The raw line string.
 */
const gchar *parsed_reservation_line(const ParsedReservationF *pr);

/**
 * @brief Frees the ParsedReservationF structure.
 * @param pr Pointer to the structure to free.
 */
void parsed_reservation_free(ParsedReservationF *pr);

#endif