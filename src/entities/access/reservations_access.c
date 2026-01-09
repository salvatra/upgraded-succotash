#include "entities/access/reservations_access.h"
#include "entities/internal/reservations_internal.h"
#include <glib.h>

void freeReservations(gpointer data)
{
  if (!data)
    return;
  Reservation *reserv = data;
  g_free(reserv->reservation_id);
  g_strfreev(reserv->flight_ids);
  g_free(reserv);
}

const Reservation *getReservation(const gchar *id, const GHashTable *reservationsTable)
{
  if (!id || !reservationsTable)
    return NULL;
  return (const Reservation *)g_hash_table_lookup((GHashTable *)reservationsTable, id);
}

const gchar *getReservationId(const Reservation *r)
{
  return r ? r->reservation_id : NULL;
}

gchar **getReservationFlightIds(const Reservation *r)
{
  return r ? r->flight_ids : NULL;
}

int getReservationDocumentNo(const Reservation *r)
{
  return r ? r->document_no : -1;
}

gfloat getReservationPrice(const Reservation *r)
{
  return r ? r->price : -1.0f;
}