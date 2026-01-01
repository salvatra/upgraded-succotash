#ifndef COMPLETION_H
#define COMPLETION_H

#include "core/dataset.h"

/**
 * @file completion.h
 * @brief Handles GNU Readline tab-completion logic.
 *
 * This module provides custom completion functions for the interactive shell.
 * It manages static references to dataset arrays (airport codes, manufacturers, etc.)
 * to allow the readline library to offer context-aware suggestions.
 */

/**
 * @brief Updates the internal pointers used for autocomplete suggestions.
 *
 * Since readline generators often require static context, this function
 * updates the module's local references to the active dataset's arrays.
 * It should be called whenever the dataset is reloaded.
 *
 * @param ds Pointer to the active Dataset structure.
 */
void update_completion_context(Dataset *ds);

/**
 * @brief Generator function for shell commands (starts with !).
 */
char *shell_cmd_generator(const char *str, int state);

char **main_completion(const char *str, int start, int end);

/**
 * @brief Completion hook for the main menu commands.
 *
 * @param str The partial string to match.
 * @param start Start index of the word.
 * @param end End index of the word.
 * @return Array of matches (NULL terminated) or NULL.
 */
char **main_completion(const char *str, int start, int end);

/**
 * @brief Completion hook for query selection menu.
 */
char **query_completion(const char *str, int start, int end);

/**
 * @brief Completion hook for the view file menu.
 */
char **view_completion(const char *str, int start, int end);

/**
 * @brief Completion hook for airport codes (Query 1).
 * Uses the dataset's airport code list.
 */
char **airport_code_completion(const char *str, int start, int end);

/**
 * @brief Completion hook for aircraft manufacturers (Query 2).
 * Uses the dataset's manufacturer list.
 */
char **aircraft_manufs_completion(const char *str, int start, int end);

/**
 * @brief Completion hook for nationalities (Query 6).
 * Uses the dataset's nationality list.
 */
char **nationality_completion(const char *str, int start, int end);


#endif