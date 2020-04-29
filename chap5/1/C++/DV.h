#ifndef DV_H
#define DV_H

#define maxn 5
#define inf 99

void cal_router_table(int (*cost)[maxn], int (*Dxy)[maxn], int (*table)[maxn]);

void print_table(int table[maxn][maxn]);

void update_topo(int (*cost)[maxn]);

#endif