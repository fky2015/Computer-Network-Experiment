#include "DV.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

using namespace std;

void cal_router_table(int (*cost)[maxn], int (*Dxy)[maxn], int (*table)[maxn]){
    int n = maxn;
    bool changed = false;
    bool changedij = false;
    int tem;
    int ecnt = 0;
    int tag;

    while(1){
        changed = false;
        for(int i = 0; i < n; i++){
            for(int j = 0; j < n; j++){
            	if(i == j)	continue;
                changedij = false;
                tem = 3*inf;
                for(int k = 0; k < n; k++){
                    if(cost[i][k] == inf)   continue;
                	if(k == i)	continue;
                    if(tem > cost[i][k] + Dxy[k][j]){
                        tem = cost[i][k] + Dxy[k][j];
                        tag = k;
                    }                    
                    if(k != j && tem > cost[j][k] + Dxy[k][i]){
                        tem = cost[j][k] + Dxy[k][i];
                        tag = k;
                    }
                }
                if(Dxy[i][j] != tem){
                    Dxy[i][j] = tem;
                    changed = true;
                    changedij = true;
                    table[i][j] = tag;
                }               
//                 if(tem >= inf){
//                     printf("\nAfter %d exchanges:\n", ++ecnt);
//                     for(int p = 0; p < n; p++){
//                         for(int q = 0; q < n; q++){
//                             printf("%d ", Dxy[p][q]);                        
//                         }           
//                         printf("\n");
//                     }
//                     printf("\n\nCan't reach!\n\n");
//                     return;
//                 }
                if(changedij){
                    printf("\nAfter %d exchanges:\n", ++ecnt);
                    for(int p = 0; p < n; p++){
                        for(int q = 0; q < n; q++){
                            printf("%d ", Dxy[p][q]);                        
                        }           
                        printf("\n");
                    }
                }
            }
        }
        if(!changed){
            printf("\nConverged after %d exchanges!\n", ecnt);
            break;
        }
    }
}

void print_table(int table[maxn][maxn]){
    for(int i = 0; i < maxn; i++){
        printf("\n\n          router table of router%d：\n", i);
        printf("--------destination----------next hop--------\n");
        for(int j = 0; j < maxn; j++){
            printf("        router%d              router%d\n", j, table[i][j]);
        }
    }
}

void update_topo(int (*cost)[maxn]){
    cin.clear();
    freopen("/dev/tty", "r", stdin);
    printf("Please input the new topology：\n");
    for(int i = 0; i < maxn; i++){
        for(int j = 0; j < maxn; j++){
            cin>>cost[i][j];
        }
    }
}