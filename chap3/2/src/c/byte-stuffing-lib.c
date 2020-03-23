#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ESC "1B"

void encode(char *dest, const char *src, const char *flag) {

    strcpy(dest, flag);

    int i;
    for (i = 0; i < strlen(src); i += 2) {
        if (strncmp(src + i, flag, 2) == 0 || strncmp(src + i, ESC, 2) == 0) {
            // need to dup
            strcat(dest, ESC);
            strcat(dest, flag);
        } else {
            strncat(dest, src + i, 2);
        }
    }

    strcat(dest, flag);
}

void decode(char *dest, const char *src, const char *flag) {
    int i;
    for (i = 2; i < strlen(src) - 2; i += 2) {
        if (strncmp(src + i, ESC, 2) == 0) {
            i += 2;
            strncat(dest, src + i, 2);
        } else {
            strncat(dest, src + i, 2);
        }
    }
}
