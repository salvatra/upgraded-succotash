#include <queries/query4.h>
#include <core/time_utils.h>
#include <entities/reservations.h>
#include <entities/flights.h>
#include <entities/passengers.h>
#include <stdlib.h>
#include <string.h>

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

// Helper struct for sorting during pre-processing
typedef struct
{
    int doc_no;
    double total_spent;
} PassengerSpend;

// --- Helper: Date to Week Index ---
// Unix Epoch (1970-01-01) was a Thursday.
// To align weeks to start on SUNDAY:
// Adding 4 days (345600 seconds!) shifts the epoch reference so that
// index changes occur on Sundays as per no relatório.
static int get_week_index(time_t timestamp)
{
    if (timestamp < 0)
        return -1;
    return (int)((timestamp + 345600) / SECONDS_IN_WEEK);
}

// Sorting math and stuff
// 1. Spend DESC
// 2. ID ASC (so like a tie-breaker)
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

static void free_weekly_top(gpointer data)
{
    g_free(data);
}

// PRE-COMPUTATION

Q4Struct *init_Q4_structure(GHashTable *reservations, GHashTable *flights)
{
    Q4Struct *q4 = g_new0(Q4Struct, 1);
    q4->weekly_tops = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, free_weekly_top);
    q4->min_week = 2147483647;  // INT_MAX
    q4->max_week = -2147483648; // INT_MIN

    // 1. Accumulate Spending per Week
    GHashTable *temp_week_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, (GDestroyNotify)g_hash_table_destroy);

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, reservations);

    while (g_hash_table_iter_next(&iter, &key, &value))
    {
        Reservation *res = (Reservation *)value;

        gchar **flight_ids = getReservationFlightIds(res);
        if (!flight_ids || !flight_ids[0])
            continue;

        // Use the first flight to determine the week
        Flight *f = getFlight(flight_ids[0], flights);
        if (!f)
            continue;

        // Use scheduled departure for week calculation
        time_t departure = getFlightDeparture(f);
        freeFlight(f); // getFlight returns a deep copy, we shall now free it

        if (departure <= 0)
            continue;

        int week_idx = get_week_index(departure);
        int doc_no = getReservationDocumentNo(res);
        double price = getReservationPrice(res);

        // Track the global bounds for the default query behavior that we have
        if (week_idx < q4->min_week)
            q4->min_week = week_idx;
        if (week_idx > q4->max_week)
            q4->max_week = week_idx;

        // Get (or create) the map for this specific week
        GHashTable *pax_map = g_hash_table_lookup(temp_week_map, GINT_TO_POINTER(week_idx));
        if (!pax_map)
        {
            pax_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);
            g_hash_table_insert(temp_week_map, GINT_TO_POINTER(week_idx), pax_map);
        }

        // Add price to the passengers total
        double *current_spend = g_hash_table_lookup(pax_map, GINT_TO_POINTER(doc_no));
        if (!current_spend)
        {
            current_spend = g_new(double, 1);
            *current_spend = 0.0;
            g_hash_table_insert(pax_map, GINT_TO_POINTER(doc_no), current_spend);
        }
        *current_spend += price;
    }

    // 2. Reduce to Top 10 per Week
    GHashTableIter week_iter;
    gpointer week_key, week_val;
    g_hash_table_iter_init(&week_iter, temp_week_map);

    while (g_hash_table_iter_next(&week_iter, &week_key, &week_val))
    {
        int week_idx = GPOINTER_TO_INT(week_key);
        GHashTable *pax_map = (GHashTable *)week_val;

        // Flatten map to array for sorting
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

        // Save only Top 10, must discard rest of cheap peasants
        WeeklyTop10 *wt = g_new0(WeeklyTop10, 1);
        wt->count = (size < 10) ? size : 10;

        for (int i = 0; i < wt->count; i++)
        {
            wt->passenger_ids[i] = g_array_index(arr, PassengerSpend, i).doc_no;
        }

        g_hash_table_insert(q4->weekly_tops, GINT_TO_POINTER(week_idx), wt);
        g_array_free(arr, TRUE);
    }

    // Free the tmporary maps
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

// RUNTIME
void query4(Q4Struct *q4_data, GHashTable *passengers,
            const char *date_begin, const char *date_end,
            FILE *output, int isSpecial)
{
    if (!q4_data)
    {
        fprintf(output, "\n");
        return;
    }

    int start_w, end_w;

    // 1. Determine week range
    // Using parse_unix_date from time_utils.c
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

    // 2. Count Frequencies
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

    // 3. Find rich winner
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
            // Tie-breaker: Lower ID wins (actually unfair)
            if (winner_doc == -1 || doc < winner_doc)
            {
                winner_doc = doc;
            }
        }
    }

    g_hash_table_destroy(freq_map);

    // 4. Output
    if (winner_doc != -1)
    {
        Passenger *p = getPassenger(winner_doc, passengers);
        if (p)
        {

            const char *output_fmt = isSpecial
                                         ? "%09d=%s=%s=%s=%s=%d\n"
                                         : "%09d;%s;%s;%s;%s;%d\n";

            // Format DOB
            time_t dob_t = getPassengerDateOfBirth(p);
            char dob_str[12] = "";
            struct tm info;

            // had to use gmtime_r directly because format_time_t
            // (that we have on timeutils) rejects dates before 1970.
            // gmtime_r handles negative time_t values (before 1970) correctly.
            if (gmtime_r(&dob_t, &info))
            {
                strftime(dob_str, sizeof(dob_str), "%Y-%m-%d", &info);
            }

            fprintf(output, output_fmt,
                    getPassengerDocumentNumber(p),
                    getPassengerFirstName(p),
                    getPassengerLastName(p),
                    dob_str,
                    getPassengerNationality(p),
                    max_freq);

            freePassenger(p);
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