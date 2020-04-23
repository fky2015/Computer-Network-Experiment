/********************************************
@Auther: 孙璋亮
@Date: 2020/4/23
@Copyright (c) 2020 孙璋亮 Studio.  All rights reserved.
********************************************/

#ifndef CPP_IPADDRESS_H
#define CPP_IPADDRESS_H

#include <bitset>
#include <iostream>
#include <string>
class IpAddress
{
private:
    std::bitset<32> address;
    std::bitset<32> mask;
    std::string destination;
public:
    static IpAddress fromstring(const std::string &add,const std::string &dest);
    IpAddress(const std::bitset<32>& address, const std::bitset<32>& mask,std::string dest);
    explicit IpAddress(std::string dest);

    const std::bitset<32> &getAddress() const;

    const std::bitset<32> &getMask() const;

    const std::string &getDestination() const;
    static std::string BitSettoString(std::bitset<32> ipaddr);
    IpAddress();
    std::string showInfo();
    bool match(const std::bitset<32> &addr);
    unsigned maskLength();
};


#endif //CPP_IPADDRESS_H
