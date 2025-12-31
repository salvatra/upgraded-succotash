/**
 * @file fenwick.h
 * @brief Implementation of Fenwick Trees (Bit Indexed Array) for date range
 * queries
 */

#ifndef FENWICK_H
#define FENWICK_H

#include <glib.h>
#include <time.h>

/**
 * @brief Opaque structure representing a Fenwick Tree or Binary Indexed Tree.
 *
 * This is a specilized structure intended to optimize queries based on date
 * range searches.
 */
typedef struct ftree FTree;

/**
 * @brief Builds a GHashTable of Fenwick Trees for each aiport's date
 * information
 *
 * Creates a Fenwick Tree (FTree) structure to hold flight date information for
 * each airport and stores them all in a GHashTable strcuture. This allows range
 * based date queries to execute in a significantly reduced time.
 *
 * @param airportDepartures GHashTable containing information on airports'
 * departures.
 * @param flights Flights hash table.
 * @return A GHashTable mapping airport codes to FTree structures.
 */
GHashTable *getFTrees(GHashTable *airportDepartures, GHashTable *flights);

/**
 * @brief Calculates the prefix sum up to a given index in the Fenwick Tree.
 *
 * @param t Pointer to the FTree structure.
 * @param idx The index up to which the sum is calculated.
 * @return The prefix sum.
 */
int ftree_prefix_sum(FTree *t, int idx);

/**
 * @brief Calculates the sum within a range [left_idx, right_idx].
 *
 * @param t Pointer to the FTree structure.
 * @param left_idx The starting index of the range.
 * @param right_idx The ending index of the range.
 * @return The sum of values within the range.
 */
int ftree_range_sum(FTree *t, int left_idx, int right_idx);

/**
 * @brief Casts a generic pointer to an FTree pointer.
 *
 * This essentially allows access to each FTree residing in the hash table of
 * FTrees by iterating over it.
 *
 * @param data Generic pointer.
 * @return FTree pointer.
 */
FTree *getFTree(gpointer data);

/**
 * @brief Gets the size (N) of the Fenwick Tree.
 *
 * @param tree Pointer to the FTree structure.
 * @return The size of the tree.
 */
int getFtreeN(FTree *tree);

/**
 * @brief Gets the array of dates associated with the Fenwick Tree.
 *
 * @param tree Pointer to the FTree structure.
 * @return Pointer to an array of time_t values.
 */
time_t *getFtreeDates(FTree *tree);

/**
 * @brief Gets the internal BIT array of the Fenwick Tree.
 *
 * @param tree Pointer to the FTree structure.
 * @return Pointer to the integer array.
 */
int *getFTreeBit(FTree *tree);

#endif
