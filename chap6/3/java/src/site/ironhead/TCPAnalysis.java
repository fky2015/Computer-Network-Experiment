package site.ironhead;

import org.junit.Test;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;
import java.util.stream.Collectors;
import java.util.stream.Stream;

public class TCPAnalysis {
    private static final int protocalid = 6; //TCP protocal
    private static final String protocal = String.format("%02x", protocalid);
    private String TCPPacket;
    private final String SourceIP, DestIP, PseudoHeader;
    private final Boolean URG, ACK, PSH, RST, SYN, FIN;
    private final Integer headerLength, SourcePort, DestPort, Sequence, ACKSeq, WindowSize;
    private final String CheckSum, Emergency, Option, Data;

    public TCPAnalysis() {
        Properties properties = new Properties();
        InputStream inputStream = null;
        try {
            inputStream = new FileInputStream("properties/config.properties");
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
        try {
            properties.load(inputStream);
        } catch (IOException e) {
            e.printStackTrace();
        }
        TCPPacket = properties.getProperty("tcpsegment");
        TCPPacket = TCPPacket.replace(" ", "");
        assert TCPPacket.length() % 2 == 0;
        SourceIP = properties.getProperty("sourceip");
        DestIP = properties.getProperty("destip");
        PseudoHeader = IPtoHexString(SourceIP) + IPtoHexString(DestIP) + "00" + protocal + String.format("%04x", TCPPacket.length() / 2  );
        SourcePort = Integer.parseUnsignedInt(TCPPacket.substring(0, 4), 16);
        DestPort = Integer.parseUnsignedInt(TCPPacket.substring(4, 8), 16);
        Sequence = Integer.parseUnsignedInt(TCPPacket.substring(8, 16), 16);
        ACKSeq = Integer.parseUnsignedInt(TCPPacket.substring(16, 24), 16);
        headerLength = Integer.parseUnsignedInt(TCPPacket.substring(24, 25), 16);
        int flags = Integer.parseUnsignedInt(TCPPacket.substring(26, 28), 16);
        URG = (flags & (1 << 5)) != 0;
        ACK = (flags & (1 << 4)) != 0;
        PSH = (flags & (1 << 3)) != 0;
        RST = (flags & (1 << 2)) != 0;
        SYN = (flags & (1 << 1)) != 0;
        FIN = (flags & 1) != 0;
        WindowSize = Integer.parseUnsignedInt(TCPPacket.substring(28, 32), 16);
        CheckSum = TCPPacket.substring(32, 36);
        Emergency = TCPPacket.substring(36, 40);
        if (headerLength > 5) {
            Option= TCPPacket.substring(40,headerLength*8);
        } else {
            Option = "none";
        }
        Data= TCPPacket.substring(headerLength*8);
    }

    public TCPAnalysis(String url) {
        Properties properties = new Properties();
        InputStream inputStream = null;
        try {
            inputStream = new FileInputStream(url);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
        try {
            properties.load(inputStream);
        } catch (IOException e) {
            e.printStackTrace();
        }
        TCPPacket = properties.getProperty("tcpsegment");
        TCPPacket = TCPPacket.replace(" ", "");
        assert TCPPacket.length() % 2 == 0;
        SourceIP = properties.getProperty("sourceip");
        DestIP = properties.getProperty("destip");
        PseudoHeader = IPtoHexString(SourceIP) + IPtoHexString(DestIP) + "00" + protocal + String.format("%04x", TCPPacket.length() / 2);
        SourcePort = Integer.parseUnsignedInt(TCPPacket.substring(0, 4), 16);
        DestPort = Integer.parseUnsignedInt(TCPPacket.substring(4, 8), 16);
        Sequence = Integer.parseUnsignedInt(TCPPacket.substring(8, 16), 16);
        ACKSeq = Integer.parseUnsignedInt(TCPPacket.substring(16, 24), 16);
        headerLength = Integer.parseUnsignedInt(TCPPacket.substring(24, 25), 16);
        int flags = Integer.parseUnsignedInt(TCPPacket.substring(26, 28), 16);
        URG = (flags & (1 << 5)) != 0;
        ACK = (flags & (1 << 4)) != 0;
        PSH = (flags & (1 << 3)) != 0;
        RST = (flags & (1 << 2)) != 0;
        SYN = (flags & (1 << 1)) != 0;
        FIN = (flags & 1) != 0;
        WindowSize = Integer.parseUnsignedInt(TCPPacket.substring(28, 32), 16);
        CheckSum = TCPPacket.substring(32, 36);
        Emergency = TCPPacket.substring(36, 40);
        if (headerLength > 5) {
            Option = TCPPacket.substring(40, headerLength * 8);
        } else {
            Option = "none";
        }
        Data = TCPPacket.substring(headerLength * 8);
    }

    public static String IPtoHexString(String ip) {
        Stream<String> ips = Stream.of(ip.split("\\."));
        return ips
                .map(tmp -> String.format("%02x", Integer.parseUnsignedInt(tmp)))
                .collect(Collectors.joining());
    }

    public String CalculateChecksum() {
        String tmp =PseudoHeader+TCPPacket.substring(0, 32) + "0000" + TCPPacket.substring(36);
        return makeCheckSum(tmp);
    }

    private static String makeCheckSum(String data) {
        if (data.length()%4!=0)
            data+="00";
        Integer dSum = 0;
        int length = data.length();
        int index = 0;
        while (index < length) {
            String s = data.substring(index, index + 4); // 截取2位字符
            dSum += Integer.parseUnsignedInt(s, 16); // 十六进制转成十进制 , 并计算十进制的总和
            index = index + 4;
        }
        while (dSum>0xffff){
            dSum=dSum/0x10000+dSum%0x10000;
        }
        return String.format("%04x",dSum^0xffff);
    }

    public void PrintTCPInfo(){
        System.out.println("-------------------Pseudo Header------------------");
        System.out.println("Pseudo Header: "+PseudoHeader);
        System.out.println("Source IP Address: "+SourceIP);
        System.out.println("Destination IP Address: "+DestIP);
        System.out.println("Reservation Codes: 0000");
        System.out.println("TCP Length: "+ TCPPacket.length()/2);
        System.out.println("-----------------End Pseudo Header----------------");
        System.out.println("-------------------TCP    Header------------------");
        System.out.println("Source Post: "+SourcePort);
        System.out.println("Destination Port: "+DestPort);
        System.out.println("Sequence Number: "+Sequence);
        System.out.println("ACK Sequence Number: "+ACKSeq);
        System.out.println("TCP Header Length: "+headerLength);
        System.out.println("flags:");
        System.out.println("\tURG: "+URG);
        System.out.println("\tACK: "+ACK);
        System.out.println("\tPSH: "+PSH);
        System.out.println("\tRST: "+RST);
        System.out.println("\tSYN: "+SYN);
        System.out.println("\tFIN: "+FIN);
        System.out.println("Window Size: "+WindowSize);
        System.out.println("Checksum: "+CheckSum);
        System.out.println("Emergency: "+Emergency);
        System.out.println("Option: "+Option);
        System.out.println("Data: "+Data);
        System.out.println("------------------End TCP Header------------------");

    }

    @Test
    private void checksumtest() {
        PrintTCPInfo();
        System.out.println(CalculateChecksum());
    }
}
