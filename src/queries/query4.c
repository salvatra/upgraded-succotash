#include <queries/query4.h>
#include <queries/query_module.h>
#include <core/time_utils.h>
#include <entities/access/reservations_access.h>
#include <entities/access/flights_access.h>
#include <entities/access/passengers_access.h>
#include <core/dataset.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define SECONDS_IN_WEEK 604800

typedef struct
{
    int passenger_ids[10];
    int count;
} WeeklyTop10;

struct q4_struct
{
    GHashTable *weekly_tops;
    int min_week;
    int max_week;
};

typedef struct
{
    int doc_no;
    double total_spent;
} PassengerSpend;

static int get_week_index(time_t timestamp)
{
    if (timestamp < 0)
        return -1;
    return (int)((timestamp + 345600) / SECONDS_IN_WEEK);
}

static gint compare_spends(gconstpointer a, gconstpointer b)
{
    const PassengerSpend *p1 = a;
    const PassengerSpend *p2 = b;
    if (p1->total_spent > p2->total_spent)
        return -1;
    if (p1->total_spent < p2->total_spent)
        return 1;
    return (p1->doc_no - p2->doc_no);
}

static void free_weekly_top(gpointer data) { g_free(data); }

Q4Struct *init_Q4_structure(const Dataset *ds)
{
    Q4Struct *q4 = g_new0(Q4Struct, 1);
    q4->weekly_tops = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, free_weekly_top);
    q4->min_week = 2147483647;
    q4->max_week = -2147483648;

    GHashTable *temp_week_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, (GDestroyNotify)g_hash_table_destroy);

    DatasetIterator *it = dataset_reservation_iterator_new(ds);
    const Reservation *res;

    while ((res = (const Reservation *)dataset_iterator_next(it)) != NULL)
    {
        gchar **flight_ids = getReservationFlightIds(res);
        if (!flight_ids || !flight_ids[0])
            continue;

        const Flight *f = dataset_get_flight(ds, flight_ids[0]);
        if (!f)
            continue;

        time_t departure = getFlightDeparture(f);
        if (departure <= 0)
            continue;

        int week_idx = get_week_index(departure);
        int doc_no = getReservationDocumentNo(res);
        double price = getReservationPrice(res);

        if (week_idx < q4->min_week)
            q4->min_week = week_idx;
        if (week_idx > q4->max_week)
            q4->max_week = week_idx;

        GHashTable *pax_map = g_hash_table_lookup(temp_week_map, GINT_TO_POINTER(week_idx));
        if (!pax_map)
        {
            pax_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);
            g_hash_table_insert(temp_week_map, GINT_TO_POINTER(week_idx), pax_map);
        }

        double *current_spend = g_hash_table_lookup(pax_map, GINT_TO_POINTER(doc_no));
        if (!current_spend)
        {
            current_spend = g_new(double, 1);
            *current_spend = 0.0;
            g_hash_table_insert(pax_map, GINT_TO_POINTER(doc_no), current_spend);
        }
        *current_spend += price;
    }
    dataset_iterator_free(it);

    GHashTableIter week_iter;
    gpointer week_key, week_val;
    g_hash_table_iter_init(&week_iter, temp_week_map);
    while (g_hash_table_iter_next(&week_iter, &week_key, &week_val))
    {
        int week_idx = GPOINTER_TO_INT(week_key);
        GHashTable *pax_map = (GHashTable *)week_val;
        guint size = g_hash_table_size(pax_map);
        GArray *arr = g_array_sized_new(FALSE, FALSE, sizeof(PassengerSpend), size);
        GHashTableIter pax_iter;
        gpointer pax_key, pax_val;
        g_hash_table_iter_init(&pax_iter, pax_map);
        while (g_hash_table_iter_next(&pax_iter, &pax_key, &pax_val))
        {
            PassengerSpend ps;
            ps.doc_no = GPOINTER_TO_INT(pax_key);
            ps.total_spent = *(double *)pax_val;
            g_array_append_val(arr, ps);
        }
        g_array_sort(arr, compare_spends);
        WeeklyTop10 *wt = g_new0(WeeklyTop10, 1);
        wt->count = (size < 10) ? size : 10;
        for (int i = 0; i < wt->count; i++)
        {
            wt->passenger_ids[i] = g_array_index(arr, PassengerSpend, i).doc_no;
        }
        g_hash_table_insert(q4->weekly_tops, GINT_TO_POINTER(week_idx), wt);
        g_array_free(arr, TRUE);
    }
    g_hash_table_destroy(temp_week_map);
    return q4;
}

