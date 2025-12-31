#include "entities/parser.h"
#include <entities/aircrafts.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <validation.h>

struct aircraft {
  gchar *id;
  gchar *manufacturer;
  gchar *model;
  // gint year;
  // gint capacity;
  // gint range;
};

void freeAircraft(gpointer data) {
  Aircraft *acrft = data;
  g_free(acrft->id);
  g_free(acrft->manufacturer);
  g_free(acrft->model);
  g_free(acrft);
}

// Used to clean the invalid line
void cleanupAircrafts(Aircraft *data, gchar **fields) {
  if (data)
    freeAircraft(data);
  if (fields)
    g_strfreev(fields);
}

GHashTable *readAircrafts(const gchar *filename, gint *errorsFlag,
                          GPtrArray *manufacturers) {
  GHashTable *aircraftTable =
      g_hash_table_new_full(g_str_hash, g_str_equal, g_free, freeAircraft);

  FILE *aircrafts = fopen(filename, "r");
  if (!aircrafts) {
    g_hash_table_destroy(aircraftTable);
    return NULL;
  }

  gchar *line = NULL;
  size_t len = 0;
  ssize_t read;
  gchar *headerLine = NULL;

  // Read header
  if ((read = getline(&line, &len, aircrafts)) != -1) {
    g_strchomp(line);
    headerLine = g_strdup(line);
  } else {
    fclose(aircrafts);
    g_hash_table_destroy(aircraftTable);
    return NULL;
  }

  while ((read = getline(&line, &len, aircrafts)) != -1) {
    g_strchomp(line);

    ParsedAircraftF *pf = parseAircraftLineRaw(line);
    if (!parsed_aircraft_ok(pf)) {
      parsed_aircraft_free(pf);
      continue;
    }

    gboolean invalid = FALSE;
    const gchar *fields[6];
    for (int i = 0; i < 6; i++)
      fields[i] = parsed_aircraft_get(pf, i);

    Aircraft *data = g_new0(Aircraft, 1);

    if (!checkAircraftId(fields[0]))
      invalid = TRUE;
    else {
      data->id = g_strdup(fields[0]);

      if (manufacturers && !g_hash_table_contains(aircraftTable, fields[1])) {
        g_ptr_array_add(manufacturers, g_strdup(fields[1]));
      }

      data->manufacturer = g_strdup(fields[1]);
      data->model = g_strdup(fields[2]);

      if (!checkYear(fields[3]))
        invalid = TRUE;
      if (!checkInt(fields[4]) || !checkInt(fields[5]))
        invalid = TRUE;
    }
    if (invalid) {
      logInvalidLine("resultados/aircrafts_errors.csv", headerLine,
                     parsed_aircraft_line(pf));
      cleanupAircrafts(data, NULL);
      *errorsFlag = 1;
      parsed_aircraft_free(pf);
      continue;
    }

    g_hash_table_insert(aircraftTable, g_strdup(fields[0]), data);
    parsed_aircraft_free(pf);
  }

  g_free(headerLine);
  free(line);
  fclose(aircrafts);
  return aircraftTable;
}
// Aircraft *getAircraft(const gchar *id, GHashTable *aircraftsTable) {
//  return g_hash_table_lookup(aircraftsTable, id);
// }

Aircraft *getAircraft(const gchar *id, const GHashTable *aircraftsTable) {
  Aircraft *original = g_hash_table_lookup((GHashTable *)aircraftsTable, id);
  Aircraft *aircraft = g_new(Aircraft, 1);

  aircraft->id = strdup(id);
  aircraft->manufacturer = strdup(original->manufacturer);
  aircraft->model = strdup(original->model);

  return aircraft;
}

gchar *getAircraftId(Aircraft *a) {
  if (!a)
    return NULL;
  return a->id;
}

gchar *getAircraftManufacturer(Aircraft *a) {
  if (!a)
    return NULL;
  return a->manufacturer;
}

gchar *getAircraftModel(Aircraft *a) {
  if (!a)
    return NULL;
  return a->model;
}

// gint getAircraftYear(const Aircraft *a) {
//   if (!a)
//     return -1;
//   return a->year;
// }

// gint getAircraftCapacity(const Aircraft *a) {
//   if (!a)
//     return -1;
//   return a->capacity;
// }

// gint getAircraftRange(const Aircraft *a) {
//   if (!a)
//     return -1;
//   return a->range;
// }
