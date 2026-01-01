#ifndef PAGER_H
#define PAGER_H

/**
 * @file pager.h
 * @brief Provides file viewing capabilities using the system pager.
 */

/**
 * @brief Displays a dataset file using the 'less' command line pager.
 *
 * This function constructs the path to the requested CSV file, opens it,
 * and streams its content to a pipe connected to `less`. It performs
 * on-the-fly formatting (replacing semicolons with tabs) to improve
 * readability in the terminal.
 *
 * @param base_dataset_path The directory path where the dataset is located.
 * @param option The index of the file to view:
 * 1 - aircrafts.csv
 * 2 - airports.csv
 * 3 - flights.csv
 * 4 - passengers.csv
 * 5 - reservations.csv
 * @return 0 on success, 1 on invalid option, 2 on file access error.
 */
int page_dataset(char *base_dataset_path, int option);

#endif