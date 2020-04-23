/********************************************
@Auther: 孙璋亮
@Date: 2020/4/23
@Copyright (c) 2020 孙璋亮 Studio.  All rights reserved.
********************************************/

#ifndef CPP_ROUTERTABLE_H
#define CPP_ROUTERTABLE_H

#include <vector>
#include "IpAddress.h"
#include <bitset>
class RouterTable
{
private:
    std::vector<IpAddress> table;
public:
    static std::bitset<32> IptoBitset(const std::string& str);
    void addRouter(const IpAddress &elem);
    IpAddress match(const std::string &destIp);
    IpAddress match(const std::bitset<32> &destIp);
    void showDetail();
    RouterTable();
};


#endif //CPP_ROUTERTABLE_H
