#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#include "core/dataset.h"

/**
 * @brief Saves the dataset path to a persistent file.
 *
 * @param path The path string to save.
 */
void save_dataset_path(const char *path);

/**
 * @brief Loads the dataset path from the persistent file.
 *
 * @return A newly allocated string containing the path, or NULL if not found.
 */
char *load_dataset_path(void);

/**
 * @brief Validates if the given directory contains the required CSV files.
 *
 * @param dataset_path The directory path to check.
 * @return 1 if valid, 0 otherwise.
 */
int validate_dataset_files(const char *dataset_path);

/**
 * @brief Displays a dataset file using the 'less' pager.
 *
 * @param base_dataset_path The base directory of the dataset.
 * @param option The file option index (1-5).
 * @return 0 on success, 1 on invalid option, 2 on file error.
 */
int page_dataset(char *base_dataset_path, int option);

/**
 * @brief Prints the main menu options.
 */
void print_options(void);

/**
 * @brief Prints the available queries menu.
 */
void print_queries(void);

/**
 * @brief Generator function for main menu commands.
 * @param str The partial string to match.
 * @param state The state (0 for first call, non-zero for subsequent calls).
 * @return A newly allocated string if a match is found, NULL otherwise.
 */
char *main_command_gen(const char *str, int state);

/**
 * @brief Generator function for query menu commands.
 * @param str The partial string to match.
 * @param state The state.
 * @return A newly allocated string if a match is found, NULL otherwise.
 */
char *query_command_gen(const char *str, int state);

/**
 * @brief Generator function for view menu commands.
 * @param str The partial string to match.
 * @param state The state.
 * @return A newly allocated string if a match is found, NULL otherwise.
 */
char *view_command_gen(const char *str, int state);

/**
 * @brief Generator function for airport codes.
 * @param str The partial string to match.
 * @param state The state.
 * @return A newly allocated string if a match is found, NULL otherwise.
 */
char *airport_code_gen(const char *str, int state);

/**
 * @brief Generator function for aircraft manufacturers.
 * @param str The partial string to match.
 * @param state The state.
 * @return A newly allocated string if a match is found, NULL otherwise.
 */
char *aircraft_manufs_gen(const char *str, int state);

/**
 * @brief Generator function for nationalities.
 * @param str The partial string to match.
 * @param state The state.
 * @return A newly allocated string if a match is found, NULL otherwise.
 */
char *nationality_gen(const char *str, int state);

/**
 * @brief Completion function for main menu.
 * @param str The partial string to match.
 * @param start Start index.
 * @param end End index.
 * @return Array of matches.
 */
char **main_completion(const char *str, int start, int end);

/**
 * @brief Completion function for query menu.
 * @param str The partial string to match.
 * @param start Start index.
 * @param end End index.
 * @return Array of matches.
 */
char **query_completion(const char *str, int start, int end);

/**
 * @brief Completion function for view menu.
 * @param str The partial string to match.
 * @param start Start index.
 * @param end End index.
 * @return Array of matches.
 */
char **view_completion(const char *str, int start, int end);

/**
 * @brief Completion function for airport codes.
 * @param str The partial string to match.
 * @param start Start index.
 * @param end End index.
 * @return Array of matches.
 */
char **airport_code_completion(const char *str, int start, int end);

/**
 * @brief Completion function for aircraft manufacturers.
 * @param str The partial string to match.
 * @param start Start index.
 * @param end End index.
 * @return Array of matches.
 */
char **aircraft_manufs_completion(const char *str, int start, int end);

/**
 * @brief Completion function for nationalities.
 * @param str The partial string to match.
 * @param start Start index.
 * @param end End index.
 * @return Array of matches.
 */
char **nationality_completion(const char *str, int start, int end);

/**
 * @brief Main interactive mode loop.
 *
 * @param ds_ref Pointer to the dataset pointer.
 * @param dataset_path_ptr Pointer to the dataset path string.
 * @return 0 on exit.
 */
int interactive_mode(Dataset **ds_ref, char **dataset_path_ptr);

#endif
