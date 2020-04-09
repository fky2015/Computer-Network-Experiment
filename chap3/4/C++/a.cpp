//
// Created by 谢威宇 on 2020/4/8.
//

#include "protocols.cpp"

int main() {

    gbn_protocol x(8880);

    std::thread([&x] { x.excute(8881); }).detach();
    std::string s;
    while (std::cin >> s) {
        x.from_network_layer(s);
    }

    return 0;
}