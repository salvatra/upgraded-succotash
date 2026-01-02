/**
 * @file fenwick.h
 * @brief Implementation of Fenwick Trees (Binary Indexed Trees) for efficient temporal queries.
 *
 * This module provides a data structure optimized for calculating prefix sums and range sums
 * over time-series data. It is specifically used to answer queries regarding flight counts
 * within variable date ranges (Query 3) with O(log N) complexity.
 */

#ifndef FENWICK_H
#define FENWICK_H

#include <core/dataset.h>
#include <glib.h>
#include <time.h>

/**
 * @brief Opaque structure representing a Fenwick Tree (or Binary Indexed Tree).
 *
 * The FTree allows for efficient calculation of cumulative frequencies.
 * internally, it maps specific dates (coordinate compression) to indices in a BIT array.
 * * Structure Layout (Internal):
 * - Dates Array: Sorted list of distinct operating days.
 * - BIT Array: Stores the cumulative frequencies.
 * - N: Number of distinct days.
 */
typedef struct ftree FTree;

/**
 * @brief Builds a registry of Fenwick Trees for all airports in the dataset.
 *
 * This function iterates through the dataset to constructs a specialized FTree
 * for every airport that has flight departures.
 *
 * The construction process involves:
 * 1. Identifying distinct flight dates for each airport (Pre-processing).
 * 2. initializing the FTree structure with the correct size.
 * 3. Populating the tree by iterating through the dataset's flights and updating frequencies.
 *
 * @param airportDepartures Helper Hash Table containing pre-calculated distinct dates per airport.
 * @param ds The main Dataset containing all flight information.
 * @return A new GHashTable mapping Airport Codes (char*) -> FTree structures (FTree*).
 */
GHashTable *getFTrees(GHashTable *airportDepartures, const Dataset *ds);

/**
 * @brief Calculates the prefix sum up to a given index.
 *
 * Computes the total number of events (flights) from the first date up to the date
 * corresponding to @p idx.
 *
 * @note Time Complexity: O(log N)
 *
 * @param t Pointer to the FTree structure.
 * @param idx The 1-based index in the BIT array up to which the sum is calculated.
 * @return The cumulative sum.
 */
int ftree_prefix_sum(FTree *t, int idx);

/**
 * @brief Calculates the sum of events within a specific range [left_idx, right_idx].
 *
 * Utilizes the property: Sum([L, R]) = PrefixSum(R) - PrefixSum(L-1).
 *
 * @note Time Complexity: O(log N)
 *
 * @param t Pointer to the FTree structure.
 * @param left_idx The starting index of the range (inclusive).
 * @param right_idx The ending index of the range (inclusive).
 * @return The total count of flights within the range.
 */
int ftree_range_sum(FTree *t, int left_idx, int right_idx);

/**
 * @brief Helper utility to cast a generic pointer to an FTree pointer.
 *
 * Useful when iterating over GHashTables or GLists where values are stored as `gpointer`.
 *
 * @param data Generic pointer (gpointer).
 * @return Casted FTree pointer.
 */
FTree *getFTree(gpointer data);

/**
 * @brief Retrieves the size (N) of the Fenwick Tree.
 *
 * Corresponds to the number of distinct dates monitored by this tree.
 *
 * @param tree Pointer to the FTree structure.
 * @return The size of the tree.
 */
int getFtreeN(FTree *tree);

/**
 * @brief Accesses the internal array of dates associated with the tree.
 *
 * This array maps the 0-based index of the date to the actual `time_t` value.
 * It is used to perform Binary Search to find the correct indices for range queries.
 *
 * @param tree Pointer to the FTree structure.
 * @return Pointer to the array of time_t values.
 */
time_t *getFtreeDates(FTree *tree);

/**
 * @brief Accesses the internal BIT (Binary Indexed Tree) integer array.
 *
 * @warning This exposes internal implementation details and should be used with caution,
 * primarily for debugging or serialization purposes.
 *
 * @param tree Pointer to the FTree structure.
 * @return Pointer to the raw integer array.
 */
int *getFTreeBit(FTree *tree);

#endif // FENWICK_H