#include <core/dataset.h>
#include <core/report.h>
#include <glib.h>
#include <queries/queries.h>
#include <stdio.h>
#include <tests/comparison.h>
#include <tests/runner.h>
#include <tests/stats.h>
#include <io/manager.h>

typedef struct
{
  TestStats *stats;
  const char *expectedPath;
} TestContext;

struct test_runner
{
  TestStats *stats;
  GTimer *total_timer;
};

static void on_query_complete(int queryNum, int lineNum, double elapsed,
                              void *ctx)
{
  TestContext *tc = (TestContext *)ctx;

  stats_add_timing(tc->stats, queryNum, elapsed);
  char generated_file[256];
  char expected_file[256];

  snprintf(generated_file, sizeof(generated_file),
           "resultados/command%d_output.txt", lineNum);
  snprintf(expected_file, sizeof(expected_file), "%s/command%d_output.txt",
           tc->expectedPath, lineNum);

  int diff_line = 0;
  gboolean passed = compare_files(generated_file, expected_file, &diff_line);

  stats_add_result(tc->stats, queryNum, lineNum, passed ? 0 : diff_line);
}

TestRunner *tests_init(void)
{
  TestRunner *runner = g_new0(TestRunner, 1);
  runner->stats = stats_init();
  runner->total_timer = g_timer_new();
  return runner;
}

void tests_run(TestRunner *runner, const char *datasetPath,
               const char *inputPath, const char *expectedPath)
{
  g_timer_start(runner->total_timer);
  initReport();

  printf("Loading datasets...\n");
  gint errors = 0;
  Dataset *ds = initDataset();

  loadAllDatasets(ds, &errors, datasetPath, TRUE);

  printf("Datasets loaded and validated.\n\n");
  printf("Running and checking queries...\n");

  TestContext ctx;
  ctx.stats = runner->stats;
  ctx.expectedPath = expectedPath;

  runAllQueries(ds, inputPath, on_query_complete, &ctx);
  printf("All done :)\n\n");
  cleanupDataset(ds);
  reportErrors(errors);
  g_timer_stop(runner->total_timer);
}

void tests_print_report(TestRunner *runner)
{
  double elapsed = g_timer_elapsed(runner->total_timer, NULL);
  stats_print_report(runner->stats, elapsed);
}

void tests_free(TestRunner *runner)
{
  if (runner)
  {
    stats_free(runner->stats);
    g_timer_destroy(runner->total_timer);
    g_free(runner);
  }
}