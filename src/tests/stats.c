#include <tests/stats.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>

typedef struct
{
    int total_runs;
    int correct_runs;
    double total_time;
    GPtrArray *errors;
} QueryMetrics;

struct test_stats
{
    GHashTable *metrics_map;
};

// Auxilliary functions

static gint compare_ints(gconstpointer a, gconstpointer b)
{
    int ia = GPOINTER_TO_INT(a);
    int ib = GPOINTER_TO_INT(b);
    return ia - ib;
}

static void free_metrics(gpointer data)
{
    QueryMetrics *m = (QueryMetrics *)data;
    if (m->errors)
    {
        g_ptr_array_free(m->errors, TRUE);
    }
    g_free(m);
}

static QueryMetrics *get_or_create_metrics(TestStats *stats, int query_type)
{
    QueryMetrics *m = g_hash_table_lookup(stats->metrics_map, GINT_TO_POINTER(query_type));
    if (!m)
    {
        m = g_new0(QueryMetrics, 1);
        m->errors = g_ptr_array_new_with_free_func(g_free);
        g_hash_table_insert(stats->metrics_map, GINT_TO_POINTER(query_type), m);
    }
    return m;
}

static long get_max_memory_usage_mb(void)
{
    struct rusage r_usage;
    getrusage(RUSAGE_SELF, &r_usage);
    return r_usage.ru_maxrss / 1024;
}

TestStats *stats_init(void)
{
    TestStats *stats = g_new(TestStats, 1);
    stats->metrics_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, free_metrics);
    return stats;
}

void stats_add_result(TestStats *stats, int query_type, int command_number, int diff_line)
{
    QueryMetrics *m = get_or_create_metrics(stats, query_type);

    m->total_runs++;

    if (diff_line == 0)
    {
        m->correct_runs++;
    }
    else
    {
        char *msg = g_strdup_printf("Differences in query %d (command %d): error detected on line %d of output.",
                                    query_type, command_number, diff_line);
        g_ptr_array_add(m->errors, msg);
    }
}

void stats_add_timing(TestStats *stats, int query_type, double time_seconds)
{
    QueryMetrics *m = get_or_create_metrics(stats, query_type);
    m->total_time += time_seconds;
}

void stats_print_report(TestStats *stats, double total_time_seconds)
{
    GList *keys = g_hash_table_get_keys(stats->metrics_map);
    keys = g_list_sort(keys, compare_ints);

    printf("\nResults:\n");

    for (GList *l = keys; l != NULL; l = l->next)
    {
        int q_type = GPOINTER_TO_INT(l->data);
        QueryMetrics *m = g_hash_table_lookup(stats->metrics_map, l->data);
        printf("Q%d: %d of %d tests ok!\n", q_type, m->correct_runs, m->total_runs);
    }

    for (GList *l = keys; l != NULL; l = l->next)
    {
        QueryMetrics *m = g_hash_table_lookup(stats->metrics_map, l->data);
        for (guint i = 0; i < m->errors->len; i++)
        {
            printf("%s\n", (char *)g_ptr_array_index(m->errors, i));
        }
    }

    printf("\nMem used: %ld MB\n", get_max_memory_usage_mb());
    printf("Runtime (total acumulated):\n");

    for (GList *l = keys; l != NULL; l = l->next)
    {
        int q_type = GPOINTER_TO_INT(l->data);
        QueryMetrics *m = g_hash_table_lookup(stats->metrics_map, l->data);
        printf("Q%d: %.1f ms\n", q_type, m->total_time * 1000.0);
    }

    printf("Total time: %.0f ms\n", total_time_seconds * 1000.0);

    fflush(stdout);

    g_list_free(keys);
}

void stats_free(TestStats *stats)
{
    if (stats)
    {
        g_hash_table_destroy(stats->metrics_map);
        g_free(stats);
    }
}