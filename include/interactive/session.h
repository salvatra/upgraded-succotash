#ifndef SESSION_H
#define SESSION_H

/**
 * @file session.h
 * @brief Manages the application session state and dataset persistence.
 *
 * This module handles the loading and saving of the dataset path to a hidden
 * configuration file (.dataset_path), ensuring that the user doesn't need to
 * re-type the path every time they restart the application. It also provides
 * validation logic to ensure the target directory contains all necessary CSV files.
 */

/**
 * @brief Saves the current dataset path to a persistent configuration file.
 *
 * Writes the provided path string to a file named `.dataset_path` in the
 * current working directory. This allows the application to remember the
 * last used dataset upon restart.
 *
 * @param path The null-terminated string containing the directory path to save.
 * If NULL, behavior is undefined (caller should ensure validity).
 */
void save_dataset_path(const char *path);

/**
 * @brief Loads the last used dataset path from the persistent configuration file.
 *
 * Reads the `.dataset_path` file if it exists. It handles reading the line
 * and stripping any trailing newline characters.
 *
 * @note The returned string is dynamically allocated. The caller is responsible
 * for freeing the memory using `free()`.
 *
 * @return A newly allocated string containing the path if the file exists and
 * is readable; NULL otherwise.
 */
char *load_dataset_path(void);

/**
 * @brief Validates if a given directory contains the required Flight System CSV files.
 *
 * Checks for the existence of the directory itself and subsequently verifies
 * the presence of the 5 mandatory files:
 * - aircrafts.csv
 * - flights.csv
 * - passengers.csv
 * - airports.csv
 * - reservations.csv
 *
 * If validation fails, it prints specific error messages to stdout indicating
 * which file is missing or if the path is invalid.
 *
 * @param dataset_path The directory path to validate.
 * @return 1 if the directory and all required files exist; 0 otherwise.
 */
int validate_dataset_files(const char *dataset_path);

#endif