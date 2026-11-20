#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define STD_PC_COUNT          20
#define VIP_PC_COUNT          5
#define TOTAL_PCS             (STD_PC_COUNT + VIP_PC_COUNT)
#define VIP_ID_BASE           100

#define MAX_CUSTOMERS         500
#define MAX_RESERVATIONS      500
#define MAX_QUEUE             100
#define MAX_SNACKS            60
#define MAX_CART              20
#define MAX_DAYS              400
#define NAME_LEN              50
#define SNACK_NAME_LEN        40
#define PHONE_LEN             20
#define FIRST_CUSTOMER_ID     1001

#define ADMIN_USER            "admin"
#define ADMIN_PASS            "fast123"
#define MAX_LOGIN_ATTEMPTS    3

#define GRACE_PERIOD_MIN      15
#define GRACE_PERIOD_SEC      (GRACE_PERIOD_MIN * 60)
#define MAX_RESERVE_LEAD      120
#define DEFAULT_PLAN_MIN      60
#define MAX_PLAN_MIN          720

#define SQUAD_MIN_MEMBERS     5
#define SQUAD_DISCOUNT_PCT    15

#define PEAK_START_HOUR       18
#define PEAK_END_HOUR         23
#define STD_RATE_OFFPEAK      150.0
#define STD_RATE_PEAK         200.0
#define VIP_RATE_OFFPEAK      350.0
#define VIP_RATE_PEAK         450.0

#define SEED_STOCK            30
#define SEED_THRESHOLD        5

#define FILE_CUSTOMERS        "customers.dat"     
#define FILE_PCS              "pcs.dat"
#define FILE_RESERVATIONS     "reservations.dat"
#define FILE_QUEUE            "waitinglist.dat"
#define FILE_SNACKS           "snacks.dat"
#define FILE_ORDERS           "orders.dat"
#define FILE_BILLS            "bills.dat"
#define FILE_PAYMENTS         "payments.txt"
#define FILE_COUNTERS         "counters.txt"

enum PcCategory     {CAT_STANDARD    = 1,  CAT_VIP       = 2 };
enum PcStatus       {PC_AVAILABLE    = 0,  PC_OCCUPIED   = 1,  PC_RESERVED   = 2 };
enum CustomerType   {CUST_INDIVIDUAL = 1,  CUST_SQUAD    = 2 };
enum ResStatus      {RES_PENDING     = 0,  RES_COMPLETED = 1,  RES_CANCELLED = 2,  RES_EXPIRED = 3 };
enum PayMethod      {PAY_CASH        = 1,  PAY_CARD      = 2,  PAY_MOBILE    = 3 };

typedef struct {
    int id;
    int category;
    int status;
    int customerId;
} PC;

typedef struct {
    int    id;
    char   name[NAME_LEN];
    char   phone[PHONE_LEN];
    int    type;
    int    squadSize;
    int    pcId;
    int    inSession;
    time_t startTime;
    int    plannedMin;
    int    durationMin;
    int    sessionCount;
} Customer;

typedef struct {
    int    id;
    int    customerID;
    int    pcId;
    int    category;
    time_t createdAt;
    time_t scheduledAt;
    int    status;
} Reservation;

typedef struct {
    int    queueNo;
    int    customerId;
    int    category;
    time_t joinedAt;
} WaitEntry;

typedef struct {
    int    id;
    char   name[SNACK_NAME_LEN];
    double price;
    int    quantity;
    int    threshold;
} Snack;

typedef struct {
    int    orderId;
    int    customerId;
    int    snackId;
    char   snackName[SNACK_NAME_LEN];
    int    quantity;
    double unitPrice;
    double lineTotal;
    time_t orderedAt;
    int    billed;
} OrderRecord;

typedef struct {
    int    id;
    int    customerId;
    char   customerName[NAME_LEN];
    int    pcId;
    int    category;
    time_t startTime;
    time_t endTime;
    int    minutes;
    int    peakMinutes;
    double usageCharge;
    double snackCharge;
    int    discountPct;
    double discountAmount;
    double grandTotal;
    int    payMethod; 
} Bill;

typedef struct {
    char   date[11];
    int    billCount;
    double gaming;
    double snacks;
    double total;
} DailyRow;

typedef struct {
    PC          pcs[TOTAL_PCS];
    Customer    customers[MAX_CUSTOMERS];
    int         customerCount;
    Reservation reservations[MAX_RESERVATIONS];
    int         reservationCount;
    WaitEntry   queue[MAX_QUEUE];
    int         queueCount;
    Snack       snacks[MAX_SNACKS];
    int         snackCount;
    int         nextQueueNo;
    int         nextCustomerId;
    long        clockOffsetSec;
} Cafe;

#endif