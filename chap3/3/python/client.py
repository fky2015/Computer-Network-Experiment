import socket
from CRC import CRC
import pickle

length = 4

class frame_kind:
    data = 1
    ack = 2
    nak = 3

class frame:
   def __init__(self):
       kind = 1  # frame_kind类型
       seq = 0  #sequence num
       ack = 0  #acknowledge num
       info = bytes("00011010" ,encoding = "utf8") #packet.info

def ascStr2bytes(t:str):
    return int(t,2).to_bytes((len(t) + 7)// 8, 'big')

def to_network_layer(r:bytes,crc_n:int):
    r = str(r,encoding='utf8')
    r=r[:-crc_n+1]
    r = ascStr2bytes(r)
    print("获取得到的帧的内容："+str(r,encoding='utf8'))

def inc(num):
    if(num == 1):
        num = 0
    elif num == 0:
        num =1
    return num


if __name__ == '__main__':
    frame_expected = 0
    client_address = ('127.0.0.1',8889)
    server_address = ('127.0.0.1',8888)
    sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
    sock.bind(client_address)
    generator = bytes('11000100110000011',encoding='utf8')
    crc_n = len('11000100110000011')
    Fexit = False

    while True:

        try:
            data, server_address = sock.recvfrom(4096)
            data = pickle.loads(data)
            if data:
                print("frame_expected is " + str(frame_expected))
                check = CRC.check_crc(data.info,generator)

                if(check and data.seq == frame_expected and data.info != b''):
                    print("接收帧的发送序号是：" + str(data.seq))
                    to_network_layer(data.info,crc_n)
                    frame_expected = inc(frame_expected)
                elif(not check):
                    print("接收帧错误")
                elif(data.info == b''):
                    frame_expected = inc(frame_expected)
                    Fexit = True

                s = frame()
                s.ack = 1 - frame_expected
                s.info = data.info
                print("发送回确认帧：" + str(data.info,encoding='utf8'))
                print("确认帧的确认序号是：" + str(s.ack))
                sent = sock.sendto(pickle.dumps(s),server_address)

                if(Fexit):
                    print("文件接收完成")
                    break

                print('\n--------------------------------------------------------------------------\n')
        except Exception as e:
            print('出错client: ' + str(e))
            print('\n--------------------------------------------------------------------------\n')


    print('closing socket')
    sock.close()

