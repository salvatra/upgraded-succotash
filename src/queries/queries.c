#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <ctype.h>
#include "queries/queries.h"
#include "queries/query_module.h"

extern QueryModule get_query1_module(void);
extern QueryModule get_query2_module(void);
extern QueryModule get_query3_module(void);
extern QueryModule get_query4_module(void);
extern QueryModule get_query5_module(void);
extern QueryModule get_query6_module(void);

struct QueryManager
{
  GHashTable *modules;
  GHashTable *contexts;
};

QueryManager *query_manager_create(Dataset *ds)
{
  QueryManager *qm = g_new0(QueryManager, 1);
  qm->modules = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);
  qm->contexts = g_hash_table_new(g_direct_hash, g_direct_equal);

  QueryModule mods[6];
  mods[0] = get_query1_module();
  mods[1] = get_query2_module();
  mods[2] = get_query3_module();
  mods[3] = get_query4_module();
  mods[4] = get_query5_module();
  mods[5] = get_query6_module();

  for (int i = 0; i < 6; i++)
  {
    QueryModule *m = g_new(QueryModule, 1);
    *m = mods[i];
    g_hash_table_insert(qm->modules, GINT_TO_POINTER(m->id), m);
    if (m->init)
    {
      void *ctx = m->init(ds);
      g_hash_table_insert(qm->contexts, GINT_TO_POINTER(m->id), ctx);
    }
  }
  return qm;
}

void query_manager_destroy(QueryManager *qm)
{
  if (!qm)
    return;
  GHashTableIter iter;
  gpointer key, value;
  g_hash_table_iter_init(&iter, qm->modules);
  while (g_hash_table_iter_next(&iter, &key, &value))
  {
    QueryModule *mod = (QueryModule *)value;
    void *ctx = g_hash_table_lookup(qm->contexts, key);
    if (mod->destroy && ctx)
      mod->destroy(ctx);
  }
  g_hash_table_destroy(qm->contexts);
  g_hash_table_destroy(qm->modules);
  g_free(qm);
}

int query_manager_execute(QueryManager *qm, int queryId, char *arg1, char *arg2,
                          int isSpecial, FILE *output, Dataset *ds)
{
  if (!qm)
    return -1;
  QueryModule *mod = g_hash_table_lookup(qm->modules, GINT_TO_POINTER(queryId));
  void *qCtx = g_hash_table_lookup(qm->contexts, GINT_TO_POINTER(queryId));
  if (mod && mod->run)
  {
    mod->run(qCtx, ds, arg1, arg2, isSpecial, output);
    return 0;
  }
  return -1;
}

void runAllQueries(Dataset *ds, const char *filePath, QueryStatsCallback callback, void *ctx)
{
  QueryManager *qm = query_manager_create(ds);
  if (!qm)
    return;

  FILE *inputFile = fopen(filePath, "r");
  if (!inputFile)
  {
    query_manager_destroy(qm);
    return;
  }

  char line[1024];
  int lineNumber = 1;

  while (fgets(line, sizeof(line), inputFile))
  {
    line[strcspn(line, "\r\n")] = '\0';
    if (strlen(line) == 0)
      continue;

    char queryIdStr[16];
    int bytesRead = 0;
    if (sscanf(line, "%15s%n", queryIdStr, &bytesRead) < 1)
    {
      lineNumber++;
      continue;
    }

    int isSpecial = 0;
    size_t idLen = strlen(queryIdStr);
    if (idLen > 0 && isalpha(queryIdStr[idLen - 1]))
    {
      isSpecial = 1;
      queryIdStr[idLen - 1] = '\0';
    }
    int queryNumber = atoi(queryIdStr);

    char *argsArea = line + bytesRead;
    while (isspace(*argsArea))
      argsArea++;

    char *arg1 = NULL;
    char *arg2 = NULL;

    if (*argsArea != '\0')
    {
      if (queryNumber == 2 || queryNumber == 3 || queryNumber == 4)
      {
        arg1 = argsArea;
        char *space = strchr(argsArea, ' ');
        if (space)
        {
          *space = '\0';
          arg2 = space + 1;
          while (isspace(*arg2))
            arg2++;
        }
      }
      else
      {
        arg1 = argsArea;
      }
    }

    char outputFileName[64];
    snprintf(outputFileName, sizeof(outputFileName), "resultados/command%d_output.txt", lineNumber);
    FILE *output = fopen(outputFileName, "w");

    if (output)
    {
      GTimer *timer = g_timer_new();
      if (query_manager_execute(qm, queryNumber, arg1, arg2, isSpecial, output, ds) != 0)
      {
        fprintf(output, "\n");
      }
      gdouble elapsed = g_timer_elapsed(timer, NULL);
      g_timer_destroy(timer);
      fclose(output);
      if (callback)
        callback(queryNumber, lineNumber, elapsed, ctx);
    }
    lineNumber++;
  }

  fclose(inputFile);
  query_manager_destroy(qm);
}