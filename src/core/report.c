#include <core/report.h>
#include <glib.h>
#include <stdio.h>

void initReport(void)
{
    // clean error files at startup
    remove("resultados/flights_errors.csv");
    remove("resultados/passengers_errors.csv");
    remove("resultados/airports_errors.csv");
    remove("resultados/aircrafts_errors.csv");
    remove("resultados/reservations_errors.csv");
}

GTimer *startTimer(void)
{
    GTimer *timer = g_timer_new();
    g_timer_start(timer);
    return timer;
}

gdouble stopTimer(GTimer *timer)
{
    gdouble elapsed = g_timer_elapsed(timer, NULL);
    g_timer_destroy(timer);

    return elapsed;
}

void reportErrors(gint errorsFlag)
{
    if (errorsFlag)
        printf("Check errors.csv files for invalid lines.\n");
}

void reportDone(void)
{
    printf("\nAll datasets processed successfully.\n");
}
