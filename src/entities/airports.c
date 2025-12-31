#include "entities/parser.h"
#include <entities/airports.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <validation.h>

struct airport {
  gchar *code;
  gchar *name;
  gchar *city;
  gchar *country;
  // gdouble latitude;
  // gdouble longitude;
  // gchar *icao;
  gchar *type;
};

void freeAirport(gpointer data) {
  Airport *airport = data;
  g_free(airport->code);
  g_free(airport->city);
  g_free(airport->country);
  // g_free(airport->icao);
  g_free(airport->name);
  g_free(airport->type);
  g_free(airport);
}

void cleanupAirport(Airport *data, gchar **fields) {
  if (data)
    freeAirport(data);
  if (fields)
    g_strfreev(fields);
}

GHashTable *readAirports(const gchar *filename, gint *errosFlag,
                         GPtrArray *codes) {
  GHashTable *airportsTable =
      g_hash_table_new_full(g_str_hash, g_str_equal, g_free, freeAirport);

  FILE *file = fopen(filename, "r");
  if (!file) {
    g_hash_table_destroy(airportsTable);
    return NULL;
  }

  gchar *line = NULL;
  size_t len = 0;
  ssize_t read;
  gchar *headerLine = NULL;

  if ((read = getline(&line, &len, file)) != -1) {
    g_strchomp(line);
    headerLine = g_strdup(line);
  } else {
    fclose(file);
    g_hash_table_destroy(airportsTable);
    return NULL;
  }

  while ((read = getline(&line, &len, file)) != -1) {
    g_strchomp(line);

    ParsedAirportF *pf = parseAirportLineRaw(line);
    if (!parsed_airport_ok(pf)) {
      parsed_airport_free(pf);
      continue;
    }

    gboolean invalid = FALSE;
    const gchar *fields[8];
    for (int i = 0; i < 8; i++)
      fields[i] = parsed_airport_get(pf, i);

    Airport *data = g_new0(Airport, 1);

    if (!fields[0] || !checkAirportCode(fields[0]))
      invalid = TRUE;
    else {
      if (codes && !g_hash_table_contains(airportsTable, fields[0])) {
        g_ptr_array_add(codes, g_strdup(fields[0]));
      }

      data->code = g_strdup(fields[0]);
    }

    if (!invalid) {
      data->name = fields[1] ? g_strdup(fields[1]) : NULL;
      data->city = fields[2] ? g_strdup(fields[2]) : NULL;
      data->country = fields[3] ? g_strdup(fields[3]) : NULL;

      if (!fields[4] || !fields[5] || !checkCoords(fields[4], fields[5]))
        invalid = TRUE;

      if (!fields[7] || !checkType(fields[7]))
        invalid = TRUE;
      else
        data->type = g_strdup(fields[7]);
    }

    if (invalid) {
      logInvalidLine("resultados/airports_errors.csv", headerLine,
                     parsed_airport_line(pf));
      *errosFlag = 1;
      cleanupAirport(data, NULL);
      parsed_airport_free(pf);
      continue;
    }

    g_hash_table_insert(airportsTable, g_strdup(fields[0]), data);
    parsed_airport_free(pf);
  }

  g_free(headerLine);
  free(line);
  fclose(file);
  return airportsTable;
}

// NOTE: Getters for Airport fields

Airport *getAirport(const gchar *code, const GHashTable *airportsTable) {
  if (!code || !airportsTable) {
    return NULL;
  }

  Airport *original = g_hash_table_lookup((GHashTable *)airportsTable, code);
  if (!original) {
    // Key not found
    return NULL;
  }
  Airport *airport = g_new(Airport, 1);

  airport->code = strdup(code);
  airport->name = strdup(original->name);
  airport->city = strdup(original->city);
  airport->country = strdup(original->country);
  airport->type = strdup(original->type);

  return airport;
}

gchar *getAirportCode(Airport *a) {
  if (!a)
    return NULL;
  return a->code;
}

gchar *getAirportName(Airport *a) {
  if (!a)
    return NULL;
  return a->name;
}

gchar *getAirportCity(Airport *a) {
  if (!a)
    return NULL;
  return a->city;
}

gchar *getAirportCountry(Airport *a) {
  if (!a)
    return NULL;
  return a->country;
}

// gdouble getAirportLatitude(const Airport *a) {
//   if (!a) return -1;
// return a->latitude;
// }

// gdouble getAirportLongitude(const Airport *a) {
//   if (!a) return -1;
//   return a->longitude;
// }

// const gchar *getAirportICAO(const Airport *a) {
//   if (!a) return NULL;
//   return a->icao;
// }

gchar *getAirportType(Airport *a) {
  if (!a)
    return NULL;
  return a->type;
}

