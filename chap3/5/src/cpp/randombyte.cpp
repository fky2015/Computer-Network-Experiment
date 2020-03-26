/********************************************
@Auther: 孙璋亮
@Date: 2020/3/26
@Copyright (c) 2020 孙璋亮 Studio.  All rights reserved.
********************************************/

#include "randombyte.h"
#include <ctime>
#include <iostream>
bool randombyte::exist(const std::string &str)
{
    return ExistMap[str];
}

bool randombyte::set_exist(const std::string &str)
{
    return ExistMap[str]= true;
}

randombyte::randombyte():e(time(nullptr)),rng(0,255)
{
    ExistMap.clear();
}

std::string randombyte::generate_str(int len)
{
    std::string ret;
    for (int i=0;i<len;i++)
    {
        ret += (char) rng(e);
    }
    if (!exist(ret))
        return set_exist(ret),ret;
    else
        return generate_str(len);
}
