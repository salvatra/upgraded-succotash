#include <ctype.h>
#include <entities/reservations.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <validation.h>
#include "entities/parser.h"
#include "entities/flights.h"

struct reservation
{
  gchar *reservation_id;
  gchar **flight_ids;
  int document_no;
  double price;
  // gchar *seat;
  // gchar *extra_luggage;
  // gchar *priority_boarding;
  // gchar *qr_code;
};

void freeReservations(gpointer data)
{
  if (!data)
    return;
  Reservation *reserv = data;
  g_free(reserv->reservation_id);
  g_strfreev(reserv->flight_ids);
  // g_free(reserv->seat);
  // g_free(reserv->qr_code);
  // g_free(reserv->extra_luggage);
  // g_free(reserv->priority_boarding);
  g_free(reserv);
}

GHashTable *readReservations(const char *filename,
                             GHashTable *passengersTable,
                             GHashTable *flightsTable,
                             int *errorsFlag)
{
  FILE *f = fopen(filename, "r");
  if (!f)
    return NULL;

  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  if ((read = getline(&line, &len, f)) == -1)
  {
    free(line);
    fclose(f);
    return NULL;
  }
  g_strchomp(line);
  char *headerLine = g_strdup(line);

  GHashTable *table =
      g_hash_table_new_full(g_str_hash, g_str_equal, g_free, freeReservations);

  while ((read = getline(&line, &len, f)) != -1)
  {
    g_strchomp(line);

    ParsedReservationF *pr = parseReservationLineRaw(line);
    if (!parsed_reservation_ok(pr))
    {
      parsed_reservation_free(pr);
      continue;
    }

    const gchar *fields[8];
    for (int i = 0; i < 8; i++)
      fields[i] = parsed_reservation_get(pr, i);

    gboolean invalid = FALSE;

    /* --- Step 1: Reservation ID --- */
    if (!checkReservationId(fields[0]))
      invalid = TRUE;

    /* --- Step 2: Document number --- */
    int docNo = 0;
    if (!invalid && !checkDocumentNo(fields[2]))
      invalid = TRUE;
    else
    {
      docNo = atoi(fields[2]);
    }

    /* --- Step 3: Passenger exists --- */
    if (!invalid &&
        !g_hash_table_contains(passengersTable, GINT_TO_POINTER(docNo)))
      invalid = TRUE;

    /* --- Step 4: Flight IDs (list validation) --- */
    gchar **flights = NULL;

    if (!invalid)
    {
      const char *s = fields[1];
      size_t l = strlen(s);
      if (l < 2 || s[0] != '[' || s[l - 1] != ']')
      {
        invalid = TRUE;
      }
    }

    if (!invalid)
    {
      flights = parseFlightIds(fields[1]);
      if (!flights || !flights[0])
      {
        invalid = TRUE;
      }
      else
      {
        int count = 0;
        while (flights[count])
          count++;

        if (count == 0 || count > 2)
        {
          invalid = TRUE;
        }
        else if (count == 1)
        {
          if (!g_hash_table_contains(flightsTable, flights[0]))
            invalid = TRUE;
        }
        else
        {
          const Flight *f1 = getFlight(flights[0], flightsTable);
          const Flight *f2 = getFlight(flights[1], flightsTable);

          if (!f1 || !f2 ||
              g_strcmp0(getFlightDestination(f1),
                        getFlightOrigin(f2)) != 0)
            invalid = TRUE;

        }

        if (invalid)
        {
          g_strfreev(flights);
          flights = NULL;
        }
      }
    }

    if (invalid)
    {
      logInvalidLine("resultados/reservations_errors.csv",
                     headerLine,
                     parsed_reservation_line(pr));
      *errorsFlag = 1;
      parsed_reservation_free(pr);
      continue;
    }

    /* --- Create reservation --- */
    Reservation *data = g_new0(Reservation, 1);
    data->reservation_id = g_strdup(fields[0]);
    data->flight_ids = flights;
    data->document_no = docNo;
    data->price = atof(fields[4]);

    g_hash_table_insert(table, g_strdup(fields[0]), data);
    parsed_reservation_free(pr);
  }

  g_free(headerLine);
  free(line);
  fclose(f);
  return table;
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
