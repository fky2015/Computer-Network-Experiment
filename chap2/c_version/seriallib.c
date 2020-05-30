#include "seriallib.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

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

// return fd
int set_config(const char *device, int baudrate, int parity, int bytesize,
               int stopbits) {

    printf("device: %s\n", device);
    printf("baud rate: %d\n", baudrate);
    printf("databits: %d\n", bytesize);
    printf("parity: %d\n", parity);
    printf("stopbits: %d\n", stopbits);
    int fd = 0;
    // device
    if ((fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY)) <= 0) {
        perror("connot open file.");
        return -1;
    }
    struct termios tio = {}, new_tio = {};

    // activate
    tio.c_cflag |= CLOCAL | CREAD;

    // set baud rate
    int v;
    switch (baudrate) {
    case 9600:
        v = B9600;
        break;
    case 4800:
        v = B4800;
        break;
    default:
        perror("baudrate error");
        exit(1);
    }
    cfsetspeed(&tio, v);

    // set bytesize
    tio.c_cflag &= ~CSIZE;
    switch (bytesize) {
    case 8:
        v = CS8;
        break;
    case 6:
        v = CS6;
    default:
        perror("bytesize error");
        exit(1);
    }
    tio.c_cflag |= v;

    // set parity
    tio.c_cflag |= PARENB;
    switch (parity) {
    case 0:
        tio.c_cflag &= PARODD;
        break;
    case 1:
        tio.c_cflag |= PARODD;
    default:
        perror("parity error");
        exit(1);
    }

    // stopbits
    switch (stopbits) {
    case 1:
        tio.c_cflag |= CSTOPB;
        break;
    case 0:
        tio.c_cflag &= ~CSTOPB;
    default:
        perror("stopbits error");
        exit(1);
    }

    if ((tcsetattr(fd, TCSANOW, &tio)) != 0) {
        perror("cannot set params.");
        return -1;
    }
    return fd;
}
