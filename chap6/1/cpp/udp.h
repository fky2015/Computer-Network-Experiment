//
// Created by 谢威宇 on 2020/5/27.
//

#ifndef COMPUTER_NETWORKS_6_1_UDP_H
#define COMPUTER_NETWORKS_6_1_UDP_H

#include <iostream>
#include <fstream>
#include <random>
#include <bitset>
#include <cstring>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>
#include <mutex>
#include <queue>
#include <vector>
#include <condition_variable>
#include <string>


unsigned from_string_to_int(std::string ip) {
    int re = 0;
    int d = 0;
    for (auto c:ip) {
        if (c == '.') {
            re = re * 256 + d;
            d = 0;
        } else {
            d = d * 10 + c - '0';
        }
    }
    re = re * 256 + d;
    return re;
}

std::string int_to_string(unsigned char *a) {
    char t[100];
    sprintf(t, "%d.%d.%d.%d", a[0], a[1], a[2], a[3]);
    return std::string(t);
}


class udp {
    // use socket udp to simulate udp layer
    int socket_id;
    socklen_t last_src_addrlen;
    sockaddr_in last_src;
public:
    udp(int port = 0) {
        socket_id = socket(AF_INET, SOCK_DGRAM, 0);
        if (socket_id < 0) {
            printf("socket create fail!\n");
        }
        sockaddr_in us{};
        us.sin_family = AF_INET;
        us.sin_addr.s_addr = htonl(INADDR_ANY);
        us.sin_port = htons(port);
        auto re = bind(socket_id, (struct sockaddr *) &us, sizeof(us));
        if (re < 0) {
            printf("socket bind fail!\n");
        }
    }

    void send_to(std::string info, std::string ip, int port) {
        sockaddr_in dst{};
        dst.sin_family = AF_INET;
        dst.sin_addr.s_addr = htonl(from_string_to_int(ip));
        dst.sin_port = htons(port);
        sendto(socket_id, info.data(), info.length(), 0, (sockaddr *) &dst, sizeof(dst));
//        std::cout << "Sent: " << dst.sin_port << std::endl;
    }

    void send_back(std::string info) {
        sendto(socket_id, info.data(), info.length(), 0, (sockaddr *) &last_src, sizeof(last_src));
    }

    std::string recv() {
        std::string re;
        const int BUFF_LEN = 1024;
        re.resize(BUFF_LEN);
        last_src_addrlen = sizeof(last_src);
        recvfrom(socket_id, (void *) re.data(), BUFF_LEN, 0, (struct sockaddr *) &last_src, &last_src_addrlen);
//        std::cout << last_src.sin_port << std::endl;
//        std::cout << last_src.sin_addr.s_addr << std::endl;
        std::cout << "From " << int_to_string((unsigned char *) &last_src.sin_addr.s_addr) << ":" << last_src.sin_port
                  << " received" << std::endl;
        while (re.back() == 0)
            re.pop_back();
        return re;
    }
};


#endif //COMPUTER_NETWORKS_6_1_UDP_H
