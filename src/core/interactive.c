#include "core/interactive.h"
#include "core/dataset.h"
#include "core/time_utils.h"
#include "entities/flights.h"
#include "glib.h"
#include "queries/queries.h"
#include <queries/query5.h>
#include <queries/query4.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/history.h>
#include <readline/readline.h>

#include <core/fenwick.h>
#include <core/utils.h>
#include <entities/aircrafts.h>
#include <signal.h>
#include <sys/types.h>
#include <validation.h>

#define DATASET_FILE ".dataset_path"

// ANSI Escape Codes for Text Formatting and Colors
#define ANSI_RESET "\x1b[0m"

// Text Attributes
#define ANSI_BOLD "\x1b[1m"
#define ANSI_DIM "\x1b[2m"
#define ANSI_UNDERLINE "\x1b[4m"
#define ANSI_REVERSE "\x1b[7m"
#define ANSI_BLINK "\x1b[5m"

// Foreground Colors
#define ANSI_COLOR_BLACK "\x1b[30m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_WHITE "\x1b[37m"

// Bright Foreground Colors
#define ANSI_BRIGHT_BLACK "\x1b[90m"
#define ANSI_BRIGHT_RED "\x1b[91m"
#define ANSI_BRIGHT_GREEN "\x1b[92m"
#define ANSI_BRIGHT_YELLOW "\x1b[93m"
#define ANSI_BRIGHT_BLUE "\x1b[94m"
#define ANSI_BRIGHT_MAGENTA "\x1b[95m"
#define ANSI_BRIGHT_CYAN "\x1b[96m"
#define ANSI_BRIGHT_WHITE "\x1b[97m"

#define CLEAR printf("\x1b[2J\x1b[H")

// global pointer to the arrays necessary for custom query completions
// NOTE: Due to the way readline expects its completion function, these
// shouldn't be passed as parameters there
static GPtrArray *active_airport_codes = NULL;
static GPtrArray *active_aircraft_manufs = NULL;
static GPtrArray *active_nationalities = NULL;

void save_dataset_path(const char *path) {
  FILE *file = fopen(DATASET_FILE, "w");
  if (file) {
    fprintf(file, "%s", path);
    fclose(file);
  }
}

char *load_dataset_path(void) {
  FILE *file = fopen(DATASET_FILE, "r");
  if (!file)
    return NULL;

  char *path = NULL;
  size_t len = 0;
  ssize_t read = getline(&path, &len, file);

  fclose(file);

  if (read != -1) {
    // Trim newline if present
    if (path[read - 1] == '\n') {
      path[read - 1] = '\0';
    }
    return path;
  } else {
    free(path);
    return NULL;
  }
}

int validate_dataset_files(const char *dataset_path) {
  if (!checkPath(dataset_path)) {
    printf(ANSI_COLOR_RED
           "The given path doesn't exist or is not accessible.\n" ANSI_RESET);
    return 0;
  }

  const char *files[] = {"aircrafts.csv", "flights.csv", "passengers.csv",
                         "airports.csv", "reservations.csv"};
  char path_buffer[1024];

  for (int i = 0; i < 5; i++) {
    snprintf(path_buffer, sizeof(path_buffer), "%s/%s", dataset_path, files[i]);
    if (!checkPath(path_buffer)) {
      printf(ANSI_COLOR_RED
             "The directory does not contain the required dataset files "
             "(missing %s).\n" ANSI_RESET,
             files[i]);
      return 0;
    }
  }
  return 1;
}

