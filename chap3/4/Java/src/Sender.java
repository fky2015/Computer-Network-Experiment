import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

/**
* @author 袁瑞泽
* @version 创建时间：2020年4月24日 下午9:16:08
* 类说明:发送类
*/
public class Sender {
	int send_port;
	int rec_port;
	InetAddress inetAddress;
	private DatagramSocket socket;
	private int MAX_LEN = 1024;
	
	public Sender(int sport, int rport) throws IOException {
		socket = new DatagramSocket();
		send_port = sport;
		rec_port = rport;
		inetAddress = InetAddress.getByName("127.0.0.1");
		System.out.println("发送端口为:" + sport);
	}
	
	//发送数据帧
	public void senddata(Frame sframe) throws IOException {
		DatagramPacket datasend = new DatagramPacket(sframe.data, sframe.len, 
				inetAddress, send_port);
		socket.send(datasend);
	}
	
	//关闭socket
	public void closesocket() {
		socket.close();
	}
}
