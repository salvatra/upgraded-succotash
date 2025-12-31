#include <core/time_utils.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// module for definiton of helper functions related to time.h
// intended to replace all uses of GDateTime for more efficient code

// Precalculated count of days before each month
// for example, days before june = DAYS_BEFORE_MONTH[5]
static const int DAYS_BEFORE_MONTH[] = {0,   31,  59,  90,  120, 151,
                                        181, 212, 243, 273, 304, 334};

// Determine if an year is a a leap year
// https://www.geeksforgeeks.org/c/c-leap-year/
static inline int is_leap(int y) {
  return (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0));
}

/* NOTE: Slower but more readable parse_unix_datetime version
 *
time_t parse_unix_datetime(const char *dt) {
  if (!dt)
    // ERROR: NULL date
    return (time_t)-1;

  struct tm t = {0};

  const char *end = strptime(dt, "%Y-%m-%d %H:%M", &t);

  if (end == NULL)
    // ERROR: invalid format
    return (time_t)-2;

  if (*end != '\0')
    // ERROR: the whole string wans't used
    return (time_t)-3;

  // Let the system handle daylight savings
  t.tm_isdst = -1;
  time_t seconds = mktime(&t);

  if (seconds == (time_t)-1)
    // ERROR: date too old or too much in the future (out of bounds)
    return (time_t)-4;

  // seconds after epoch
  return seconds;
}
*/

// Parse string to time.h time
int compare_unix_datetime(const char *t1, const char *t2) {
  // pre: t1 != NULL && t2 != NULL

  time_t time1 = parse_unix_datetime(t1, NULL);
  time_t time2 = parse_unix_datetime(t2, NULL);

  if (time1 == -1 || time2 == -1)
    return -2;

  if (time1 < time2)
    return -1;
  else if (time1 > time2)
    return 1;
  else
    return 0;
}

time_t parse_unix_datetime(const char *dt, int *cancelFlag) {
  if (!dt)
    return (time_t)-2;

  //
  if (!strcmp(dt, "N/A")) {
    if (cancelFlag)
      *cancelFlag = 1;
    return (time_t)-1;
  }

  // Super fast function to parse strings into seconds after epoch, using only
  // native maths

  // Check if the format spearators are in the correct positions
  if (dt[4] != '-' || dt[7] != '-' || dt[10] != ' ' || dt[13] != ':')
    return (time_t)-3;

  // Convert string to int via multiplication by the base
  // EX.: "2025" => 2 * 1000 + 0 * 100 + 2 * 10 + 5 * 1 = 2000 + 0 + 20 + 5 =
  // 2025
  int y = (dt[0] - '0') * 1000 + (dt[1] - '0') * 100 + (dt[2] - '0') * 10 +
          (dt[3] - '0');
  int m = (dt[5] - '0') * 10 + (dt[6] - '0');
  int d = (dt[8] - '0') * 10 + (dt[9] - '0');
  int h = (dt[11] - '0') * 10 + (dt[12] - '0');
  int min = (dt[14] - '0') * 10 + (dt[15] - '0');

  // Check if values are not out of bounds
  if (m < 1 || m > 12 || d < 1 || d > 31 || h > 23 || h < 0 || min < 0 ||
      min > 59)
    return (time_t)-4;

  // Determine years since epoch (1970)
  int years_since_epoch = y - 1970;

  // Caculate how many of those years are leap
  int leap_days = (years_since_epoch + 1) / 4;

  // Sum past years' days with current one
  long total_days =
      years_since_epoch * 365 + leap_days + DAYS_BEFORE_MONTH[m - 1] + (d - 1);

  // Add one extra day for February 29th
  if (is_leap(y) && m > 2) {
    total_days++;
  }

  // Convert it all to seconds
  time_t seconds = (total_days * 86400) + (h * 3600) + (min * 60);

  return seconds;
}

time_t parse_unix_date(const char *dt, int *cancelFlag) {
  if (!dt)
    return (time_t)-2;

  //
  if (!strcmp(dt, "N/A")) {
    if (cancelFlag)
      *cancelFlag = 1;
    return (time_t)-1;
  }

  // Super fast function to parse strings into seconds after epoch, using only
  // native maths

  // Check if the format spearators are in the correct positions
  if (dt[4] != '-' || dt[7] != '-')
    return (time_t)-3;

  // Convert string to int via multiplication by the base
  // EX.: "2025" => 2 * 1000 + 0 * 100 + 2 * 10 + 5 * 1 = 2000 + 0 + 20 + 5 =
  // 2025
  int y = (dt[0] - '0') * 1000 + (dt[1] - '0') * 100 + (dt[2] - '0') * 10 +
          (dt[3] - '0');
  int m = (dt[5] - '0') * 10 + (dt[6] - '0');
  int d = (dt[8] - '0') * 10 + (dt[9] - '0');

  if (m < 1 || m > 12 || d < 1 || d > 31)
    return (time_t)-4;

  // Strict Calendar Check
  int days_in_month;
  if (m == 2) {
    days_in_month = is_leap(y) ? 29 : 28;
  } else if (m == 4 || m == 6 || m == 9 || m == 11) {
    days_in_month = 30;
  } else {
    days_in_month = 31;
  }

  if (d > days_in_month) {
    return (time_t)-4;
  }

  // Determine years since epoch (1970)
  int years_since_epoch = y - 1970;

  // Caculate how many of those years are leap
  int leap_days = (years_since_epoch + 1) / 4;

  // Sum past years' days with current one
  long total_days =
      years_since_epoch * 365 + leap_days + DAYS_BEFORE_MONTH[m - 1] + (d - 1);

  // Add one extra day for February 29th
  if (is_leap(y) && m > 2) {
    total_days++;
  }

  // Convert it all to seconds
  time_t seconds = (total_days * 86400);

  return seconds;
}

int compare_time_pointers(gconstpointer a, gconstpointer b) {
  time_t t1 = *(const time_t *)a;
  time_t t2 = *(const time_t *)b;
  if (t1 < t2)
    return -1;
  if (t1 > t2)
    return 1;
  return 0;
}

int compare_time_t(time_t t1, time_t t2) {
  if (t1 < t2)
    return -1;
  if (t1 > t2)
    return 1;
  return 0;
}

int format_time_t(time_t t, char *buffer) {
  // Check if date is valid
  if (t < (time_t)0)
    return -1;

  struct tm info;

  // Convert time_t to struct tm without timezones
  gmtime_r(&t, &info);

  sprintf(buffer, "%04d-%02d-%02d %02d:%02d", info.tm_year + 1900,
          info.tm_mon + 1, info.tm_mday, info.tm_hour, info.tm_min);

  return 0;
}
