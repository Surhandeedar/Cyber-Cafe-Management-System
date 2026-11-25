/*
 * storage.c
 * FILE HANDLING. Every record is saved in ONE text file: cafe_records.txt. saveAll() writes it after every change, loadFromFile() reads it at start-up.
 */
#include "config.h"
#include "storage.h"
#include "timeutil.h"

static const char *SEED_NAMES[] = {
    "Respawn Fries", "AFK Fries", "Frag Nuggets", "GG Nachos",
    "Combo Breaker Wings", "Boss Level Popcorn", "The Headshot Slider",
    "Speedrun Noodles", "Hp Boost Rice Bowl", "Loot Box Cookie",
    "Glitch-Free Brownie", "Co-Op Pastry", "Keyboard Clean Nuts",
    "Pixel Pretzel Bites", "Stamina Bar", "Ping Booster", "Dev Mode Black",
    "Karak Stream Chai", "Rage Quit Energy", "Overclocked Overdrive",
    "Mana Potion Shake", "Liquid Cooling Iced Tea", "NPC Soda", "Base Hydration"
};
static const double SEED_PRICES[] = {
    550, 300, 450, 450, 550, 200, 350, 300, 480, 180, 250, 350,
    300, 220, 200, 480, 350, 150, 150, 750, 450, 320, 130, 80
};
#define SEED_COUNT 24

void initDefaultPcs(Cafe *c)
{
    int i;
    for (i = 0; i < TOTAL_PCS; i++) {
        c->pcs[i].id = (i < STD_PC_COUNT) ? i + 1 : VIP_ID_BASE + (i - STD_PC_COUNT) + 1;
        c->pcs[i].category = (i < STD_PC_COUNT) ? CAT_STANDARD : CAT_VIP;
        c->pcs[i].status = PC_AVAILABLE;
        c->pcs[i].customerId = 0;
    }
}

void seedSnacks(Cafe *c)
{
    int i;
    for (i = 0; i < SEED_COUNT; i++) {
        c->snacks[i].id = i + 1;
        strncpy(c->snacks[i].name, SEED_NAMES[i], SNACK_NAME_LEN - 1);
        c->snacks[i].name[SNACK_NAME_LEN - 1] = '\0';
        c->snacks[i].price = SEED_PRICES[i];
        c->snacks[i].quantity = SEED_STOCK;
        c->snacks[i].threshold = SEED_THRESHOLD;
    }
    c->snackCount = SEED_COUNT;
    c->nextSnackId = SEED_COUNT + 1;
}

/* ---------- build the file ---------- */

/* Readable time for the extra column at the end of some lines ('-' if not set). */
static void readableTime(time_t t, char *buf, size_t n)
{
    if (t == 0) { strncpy(buf, "-", n); buf[n - 1] = '\0'; return; }
    formatTime(t, buf, n, "%Y-%m-%d %H:%M");
}

/* Saves EVERYTHING into cafe_records.txt. It is called after every change,
 * so the file is always up to date. The data is first written to a temporary
 * file, then the old file is replaced - this way a crash can not leave a
 * half-written records file. */
