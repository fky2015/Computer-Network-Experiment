#include <iostream>
#include "tcp.h"
#include <chrono>

typedef std::chrono::high_resolution_clock Clock;
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

    while (true) {
        tcp p(0);
        auto t1 = Clock::now();
        cout << "Ping " << ip << ":" << port << endl;
        p.connect_to(ip, port);
        auto t2 = Clock::now();
        cout << "Round Time: " << (std::chrono::nanoseconds(t2 - t1).count() / 1e6) << "ms" << endl;
        sleep(1);
    }
    return 0;
}
