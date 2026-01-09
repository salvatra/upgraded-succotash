#ifndef VALIDATION_H
#define VALIDATION_H

#include "core/utils.h"
#include <glib.h>

/**
 * @brief Checks if a string represents a valid integer.
 * @param str The string to check.
 * @return TRUE if valid, FALSE otherwise.
 */
gboolean checkInt(const gchar *str);

/**
 * @brief Checks if an aircraft ID is valid.
 * @param id The ID to check.
 * @return TRUE if valid, FALSE otherwise.
 */
gboolean checkAircraftId(const gchar *id);

/**
 * @brief Checks if a date (time_t) is valid.
 * @param dt The date to check.
 * @return TRUE if valid, FALSE otherwise.
 */
gboolean checkDate(const time_t dt);

/**
 * @brief Checks if a datetime (time_t) is valid.
 * @param dt The datetime to check.
 * @return TRUE if valid, FALSE otherwise.
 */
gboolean checkDatetime(const time_t dt);

/**
 * @brief Checks if a year string is valid.
 * @param yearChar The year string.
 * @return TRUE if valid, FALSE otherwise.
 */
gboolean checkYear(const gchar *yearChar);

/**
 * @brief Compares two datetime strings.
 * @param t1 First datetime string.
 * @param t2 Second datetime string.
 * @return TRUE if t1 <= t2 (check implementation specifics), FALSE otherwise.
 */
gboolean compareDatetime(const gchar *t1, const gchar *t2);

/**
 * @brief Checks if an email address is valid.
 * @param email The email string.
 * @return TRUE if valid, FALSE otherwise.
 */
gboolean checkEmail(const char *email);

/**
 * @brief Checks if an airport type is valid.
 * @param type The type string.
 * @return TRUE if valid, FALSE otherwise.
 */
gboolean checkType(const gchar *type);

/**
 * @brief Checks if latitude and longitude coordinates are valid.
 * @param latitude The latitude string.
 * @param longitude The longitude string.
 * @return TRUE if valid, FALSE otherwise.
 */
gboolean checkCoords(const gchar *latitude, const gchar *longitude);

/**
 * @brief Checks if a flight ID is valid.
 * @param id The ID to check.
 * @return TRUE if valid, FALSE otherwise.
 */
gboolean checkFlightId(const gchar *id);

/**
 * @brief Comparison function for aircraft sorting/searching.
 * @param a First element.
 * @param b Second element.
 * @return Comparison result (negative, zero, positive).
 */
gint compareAircraft(const void *a, const void *b);

/**
 * @brief Checks if an aircraft exists in a list of aircraft.
 * @param flightField The aircraft ID from a flight record.
 * @param aircrafts Array of available aircraft.
 * @return TRUE if found, FALSE otherwise.
 */
gboolean checkAircraft(const gchar *flightField, GArray *aircrafts);

/**
 * @brief Checks if destination and origin are different.
 * @param destination Destination code.
 * @param origin Origin code.
 * @return TRUE if different (valid), FALSE otherwise.
 */
gboolean checkDestinationOrigin(const gchar *destination, const gchar *origin);

/**
 * @brief Checks if a reservation ID is valid.
 * @param id The ID to check.
 * @return TRUE if valid, FALSE otherwise.
 */
gboolean checkReservationId(const gchar *id);

/**
 * @brief Checks if a document number is valid.
 * @param no The document number string.
 * @return TRUE if valid, FALSE otherwise.
 */
gboolean checkDocumentNo(const gchar *no);

/**
 * @brief Checks if a passenger gender is valid.
 * @param gender The gender string.
 * @return TRUE if valid, FALSE otherwise.
 */
gboolean checkPassangerGender(const gchar *gender);

/**
 * @brief Checks if a CSV list string is valid.
 * @param list The list string.
 * @return TRUE if valid, FALSE otherwise.
 */
gboolean checkCsvList(const gchar *list);

/**
 * @brief Checks consistency of cancellation status.
 * @param status The flight status.
 * @param act_dep Actual departure time.
 * @param act_arr Actual arrival time.
 * @return TRUE if consistent, FALSE otherwise.
 */
gboolean checkCancellation(const gchar *status, time_t act_dep, time_t act_arr);

/**
 * @brief Checks if a flight is delayed correctly.
 * @param status Flight status.
 * @param scheduledDep Scheduled departure.
 * @param scheduledArr Scheduled arrival.
 * @param actualDep Actual departure.
 * @param actualArr Actual arrival.
 * @param cancel_flag Flag indicating cancellation.
 * @return TRUE if consistent, FALSE otherwise.
 */
gboolean checkDelayed(const gchar *status, time_t scheduledDep,
                      time_t scheduledArr, time_t actualDep, time_t actualArr,
                      int cancel_flag);

/**
 * @brief Checks if reservation flight IDs exist in the reservations list.
 * @param flight The flight ID.
 * @param reservations Array of reservations.
 * @return TRUE if valid, FALSE otherwise.
 */
gboolean checkReservationIdList(const gchar *flight, GArray *reservations);

/**
 * @brief Checks if a passenger document number exists.
 * @param docNumber Document number string.
 * @param passengers Array of passengers.
 * @return TRUE if valid, FALSE otherwise.
 */
gboolean checkDocumentNumber(const gchar *docNumber, GArray *passengers);

/**
 * @brief Checks logic for connecting flights in a reservation.
 * @param DestinationFirstFlight Destination of the first flight.
 * @param OriginSecondFlight Origin of the second flight.
 * @return TRUE if they match (valid connection), FALSE otherwise.
 */
gboolean checkReservationFlightDestOrig(const gchar *DestinationFirstFlight,
                                        const gchar *OriginSecondFlight);

/**
 * @brief Checks if an airport code is valid (e.g., IATA format).
 * @param code The code string.
 * @return TRUE if valid, FALSE otherwise.
 */
gboolean checkAirportCode(const gchar *code);

/**
 * @brief Logs an invalid CSV line to an error file.
 * @param filename The path to the error file.
 * @param header The CSV header.
 * @param line The invalid line content.
 */
void logInvalidLine(const gchar *filename, const gchar *header,
                    const gchar *line);

/**
 * @brief Validates flight connections within reservations and logs errors.
 * @param reservationsTable Hash table of reservations.
 * @param flightsTable Hash table of flights.
 * @param header CSV header for logging.
 * @param errorsFile Path to error file.
 * @return 0 on success, error code otherwise.
 */
gint checkFlightConnection(GHashTable *reservationsTable,
                           const GHashTable *flightsTable, const gchar *header,
                           const gchar *errorsFile);

#endif // !VALIDATION_H