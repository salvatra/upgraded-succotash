#include "interactive/runner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <readline/readline.h>
#include <glib.h>

#include "core/dataset.h"
#include "core/utils.h"
#include "io/manager.h" // Logic for loadAllDatasets

#include "interactive/session.h"
#include "interactive/shell.h"
#include "interactive/ui.h"
#include "interactive/completion.h"

void interactive_run(void)
{
    // 1. Setup Signals
    signal(SIGPIPE, SIG_IGN);

    // 2. UI Init
    clear_screen();
    printf(ANSI_BOLD "Welcome to the Flight Management System\n" ANSI_RESET);

    Dataset *ds = NULL;
    char *saved_path = load_dataset_path();
    char *dataset_path = NULL;

    // 3. Dataset Loading Loop
    while (1)
    {
        char prompt[1024];
        if (saved_path)
        {
            snprintf(prompt, sizeof(prompt),
                     "Dataset path [" ANSI_COLOR_CYAN "%s" ANSI_RESET "]: ", saved_path);
        }
        else
        {
            strcpy(prompt, "Dataset path: ");
        }

        rl_attempted_completion_function = NULL;
        dataset_path = readline(prompt);

        if (!dataset_path)
        {
            free(saved_path);
            return; // Exit
        }

        trim_whitespace(dataset_path);

        // Use saved path if enter is pressed
        if (strlen(dataset_path) == 0 && saved_path)
        {
            free(dataset_path);
            dataset_path = strdup(saved_path);
        }

        if (!validate_dataset_files(dataset_path))
        {
            free(dataset_path);
            continue;
        }

        ds = initDataset();
        gint errors = 0;

        loadAllDatasets(ds, &errors, dataset_path, FALSE);

        if (errors != 0)
        {
            printf(ANSI_BLINK ANSI_COLOR_RED
                   "Errors occurred while loading dataset.\nCheck *_errors.csv for "
                   "invalid lines.\n" ANSI_RESET);
        }
        else
        {
            printf(ANSI_COLOR_GREEN "Dataset loaded and ready to go!\n" ANSI_RESET);
            save_dataset_path(dataset_path);
            break;
        }
    }

    free(saved_path);

    // 4. Configure Shell
    printf("Starting interactive mode...\n");

    rl_bind_key('\t', rl_menu_complete);
    rl_sort_completion_matches = 0;
    rl_attempted_completion_function = main_completion;

    // 5. Hand off to Shell
    interactive_mode(&ds, &dataset_path);

    // 6. Cleanup
    if (ds)
        cleanupDataset(ds);
    free(dataset_path);

    printf(ANSI_BOLD "Goodbye!\n" ANSI_RESET);
}