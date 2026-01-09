#include "entities/access/passengers_access.h"
#include "entities/internal/passengers_internal.h"
#include <stdlib.h>

void freePassenger(gpointer data)
{
  if (!data)
    return;
  Passenger *passenger = data;
  g_free(passenger->first_name);
  g_free(passenger->last_name);
  g_free(passenger->nationality);
  g_free(passenger);
}

const Passenger *getPassenger(int documentNumber, const GHashTable *passengersTable)
{
  if (!passengersTable)
    return NULL;
  return (const Passenger *)g_hash_table_lookup((GHashTable *)passengersTable, GINT_TO_POINTER(documentNumber));
}

int getPassengerDocumentNumber(const Passenger *p)
{
  return p ? p->document_number : -1;
}

const gchar *getPassengerFirstName(const Passenger *p)
{
  return p ? p->first_name : NULL;
}

const gchar *getPassengerLastName(const Passenger *p)
{
  return p ? p->last_name : NULL;
}

time_t getPassengerDateOfBirth(const Passenger *p)
{
  return p ? p->dob : (time_t)0;
}

const gchar *getPassengerNationality(const Passenger *p)
{
  return p ? p->nationality : NULL;
}

char getPassengerGender(const Passenger *p)
{
  return p ? p->gender : '\0';
}