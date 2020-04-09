//
// Created by 谢威宇 on 2020/4/9.
//

class Timer {
public:
    static int cnt;
    bool clear = false;

public:
    Timer() {
        cnt++;
//        std::cout << cnt << std::endl;
    }

    ~Timer() {};

    template<typename Function>
    void setTimeout(Function function, int delay) {
        this->clear = false;
        std::thread([=]() {
            if (this->clear) return;
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            if (this->clear) return;
            function();
        }).detach();
    }

    void stop() {
        clear = true;
    }
};

int Timer::cnt = 0;

const __int16_t poly = 0x1021;//defined by CRC-CCITT
const __int16_t init = 0xffff;//defined by CRC-CCITT

__int16_t crc_gen(char *info, int len) {
    __int16_t hold = init;
    __int16_t divider = poly;
    for (int i = 0; i < len + 2; i++) {
        for (int j = 7; j >= 0; j--) {
            bool last = hold & (1 << 15);
            hold <<= 1;
            if (i < len)
                hold |= (info[i] & (1 << j) ? 1 : 0);
            if (last)
                hold ^= divider;
        }
    }
//    std::cout << std::bitset<16>(hold) << std::endl;
    return hold;
}

bool crc_verify(char *info, int len, __int16_t reminder) {
    __int16_t hold = init;
    __int16_t divider = poly;
    for (int i = 0; i < len + 2; i++) {
        for (int j = 7; j >= 0; j--) {
            bool last = hold & (1 << 15);
            hold <<= 1;
            if (i < len)
                hold |= (info[i] & (1 << j) ? 1 : 0);
            else {
                hold |= (reminder & (1 << ((len - i + 1) * 8 + j))) ? 1 : 0;
            }
            if (last)
                hold ^= divider;
        }
    }
    return hold == 0;
}

#include <vector>
#include <string>
#include <set>

std::vector<std::string> split(std::string s, std::set<char> cs) {
    std::vector<std::string> re;
    std::string x;
    for (auto c:s) {
        if (cs.count(c)) {
            if (x.length()) {
                re.push_back(x);
                x = "";
            }
        } else {
            x.push_back(c);
        }
    }
    if (x.size())
        re.push_back(x);
    return re;
}