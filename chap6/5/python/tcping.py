import socket
import argparse

def checksum(header: bytes)-> int:
    cs = []
    for i in range(0,len(header),2):
        cs.append(int.from_bytes(header[i:i+2], "big"))
    cs = sum([i for i in cs])
    while cs > 0xffff:
        cs = (cs & 0xffff) + (cs >> 16);
    cs = cs ^ 0xffff
    return cs

def tcp_checksum(ip_header: bytes, tcp_header: bytes) -> int:
    qseudo = ip_header[12:16]
    qseudo += ip_header[16:20]
    qseudo += b"\x00"
    qseudo += b"\x06"
    qseudo += b"\x00\x14"
    tcpseg = qseudo + tcp_header
    if len(tcpseg) % 2 != 0:
        tcpseg += b'\x00'
    return checksum(tcpseg)



parser = argparse.ArgumentParser()
parser.add_argument("ip", help="ip address of destination.")
parser.add_argument("port", help="port of dest ip address.")
args=parser.parse_args()

s = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_TCP)
s.setsockopt(socket.IPPROTO_IP, socket.IP_HDRINCL, 1)

srcip = socket.gethostbyname(socket.gethostname())
print(srcip)
srcip = socket.inet_aton(srcip)
destip = socket.inet_aton(args.ip) 

print(srcip)
print(destip)

ip_header = b"\x45\x00\x00\x28"
ip_header += b"\xab\xcd\x00\x00"
ip_header += b"\x40\x06\x00\x00" # including checksum
ip_header += srcip
ip_header += destip

ip_header = ip_header[:10] + checksum(ip_header).to_bytes(2,'big') + ip_header[12:]

print(ip_header)

tcp_header  = b"\x30\x39" # Source Port | Destination Port
tcp_header += int(args.port).to_bytes(2, byteorder='big')
tcp_header += b'\x00\x00\x00\x01' # Sequence Number
tcp_header += b'\x00\x00\x00\x00' # Acknowledgement Number
tcp_header += b'\x50\x02\x71\x10' # Data Offset, Reserved, Flags | Window Size
tcp_header += b'\x00\x00\x00\x00' # Checksum | Urgent Pointer

tcp_header = tcp_header[:16] + tcp_checksum(ip_header, tcp_header).to_bytes(2, 'big') + tcp_header[18:]

print(ip_header.hex())
print(tcp_header.hex())

packet = ip_header + tcp_header

s.sendto(packet,(args.ip, int(args.port)))
