#include <stdio.h>
int main() {
    int choice= -1;
    int staff_choice;
    int cust_choice;
    int pc_status[10] = {0,0,0,0,0,0,0,0,0,0};   
    int pc_choice[10];

    while(choice != 0) { 

    printf("Enter your category (0 to stop):\n");
    printf("1. Staff\n2. Customer\n");
    printf("Enter you choice(1 0r 2): ");
    scanf("%d", &choice);

    switch (choice) 
    {

    case 0:
        // Exit
        printf("Thank you for using the system!\n");
        printf("Shuting down the system!");
        break;

    case 1:
        // Staff
        printf("Choose your next step:\n ");
        printf("1. Check the availability of PC\n2. Assign a PC\n3. Salary\n4. Request for Leave\n5. File a Complain\n6. Give Review/Comments\n");
        printf("Enter here(1 to 5): ");
        scanf("%d", &staff_choice);

        switch (staff_choice)
        {
            case 1:
            // Check the availability of PC
            for (int i =0; i < 10; i++) {
                if (pc_status[i] == 0) {
                    printf("PC %d: Free\n", i+1);
                }
                else {
                    printf("PC %d: Busy\n", i+1);
                }
            }
            break;

            case 2:
            // assign a PC
            printf("Enter the PC number to assign(1 to 10): ");
            scanf("%d", &pc_choice);
            
            if (pc_status[pc_choice[10] -1] == 0) {
                printf("PC %d is assigned successfully\n", pc_choice -1);
                pc_status[pc_choice[10] -1] =1;
            }
            else {
                printf("PC %d is already booked\n", pc_choice -1);
            }
            break;

        }
        break;
    
    case 2:
        // Customer
        printf("Choose your next step:\n");
        printf("1. View available PCs\n2. Book a PC\n3. Order Snack\n4. File a Complaint\n ");
        printf("Enter here(1 to 4): ");
        scanf("%d", &cust_choice);

        switch (cust_choice) {
            case 1:
            // View available PCs
            for (int i =0; i < 10; i++) {
                if (pc_status[i] == 0) {
                    printf("PC %d: Free", i+1);
                }
                else {
                    printf("PC %d: Busy", i+1);
                }
            }

            break;

            case 2:
            // Book a PC
            printf("Enter the PC number to book(1 to 10): ");
            scanf("%d", &pc_choice);
            if (pc_status[pc_choice[10] -1] == 0) {
                printf("PC %d is assigned successfully\n", pc_choice -1);
                pc_status[pc_choice[10] -1] =1;
            }
            else {
                printf("PC %d is already booked\n", pc_choice -1);
            }    
            break;
        }
        break;

    
    }
    }
    return 0;
}
