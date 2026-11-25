/* storage.h - FILE HANDLING. Every record is saved in ONE text file: cafe_records.txt. saveAll() writes it after every change, loadFromFile() reads it at start-up. */
#ifndef STORAGE_H
#define STORAGE_H

#include "config.h"

void initDefaultPcs(Cafe *c);
void seedSnacks(Cafe *c);
int saveAll(const Cafe *c);
int loadFromFile(Cafe *c);
void viewRecordsFile(void);

#endif