int saveAll(const Cafe *c)
{
    FILE *fp = fopen(RECORDS_TMP, "w");
    int i;
    char t1[32], t2[32];

    if (fp == NULL) {
        printf("  [File error] Cannot write %s\n", RECORDS_TMP);
        return 0;
    }
    fputs("# CYBER CAFE RECORDS FILE - written automatically by the program.\n", fp);
    fputs("# Open it in Notepad to see every record. Fields are separated by '|'.\n", fp);
    fputs("# Do not edit it while the program is running.\n\n", fp);

    fputs("[COUNTERS]\n# nextQueueNo|nextCustomerId|nextSnackId|clockOffsetSeconds\n", fp);
    fprintf(fp, "%d|%d|%d|%ld\n\n", c->nextQueueNo, c->nextCustomerId, c->nextSnackId,
            c->clockOffsetSec);

    fputs("[PCS]\n# id|category(1=standard,2=vip)|status(0=free,1=occupied,2=reserved)|customerId\n", fp);
    for (i = 0; i < TOTAL_PCS; i++)
        fprintf(fp, "%d|%d|%d|%d\n", c->pcs[i].id, c->pcs[i].category,
                c->pcs[i].status, c->pcs[i].customerId);

    fputs("\n[CUSTOMERS]\n# id|name|phone|type(1=individual,2=squad)|squadSize|pcId|inSession|startTime|plannedMin|lastDurationMin|sessionCount|start(readable)\n", fp);
    for (i = 0; i < c->customerCount; i++) {
        const Customer *cu = &c->customers[i];
        readableTime(cu->startTime, t1, sizeof(t1));
        fprintf(fp, "%d|%s|%s|%d|%d|%d|%d|%.0f|%d|%d|%d|%s\n", cu->id, cu->name, cu->phone,
                cu->type, cu->squadSize, cu->pcId, cu->inSession, (double)cu->startTime,
                cu->plannedMin, cu->durationMin, cu->sessionCount, t1);
    }

    fputs("\n[RESERVATIONS]\n# id|customerId|pcId|category|createdAt|scheduledAt|status(0=pending,1=completed,2=cancelled,3=expired)|scheduled(readable)\n", fp);
    for (i = 0; i < c->reservationCount; i++) {
        const Reservation *r = &c->reservations[i];
        readableTime(r->scheduledAt, t1, sizeof(t1));
        fprintf(fp, "%d|%d|%d|%d|%.0f|%.0f|%d|%s\n", r->id, r->customerId, r->pcId,
                r->category, (double)r->createdAt, (double)r->scheduledAt, r->status, t1);
    }

    fputs("\n[WAITINGLIST]\n# queueNo|customerId|category|joinedAt|joined(readable)\n", fp);
    for (i = 0; i < c->queueCount; i++) {
        readableTime(c->queue[i].joinedAt, t1, sizeof(t1));
        fprintf(fp, "%d|%d|%d|%.0f|%s\n", c->queue[i].queueNo, c->queue[i].customerId,
                c->queue[i].category, (double)c->queue[i].joinedAt, t1);
    }

    fputs("\n[SNACKS]\n# id|name|price|stockQuantity|lowStockThreshold\n", fp);
    for (i = 0; i < c->snackCount; i++)
        fprintf(fp, "%d|%s|%.2f|%d|%d\n", c->snacks[i].id, c->snacks[i].name,
                c->snacks[i].price, c->snacks[i].quantity, c->snacks[i].threshold);

    fputs("\n[ORDERS]\n# orderId|customerId|snackId|snackName|qty|unitPrice|lineTotal|orderedAt|billed(0/1)|ordered(readable)\n", fp);
    for (i = 0; i < c->orderCount; i++) {
        const OrderRecord *o = &c->orders[i];
        readableTime(o->orderedAt, t1, sizeof(t1));
        fprintf(fp, "%d|%d|%d|%s|%d|%.2f|%.2f|%.0f|%d|%s\n", o->orderId, o->customerId,
                o->snackId, o->snackName, o->quantity, o->unitPrice, o->lineTotal,
                (double)o->orderedAt, o->billed, t1);
    }

    fputs("\n[BILLS]\n# id|customerId|name|pcId|category|start|end|minutes|peakMinutes|gamingCharge|snackCharge|discountPct|discountAmount|grandTotal|payMethod|start(readable)|end(readable)\n", fp);
    for (i = 0; i < c->billCount; i++) {
        const Bill *b = &c->bills[i];
        readableTime(b->startTime, t1, sizeof(t1));
        readableTime(b->endTime, t2, sizeof(t2));
        fprintf(fp, "%d|%d|%s|%d|%d|%.0f|%.0f|%d|%d|%.2f|%.2f|%d|%.2f|%.2f|%d|%s|%s\n", b->id,
                b->customerId, b->customerName, b->pcId, b->category, (double)b->startTime,
                (double)b->endTime, b->minutes, b->peakMinutes, b->usageCharge,
                b->snackCharge, b->discountPct, b->discountAmount, b->grandTotal,
                b->payMethod, t1, t2);
    }

    fputs("\n[PAYMENTS]\n# id|billId|customerId|method(1=cash,2=card,3=mobile)|amount|paidAt|paid(readable)\n", fp);
    for (i = 0; i < c->paymentCount; i++) {
        const Payment *p = &c->payments[i];
        readableTime(p->paidAt, t1, sizeof(t1));
        fprintf(fp, "%d|%d|%d|%d|%.2f|%.0f|%s\n", p->id, p->billId, p->customerId,
                p->method, p->amount, (double)p->paidAt, t1);
    }
    fputs("\n[END]\n", fp);
    fclose(fp);

    remove(RECORDS_FILE);                          /* replace the old file */
    if (rename(RECORDS_TMP, RECORDS_FILE) != 0) {
        printf("  [File error] Could not replace %s (your data is in %s)\n",
               RECORDS_FILE, RECORDS_TMP);
        return 0;
    }
    return 1;
}

