infostring = input()
flagstring = input()

info = infostring.split('=')[-1]
flag = flagstring.split('=')[-1]

print("帧起始标志：{}，帧数据信息：{}，帧结束表示{}".format(flag, info, flag))
frame = flag + info.replace('11111', '111110') + flag
print("比特填充之后的发送帧：{}".format(frame))
decode = frame[len(flag):-len(flag)].replace('111110','11111')
print("比特删除之后的接收帧：{}".format(decode))

