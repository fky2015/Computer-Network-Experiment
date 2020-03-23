import re

infostring = input()
flagstring = input()
escstring = input()

info = infostring.split('=')[-1]
flag = flagstring.split('=')[-1]
esc = escstring.split('=')[-1]

print("帧起始标志：{}，帧数据信息：{}，帧结束表示{}".format(flag, info, flag))
frame = flag + re.sub(r'({}|{})'.format(flag, esc), r'{}\1'.format(esc), info) + flag
print("字节填充之后的发送帧：{}".format(frame))
decode = re.sub(r'{}({}|{})'.format(esc, esc, flag), r'\1', frame[len(flag):-len(flag)])
print("字节删除之后的接收帧：{}".format(decode))
