#include "glib.h"
#include <core/dataset.h>
#include <core/report.h>
#include <queries/queries.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Needs dataset and input file paths\n");
    return EXIT_FAILURE;
  }

  const char *datasetPath = argv[1];
  const char *inputFilePath = argv[2];

  Dataset *ds = initDataset();
  gint errors = 0;

  initReport();

  loadAllDatasets(ds, &errors, datasetPath, FALSE);
  // if (!validateDataset(ds)) errors = 1;

  runAllQueries(ds, inputFilePath, NULL, NULL);
  cleanupDataset(ds);
  reportErrors(errors);
  reportDone();

  return EXIT_SUCCESS;
}
