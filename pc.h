/* pc.h - PC / gaming station management: names, availability lists, text PC map, occupy a PC. */
#ifndef PC_H
#define PC_H

#include "config.h"

const char *categoryName(int cat);
void showPcMap(const Cafe *c);
int listPcsByStatus(const Cafe *c, int status, int category);
void beginSession(Cafe *c, int custIdx, int pcIdx, int plannedMin);

#endif
