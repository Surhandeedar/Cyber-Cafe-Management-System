/* report.h - Admin reports: dashboard, daily sales, snack sales, history, payments, test clock. */
#ifndef REPORT_H
#define REPORT_H

#include "config.h"

void showSessionHistory(const Cafe *c, int customerId);
void historyForCustomerFlow(const Cafe *c);
void showSnackSales(const Cafe *c);
void showDailySales(const Cafe *c);
void showDashboard(const Cafe *c);
void showPayments(const Cafe *c);
void advanceClock(Cafe *c);

#endif
