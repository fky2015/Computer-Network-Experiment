/********************************************
@Auther: 孙璋亮
@Date: 2020/4/23
@Copyright (c) 2020 孙璋亮 Studio.  All rights reserved.
********************************************/

#include "IpAddress.h"

#include <utility>

using namespace std;

IpAddress IpAddress::fromstring(const std::string &add, const std::string &dest)
{
    unsigned long long addr = 0;
    unsigned int tmp = 0;
    for (auto &ch:add)
    {
        switch (ch)
        {
            case '.':
                addr += tmp;
                addr *= 0x100;
                tmp=0;
                break;
            case '/':
                addr += tmp;
                tmp = 0;
                break;
            default:
                if (ch >= '0' && ch <= '9')
                {
                    tmp = tmp * 10 + ch - '0';
                }
                break;
        }
    }
    string str;
    for (int i = 1; i <= 32; i++)
        if (i <= tmp)
            str += '1';
        else
            str += '0';


//    cout << "address: " << bitset<32>(addr) << endl;
//    cout << "mask: " << bitset<32>(str) << endl;
//    cout << "dest: " << dest << endl;
    return IpAddress(bitset<32>(addr), bitset<32>(str), dest);
}

IpAddress::IpAddress(const std::bitset<32> &address, const std::bitset<32> &mask, std::string dest) : address(address),
                                                                                                      mask(mask),
                                                                                                      destination(
                                                                                                              std::move(
                                                                                                                      dest))
{
}

IpAddress::IpAddress(std::string dest) : address(bitset<32>()), mask(bitset<32>()), destination(std::move(dest))
{
}

IpAddress::IpAddress() {}

string IpAddress::showInfo()
{
    string ret;
    unsigned long long tmp=this->address.to_ullong();
    ret+="Address:\t";
    ret+=BitSettoString(this->address)+"\t";
    ret+="Mask:\t";
    ret+=BitSettoString(this->mask)+"\t";
    ret+="Destination:\t"+this->destination+"\t";
    return ret;
}


bool IpAddress::match(const std::bitset<32> &addr)
{
    bool ret=(addr&this->mask).operator==(this->address);
    cout <<showInfo()+"\tMatch:\t" << (ret ? "true" : "false") << endl;
    return ret;
}

unsigned IpAddress::maskLength()
{
    return this->mask.count();
}

const bitset<32> &IpAddress::getAddress() const
{
    return address;
}

const bitset<32> &IpAddress::getMask() const
{
    return mask;
}

const string &IpAddress::getDestination() const
{
    return destination;
}

std::string IpAddress::BitSettoString(std::bitset<32> ipaddr)
{
    string ret;
    unsigned long long tmp=ipaddr.to_ullong();
    ret+=to_string(tmp/(1<<24))+"."+to_string(tmp%(1<<24)/(1<<16))+"."+to_string(tmp%(1<<16)/(1<<8))+"."+to_string(tmp%(1<<8));
    ret.resize(15);
    for (auto &ch:ret)
    {
        if (ch==0)
            ch=' ';
    }
    return ret;
}
