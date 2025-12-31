#include <stdio.h>
#include <stdlib.h>
#include <tests/runner.h>

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Use: %s <dataset_path> <input_file> <expected_results_path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *datasetPath = argv[1];
    const char *inputPath = argv[2];
    const char *expectedPath = argv[3];

    TestRunner *runner = tests_init();
    tests_run(runner, datasetPath, inputPath, expectedPath);
    tests_print_report(runner);
    tests_free(runner);

    return EXIT_SUCCESS;
}