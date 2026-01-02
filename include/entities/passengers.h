/**
 * @file passengers.h
 * @brief Definition and management logic for Passenger entities (Users).
 *
 * This module defines the Passenger entity and handles its parsing, validation, and storage.
 * It is optimized for zero-copy access, ensuring high performance during query execution.
 *
 * @note This header strictly enforces encapsulation. The internal structure is opaque,
 * and memory management is handled entirely by the dataset container.
 */

#ifndef PASSENGERS_H
#define PASSENGERS_H

#include <glib.h>
#include <time.h>

/**
 * @brief Opaque structure representing a Passenger (User).
 *
 * Encapsulates personal data such as Name, Document ID, Nationality, and Birth Date.
 * The internal layout is hidden to ensure data integrity and abstraction.
 */
typedef struct passenger Passenger;

/**
 * @brief Memory cleanup function for a Passenger structure.
 *
 * Designed to be used as a @c GDestroyNotify callback for GHashTables.
 * It performs a deep free, releasing all internal dynamic strings and the structure itself.
 *
 * @param data A generic pointer to the Passenger structure to be freed.
 * If @p data is NULL, does nothing.
 */
void freePassenger(gpointer data);

/**
 * @brief Parses the Passengers CSV file and populates a Hash Table.
 *
 * This function handles file I/O, CSV parsing, and data validation.
 * - Parses Document ID, Name, Birth Date, Gender, etc.
 * - Validates essential fields (e.g., non-empty names).
 * - Aggregates unique nationality strings into @p nationalities_list (if provided)
 * to facilitate Query 6 logic.
 *
 * @param filename The full path to the `passengers.csv` (or users.csv) file.
 * @param errorsFlag Pointer to an integer that will be set to 1 if any invalid lines are encountered.
 * @param nationalities_list Optional @c GPtrArray to store unique nationality strings.
 *
 * @return A new @c GHashTable where:
 * - Key: Document ID (stored as pointer via `GINT_TO_POINTER` or equivalent).
 * - Value: Pointer to @c Passenger structure.
 * Returns @c NULL if the file cannot be opened.
 */
GHashTable *readPassengers(const gchar *filename, gint *errorsFlag, GPtrArray *nationalities_list);

/**
 * @brief Retrieves a read-only reference to a passenger from the repository.
 *
 * **PERFORMANCE CRITICAL:** This function performs a direct O(1) lookup in the hash table
 * and returns a pointer to the existing data. NO copying or allocation is performed.
 *
 * @warning The returned pointer is owned by the Dataset. The caller **MUST NOT**
 * free it, modify it, or store it beyond the lifespan of the dataset.
 *
 * @param documentNumber The unique passenger document number (used as the lookup key).
 * @param passengersTable The hash table containing the passenger dataset.
 * @return A @c const pointer to the Passenger structure, or @c NULL if not found.
 */
const Passenger *getPassenger(int documentNumber, const GHashTable *passengersTable);

/**
 * @brief Accessor for the Passenger Document Number.
 *
 * @param p Pointer to the constant Passenger structure.
 * @return The document number as an integer. Returns 0 if @p p is NULL.
 */
int getPassengerDocumentNumber(const Passenger *p);

/**
 * @brief Accessor for the Passenger's First Name.
 *
 * @param p Pointer to the constant Passenger structure.
 * @return A constant string representing the first name.
 * Returns @c NULL if @p p is NULL.
 */
const gchar *getPassengerFirstName(const Passenger *p);

/**
 * @brief Accessor for the Passenger's Last Name.
 *
 * @param p Pointer to the constant Passenger structure.
 * @return A constant string representing the last name.
 * Returns @c NULL if @p p is NULL.
 */
const gchar *getPassengerLastName(const Passenger *p);

/**
 * @brief Accessor for the Passenger's Date of Birth.
 *
 * @param p Pointer to the constant Passenger structure.
 * @return The birth date as a @c time_t timestamp.
 * Returns 0 if @p p is NULL.
 */
time_t getPassengerDateOfBirth(const Passenger *p);

/**
 * @brief Accessor for the Passenger's Nationality.
 *
 * @param p Pointer to the constant Passenger structure.
 * @return A constant string representing the nationality (e.g., "Portugal").
 * Returns @c NULL if @p p is NULL.
 */
const gchar *getPassengerNationality(const Passenger *p);

/**
 * @brief Accessor for the Passenger's Gender.
 *
 * @param p Pointer to the constant Passenger structure.
 * @return A char representing gender (e.g., 'M', 'F').
 * Returns 0 if @p p is NULL.
 */
char getPassengerGender(const Passenger *p);

#endif // !PASSENGERS_H