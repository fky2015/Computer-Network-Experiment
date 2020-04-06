#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char crc_str[50];

int get_config(const char* buffer, const char* var, char* dest) {
    const char* start = strstr(buffer, var);
    if (start == 0) {
        return 1;
    }

    const char* end = strstr(start, "\n");
    if (end == 0) {
        return 2;
    }

    strncpy(dest, start + strlen(var) + 1, end - start - strlen(var) - 1);
    return 0;
}

long long str2int(char str[])
{
    int len = strlen(str);
    long long dic = 0;
    int i;  
    for (i = 0; i < len; i++)
    {
        dic = dic * 2 + (str[i] - '0');
    }
    return dic;
}

void out_crc_bin(int n)
{
    int a[50];
    int i = 0;
    while (n>0)
    {
        a[i]=n%2;
        i=i+1;
        n=n/2;
    }
    for(int j=15; j >=i; j--)
    {
        printf("0");
        crc_str[15-j] = '0';
    }
    for(i--; i>=0; i--)
    {
        printf("%d", a[i]);
        crc_str[15-i] = a[i] + '0';
    }
    printf("\n");
}

int crc(long long data, int gen, int len)
{
    int crc = 0xffff; 
    unsigned char ptr[4];
    for(int i = 0; i < len; i++)
    {
        
        ptr[i] = (data>>((len-i-1)*8))&0xff;
//         printf("%d ", ptr[i]);
        crc ^= (ptr[i] << 8);
        for(int j = 0; j < 8; j++)
        {
            if((crc & 0x8000) != 0)
            {
                crc = (crc << 1) ^ gen;
            }
            else
            {
                crc <<= 1;
            }
            crc &= 0xffff; 
        }
    };
//     printf("%d\n", (crc & (0xffff)));
    return(crc & (0xffff));
}
