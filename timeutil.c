/*
 * timeutil.c
 * Time helpers (time.h): current time (test clock), formatting, peak-hour test.
 */
#include "config.h"
#include "timeutil.h"

time_t nowTime(const Cafe *c)
{
    return time(NULL) + c->clockOffsetSec;
}

void formatTime(time_t t, char *buf, size_t n, const char *fmt)
{
    struct tm *tmv = localtime(&t);
    if (tmv == NULL) { strncpy(buf, "?", n); return; }
    strftime(buf, n, fmt, tmv);
}

int isPeakHour(time_t t)
{
    struct tm *tmv = localtime(&t);
    if (tmv == NULL) return 0;
    return tmv->tm_hour >= PEAK_START_HOUR && tmv->tm_hour < PEAK_END_HOUR;
}
