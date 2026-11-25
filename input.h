/* input.h - Console input helpers: validated reading of numbers, names, phone numbers, yes/no. */
#ifndef INPUT_H
#define INPUT_H

#include "config.h"

void readLine(const char *prompt, char *buf, int size);
int readIntInRange(const char *prompt, int min, int max);
int readYesNo(const char *prompt);
void readValidName(const char *prompt, char *buf, int size);
void readValidPhone(char *buf, int size);
int compareIgnoreCase(const char *a, const char *b);
void printLine(void);

#endif