int page_dataset(char *base_dataset_path, int option) {
  FILE *pager = popen("less -R -S -N -P\" -- Page %dt (Press Space for Next "
                      "Page, q to Quit) -- \"",
                      "w");

  if (!pager) {
    pager = stdout;
  }

  FILE *file = NULL;
  char full_path[1024];

  if (option == 1)
    snprintf(full_path, sizeof(full_path), "%s/aircrafts.csv",
             base_dataset_path);
  else if (option == 2)
    snprintf(full_path, sizeof(full_path), "%s/airports.csv",
             base_dataset_path);
  else if (option == 3)
    snprintf(full_path, sizeof(full_path), "%s/flights.csv", base_dataset_path);
  else if (option == 4)
    snprintf(full_path, sizeof(full_path), "%s/passengers.csv",
             base_dataset_path);
  else if (option == 5)
    snprintf(full_path, sizeof(full_path), "%s/reservations.csv",
             base_dataset_path);
  else {
    if (pager != stdout)
      pclose(pager);
    return 1;
  }

  file = fopen(full_path, "r");
  if (!file) {
    if (pager != stdout)
      pclose(pager);
    return 2;
  }

  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  while ((read = getline(&line, &len, file)) != -1) {
    for (int i = 0; i < read; i++) {
      if (line[i] == ';') {
        line[i] = '\t';
      }
    }
    fprintf(pager, "%s", line);
  }

  free(line);
  fclose(file);

  if (pager != stdout) {
    pclose(pager);
  } else {
    free(readline("Press ENTER to continue..."));
  }

  return 0;
}

void print_options(void) {
  CLEAR;
  printf(ANSI_BOLD ANSI_COLOR_CYAN "Flight System Options:\n" ANSI_RESET);
  printf(ANSI_COLOR_GREEN "1." ANSI_RESET
                          " [dataset]   Change current dataset.\n");
  printf(ANSI_COLOR_GREEN "2." ANSI_RESET " [queries]   Run a query.\n");
  printf(ANSI_COLOR_GREEN "3." ANSI_RESET " [view]      View dataset files.\n");
  printf(ANSI_COLOR_GREEN "4." ANSI_RESET " [clear]     Clears the screen.\n");
  printf(ANSI_COLOR_RED "5." ANSI_RESET " [exit/quit] Exit the application.\n");
  printf("\n");
}

void print_queries(void) {
  CLEAR;
  printf(ANSI_BOLD ANSI_COLOR_YELLOW "Available Queries:\n" ANSI_RESET);
  printf(ANSI_BOLD "[1]" ANSI_RESET " Airport summary\n"
                   "\tLists name, city, country, type, arrival count and "
                   "departure count.\n");
  printf(ANSI_BOLD "[2]" ANSI_RESET " Top N aircrafts with more flights\n"
                   "\tLists name, manufacturer, model and flight count.\n");
  printf(ANSI_BOLD "[3]" ANSI_RESET " Airport with most flights in date range\n"
                   "\tLists airport code, name, city, country and flight "
                   "count.\n");
  printf(ANSI_BOLD "[4]" ANSI_RESET " Top Passenger Frequency\n"
                   "\tLists the passenger who most often appeared in the weekly top 10.\n");
  printf(ANSI_BOLD "[5]" ANSI_RESET " Flight delays by airline\n"
                   "\tLists airlines with top N average delays.\n");
  printf(ANSI_BOLD "[6]" ANSI_RESET " Passenger statistics by nationality\n"
                   "\tLists airport with most passengers for a nationality.\n");
  printf("\n");
}

