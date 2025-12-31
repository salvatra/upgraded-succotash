#include "core/time_utils.h"
#include "glibconfig.h"
#include <entities/passengers.h>
#include <glib.h>
#include <glib/gdate.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <validation.h>
#include "entities/parser.h"

struct passenger {
  int document_number;
  gchar *first_name;
  gchar *last_name;
  time_t dob;
  gchar *nationality;
  char gender;
  //gchar *email;
  //gchar *phone;
  //gchar *address;
  //gchar *photo;
};

void freePassenger(gpointer data) {
  Passenger *passenger = data;
  g_free(passenger->first_name);
  g_free(passenger->last_name);
  g_free(passenger->nationality);
  //g_free(passenger->email);
  //g_free(passenger->phone);
  //g_free(passenger->address);
  //g_free(passenger->photo);
  g_free(passenger);
}

void cleanupPassengers(Passenger *data) {
  if (data)
    freePassenger(data);
}

GHashTable *readPassengers(const char *filename, int *errorsFlag, GPtrArray *nationalities_list) {
    FILE *passengers = fopen(filename, "r");
    if (!passengers) {
        return NULL;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // Read header
    if ((read = getline(&line, &len, passengers)) == -1) {
        fclose(passengers);
        return NULL;
    }
    g_strchomp(line);
    char *headerLine = g_strdup(line);

    GHashTable *passengersTable =
        g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freePassenger);

    // Temporary set to track unique nationalities seen in this file
    GHashTable *seen_nationalities = NULL;
    if (nationalities_list) {
        seen_nationalities = g_hash_table_new(g_str_hash, g_str_equal);
    }

    while ((read = getline(&line, &len, passengers)) != -1) {
        g_strchomp(line);

        ParsedPassengerF *pf = parsePassengerLineRaw(line);

        if (!parsed_passenger_ok(pf)) {
            parsed_passenger_free(pf);
            continue;
        }

        const gchar *fields[10];
        for (int i = 0; i < 10; i++)
            fields[i] = parsed_passenger_get(pf, i);

        gboolean invalid = FALSE;
        time_t dob_t = 0;

        // --- Validations ---
        if (!checkDocumentNo(fields[0])) invalid = TRUE;

        if (!invalid && !checkPassangerGender(fields[5])) invalid = TRUE;

        if (!invalid) {
            dob_t = parse_unix_date(fields[3], NULL);
            if (!checkDate(dob_t)) invalid = TRUE;
        }

        if (!invalid && !checkEmail(fields[6])) invalid = TRUE;

        // Invalid line → log FULL original line
        if (invalid) {
            logInvalidLine("resultados/passengers_errors.csv",
                           headerLine,
                           parsed_passenger_line(pf));

            *errorsFlag = 1;
            parsed_passenger_free(pf);
            continue;
        }

        // --- Create passenger ---
        Passenger *data = g_new0(Passenger, 1);

        data->document_number = atoi(fields[0]);
        data->first_name      = g_strdup(fields[1]);
        data->last_name       = g_strdup(fields[2]);
        data->dob             = dob_t;
        data->nationality     = g_strdup(fields[4]);
        data->gender          = fields[5][1];
        //data->email           = g_strdup(fields[6]);
        //data->phone           = g_strdup(fields[7]);
        //data->address         = g_strdup(fields[8]);
        //data->photo           = g_strdup(fields[9]);

        g_hash_table_insert(passengersTable,
                            GINT_TO_POINTER(data->document_number), data);

        // --- Nationality Autocomplete Logic ---
        if (seen_nationalities && !g_hash_table_contains(seen_nationalities, data->nationality)) {
            // Add to our list (duplicate string because the list has its own lifecycle)
            g_ptr_array_add(nationalities_list, g_strdup(data->nationality));
            // Mark as seen (use the Passenger's string pointer as the key for the set)
            g_hash_table_add(seen_nationalities, data->nationality);
        }

        parsed_passenger_free(pf);
    }

    if (seen_nationalities) {
        g_hash_table_destroy(seen_nationalities);
    }

    g_free(headerLine);
    free(line);
    fclose(passengers);

    return passengersTable;
}

Passenger *getPassenger(int documentNumber, GHashTable *passengersTable) {
  Passenger *src =
      g_hash_table_lookup(passengersTable, GINT_TO_POINTER(documentNumber));
  if (!src) return NULL;

  Passenger *passenger = g_new0(Passenger, 1);
  passenger->document_number = src->document_number;
  passenger->first_name = g_strdup(src->first_name);
  passenger->last_name = g_strdup(src->last_name);
  passenger->dob = src->dob;
  passenger->nationality = g_strdup(src->nationality);
  passenger->gender = src->gender;
  //passenger->email = g_strdup(src->email);
  //passenger->phone = g_strdup(src->phone);
  //passenger->address = g_strdup(src->address);
  //passenger->photo = g_strdup(src->photo);

  return passenger;
}


int getPassengerDocumentNumber(Passenger *p) {
  if (!p)
    return -1;
  return p->document_number;
}


const gchar *getPassengerFirstName(const Passenger *p) {
  if (!p)
    return NULL;
  return p->first_name;
}

const gchar *getPassengerLastName(const Passenger *p) {
  if (!p)
    return NULL;
  return p->last_name;
}

time_t getPassengerDateOfBirth(const Passenger *p) {
    if (!p) return (time_t)0; 
    return p->dob;
}


const gchar *getPassengerNationality(const Passenger *p) {
  if (!p)
    return NULL;
  return p->nationality;
}

char getPassengerGender(const Passenger *p) {
    if (!p) return '\0'; 
    return p->gender;
}
/*
const gchar *getPassengerEmail(const Passenger *p) {
  if (!p)
    return NULL;
  return p->email;
}

const gchar *getPassengerPhone(const Passenger *p) {
  if (!p)
    return NULL;
  return p->phone;
}

const gchar *getPassengerAddress(const Passenger *p) {
  if (!p)
    return NULL;
  return p->address;
}

const gchar *getPassengerPhoto(const Passenger *p) {
  if (!p)
    return NULL;
  return p->photo;
}
*/