/* ---------- read the file back ---------- */

enum Section { SEC_NONE, SEC_COUNTERS, SEC_PCS, SEC_CUSTOMERS, SEC_RESERVATIONS,
               SEC_QUEUE, SEC_SNACKS, SEC_ORDERS, SEC_BILLS, SEC_PAYMENTS };

static int sectionFromLine(const char *line)
{
    if (strncmp(line, "[COUNTERS]", 10) == 0) return SEC_COUNTERS;
    if (strncmp(line, "[PCS]", 5) == 0) return SEC_PCS;
    if (strncmp(line, "[CUSTOMERS]", 11) == 0) return SEC_CUSTOMERS;
    if (strncmp(line, "[RESERVATIONS]", 14) == 0) return SEC_RESERVATIONS;
    if (strncmp(line, "[WAITINGLIST]", 13) == 0) return SEC_QUEUE;
    if (strncmp(line, "[SNACKS]", 8) == 0) return SEC_SNACKS;
    if (strncmp(line, "[ORDERS]", 8) == 0) return SEC_ORDERS;
    if (strncmp(line, "[BILLS]", 7) == 0) return SEC_BILLS;
    if (strncmp(line, "[PAYMENTS]", 10) == 0) return SEC_PAYMENTS;
    return SEC_NONE;
}

/* Splits ONE data line with sscanf and stores it in the right array.
 * Returns 1 if it was a PC line (so the caller can count the PCs). */
