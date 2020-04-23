/********************************************
@Auther: 孙璋亮
@Date: 2020/4/23
@Copyright (c) 2020 孙璋亮 Studio.  All rights reserved.
********************************************/

#include "RouterTable.h"

using namespace std;

void RouterTable::addRouter(const IpAddress &elem)
{
    table.push_back(elem);
}

IpAddress RouterTable::match(const std::string &destIp)
{
    return match(IptoBitset(destIp));
}


void RouterTable::showDetail()
{
    cout
            << "-------------------------------------------------------Router Table-------------------------------------------------------"
            << endl;
    for (auto &elem:table)
        cout << elem.showInfo() << endl;
    cout
            << "--------------------------------------------------------------------------------------------------------------------------"
            << endl;
}

RouterTable::RouterTable()
= default;

std::bitset<32> RouterTable::IptoBitset(const std::string &str)
{
    unsigned long long ret = 0;
    unsigned int tmp = 0;
    for (auto &ch:str)
    {
        if (ch >= '0' && ch <= '9')
            tmp = tmp * 10 + ch - '0';
        else if (ch == '.')
        {
            ret += tmp;
            ret *= 256;
            tmp = 0;
        }
    }
    ret += tmp;
    return {ret};
}

IpAddress RouterTable::match(const std::bitset<32> &destIp)
{
    cout << "trying matching..." << endl;
    unsigned int masklength = 0;
    IpAddress matchAddress;
    for (auto &elem:table)
        if (elem.maskLength() >= masklength && elem.match(destIp))
        {
            matchAddress = elem;
            masklength = elem.maskLength();
        }
    cout << "matching complete!" << endl;
    return matchAddress;
}
