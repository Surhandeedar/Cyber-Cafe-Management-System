/*
 * pc.c
 * PC / gaming station management: names, availability lists, text PC map, occupy a PC.
 */
#include "config.h"
#include "pc.h"
#include "timeutil.h"
#include "storage.h"

const char *categoryName(int cat)
{ return cat == CAT_VIP ? "VIP Room" : "Standard Hall"; }

static const char *statusName(int s)
{
    switch (s) {
        case PC_AVAILABLE: return "Available";
        case PC_OCCUPIED:  return "Occupied";
        case PC_RESERVED:  return "Reserved";
    }
    return "Unknown";
}

void showPcMap(const Cafe *c)
{
    int i;
    printf("\n========== PC AVAILABILITY MAP ==========\n");
    printf("F = Free   X = Occupied   R = Reserved\n");
    printf("\n--- Standard Hall ---\n");
    for (i = 0; i < STD_PC_COUNT; i++) {
        printf("[%c] PC%-3d  ", c->pcs[i].status == PC_AVAILABLE ? 'F' :
               (c->pcs[i].status == PC_OCCUPIED ? 'X' : 'R'), c->pcs[i].id);
        if ((i + 1) % 5 == 0) printf("\n");
    }
    printf("\n--- VIP Gaming Rooms ---\n");
    for (i = STD_PC_COUNT; i < TOTAL_PCS; i++) {
        printf("[%c] PC%-3d  ", c->pcs[i].status == PC_AVAILABLE ? 'F' :
               (c->pcs[i].status == PC_OCCUPIED ? 'X' : 'R'), c->pcs[i].id);
    }
    printf("\n\n");
}

int listPcsByStatus(const Cafe *c, int status, int category)
{
    int i, shown = 0;
    for (i = 0; i < TOTAL_PCS; i++) {
        if (c->pcs[i].status != status) continue;
        if (category != 0 && c->pcs[i].category != category) continue;
        if (c->pcs[i].status == PC_OCCUPIED)
            printf("  PC %-3d | %-13s | in use by customer %d\n", c->pcs[i].id,
                   categoryName(c->pcs[i].category), c->pcs[i].customerId);
        else
            printf("  PC %-3d | %-13s | %s\n", c->pcs[i].id,
                   categoryName(c->pcs[i].category), statusName(c->pcs[i].status));
        shown++;
    }
    if (shown == 0) printf("  (none)\n");
    return shown;
}

void beginSession(Cafe *c, int custIdx, int pcIdx, int plannedMin)
{
    Customer *cu = &c->customers[custIdx];
    PC *pc = &c->pcs[pcIdx];
    pc->status = PC_OCCUPIED;
    pc->customerId = cu->id;
    cu->pcId = pc->id;
    cu->startTime = nowTime(c);
    cu->plannedMin = plannedMin;
    cu->inSession = 1;
    saveAll(c);
}
