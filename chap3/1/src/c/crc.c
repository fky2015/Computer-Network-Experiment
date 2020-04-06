#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crc.h"

long long data_send, gen, data_rec, rec_frame;
char crc_str[50];


int main()
{
    FILE* f = fopen("CRC.config", "r");

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    rewind(f);

    char* string = malloc(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);

    string[fsize] = 0;

    char strdata[100];
    char strgen[50];
    char strrec[50];
    get_config(string, "InfoString1", strdata);
    get_config(string, "GenXString", strgen);
    get_config(string, "InfoString2", strrec);

//     printf("%s\n%s\n", strdata, strgen);
    strrec[32] = '\0';
    strdata[32] = '\0';
    strgen[17] = '\0';

    data_send = str2int(strdata);
//     printf("data_send:%d\n", data_send);
    gen = str2int(strgen);
//     printf("gen:%d\n", gen);
    data_rec = str2int(strrec);
    rec_frame = data_rec<<16;
//     printf("data: %lld\ngen: %d\n", data, gen);
    int crc_send = crc(data_send, gen, 4);
    int crc_rec = crc(data_rec, gen, 4);
//     printf("strrec:%s\n",strrec);
//     printf("data_rec:%d\n", data_rec);
//     printf("crc_send:%d\n", crc_send);
//     printf("crc_rec:%d\n", crc_rec);
    rec_frame += crc_send;
//      printf("rec_frame:%lld\n", rec_frame);

    printf("待发送的数据信息二进制比特串：%s\n", strdata);
    printf("收发双方预定的生成多项式对应的二进制比特串：%s\n", strgen);
    printf("生成的CRC-Code：");
    out_crc_bin(crc_send);
    strdata[32] = '\0';
    crc_str[16] = '\0';
    strrec[32] = '\0';
    strcat(strdata, crc_str);
    strdata[48] = '\0';
    
    int rec_res = crc(rec_frame, gen, 6);
    printf("带校验和的发送帧：%s\n", strdata);
//     printf("%lld\n%lld\n", rec_frame, rec_res);
    printf("\n\n接收的数据信息二进制比特串：%s\n", strrec);
    printf("生成的CRC-Code：");
    out_crc_bin(crc_rec);
    printf("计算得到的余数：");
    out_crc_bin(rec_res);
}
