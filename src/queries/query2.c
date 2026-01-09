#include <queries/query2.h>
#include <queries/query_module.h>
#include <core/dataset.h>
#include <entities/access/aircrafts_access.h>
#include <entities/access/flights_access.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <glib.h>

// --- Data Structures ---

struct aircraft_stats
{
  char *id;
  char *manufacturer;
  char *model;
  int count;
};

// Internal structure for heap nodes
typedef struct
{
  const Aircraft *aircraft;
  const char *id;
  int count;
} AircraftCount;

// The Context object for the Module
typedef struct
{
  GPtrArray *aircrafts;
  int *flightCounts;
} Q2Context;

// --- Heap Logic (Optimized for Top N) ---

// Comparison function: returns true if a < b for min-heap logic
static inline int acfLess(AircraftCount *a, AircraftCount *b)
{
  if (a->count < b->count)
    return 1;
  if (a->count > b->count)
    return 0;
  // Tie-breaker: ID (reversed for min-heap behavior to get alphabetical Top N)
  return strcmp(a->id, b->id) > 0;
}

static void heapSwap(AircraftCount **a, AircraftCount **b)
{
  AircraftCount *tmp = *a;
  *a = *b;
  *b = tmp;
}

static void heapifyUp(AircraftCount **data, int idx)
{
  while (idx > 0)
  {
    int parent = (idx - 1) / 2;
    if (!acfLess(data[idx], data[parent]))
      break;
    heapSwap(&data[idx], &data[parent]);
    idx = parent;
  }
}

static void heapifyDown(AircraftCount **data, int size, int idx)
{
  while (1)
  {
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;
    int smallest = idx;
    if (left < size && acfLess(data[left], data[smallest]))
      smallest = left;
    if (right < size && acfLess(data[right], data[smallest]))
      smallest = right;
    if (smallest == idx)
      break;
    heapSwap(&data[idx], &data[smallest]);
    idx = smallest;
  }
}

static void minheapPush(AircraftCount **heap, int *size, int capacity, AircraftCount *item)
{
  if (*size < capacity)
  {
    heap[*size] = item;
    heapifyUp(heap, *size);
    (*size)++;
  }
  else if (acfLess(heap[0], item))
  {
    AircraftCount *old = heap[0];
    heap[0] = item;
    heapifyDown(heap, *size, 0);
    free(old);
  }
  else
  {
    free(item);
  }
}

// --- Core Logic (Heap-based version of query2) ---

AircraftStats **query2(int N, GPtrArray *aircrafts, const char *manufacturerFilter,
                       int *outSize, int *precomputedCounts)
{
  if (N <= 0)
  {
    if (outSize)
      *outSize = 0;
    return NULL;
  }

  int numAircrafts = aircrafts->len;
  AircraftCount **heap = malloc(sizeof(AircraftCount *) * N);
  int heapSize = 0;

  for (int i = 0; i < numAircrafts; i++)
  {
    int count = precomputedCounts[i];
    if (count == 0)
      continue;

    const Aircraft *ac = g_ptr_array_index(aircrafts, i);
    const char *manuf = getAircraftManufacturer(ac);

    if (manufacturerFilter && *manufacturerFilter &&
        (!manuf || strcmp(manuf, manufacturerFilter) != 0))
      continue;

    AircraftCount *node = malloc(sizeof(AircraftCount));
    node->aircraft = ac;
    node->id = getAircraftId(ac);
    node->count = count;

    minheapPush(heap, &heapSize, N, node);
  }

  int resultCount = heapSize;
  AircraftStats **result = g_new(AircraftStats *, resultCount);

  // Extract from heap in descending order
  for (int i = resultCount - 1; i >= 0; i--)
  {
    AircraftCount *node = heap[0];
    heap[0] = heap[heapSize - 1];
    heapSize--;
    heapifyDown(heap, heapSize, 0);

    const Aircraft *ac = node->aircraft;
    AircraftStats *as = g_new(AircraftStats, 1);
    as->id = strdup(node->id);
    as->manufacturer = strdup(getAircraftManufacturer(ac));
    as->model = strdup(getAircraftModel(ac));
    as->count = node->count;

    result[i] = as;
    free(node);
  }

  free(heap);
  if (outSize)
    *outSize = resultCount;
  return result;
}

