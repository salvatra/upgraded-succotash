/**
 * @file pager.h
 * @brief Terminal paging utility for viewing dataset files.
 *
 * This module provides functionality to display large CSV files directly in the
 * terminal using the system's pager (specifically `less`). It handles path
 * resolution and basic on-the-fly formatting (converting CSV delimiters to
 * tabs) to improve readability without modifying the source files.
 */

#ifndef PAGER_H
#define PAGER_H

/**
 * @brief Displays a specific dataset file using the system pager ('less').
 *
 * This function constructs the full path to the requested CSV file based on the
 * selected option. It then creates a pipe to the `less` command, reads the source
 * file line by line, replaces semicolons (';') with tabs/spacing for alignment,
 * and feeds the formatted output to the pager.
 *
 * This allows the user to scroll through large files (like flights.csv) comfortably
 * within the interactive shell.
 *
 * @param base_dataset_path The root directory path of the currently loaded dataset.
 * @param option The numeric identifier of the file to view:
 * - 1: aircrafts.csv
 * - 2: airports.csv
 * - 3: flights.csv
 * - 4: passengers.csv
 * - 5: reservations.csv
 *
 * @return
 * - 0 on success.
 * - 1 if the provided @p option is invalid (out of range 1-5).
 * - 2 if the file could not be opened (e.g., path incorrect or permissions denied).
 */
int page_dataset(char *base_dataset_path, int option);

#endif // PAGER_H