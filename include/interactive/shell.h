#ifndef SHELL_H
#define SHELL_H

#include "core/dataset.h"

/**
 * @file shell.h
 * @brief Main interactive controller (The "Loop").
 *
 * This module orchestrates the interaction between the user input, the
 * visual interface (UI), and the business logic (Queries/Dataset).
 */

/**
 * @brief Starts the interactive shell mode.
 *
 * This function enters a blocking loop that:
 * 1. Displays menus.
 * 2. Reads user input via GNU Readline.
 * 3. Dispatches commands to appropriate handlers (Dataset loading, Queries, Views).
 * 4. Manages command history.
 *
 * @param ds_ref Pointer to the dataset pointer (allows reloading the dataset).
 * @param dataset_path_ptr Pointer to the current dataset path string.
 * @return 0 upon graceful exit.
 */
int interactive_mode(Dataset **ds_ref, char **dataset_path_ptr);

#endif