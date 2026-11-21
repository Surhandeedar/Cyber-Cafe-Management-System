#ifndef RESERVATION_H
#define RESERVATION_H

#include "Types.h"

const char *resStatusName(int s);
int findPendingReservationByCustomer(const Cafe *c, int customerId);
int findPendingReservationByPc(const Cafe *c, int pcId);

void expireReservations(Cafe *c);
void createReservation(Cafe *c);
void checkInReservation(Cafe *c);
void cancelReservation(Cafe *c);
void showReservations(const Cafe *c);

#endif