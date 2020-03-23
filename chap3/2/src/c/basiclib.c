#include "basiclib.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

const char *load_file(const char *config_file_name) {

    // read config file
    FILE *f = fopen(config_file_name, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET); /* same as rewind(f); */

    char *string = malloc(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);

    string[fsize] = 0;
    return string;
}

int get_config(const char *buffer, const char *var, char *dest) {
    const char *start = strstr(buffer, var);
    if (start == 0) {
        return 1;
    }

    const char *end = strstr(start, "\n");
    if (end == 0) {
        return 2;
    }

    strncpy(dest, start + strlen(var) + 1, end - start - strlen(var) - 1);
    return 0;
}
