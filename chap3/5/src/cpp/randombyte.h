/********************************************
@Auther: 孙璋亮
@Date: 2020/3/26
@Copyright (c) 2020 孙璋亮 Studio.  All rights reserved.
********************************************/

#ifndef CHAP_VERTIFY_RANDOMBYTE_H
#define CHAP_VERTIFY_RANDOMBYTE_H

#include <unordered_map>
#include <string>
#include <random>
class randombyte
{
private:
    std::unordered_map<std::string,bool> ExistMap;
    std::default_random_engine e;
    std::uniform_int_distribution<unsigned char> rng;
    bool exist(const std::string& str);
    bool set_exist(const std::string& str);
public:
    randombyte();
    std::string generate_str(int len=32);
};


#endif //CHAP_VERTIFY_RANDOMBYTE_H
