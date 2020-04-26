package receiver;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

import ARQ.config;
import ARQ.frame;

public class receiver {
    public static void main(String[] args) throws IOException {
        DatagramSocket receiver = new DatagramSocket(config.RECEIVER_PORT);
        DatagramSocket sender = new DatagramSocket();
        int frameExpected = 0;

        FileOutputStream fOut = new FileOutputStream(new File(".\\static\\fileRec.txt"));

        while (true) {
            byte[] buffer = new byte[frame.MAX_BYTE];
            DatagramPacket p = new DatagramPacket(buffer, frame.MAX_BYTE);
            System.out.println(String.format("frame expected:%d", frameExpected));
            receiver.receive(p);

            //the length of p's data is always the MAX_BYTE!!!!!
            frame f = new frame(p.getData());

            if (f.seq == frameExpected){
                //check CRC
                frame fCheck = new frame(f.type, f.seq, f.length, f.data);

                if (f.checkSum[0] == fCheck.checkSum[0] && f.checkSum[1] == fCheck.checkSum[1]) {
                    //CRC ok
                    System.out.println(String.format("frame received rightly, seq:%d", f.seq));
                    frameExpected = (frameExpected + 1) % 2;
                    if (f.type == 2) {
                        //end transmission
                        frame frameToSend = new frame((byte) 1, f.seq, (byte) 1, new byte[]{0});
                        p = new DatagramPacket(frameToSend.toByte(), 6, InetAddress.getByName("127.0.0.1"), config.SENDER_PORT);
                        sender.send(p);
                        break;
                    } else {
                        fOut.write(f.data);
                        frame frameToSend = new frame((byte) 1, f.seq, (byte) 1, new byte[]{0});
                        System.out.println(String.format("send ack back, seq:%d\n", f.seq));
                        p = new DatagramPacket(frameToSend.toByte(), 6, InetAddress.getByName("127.0.0.1"), config.SENDER_PORT);
                        sender.send(p);
                    }
                } else {
                    // CRC error
                    System.out.println(String.format("error CRC! seq:%d\n", f.seq));
                    // send nak
                    frame nakFrame = new frame((byte) 2, f.seq, (byte) 1, new byte[]{0});
                    p = new DatagramPacket(nakFrame.toByte(), 6, InetAddress.getByName("127.0.0.1"), config.SENDER_PORT);
                    sender.send(p);
                }
            } else {
                System.out.println("unexpected frame!\n");
            }
        }
        receiver.close();
        sender.close();
        fOut.close();
    }
}