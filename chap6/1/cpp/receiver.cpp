//
// Created by 谢威宇 on 2020/5/27.
//

#include <iostream>
#include "udp.h"

using namespace std;

int main() {
    udp p(8001);
    while (true) {
        auto re = p.recv();
        cout << re << endl;
        for (auto &c:re) {
            if (c >= 'a' && c <= 'z') {
                c += 'A' - 'a';
            }
        }
        p.send_back(re);
    }
    return 0;
}
