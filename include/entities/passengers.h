/**
 * @file passengers.h
 * @brief Passenger entity definition and management.
 *
 * This header defines the interface for the Passenger entity.
 * It is optimized for zero-copy access, ensuring high performance during queries.
 * Memory is managed internally by the dataset container.
 */

#ifndef PASSENGERS_H
#define PASSENGERS_H

#include <glib.h>
#include <time.h>

/**
 * @brief Opaque structure representing a passenger (user).
 *
 * Internal layout is hidden. Users interact via read-only accessors.
 */
typedef struct passenger Passenger;

/**
 * @brief Frees the memory allocated for a Passenger structure.
 *
 * Intended for use as a GDestroyNotify callback.
 * Frees internal strings and the structure itself.
 *
 * @param data A generic pointer to the Passenger structure.
 */
void freePassenger(gpointer data);

/**
 * @brief Reads passenger data from a CSV file and populates a hash table.
 *
 * Parses the CSV, validates records, and stores valid passengers.
 * Also collects unique nationalities into a provided list for statistical purposes.
 *
 * @param filename The path to the CSV file.
 * @param errorsFlag Pointer to an integer updated to 1 if invalid lines are found.
 * @param nationalities_list Optional GPtrArray to store unique nationality strings.
 * @return A GHashTable (Key: Document ID (gpointer/int), Value: Passenger*).
 * Returns NULL on failure.
 */
GHashTable *readPassengers(const gchar *filename, gint *errorsFlag, GPtrArray *nationalities_list);

/**
 * @brief Retrieves a read-only reference to a passenger.
 *
 * **PERFORMANCE CRITICAL:** Returns a direct pointer to the data in the hash table.
 * NO copying or allocation is performed.
 *
 * @warning The returned pointer is owned by the dataset. DO NOT free or modify it.
 *
 * @param documentNumber The passenger's document number (used as key).
 * @param passengersTable The hash table containing passenger data.
 * @return A const pointer to the Passenger structure, or NULL if not found.
 */
const Passenger *getPassenger(int documentNumber, const GHashTable *passengersTable);

/**
 * @brief Gets the passenger's document number.
 *
 * @param p Pointer to the constant Passenger structure.
 * @return The document number as an integer.
 */
int getPassengerDocumentNumber(const Passenger *p);

/**
 * @brief Gets the passenger's first name.
 *
 * @param p Pointer to the constant Passenger structure.
 * @return Constant string of the first name. Owned by the struct.
 */
const gchar *getPassengerFirstName(const Passenger *p);

/**
 * @brief Gets the passenger's last name.
 *
 * @param p Pointer to the constant Passenger structure.
 * @return Constant string of the last name. Owned by the struct.
 */
const gchar *getPassengerLastName(const Passenger *p);

/**
 * @brief Gets the passenger's date of birth.
 *
 * @param p Pointer to the constant Passenger structure.
 * @return Date of birth as time_t.
 */
time_t getPassengerDateOfBirth(const Passenger *p);

/**
 * @brief Gets the passenger's nationality.
 *
 * @param p Pointer to the constant Passenger structure.
 * @return Constant string of the nationality. Owned by the struct.
 */
const gchar *getPassengerNationality(const Passenger *p);

/**
 * @brief Gets the passenger's gender.
 *
 * @param p Pointer to the constant Passenger structure.
 * @return Gender as a char.
 */
char getPassengerGender(const Passenger *p);

#endif // !PASSENGERS_H