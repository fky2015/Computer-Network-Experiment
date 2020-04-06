#ifndef CRC_H
#define CRC_H

int get_config(const char* buffer, const char* var, char* dest);
long long str2int(char *);
void out_crc_bin(int n);
int crc(long long data, int gen, int len);


#endif

