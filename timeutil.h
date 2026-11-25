/* timeutil.h - Time helpers (time.h): current time (test clock), formatting, peak-hour test. */
#ifndef TIMEUTIL_H
#define TIMEUTIL_H

#include "config.h"

time_t nowTime(const Cafe *c);
void formatTime(time_t t, char *buf, size_t n, const char *fmt);
int isPeakHour(time_t t);

#endif
