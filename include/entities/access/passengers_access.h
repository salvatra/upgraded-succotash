/**
 * @file passengers_access.h
 * @brief Public interface for accessing and managing Passenger entities.
 *
 * This header defines the opaque `Passenger` structure and provides functions for:
 * - Lifecycle management (freeing memory).
 * - Parsing (reading from CSV).
 * - Data retrieval (getters for entity properties).
 *
 * This module allows other parts of the system to interact with passenger data
 * without exposing the internal memory layout or parsing logic.
 */

#ifndef PASSENGERS_ACCESS_H
#define PASSENGERS_ACCESS_H

#include <glib.h>
#include <time.h>

/**
 * @typedef Passenger
 * @brief Opaque structure representing a single Passenger entity.
 *
 * The internal layout is defined in `src/entities/internal/passengers_internal.h`
 * and is hidden from external modules to enforce encapsulation.
 */
typedef struct passenger Passenger;

/**
 * @brief Memory cleanup function for a Passenger structure.
 *
 * Compatible with `GDestroyNotify`. Frees all dynamically allocated memory associated
 * with the Passenger instance (strings, etc.) and the structure itself.
 *
 * @param data A pointer to the `Passenger` structure to free.
 */
void freePassenger(gpointer data);

/**
 * @brief Parses the Passengers CSV file and populates a hash table.
 *
 * Reads `passengers.csv`, validates fields such as document numbers, email formats,
 * and birth dates. Valid records are stored in a hash table.
 *
 * @param filename The full path to the `passengers.csv` file.
 * @param errorsFlag Pointer to an integer flag. Set to 1 if any invalid lines are encountered/logged.
 * @param nationalities_list A pointer to a `GPtrArray`. If provided, unique nationality
 * strings found during parsing will be added to this array (useful for Query 6).
 * @return A `GHashTable*` containing `Passenger*` values indexed by their Document Number (int).
 * Returns NULL on file error.
 */
GHashTable *readPassengers(const gchar *filename, gint *errorsFlag, GPtrArray *nationalities_list);

/**
 * @brief Retrieves a read-only reference to a passenger from the lookup table.
 *
 * @param documentNumber The unique document number of the passenger (parsed as an integer).
 * @param passengersTable The hash table containing all passenger entities.
 * @return A const pointer to the `Passenger` entity, or NULL if not found.
 */
const Passenger *getPassenger(int documentNumber, const GHashTable *passengersTable);

// --- Getters ---

/**
 * @brief Gets the unique document number of the passenger.
 * @param p The passenger entity.
 * @return The document number as an integer.
 */
int getPassengerDocumentNumber(const Passenger *p);

/**
 * @brief Gets the first name of the passenger.
 * @param p The passenger entity.
 * @return The first name string.
 */
const gchar *getPassengerFirstName(const Passenger *p);

/**
 * @brief Gets the last name of the passenger.
 * @param p The passenger entity.
 * @return The last name string.
 */
const gchar *getPassengerLastName(const Passenger *p);

/**
 * @brief Gets the date of birth of the passenger.
 * @param p The passenger entity.
 * @return The date of birth timestamp (Unix epoch).
 */
time_t getPassengerDateOfBirth(const Passenger *p);

/**
 * @brief Gets the nationality of the passenger.
 * @param p The passenger entity.
 * @return The nationality string (e.g., "Portuguese").
 */
const gchar *getPassengerNationality(const Passenger *p);

/**
 * @brief Gets the gender of the passenger.
 * @param p The passenger entity.
 * @return A char representing gender (e.g., 'M', 'F').
 */
char getPassengerGender(const Passenger *p);

#endif