#include "Types.h"
#include "Customer.h"
#include "Input.h"
#include "Reservations.h"
#include "Storage.h"
#include "TimeUtil.h"
#include "WaitList.h"

int findCustomerIndex(const Cafe *c, int id) {
    int lo = 0, hi = c->customerCount - 1, mid;
    while (lo <= hi) {
        mid = (lo + hi) / 2;
        if (c->customers[mid].id == id) return mid;
        if (c->customers[mid].id < id) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

int askCustomerInder(const Cafe *c) {
    int id = readIntInRange("Enter Customer ID (0 to cancel): ", 0, 999999);
    int idx;
    if (id == 0) return -1;
    idx = findCustomerIndex(c, id);
    if (idx < 0) printf(" Customer ID %d not found.\n", id);
    return idx;
}

void printCustomer(const Customer *cu) {
    char t[32];
    printf(" ID %-5d | %-20s | %-15s | %-10s", cu->id, cu->name, cu->phone, 
            cu->type == CUST_SQUAD ? "Squad" : "Individual");
    if (cu->type == CUST_SQUAD) printf("(%d members)", cu->squadSize);
    printf("\n");
    if (cu->inSession) {
        formatTime(cu->startTime, t, sizeof(t), "%d-%m-%Y %H:%M");
        printf("            In session on PC %d since %s (planned %d min)\n", cu->pcId, t, cu->plannedMin);
    } 
    else {
        printf("            Not in session | Last session %d min | sessions completed: %d\n", cu->durationMin, cu->sessionCount);
    }
}

void showAllCustomers(const Cafe *c) {
    int order[MAX_CUSTOMERS];
    int i, j, tmp, byName;
    if (c->customerCount == 0)   {  printf("  No customers registered yet.\n"); return; }
    byName = readIntInRange("Sort by 1) ID 2) Name : ", 1, 2) == 2;
    
    for (i = 0; i < c->customerCount; i++)  order[i] = i;
    if (byName) {
        for (i = 0; i < c-> customerCount - 1; i++) {
            int m = i;
            for (j = i + 1; j < c->customerCount; j++)
                if (compareIgnoreCase(c->customers[order[j]].name, c->customers[order[m]].name) < 0) m = j;
            temp = order[i]; 
            order[i] = order[m];
            order[m] = temp;
        }
    }
    printf("\n ====== REGISTERED CUSTOMERS (%d) ======\n", c->customerCount);
    for (i = 0; i < c->customerCount; i++) 
        printCustomer(&c->customers[order[i]] );
}

void registerCustomer(Cafe *c) {
    Customer cu;
    if (c->customerCount >= MAX_CUSTOMERS) {
        printf("  Customer storage is full.\n");
        return;
    }
    memset(&cu, 0, sizeof(cu));
    readValidName("Customer / squad name: ", cu.name, NAME_LEN);
    readValidPhone(cu.phone, PHONE_LEN);
    cu.type = readIntInRange("Type 1) Individual 2) Squad : ", 1, 2);
    cu.squadSize = 1;
    if (cu.type == CUST_SQUAD)
        cu.squadSize = readIntInRange("Number of squad members (2-20): ", 2, 20);
    if (findCustomerIndex(c, c->nextCustomerId) >= 0) {
        c->nextCustomerId = c->customers[c->customerCount - 1].id + 1;
    }
    cu.id = c->nextCustomerId++;
    c->customers[c->customerCount++] = cu;
    appendRecord(FILE_CUSTOMERS, &cu, sizeof(Customer));
    saveCounters(c);
    printf("  Registered! Your Customer ID is %d\n", cu.id);
    if (cu.type == CUST_SQUAD && cu.squadSize >= SQUAD_MIN_MEMBERS)
       printf("  Squad discount of %d%% will be applied to gaming charges.\n", SQUAD_DISCOUNT_PCT);
}

void updateCustomer(Cafe *c) {
    int idx = askCustomerIndex(c), choice;
    Customer *cu;
    if (idx < 0) return;
    cu = &c->customers[idx];
    printCustomer(cu);
    choice = readIntInRange("Update 1) Name 2) Phone 3) Type/Size 0) Cancel : ", 0, 3);
    if(choice == 0) return;
    if (choice == 1) readValidName("New name: ", cu->name, NAME_LEN);
    else if (choice == 2) readValidPhone(cu->phone, PHONE_LEN);
    else {
        cu->type = readIntInRange("Type 1) Individual 2) Squad : ", 1, 2);
        cu->squadSize = 1;
        if (cu->type == CUST_SQUAD)
            cu->squadSize = readIntInRange("Number of squad members (2-20): ", 2, 20);
    }
    saveCustomerAt(c, idx);
    printf("  Customer record updated.\n");
}

void deleteCustomer (Cafe *c) {
    int idx = askCustomerInder(c), i;
    if (idx < 0) return;
    if (c->customers[idx].insession ||
        findPendingReservationByCustomer(c, c->customers[idx].id) >= 0 ||
        findQueueIndexyCustomer(c, c->customers[idx].id) >= 0)  {
            printf("  Cannot delete: customer has an active session, reservation, or queue entry.\n");
            return;
        }
        printfCustomer(&c->customers[idx]);
        if (!readYesNo("Delete this Customer record permanently? (y/n): ")) return;

        for (i = idx; i < c->customerCount - 1; i++) 
        c->customer[i] = c->customers[i + 1];
        c->customerCount--;
        persistCustomers(c);
        printf("  Customer deleted (billing histiry is kept).\n";)
}