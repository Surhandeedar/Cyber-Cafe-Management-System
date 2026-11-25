/*
 * snack.c
 * Snacks and inventory: menu, multi-item orders, stock decrease, low-stock alert, add stock, add item, DELETE item.
 */
#include "config.h"
#include "snack.h"
#include "input.h"
#include "timeutil.h"
#include "search.h"
#include "storage.h"
#include "customer.h"

void showSnackMenu(const Cafe *c)
{
    int i;
    printf("\n=========== SNACK MENU ===========\n");
    printf("  ID | %-26s | Price(PKR) | Stock\n", "Item");
    printLine();
    for (i = 0; i < c->snackCount; i++)
        printf("  %-3d| %-26s | %9.0f  | %d%s\n", c->snacks[i].id, c->snacks[i].name,
               c->snacks[i].price, c->snacks[i].quantity,
               c->snacks[i].quantity == 0 ? "  (SOLD OUT)" : "");
}

int showLowStock(const Cafe *c)
{
    int i, n = 0;
    for (i = 0; i < c->snackCount; i++) {
        if (c->snacks[i].quantity <= c->snacks[i].threshold) {
            if (n == 0) printf("\n*** LOW-STOCK ALERT ***\n");
            printf("  %-26s stock %d (threshold %d)\n", c->snacks[i].name,
                   c->snacks[i].quantity, c->snacks[i].threshold);
            n++;
        }
    }
    return n;
}

/* Place a snack order for a customer who is in a session. The cost is added
 * to that session's bill, stock decreases, and low stock raises an alert. */
void orderSnackFlow(Cafe *c)
{
    int custIdx, cartSnack[MAX_CART], cartQty[MAX_CART], cartCount = 0;
    int sid, si, qty, i, already, orderId;
    double total = 0.0;
    OrderRecord *o;
    time_t now;

    custIdx = askCustomerIndex(c);
    if (custIdx < 0) return;
    if (!c->customers[custIdx].inSession) {
        printf("  Snacks are added to a running session's bill. Start a session first.\n");
        return;
    }
    showSnackMenu(c);
    for (;;) {
        if (cartCount >= MAX_CART) { printf("  Order is full (%d items).\n", MAX_CART); break; }
        sid = readIntInRange("Snack ID to add (0 = finish): ", 0, 9999);
        if (sid == 0) break;
        si = findSnackIndex(c, sid);
        if (si < 0) { printf("  No snack with that ID.\n"); continue; }
        already = 0;
        for (i = 0; i < cartCount; i++) if (cartSnack[i] == si) already = cartQty[i];
        if (c->snacks[si].quantity - already <= 0) { printf("  Out of stock.\n"); continue; }
        qty = readIntInRange("Quantity: ", 1, 100);
        if (qty > c->snacks[si].quantity - already) {
            printf("  Only %d available. Order not added.\n", c->snacks[si].quantity - already);
            continue;
        }
        for (i = 0; i < cartCount; i++) if (cartSnack[i] == si) break;
        if (i == cartCount) { cartSnack[i] = si; cartQty[i] = 0; cartCount++; }
        cartQty[i] += qty;
        printf("  Added %d x %s\n", qty, c->snacks[si].name);
    }
    if (cartCount == 0) { printf("  Nothing ordered.\n"); return; }

    printf("\n--- ORDER SUMMARY ---\n");
    for (i = 0; i < cartCount; i++) {
        total += cartQty[i] * c->snacks[cartSnack[i]].price;
        printf("  %d x %-26s PKR %.0f\n", cartQty[i], c->snacks[cartSnack[i]].name,
               cartQty[i] * c->snacks[cartSnack[i]].price);
    }
    printf("  ORDER TOTAL: PKR %.0f\n", total);
    if (!readYesNo("Confirm order? (y/n): ")) { printf("  Order cancelled.\n"); return; }
    if (c->orderCount + cartCount > MAX_ORDERS) {
        printf("  Order storage is full.\n");
        return;
    }

    orderId = c->orderCount > 0 ? c->orders[c->orderCount - 1].orderId + 1 : 1;
    now = nowTime(c);
    for (i = 0; i < cartCount; i++) {
        Snack *s = &c->snacks[cartSnack[i]];
        s->quantity -= cartQty[i];                          /* inventory decreases */
        o = &c->orders[c->orderCount++];                    /* sales record */
        memset(o, 0, sizeof(*o));
        o->orderId = orderId;
        o->customerId = c->customers[custIdx].id;
        o->snackId = s->id;
        strncpy(o->snackName, s->name, SNACK_NAME_LEN - 1);
        o->quantity = cartQty[i];
        o->unitPrice = s->price;
        o->lineTotal = cartQty[i] * s->price;
        o->orderedAt = now;
    }
    saveAll(c);
    printf("  Order #%d placed and saved. Added to your session bill.\n", orderId);
    for (i = 0; i < cartCount; i++)
        if (c->snacks[cartSnack[i]].quantity <= c->snacks[cartSnack[i]].threshold)
            printf("  [ADMIN ALERT] Low stock: %s (%d left, threshold %d)\n",
                   c->snacks[cartSnack[i]].name, c->snacks[cartSnack[i]].quantity,
                   c->snacks[cartSnack[i]].threshold);
}

