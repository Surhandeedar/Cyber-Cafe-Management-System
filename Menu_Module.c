#include "Types.h"
#include "Menu.h"
#include "Customer.h"
#include "Input.h"
#include "Pc.h"
#include "Report.h"
#include "Reservation.h"
#include "Session.h"
#include "Snack.h"
#include "Waitlist.h"

void staffMenu(cafe *c) {
    int choice;
    do {
        expireReservations(c);
        printf("\n======== STAFF MENU ========\n");
        printf(" 01. View available PCs\n     02. View occupied PCs\n       03. PC availability map\n");
        printf(" 04. Register customer\n      05. Assign a PC\n             06. Release PC / end session & bill\n");
        printf(" 07. Display all customers\n  08. Search customer by ID\n   09. View reservations\n");
        printf(" 10. View waiting list\n      11. Add snack stock\n         12. Snack menu & low stock\n     0. Back\n");

        choice = readIntInRange("Choice: ", 0, 12);

        switch (choice) {
            case 1: printf("\nAvailable PCs:\n"); listPcsByStatus(c, PC_AVAILABLE, 0);
            break;
            case 2: printf("\nOccupied Pcs:\n"); listPcsbyStatus(c, PC_OCCUPIED, 0);
            break;
            case 3:  showPcMap(c);                         break;
            case 4:  registerCustomer(c);                  break;
            case 5:  startSessionFlow(c, 1);               break;
            case 6:  endSessionFlow(c);                    break;
            case 7:  showAllCustomers(c);                  break;
            case 8:  searchCustomer(c);                    break;
            case 9:  showReservations(c);                  break;
            case 10: showWaitingList(c);                   break;
            case 11: addStockFlow(c);                      break;
            case 12: showSnackMenu(c);  showLowStock(c);   break; 
            default:                                       break;
        }
    } while (choice != 0);
}
void customerMenu(Cafe *c) {
    int choice;
    do {
        expireReservations(c);
        printf("\n======== CUSTOMER MENU ========\n");
        printf(" 01. Register\n                     02. Update my information\n   03. View PCs / availability map\n");
        printf(" 04. Start a session(walk-in)\n     05. Make a reservation\n      06. Check in to my reservation\n");
        printf(" 07. Cancel my reservation\n        08. Order snacks\n            09. Join waiting list\n");
        printf(" 10. View waiting list\n            11. End session & pay\n       12. My session history\n        0. Back\n");
        choice = readIntInRange("Choice: ", 0, 12);

        switch (choice) {
            case 1:  registerCustomer(c);          break;
            case 2:  updateCustomer(c);            break;
            case 3:  showPcMap(c);                 break;
            case 4:  startSessionFlow(c, 0);       break;
            case 5:  createReservation(c);         break;
            case 6:  checkInReservation(c);        break;
            case 7:  cancelReservation(c);         break;
            case 8:  orderSnackFlow(c);            break;
            case 9:  joinWaitingListFlow(c);       break;
            case 10: showWaitingList(c);           break;
            case 11: endSessionFlow(c);            break;
            case 12: historyForCustomerFlow(c);    break;
            default:                               break;
        }
    } while (choice != 0);
}
void adminMenu(Cafe *c) {
    int choice;
    printf("\nLogin Successful\n");
    if (showLowStock(c) > 0) 
        printf("(Please restock the items above.)\n");
    do {
        expireReservations(c);
        printf("\n========= ADMIN MENU ========\n");
        printf(" 01. Dashboard / statistics\n    02. Daily sales records\n               03. Snack sales records\n");
        printf(" 04. Low-stock snacks\n          05. Session history(all customers)\n    06. Customer session history\n");
        printf(" 07. Reservations report\n       08. Add new snack item\n                09. Add snack stock\n");
        printf(" 10. Display all customers\n     11. Update Customer\n                   12. Delete customer\n");
        printf(" 13 Payment records\n            14. Advance cafe clock(testing)\n       0. Logout\n");
        choice = readIntInRange("Choice: ", 0, 14);

        switch (choice) {
            case 1:  showDashboard(c);             break;
            case 2:  showDailySales();             break;
            case 3:  showSnackSales();             break;
            case 4:  if (showLowStock(c) == 0) 
                printf(" All snacks are sufficiently stocked.\n");
                                                   break;
            case 5:  showSessionHistory(0);        break;
            case 6:  historyForCustomerFlow(c);    break;
            case 7:  showReservations(c);          break;
            case 8:  addNewSnackFlow(c);           break;
            case 9:  addStockFlow(c);              break;
            case 10: showAllCustomers(c);          break;
            case 11: updateCustomer(c);            break;
            case 12: deleteCustomer(c);            break;
            case 13: showPayments();               break;
            case 14: advanceClock(c);              break;
            default:                               break;
        }
    } while (choice != 0);
}

void adminLogin(Cafe *c) {
    char user[40], pass[40];
    int attempt;
    for (attempt = 1; attempt <= MAX_LOGIN_ATTEMPTS; attempt++) {
        readLine("Username: ", user, (int)sizeof(user) );
        readLine("Password: ", pass, (int)sizeof(pass) );
        if (strcmp(user, ADMIN_USER) == 0 && strcmp(pass, ADMIN_PASS) == 0) {
            adminMenu(c);
            return;
        }
        printf(" wrong credentials (%d/%d).\n", attempt, MAX_LOGIN_ATTEMPTS);
    }
    printf(" Too many failed attempts. Returning to main menu.\n");
}