#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "Types.h"

int findCustomerIndex(const Cafe *c, int id);
int askCustomerIndex(const Cafe *c);
void printCustomer(const Customer *cu);
void showAllCustomers(const Cafe *c);
void registerCustomer(Cafe *c);
void updateCustomer(Cafe *c);
void searchCustomer(const Cafe *c);
void deleteCustomer(Cafe *c);

#endif