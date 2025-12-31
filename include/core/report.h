#ifndef REPORT_H
#define REPORT_H

#include <glib.h>

/**
 * @brief Initializes the reporting system.
 *
 * Cleans up files created by previous runs to prevent conflicts.
 */
void initReport(void);

/**
 * @brief Starts a performance timer.
 *
 * @return A pointer to a new GTimer.
 */
GTimer *startTimer(void);

/**
 * @brief Stops a performance timer and returns the elapsed time.
 *
 * @param timer A pointer to the GTimer to stop.
 * @return The elapsed time in seconds.
 */
gdouble stopTimer(GTimer *timer);

/**
 * @brief Reports whether any errors occurred during data loading or processing.
 *
 * @param errorsFlag An integer flag (0 for no errors, 1 for errors).
 */
void reportErrors(gint errorsFlag);

/**
 * @brief Finalizes the reporting process.
 *
 * Cleans up resources and prints any final messages (e.g., "Done").
 */
void reportDone(void);

#endif
