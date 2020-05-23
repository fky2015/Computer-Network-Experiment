package site.ironhead;

import java.io.DataInputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class TCPReceiver {
    private final int port;
    public TCPReceiver() {
        port=8121;
    }

    public TCPReceiver(int port) {
        this.port = port;
    }

    public void serve() throws IOException {
        System.out.println("Server Start...");
        ServerSocket server = new ServerSocket(port);
        try (server; Socket socket = server.accept()) {
            DataInputStream inputStream = new DataInputStream(socket.getInputStream());
            while (true) {
//                String bytes = inputStream.readUTF();
                inputStream.readByte();
            }
        }
    }

    public static void main(String[] args) throws IOException {
        new TCPReceiver().serve();
    }
}
