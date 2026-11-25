/*
 * startup.c
 * Start-up: load cafe_records.txt, create defaults if it is empty/missing, make PC status agree with the records.
 */
#include "config.h"
#include "startup.h"
#include "search.h"
#include "storage.h"

/* Rebuilds PC status from sessions + pending reservations so the PC list
 * always agrees with the customer and reservation records after a restart. */
static void reconcileState(Cafe *c)
{
    int i, pcIdx;
    for (i = 0; i < TOTAL_PCS; i++) {
        c->pcs[i].status = PC_AVAILABLE;
        c->pcs[i].customerId = 0;
    }
    for (i = 0; i < c->reservationCount; i++) {
        if (c->reservations[i].status != RES_PENDING) continue;
        pcIdx = findPcIndex(c, c->reservations[i].pcId);
        if (pcIdx >= 0) {
            c->pcs[pcIdx].status = PC_RESERVED;
            c->pcs[pcIdx].customerId = c->reservations[i].customerId;
        }
    }
    for (i = 0; i < c->customerCount; i++) {
        if (!c->customers[i].inSession) continue;
        pcIdx = findPcIndex(c, c->customers[i].pcId);
        if (pcIdx >= 0) {
            c->pcs[pcIdx].status = PC_OCCUPIED;
            c->pcs[pcIdx].customerId = c->customers[i].id;
        }
    }
}

/* Called once when the program starts: loads cafe_records.txt (or creates
 * the defaults if the file is missing/empty) and saves it back. */
void loadAll(Cafe *c)
{
    int i, maxId, pcsLoaded;

    memset(c, 0, sizeof(*c));
    c->nextQueueNo = 1;
    c->nextCustomerId = FIRST_CUSTOMER_ID;
    pcsLoaded = loadFromFile(c);
    if (pcsLoaded != TOTAL_PCS) initDefaultPcs(c);
    if (c->snackCount == 0) seedSnacks(c);

    maxId = 0;                                     /* never reuse a snack id */
    for (i = 0; i < c->snackCount; i++)
        if (c->snacks[i].id > maxId) maxId = c->snacks[i].id;
    if (c->nextSnackId <= maxId) c->nextSnackId = maxId + 1;
    if (c->customerCount > 0 && c->nextCustomerId <= c->customers[c->customerCount - 1].id)
        c->nextCustomerId = c->customers[c->customerCount - 1].id + 1;

    reconcileState(c);
    saveAll(c);                                    /* creates the file if it was missing */
    printf("Records file: %s (saved in the folder where you run the program)\n", RECORDS_FILE);
    printf("Loaded: %d customers, %d reservations, %d waiting, %d snack items, %d snack orders, %d bills.\n",
           c->customerCount, c->reservationCount, c->queueCount, c->snackCount,
           c->orderCount, c->billCount);
}
