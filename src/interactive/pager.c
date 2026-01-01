#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>

#include "interactive/pager.h"

int page_dataset(char *base_dataset_path, int option)
{
    FILE *pager = popen("less -R -S -N -P\" -- Page %dt (Press Space for Next "
                        "Page, q to Quit) -- \"",
                        "w");

    if (!pager)
    {
        pager = stdout;
    }

    FILE *file = NULL;
    char full_path[1024];

    if (option == 1)
        snprintf(full_path, sizeof(full_path), "%s/aircrafts.csv", base_dataset_path);
    else if (option == 2)
        snprintf(full_path, sizeof(full_path), "%s/airports.csv", base_dataset_path);
    else if (option == 3)
        snprintf(full_path, sizeof(full_path), "%s/flights.csv", base_dataset_path);
    else if (option == 4)
        snprintf(full_path, sizeof(full_path), "%s/passengers.csv", base_dataset_path);
    else if (option == 5)
        snprintf(full_path, sizeof(full_path), "%s/reservations.csv", base_dataset_path);
    else
    {
        if (pager != stdout)
            pclose(pager);
        return 1;
    }

    file = fopen(full_path, "r");
    if (!file)
    {
        if (pager != stdout)
            pclose(pager);
        return 2;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, file)) != -1)
    {
        for (int i = 0; i < read; i++)
        {
            if (line[i] == ';')
            {
                line[i] = '\t';
            }
        }
        fprintf(pager, "%s", line);
    }

    free(line);
    fclose(file);

    if (pager != stdout)
    {
        pclose(pager);
    }
    else
    {
        free(readline("Press ENTER to continue..."));
    }

    return 0;
}