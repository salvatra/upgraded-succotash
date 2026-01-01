#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <glib.h>
#include "interactive/shell.h"
#include "interactive/ui.h"
#include "interactive/session.h"
#include "interactive/completion.h"
#include "interactive/pager.h"
#include "interactive/handlers.h"
#include "core/dataset.h"
#include "core/utils.h"
#include "core/time_utils.h"
#include "validation.h"
#include "queries/queries.h"
#include "queries/query4.h"
#include "queries/query5.h"

#define CLEAR clear_screen()

int interactive_mode(Dataset **ds_ref, char **dataset_path_ptr)
{
    char *input;

    update_completion_context(*ds_ref);

    read_history(".apphistory");

    GHashTable *airportFtrees = NULL;
    GPtrArray *aircraftsArray = NULL;
    int *flightCounts = NULL;
    GList *airlineDelays = NULL;
    GHashTable *natTable = NULL;
    Q4Struct *q4_data = NULL;

    if (*ds_ref)
    {
        build_query_context(*ds_ref, &aircraftsArray, &flightCounts, &airportFtrees,
                            &q4_data, &airlineDelays, &natTable);
        if (!q4_data)
        {
            q4_data = init_Q4_structure((GHashTable *)getDatasetReservations(*ds_ref),
                                        (GHashTable *)getDatasetFlights(*ds_ref));
        }
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

                if (input)
                {
                    trim_whitespace(input);
                    // 'S'pecial :P
                    int special = (strlen(input) > 1 && *(input + 1) == 'S') ? 1 : 0;

                    // QUERY 1
                    if (input[0] == '1')
                    {
                        free(input);
                        rl_attempted_completion_function = airport_code_completion;
                        input = readline("Airport code:\t");
                        trim_whitespace(input);
                        if (!checkAirportCode(input))
                        {
                            printf(ANSI_BLINK ANSI_COLOR_RED "Invalid code syntax!\n" ANSI_RESET);
                            free(readline("Press ENTER..."));
                        }
                        else
                        {
                            CLEAR;
                            printf(ANSI_BOLD "Query 1 Result (%s):\n" ANSI_RESET, input);
                            if (query1wrapper(input, special, stdout, *ds_ref) == 1)
                                printf(ANSI_BLINK ANSI_COLOR_RED "Code not found.\n" ANSI_RESET);
                            free(readline(ANSI_DIM "\nPress ENTER to return..." ANSI_RESET));
                        }
                        rl_attempted_completion_function = main_completion;
                    }
                    // QUERY 2
                    else if (input[0] == '2')
                    {
                        free(input);
                        rl_attempted_completion_function = NULL;
                        char *num = readline("Top N flights: ");
                        trim_whitespace(num);
                        if (!checkInt(num))
                        {
                            printf("Invalid integer!\n");
                            free(num);
                        }
                        else
                        {
                            rl_attempted_completion_function = aircraft_manufs_completion;
                            input = readline("Manufacturer [OPTIONAL]: ");
                            trim_whitespace(input);
                            int r = query2wrapper(num, input, stdout, aircraftsArray, flightCounts, special);
                            free(num);
                            if (r == 1)
                                printf(ANSI_COLOR_RED "Cannot view top 0!\n" ANSI_RESET);
                            else if (r == 2)
                                printf(ANSI_COLOR_RED "Manufacturer not found.\n" ANSI_RESET);
                            free(readline(ANSI_DIM "\nPress ENTER to return..." ANSI_RESET));
                            rl_attempted_completion_function = main_completion;
                        }
                    }
                    // QUERY 3
                    else if (input[0] == '3')
                    {
                        free(input);
                        input = readline("Start date (YYYY-MM-DD): ");
                        trim_whitespace(input);
                        if (!checkDate(parse_unix_date(input, NULL)) || !strcmp(input, "\n"))
                        {
                            printf(ANSI_COLOR_RED "Invalid start date!\n" ANSI_RESET);
                            free(readline("Press ENTER..."));
                        }
                        else
                        {
                            char *arg2 = readline("End date (YYYY-MM-DD):   ");
                            trim_whitespace(arg2);
                            if (!checkDate(parse_unix_date(arg2, NULL)) || !strcmp(arg2, "\n"))
                            {
                                printf(ANSI_COLOR_RED "Invalid end date!\n" ANSI_RESET);
                                free(arg2);
                                free(readline("Press ENTER..."));
                            }
                            else
                            {
                                CLEAR;
                                printf(ANSI_BOLD "Query 3 Result:\n" ANSI_RESET);
                                if (query3wrapper(input, arg2, stdout, *ds_ref, special, airportFtrees) == 1)
                                    printf(ANSI_COLOR_RED "No data found.\n" ANSI_RESET);
                                free(arg2);
                                free(readline(ANSI_DIM "\nPress ENTER to return..." ANSI_RESET));
                            }
                        }
                    }
                    // QUERY 4
                    else if (input[0] == '4')
                    {
                        free(input);
                        input = readline("Start date [ENTER for all]: ");
                        trim_whitespace(input);
                        int valid = (strlen(input) == 0 || checkDate(parse_unix_date(input, NULL)));
                        if (!valid)
                        {
                            printf(ANSI_COLOR_RED "Invalid start date!\n" ANSI_RESET);
                            free(readline("Press ENTER..."));
                        }
                        else
                        {
                            char *arg2 = readline("End date [ENTER for all]:   ");
                            trim_whitespace(arg2);
                            if (strlen(arg2) > 0 && !checkDate(parse_unix_date(arg2, NULL)))
                            {
                                printf(ANSI_COLOR_RED "Invalid end date!\n" ANSI_RESET);
                                free(readline("Press ENTER..."));
                            }
                            else
                            {
                                CLEAR;
                                printf(ANSI_BOLD "Query 4 Result:\n" ANSI_RESET);
                                query4(q4_data, (GHashTable *)getDatasetPassengers(*ds_ref),
                                       strlen(input) > 0 ? input : NULL, strlen(arg2) > 0 ? arg2 : NULL, stdout, special);
                                free(readline(ANSI_DIM "\nPress ENTER to return..." ANSI_RESET));
                            }
                            free(arg2);
                        }
                    }
                    // QUERY 5
                    else if (input[0] == '5')
                    {
                        free(input);
                        input = readline("Top N: ");
                        trim_whitespace(input);
                        if (!checkInt(input))
                        {
                            printf(ANSI_COLOR_RED "Invalid integer!\n" ANSI_RESET);
                            free(readline("Press ENTER..."));
                        }
                        else
                        {
                            if (query5wrapper(input, stdout, airlineDelays, special) != 3)
                                printf(ANSI_COLOR_RED "Error or invalid input.\n" ANSI_RESET);
                            free(readline(ANSI_DIM "\nPress ENTER to return..." ANSI_RESET));
                        }
                    }
                    // QUERY 6
                    else if (input[0] == '6')
                    {
                        free(input);
                        rl_attempted_completion_function = nationality_completion;
                        input = readline("Nationality: ");
                        trim_whitespace(input);
                        if (query6wrapper(input, stdout, natTable, special) != 0)
                            printf(ANSI_COLOR_RED "Error or no data found.\n" ANSI_RESET);
                        free(readline(ANSI_DIM "\nPress ENTER to return..." ANSI_RESET));
                        rl_attempted_completion_function = main_completion;
                    }
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
                        cleanupDataset(*ds_ref);
                        // aux cleanup
                        if (airportFtrees)
                            g_hash_table_destroy(airportFtrees);
                        if (aircraftsArray)
                            g_ptr_array_free(aircraftsArray, TRUE);
                        if (flightCounts)
                            free(flightCounts);
                        if (airlineDelays)
                            freeAirlineDelays(airlineDelays);
                        if (natTable)
                            g_hash_table_destroy(natTable);
                        if (q4_data)
                            destroy_Q4_structure(q4_data);

                        *ds_ref = initDataset();
                        gint errors = 0;
                        loadAllDatasets(*ds_ref, &errors, input, FALSE);

                        build_query_context(*ds_ref, &aircraftsArray, &flightCounts, &airportFtrees, &q4_data, &airlineDelays, &natTable);
                        if (!q4_data)
                            q4_data = init_Q4_structure((GHashTable *)getDatasetReservations(*ds_ref), (GHashTable *)getDatasetFlights(*ds_ref));

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
            // --- EXIT ---
            else if (!strcmp(input, "exit") || !strcmp(input, "quit") || !strcmp(input, "5"))
            {
                break;
            }
        }
        free(input);
    }

    // Cleanup
    write_history(".apphistory");
    if (airportFtrees)
        g_hash_table_destroy(airportFtrees);
    if (aircraftsArray)
        g_ptr_array_free(aircraftsArray, TRUE);
    if (flightCounts)
        free(flightCounts);
    if (airlineDelays)
        freeAirlineDelays(airlineDelays);
    if (natTable)
        g_hash_table_destroy(natTable);
    if (q4_data)
        destroy_Q4_structure(q4_data);

    return 0;
}