#ifndef PASSENGERS_H
#define PASSENGERS_H

#include <glib.h>

/**
 * @brief Opaque structure representing a passenger (user).
 */
typedef struct passenger Passenger;

/**
 * @brief Frees the memory allocated for a Passenger structure.
 *
 * This function is intended to be used as a GDestroyNotify callback for
 * GHashTable or other GLib data structures.
 *
 * @param data A pointer to the Passenger structure to be freed.
 */
void freePassenger(gpointer data);

/**
 * @brief Reads passenger data from a CSV file and populates a hash table.
 *
 * @param filename The path to the CSV file containing passenger data.
 * @param errorsFlag A pointer to an integer that will be set to 1 if any
 * invalid lines are encountered.
 * @return A GHashTable where keys are passenger document numbers (as integers?
 * or strings? Implementation suggests int key or string key, header implies
 * getPassenger takes int doc number) and values are Passenger pointers.
 */
GHashTable *readPassengers(const gchar *filename, gint *errorsFlag, GPtrArray *nationalities_list);

/**
 * @brief Retrieves a copy of a passenger from the hash table by their document
 * number.
 *
 * This function looks up the passenger in the provided hash table.
 *
 * @param documentNumber The document number of the passenger to retrieve.
 * @param passengersTable The hash table containing passenger data.
 * @return A pointer to the Passenger structure, or NULL if not found.
 */
Passenger *getPassenger(int documentNumber, GHashTable *passengersTable);

/**
 * @brief Gets the document number of a passenger.
 *
 * @param p A pointer to the Passenger structure.
 * @return The passenger's document number.
 */
int getPassengerDocumentNumber(Passenger *p);

/**
 * @brief Gets the first name of a passenger.
 *
 * @param p A pointer to the Passenger structure.
 * @return The first name string.
 */
const gchar *getPassengerFirstName(const Passenger *p);

/**
 * @brief Gets the last name of a passenger.
 *
 * @param p A pointer to the Passenger structure.
 * @return The last name string.
 */
const gchar *getPassengerLastName(const Passenger *p);

/**
 * @brief Gets the date of birth of a passenger.
 *
 * @param p A pointer to the Passenger structure.
 * @return The date of birth as time_t.
 */
time_t getPassengerDateOfBirth(const Passenger *p);

/**
 * @brief Gets the nationality of a passenger.
 *
 * @param p A pointer to the Passenger structure.
 * @return The nationality string.
 */
const gchar *getPassengerNationality(const Passenger *p);

/**
 * @brief Gets the gender of a passenger.
 *
 * @param p A pointer to the Passenger structure.
 * @return The gender character.
 */
char getPassengerGender(const Passenger *p);

/**
 * @brief Gets the email of a passenger.
 *
 * @param p A pointer to the Passenger structure.
 * @return The email string.
 */
const gchar *getPassengerEmail(const Passenger *p);

/**
 * @brief Gets the phone number of a passenger.
 *
 * @param p A pointer to the Passenger structure.
 * @return The phone number string.
 */
const gchar *getPassengerPhone(const Passenger *p);

/**
 * @brief Gets the address of a passenger.
 *
 * @param p A pointer to the Passenger structure.
 * @return The address string.
 */
const gchar *getPassengerAddress(const Passenger *p);

/**
 * @brief Gets the photo URL of a passenger.
 *
 * @param p A pointer to the Passenger structure.
 * @return The photo URL string.
 */
const gchar *getPassengerPhoto(const Passenger *p);

#endif // !PASSENGERS_H
