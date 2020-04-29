#include <iostream>
#include <string.h>
#include <cstdlib>
#include <cstdio>
#include "DV.h"

using namespace std;


int main()
{
    int cost[maxn][maxn];
    int Dxy[maxn][maxn];
    int table[maxn][maxn];
    memset(cost, 0, sizeof(cost));

    freopen("DV.conf","r",stdin);

    for(int i = 0; i < maxn; i++){
        for(int j = 0; j < maxn; j++){
            cin>>cost[i][j];
            Dxy[i][j] = cost[i][j];
            if(cost[i][j] != inf){
                table[i][j] = j;
            }
            if(i == j){
                Dxy[i][j] = 0;
                table[i][j] = i;
            }
        }    
    }

    fclose(stdin);

    cal_router_table(cost, Dxy, table);
    print_table(table);

    update_topo(cost);

    for(int p = 0; p < maxn; p++){
        for(int q = 0; q < maxn; q++){
            printf("%d ", cost[p][q]);                        
        }       
    }        

    cal_router_table(cost, Dxy, table);
    printf("\n\nUpdated router tables：\n");
    print_table(table);
}