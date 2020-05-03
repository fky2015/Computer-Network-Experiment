//
// Created by hsx on 2020/5/3.
//


#include <string>
#include <string.h>
#include <fstream>
#include <iostream>
#include "F_R.h"

using namespace std;

int get_config(const char* filename, const char* var, char* dest) {
    ifstream in(filename);
    string line;

    if(in){
        while(getline(in, line)){
            int idx = line.find(var);
            if(idx == string::npos)   continue;
            line.replace(idx, strlen(var)+1, "");
            strcpy(dest,line.c_str());
            return 0;
        }
    }
}

void frag(int mtu, datagram bigDatagram, int header_len) {
    int num;
    if (bigDatagram.length % (mtu - header_len)) {
        num = bigDatagram.length / (mtu - header_len) + 1;
    } else {
        num = bigDatagram.length / (mtu - header_len);
    }
    cout << "fragmentation numbers: " << num << endl;
    cout << "----------------fragment information----------------" << endl;
    cout << "----ID----Length----Flags--------fragment offset----" << endl;

    int last_len = bigDatagram.length;
    for (int i = 0; i < num; ++i) {
        datagram fragDatagram{};
        fragDatagram.length = mtu;
        fragDatagram.ID = bigDatagram.ID;
        fragDatagram.DF = true;
        fragDatagram.MF = true;
        if (i == num - 1) {
            fragDatagram.MF = false;
            fragDatagram.length = last_len;
        }
        last_len -= mtu-header_len;
        fragDatagram.offset = (mtu - header_len) * i / 8;
        cout << "    " << fragDatagram.ID << "   " << fragDatagram.length << "      " << "DF="
             << fragDatagram.DF << ",MF=" << fragDatagram.MF << "    " << fragDatagram.offset <<endl;
    }
}



void assemble(datagram frag[], int num, int head_len){
    cout<<"-----------datagram after assembled-----------"<<endl;
    datagram bigDatagram{};
    bigDatagram.length = 0;
    bigDatagram.ID = frag[0].ID;
    bigDatagram.offset = 0;
    bigDatagram.DF = false;
    bigDatagram.MF = false;
    for (int i = 0; i < num; ++i) {
        bigDatagram.length += frag[i].length-head_len;
    }
    bigDatagram.length += head_len;
    cout<<"------ID----Length----Flags--------offset-----"<<endl;
    cout<<"      "<<bigDatagram.ID<<"   "<<bigDatagram.length<<"      "<<"DF="
        <<bigDatagram.DF<<",MF="<<bigDatagram.MF<<"    "<<bigDatagram.offset<<endl<<endl<<endl;
}