// --- Accessors (Matching include/queries/query2.h exactly) ---

char *get_aircraftstats_id(AircraftStats *stats)
{
  return (stats != NULL) ? stats->id : NULL;
}

char *get_aircraftstats_manufacturer(AircraftStats *stats)
{
  return (stats != NULL) ? stats->manufacturer : NULL;
}

char *get_aircraftstats_model(AircraftStats *stats)
{
  return (stats != NULL) ? stats->model : NULL;
}

int get_aircraftstats_count(AircraftStats *stats)
{
  return (stats != NULL) ? stats->count : 0;
}

void free_aircraftstats_array(AircraftStats **array, int size)
{
  if (!array)
    return;
  for (int i = 0; i < size; i++)
  {
    free(array[i]->id);
    free(array[i]->manufacturer);
    free(array[i]->model);
    free(array[i]);
  }
  g_free(array);
}


static void *q2_init_wrapper(Dataset *ds)
{
  if (!ds)
    return NULL;
  Q2Context *ctx = g_new0(Q2Context, 1);
  ctx->aircrafts = g_ptr_array_new();

  DatasetIterator *it = dataset_aircraft_iterator_new(ds);
  const Aircraft *ac;
  while ((ac = dataset_iterator_next(it)) != NULL)
  {
    g_ptr_array_add(ctx->aircrafts, (gpointer)ac);
  }
  dataset_iterator_free(it);

  int numAircrafts = ctx->aircrafts->len;
  ctx->flightCounts = calloc(numAircrafts, sizeof(int));
  GHashTable *idToIndex = g_hash_table_new(g_str_hash, g_str_equal);

  for (int i = 0; i < numAircrafts; i++)
  {
    const Aircraft *a = g_ptr_array_index(ctx->aircrafts, i);
    g_hash_table_insert(idToIndex, (gpointer)getAircraftId(a), GINT_TO_POINTER(i + 1));
  }

  it = dataset_flight_iterator_new(ds);
  const Flight *f;
  while ((f = dataset_iterator_next(it)) != NULL)
  {
    if (strcmp(getFlightStatus(f), "Cancelled") == 0)
      continue;
    const char *acId = getFlightAircraft(f);
    if (!acId)
      continue;
    gpointer idxPtr = g_hash_table_lookup(idToIndex, acId);
    if (idxPtr)
    {
      ctx->flightCounts[GPOINTER_TO_INT(idxPtr) - 1]++;
    }
  }
  dataset_iterator_free(it);
  g_hash_table_destroy(idToIndex);
  return ctx;
}

static void q2_run_wrapper(void *ctx_void, Dataset *ds, char *arg1, char *arg2, int isSpecial, FILE *output)
{
  Q2Context *ctx = (Q2Context *)ctx_void;
  if (!ctx || !arg1)
    return;
  (void)ds;

  int N = atoi(arg1);
  const char *filter = (arg2 && *arg2) ? arg2 : NULL;
  int size = 0;

  AircraftStats **top = query2(N, ctx->aircrafts, filter, &size, ctx->flightCounts);

  if (top && size > 0)
  {
    char sep = isSpecial ? '=' : ';';
    for (int i = 0; i < size; i++)
    {
      fprintf(output, "%s%c%s%c%s%c%d\n",
              get_aircraftstats_id(top[i]), sep,
              get_aircraftstats_manufacturer(top[i]), sep,
              get_aircraftstats_model(top[i]), sep,
              get_aircraftstats_count(top[i]));
    }
    free_aircraftstats_array(top, size);
  }
  else
  {
    fprintf(output, "\n");
  }
}

static void q2_destroy_wrapper(void *ctx_void)
{
  Q2Context *ctx = (Q2Context *)ctx_void;
  if (ctx)
  {
    if (ctx->aircrafts)
      g_ptr_array_free(ctx->aircrafts, TRUE);
    if (ctx->flightCounts)
      free(ctx->flightCounts);
    g_free(ctx);
  }
}

QueryModule get_query2_module(void)
{
  QueryModule mod = {
      .id = 2,
      .init = q2_init_wrapper,
      .run = q2_run_wrapper,
      .destroy = q2_destroy_wrapper};
  return mod;
}