package site.ironhead;

public class Main {
    public static void main(String[] args) {
        TCPAnalysis tcpAnalysis=new TCPAnalysis();
        tcpAnalysis.PrintTCPInfo();
        System.out.println("The CheckSum which I calculated: "+tcpAnalysis.CalculateChecksum());
    }
}
