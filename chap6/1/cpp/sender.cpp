#include <iostream>
#include "udp.h"
using namespace std;

int main(int argc, char **argv) {
    int c;
    string ip = "47.95.223.44";
    int port = 8001;
    while ((c = getopt(argc, argv, "i:p:")) != -1) {
        switch (c) {
            case 'i':
                ip = string(optarg);
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case '?':
                cout << "Bad argument setting!" << endl;
                break;
        }
    }
    udp p(0);
    string x;
    while (cin >> x) {
        p.send_to(x, ip, port);
        cout << p.recv() << endl;
    }
    return 0;
}
