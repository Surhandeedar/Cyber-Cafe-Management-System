#ifedef PC_H
#define PC_H

#include "Types.h"

const char *categoryName(int cat);
const char *statusName(int s);
int findPcindex(const Cafe *c, int pcId);
int findFreePc(const Cafe *c, int category);
void showPcMap(const Cafe *c);
int listPcsByStatus(const Cafe *c, int status, int category);

#endif