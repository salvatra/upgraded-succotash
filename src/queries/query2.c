#include <../include/queries/query2.h>
#include <stdlib.h>
#include <string.h>

struct aircraftstats {
  gchar *id;
  gchar *manufacturer;
  gchar *model;
  int count;
};

typedef struct {
  Aircraft *aircraft;
  const char *id; // cached aircraft ID
  int count;
} AircraftCount;

// Comparison function: returns true if a < b for min-heap
static inline int acfLess(AircraftCount *a, AircraftCount *b) {
  if (a->count < b->count)
    return 1;
  if (a->count > b->count)
    return 0;
  return strcmp(a->id, b->id) > 0; // reversed for min-heap
}

static void heapSwap(AircraftCount **a, AircraftCount **b) {
  AircraftCount *tmp = *a;
  *a = *b;
  *b = tmp;
}

static void heapifyUp(AircraftCount **data, int idx) {
  while (idx > 0) {
    int parent = (idx - 1) / 2;
    if (!acfLess(data[idx], data[parent]))
      break;
    heapSwap(&data[idx], &data[parent]);
    idx = parent;
  }
}

static void heapifyDown(AircraftCount **data, int size, int idx) {
  while (1) {
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
                        AircraftCount *item) {
  if (*size < capacity) {
    heap[*size] = item;
    heapifyUp(heap, *size);
    (*size)++;
  } else if (acfLess(heap[0], item)) {
    AircraftCount *old = heap[0]; // keep the old node
    heap[0] = item;
    heapifyDown(heap, *size, 0);
    free(old); // free the replaced node
  } else {
    // new item is smaller than the current heap min, discard it
    free(item); // also free this to avoid leak
  }
}

// Optimized query2
// This function returns the top-N aircraft (by count) from the given array of
// aircrafts. It can optionally filter by manufacturer name, and it uses a
// min-heap to efficiently keep track of the N aircrafts with the highest
// counts.
AircraftStats **query2(int N, GPtrArray *aircrafts,
                       const char *manufacturerFilter, int *outSize,
                       int *precomputedCounts) {

  // Get the number of aircrafts in the input array.
  int numAircrafts = aircrafts->len;

  // Will hold the final number of results (aircrafts returned).
  int resultCount = 0;

  // Pointer to the final result array (allocated later).
  AircraftStats **result = NULL;

  // Allocate memory for a min-heap that can hold up to N AircraftCount
  // pointers. Each AircraftCount represents one aircraft and its count value.
  AircraftCount **heap = malloc(sizeof(AircraftCount *) * N);

  // Current number of elements stored in the heap.
  int heapSize = 0;

  // Loop through every aircraft in the array.
  for (int i = 0; i < numAircrafts; i++) {

    // Get the precomputed count for this aircraft.
    int count = precomputedCounts[i];

    // Skip aircrafts that have a count of zero (not relevant).
    if (count == 0)
      continue;

    // Get the Aircraft pointer from the GPtrArray at index i.
    Aircraft *ac = g_ptr_array_index(aircrafts, i);

    // Retrieve the manufacturer name of this aircraft.
    const char *manuf = getAircraftManufacturer(ac);

    // If a manufacturer filter is provided (not NULL or empty string),
    // check if this aircraft's manufacturer matches it.
    // If not, skip this aircraft.
    if (manufacturerFilter && *manufacturerFilter &&
        (!manuf || strcmp(manuf, manufacturerFilter) != 0))
      continue;

    // Allocate a new AircraftCount structure for use in the heap.
    AircraftCount *node = malloc(sizeof(AircraftCount));

    // Store a pointer to the aircraft itself.
    node->aircraft = ac;

    // Cache the aircraft ID string pointer.
    node->id = getAircraftId(ac);

    // Store the count value (the metric we're sorting by).
    node->count = count;

    // Insert this node into the min-heap.
    // minheapPush keeps the heap ordered by the smallest count at the root.
    // If the heap exceeds size N, it removes the smallest element
    // automatically.
    minheapPush(heap, &heapSize, N, node);
  }

  // After processing all aircrafts, heapSize holds the number of top aircrafts
  // found.
  resultCount = heapSize;

  // Allocate memory for the final result array, which will contain pointers
  // to AircraftStats structures for each of the top aircrafts.
  result = malloc(sizeof(AircraftStats *) * resultCount);

  // Now extract elements from the heap to fill the result array.
  // Since the heap is a min-heap (smallest element on top),
  // we'll extract elements one by one and store them in reverse order
  // (descending count).
  for (int i = resultCount - 1; i >= 0; i--) {

    // Get the smallest element (at heap root).
    AircraftCount *node = heap[0];

    // Replace the root with the last element in the heap.
    heap[0] = heap[heapSize - 1];

    // Decrease heap size by one.
    heapSize--;

    // Restore the heap property by moving the new root down as needed.
    heapifyDown(heap, heapSize, 0);

    // Get the aircraft pointer from the heap node.
    Aircraft *ac = node->aircraft;

    // Allocate a new AircraftStats struct for the result.
    AircraftStats *as = malloc(sizeof(AircraftStats));

    // Copy the aircraft's ID string (duplicate it so it can be safely freed
    // later).
    as->id = strdup(node->id);

    // Copy the manufacturer string from the Aircraft object.
    as->manufacturer = strdup(getAircraftManufacturer(ac));

    // Copy the model name string from the Aircraft object.
    as->model = strdup(getAircraftModel(ac));

    // Copy the count value.
    as->count = node->count;

    // Store this AircraftStats pointer into the result array at position i.
    // (We fill it backwards so the array is sorted in descending order.)
    result[i] = as;

    // Free the temporary AircraftCount node (no longer needed).
    free(node);
  }

  // Free the heap array itself (the container of pointers).
  // All its individual nodes have already been freed in the loop above.
  free(heap);

  // If the caller provided a valid outSize pointer, store the number of
  // results.
  if (outSize)
    *outSize = resultCount;

  // Return the array of AircraftStats pointers.
  // The caller is responsible for freeing this array and all its contents.
  return result;
}

const gchar *get_aircraftstats_id(const AircraftStats *a) {
  return (a != NULL) ? a->id : NULL;
}

const gchar *get_aircraftstats_manufacturer(const AircraftStats *a) {
  return (a != NULL) ? a->manufacturer : NULL;
}

const gchar *get_aircraftstats_model(const AircraftStats *a) {
  return (a != NULL) ? a->model : NULL;
}

int get_aircraftstats_count(const AircraftStats *a) {
  return (a != NULL) ? a->count : 0;
}

void free_aircraftstats(AircraftStats *a) {
  if (!a)
    return;
  g_free(a->id);
  g_free(a->manufacturer);
  g_free(a->model);
  g_free(a);
}

void free_aircraftstats_array(AircraftStats **array, int size) {
  if (!array)
    return;
  for (int i = 0; i < size; i++)
    free_aircraftstats(array[i]);
  g_free(array);
}
