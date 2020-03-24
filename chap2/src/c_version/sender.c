#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "seriallib.h"

const char *config_file_name = "sender_config";

char buffer[100];
char device[100];
char value[100];

int main() {

    // read config file
    FILE *f = fopen(config_file_name, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET); /* same as rewind(f); */

    char *string = malloc(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);

    string[fsize] = 0;

    // get value
    get_config(string, "SerialPort", device);
    get_config(string, "BaudRate", value);
    int _br = atoi(value);
    memset(value, 0, sizeof(value));
    get_config(string, "DataBits", value);
    int _db = atoi(value);
    memset(value, 0, sizeof(value));
    get_config(string, "Parity", value);
    int _parity = atoi(value);
    memset(value, 0, sizeof(value));
    get_config(string, "StopBits", value);
    int _sb = atoi(value);
    memset(value, 0, sizeof(value));

    // set config and print config

    int fd = set_config(device, _br, _parity, _db, _sb);

    while (1) {
        puts("wait for input:");
        char *line = 0;
        ssize_t len = 0;
        getline(&line, &len, stdin);
        strcpy(buffer,line);
        write(fd, buffer, sizeof(buffer));
        memset(buffer, 0, sizeof(buffer));
    }

    return 0;
}
