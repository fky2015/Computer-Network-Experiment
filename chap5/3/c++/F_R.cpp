#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <cstring>
#include "F_R.h"

using namespace std;

int main() {
    const char* filename = "/home/hsx/homework/network/lab5-3/F_R.conf";

    datagram bigDatagram{};
    char strl[10];
    char strid[10];
    char strmtu[10];
    char strnum[10];
    char str_len_arr[30];
    char str_id_arr[30];
    char str_mf_arr[30];
    char str_off_arr[30];
    int FragNum;

    get_config(filename, "BigIPTotalLen", strl);
    get_config(filename, "ID", strid);
    get_config(filename, "MTU", strmtu);
    get_config(filename, "FragNum", strnum);
    get_config(filename, "TotalLenlist", str_len_arr);
    get_config(filename, "IDlist", str_id_arr);
    get_config(filename, "FragMF", str_mf_arr);
    get_config(filename, "FragOffset", str_off_arr);

    bigDatagram.length = atoi(strl);

    bigDatagram.ID = atoi(strid);
    bigDatagram.DF = false;
    bigDatagram.MF = false;
    bigDatagram.offset = 0;


    int mtu = atoi(strmtu);

    cout<<"-----------big datagram information-----------"<<endl;
    cout<<"------ID----Length----Flags--------offset-----"<<endl;
    cout<<"      "<<bigDatagram.ID<<"   "<<bigDatagram.length<<"      "<<"DF="
        <<bigDatagram.DF<<",MF="<<bigDatagram.MF<<"    "<<bigDatagram.offset<<endl<<endl<<endl;

    cout<<"MTU: "<<mtu<<endl;
    frag(mtu, bigDatagram);

    cout<<"fragments are about to be reassembled..."<<endl;
    FragNum = atoi(strnum);
    datagram fragDatagram[FragNum];
    cout<<"fragmentation numbers: "<<FragNum<<endl;
    cout<<"----------------fragment information----------------"<<endl;
    cout<<"----ID----Length----Flags--------fragment offset----"<<endl;

    const char *d = ",";

    char* len = strtok(str_len_arr,d);
    for (int i = 0; i < FragNum; ++i) {
        fragDatagram[i].length = atoi(len);
        fragDatagram[i].DF = true;
        len = strtok(NULL, d);
    }

    char* id = strtok(str_id_arr,d);
    for (int i = 0;  i < FragNum ; i++){
        fragDatagram[i].ID = atoi(id);
        id = strtok(NULL, d);
    }

    char* mf = strtok(str_mf_arr,d);
    for (int i = 0;  i < FragNum ; i++){
        fragDatagram[i].MF = atoi(mf);
        mf = strtok(NULL, d);
    }

    char* offset= strtok(str_off_arr,d);
    for (int i = 0;  i < FragNum ; i++){
        fragDatagram[i].offset = atoi(offset);
        offset = strtok(NULL, d);
    }

    for (int i = 0;  i < FragNum ; i++) {
        cout<<"    "<<fragDatagram[i].ID<<"   "<<fragDatagram[i].length<<"      "<<"DF="
            <<fragDatagram[i].DF<<",MF="<<fragDatagram[i].MF<<"    "<<fragDatagram[i].offset<<endl;
    }

    cout<<endl;

    assemble(fragDatagram, FragNum);

    return 0;
}
