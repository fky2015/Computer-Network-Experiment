#include "basiclib.h"
#include "zero-bit-stuffing-lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 100

char info_str[BUFFER_SIZE];
char flag_str[BUFFER_SIZE];
char rest_str[BUFFER_SIZE];
char decoded_str[BUFFER_SIZE];

int main() {
    const char *config_string = load_file("config1");

    get_config(config_string, "InfoString1", info_str);
    get_config(config_string, "FlagString", flag_str);

    printf("start_flag: %s, info_str: %s, end_flag: %s\n", flag_str, info_str,
           flag_str);
    encode(rest_str, info_str, flag_str);
    printf("frame: %s\n", rest_str);
    decode(decoded_str, rest_str, flag_str);

    printf("after bit removed: %s\n", decoded_str);
}
