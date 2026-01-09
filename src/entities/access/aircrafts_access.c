#include "entities/access/aircrafts_access.h"
#include "entities/internal/aircrafts_internal.h"
#include <glib.h>

void freeAircraft(gpointer data)
{
  if (!data)
    return;
  Aircraft *acrft = data;
  g_free(acrft->id);
  g_free(acrft->manufacturer);
  g_free(acrft->model);
  g_free(acrft);
}

const Aircraft *getAircraft(const gchar *id, const GHashTable *aircraftsTable)
{
  if (!id || !aircraftsTable)
    return NULL;
  return (const Aircraft *)g_hash_table_lookup((GHashTable *)aircraftsTable, id);
}

const gchar *getAircraftId(const Aircraft *a)
{
  return a ? a->id : NULL;
}

const gchar *getAircraftManufacturer(const Aircraft *a)
{
  return a ? a->manufacturer : NULL;
}

const gchar *getAircraftModel(const Aircraft *a)
{
  return a ? a->model : NULL;
}