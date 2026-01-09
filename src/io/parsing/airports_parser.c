#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "entities/access/airports_access.h"
#include "entities/internal/airports_internal.h"
#include "io/parsing/parser_utils.h"
#include "io/validation/validation_utils.h"
#include "io/validation/airports_validator.h"

static void cleanupAirportData(Airport *data)
{
  if (data)
    freeAirport(data);
}

GHashTable *readAirports(const gchar *filename, gint *errorsFlag, GPtrArray *codes)
{
  GHashTable *airportsTable =
      g_hash_table_new_full(g_str_hash, g_str_equal, g_free, freeAirport);

  FILE *file = fopen(filename, "r");
  if (!file)
  {
    g_hash_table_destroy(airportsTable);
    return NULL;
  }

  gchar *line = NULL;
  size_t len = 0;
  ssize_t read;
  gchar *headerLine = NULL;

  if ((read = getline(&line, &len, file)) != -1)
  {
    g_strchomp(line);
    headerLine = g_strdup(line);
  }
  else
  {
    free(line);
    fclose(file);
    g_hash_table_destroy(airportsTable);
    return NULL;
  }

  while ((read = getline(&line, &len, file)) != -1)
  {
    g_strchomp(line);

    ParsedAirportF *pf = parseAirportLineRaw(line);
    if (!parsed_airport_ok(pf))
    {
      parsed_airport_free(pf);
      continue;
    }

    gboolean invalid = FALSE;
    const gchar *fields[8];
    for (int i = 0; i < 8; i++)
      fields[i] = parsed_airport_get(pf, i);

    Airport *data = g_new0(Airport, 1);

    if (!fields[0] || !checkAirportCode(fields[0]))
    {
      invalid = TRUE;
    }
    else
    {
      if (codes && !g_hash_table_contains(airportsTable, fields[0]))
      {
        g_ptr_array_add(codes, g_strdup(fields[0]));
      }
      data->code = g_strdup(fields[0]);
    }

    if (!invalid)
    {
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

    if (invalid)
    {
      logInvalidLine("resultados/airports_errors.csv", headerLine,
                     parsed_airport_line(pf));
      *errorsFlag = 1;
      cleanupAirportData(data);
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