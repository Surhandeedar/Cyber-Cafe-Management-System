/*
 * billing.c
 * Billing helpers: peak/non-peak rates, per-minute charge, snack total, bill printing.
 */
#include "config.h"
#include "billing.h"
#include "timeutil.h"
#include "pc.h"

const char *payName(int m)
{
    switch (m) {
        case PAY_CASH:   return "Cash";
        case PAY_CARD:   return "Card";
        case PAY_MOBILE: return "Mobile Wallet";
    }
    return "Unknown";
}

static double hourlyRate(int category, int peak)
{
    if (category == CAT_VIP) return peak ? VIP_RATE_PEAK : VIP_RATE_OFFPEAK;
    return peak ? STD_RATE_PEAK : STD_RATE_OFFPEAK;
}

void printBill(const Bill *b)
{
    char s[32], e[32];
    double subtotal = b->usageCharge + b->snackCharge;
    formatTime(b->startTime, s, sizeof(s), "%d-%m-%Y %H:%M");
    formatTime(b->endTime, e, sizeof(e), "%d-%m-%Y %H:%M");
    printf("\n============== BILL #%d ==============\n", b->id);
    printf("Customer     : %d - %s\n", b->customerId, b->customerName);
    printf("PC           : %d (%s)\n", b->pcId, categoryName(b->category));
    printf("Start        : %s\n", s);
    printf("End          : %s\n", e);
    printf("Duration     : %d min (%d peak, %d non-peak)\n", b->minutes,
           b->peakMinutes, b->minutes - b->peakMinutes);
    printf("Hourly rate  : PKR %.0f non-peak / PKR %.0f peak\n",
           hourlyRate(b->category, 0), hourlyRate(b->category, 1));
    printf("Gaming charge: PKR %.2f\n", b->usageCharge);
    printf("Snack charge : PKR %.2f\n", b->snackCharge);
    printf("Original amt : PKR %.2f\n", subtotal);
    printf("Squad disc.  : %d%% of gaming = PKR %.2f\n", b->discountPct, b->discountAmount);
    printf("GRAND TOTAL  : PKR %.2f\n", b->grandTotal);
    if (b->payMethod != 0) printf("Paid by      : %s\n", payName(b->payMethod));
    printf("======================================\n");
}

/* Charges per minute so a session crossing 18:00 / 23:00 is billed correctly. */
void calculateUsage(int category, time_t start, int minutes,
                           double *usage, int *peakMinutes)
{
    int m, peak;
    double total = 0.0;
    *peakMinutes = 0;
    for (m = 0; m < minutes; m++) {
        peak = isPeakHour(start + (time_t)m * 60);
        if (peak) (*peakMinutes)++;
        total += hourlyRate(category, peak) / 60.0;
    }
    *usage = total;
}

/* Total of this customer's snack orders that are not on a bill yet. */
double sumUnbilledSnacks(const Cafe *c, int customerId)
{
    int i;
    double total = 0.0;
    for (i = 0; i < c->orderCount; i++)
        if (c->orders[i].customerId == customerId && !c->orders[i].billed)
            total += c->orders[i].lineTotal;
    return total;
}

/* After billing, mark those orders as billed so they are never charged twice. */
void markOrdersBilled(Cafe *c, int customerId)
{
    int i;
    for (i = 0; i < c->orderCount; i++)
        if (c->orders[i].customerId == customerId)
            c->orders[i].billed = 1;
}
