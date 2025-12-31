#ifndef RUNNER_H
#define RUNNER_H

/**
 * @brief Opaque structure for managing test execution.
 */
typedef struct test_runner TestRunner;

/**
 * @brief Initializes a new TestRunner.
 *
 * @return Pointer to the allocated TestRunner.
 */
TestRunner *tests_init(void);

/**
 * @brief Runs the test suite.
 *
 * Executes queries using the provided dataset and input commands, then compares results with expected outputs.
 *
 * @param runner Pointer to the TestRunner.
 * @param datasetPath Path to the dataset directory.
 * @param inputPath Path to the input commands file.
 * @param expectedPath Path to the expected output file.
 */
void tests_run(TestRunner *runner, const char *datasetPath, const char *inputPath, const char *expectedPath);

/**
 * @brief Prints the final test report (passed/failed tests, timing).
 *
 * @param runner Pointer to the TestRunner.
 */
void tests_print_report(TestRunner *runner);

/**
 * @brief Frees the TestRunner resources.
 *
 * @param runner Pointer to the TestRunner.
 */
void tests_free(TestRunner *runner);

#endif