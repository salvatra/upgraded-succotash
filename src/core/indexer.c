#include <core/indexer.h>
#include <core/dataset.h>
#include <entities/flights.h>
#include <core/time_utils.h>
#include <glib.h>
#include <string.h>

struct datesinfo
{
    GArray *distinctDates;
    GHashTable *dateSet;
};

// Helper destructor internal to this module
static void freeDatesInfo(gpointer data)
{
    DatesInfo *di = (DatesInfo *)data;
    if (di)
    {
        if (di->distinctDates)
            g_array_free(di->distinctDates, TRUE);
        if (di->dateSet)
            g_hash_table_destroy(di->dateSet);
        g_free(di);
    }
}

GHashTable *create_date_index(const Dataset *ds)
{
    // We still access the raw table for iteration (for now)
    const GHashTable *flights = getDatasetFlights(ds);

    GHashTable *airportsDepartures = g_hash_table_new_full(
        g_str_hash, g_str_equal, g_free, (GDestroyNotify)freeDatesInfo);

    GHashTableIter iter;
    gpointer key;
    // Cast const away just for initialization of the iterator, strictly safe here
    g_hash_table_iter_init(&iter, (GHashTable *)flights);

    while (g_hash_table_iter_next(&iter, &key, NULL))
    {
        // Zero-Copy access
        const Flight *flight = getFlight(key, flights);
        if (!flight)
            continue;

        const char *status = getFlightStatus(flight);
        if (status && strcmp(status, "Cancelled") == 0)
        {
            continue;
        }

        const gchar *airportCode = getFlightOrigin(flight);
        if (!airportCode)
            continue;

        DatesInfo *di = g_hash_table_lookup(airportsDepartures, airportCode);
        if (!di)
        {
            di = g_new0(DatesInfo, 1);
            di->distinctDates = g_array_new(FALSE, FALSE, sizeof(time_t));
            di->dateSet = g_hash_table_new(g_direct_hash, g_direct_equal);
            g_hash_table_insert(airportsDepartures, g_strdup(airportCode), di);
        }

        time_t actualDep = getFlightActualDeparture(flight);
        if (actualDep < 0)
            continue;

        time_t dayDep = actualDep - (actualDep % 86400);

        if (!g_hash_table_contains(di->dateSet, (gpointer)dayDep))
        {
            g_hash_table_add(di->dateSet, (gpointer)dayDep);
            g_array_append_val(di->distinctDates, dayDep);
        }
    }

    // Sort all arrays
    GHashTableIter datesIter;
    gpointer datesKey, datesVal;
    g_hash_table_iter_init(&datesIter, airportsDepartures);
    while (g_hash_table_iter_next(&datesIter, &datesKey, &datesVal))
    {
        DatesInfo *di = (DatesInfo *)datesVal;
        g_array_sort(di->distinctDates, compare_time_pointers);
    }

    return airportsDepartures;
}

const GArray *getDiDates(const DatesInfo *di) { return di ? di->distinctDates : NULL; }
const GHashTable *getDiSet(const DatesInfo *di) { return di ? di->dateSet : NULL; }