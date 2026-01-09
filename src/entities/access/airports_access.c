#include "entities/access/airports_access.h"
#include "entities/internal/airports_internal.h"
#include <glib.h>

void freeAirport(gpointer data)
{
  if (!data)
    return;
  Airport *airport = data;
  g_free(airport->code);
  g_free(airport->city);
  g_free(airport->country);
  g_free(airport->name);
  g_free(airport->type);
  g_free(airport);
}

const Airport *getAirport(const gchar *code, const GHashTable *airportsTable)
{
  if (!code || !airportsTable)
    return NULL;
  return (const Airport *)g_hash_table_lookup((GHashTable *)airportsTable, code);
}

const gchar *getAirportCode(const Airport *a)
{
  return a ? a->code : NULL;
}

const gchar *getAirportName(const Airport *a)
{
  return a ? a->name : NULL;
}

const gchar *getAirportCity(const Airport *a)
{
  return a ? a->city : NULL;
}

const gchar *getAirportCountry(const Airport *a)
{
  return a ? a->country : NULL;
}

const gchar *getAirportType(const Airport *a)
{
  return a ? a->type : NULL;
}