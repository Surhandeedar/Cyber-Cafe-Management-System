/*
 * session.c
 * Sessions: start a session on a chosen PC, end a session (bill, payment, release PC, serve the queue).
 */
#include "config.h"
#include "session.h"
#include "input.h"
#include "timeutil.h"
#include "search.h"
#include "storage.h"
#include "pc.h"
#include "customer.h"
#include "billing.h"
#include "waitlist.h"

/* Start a session: the customer/staff picks the category, then picks
 * WHICH free PC they want (same for Standard and VIP). */
void startSessionFlow(Cafe *c)
{
    int custIdx, category, pcIdx, planned, pcId, shown;
    Customer *cu;

    custIdx = askCustomerIndex(c);
    if (custIdx < 0) return;
    cu = &c->customers[custIdx];
    if (cu->inSession) { printf("  Customer is already using PC %d.\n", cu->pcId); return; }
    if (findPendingReservationByCustomer(c, cu->id) >= 0) {
        printf("  Customer has a reservation - use the check-in option.\n");
        return;
    }
    if (findQueueIndexByCustomer(c, cu->id) >= 0) {
        printf("  Customer is already waiting in the queue.\n");
        return;
    }
    category = readIntInRange("Category 1) Standard Hall  2) VIP Room : ", 1, 2);
    printf("Free PCs in %s:\n", categoryName(category));
    shown = listPcsByStatus(c, PC_AVAILABLE, category);
    if (shown == 0) {
        printf("  All %s PCs are busy or reserved.\n", categoryName(category));
        if (readYesNo("Join the waiting list? (y/n): ")) addToQueue(c, custIdx, category);
        return;
    }
    for (;;) {
        pcId = readIntInRange("Enter the PC id you want from the list above (0 to cancel): ", 0, 999);
        if (pcId == 0) return;
        pcIdx = findPcIndex(c, pcId);
        if (pcIdx >= 0 && c->pcs[pcIdx].category == category &&
            c->pcs[pcIdx].status == PC_AVAILABLE) break;
        printf("  That PC is not in the free list above.\n");
    }
    planned = readIntInRange("Expected session length in minutes: ", 1, MAX_PLAN_MIN);
    beginSession(c, custIdx, pcIdx, planned);
    printf("  Session started on PC %d (%s). Status: Occupied.\n", c->pcs[pcIdx].id,
           categoryName(category));
}

/* End a session: calculate the bill, take payment, record everything,
 * free the PC and give it to the next waiting customer. */
void endSessionFlow(Cafe *c)
{
    int custIdx, pcIdx;
    Customer *cu;
    Bill b;
    Payment *p;
    time_t now;

    custIdx = askCustomerIndex(c);
    if (custIdx < 0) return;
    cu = &c->customers[custIdx];
    if (!cu->inSession) { printf("  This customer has no active session.\n"); return; }
    if (c->billCount >= MAX_BILLS || c->paymentCount >= MAX_PAYMENTS) {
        printf("  Bill storage is full.\n");
        return;
    }
    pcIdx = findPcIndex(c, cu->pcId);
    now = nowTime(c);

    memset(&b, 0, sizeof(b));
    b.id = c->billCount > 0 ? c->bills[c->billCount - 1].id + 1 : 1;
    b.customerId = cu->id;
    strncpy(b.customerName, cu->name, NAME_LEN - 1);
    b.pcId = cu->pcId;
    b.category = c->pcs[pcIdx].category;
    b.startTime = cu->startTime;
    b.endTime = now;
    b.minutes = (int)((now - cu->startTime + 59) / 60);      /* round up */
    if (b.minutes < 1) b.minutes = 1;
    calculateUsage(b.category, b.startTime, b.minutes, &b.usageCharge, &b.peakMinutes);
    b.snackCharge = sumUnbilledSnacks(c, cu->id);
    if (cu->type == CUST_SQUAD && cu->squadSize >= SQUAD_MIN_MEMBERS)
        b.discountPct = SQUAD_DISCOUNT_PCT;
    b.discountAmount = b.usageCharge * b.discountPct / 100.0;
    b.grandTotal = b.usageCharge + b.snackCharge - b.discountAmount;

    printBill(&b);
    printf("Payment method 1) Cash  2) Card  3) Mobile Wallet\n");
    b.payMethod = readIntInRange("Select (1-3): ", 1, 3);

    c->bills[c->billCount++] = b;                            /* bill record */
    p = &c->payments[c->paymentCount];                       /* payment record */
    p->id = c->paymentCount > 0 ? c->payments[c->paymentCount - 1].id + 1 : 1;
    p->billId = b.id;
    p->customerId = b.customerId;
    p->method = b.payMethod;
    p->amount = b.grandTotal;
    p->paidAt = now;
    c->paymentCount++;
    markOrdersBilled(c, cu->id);

    c->pcs[pcIdx].status = PC_AVAILABLE;
    c->pcs[pcIdx].customerId = 0;
    cu->inSession = 0;
    cu->pcId = 0;
    cu->durationMin = b.minutes;
    cu->sessionCount++;
    saveAll(c);
    printf("  Payment of PKR %.2f received (%s). PC %d is now Available.\n", b.grandTotal,
           payName(b.payMethod), b.pcId);
    processWaitingList(c);
}
