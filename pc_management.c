#include <stdio.h>
#include "pc_management.h"

int standard_status[20] = {0};
int vip_status[5] = {0};

void displayPCAvailablility() {
    int category;
    printf("\n===== PC AVAILABLILITY =====\N");
    printf("1. Standard Hall\n");
    printf("2. VIP Room\n");
    printf("Enter here(1 to 2): ");
    scanf("%d", &category);

    if (category == 1) {
        printf("\n===== STANDARD HALL =====\n");
        for (int i =0; i<20; i++) {
            if (standard_status[i] == 0) {
                printf("PC %d: Free\t", i+1);
                }
            else {
                printf("PC %d: Busy\t", i+1);
            }
            if ((i + 1) % 5 == 0) {
                printf("\n");
            }
        }
    }
    else if (category == 2)
    {
        printf("\n===== VIP ROOM =====\n");
        for (int i =0; i<5; i++) {
            if (vip_status[i] == 0) {
                printf("VIP PC %d: Free\t", i+1);
                }
            else {
                printf("VIP PC %d: Busy\t", i+1);
            }
            printf("\n");
        }
        printf("Invalid category!\n");
    }
}

void bookPC() {
    int category, pc_choice;
    printf("\n===== BOOK A PC =====\n");
    printf("1. Standard Hall\n");
    printf("2. VIP Room\n");
    printf("Enter here(1 to 2): ");
    scanf("%d", &category);

    if (category == 1) {
        printf("\n===== STANDARD PC AVAILABILITY =====\n");
        for (int i =0; i< 20; i++) {
            printf("PC %d: %s\t", i+1, standard_status[i] == 0 ? "Free" : "Busy");
            if ((i+1) % 5 == 0)
            printf("\n");
        }
        printf("\nEnter Standard PC number (1-20): ");
        scanf("%d", &pc_choice);

        while(pc_choice < 1 || pc_choice > 20) {
            printf("Error!!! Invalid PC number. Enter again(1 to 20): ");
                scanf("%d", &pc_choice);
            }
        if (standard_status[pc_choice -1] == 0) {
            printf("PC %d is booked successfully\n", pc_choice);
            standard_status[pc_choice -1] =1;
            }
        else {
            printf("PC %d is already booked\n", pc_choice);
            }    
    }

    else if (category == 2) {
        printf("\n===== VIP PC AVAILABLILITY =====\n");
        for (int i =0; i< 5; i++) {
            printf("VIP PC %d: %s\t", i+1, vip_status[i] == 0 ? "Free" : "Busy");
        }
        printf("\nEnter VIP PC number (1-5): ");
        scanf("%d", &pc_choice);

        while(pc_choice < 1 || pc_choice > 5) {
            printf("Error!!! Invalid PC number. Enter again(1 to 5): ");
                scanf("%d", &pc_choice);
        }
        if (vip_status[pc_choice -1] == 0) {
            printf("VIP PC %d is booked successfully\n", pc_choice);
            vip_status[pc_choice -1] =1;
        }
        else {
            printf("VIP PC %d is already booked\n", pc_choice);
        }    
    }
    else {
        printf("Invalid category!\n");
    }
}

void assignPC() {
    int category, pc_choice;
    printf("\n===== ASSIGN A PC =====\n");
    printf("1. Standard Hall\n");
    printf("2. VIP Room\n");
    printf("Enter here(1 to 2): ");
    scanf("%d", &category);

    if (category == 1) {
        printf("\n===== STANDARD PC AVAILABILITY =====\n");
        for (int i =0; i< 20; i++) {
            printf("PC %d: %s\t", i+1, standard_status[i] == 0 ? "Free" : "Busy");
            if ((i+1) % 5 == 0)
            printf("\n");
        }
        printf("\nEnter Standard PC number (1-20): ");
        scanf("%d", &pc_choice);

        while(pc_choice < 1 || pc_choice > 20) {
            printf("Error!!! Invalid PC number. Enter again(1 to 20): ");
                scanf("%d", &pc_choice);
            }
        if (standard_status[pc_choice -1] == 0) {
            printf("PC %d is booked successfully\n", pc_choice);
            standard_status[pc_choice -1] =1;
            }
        else {
            printf("PC %d is already booked\n", pc_choice);
            }    
    }

    else if (category == 2) {
        printf("\n===== VIP PC AVAILABLILITY =====\n");
        for (int i =0; i< 5; i++) {
            printf("VIP PC %d: %s\t", i+1, vip_status[i] == 0 ? "Free" : "Busy");
        }
        printf("\nEnter VIP PC number (1-5): ");
        scanf("%d", &pc_choice);

        while(pc_choice < 1 || pc_choice > 5) {
            printf("Error!!! Invalid PC number. Enter again(1 to 5): ");
                scanf("%d", &pc_choice);
        }
        if (vip_status[pc_choice -1] == 0) {
            printf("VIP PC %d is booked successfully\n", pc_choice);
            vip_status[pc_choice -1] =1;
        }
        else {
            printf("VIP PC %d is already booked\n", pc_choice);
        }    
    }
    else {
        printf("Invalid category!\n");
    }
}