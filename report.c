/*
 * report.c
 * Admin reports: dashboard, daily sales, snack sales, history, payments, test clock.
 */
#include "config.h"
#include "report.h"
#include "input.h"
#include "timeutil.h"
#include "storage.h"
#include "customer.h"
#include "snack.h"
#include "billing.h"
#include "reservation.h"

/* Completed sessions (bills). customerId 0 = everybody. */
void showSessionHistory(const Cafe *c, int customerId)
{
    int i, n = 0;
    char s[32], e[32];
    printf("\n========== SESSION HISTORY ==========\n");
    for (i = 0; i < c->billCount; i++) {
        const Bill *b = &c->bills[i];
        if (customerId != 0 && b->customerId != customerId) continue;
        formatTime(b->startTime, s, sizeof(s), "%d-%m-%Y %H:%M");
        formatTime(b->endTime, e, sizeof(e), "%H:%M");
        printf("  Bill %-3d | cust %-5d %-15s | PC %-3d | %s - %s | %3d min | PKR %.2f | %s\n",
               b->id, b->customerId, b->customerName, b->pcId, s, e, b->minutes,
               b->grandTotal, payName(b->payMethod));
        n++;
    }
    if (n == 0) printf("  (no records)\n");
}

void historyForCustomerFlow(const Cafe *c)
{
    int idx = askCustomerIndex(c);
    if (idx >= 0) showSessionHistory(c, c->customers[idx].id);
}

void showSnackSales(const Cafe *c)
{
    int i;
    char t[32];
    double total = 0.0;
    printf("\n========== SNACK SALES RECORDS ==========\n");
    for (i = 0; i < c->orderCount; i++) {
        const OrderRecord *o = &c->orders[i];
        formatTime(o->orderedAt, t, sizeof(t), "%d-%m-%Y %H:%M");
        printf("  Order %-3d | cust %-5d | %2d x %-24s | PKR %7.0f | %s\n", o->orderId,
               o->customerId, o->quantity, o->snackName, o->lineTotal, t);
        total += o->lineTotal;
    }
    printf("  Lines: %d | Total snack sales: PKR %.0f\n", c->orderCount, total);
}

void showDailySales(const Cafe *c)
{
    static DailyRow rows[MAX_DAYS];
    char d[11];
    int n = 0, i, j, found;
    DailyRow tmp;

    for (i = 0; i < c->billCount; i++) {
        const Bill *b = &c->bills[i];
        formatTime(b->endTime, d, sizeof(d), "%Y-%m-%d");
        found = -1;
        for (j = 0; j < n; j++) if (strcmp(rows[j].date, d) == 0) { found = j; break; }
        if (found < 0) {
            if (n >= MAX_DAYS) continue;
            found = n++;
            strcpy(rows[found].date, d);
            rows[found].billCount = 0;
            rows[found].gaming = rows[found].snacks = rows[found].total = 0.0;
        }
        rows[found].billCount++;
        rows[found].gaming += b->usageCharge - b->discountAmount;
        rows[found].snacks += b->snackCharge;
        rows[found].total += b->grandTotal;
    }
    for (i = 0; i < n - 1; i++)                         /* sort dates ascending */
        for (j = 0; j < n - 1 - i; j++)
            if (strcmp(rows[j].date, rows[j + 1].date) > 0) {
                tmp = rows[j]; rows[j] = rows[j + 1]; rows[j + 1] = tmp;
            }
    printf("\n============ DAILY SALES ============\n");
    printf("  Date       | Bills | Gaming(net) | Snacks | Total\n");
    for (i = 0; i < n; i++)
        printf("  %s | %5d | %11.2f | %6.0f | %.2f\n", rows[i].date, rows[i].billCount,
               rows[i].gaming, rows[i].snacks, rows[i].total);
    if (n == 0) printf("  (no records)\n");
}

void showDashboard(const Cafe *c)
{
    int i, avail = 0, occ = 0, resv = 0;
    int rp = 0, rc = 0, rx = 0, rk = 0;
    double revenue = 0.0, snackSales = 0.0;

    for (i = 0; i < TOTAL_PCS; i++) {
        if (c->pcs[i].status == PC_AVAILABLE) avail++;
        else if (c->pcs[i].status == PC_OCCUPIED) occ++;
        else resv++;
    }
    for (i = 0; i < c->reservationCount; i++) {
        switch (c->reservations[i].status) {
            case RES_PENDING:   rp++; break;
            case RES_COMPLETED: rc++; break;
            case RES_CANCELLED: rx++; break;
            case RES_EXPIRED:   rk++; break;
        }
    }
    for (i = 0; i < c->billCount; i++) revenue += c->bills[i].grandTotal;
    for (i = 0; i < c->orderCount; i++) snackSales += c->orders[i].lineTotal;

    printf("\n============ ADMIN DASHBOARD ============\n");
    printf("  Registered customers : %d\n", c->customerCount);
    printf("  PCs                  : %d available, %d occupied, %d reserved (of %d)\n",
           avail, occ, resv, TOTAL_PCS);
    printf("  Total reservations   : %d (pending %d, completed %d)\n",
           c->reservationCount, rp, rc);
    printf("  Cancelled reservations: %d (%d by customer, %d expired automatically)\n",
           rx + rk, rx, rk);
    printf("  Customers waiting    : %d\n", c->queueCount);
    printf("  Total snack sales    : PKR %.0f\n", snackSales);
    printf("  Total revenue (bills): PKR %.2f\n", revenue);
    if (showLowStock(c) == 0) printf("  Low-stock snacks     : none\n");
}

void showPayments(const Cafe *c)
{
    int i;
    char t[32];
    printf("\n========== PAYMENT RECORDS ==========\n");
    for (i = 0; i < c->paymentCount; i++) {
        formatTime(c->payments[i].paidAt, t, sizeof(t), "%d-%m-%Y %H:%M");
        printf("  Payment %-3d | bill %-3d | cust %-5d | %-13s | PKR %.2f | %s\n",
               c->payments[i].id, c->payments[i].billId, c->payments[i].customerId,
               payName(c->payments[i].method), c->payments[i].amount, t);
    }
    if (c->paymentCount == 0) printf("  (no payments yet)\n");
}

void advanceClock(Cafe *c)
{
    int minutes = readIntInRange("Advance the cafe clock by how many minutes (0-1440)? ", 0, 1440);
    char t[32];
    c->clockOffsetSec += (long)minutes * 60;
    saveAll(c);
    formatTime(nowTime(c), t, sizeof(t), "%d-%m-%Y %H:%M");
    printf("  Cafe clock is now %s (used for sessions and reservation expiry).\n", t);
    expireReservations(c);
}
