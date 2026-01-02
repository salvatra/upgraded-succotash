#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "interactive/handlers.h"
#include "core/dataset.h"
#include "queries/query1.h"
#include "queries/query2.h"
#include "queries/query3.h"
#include "queries/query4.h"
#include "queries/query5.h"
#include "queries/query6.h"

static void specialize(int isSpecial, char *str)
{
    if (isSpecial)
    {
        for (char *p = str; *p; p++)
            if (*p == ';')
                *p = '=';
    }
}

int query1wrapper(char *code, int isSpecial, FILE *stream, Dataset *ds)
{
    gchar *res = query1(code, ds);

    if (res)
    {
        specialize(isSpecial, res);
        fprintf(stream, "%s\n", res);
        g_free(res);
    }
    else
    {
        fprintf(stream, "\n");
        return 1;
    }
    return 0;
}

int query2wrapper(char *number, char *manufacturer, FILE *stream,
                  GPtrArray *aircraftsArray, int *flightCounts, int isSpecial)
{
    int N = atoi(number);
    if (N == 0)
        return 1;
    int size;
    int r = 0;

    AircraftStats **top = query2(N, aircraftsArray,
                                 (manufacturer && strlen(manufacturer) > 0) ? manufacturer : NULL,
                                 &size, flightCounts);

    char separator = (isSpecial) ? '=' : ';';

    if (top && size > 0)
    {
        for (int i = 0; i < size; i++)
        {
            fprintf(stream, "%s%c%s%c%s%c%d\n",
                    get_aircraftstats_id(top[i]), separator,
                    get_aircraftstats_manufacturer(top[i]), separator,
                    get_aircraftstats_model(top[i]), separator,
                    get_aircraftstats_count(top[i]));
        }
        free_aircraftstats_array(top, size);
    }
    else
    {
        fprintf(stream, "\n");
        r = 2;
    }
    return r;
}

int query3wrapper(char *arg1, char *arg2, FILE *stream, Dataset *ds,
                  int isSpecial, GHashTable *airportFtrees)
{
    gchar *result = query3(airportFtrees, ds, arg1, arg2);

    if (result)
    {
        if (isSpecial)
        {
            for (char *p = result; *p; p++)
                if (*p == ';')
                    *p = '=';
        }
        fprintf(stream, "%s\n", result);
        g_free(result);
    }
    else
    {
        fprintf(stream, "\n");
        return 1;
    }
    return 0;
}

int query5wrapper(char *arg1, FILE *stream, GList *airlineDelays, int isSpecial)
{
    if (!arg1 || strlen(arg1) == 0)
    {
        fprintf(stream, "\n");
        return 1;
    }
    else
    {
        int N = atoi(arg1);
        if (N <= 0)
        {
            fprintf(stream, "\n");
            return 2;
        }
        else
        {
            if (query5(airlineDelays, N, stream, isSpecial) == 0)
                return 3;
        }
    }
    return 0;
}

int query6wrapper(char *arg1, FILE *stream, GHashTable *natTable, int isSpecial)
{
    if (!arg1 || strlen(arg1) == 0)
    {
        fprintf(stream, "\n");
        return 1;
    }
    else
    {
        if (query_Q6(natTable, arg1, stream, isSpecial) == 0)
            return 2;
    }
    return 0;
}