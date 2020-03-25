import re

infostring = input()
flagstring = input()
escstring = input()

info = infostring.split('=')[-1]
flag = flagstring.split('=')[-1]
esc = escstring.split('=')[-1]

print("帧起始标志：{}，帧数据信息：{}，帧结束表示{}".format(flag, info, flag))


def encode(s):
    re = flag
    for i in range(len(info) - 2):
        if i % 2 == 0:
            x = info[i:i + 2]
            if x == flag or x == esc:
                re += esc
            re += x
    re += flag
    return re


def decode(frame):
    re = ''
    frame = frame[len(flag):-len(flag)]
    e = True
    for i in range(len(frame) - 2):
        if i % 2 == 0:
            x = frame[i:i + 2]
            if x == esc and e :
                continue
            re += x
            e = True
    return re


frame = encode(info)

print("字节填充之后的发送帧：{}".format(frame))

print("字节删除之后的接收帧：{}".format(decode(frame)))
