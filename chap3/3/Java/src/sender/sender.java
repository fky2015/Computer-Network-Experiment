package sender;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.concurrent.Semaphore;

import ARQ.config;
import ARQ.frame;

public class sender {

    enum mainStatus {
        UNSET, TIME_OUT, ACK
    }

    static mainStatus status;
    static Semaphore statusSem;
    static Semaphore eventCome;
    static DatagramSocket receiver;
    static DatagramSocket sender;
    static byte[] receiveBytes;
    static DatagramPacket receiveFramePacket;
    static int timerId;

    static class timer extends Thread {

        private int id;

        timer() {
            id = timerId;
        }

        @Override
        public void run() {
            try {
                sleep(config.MAX_WAITING_TIME);
                statusSem.acquire();
                if (id != timerId) {
                    statusSem.release();
                    return;
                }
                if (status == mainStatus.UNSET) {
                    status = mainStatus.TIME_OUT;
                    eventCome.release();
                }
                statusSem.release();
                return;
            } catch (Exception e) {
                System.out.println(e);
            }
        }
    }

    static class receive extends Thread {

        @Override
        public void run() {
            try {
                receiver.receive(receiveFramePacket);
                statusSem.acquire();
                if (status == mainStatus.UNSET) {
                    status = mainStatus.ACK;
                }
                eventCome.release();
                timerId++;
                statusSem.release();
            } catch (Exception e) {
                System.out.println(e);
            }
        }
    }

    //true - send rightly
    static boolean filterError() {
        if (Math.random() > (1. / config.FILTER_ERROR)) {
            return true;
        }
        return false;
    }

    //true - send rightly
    static boolean filterLost() {
        if (Math.random() > (1. / config.FILTER_LOST)) {
            return true;
        }
        return false;
    }

    public static void main(String[] args) throws IOException {
        receiver = new DatagramSocket(config.SENDER_PORT);
        sender = new DatagramSocket();
        statusSem = new Semaphore(1);
        eventCome = new Semaphore(0);

        receiveBytes = new byte[frame.MAX_BYTE];
        receiveFramePacket = new DatagramPacket(receiveBytes, receiveBytes.length);
        timerId = 0;

        FileInputStream fIn = new FileInputStream(new File(".\\static\\test.txt"));

        byte nextFrameToSend = 0;

        byte nextByte = (byte) fIn.read();
        while (nextByte != -1) {
            byte[] dataBuffer = new byte[frame.MAX_DATA_BYTE];
            byte dataLength = 0;

            for (int i = 0; i < 8; i++) {
                dataBuffer[i] = nextByte;
                if (nextByte == -1) {
                    break;
                } else {
                    dataLength++;
                    nextByte = (byte) fIn.read();
                }
            }
            if (dataLength < 8) {
                byte[] dataTemp = new byte[dataLength];
                for (int i = 0; i < dataLength; i++) {
                    dataTemp[i] = dataBuffer[i];
                }
                dataBuffer = dataTemp;
            }

            if (dataLength > 0) {
                while (true) {
                    System.out.println(String.format("next frame seq:%d", nextFrameToSend));
                    status = mainStatus.UNSET;
                    frame f = new frame((byte) 0, nextFrameToSend, dataLength, dataBuffer);
                    DatagramPacket packet = new DatagramPacket(f.toByte(), f.toByte().length,
                            InetAddress.getByName("127.0.0.1"), config.RECEIVER_PORT);
                    timer t = new timer();
                    receive r = new receive();

                    boolean sendWithoutError = filterError();
                    boolean sendWithoutLost = filterLost();
                    // sendWithoutError = true;sendWithoutLost = true;

                    if (sendWithoutError) {
                        if (sendWithoutLost) {
                            System.out.println(String.format("begin to send frame with seq:%d", nextFrameToSend));
                            sender.send(packet);
                            r.start();
                        } else {
                            System.out.println("---------- simu lost ----------\n");
                            //lost
                        }
                    } else {
                        //error
                        if (sendWithoutLost) {
                            System.out.println("---------- simu error ----------\n");
                            byte[] rawData = packet.getData();
                            rawData[3] = -1;
                            // packet = new DatagramPacket(rawData, rawData.length, InetAddress.getByName("127.0.0.1"), config.RECEIVER_PORT);
                            packet.setData(rawData);
                            sender.send(packet);
                            r.start();
                        } else {
                            System.out.println("---------- simu lost ----------\n");
                            //lost
                        }
                    }
                    t.start();

                    // wait for event
                    try {
                        eventCome.acquire();
                    } catch (Exception e) {
                        System.out.println(e);
                    }
                    if (status == mainStatus.ACK) {
                        if (receiveFramePacket.getData()[0] == 1) {
                        System.out.println(String.format("received ack frame, seq:%d", receiveFramePacket.getData()[1]));
                            if (receiveFramePacket.getData()[1] == nextFrameToSend) {
                                System.out.println(String.format("send %d successful\n", nextFrameToSend));
                                nextFrameToSend = (byte) ((nextFrameToSend + 1) % 2);
                                break;
                            }
                        } else {
                            //nak
                            System.out.println(String.format("nak received, frame error! seq:%d\n", receiveFramePacket.getData()[1]));
                        }
                    } else {
                        System.out.println("time out!\n");
                    }

                }

            }

        }

        // send an nak frame rightly.
        while (true) {
            DatagramPacket p = new DatagramPacket(
                    new frame((byte) 2, nextFrameToSend, (byte) 1, new byte[] { 0 }).toByte(), frame.BASE_BYTE + 1,
                    InetAddress.getByName("127.0.0.1"), config.RECEIVER_PORT);
            timer t = new timer();
            receive r = new receive();
            sender.send(p);
            r.start();
            t.start();

            try {
                eventCome.acquire();
            } catch (Exception e) {
                System.out.println(e);
            }
            if (status == mainStatus.ACK) {
                if (receiveFramePacket.getData()[1] == nextFrameToSend) {
                    System.out.println("send nak successful\n");
                    try {
                        // main waits for the thread
                        t.join();
                    } catch (Exception e) {
                        System.out.println(e);
                    }
                    break;
                }
            }
        }

        System.out.println("close successfully.\n");
        receiver.close();
        sender.close();
        fIn.close();

        return;
    }
}