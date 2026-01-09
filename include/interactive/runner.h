/**
 * @file runner.h
 * @brief Entry point and orchestrator for the Interactive Mode.
 *
 * This header exposes the single public function required to start the
 * interactive CLI session. It acts as the bridge between the program's `main()`
 * function and the complex state machine of the interactive shell, isolating
 * the interactive logic from the rest of the application.
 */

#ifndef INTERACTIVE_RUNNER_H
#define INTERACTIVE_RUNNER_H

/**
 * @brief Initializes and runs the interactive session.
 *
 * This function orchestrates the entire lifecycle of the interactive program.
 * It performs the following sequence of operations:
 * 1. **UI Initialization**: Sets up terminal colors, banners, and pager settings.
 * 2. **Dataset Loading**: Checks for a `.dataset_config` file or prompts the user
 * to input a path. It handles validation and loading of the dataset into memory.
 * 3. **Environment Setup**: Configures `readline` for tab-completion and history management.
 * 4. **Main Loop**: Transfers control to `interactive_mode()` which processes user commands.
 * 5. **Cleanup**: Frees the dataset, saves history, and tears down UI resources upon exit.
 *
 * @note This function blocks execution until the user explicitly exits the application.
 */
void interactive_run(void);

#endif