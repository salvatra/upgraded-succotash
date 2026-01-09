/**
 * @file flights_internal.h
 * @brief Internal structure definition for the Flight entity.
 *
 * This header defines the actual memory layout of the `struct flight` and the
 * enumeration for flight status. It is intended for internal use only by the
 * Flight Access module (`src/entities/access/flights_access.c`) to implement
 * getters and parsing logic.
 *
 * @warning **Internal Header**: Do not include this file in public API headers or
 * consumer modules. Use the opaque `Flight` typedef from `flights_access.h` instead.
 */

#ifndef FLIGHTS_INTERNAL_H
#define FLIGHTS_INTERNAL_H

#include <glib.h>
#include <time.h>

/**
 * @enum FlightStatus
 * @brief Enumeration representing the operational status of a flight.
 *
 * Parsed from the string status in the CSV file (e.g., "On Time").
 */
typedef enum
{
    /** Flight departed and arrived according to schedule. */
    FLIGHT_ON_TIME,

    /** Flight was delayed relative to the schedule. */
    FLIGHT_DELAYED,

    /** Flight was cancelled and did not occur. */
    FLIGHT_CANCELLED,

    /** Status could not be parsed or is unknown. */
    FLIGHT_UNKNOWN
} FlightStatus;

/**
 * @struct flight
 * @brief Internal representation of a Flight.
 *
 * Stores the specific attributes of a flight journey as parsed from the dataset.
 * Uses `time_t` for efficient timestamp comparison and storage.
 */
struct flight
{
    /**
     * @brief The unique identifier for the flight.
     * Example: "0000000001"
     */
    gchar *id;

    /**
     * @brief Scheduled departure time (Unix timestamp).
     */
    time_t departure;

    /**
     * @brief Actual departure time (Unix timestamp).
     * May be 0 or -1 if the flight was cancelled.
     */
    time_t actual_departure;

    /**
     * @brief Scheduled arrival time (Unix timestamp).
     */
    time_t arrival;

    /**
     * @brief Actual arrival time (Unix timestamp).
     * May be 0 or -1 if the flight was cancelled.
     */
    time_t actual_arrival;

    /**
     * @brief Current operational status of the flight.
     */
    FlightStatus status;

    /**
     * @brief 3-letter IATA code of the origin airport.
     * Example: "LIS"
     */
    gchar *origin;

    /**
     * @brief 3-letter IATA code of the destination airport.
     * Example: "OPO"
     */
    gchar *destination;

    /**
     * @brief ID of the aircraft used for this flight.
     * Example: "A380-800"
     */
    gchar *aircraft;

    /**
     * @brief Name of the airline operating the flight.
     * Example: "TAP Air Portugal"
     */
    gchar *airline;
};

#endif