/* reservation.h - Reservations with real time-based expiry (time.h) and the 15-minute grace period. */
#ifndef RESERVATION_H
#define RESERVATION_H

#include "config.h"

void expireReservations(Cafe *c);
void createReservation(Cafe *c);
void checkInReservation(Cafe *c);
void cancelReservation(Cafe *c);
void showReservations(const Cafe *c);

#endif
