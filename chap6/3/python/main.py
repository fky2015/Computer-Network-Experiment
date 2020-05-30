import configparser
import ipaddress

def checksum(header: str)-> int:
    cs = []
    for i in range(0,len(header),4):
        cs.append(int(header[i:i+4], 16))
    cs = sum([i for i in cs])
    while cs > 65535:
        cs = (cs & 0xffff) + (cs >> 16);
    cs = cs ^ 0xffff
    return cs

cfg = "./config.ini"
config = configparser.ConfigParser()
config.read(cfg)
tcpseg = config['DEFAULT']['tcpsegment']
sourceip = config['DEFAULT']['sourceip']
destip = config['DEFAULT']['destip']
print("======= pseudo-header =======")
print("source ip:", sourceip)
print("dest ip:", destip)
print("zeros:", 0)
print("protocal:", 6)
print("TCP length:", len(tcpseg)//2)
print("=============================")
print("======= TCP-header =========")
print("source port:", int(tcpseg[:4], 16))
print("dest port:", int(tcpseg[4:8], 16))
print("sequence number:", int(tcpseg[8:16], 16))
print("acknowledge number:", int(tcpseg[16:24], 16))
print("data offset:", int(tcpseg[24:25], 16))
print("reserved:", int(tcpseg[25:26], 16))
print("flags:", int(tcpseg[26:28], 16))
print("window:", int(tcpseg[28:32], 16))
print("checksum:", int(tcpseg[32:36], 16))
print("Urgent pointer:", int(tcpseg[36:40], 16))
print("============================")
pseudo = "%08x" % int(ipaddress.ip_address(sourceip))
pseudo += "%08x" % int(ipaddress.ip_address(destip))
pseudo += "00"
pseudo += "06"
pseudo += "%04x" % 20
tcpseg = tcpseg[:32] + "0000" + tcpseg[36:]
tcpseg = pseudo + tcpseg
if len(tcpseg) % 4 != 0:
    tcpseg += "00"
print(checksum(tcpseg))
