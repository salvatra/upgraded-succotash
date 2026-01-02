/**
 * @file shell.h
 * @brief Main Interactive Controller and REPL (Read-Eval-Print Loop) entry point.
 *
 * This module orchestrates the entire interactive session of the application.
 * It serves as the central hub that connects:
 * 1. **User Input:** Via GNU Readline (handling history, tab-completion, and line editing).
 * 2. **Presentation Layer:** Displaying menus, prompts, and paginated outputs.
 * 3. **Business Logic:** Dispatching parsed commands to the Query Handlers or Dataset operations.
 */

#ifndef SHELL_H
#define SHELL_H

#include "core/dataset.h"

/**
 * @brief Starts the interactive shell session.
 *
 * This function enters a blocking infinite loop (REPL) that persists until the user
 * issues an exit command or an EOF signal (Ctrl+D) is received.
 *
 * It manages the application lifecycle state, including:
 * - initializing the autocomplete context.
 * - Managing the command history file (`.apphistory`).
 * - Reloading the dataset dynamically without restarting the program.
 *
 * @param ds_ref A **handle** (double pointer) to the active Dataset instance.
 * Passed as a double pointer to allow the shell to destroy the existing dataset
 * and allocate a new one if the user executes the `dataset` command to switch paths.
 *
 * @param dataset_path_ptr A **handle** (double pointer) to the current dataset path string.
 * This string is updated and reallocated if the user successfully loads a new dataset.
 *
 * @return
 * - 0 upon graceful exit (user typed "exit" or "quit").
 * - Non-zero if a critical runtime error occurred (though mostly handles errors internally).
 */
int interactive_mode(Dataset **ds_ref, char **dataset_path_ptr);

#endif // SHELL_H