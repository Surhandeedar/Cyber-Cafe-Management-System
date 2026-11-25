/* search.h - Searching: find a PC, customer (binary search), reservation, queue entry or snack in the in-memory arrays. */
#ifndef SEARCH_H
#define SEARCH_H

#include "config.h"

int findPcIndex(const Cafe *c, int pcId);
int findFreePc(const Cafe *c, int category);
int findCustomerIndex(const Cafe *c, int id);
int findPendingReservationByCustomer(const Cafe *c, int customerId);
int findPendingReservationByPc(const Cafe *c, int pcId);
int findQueueIndexByCustomer(const Cafe *c, int customerId);
int findSnackIndex(const Cafe *c, int snackId);

#endif
