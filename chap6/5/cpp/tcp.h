//
// Created by 谢威宇 on 2020/5/28.
//

#ifndef COMPUTER_GRAPHICS_6_5_TCP_H
#define COMPUTER_GRAPHICS_6_5_TCP_H
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


class tcp {
    int socket_id;
    socklen_t last_src_addrlen;
    sockaddr_in last_src;
public:
    tcp(int port = 0) {
        socket_id = socket(AF_INET, SOCK_STREAM, 0);
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

    ~tcp() {
        close(socket_id);
    }

    void connect_to(std::string ip, int port) {
        sockaddr_in dst{};
        dst.sin_family = AF_INET;
        dst.sin_addr.s_addr = htonl(from_string_to_int(ip));
        dst.sin_port = htons(port);
        if (auto cnn = connect(socket_id, (sockaddr *) &dst, sizeof(dst)) < 0) {
            std::cout << "Connect Failed" << std::endl;
        } else {
            shutdown(cnn, 2);    /* close the connection */
            std::cout << "Connection Success!" << std::endl;
        }
    }


    void listen_from() {
        if (listen(socket_id, 5) < 0) {
            std::cout << "Listen Failed" << std::endl;
        }
        int rqst;
        for (;;) {
            while ((rqst = accept(socket_id,
                                  (struct sockaddr *) &last_src, &last_src_addrlen)) < 0) {
                if ((errno != ECHILD) && (errno != EINTR)) {
                    perror("accept failed");
                    exit(1);
                }
            }
            printf("received a connection from: %s port %d\n",
                   inet_ntoa(last_src.sin_addr), ntohs(last_src.sin_port));
            shutdown(rqst, 2);    /* close the connection */
        }
    }
};


#endif //COMPUTER_NETWORKS_6_1_UDP_H

#endif //COMPUTER_GRAPHICS_6_5_TCP_H
