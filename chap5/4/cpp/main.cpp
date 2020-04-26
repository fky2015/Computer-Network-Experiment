#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int gethex(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

int gethex(string x) {
    int re = 0;
    for (auto c:x)
        re = re * 16 + gethex(c);
    return re;
}

int main() {
    ifstream config("config.txt");
    char t[1000];
    config.getline(t, 1000);
    config.close();
    string header;
    bool p = false;
    for (int i = 0; t[i]; i++) {
        if (p)
            header.push_back(t[i]);
        if (t[i] == '=')
            p = true;
    }


    cout << header << endl;
    cout << "Protocol Version: " << gethex(header[0]) << endl;
    cout << "IP Header Length: " << gethex(header[1]) * 4 << " Bytes" << endl;
    cout << "Differentiated Services Field: 0x" << header[2] << header[3] << endl;
    cout << "Total Length: " << gethex(header.substr(4, 4)) << endl;
    cout << "Identification: 0x" << header.substr(8, 4) << " (" << gethex(header.substr(8, 4)) << ")" << endl;
    cout << "Flags: 0x" << header.substr(12, 4) << endl;
    cout << "Time to live: " << gethex(header.substr(16, 2)) << endl;
    cout << "Protocol: " << gethex(header.substr(18, 2)) << endl;
    cout << "Header checksum: 0x" << header.substr(20, 4) << endl;
    cout << "Source: ";
    for (int i = 0; i < 4; i++) {
        cout << gethex(header.substr(24 + i * 2, 2)) << (i == 3 ? "" : ".");
    }
    cout << endl;
    cout << "Destination: ";
    for (int i = 0; i < 4; i++) {
        cout << gethex(header.substr(32 + i * 2, 2)) << (i == 3 ? "" : ".");
    }
    cout << endl;

    int tempck = 0;
    for (int i = 0; i < 10; i++) {
        if (i == 5)
            continue;
        tempck += gethex(header.substr(i * 4, 4));
    }
    while (tempck >= (1 << 16)) {
        tempck = (((1 << 16) - 1) & tempck) + (tempck >> 16);
    }
    __int16_t checksum = ~tempck;
    cout << "Computed Checksum: 0x" << hex << checksum << endl;

    return 0;
}
