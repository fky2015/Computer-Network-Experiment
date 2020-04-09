//
// Created by 谢威宇 on 2020/4/8.
//

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
#include "utils.cpp"

const std::string config_root = "../config.txt";

std::random_device dev;
std::mt19937 rng(dev());
std::uniform_int_distribution<std::mt19937::result_type> dist(100, 200);

int randint() {
    return dist(rng);
}

class physical {
    // use socket udp to simulate physical layer
    int socket_id;
    socklen_t last_src_addrlen;
    sockaddr_in last_src;
public:
    physical(int port) {
        socket_id = socket(AF_INET, SOCK_DGRAM, 0);
        sockaddr_in us{};
        us.sin_family = AF_INET;
        us.sin_addr.s_addr = htonl(INADDR_ANY);
        us.sin_port = htons(port);
        auto re = bind(socket_id, (struct sockaddr *) &us, sizeof(us));
        if (re < 0) {
            printf("socket bind fail!\n");
        }
    }

    void send_to_port(std::string info, int port) {
        sockaddr_in dst{};
        dst.sin_family = AF_INET;
        dst.sin_addr.s_addr = htonl(INADDR_ANY);
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
        recvfrom(socket_id, re.data(), BUFF_LEN, 0, (struct sockaddr *) &last_src, &last_src_addrlen);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
//        std::cout << last_src.sin_port << std::endl;
//        std::cout << last_src.sin_addr.s_addr << std::endl;
        return re;
    }
};

enum frame_kind {
    DATA,
    ACK,
    NAK,
};

class frame {
public:
    struct head {
        unsigned len;
        frame_kind kind;
        unsigned seq = -1;
        unsigned ack = -1;
        bool withcrc = false;
        bool out = true;
        bool timer = false;
    };
    head h;
    std::string info;

    void gen() {
        auto crc = crc_gen(info.data(), info.length());
        info.resize(info.length() + 2);
        *(__int16_t *) (info.data() + info.length() - 2) = crc;
        h.withcrc = true;
//        std::cout<<std::bitset<8>(char(crc >> 8))<<std::endl;
//        std::cout<<std::bitset<8>(char(crc))<<std::endl;
//        std::cout << std::bitset<16>(crc) << std::endl;
    }

    bool verify() {
        h.withcrc = false;
        __int16_t reminder = *(__int16_t *) (info.data() + info.length() - 2);
//        std::cout<<std::bitset<8>(info[info.length()-2])<<std::endl;
//        std::cout<<std::bitset<8>(info[info.length()-1])<<std::endl;
//        std::cout<<std::bitset<16>(reminder)<<std::endl;
        info = info.substr(0, info.length() - 2);
        bool re = crc_verify(info.data(), info.length(), reminder);
        return re;
    }

    void debug() {
        std::cout << "---------frame---------" << std::endl;
        std::cout << "frame kind: " << h.kind << std::endl;
        std::cout << "frame seq: " << h.seq << std::endl;
        std::cout << "frame ack: " << h.ack << std::endl;
        std::cout << "with crc: " << h.withcrc << std::endl;
        if (h.withcrc) {
            __int16_t reminder = *(__int16_t *) (info.data() + info.length() - 2);
            std::cout << std::bitset<16>(reminder) << std::endl;
        }
        std::cout << "frame info: " << info << std::endl;
        std::cout << "-----------------------" << std::endl;
    }

};

std::string to_string(frame &f) {
    f.h.len = f.info.length();
    std::string re;
    re.resize(sizeof(f.h));
    *(frame::head *) re.data() = f.h;
    return re + f.info;
}

frame to_frame(const std::string &s) {
    frame re;
    re.h = *(frame::head *) s.data();
    re.info = s.substr(sizeof(frame::head), re.h.len);
    return re;
}


class FrameQueue {
public:
    FrameQueue() {
        while (frames.size())
            frames.pop();
    }

    inline void put_frame(frame f) {
        std::unique_lock<std::mutex> lock(mtx);
        frames.push(f);
        cv.notify_one();
    }

    inline frame get_frame() {
        std::unique_lock<std::mutex> lock(mtx);

        while (frames.empty()) {
            cv.wait(lock);
        }
        auto re = frames.front();
        frames.pop();
        return re;
    }

private:
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<frame> frames;
};


class gbn_protocol {
    double error_rate;
    double loss_rate;
    physical *p;

    unsigned next_frame_to_send;
    unsigned frame_expected;
    unsigned ack_expected;
    std::vector<frame> frame_buffer;
    std::vector<Timer *> frame_timer;

    const int wait_time = 5000;

    void debug() {
        std::cout << "----------gbn-----------" << std::endl;
        std::cout << "next_frame_to_send:" << next_frame_to_send << std::endl;
        std::cout << "frame_expected:" << frame_expected << std::endl;
        std::cout << "ack_expected:" << ack_expected << std::endl;
        std::cout << "-----------------------" << std::endl;

    }


