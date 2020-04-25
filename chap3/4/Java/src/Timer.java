/**
* @author 袁瑞泽
* @version 创建时间：2020年4月24日 下午3:26:24
* 类说明:计时器
*/
public class Timer extends Thread{
	
	public boolean ack = false;
	
	public void timerStart() throws InterruptedException {
		//计时器延时1s
		Thread.sleep(1000);
		//如果该计时器已停止或者协议已处于超时状态，直接返回
		if (ack || GBNProtocol.settimeout(0)) {
			return;
		}
		//否则设置超时标记
		else {
			GBNProtocol.settimeout(1);
			GBNProtocol.addevent(EventType.timeout);
		}
	}
	
	public void timerStop() {
		ack = true;
	}
	
	public void run() {
		try {
			timerStart();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}
	
}