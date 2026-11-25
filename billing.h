/* billing.h - Billing helpers: peak/non-peak rates, per-minute charge, snack total, bill printing. */
#ifndef BILLING_H
#define BILLING_H

#include "config.h"

const char *payName(int m);
void printBill(const Bill *b);
void calculateUsage(int category, time_t start, int minutes, double *usage, int *peakMinutes);
double sumUnbilledSnacks(const Cafe *c, int customerId);
void markOrdersBilled(Cafe *c, int customerId);

#endif
