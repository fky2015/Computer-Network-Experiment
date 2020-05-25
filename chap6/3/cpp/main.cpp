#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstdio>
#include <vector>
#include <bitset>

using namespace std;

long long gethex(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

long long gethex(string x) {
    long long re = 0;
    for (auto c:x)
        re = re * 16 + gethex(c);
    return re;
}

int main() {
    ifstream config("config.txt");
    char temp[2000];
    map<string, string> ma;
    while (config.getline(temp, 2000)) {
        string s = temp;
        string key, value;
        for (int i = 0; i < s.length(); i++) {
            if (s[i] == '=') {
                key = s.substr(0, i);
                value = s.substr(i + 1, s.length() - i - 1);
                break;
            }
        }
        ma[key] = value;
    }
    cout << "Source IP:" << ma["sourceip"] << endl;
    cout << "Destination IP:" << ma["destip"] << endl;
    cout << "Protocal:" << "06" << endl;

    string buff;

    for (auto x:vector<string>({ma["sourceip"], ma["destip"]})) {
        int d = 0;
        for (int i = 0; i < x.length(); i++) {
            if (x[i] >= '0' && ma["sourceip"][i] <= '9') {
                d = d * 10 + x[i] - '0';
            } else {
                sprintf(temp, "%02x", d);
                buff.push_back(temp[0]);
                buff.push_back(temp[1]);
                d = 0;
            }
        }
        sprintf(temp, "%02x", d);
        buff.push_back(temp[0]);
        buff.push_back(temp[1]);
        d = 0;
    }

    cout << buff << endl;
    buff.push_back('0');
    buff.push_back('0');
    buff.push_back('0');
    buff.push_back('6');

    int len = ma["tcpsegment"].length()/2;
    sprintf(temp, "%04x", len);
    for (int i = 0; i < 4; i++) {
        buff.push_back(temp[i]);
    }
    buff += ma["tcpsegment"];

    cout << "TCP Length: " << len << endl;

    cout << "Source Port: " << gethex(buff.substr(24, 4)) << endl;
    cout << "Destination Port: " << gethex(buff.substr(28, 4)) << endl;

    cout << "Sequence Number: " << gethex(buff.substr(32, 8)) << endl;
    cout << "Acknowledge Number: " << gethex(buff.substr(40, 8)) << endl;
    cout << "Data Offset: " << gethex(buff.substr(48, 1)) << endl;
    cout << "Flags: " << bitset<12>(gethex(buff.substr(49, 3))) << endl;
    cout << "Window: " << gethex(buff.substr(52, 4)) << endl;
    cout << "Checksum: 0x" << hex<<gethex(buff.substr(56, 4)) << endl;
    cout << "Urgent Pointer: " << gethex(buff.substr(60, 4)) << endl;

    if (buff.length() % 2 == 0)
        buff.push_back('0');
    for (int i = 56; i < 60; i++) {
        buff[i] = '0';
    }
    int tempck = 0;
    for (int i = 0; i < buff.length(); i += 4) {
        tempck += gethex(buff.substr(i, 4));
    }
    while (tempck >= (1 << 16)) {
        tempck = (((1 << 16) - 1) & tempck) + (tempck >> 16);
    }
    __int16_t checksum = ~tempck;
    cout << "Computed Checksum: 0x" << hex << checksum << endl;


    return 0;
}
