/*
 * input.c
 * Console input helpers: validated reading of numbers, names, phone numbers, yes/no.
 */
#include "config.h"
#include "input.h"

/* Reads one line safely; exits cleanly if input is closed (no infinite loops). */
void readLine(const char *prompt, char *buf, int size)
{
    int ch;
    size_t len;
    printf("%s", prompt);
    if (fgets(buf, size, stdin) == NULL) {
        printf("\nInput closed. Exiting.\n");
        exit(0);
    }
    len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        while ((ch = getchar()) != '\n' && ch != EOF) { }
    }
}

int readIntInRange(const char *prompt, int min, int max)
{
    char line[64];
    char *end;
    long v;
    for (;;) {
        readLine(prompt, line, (int)sizeof(line));
        v = strtol(line, &end, 10);
        if (end != line && *end == '\0' && v >= min && v <= max)
            return (int)v;
        printf("  Invalid input. Enter a number from %d to %d.\n", min, max);
    }
}

int readYesNo(const char *prompt)
{
    char line[16];
    for (;;) {
        readLine(prompt, line, (int)sizeof(line));
        if (line[0] == 'y' || line[0] == 'Y') return 1;
        if (line[0] == 'n' || line[0] == 'N') return 0;
        printf("  Please answer y or n.\n");
    }
}

static int isValidName(const char *s)
{
    int i, letters = 0;
    for (i = 0; s[i] != '\0'; i++) {
        if (isalnum((unsigned char)s[i])) letters++;
        else if (s[i] != ' ' && s[i] != '-' && s[i] != '_' && s[i] != '.' && s[i] != '\'')
            return 0;
    }
    return letters > 0;
}

void readValidName(const char *prompt, char *buf, int size)
{
    for (;;) {
        readLine(prompt, buf, size);
        if (isValidName(buf)) return;
        printf("  Invalid name (letters, digits, space, - _ . ' only; not empty).\n");
    }
}

static int isValidPhone(const char *s)
{
    size_t i, len = strlen(s);
    if (len < 10 || len > 15) return 0;
    for (i = 0; i < len; i++)
        if (!isdigit((unsigned char)s[i])) return 0;
    return 1;
}

void readValidPhone(char *buf, int size)
{
    for (;;) {
        readLine("Phone number (10-15 digits): ", buf, size);
        if (isValidPhone(buf)) return;
        printf("  Invalid phone number.\n");
    }
}

int compareIgnoreCase(const char *a, const char *b)
{
    while (*a && *b && tolower((unsigned char)*a) == tolower((unsigned char)*b)) {
        a++;
        b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

void printLine(void)
{ printf("------------------------------------------------------------\n"); }
