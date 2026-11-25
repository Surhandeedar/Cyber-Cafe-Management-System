/* customer.h - Customer management: register, search, update, delete, list with sorting. */
#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "config.h"

int askCustomerIndex(const Cafe *c);
void showAllCustomers(const Cafe *c);
void registerCustomer(Cafe *c);
void updateCustomer(Cafe *c);
void searchCustomer(const Cafe *c);
void deleteCustomer(Cafe *c);

#endif