void addStockFlow(Cafe *c)
{
    int sid, si, qty;
    showSnackMenu(c);
    sid = readIntInRange("Snack ID to restock (0 to cancel): ", 0, 9999);
    if (sid == 0) return;
    si = findSnackIndex(c, sid);
    if (si < 0) { printf("  No snack with that ID.\n"); return; }
    qty = readIntInRange("Quantity to add (1-1000): ", 1, 1000);
    c->snacks[si].quantity += qty;
    saveAll(c);
    printf("  %s stock is now %d.\n", c->snacks[si].name, c->snacks[si].quantity);
}

void addNewSnackFlow(Cafe *c)
{
    Snack s;
    int i;
    if (c->snackCount >= MAX_SNACKS) { printf("  Snack menu is full.\n"); return; }
    memset(&s, 0, sizeof(s));
    for (;;) {
        readValidName("New snack name: ", s.name, SNACK_NAME_LEN);
        for (i = 0; i < c->snackCount; i++)
            if (compareIgnoreCase(c->snacks[i].name, s.name) == 0) break;
        if (i == c->snackCount) break;
        printf("  A snack with this name already exists.\n");
    }
    s.price = (double)readIntInRange("Unit price in PKR (1-10000): ", 1, 10000);
    s.quantity = readIntInRange("Starting quantity (0-1000): ", 0, 1000);
    s.threshold = readIntInRange("Low-stock threshold (0-100): ", 0, 100);
    s.id = c->nextSnackId++;                /* ids are never reused */
    c->snacks[c->snackCount++] = s;
    saveAll(c);
    printf("  Snack added with ID %d.\n", s.id);
}

/* Admin: remove a dish from the menu. Old sales records keep the dish name. */
void deleteSnackFlow(Cafe *c)
{
    int sid, si, i;
    char name[SNACK_NAME_LEN];

    showSnackMenu(c);
    sid = readIntInRange("Snack ID to DELETE from the menu (0 to cancel): ", 0, 9999);
    if (sid == 0) return;
    si = findSnackIndex(c, sid);
    if (si < 0) { printf("  No snack with that ID.\n"); return; }
    strncpy(name, c->snacks[si].name, SNACK_NAME_LEN - 1);
    name[SNACK_NAME_LEN - 1] = '\0';
    printf("  Delete \"%s\" (stock %d, PKR %.0f)?\n", name, c->snacks[si].quantity, c->snacks[si].price);
    if (!readYesNo("Confirm delete (y/n): ")) { printf("  Not deleted.\n"); return; }
    for (i = si; i < c->snackCount - 1; i++) c->snacks[i] = c->snacks[i + 1];
    c->snackCount--;
    saveAll(c);
    printf("  \"%s\" removed from the menu and the records file.\n", name);
}
