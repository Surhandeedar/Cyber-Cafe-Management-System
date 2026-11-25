/* session.h - Sessions: start a session on a chosen PC, end a session (bill, payment, release PC, serve the queue). */
#ifndef SESSION_H
#define SESSION_H

#include "config.h"

void startSessionFlow(Cafe *c);
void endSessionFlow(Cafe *c);

#endif