static int parseRecordLine(Cafe *c, int section, const char *line, int pcsLoaded)
{
    double t1, t2;
    int isPc = 0;

    if (section == SEC_COUNTERS) {
        sscanf(line, "%d|%d|%d|%ld", &c->nextQueueNo, &c->nextCustomerId,
               &c->nextSnackId, &c->clockOffsetSec);
    } else if (section == SEC_PCS) {
        if (pcsLoaded < TOTAL_PCS) {
            PC *p = &c->pcs[pcsLoaded];
            if (sscanf(line, "%d|%d|%d|%d", &p->id, &p->category, &p->status,
                       &p->customerId) == 4) isPc = 1;
        }
    } else if (section == SEC_CUSTOMERS && c->customerCount < MAX_CUSTOMERS) {
        Customer *cu = &c->customers[c->customerCount];
        memset(cu, 0, sizeof(*cu));
        if (sscanf(line, "%d|%49[^|]|%19[^|]|%d|%d|%d|%d|%lf|%d|%d|%d", &cu->id, cu->name,
                   cu->phone, &cu->type, &cu->squadSize, &cu->pcId, &cu->inSession, &t1,
                   &cu->plannedMin, &cu->durationMin, &cu->sessionCount) == 11) {
            cu->startTime = (time_t)t1;
            c->customerCount++;
        }
    } else if (section == SEC_RESERVATIONS && c->reservationCount < MAX_RESERVATIONS) {
        Reservation *r = &c->reservations[c->reservationCount];
        if (sscanf(line, "%d|%d|%d|%d|%lf|%lf|%d", &r->id, &r->customerId, &r->pcId,
                   &r->category, &t1, &t2, &r->status) == 7) {
            r->createdAt = (time_t)t1;
            r->scheduledAt = (time_t)t2;
            c->reservationCount++;
        }
    } else if (section == SEC_QUEUE && c->queueCount < MAX_QUEUE) {
        WaitEntry *w = &c->queue[c->queueCount];
        if (sscanf(line, "%d|%d|%d|%lf", &w->queueNo, &w->customerId, &w->category, &t1) == 4) {
            w->joinedAt = (time_t)t1;
            c->queueCount++;
        }
    } else if (section == SEC_SNACKS && c->snackCount < MAX_SNACKS) {
        Snack *s = &c->snacks[c->snackCount];
        memset(s, 0, sizeof(*s));
        if (sscanf(line, "%d|%39[^|]|%lf|%d|%d", &s->id, s->name, &s->price,
                   &s->quantity, &s->threshold) == 5)
            c->snackCount++;
    } else if (section == SEC_ORDERS && c->orderCount < MAX_ORDERS) {
        OrderRecord *o = &c->orders[c->orderCount];
        memset(o, 0, sizeof(*o));
        if (sscanf(line, "%d|%d|%d|%39[^|]|%d|%lf|%lf|%lf|%d", &o->orderId, &o->customerId,
                   &o->snackId, o->snackName, &o->quantity, &o->unitPrice, &o->lineTotal,
                   &t1, &o->billed) == 9) {
            o->orderedAt = (time_t)t1;
            c->orderCount++;
        }
    } else if (section == SEC_BILLS && c->billCount < MAX_BILLS) {
        Bill *b = &c->bills[c->billCount];
        memset(b, 0, sizeof(*b));
        if (sscanf(line, "%d|%d|%49[^|]|%d|%d|%lf|%lf|%d|%d|%lf|%lf|%d|%lf|%lf|%d", &b->id,
                   &b->customerId, b->customerName, &b->pcId, &b->category, &t1, &t2,
                   &b->minutes, &b->peakMinutes, &b->usageCharge, &b->snackCharge,
                   &b->discountPct, &b->discountAmount, &b->grandTotal, &b->payMethod) == 15) {
            b->startTime = (time_t)t1;
            b->endTime = (time_t)t2;
            c->billCount++;
        }
    } else if (section == SEC_PAYMENTS && c->paymentCount < MAX_PAYMENTS) {
        Payment *p = &c->payments[c->paymentCount];
        if (sscanf(line, "%d|%d|%d|%d|%lf|%lf", &p->id, &p->billId, &p->customerId,
                   &p->method, &p->amount, &t1) == 6) {
            p->paidAt = (time_t)t1;
            c->paymentCount++;
        }
    }
    return isPc;
}

/* Loads cafe_records.txt into memory. Returns how many PC lines were found
 * (0 if the file is missing or empty - the program then starts with defaults). */
int loadFromFile(Cafe *c)
{
    FILE *fp = fopen(RECORDS_FILE, "r");
    char line[512];
    int section = SEC_NONE, pcsLoaded = 0;

    if (fp == NULL) return 0;
    while (fgets(line, (int)sizeof(line), fp) != NULL) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r' || line[0] == '\0')
            continue;                               /* heading or blank line */
        if (line[0] == '[') { section = sectionFromLine(line); continue; }
        pcsLoaded += parseRecordLine(c, section, line, pcsLoaded);
    }
    fclose(fp);
    return pcsLoaded;
}

/* Prints the whole records file on the screen (for staff/admin to check). */
void viewRecordsFile(void)
{
    FILE *fp = fopen(RECORDS_FILE, "r");
    char line[512];
    if (fp == NULL) { printf("  %s does not exist yet.\n", RECORDS_FILE); return; }
    printf("\n======== CONTENT OF %s ========\n", RECORDS_FILE);
    while (fgets(line, (int)sizeof(line), fp) != NULL) fputs(line, stdout);
    fclose(fp);
    printf("======== END OF FILE ========\n");
}
