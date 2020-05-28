//
// Created by 谢威宇 on 2020/5/27.
//

#include <iostream>
#include "tcp.h"

using namespace std;

int main() {
    tcp p(8001);
    p.listen_from();
    return 0;
}
