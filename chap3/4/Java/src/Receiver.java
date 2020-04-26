import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

/**
* @author 袁瑞泽
* @version 创建时间：2020年4月24日 下午9:16:15
* 类说明:接收类
*/
public class Receiver {
	int send_port;
	int rec_port;
	InetAddress inetAddress;
	private DatagramSocket socket;
	private int MAX_LEN = 1024;
	WriteFile output_file;
	int rec_cnt = 0;
	
	public Receiver(int sport, int rport, String outname) throws IOException {
		socket = new DatagramSocket(rport);
		send_port = sport;
		rec_port = rport;
		inetAddress = InetAddress.getByName("127.0.0.1");
		System.out.println("接收端口为:" + rport);
		output_file = new WriteFile(outname);
	}
	
	public void rec_len() throws IOException {
		byte[] rec_buf = new byte[MAX_LEN];
		DatagramPacket rec_data = new DatagramPacket(rec_buf, MAX_LEN);
		socket.receive(rec_data);
		int cur = 0;
		byte[] file_len = rec_data.getData();
		System.out.printf("接收的总数据长度为：%x %x %x %x\n", 
				file_len[3],
				file_len[2],
				file_len[1],
				file_len[0]);
		for(int i=3;i>=0;i--) {
			int cbuf = 0;
			cbuf = file_len[i] & 0xff;
			cur = cur | cbuf;
			cur = cur << 8;
		}
		GBNProtocol.file_len = cur;
	}
	
	public void rec_data() throws IOException {
		byte[] rec_buf = new byte[MAX_LEN];
		DatagramPacket rec_data = new DatagramPacket(rec_buf, MAX_LEN);
		while(true) {
			if(GBNProtocol.finish_send && GBNProtocol.finish_receive) {
				System.out.println("文件接收完毕，关闭监听");
				socket.close();
				break;
			}
			socket.receive(rec_data);
			byte data[] = rec_data.getData();
			int data_len = rec_data.getLength();
			Frame cur_frame = new Frame();
			System.arraycopy(data, 0, cur_frame.data, 0, data_len);
			cur_frame.len = data_len;
			int check = GBNProtocol.crc.CRC16(cur_frame.data, cur_frame.len);
			
			System.out.println("----------" + "侦听到数据" + "----------");
			//检查接受结果是否正确
			System.out.printf("第%d帧的校验结果为:%d\n", rec_cnt, check);
			rec_cnt ++;
			if(check == 0) {
				//如果正确，判断是否是纯确认帧
				Frame rec = GBNProtocol.getframefromrec(cur_frame);
				GBNProtocol.received.add(rec);
				
				if(rec.pure_ack == 1){
					GBNProtocol.addevent(EventType.pure_ack);
				}
				else{
					GBNProtocol.addevent(EventType.frame_arrival);
				}
			}
			//否则返回校验错误
			else {
				GBNProtocol.addevent(EventType.cksum_error);
			}
		}
	}
	
	
}
