/**
* @author 袁瑞泽
* @version 创建时间：2020年4月18日 下午8:49:57
* 类说明:链路层抓包及协议分析
*/

import java.io.IOException;
import java.util.Scanner;

import jpcap.*;
import jpcap.JpcapCaptor;
import jpcap.packet.ARPPacket;
import jpcap.packet.EthernetPacket;
import jpcap.packet.ICMPPacket;
import jpcap.packet.IPPacket;
import jpcap.packet.Packet;
import jpcap.packet.TCPPacket;
import jpcap.packet.UDPPacket;

public class JCapture {
	public static void main(String[] args) {
		NetworkInterface[] interfaces = JpcapCaptor.getDeviceList();
		
		if (interfaces == null) {
			System.out.println("No device!");
		}
		
		System.out.println("---------------------------------------------");
		System.out.println("当前配置的网络适配器:");
		System.out.println("---------------------------------------------");
		
		for (int i = 0; i < interfaces.length; i++) {
			System.out.println(i + ": " + interfaces[i].name 
					+ " | " + interfaces[i].description);
		}
		
		System.out.println("请选择捕获的网络适配器编号:");
		
		Scanner sc = new Scanner(System.in);
		int dvNum = sc.nextInt();
		sc.close();
		
		JpcapCaptor jc = null;
		
		try {
			jc = JpcapCaptor.openDevice(interfaces[dvNum], 2000, true, 50);
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		//获取指定个数的数据包
		capture(jc, 10);
		
		jc.close();
		
	}
	
	public static void capture(JpcapCaptor jc, int times) {
		for (int i = 0; i < times; i++) {
			Packet pa = jc.getPacket();
			analysis(pa);
//			if (pa != null && pa instanceof TCPPacket) {
//				TCPPacket tcpPacket = (TCPPacket) pa;
//				if (tcpPacket.src_port == 80) {
//					analysis(pa);
//				}
//				else {
//					i--;
//				}
//			} else {
//				i--;
//			}
		}
	}
	
	public static void analysis(Packet pa) {
		System.out.println("---------------------------------------------");
		System.out.println("数据链路层:");
		System.out.println("---------------------------------------------");
		EthernetPacket ePacket = (EthernetPacket) pa.datalink;
		System.out.print("dst ");
		for (byte b : ePacket.dst_mac) {
			//转化为16进制,并去除符号位
			System.out.print(":" + Integer.toHexString(b & 0xff));
		}
		System.out.println();
		System.out.print("src ");
		for (byte b : ePacket.src_mac) {
			//转化为16进制,并去除符号位
			System.out.print(":" + Integer.toHexString(b & 0xff));
		}
		System.out.println();
		System.out.println("type :0x0" + Integer.toHexString(ePacket.frametype));
		
		int ipproto = 0;
		
		System.out.println("---------------------------------------------");
		System.out.println("网络层:");
		System.out.println("---------------------------------------------");
		
		if (pa instanceof IPPacket && ((IPPacket)pa).version == 4) {
			
			
			IPPacket ipPacket = (IPPacket) pa;
			System.out.println("IPv4");
			System.out.println("Version:" + ipPacket.version);
			String a=Integer.toHexString(ipPacket.header[14]);
			System.out.println("Header Length:" + (a.charAt(1) - '0') * 4 + " bytes");
			System.out.println("Length:" + ipPacket.length);
			System.out.println("Flow Label:" + ipPacket.flow_label);
			System.out.println("CaptureLength:" + ipPacket.caplen);
			System.out.println("Identification:0x" + ipPacket.ident);
			System.out.println("DF (Don't Fragment):" + ipPacket.dont_frag);
			System.out.println("NF (Nore Fragment): " + ipPacket.more_frag);
			System.out.println("Offset:" + ipPacket.offset);
			System.out.println("Priority:" + ipPacket.priority);
			System.out.println("Time To Live:" + ipPacket.hop_limit);
			System.out.println("Upper protocol:" + ipPacket.protocol);
			System.out.println("Source IP:" + ipPacket.src_ip);
			System.out.println("Destination IP:" + ipPacket.dst_ip);
			
			ipproto = ipPacket.protocol;
			
		} else if (pa instanceof ARPPacket) {
			ARPPacket arp =(ARPPacket) pa;
			System.out.println("ARP");
            System.out.println("Capture Length:" + arp.caplen);
            System.out.println("Sender Mac Address:"+arp.getSenderHardwareAddress());
            System.out.println("Sender IP Address:"+arp.getSenderProtocolAddress());
            System.out.println("Target Mac Address:"+arp.getTargetHardwareAddress());
            System.out.println("Target IP Address:"+arp.getTargetProtocolAddress());
            System.out.println("Times:" + arp.sec);
            System.out.println("Hardware Address Length:" + arp.hlen);
            System.out.println("Protocol Address Length:" + arp.plen);
            System.out.println("Hard type:" + Integer.toHexString(arp.hardtype));
            System.out.println("Protocol type:" + arp.prototype);
            System.out.println("Operation type:" + Integer.toHexString(arp.operation));

            ipproto=arp.prototype;
            
		}
		
		System.out.println("---------------------------------------------");
		System.out.println("运输层:");
		System.out.println("---------------------------------------------");
		
		int ishttp = 0;
		
		if (ipproto == 6) {
			
            System.out.println("TCP");
            TCPPacket tcp = (TCPPacket) pa;
            System.out.println("Capture Length:" + tcp.caplen);
            System.out.println("Time Stamp:" + tcp.sec);
            System.out.println("Source IP Address:" + tcp.src_ip);
            System.out.println("Source Port:" + tcp.src_port);
            System.out.println("Destination IP Address:" + tcp.dst_ip);
            System.out.println("Destination Port:" + tcp.dst_port);
            System.out.println("Sequence Number:" + tcp.sequence);
            System.out.println("ACK:" + tcp.ack);
            System.out.println("Push:" + tcp.psh);
            System.out.println("SYN:" + tcp.syn);
            System.out.println("FIN:" + tcp.fin);
            System.out.println("Urgent:" + tcp.urg);
            System.out.println("Reset:" + tcp.rst);
            System.out.println("Window size value:" + tcp.window);
            System.out.println("Acknowledgment Number:" + tcp.ack_num);
            System.out.println("Time To Live:" + tcp.hop_limit);
            System.out.println("Version:" + tcp.version);
            System.out.println("Option:" + byteToHex(tcp.option));
            
            if (tcp.src_port == 80 || tcp.dst_port == 80) {
				ishttp = 1;
			}
            
		}else if (ipproto == 17) {
			
			System.out.println("UDP");
            UDPPacket udp = (UDPPacket) pa;
            System.out.println("Source Port:" + udp.src_port);
            System.out.println("Destination Port:" + udp.dst_port);
            System.out.println("Length:" + udp.length);
            
		}else if (ipproto == 1) {
			System.out.println("ICMP");
			ICMPPacket icmp = (ICMPPacket) pa;
			System.out.println("Type:" + icmp.type);
			System.out.println("Code:" + icmp.code);
			System.out.println("Checksum:" + icmp.checksum);
		}
		
		System.out.println("---------------------------------------------");
		System.out.println("应用层:");
		System.out.println("---------------------------------------------");
		
		if (ishttp == 1) {
			System.out.println("HTTP");
			System.out.println("Length:" + pa.data.length);
			String str = AsciiToString(pa.data);
			System.out.println(str);
		}else {
			System.out.println("Length:" + pa.data.length);
			String str = byteToHex(pa.data);
			System.out.println(str);
		}
	}
	
	public static String AsciiToString(byte[] bytes) {
		String str = "";
		for (int i = 0; i < bytes.length; i++) {
			char ch = (char) (bytes[i] & 0xFF);
			str = str + ch;
		}
		return str;
		
	}
	
	public static String byteToString(byte[] bytes) {
		char ch = 0;
        StringBuilder sb = new StringBuilder("");
        for (int n = 0; n < bytes.length; n++) {
            ch = (char) bytes[n];
            sb.append(ch);
        }
        return sb.toString().trim();
	}

	public static String byteToHex(byte[] bytes){
        String strHex = "";
        StringBuilder sb = new StringBuilder("");
        for (int n = 0; n < bytes.length; n++) {
            strHex = Integer.toHexString(bytes[n] & 0xFF);
         // 每个字节由两个字符表示，高位补0
            sb.append((strHex.length() == 1) ? "0" + strHex : strHex);
        }
        return sb.toString().trim();
    }
}
