import socket
import struct
import binascii

with open("config.txt") as f:
    config_str = f.readline()

header = config_str.strip().split("=")[1]

def int2ip(addr):
    return socket.inet_ntoa(struct.pack("!I", addr))

def extract(start:int, length:int):
    return int(header[start:start+length], 16)

def parse_header(name: str, start: int, length: int):
    print(name, ":", extract(start,length))

def checksum(header: str):
    cs = []
    for i in range(0,len(header),4):
        cs.append(int(header[i:i+4], 16))
    cs[5] = 0
    cs = sum([i for i in cs])
    if cs > 65535:
        cs = int(hex(cs)[2], 16) + int(hex(cs)[3:], 16)
    cs = cs ^ 0xffff
    return cs

parse_header("Version", 0, 1)
parse_header("Header Length", 1, 1)
parse_header("Type of Service", 2, 2)
parse_header("Total Length", 4, 4)
parse_header("Identifier", 8, 4)
parse_header("Flags", 12, 1)
parse_header("Fragmented Offset", 13, 3)
parse_header("Time to Live", 16, 2)
parse_header("Protocal", 18, 2)
parse_header("Header Checksum", 20, 4)
print("Source IP Address", ":", int2ip(extract(24,8)))
print("Destination IP Address", ":", int2ip(extract(32,8)))
print()
print("calculated checksum is: ", checksum(header))