    void send_frame(frame f, int dst) {
        frame s = f;
        s.gen();
        auto x = (float) rand() / (float) RAND_MAX;
        std::cout << x << std::endl;
        if (x < 1 / loss_rate) {
            std::cout << "Frame Loss" << std::endl;;
        } else if (x < 1 / loss_rate + 1 / error_rate) {
            std::cout << "Frame Error" << std::endl;
            s.info[0] += '1';
            p->send_to_port(to_string(s), dst);
        } else {

            p->send_to_port(to_string(s), dst);
            std::cout << "Frame Sent" << std::endl;
        }

        Timer *t = new Timer;
        if (frame_timer.size() > f.h.seq) {
            frame_timer[f.h.seq]->stop();
            frame_timer[f.h.seq] = t;
        } else {
            frame_buffer.push_back(f);
            frame_timer.push_back(t);
        }

        t->setTimeout([this, f, t] {
            frame t;
            t.h.timer = true;
            t.h.ack = f.h.seq;
            this->fq.put_frame(t);
        }, wait_time);
    }

    void send_back(frame f) {
//        std::cout << "Sent Back:" << std::endl;
//        f.debug();
        sleep(1);
        f.gen();
        p->send_back(to_string(f));
    }

    frame recv_frame() {
//        std::cout << "Receive Frame:" << std::endl;
        auto r = p->recv();
        frame re = to_frame(r);
        re.h.out = false;
        return re;
    }

public:
    FrameQueue fq;
    int port;

    gbn_protocol(int port) : port(port) {
        error_rate = 0;
        loss_rate = 0;
        next_frame_to_send = 0;
        frame_expected = 0;
        ack_expected = 0;
        p = new physical(port);

        std::ifstream config;
        config.open(config_root);
        char in[100];
        while (config.getline(in, 1000)) {
            auto x = split(in, std::set({' ', '=', '\t'}));
//            std::cout<<x[0]<<" "<<x[1]<<std::endl;
            if (x[0] == "FilterError") {
                error_rate = std::atoi(x[1].data());
            }
            if (x[0] == "FilterLost") {
                loss_rate = std::atoi(x[1].data());
            }

        }

    }

    void to_network_layer(std::string s) {
        std::ofstream myfile;
        myfile.open("receive" + std::to_string(port) + ".txt", std::ios_base::app);
        myfile << s;
        myfile.close();
    }

    void from_network_layer(std::string s) {
        frame f;
        f.info = s;
        fq.put_frame(f);
    }


    void excute(int dst) {
        std::cout << "excuting..." << std::endl;
        std::thread recv([this] {
            std::cout << "receiving..." << std::endl;
            while (true) {
                auto f = this->recv_frame();
                std::thread([this, f] {
                    std::chrono::milliseconds timespan(randint());
                    std::this_thread::sleep_for(timespan);
                    this->fq.put_frame(f);
                }).detach();
            }
        });
        recv.detach();

        int timercnt = 0;
        while (true) {
            frame f = fq.get_frame();
            std::cout << std::endl;
            debug();
            if (f.h.timer) {
                std::cout << "Timer Out:" << timercnt++ << " seq:" << f.h.ack << std::endl;
                f.debug();
                for (int i = ack_expected; i < frame_buffer.size(); i++) {
                    send_frame(frame_buffer[i], dst);
                }
            }else {
                timercnt = 0;
                if (f.h.out) {
                    f.h.seq = next_frame_to_send;
                    f.h.kind = DATA;
                    std::cout << "sending..." << std::endl;
                    f.debug();
                    send_frame(f, dst);
                    next_frame_to_send++;

                } else {
                    if (f.verify()) {
                        if (f.h.kind == DATA) {
                            if (f.h.seq == frame_expected) {
                                std::cout << "get data ..." << std::endl;
                                f.debug();
                                to_network_layer(f.info);
                                frame_expected++;
                                frame ack;
                                ack.h.kind = ACK;
                                ack.h.ack = f.h.seq;
                                std::cout << "sending ack ..." << std::endl;
                                ack.debug();
                                send_back(ack);
                            } else {
                                std::cout << "Not expected frame" << std::endl;
                                f.debug();
                            }
                        } else if (f.h.kind == ACK && f.h.ack == ack_expected) {
                            std::cout << "get ack ..." << std::endl;
                            f.debug();
                            frame_timer[ack_expected]->stop();
                            ack_expected++;
                        } else {
                            std::cout << "??? ..." << std::endl;
                            f.debug();
                        }
                    } else {
                        std::cout << "crc failed ..." << std::endl;
                        f.debug();
                    }
                }
            }
        }
    }
};







