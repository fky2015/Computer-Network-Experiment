import java.io.IOException;

/**
* @author 袁瑞泽
* @version 创建时间：2020年4月25日 下午12:47:05
* 类说明:接收线程
*/
public class ReceiveThread extends Thread{
	public Receiver receiver;
	
	public ReceiveThread(int sport, int rport, String outname) throws IOException {
		receiver = new Receiver(sport, rport, outname);
	}
	
	public void run() {
		try {
			receiver.rec_data();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
