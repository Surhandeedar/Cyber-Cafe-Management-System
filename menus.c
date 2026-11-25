/*
 * menus.c
 * Menus for Staff, Customer and Admin, plus admin login.
 */
#include "config.h"
#include "menus.h"
#include "input.h"
#include "storage.h"
#include "pc.h"
#include "customer.h"
#include "snack.h"
#include "waitlist.h"
#include "reservation.h"
#include "session.h"
#include "report.h"

void staffMenu(Cafe *c)
{
    int choice;
    do {
        expireReservations(c);
        printf("\n=========== STAFF MENU ===========\n");
        printf(" 1. View available PCs\n 2. View occupied PCs\n 3. PC availability map\n");
        printf(" 4. Register customer\n 5. Assign a PC\n 6. Release PC / end session & bill\n");
        printf(" 7. Display all customers\n 8. Search customer by ID\n 9. View reservations\n");
        printf("10. View waiting list\n11. Add snack stock\n12. Snack menu & low-stock\n");
        printf("13. View saved records file\n 0. Back\n");
        choice = readIntInRange("Choice: ", 0, 13);
        switch (choice) {
            case 1: printf("\nAvailable PCs:\n"); listPcsByStatus(c, PC_AVAILABLE, 0); break;
            case 2: printf("\nOccupied PCs:\n"); listPcsByStatus(c, PC_OCCUPIED, 0); break;
            case 3: showPcMap(c); break;
            case 4: registerCustomer(c); break;
            case 5: startSessionFlow(c); break;
            case 6: endSessionFlow(c); break;
            case 7: showAllCustomers(c); break;
            case 8: searchCustomer(c); break;
            case 9: showReservations(c); break;
            case 10: showWaitingList(c); break;
            case 11: addStockFlow(c); break;
            case 12: showSnackMenu(c); showLowStock(c); break;
            case 13: viewRecordsFile(); break;
            default: break;
        }
    } while (choice != 0);
}

void customerMenu(Cafe *c)
{
    int choice;
    do {
        expireReservations(c);
        printf("\n=========== CUSTOMER MENU ===========\n");
        printf(" 1. Register\n 2. Update my information\n 3. View PCs / availability map\n");
        printf(" 4. Start a session (walk-in)\n 5. Make a reservation\n 6. Check in to my reservation\n");
        printf(" 7. Cancel my reservation\n 8. Order snacks\n 9. Join waiting list\n");
        printf("10. View waiting list\n11. End session & pay\n12. My session history\n 0. Back\n");
        choice = readIntInRange("Choice: ", 0, 12);
        switch (choice) {
            case 1: registerCustomer(c); break;
            case 2: updateCustomer(c); break;
            case 3: showPcMap(c); break;
            case 4: startSessionFlow(c); break;
            case 5: createReservation(c); break;
            case 6: checkInReservation(c); break;
            case 7: cancelReservation(c); break;
            case 8: orderSnackFlow(c); break;
            case 9: joinWaitingListFlow(c); break;
            case 10: showWaitingList(c); break;
            case 11: endSessionFlow(c); break;
            case 12: historyForCustomerFlow(c); break;
            default: break;
        }
    } while (choice != 0);
}

static void adminMenu(Cafe *c)
{
    int choice;
    printf("\nLogin successful.\n");
    if (showLowStock(c) > 0) printf("(Please restock the items above.)\n");
    do {
        expireReservations(c);
        printf("\n=========== ADMIN MENU ===========\n");
        printf(" 1. Dashboard / statistics\n 2. Daily sales records\n 3. Snack sales records\n");
        printf(" 4. Low-stock snacks\n 5. Session history (all customers)\n 6. Customer session history\n");
        printf(" 7. Reservations report\n 8. Add new snack item\n 9. Add snack stock\n");
        printf("10. Display all customers\n11. Update customer\n12. Delete customer\n");
        printf("13. Payment records\n14. Advance cafe clock (testing)\n15. Delete snack item from menu\n");
        printf("16. View saved records file\n 0. Logout\n");
        choice = readIntInRange("Choice: ", 0, 16);
        switch (choice) {
            case 1: showDashboard(c); break;
            case 2: showDailySales(c); break;
            case 3: showSnackSales(c); break;
            case 4: if (showLowStock(c) == 0) printf("  All snacks are sufficiently stocked.\n"); break;
            case 5: showSessionHistory(c, 0); break;
            case 6: historyForCustomerFlow(c); break;
            case 7: showReservations(c); break;
            case 8: addNewSnackFlow(c); break;
            case 9: addStockFlow(c); break;
            case 10: showAllCustomers(c); break;
            case 11: updateCustomer(c); break;
            case 12: deleteCustomer(c); break;
            case 13: showPayments(c); break;
            case 14: advanceClock(c); break;
            case 15: deleteSnackFlow(c); break;
            case 16: viewRecordsFile(); break;
            default: break;
        }
    } while (choice != 0);
}

void adminLogin(Cafe *c)
{
    char user[40], pass[40];
    int attempt;
    for (attempt = 1; attempt <= MAX_LOGIN_ATTEMPTS; attempt++) {
        readLine("Username: ", user, (int)sizeof(user));
        readLine("Password: ", pass, (int)sizeof(pass));
        if (strcmp(user, ADMIN_USER) == 0 && strcmp(pass, ADMIN_PASS) == 0) {
            adminMenu(c);
            return;
        }
        printf("  Wrong credentials (%d/%d).\n", attempt, MAX_LOGIN_ATTEMPTS);
    }
    printf("  Too many failed attempts. Returning to main menu.\n");
}
