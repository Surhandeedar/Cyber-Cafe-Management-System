/*
 * main.c - CYBER CAFE AND GAMING ARENA MANAGEMENT SYSTEM (program entry point)
 * All records are saved automatically in the text file  cafe_records.txt
 */
#include "config.h"
#include "input.h"
#include "storage.h"
#include "startup.h"
#include "reservation.h"
#include "menus.h"

int main(void)
{
    static Cafe cafe;       /* static: keeps the large structure off the stack */
    int choice;

    loadAll(&cafe);
    do {
        expireReservations(&cafe);
        printf("\n==============================================\n");
        printf("   CYBER CAFE & GAMING ARENA MANAGEMENT SYSTEM\n");
        printf("==============================================\n");
        printf(" 1. Admin Login\n 2. Customer Menu\n 3. Staff Menu\n 4. Exit\n");
        choice = readIntInRange("Choice: ", 1, 4);
        switch (choice) {
            case 1: adminLogin(&cafe); break;
            case 2: customerMenu(&cafe); break;
            case 3: staffMenu(&cafe); break;
            default: break;
        }
    } while (choice != 4);
    saveAll(&cafe);
    printf("All data saved in %s. Thank you for using the system!\n", RECORDS_FILE);
    return 0;
}
