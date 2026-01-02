/**
 * @file completion.h
 * @brief GNU Readline tab-completion integration and logic.
 *
 * This module provides custom completion strategies for the interactive command-line interface (CLI).
 * It acts as a bridge between the GNU Readline library and the application's domain data.
 * * Since Readline callbacks do not natively support user-data arguments, this module manages
 * internal static references to the active @c Dataset to provide context-aware suggestions
 * (e.g., auto-completing real airport codes or manufacturers loaded from the CSV).
 */

#ifndef COMPLETION_H
#define COMPLETION_H

#include "core/dataset.h"

/**
 * @brief Updates the internal static context used for autocomplete suggestions.
 *
 * Because GNU Readline's generator functions follow a fixed signature that precludes
 * passing the @c Dataset pointer directly, this function caches references to the
 * necessary data arrays (airport codes, manufacturers, nationalities) within the
 * completion module's local scope.
 *
 * @note This function **must** be called whenever a new dataset is loaded or reloaded
 * to ensure autocomplete suggestions remain synchronized with the actual data.
 *
 * @param ds Pointer to the active @c Dataset structure.
 */
void update_completion_context(Dataset *ds);

/**
 * @brief Generator function for system shell pass-through commands.
 *
 * Generates matches for commands starting with '!' (e.g., "!ls", "!clear").
 * Intended to be used internally by the completion logic.
 *
 * @param str The partial string to complete.
 * @param state The generation state (0 for first match, non-zero for subsequent).
 * @return A newly allocated string containing the match, or @c NULL if done.
 */
char *shell_cmd_generator(const char *str, int state);

/**
 * @brief Completion hook for the Main Menu.
 *
 * Provides suggestions for the top-level application commands, such as:
 * - "queries"
 * - "view"
 * - "dataset"
 * - "exit"
 * - "clear"
 *
 * @param str The partial string to match (text user is typing).
 * @param start Start index of the word in the buffer.
 * @param end End index of the word in the buffer.
 * @return A @c NULL terminated array of strings (matches), or @c NULL if no match found.
 */
char **main_completion(const char *str, int start, int end);

/**
 * @brief Completion hook for the Query Selection Menu.
 *
 * Provides suggestions for selecting specific queries (e.g., "1", "2", "2S", etc.).
 *
 * @param str The partial string to match.
 * @param start Start index of the word.
 * @param end End index of the word.
 * @return A @c NULL terminated array of strings (matches).
 */
char **query_completion(const char *str, int start, int end);

/**
 * @brief Completion hook for the File Viewer Menu.
 *
 * Provides suggestions for the dataset file viewer options (e.g., "Aircrafts", "Airports", "Flights").
 *
 * @param str The partial string to match.
 * @param start Start index of the word.
 * @param end End index of the word.
 * @return A @c NULL terminated array of strings (matches).
 */
char **view_completion(const char *str, int start, int end);

/**
 * @brief Context-aware completion hook for Airport Codes (Query 1).
 *
 * Iterates through the sorted list of **valid Airport IATA codes** present in the
 * currently loaded dataset to provide suggestions.
 *
 * @param str The partial string to match.
 * @param start Start index of the word.
 * @param end End index of the word.
 * @return A @c NULL terminated array of strings (matches).
 */
char **airport_code_completion(const char *str, int start, int end);

/**
 * @brief Context-aware completion hook for Aircraft Manufacturers (Query 2).
 *
 * Iterates through the sorted list of **valid Manufacturers** present in the
 * currently loaded dataset.
 *
 * @param str The partial string to match.
 * @param start Start index of the word.
 * @param end End index of the word.
 * @return A @c NULL terminated array of strings (matches).
 */
char **aircraft_manufs_completion(const char *str, int start, int end);

/**
 * @brief Context-aware completion hook for Passenger Nationalities (Query 6).
 *
 * Iterates through the sorted list of **valid Nationalities** present in the
 * currently loaded dataset.
 *
 * @param str The partial string to match.
 * @param start Start index of the word.
 * @param end End index of the word.
 * @return A @c NULL terminated array of strings (matches).
 */
char **nationality_completion(const char *str, int start, int end);

#endif // COMPLETION_H