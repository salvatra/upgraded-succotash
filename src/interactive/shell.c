#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <glib.h>
#include <ctype.h>
#include "interactive/shell.h"
#include "interactive/ui.h"
#include "interactive/session.h"
#include "interactive/completion.h"
#include "interactive/pager.h"
#include "interactive/handlers.h"
#include "core/dataset.h"
#include "core/utils.h"
#include "core/time_utils.h"
#include "io/validation/validation_utils.h"
#include "io/manager.h"
#include "queries/queries.h"
#define CLEAR clear_screen()

extern QueryManager *query_manager_create(Dataset *ds);
extern void query_manager_destroy(QueryManager *qm);
extern int query_manager_execute(QueryManager *qm, int queryId, char *arg1, char *arg2,
                                 int isSpecial, FILE *output, Dataset *ds);

int interactive_mode(Dataset **ds_ref, char **dataset_path_ptr)
{
    char *input;

    update_completion_context(*ds_ref);
    read_history(".apphistory");

    QueryManager *qm = NULL;
    if (*ds_ref)
    {
        qm = query_manager_create(*ds_ref);
    }

    // ===== MAIN LOOP =====
    while (1)
    {
        print_options();
        input = readline(ANSI_BOLD "> " ANSI_RESET);

        if (!input)
            break; // EOF (Ctrl+D)

        if (strlen(input) > 0)
        {
            add_history(input);
            trim_whitespace(input);

            // --- SYSTEM COMMANDS (!) ---
            if (input[0] == '!')
            {
                char *cmd = input + 1;
                while (*cmd != '\0' && *cmd == ' ')
                    cmd++;
                if (*cmd != '\0')
                    system(cmd);
                else
                    printf(ANSI_COLOR_RED "No command provided!\n" ANSI_RESET);
                free(readline(ANSI_DIM "\nPress ENTER to return..." ANSI_RESET));
            }
            // --- CLEAR ---
            else if (!strcmp(input, "clear"))
            {
                CLEAR;
            }
            // --- QUERIES ---
            else if (!strcmp(input, "queries") || !strcmp(input, "2"))
            {
                free(input);
                rl_attempted_completion_function = query_completion;
                print_queries();
                input = readline(ANSI_BOLD "Select query > " ANSI_RESET);
                rl_attempted_completion_function = main_completion;

                if (input && *input)
                {
                    trim_whitespace(input);
                    int special = (strlen(input) > 1 && toupper(*(input + 1)) == 'S') ? 1 : 0;
                    int queryNum = input[0] - '0';

                    char *arg1 = NULL;
                    char *arg2 = NULL;
                    int valid = 1;

                    // QUERY 1
                    if (queryNum == 1)
                    {
                        free(input);
                        rl_attempted_completion_function = airport_code_completion;
                        input = readline("Airport code:\t");
                        trim_whitespace(input);
                        arg1 = strdup(input);
                    }
                    // QUERY 2
                    else if (queryNum == 2)
                    {
                        free(input);
                        rl_attempted_completion_function = NULL;
                        char *num = readline("Top N flights: ");
                        trim_whitespace(num);
                        arg1 = strdup(num);
                        free(num);

                        rl_attempted_completion_function = aircraft_manufs_completion;
                        input = readline("Manufacturer [OPTIONAL]: ");
                        trim_whitespace(input);
                        if (strlen(input) > 0)
                            arg2 = strdup(input);
                    }
                    // QUERY 3
                    else if (queryNum == 3)
                    {
                        free(input);
                        input = readline("Start date (YYYY-MM-DD): ");
                        trim_whitespace(input);
                        arg1 = strdup(input);

                        char *d2 = readline("End date (YYYY-MM-DD):   ");
                        trim_whitespace(d2);
                        arg2 = strdup(d2);
                        free(d2);
                    }
                    // QUERY 4
                    else if (queryNum == 4)
                    {
                        free(input);
                        input = readline("Start date [ENTER for all]: ");
                        trim_whitespace(input);
                        arg1 = strdup(input);

                        char *d2 = readline("End date [ENTER for all]:   ");
                        trim_whitespace(d2);
                        arg2 = strdup(d2);
                        free(d2);
                    }
                    // QUERY 5
                    else if (queryNum == 5)
                    {
                        free(input);
                        input = readline("Top N: ");
                        trim_whitespace(input);
                        arg1 = strdup(input);
                    }
                    // QUERY 6
                    else if (queryNum == 6)
                    {
                        free(input);
                        rl_attempted_completion_function = nationality_completion;
                        input = readline("Nationality: ");
                        trim_whitespace(input);
                        arg1 = strdup(input);
                    }
                    else
                    {
                        valid = 0;
                    }

                    rl_attempted_completion_function = main_completion;

                    if (valid && qm)
                    {
                        CLEAR;
                        printf(ANSI_BOLD "Query %d Result:\n" ANSI_RESET, queryNum);

                        query_manager_execute(qm, queryNum, arg1, arg2, special, stdout, *ds_ref);

                        free(readline(ANSI_DIM "\nPress ENTER to return..." ANSI_RESET));
                    }
                    else if (!qm)
                    {
                        printf(ANSI_COLOR_RED "Dataset not loaded!\n" ANSI_RESET);
                        free(readline(ANSI_DIM "\nPress ENTER to return..." ANSI_RESET));
                    }

                    if (arg1)
                        free(arg1);
                    if (arg2)
                        free(arg2);
                }
            }
            // --- VIEW ---
            else if (!strcmp(input, "view") || !strcmp(input, "3"))
            {
                free(input);
                CLEAR;
                printf(ANSI_BOLD ANSI_COLOR_YELLOW "Select file to view:\n" ANSI_RESET);
                printf("[1] Aircrafts\n[2] Airports\n[3] Flights\n[4] Passengers\n[5] Reservations\n\n");
                rl_attempted_completion_function = view_completion;
                input = readline(ANSI_BOLD "Select file > " ANSI_RESET);
                rl_attempted_completion_function = main_completion;
                if (input && strcmp(input, "\n"))
                {
                    int res = page_dataset(*dataset_path_ptr, atoi(input));
                    if (res == 1)
                        printf(ANSI_COLOR_RED "Invalid selection.\n" ANSI_RESET);
                    else if (res == 2)
                        printf(ANSI_COLOR_RED "Error opening file.\n" ANSI_RESET);
                }
            }
            // --- DATASET ---
            else if (!strcmp(input, "dataset") || !strcmp(input, "1"))
            {
                free(input);
                rl_attempted_completion_function = NULL;
                input = readline("New dataset path: ");
                rl_attempted_completion_function = main_completion;
                if (input)
                {
                    trim_whitespace(input);
                    if (validate_dataset_files(input))
                    {
                        printf("Reloading dataset...\n");

                        if (qm)
                            query_manager_destroy(qm);
                        qm = NULL;

                        cleanupDataset(*ds_ref);

                        *ds_ref = initDataset();
                        gint errors = 0;
                        loadAllDatasets(*ds_ref, &errors, input, FALSE);

                        qm = query_manager_create(*ds_ref);

                        if (errors != 0)
                            printf(ANSI_COLOR_RED "Dataset loaded with errors.\n" ANSI_RESET);
                        else
                        {
                            printf(ANSI_COLOR_GREEN "Dataset loaded successfully!\n" ANSI_RESET);
                            save_dataset_path(input);
                            if (*dataset_path_ptr)
                                free(*dataset_path_ptr);
                            *dataset_path_ptr = strdup(input);
                            update_completion_context(*ds_ref);
                        }
                    }
                    free(readline("Press ENTER to continue..."));
                }
            }
            else if (!strcmp(input, "exit") || !strcmp(input, "quit") || !strcmp(input, "5"))
            {
                break;
            }
        }
        if (input)
            free(input);
    }

    // Cleanup
    write_history(".apphistory");
    if (qm)
        query_manager_destroy(qm);

    return 0;
}