void destroy_Q4_structure(Q4Struct *q4)
{
    if (q4)
    {
        g_hash_table_destroy(q4->weekly_tops);
        g_free(q4);
    }
}

void query4(Q4Struct *q4_data, const Dataset *ds,
            const char *date_begin, const char *date_end,
            FILE *output, int isSpecial)
{
    if (!q4_data)
    {
        fprintf(output, "\n");
        return;
    }

    int start_w, end_w;
    if (date_begin && strlen(date_begin) > 0)
    {
        time_t t = parse_unix_date(date_begin, NULL);
        start_w = get_week_index(t);
    }
    else
    {
        start_w = q4_data->min_week;
    }

    if (date_end && strlen(date_end) > 0)
    {
        time_t t = parse_unix_date(date_end, NULL);
        end_w = get_week_index(t);
    }
    else
    {
        end_w = q4_data->max_week;
    }

    GHashTable *freq_map = g_hash_table_new(g_direct_hash, g_direct_equal);
    for (int w = start_w; w <= end_w; w++)
    {
        WeeklyTop10 *wt = g_hash_table_lookup(q4_data->weekly_tops, GINT_TO_POINTER(w));
        if (wt)
        {
            for (int i = 0; i < wt->count; i++)
            {
                int doc = wt->passenger_ids[i];
                gpointer count_ptr = g_hash_table_lookup(freq_map, GINT_TO_POINTER(doc));
                int new_count = GPOINTER_TO_INT(count_ptr) + 1;
                g_hash_table_insert(freq_map, GINT_TO_POINTER(doc), GINT_TO_POINTER(new_count));
            }
        }
    }

    int winner_doc = -1;
    int max_freq = -1;
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, freq_map);
    while (g_hash_table_iter_next(&iter, &key, &value))
    {
        int doc = GPOINTER_TO_INT(key);
        int freq = GPOINTER_TO_INT(value);
        if (freq > max_freq)
        {
            max_freq = freq;
            winner_doc = doc;
        }
        else if (freq == max_freq)
        {
            if (winner_doc == -1 || doc < winner_doc)
                winner_doc = doc;
        }
    }
    g_hash_table_destroy(freq_map);

    if (winner_doc != -1)
    {
        const Passenger *p = dataset_get_passenger(ds, winner_doc);
        if (p)
        {
            char separator = isSpecial ? '=' : ';';
            time_t dob_t = getPassengerDateOfBirth(p);
            char dob_str[12] = "";
            struct tm info;
            if (gmtime_r(&dob_t, &info))
                strftime(dob_str, sizeof(dob_str), "%Y-%m-%d", &info);

            fprintf(output, "%09d%c%s%c%s%c%s%c%s%c%d\n",
                    getPassengerDocumentNumber(p), separator,
                    getPassengerFirstName(p), separator,
                    getPassengerLastName(p), separator,
                    dob_str, separator,
                    getPassengerNationality(p), separator,
                    max_freq);
        }
        else
        {
            fprintf(output, "\n");
        }
    }
    else
    {
        fprintf(output, "\n");
    }
}

static void *q4_init_wrapper(Dataset *ds)
{
    return (void *)init_Q4_structure(ds);
}

static void q4_run_wrapper(void *ctx, Dataset *ds, char *arg1, char *arg2, int isSpecial, FILE *output)
{
    Q4Struct *data = (Q4Struct *)ctx;

    query4(data, ds, arg1, arg2, output, isSpecial);
}

static void q4_destroy_wrapper(void *ctx)
{
    destroy_Q4_structure((Q4Struct *)ctx);
}

QueryModule get_query4_module(void)
{
    QueryModule mod = {
        .id = 4,
        .init = q4_init_wrapper,
        .run = q4_run_wrapper,
        .destroy = q4_destroy_wrapper};
    return mod;
}