package site.ironhead;

import javafx.util.converter.ByteStringConverter;

import java.io.*;
import java.net.Socket;
import java.util.Arrays;
import java.util.Date;
import java.util.Properties;
import java.util.Random;

public class TCPSender {
    private final int MSS;
    private int Threshold;
    private final int TriACKRound;
    private final int TimeoutRound;
    private final int EndRound;
    private final String host;
    private final int port;

    public TCPSender() throws IOException {
        Properties properties = new Properties();
        InputStream inputStream = new FileInputStream("properties/conf.properties");
        properties.load(inputStream);
        MSS = Integer.parseInt(properties.get("MSS").toString());
        Threshold = Integer.parseInt(properties.get("Threshold").toString());
        TriACKRound = Integer.parseInt(properties.get("TriACKRound").toString());
        TimeoutRound = Integer.parseInt(properties.get("TimeoutRound").toString());
        EndRound = Integer.parseInt(properties.get("EndRound").toString());
        host = "localhost";
        port = 8121;
    }

    public TCPSender(String url) throws IOException {
        Properties properties = new Properties();
        InputStream inputStream = new FileInputStream(url);
        properties.load(inputStream);
        MSS = Integer.parseInt(properties.get("MSS").toString());
        Threshold = Integer.parseInt(properties.get("Threshold").toString());
        TriACKRound = Integer.parseInt(properties.get("TriACKRound").toString());
        TimeoutRound = Integer.parseInt(properties.get("TimeoutRound").toString());
        EndRound = Integer.parseInt(properties.get("EndRound").toString());
        host = "localhost";
        port = 8121;
    }

    public void sendPacket() throws IOException {
        System.out.println("Initial MSS: "+MSS);
        System.out.println("Initial Threshold: "+Threshold);

        Socket socket = new Socket(host, port);
        DataOutputStream out = new DataOutputStream(socket.getOutputStream());
        try {
            int currentRound = 1;
            int currentSize = MSS;
            boolean aboveThreshold = false;
            while (currentRound <= EndRound) {
                System.out.println("---------------------------------------");
                if (currentSize >= Threshold && !aboveThreshold) {
                    currentSize = Threshold;
                    aboveThreshold = true;
                }
                System.out.println("Current Round: "+ currentRound);
                System.out.println("Current Congestion Window Size: "+ currentSize);
                byte[] bytes=new byte[currentSize];
                new Random(0).nextBytes(bytes);
                System.out.println("Data: "+ Arrays.toString(bytes));
                out.write(bytes);
                if (currentRound == TriACKRound || currentRound == TimeoutRound) {
                    aboveThreshold = false;
                    Threshold = currentSize / 2;
                    currentSize = MSS;
                } else {
                    if (aboveThreshold) {
                        currentSize += MSS;
                    } else {
                        currentSize *= 2;
                    }
                }
                System.out.println("---------------------------------------");
                currentRound++;
            }
        }finally {
            socket.close();
        }

    }

    public static void main(String[] args) throws IOException {
        new TCPSender().sendPacket();
    }
}
