#include <iostream>
#include "randombyte.h"
#include "md5.h"

const char HEX_NUMBERS[16] = {
        '0', '1', '2', '3',
        '4', '5', '6', '7',
        '8', '9', 'a', 'b',
        'c', 'd', 'e', 'f'
};

/**
 * @Convert byte to hex string.
 *
 * @return the hex string.
 *
 */
std::string toHex(const std::string& bytestr)
{
    string str="0x";
    for (unsigned char t : bytestr) {
//        int t=(unsigned char)tmp;
        int a = t / 16;
        int b = t % 16;
        str.append(1, HEX_NUMBERS[a]);
        str.append(1, HEX_NUMBERS[b]);
    }
    return str;
}

int main(int argc,char **argv)
{
    std::string pwd;
    if (argc<2)
    {
        std::cout<<"请输入口令："<<std::endl;
        std::cin>>pwd;
    } else
    {
        pwd=argv[1];
    }
    std::cout<<"需要验证的口令：\t"<<pwd<<std::endl;
    std::cout<<"口令使用八进制表示：\t"<<toHex(pwd)<<std::endl;
    randombyte random_byte;
    std::string rdnum=random_byte.generate_str();
    std::cout<<"生成的随机数：\t"<<toHex(rdnum)<<std::endl;
    std::string challenge=rdnum+pwd;
    MD5 md5(challenge);
    std::cout<<"当前生成的MD5摘要值：\t"<<md5.toStr()<<std::endl;
}
