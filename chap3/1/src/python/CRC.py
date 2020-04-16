import  math
class CRC:

    # self.info: bytes    发送消息原内容
    # self.generator: bytes  生成多项式
    # self.crc: bytes  余数/crc code
    # self.code: bytes 最终传送消息

    def _bytes2list_(self,b):
        b = str(b, encoding = "utf-8")
        b = list(b)
        return b

    def _list2bytes_(self,l):
        str = "".join('%s' %id for id in l)
        return bytes(str,encoding = "utf8")

    @staticmethod
    def create_crc(info:bytes, generator:bytes):
        res = CRC()
        res.info = info
        res.generator = generator
        info = res._bytes2list_(info)
        info_init = info.copy()
        generator = res._bytes2list_(generator)
        crc_n = len(generator)

        for i in range(crc_n - 1):
            info.append('0')

        for i in range(len(info) - crc_n + 1):
            if(info[i] != '0'):
                for j in range(len(generator)):
                    tem = int(info[i+j])^int(generator[j])
                    info[i + j] = str(tem)

        res.crc = res._list2bytes_(info[-crc_n+1:])
        code = info_init + info[-crc_n+1:]
        res.code = res._list2bytes_(code)

        return res

    @staticmethod
    def check_crc(message):
        print("接收的数据信息二进制比特串" + str(message.code, encoding="utf8"))
        # c2 = CRC()
        c2 = CRC.create_crc(message.code, message.generator)
        print("CRC-Code: " + str(c2.crc, encoding="utf8"))
        if (int(c2.crc) == 0):
            print("无错")
            return True
        else:
            print("出错")
            return False

if __name__ == '__main__':
    info = '1101011111'
    info = bytes(info,encoding = "utf8")
    generator = "10011"
    generator = bytes(generator,encoding = "utf8")
    print("待发送的数据信息二进制比特串: " + str(info, encoding="utf8"))
    print("生成多项式对应的二进制比特串: " + str(generator, encoding="utf8"))
    # c = CRC()
    c = CRC.create_crc(info,generator)
    print("CRC-Code: " + str(c.crc, encoding="utf8"))
    print("带校验和的发送帧: " + str(c.code, encoding="utf8"))

    c.check_crc(c)




