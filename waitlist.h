/* waitlist.h - Waiting list (FIFO queue): queue numbers, duplicate check, estimated wait, automatic PC assignment. */
#ifndef WAITLIST_H
#define WAITLIST_H

#include "config.h"

void processWaitingList(Cafe *c);
int addToQueue(Cafe *c, int custIdx, int category);
void showWaitingList(const Cafe *c);
void joinWaitingListFlow(Cafe *c);

#endif
