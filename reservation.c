/*
 * reservation.c
 * Reservations with real time-based expiry (time.h) and the 15-minute grace period.
 */
#include "config.h"
#include "reservation.h"
#include "input.h"
#include "timeutil.h"
#include "search.h"
#include "storage.h"
#include "pc.h"
#include "customer.h"
#include "waitlist.h"

static const char *resStatusName(int s)
{
    switch (s) {
        case RES_PENDING:   return "Pending";
        case RES_COMPLETED: return "Completed";
        case RES_CANCELLED: return "Cancelled";
        case RES_EXPIRED:   return "Expired";
    }
    return "Unknown";
}

/* Cancels pending reservations whose arrival time + grace period has passed. */
void expireReservations(Cafe *c)
{
    int i, pcIdx, expired = 0;
    time_t now = nowTime(c);
    for (i = 0; i < c->reservationCount; i++) {
        Reservation *r = &c->reservations[i];
        if (r->status != RES_PENDING) continue;
        if (difftime(now, r->scheduledAt) > GRACE_PERIOD_SEC) {
            r->status = RES_EXPIRED;
            pcIdx = findPcIndex(c, r->pcId);
            if (pcIdx >= 0 && c->pcs[pcIdx].status == PC_RESERVED) {
                c->pcs[pcIdx].status = PC_AVAILABLE;
                c->pcs[pcIdx].customerId = 0;
            }
            printf("\n>> Reservation #%d (customer %d) EXPIRED after the %d-minute grace period. PC %d released.\n",
                   r->id, r->customerId, GRACE_PERIOD_MIN, r->pcId);
            expired++;
        }
    }
    if (expired > 0) {
        saveAll(c);
        processWaitingList(c);
    }
}

void createReservation(Cafe *c)
{
    int custIdx, category, pcId, pcIdx, lead, i, shown;
    Reservation r;
    char t[32];

    if (c->reservationCount >= MAX_RESERVATIONS) { printf("  Reservation storage is full.\n"); return; }
    custIdx = askCustomerIndex(c);
    if (custIdx < 0) return;
    if (c->customers[custIdx].inSession) { printf("  Customer is already in a session.\n"); return; }
    if (findPendingReservationByCustomer(c, c->customers[custIdx].id) >= 0) {
        printf("  Customer already has a pending reservation.\n");
        return;
    }
    if (findQueueIndexByCustomer(c, c->customers[custIdx].id) >= 0) {
        printf("  Customer is in the waiting list.\n");
        return;
    }
    category = readIntInRange("Category 1) Standard Hall  2) VIP Room : ", 1, 2);
    printf("Free PCs in %s:\n", categoryName(category));
    shown = listPcsByStatus(c, PC_AVAILABLE, category);
    if (shown == 0) { printf("  No free PCs to reserve right now.\n"); return; }
    for (;;) {
        pcId = readIntInRange("Choose PC id to reserve (0 to cancel): ", 0, 999);
        if (pcId == 0) return;
        pcIdx = findPcIndex(c, pcId);
        if (pcIdx >= 0 && c->pcs[pcIdx].category == category &&
            c->pcs[pcIdx].status == PC_AVAILABLE) break;
        printf("  That PC is not in the free list above.\n");
    }
    lead = readIntInRange("Arrival in how many minutes from now (0-120)? ", 0, MAX_RESERVE_LEAD);

    r.id = 1;
    for (i = 0; i < c->reservationCount; i++)       /* extra safety: unique id */
        if (c->reservations[i].id >= r.id) r.id = c->reservations[i].id + 1;
    r.customerId = c->customers[custIdx].id;
    r.pcId = pcId;
    r.category = category;
    r.createdAt = nowTime(c);
    r.scheduledAt = r.createdAt + (time_t)lead * 60;
    r.status = RES_PENDING;
    c->reservations[c->reservationCount++] = r;
    c->pcs[pcIdx].status = PC_RESERVED;
    c->pcs[pcIdx].customerId = r.customerId;
    saveAll(c);
    formatTime(r.scheduledAt, t, sizeof(t), "%d-%m-%Y %H:%M");
    printf("  Reservation #%d created: PC %d (%s) for %s.\n", r.id, pcId, categoryName(category), t);
    printf("  It is cancelled automatically if you do not arrive within %d minutes after that time.\n",
           GRACE_PERIOD_MIN);
}

void checkInReservation(Cafe *c)
{
    int custIdx, ri, pcIdx, planned;
    custIdx = askCustomerIndex(c);
    if (custIdx < 0) return;
    ri = findPendingReservationByCustomer(c, c->customers[custIdx].id);
    if (ri < 0) { printf("  No pending reservation for this customer.\n"); return; }
    pcIdx = findPcIndex(c, c->reservations[ri].pcId);
    planned = readIntInRange("Expected session length in minutes: ", 1, MAX_PLAN_MIN);
    c->reservations[ri].status = RES_COMPLETED;
    saveAll(c);
    beginSession(c, custIdx, pcIdx, planned);
    printf("  Checked in. Session started on PC %d.\n", c->pcs[pcIdx].id);
}

void cancelReservation(Cafe *c)
{
    int custIdx, ri, pcIdx;
    custIdx = askCustomerIndex(c);
    if (custIdx < 0) return;
    ri = findPendingReservationByCustomer(c, c->customers[custIdx].id);
    if (ri < 0) { printf("  No pending reservation for this customer.\n"); return; }
    c->reservations[ri].status = RES_CANCELLED;
    pcIdx = findPcIndex(c, c->reservations[ri].pcId);
    if (pcIdx >= 0) {
        c->pcs[pcIdx].status = PC_AVAILABLE;
        c->pcs[pcIdx].customerId = 0;
    }
    saveAll(c);
    printf("  Reservation #%d cancelled and PC %d released.\n", c->reservations[ri].id, c->reservations[ri].pcId);
    processWaitingList(c);
}

void showReservations(const Cafe *c)
{
    int i;
    char sch[32];
    if (c->reservationCount == 0) { printf("  No reservations recorded.\n"); return; }
    printf("\n============ RESERVATIONS ============\n");
    for (i = 0; i < c->reservationCount; i++) {
        formatTime(c->reservations[i].scheduledAt, sch, sizeof(sch), "%d-%m-%Y %H:%M");
        printf("  #%-3d | cust %-5d | PC %-3d (%-13s) | arrival %s | %s\n",
               c->reservations[i].id, c->reservations[i].customerId, c->reservations[i].pcId,
               categoryName(c->reservations[i].category), sch,
               resStatusName(c->reservations[i].status));
    }
}
