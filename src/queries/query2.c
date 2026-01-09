#include <queries/query2.h>
#include "entities/access/aircrafts_access.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <glib.h>

struct aircraftstats
{
  gchar *id;
  gchar *manufacturer;
  gchar *model;
  int count;
};

typedef struct
{
  const Aircraft *aircraft;
  const char *id;
  int count;
} AircraftCount;

// Comparison function: returns true if a < b for min-heap
static inline int acfLess(AircraftCount *a, AircraftCount *b)
{
  if (a->count < b->count)
    return 1;
  if (a->count > b->count)
    return 0;
  return strcmp(a->id, b->id) > 0; // reversed for min-heap
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

static void minheapPush(AircraftCount **heap, int *size, int capacity,
                        AircraftCount *item)
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

AircraftStats **query2(int N, GPtrArray *aircrafts,
                       const char *manufacturerFilter, int *outSize,
                       int *precomputedCounts)
{
  int numAircrafts = aircrafts->len;
  int resultCount = 0;
  AircraftStats **result = NULL;
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

  resultCount = heapSize;

  result = g_new(AircraftStats *, resultCount);

  for (int i = resultCount - 1; i >= 0; i--)
  {
    AircraftCount *node = heap[0];
    heap[0] = heap[heapSize - 1];
    heapSize--;
    heapifyDown(heap, heapSize, 0);

    const Aircraft *ac = node->aircraft;

    AircraftStats *as = g_new(AircraftStats, 1);
    as->id = g_strdup(node->id);
    as->manufacturer = g_strdup(getAircraftManufacturer(ac));
    as->model = g_strdup(getAircraftModel(ac));
    as->count = node->count;

    result[i] = as;
    free(node);
  }

  free(heap);

  if (outSize)
    *outSize = resultCount;

  return result;
}

const gchar *get_aircraftstats_id(const AircraftStats *a)
{
  return (a != NULL) ? a->id : NULL;
}

const gchar *get_aircraftstats_manufacturer(const AircraftStats *a)
{
  return (a != NULL) ? a->manufacturer : NULL;
}

const gchar *get_aircraftstats_model(const AircraftStats *a)
{
  return (a != NULL) ? a->model : NULL;
}

int get_aircraftstats_count(const AircraftStats *a)
{
  return (a != NULL) ? a->count : 0;
}

void free_aircraftstats(AircraftStats *a)
{
  if (!a)
    return;
  g_free(a->id);
  g_free(a->manufacturer);
  g_free(a->model);
  g_free(a);
}

void free_aircraftstats_array(AircraftStats **array, int size)
{
  if (!array)
    return;
  for (int i = 0; i < size; i++)
    free_aircraftstats(array[i]);
  g_free(array);
}