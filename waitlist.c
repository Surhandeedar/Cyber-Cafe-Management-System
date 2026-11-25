/*
 * waitlist.c
 * Waiting list (FIFO queue): queue numbers, duplicate check, estimated wait, automatic PC assignment.
 */
#include "config.h"
#include "waitlist.h"
#include "input.h"
#include "timeutil.h"
#include "search.h"
#include "storage.h"
#include "pc.h"
#include "customer.h"

static int estimateWaitMinutes(const Cafe *c, int category, int position)
{
    int rem[TOTAL_PCS];
    int n = 0, i, j, tmp, elapsed, ci, ri;
    time_t now = nowTime(c);

    for (i = 0; i < TOTAL_PCS; i++) {
        if (c->pcs[i].category != category) continue;
        rem[n] = 0;
        if (c->pcs[i].status == PC_OCCUPIED) {
            ci = findCustomerIndex(c, c->pcs[i].customerId);
            if (ci >= 0) {
                elapsed = (int)((now - c->customers[ci].startTime) / 60);
                rem[n] = c->customers[ci].plannedMin - elapsed;
            }
        } else if (c->pcs[i].status == PC_RESERVED) {
            ri = findPendingReservationByPc(c, c->pcs[i].id);
            if (ri >= 0)
                rem[n] = (int)((c->reservations[ri].scheduledAt + GRACE_PERIOD_SEC - now) / 60);
        }
        if (rem[n] < 0) rem[n] = 0;
        n++;
    }
    if (n == 0) return 0;
    for (i = 0; i < n - 1; i++)                     /* bubble sort ascending */
        for (j = 0; j < n - 1 - i; j++)
            if (rem[j] > rem[j + 1]) { tmp = rem[j]; rem[j] = rem[j + 1]; rem[j + 1] = tmp; }
    return rem[(position - 1) % n] + ((position - 1) / n) * DEFAULT_PLAN_MIN;
}

static void removeQueueAt(Cafe *c, int index)
{
    int i;
    for (i = index; i < c->queueCount - 1; i++) c->queue[i] = c->queue[i + 1];
    c->queueCount--;
    saveAll(c);
}

/* Gives freed PCs to waiting customers in FIFO order (per category). */
void processWaitingList(Cafe *c)
{
    int i = 0, pcIdx, custIdx;
    while (i < c->queueCount) {
        custIdx = findCustomerIndex(c, c->queue[i].customerId);
        if (custIdx < 0 || c->customers[custIdx].inSession) {  /* stale entry */
            removeQueueAt(c, i);
            continue;
        }
        pcIdx = findFreePc(c, c->queue[i].category);
        if (pcIdx >= 0) {
            beginSession(c, custIdx, pcIdx, DEFAULT_PLAN_MIN);
            printf("\n>> WAITING LIST: Queue #%d - %s (ID %d) automatically assigned PC %d (%s).\n",
                   c->queue[i].queueNo, c->customers[custIdx].name,
                   c->customers[custIdx].id, c->pcs[pcIdx].id,
                   categoryName(c->pcs[pcIdx].category));
            removeQueueAt(c, i);                  /* next entry shifts into i */
        } else {
            i++;
        }
    }
}

int addToQueue(Cafe *c, int custIdx, int category)
{
    WaitEntry *w;
    int pos, i;
    if (findQueueIndexByCustomer(c, c->customers[custIdx].id) >= 0) {
        printf("  This customer is already in the waiting list.\n");
        return 0;
    }
    if (c->queueCount >= MAX_QUEUE) {
        printf("  The waiting list is full.\n");
        return 0;
    }
    w = &c->queue[c->queueCount];
    w->queueNo = c->nextQueueNo++;
    w->customerId = c->customers[custIdx].id;
    w->category = category;
    w->joinedAt = nowTime(c);
    c->queueCount++;
    saveAll(c);
    pos = 0;
    for (i = 0; i < c->queueCount; i++)
        if (c->queue[i].category == category) pos++;
    printf("  Added to waiting list. Queue number: %d | position in %s line: %d | est. wait: ~%d min\n",
           w->queueNo, categoryName(category), pos, estimateWaitMinutes(c, category, pos));
    return 1;
}

void showWaitingList(const Cafe *c)
{
    int i, j, pos;
    char t[32];
    int ci;
    if (c->queueCount == 0) { printf("  The waiting list is empty.\n"); return; }
    printf("\n========== WAITING LIST (FIFO) ==========\n");
    for (i = 0; i < c->queueCount; i++) {
        pos = 0;
        for (j = 0; j <= i; j++)
            if (c->queue[j].category == c->queue[i].category) pos++;
        ci = findCustomerIndex(c, c->queue[i].customerId);
        formatTime(c->queue[i].joinedAt, t, sizeof(t), "%H:%M");
        printf("  #%-3d | %-20s (ID %d) | %-13s | joined %s | est. wait ~%d min\n",
               c->queue[i].queueNo, ci >= 0 ? c->customers[ci].name : "?",
               c->queue[i].customerId, categoryName(c->queue[i].category), t,
               estimateWaitMinutes(c, c->queue[i].category, pos));
    }
}

void joinWaitingListFlow(Cafe *c)
{
    int custIdx = askCustomerIndex(c), category;
    if (custIdx < 0) return;
    if (c->customers[custIdx].inSession) { printf("  Customer is already in a session.\n"); return; }
    category = readIntInRange("Category 1) Standard Hall  2) VIP Room : ", 1, 2);
    if (findFreePc(c, category) >= 0) {
        printf("  A PC is free right now - start a session instead.\n");
        return;
    }
    addToQueue(c, custIdx, category);
}
