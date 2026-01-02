/**
 * @file session.h
 * @brief Session state persistence and dataset integrity validation.
 *
 * This module handles the preservation of application state between executions.
 * Specifically, it manages the loading and saving of the active dataset path
 * to a local configuration file (`.dataset_path`), improving the user experience
 * by avoiding repetitive setup.
 *
 * It also acts as a gatekeeper, verifying that any proposed dataset directory
 * adheres to the expected schema (contains all required CSV files) before loading.
 */

#ifndef SESSION_H
#define SESSION_H

/**
 * @brief Persists the current dataset path to a configuration file.
 *
 * Writes the provided directory path to a hidden file named `.dataset_path`
 * in the application's root directory. If the file exists, it is overwritten.
 * This allows the application to "remember" the last working environment upon restart.
 *
 * @param path The null-terminated string containing the directory path to save.
 * If @p path is NULL, the function does nothing.
 */
void save_dataset_path(const char *path);

/**
 * @brief Restores the last used dataset path from the configuration file.
 *
 * Attempts to open and read the `.dataset_path` file. If successful, it reads
 * the path, strips any trailing newline characters (left by editors or file I/O),
 * and returns it.
 *
 * @warning The returned string is **dynamically allocated** using `malloc/strdup`.
 * The caller is responsible for freeing this memory using `free()`.
 *
 * @return A newly allocated string containing the path if the file exists and
 * contains data; @c NULL otherwise (e.g., first run or file deleted).
 */
char *load_dataset_path(void);

/**
 * @brief Validates the structural integrity of a dataset directory.
 *
 * Performs a sanity check on the provided path to ensure it is a valid directory
 * and contains all the mandatory CSV files required for the application schema:
 * - `aircrafts.csv`
 * - `flights.csv`
 * - `passengers.csv` (or `users.csv`)
 * - `airports.csv`
 * - `reservations.csv`
 *
 * @note This function has side effects: it prints specific error messages to @c stdout
 * indicating exactly which file is missing or if the directory is inaccessible.
 *
 * @param dataset_path The directory path to inspect.
 * @return
 * - @c 1 if the directory exists and contains all 5 required files.
 * - @c 0 if validation fails (missing files or invalid path).
 */
int validate_dataset_files(const char *dataset_path);

#endif // SESSION_H