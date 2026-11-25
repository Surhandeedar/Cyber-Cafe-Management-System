/*
 * search.c
 * Searching: find a PC, customer (binary search), reservation, queue entry or snack in the in-memory arrays.
 */
#include "config.h"
#include "search.h"

int findPcIndex(const Cafe *c, int pcId)
{
    int i;
    for (i = 0; i < TOTAL_PCS; i++)
        if (c->pcs[i].id == pcId) return i;
    return -1;
}

int findFreePc(const Cafe *c, int category)
{
    int i;
    for (i = 0; i < TOTAL_PCS; i++)
        if (c->pcs[i].category == category && c->pcs[i].status == PC_AVAILABLE)
            return i;
    return -1;
}

/* Binary search: customers are always stored in ascending id order. */
int findCustomerIndex(const Cafe *c, int id)
{
    int lo = 0, hi = c->customerCount - 1, mid;
    while (lo <= hi) {
        mid = (lo + hi) / 2;
        if (c->customers[mid].id == id) return mid;
        if (c->customers[mid].id < id) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

int findPendingReservationByCustomer(const Cafe *c, int customerId)
{
    int i;
    for (i = 0; i < c->reservationCount; i++)
        if (c->reservations[i].customerId == customerId &&
            c->reservations[i].status == RES_PENDING) return i;
    return -1;
}

int findPendingReservationByPc(const Cafe *c, int pcId)
{
    int i;
    for (i = 0; i < c->reservationCount; i++)
        if (c->reservations[i].pcId == pcId &&
            c->reservations[i].status == RES_PENDING) return i;
    return -1;
}

int findQueueIndexByCustomer(const Cafe *c, int customerId)
{
    int i;
    for (i = 0; i < c->queueCount; i++)
        if (c->queue[i].customerId == customerId) return i;
    return -1;
}

int findSnackIndex(const Cafe *c, int snackId)
{
    int i;
    for (i = 0; i < c->snackCount; i++)
        if (c->snacks[i].id == snackId) return i;
    return -1;
}
