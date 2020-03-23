#include <stdio.h>
#include <string.h>

void encode(char *dest, const char *src, const char *flag) {
    int i;
    int accumulated_one = 0;
    int dest_idx = 0;


    strcpy(dest, flag);
    dest_idx += strlen(flag);

    for (i = 0; i < strlen(src); ++i) {
        if (src[i] == '0') {
            // bit 0
            dest[dest_idx++] = src[i];
            accumulated_one = 0;
        } else {
            // bit 1
            if (accumulated_one == 5) {

                dest[dest_idx++] = '0';
                accumulated_one = 0;
            }
            dest[dest_idx++] = src[i];
            accumulated_one++;
        }
    }
    strcat(dest, flag);

}

void decode(char *dest, const char *src, const char *flag) {
    int i = strstr(src, flag) - src + strlen(flag);
    int end = strstr(src + i, flag) - src;
    int accumulated_one = 0;
    int dest_idx = 0;
    for (; i < end; ++i) {
        if (src[i] == '0') {
            // bit 0
            if (accumulated_one == 5) {
                accumulated_one = 0;
                continue;
            }
            dest[dest_idx++] = src[i];
            accumulated_one = 0;
        } else {
            // bit 1
            dest[dest_idx++] = src[i];
            accumulated_one++;
        }
    }
}
