#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "interactive/session.h"
#include "interactive/ui.h"
#include "core/utils.h"

#define DATASET_FILE ".dataset_path"

void save_dataset_path(const char *path)
{
    FILE *file = fopen(DATASET_FILE, "w");
    if (file)
    {
        fprintf(file, "%s", path);
        fclose(file);
    }
}

char *load_dataset_path(void)
{
    FILE *file = fopen(DATASET_FILE, "r");
    if (!file)
        return NULL;

    char *path = NULL;
    size_t len = 0;
    ssize_t read = getline(&path, &len, file);

    fclose(file);

    if (read != -1)
    {
        // Trim newline if present
        if (path[read - 1] == '\n')
        {
            path[read - 1] = '\0';
        }
        return path;
    }
    else
    {
        free(path);
        return NULL;
    }
}

int validate_dataset_files(const char *dataset_path)
{
    if (!checkPath(dataset_path))
    {
        printf(ANSI_COLOR_RED
               "The given path doesn't exist or is not accessible.\n" ANSI_RESET);
        return 0;
    }

    const char *files[] = {"aircrafts.csv", "flights.csv", "passengers.csv",
                           "airports.csv", "reservations.csv"};
    char path_buffer[1024];

    for (int i = 0; i < 5; i++)
    {
        snprintf(path_buffer, sizeof(path_buffer), "%s/%s", dataset_path, files[i]);
        if (!checkPath(path_buffer))
        {
            printf(ANSI_COLOR_RED
                   "The directory does not contain the required dataset files "
                   "(missing %s).\n" ANSI_RESET,
                   files[i]);
            return 0;
        }
    }
    return 1;
}