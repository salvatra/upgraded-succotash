#include <stdio.h>
#include "interactive/ui.h"

void clear_screen(void) {
    printf("\x1b[2J\x1b[H");
}

void print_options(void) {
    clear_screen();
    printf(ANSI_BOLD ANSI_COLOR_CYAN "Flight System Options:\n" ANSI_RESET);
    printf(ANSI_COLOR_GREEN "1." ANSI_RESET " [dataset]   Change current dataset.\n");
    printf(ANSI_COLOR_GREEN "2." ANSI_RESET " [queries]   Run a query.\n");
    printf(ANSI_COLOR_GREEN "3." ANSI_RESET " [view]      View dataset files.\n");
    printf(ANSI_COLOR_GREEN "4." ANSI_RESET " [!]         Runs the shell command specified after '!'.\n");
    printf(ANSI_COLOR_RED "5." ANSI_RESET " [exit/quit] Exit the application.\n");
    printf("\n");
}

void print_queries(void) {
    clear_screen();
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