import socket
from CRC import CRC
import random
import  pickle

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

def bytes2ascStr(src:bytes) -> str:
    return ''.join(f'{byte:08b}' for byte in src)

def from_network_layer():
    flist = []
    file_object = open('test.txt','rb')

    try:
        while True:
            chunk = file_object.read(length)
            # print(chunk)
            if not chunk:
                break
            chunk = bytes2ascStr(chunk)
            chunk = bytes(chunk,encoding='utf8')
            flist.append(chunk)
    finally:
        file_object.close()

    return flist

def generate_random_list(info_len:int):
    rlist = ''
    for i in range(info_len):
        rlist = rlist + str(random.randint(0,1))
    return bytes(rlist,encoding = "utf8")

def ascStr2bytes(t:str):
    return int(t,2).to_bytes((len(t) + 7)// 8, 'big')

def to_network_layer(r:bytes,crc_n:int):
    r = str(r,encoding='utf8')
    r=r[:-crc_n+1]
    r = ascStr2bytes(r)
    print("接收到确认帧的内容："+str(r,encoding='utf8'))

def inc(num):
    if(num == 1):
        num = 0
    elif num == 0:
        num =1
    return num

if __name__ == '__main__':
    next_frame_to_send = 0
    flist = from_network_layer()
    findex = 0
    FilterError = 10
    FilterLost = 10
    ferr = False
    flost = False
    generator = bytes('11000100110000011',encoding='utf8')
    crc_n = len('11000100110000011')

    sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
    server_address = ('127.0.0.1',8888)
    sock.bind(server_address)
    client_address = ('127.0.0.1',8889)


    while True:
        s = frame()
        s.seq = next_frame_to_send

        if (findex >= len(flist)):
            s.info = bytes('',encoding='utf8')
        elif (findex % FilterError == 0 and not ferr):
            # 模拟出错
            s.info = generate_random_list(length * 8 + crc_n - 1)
            ferr = True
        else:
            ferr = False
            info = flist[findex]
            res = CRC.create_crc(info,generator)
            s.info = res.code

        print("next_frame_to_send is " + str(next_frame_to_send))
        # print(str(s.info,encoding='utf8'))

        try:
            if (not (findex % FilterLost == 1) or flost or findex >= len(flist)):
                sent = sock.sendto(pickle.dumps(s), client_address)
                flost = False
            else:
                # 模拟丢失
                flost = True

            sock.settimeout(10)
            print('正在发送帧的编号: ' + str(next_frame_to_send))

            data, client_address = sock.recvfrom(4096)
            data = pickle.loads(data)
            if data:
                sock.settimeout(None)
                if(data.ack == next_frame_to_send and findex >= len(flist) ):
                    print("传输完毕")
                    break
                elif (data.ack == next_frame_to_send):
                    print("正确发送")
                    print("确认帧的确认号：" + str(data.ack))
                    to_network_layer(data.info,crc_n)
                    next_frame_to_send = inc(next_frame_to_send)
                    findex = findex + 1
                else:
                    print("传输错误")
            print('\n--------------------------------------------------------------------------\n')

        except Exception as e:
            print('出错server: ' + str(e))
            print('\n--------------------------------------------------------------------------\n')

    print('closing socket')
    sock.close()