int interactive_mode(Dataset **ds_ref, char **dataset_path_ptr) {
  char *input;

  // update completion arrays
  active_airport_codes = get_dataset_airport_codes(*ds_ref);
  active_aircraft_manufs = get_dataset_aircraft_manufacturers(*ds_ref);
  active_nationalities = get_dataset_nationalities(*ds_ref);

  // ===== recover history from file =====
  read_history(".apphistory");

  // ===== build query context =====
  GHashTable *airportFtrees = NULL;
  GPtrArray *aircraftsArray = NULL;
  int *flightCounts = NULL;
  GList *airlineDelays = NULL;
  GHashTable *natTable = NULL;
  Q4Struct *q4_data = NULL;

  if (*ds_ref) {
    build_query_context(*ds_ref, &aircraftsArray, &flightCounts, &airportFtrees,
                        &airlineDelays, &natTable);
  }

  // ===== main interactive loop =====
  while (1) {
    // show menu
    print_options();

    // read user input option
    input = readline(ANSI_BOLD "> " ANSI_RESET);
    int special = 0;

    // handle manual exit (Ctr+D)
    if (!input) {
      break;
    }

    // check line length to skip empty ones
    if (strlen(input) > 0) {
      // add to history
      add_history(input);
      // remove spaces to enable easier matching
      trim_whitespace(input);

      // ===== CALL =====
      // clear screen on command call [DEPRECATED]
      if (!strcmp(input, "clear") || !strcmp(input, "4"))
        CLEAR;

      // ===== QUERIES =====
      else if (!strcmp(input, "queries") || !strcmp(input, "2")) {
        free(input);
        // set queries completion
        rl_attempted_completion_function = query_completion;

        // show query options
        print_queries();
        // read user option and store in the same variable (less mem used)
        input = readline(ANSI_BOLD "Select query > " ANSI_RESET);

        // reset completion function
        rl_attempted_completion_function = main_completion;
        if (input) {
          // remove spaces for easier matching
          trim_whitespace(input);

          // ===== check if special formatting is required =====
          if (*(input + 1) == 'S')
            special = 1;

          // ===== QUERY 1 =====
          // check if user entered either "1" or "1S"
          if (input[0] == '1' && (input[1] == 'S' || input[1] == '\0')) {
            free(input);
            // set custom airport code completion
            rl_attempted_completion_function = airport_code_completion;
            // read inputted ariport code and perform necessary
            // verifications
            input = readline("Airport code:\t");
            trim_whitespace(input);
            if (!checkAirportCode(input)) {
              printf(ANSI_BLINK ANSI_COLOR_RED
                     "Invalid code syntax!\n" ANSI_RESET);
              free(readline("Press ENTER to continue..."));
            } else {
              // clear screen for more stylized visuals, I guess...
              CLEAR;
              printf(ANSI_BOLD "Query 1 Result (%s):\n" ANSI_RESET, input);
              // run actual query as wrapper
              int res = query1wrapper(input, special, stdout, *ds_ref);

              // check for run errors
              if (res == 1)
                printf(ANSI_BLINK ANSI_COLOR_RED
                       "The given code does not exist in the currently loaded "
                       "dataset\n" ANSI_RESET);
              free(readline(ANSI_DIM
                            "\nPress ENTER to return to menu..." ANSI_RESET));

              rl_attempted_completion_function = main_completion;
            }

            // ===== QUERY 2 =====
            // check if user entered either "2" or "2S"
          } else if (input[0] == '2' && (input[1] == 'S' || input[1] == '\0')) {
            free(input);
            // set query 2 custom completions
            rl_attempted_completion_function = NULL;
            input = NULL;
            // read first parameter and store it in auxiliary variable
            char *number =
                readline("How many flights should the top include: ");
            trim_whitespace(number);
            // check if user input is valid
            if (!checkInt(number)) {
              printf("Supplied value is not a valid integer!\n");
              free(number);
            } else {
              rl_attempted_completion_function = aircraft_manufs_completion;
              // read second parameter and reuse input variable to save memory
              input = readline(
                  "Manufacturer to filter for [OPTIONAL - ENTER to skip]: ");
              trim_whitespace(input);
              // run query via wrapper and store exit/error code for further
              // verifications
              int res = query2wrapper(number, input, stdout, aircraftsArray,
                                      flightCounts, special);
              free(number);

              // print error message respective to the outputted error code
              if (res == 1)
                printf(ANSI_BLINK ANSI_COLOR_RED
                       "Cannot view top 0! Parameter must be "
                       "higer than 0." ANSI_RESET "\n");
              else if (res == 2)
                printf(ANSI_BLINK ANSI_COLOR_RED
                       "Specified manufacturer doesn't exist in the database "
                       "or is of invalid format" ANSI_RESET "\n");

              free(readline("\n " ANSI_DIM
                            "Press ENTER to return to menu..." ANSI_RESET));

              rl_attempted_completion_function = main_completion;
            }
            // ===== QUERY 3 ====
            // check if user inputted wither "3" or "3S"
          } else if (input[0] == '3' && (input[1] == 'S' || input[1] == '\0')) {
            free(input);
            // read first parameter and reuse input variable
            input = readline("Start date (YYYY-MM-DD): ");
            trim_whitespace(input);
            // parse date string to time_t
            // NOTE: this step is not very important, as the time_t value is not
            // really used by the query itself
            // However, it is the easiest, already implemented way to check
            // wether or not the supplied date string is valid, due to
            // parse_unix_date's exit codes and checkDate
            time_t start_date = parse_unix_date(input, NULL);
            if (!checkDate(start_date) || !strcmp(input, "\n")) {
              printf(ANSI_BLINK ANSI_COLOR_RED
                     "Invalid date syntax!\n" ANSI_RESET);
              free(readline("Press ENTER to continue..."));
            } else {
              // do the same for the end date, using a new temporary variable
              char *arg2 = readline("End date (YYYY-MM-DD):   ");
              trim_whitespace(arg2);
              time_t end_date = parse_unix_date(arg2, NULL);
              if (!checkDate(end_date) || !strcmp(arg2, "\n")) {
                printf(ANSI_BLINK ANSI_COLOR_RED
                       "Invalid date syntax!\n" ANSI_RESET);
                free(readline("Press ENTER to continue..."));
                free(arg2);
              } else {

                CLEAR;
                printf(ANSI_BOLD "Query 3 Result (%s - %s):\n" ANSI_RESET,
                       input, arg2);
                int res = query3wrapper(input, arg2, stdout, *ds_ref, special,
                                        airportFtrees);

                if (res == 1)
                  printf(ANSI_BLINK ANSI_COLOR_RED
                         "No data found in the given range!\n\n" ANSI_RESET);

                free(arg2);
                free(readline(ANSI_DIM
                              "Press ENTER to return to menu..." ANSI_RESET));
              }
            }

            // ===== QUERY 4 =====
          } else if (input[0] == '4' && (input[1] == 'S' || input[1] == '\0')) {
            free(input);

            // Read Start Date
            input = readline("Start date (YYYY-MM-DD) [ENTER for all]: ");
            trim_whitespace(input);

            int valid = 1;
            // Validate start date only if user typed something
            if (strlen(input) > 0)
            {
              time_t sd = parse_unix_date(input, NULL);
              if (!checkDate(sd))
                valid = 0;
            }

            if (!valid)
            {
              printf(ANSI_BLINK ANSI_COLOR_RED "Invalid start date syntax!\n" ANSI_RESET);
              free(readline("Press ENTER to continue..."));
            }
            else
            {
              // Read End Date
              char *arg2 = readline("End date (YYYY-MM-DD) [ENTER for all]:   ");
              trim_whitespace(arg2);

              // Validate end date only if user typed something
              if (strlen(arg2) > 0 && !checkDate(parse_unix_date(arg2, NULL)))
              {
                printf(ANSI_BLINK ANSI_COLOR_RED "Invalid end date syntax!\n" ANSI_RESET);
                free(readline("Press ENTER to continue..."));
              }
              else
              {
                CLEAR;
                printf(ANSI_BOLD "Query 4 Result:\n" ANSI_RESET);

                // Call query
                // Pass NULL if string is empty to trigger like the "all time" logic
                query4(q4_data,
                         (GHashTable *)getDatasetPassengers(*ds_ref),
                         strlen(input) > 0 ? input : NULL,
                         strlen(arg2) > 0 ? arg2 : NULL,
                         stdout, special);

                free(readline(ANSI_DIM "\nPress ENTER to return to menu..." ANSI_RESET));
              }
              free(arg2);
            }

            // ===== QUERY 5 =====
            // check if user entered "5" or "5S"
          }
          else if (input[0] == '5' && (input[1] == 'S' || input[1] == '\0'))
          {
            free(input);
            // same as always, just checking if input is an integer
            input = readline("Top N: ");
            trim_whitespace(input);
            if (!checkInt(input)) {
              printf(ANSI_BLINK ANSI_COLOR_RED
                     "Input is not a valid integer!\n" ANSI_RESET);
              free(readline("Press ENTER to continue..."));
            } else {
              // run the query wrapper and check for errors
              int res = query5wrapper(input, stdout, airlineDelays, special);
              if (res == 1)
                printf(ANSI_BLINK ANSI_COLOR_RED "Invalid input!\n" ANSI_RESET);
              else if (res == 2)
                printf(ANSI_BLINK ANSI_COLOR_RED
                       "Input has to be a non-negative number!\n" ANSI_RESET);
              else if (res == 3)
                printf(ANSI_BLINK ANSI_COLOR_RED
                       "Values not present in current dataset\n" ANSI_RESET);

              free(readline(ANSI_DIM
                            "Press ENTER to return to menu..." ANSI_RESET));
            }
            // ===== QUERY 6 =====
            // for the last time, check if user entered "6" or "6S"...
          }
          else if (input[0] == '6' && (input[1] == 'S' || input[1] == '\0'))
          {
            free(input);
            // ...and read user input
            // NOTE: As nationality can be a pretty varied string, there's not
            // really a good check to perform Just check afterwards if there was
            // useful output by the query
            rl_attempted_completion_function = nationality_completion;
            input = readline("Nationality to consider: ");
            trim_whitespace(input);
            int res = query6wrapper(input, stdout, natTable, special);
            if (res == 1)
              printf(ANSI_BLINK ANSI_COLOR_RED "Invalid input!\n" ANSI_RESET);
            else if (res == 2)
              printf(ANSI_BLINK ANSI_COLOR_RED
                     "Values not present in current dataset\n" ANSI_RESET);
            free(readline(ANSI_DIM
                          "Press ENTER to return to menu..." ANSI_RESET));

            rl_attempted_completion_function = main_completion;
          }
        }
        // ===== VIEW =====
        // check if user choose the view option
      } else if (!strcmp(input, "view") || !strcmp(input, "3")) {
        free(input);
        CLEAR;
        printf(ANSI_BOLD ANSI_COLOR_YELLOW "Select file to view:\n" ANSI_RESET);
        printf("[1] Aircrafts\n");
        printf("[2] Airports\n");
        printf("[3] Flights\n");
        printf("[4] Passengers\n");
        printf("[5] Reservations\n");
        printf("\n");

        // set view completion function
        rl_attempted_completion_function = view_completion;

        // read user input, page dataset and display eventual error messages
        input = readline(ANSI_BOLD "Select file > " ANSI_RESET);

        // reset main completion function
        rl_attempted_completion_function = main_completion;

        if (input && strcmp(input, "\n")) {
          int option_num = atoi(input);
          // NOTE:
          // - The page_dataset function pipes the original csv files into
          // less for better style and functionality If the intent is showing
          // ONLY valid lines, such approach is faulty, but until a decision is
          // reached, it's the most efficient implementation, both in
          // developmente and execution time
          // - The pager function uses less, AKA the most common and default
          // pager utility in pretty much all GNU/Linux distributions. For the
          // best usage, the user should make sure they have less installed on
          // their system
          int page_res = page_dataset(*dataset_path_ptr, option_num);

          if (page_res == 1) {
            printf(ANSI_COLOR_RED "Invalid selection.\n" ANSI_RESET);
          } else if (page_res == 2) {
            printf(ANSI_COLOR_RED "Error opening file.\n" ANSI_RESET);
          }
        }
        // ===== DATASET =====
        // once more, check user input and enable dataset specific code
      } else if (!strcmp(input, "dataset") || !strcmp(input, "1")) {
        free(input);

        // use default filename completion for dataset path
        // this overrides any custom completion function currently used by
        // readline
        rl_attempted_completion_function = NULL;

        input = readline("New dataset path: ");

        // restore main menu completions
        // the next TAB-completion will thus be of a main menu option, as the
        // user is prompted back to it after this code block runs
        rl_attempted_completion_function = main_completion;

        // check if user did input some path
        if (input) {

          // remove spaces for easier matching
          trim_whitespace(input);

          // only try loading dataset if the provided path exists and contains
          // the expected csv files
          if (validate_dataset_files(input)) {
            printf("Reloading dataset...\n");
            // remove/free previous dataset via a pointer to pointer
            cleanupDataset(*ds_ref);

            // free all previously used auxiliary data structures and set them
            // to NULL so they can be re-initialized when reloading dataset
            if (airportFtrees) {
              g_hash_table_destroy(airportFtrees);
              airportFtrees = NULL;
            }
            if (aircraftsArray) {
              g_ptr_array_free(aircraftsArray, TRUE);
              aircraftsArray = NULL;
            }
            if (flightCounts) {
              free(flightCounts);
              flightCounts = NULL;
            }
            if (airlineDelays) {
              freeAirlineDelays(airlineDelays);
              airlineDelays = NULL;
            }
            if (natTable) {
              g_hash_table_destroy(natTable);
              natTable = NULL;
            }
            if (q4_data) {
                destroy_Q4_structure(q4_data);
                q4_data = NULL;
            }

            // initialize a clean dataset and load the csv data into it
            *ds_ref = initDataset();
            gint errors = 0;
            loadAllDatasets(*ds_ref, &errors, input, FALSE);
            // build necessary auxiliary data structures for the queries AKA the
            // query context
            build_query_context(*ds_ref, &aircraftsArray, &flightCounts,
                                &airportFtrees, &airlineDelays, &natTable);
            q4_data = init_Q4_structure((GHashTable *)getDatasetReservations(*ds_ref), (GHashTable *)getDatasetFlights(*ds_ref));

            // check if any csv lines had errors and display msg
            if (errors != 0) {
              printf(ANSI_BLINK ANSI_COLOR_RED
                     "Errors occurred while loading dataset. Partial data "
                     "may be "
                     "loaded.\n" ANSI_RESET);
            } else {
              printf(ANSI_COLOR_GREEN
                     "Dataset loaded successfully!\n" ANSI_RESET);
              // save the new dataset path persistently
              save_dataset_path(input);

              // update dataset path string
              if (*dataset_path_ptr)
                free(*dataset_path_ptr);
              *dataset_path_ptr = strdup(input);

              // update completions arrays
              active_airport_codes = get_dataset_airport_codes(*ds_ref);
              active_aircraft_manufs =
                  get_dataset_aircraft_manufacturers(*ds_ref);
              active_nationalities = get_dataset_nationalities(*ds_ref);
            }
          }
          free(readline("Press ENTER to continue..."));
        }

        // ===== EXIT/QUIT ====
        // ...finaly, check if the user wants to throw the towel
      } else if (!strcmp(input, "exit") || !strcmp(input, "quit") ||
                 !strcmp(input, "5")) {
        break;
      }
    }

    free(input);
  }

  // save history for next run
  write_history(".apphistory");

  // cleanup
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

char *main_command_gen(const char *str, int state) {
  char *main_commands[] = {"dataset", "1", "queries", "2",    "view", "3",
                           "clear",   "4", "exit",    "quit", "5",    NULL};
  static int idx, len;
  char *cmd;

  if (!state) {
    idx = 0;
    len = strlen(str);
  }

  while ((cmd = main_commands[idx++])) {
    if (!strncmp(cmd, str, len))
      return strdup(cmd);
  }

  return NULL;
}

char *query_command_gen(const char *str, int state) {
  char *query_commands[] = {"1", "2", "3", "4", "5", "6", NULL};
  static int idx, len;
  char *cmd;

  if (!state) {
    idx = 0;
    len = strlen(str);
  }

  while ((cmd = query_commands[idx++])) {
    if (!strncmp(cmd, str, len))
      return strdup(cmd);
  }

  return NULL;
}

char *view_command_gen(const char *str, int state) {
  char *view_commands[] = {"1", "2", "3", "4", "5", NULL};
  static int idx, len;
  char *cmd;

  if (!state) {
    idx = 0;
    len = strlen(str);
  }

  while ((cmd = view_commands[idx++])) {
    if (!strncmp(cmd, str, len))
      return strdup(cmd);
  }

  return NULL;
}

char *airport_code_gen(const char *str, int state) {
  static guint idx;
  static int len;

  if (!active_airport_codes)
    return NULL;

  if (!state) {
    idx = 0;
    len = strlen(str);
  }

  while (idx < active_airport_codes->len) {
    char *code = g_ptr_array_index(active_airport_codes, idx++);

    if (strncmp(code, str, len) == 0) {
      return strdup(code);
    }
  }

  return NULL;
}

char *aircraft_manufs_gen(const char *str, int state) {
  static guint idx;
  static int len;

  if (!active_aircraft_manufs)
    return NULL;

  if (!state) {
    idx = 0;
    len = strlen(str);
  }

  while (idx < active_aircraft_manufs->len) {
    char *manuf = g_ptr_array_index(active_aircraft_manufs, idx++);

    if (strncmp(manuf, str, len) == 0) {
      return strdup(manuf);
    }
  }

  return NULL;
}

char *nationality_gen(const char *str, int state) {
  static guint idx;
  static int len;

  if (!active_nationalities)
    return NULL;

  if (!state) {
    idx = 0;
    len = strlen(str);
  }

  while (idx < active_nationalities->len) {
    char *nat = g_ptr_array_index(active_nationalities, idx++);

    if (strncmp(nat, str, len) == 0) {
      return strdup(nat);
    }
  }

  return NULL;
}

char **main_completion(const char *str, int start, int end) {
  // do not complete filenames even when no other choice is present
  rl_attempted_completion_over = 1;
  // returning NULL will allow the usage of the default completer
  return rl_completion_matches(str, main_command_gen);
}

char **query_completion(const char *str, int start, int end) {
  // do not complete filenames even when no other choice is present
  rl_attempted_completion_over = 1;
  // returning NULL will allow the usage of the default completer
  return rl_completion_matches(str, query_command_gen);
}

char **view_completion(const char *str, int start, int end) {
  // do not complete filenames even when no other choice is present
  rl_attempted_completion_over = 1;
  // returning NULL will allow the usage of the default completer
  return rl_completion_matches(str, view_command_gen);
}

char **airport_code_completion(const char *str, int start, int end) {
  // do not complete filenames even when no other choice is present
  rl_attempted_completion_over = 1;
  // returning NULL will allow the usage of the default completer
  return rl_completion_matches(str, airport_code_gen);
}

char **aircraft_manufs_completion(const char *str, int start, int end) {
  // do not complete filenames even when no other choice is present
  rl_attempted_completion_over = 1;
  // returning NULL will allow the usage of the default completer
  return rl_completion_matches(str, aircraft_manufs_gen);
}

char **nationality_completion(const char *str, int start, int end) {
  // do not complete filenames even when no other choice is present
  rl_attempted_completion_over = 1;
  // returning NULL will allow the usage of the default completer
  return rl_completion_matches(str, nationality_gen);
}

int main() {
  // necessary so readline works properly after less exits, as it expects to own
  // the terminal at all times
  signal(SIGPIPE, SIG_IGN);
  // clear terminal on startup, for aesthetics
  CLEAR;
  printf(ANSI_BOLD "Welcome to the Flight Management System\n" ANSI_RESET);

  // initialize core values
  char *dataset_path;
  Dataset *ds = NULL;
  char *saved_path = load_dataset_path();

  // prompt the user for intial datatset path
  while (1) {
    char prompt[1024];
    if (saved_path) {
      snprintf(
          prompt, sizeof(prompt),
          "Dataset path [" ANSI_COLOR_CYAN "%s" ANSI_RESET "]: ", saved_path);
    } else {
      strcpy(prompt, "Dataset path: ");
    }

    dataset_path = readline(prompt);

    if (!dataset_path) {
      free(saved_path);
      return 0; // EOF
    }

    trim_whitespace(dataset_path);

    // use saved path if input is empty
    if (strlen(dataset_path) == 0 && saved_path) {
      free(dataset_path);
      dataset_path = strdup(saved_path);
    }

    // skip iteration if the provided dataset path is invalid
    if (!validate_dataset_files(dataset_path)) {
      free(dataset_path);
      continue;
    }

    // initialize and load the dataset
    ds = initDataset();
    gint errors = 0;

    loadAllDatasets(ds, &errors, dataset_path, FALSE);

    if (errors != 0) {
      printf(ANSI_BLINK ANSI_COLOR_RED
             "Errors ocurred while loading dataset.\nCheck *_errors.csv for "
             "invalid lines.\n" ANSI_RESET);
    } else {
      printf(ANSI_COLOR_GREEN "Dataset loaded and ready to go!\n" ANSI_RESET);
      save_dataset_path(dataset_path);
      break;
    }
  }

  free(saved_path);

  printf("Starting interactive mode...\n");

  // enable menu completion (cycling) on TAB press
  rl_bind_key('\t', rl_menu_complete);

  // disable automatic alphabetical sorting of completion options
  rl_sort_completion_matches = 0;

  // use custom completions for main menu commands
  rl_attempted_completion_function = main_completion;

  interactive_mode(&ds, &dataset_path);
  cleanupDataset(ds);

  printf(ANSI_BOLD "Goodbye!\n" ANSI_RESET);

  free(dataset_path);

  return 0;
}
