/* snack.h - Snacks and inventory: menu, multi-item orders, stock decrease, low-stock alert, add stock, add item, DELETE item. */
#ifndef SNACK_H
#define SNACK_H

#include "config.h"

void showSnackMenu(const Cafe *c);
int showLowStock(const Cafe *c);
void orderSnackFlow(Cafe *c);
void addStockFlow(Cafe *c);
void addNewSnackFlow(Cafe *c);
void deleteSnackFlow(Cafe *c);

#endif
