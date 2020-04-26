import java.io.IOException;
import java.util.LinkedList;
import java.util.Queue;

/**
* @author 袁瑞泽
* @version 创建时间：2020年4月24日 下午3:26:24
* 类说明:GBN协议
*/

enum EventType{
	frame_arrival,
	timeout,
	network_layer_ready,
	cksum_error,
	pure_ack,
	nothing
}

public class GBNProtocol {
	public static int MAX_SEQ = 7;
	
	public int next_frame_to_send;			//要发送的帧号
	public int ack_expected;				//期待的确认帧号
	public int frame_expected;				//期待的接收帧号
	public int buffer_cnt;					//缓冲区使用情况
	
	private int total_frame = 0;			//发送总帧数
	private int acknum = 0;					//接收到的帧数
	
	public int filter_error = 1000;          //出错模拟间隔，模拟帧校验出错
	public int filter_lost = 1000;			//丢失模拟间隔，模拟帧丢失
	private int error_offset = 2;			//设置出错偏移值
	private int lost_offset = 4;			//设置丢失偏移值
	
	public static Frame[] buffer = new Frame[MAX_SEQ+5];					//发送缓冲区
	public static Timer[] mytimers = new Timer[MAX_SEQ+5];					//计时器组
	public static Queue<Frame> received = new LinkedList<Frame>(); 			//接收到的帧
	public static Queue<EventType> events = new LinkedList<EventType>();	//消息队列
	
	public static boolean timeout_flag = false;		//超时标记
	public static boolean finish_send = false;		//发送完成标记
	public static boolean finish_receive = false;		//接受完成标记
	
	public static int file_len;						//接收文件的帧数
	public ReadFile input_file;						//读取文件
	public WriteFile output_file;					//输出文件
	public static CRC crc;							//CRC检验
	public String output_name;						//输出文件名
	public Sender sender;							//发送类
	public ReceiveThread receiver;					//接收线程
	
	public GBNProtocol(int rport, int sport, String outputname) throws IOException {
		
		next_frame_to_send = 0;
		ack_expected = 0;
		frame_expected = 0;
		buffer_cnt = 0;
		
		input_file = new ReadFile();
		output_file = new WriteFile(outputname);
		output_name = outputname;
		crc = new CRC();
		sender = new Sender(sport, rport);
				
	}
	
	//执行函数
	public void execute(int rport, int sport, String inputname) throws IOException, InterruptedException {
		receiver = new ReceiveThread(sport, rport, output_name);
		receiver.start();
		input_file.readfile(inputname);
		sendlen();
		protocol();
	}
	
	//加一
	public int inc(int x) {
		x = (x + 1) % (MAX_SEQ + 1);  
		return x;
	}
	
	//判断b是否在a，c之间
	private boolean between(int a,int b,int c) {
		if(((a <= b) && (b < c)) 
				|| ((a <= b) && (c < a)) 
				|| ((c < a) && (b < c))) {
			return true;
		}
		return false;
	}
	
	//获取当前消息状态
	public EventType waitforevent() {
		//消息队列为空，说明网络层准备好接收消息
		if (events.size() == 0) {
			return EventType.network_layer_ready;
		}else {
			return events.poll();
		}
	}
	
	//添加事件到消息队列
	public static synchronized void addevent(EventType event) {
		events.add(event);
	}
	
	//设置超时标记，0直接返回该标记的值，1设置超时，2设置不超时
	public static synchronized boolean settimeout(int key) {
		switch (key) {
		case 0:
			return timeout_flag;
			
		case 1:
			timeout_flag = true;
			break;
			
		case 2:
			timeout_flag = false;
			break;

		default:
			break;
		}
		
		return timeout_flag;
		
	}
	
	//将待发送帧中的标记变量加到发送数据data后
	public static Frame getframetosend(Frame sframe) {
		sframe.data[sframe.len] = (byte)(sframe.seq & 0xff);
		sframe.data[sframe.len + 1] = (byte)(sframe.ack & 0xff);
		sframe.data[sframe.len + 2] = (byte)(sframe.pure_ack & 0xff);
		int check = crc.CRC16(sframe.data, sframe.len + 3);
		sframe.data[sframe.len + 4] = (byte)(check & 0xff);
		sframe.data[sframe.len + 3] = (byte)((check >> 8) & 0xff);
		sframe.len += 5;
		return sframe;
	}
	
	// 将接收帧中的标记变量从数据data后提取
	public static Frame getframefromrec(Frame rframe) {
		rframe.crc = (int) rframe.data[rframe.len - 2];
		rframe.crc <<= 8;
		rframe.crc = rframe.crc & (rframe.data[rframe.len - 1] & 0xff);
		rframe.pure_ack = rframe.data[rframe.len - 3] & 0xff;
		rframe.ack = rframe.data[rframe.len - 4] & 0xff;
		rframe.seq = rframe.data[rframe.len - 5] & 0xff;
		rframe.len -= 5;
		rframe.data[rframe.len] = 0x00;
		return rframe;
	}
	
	//GBN协议实现
	public void protocol() throws InterruptedException, IOException {
		EventType event = EventType.nothing;
		while (true) {
			if (finish_receive && finish_send) {
				output_file.writeOut();
				break;
			}
			
			event = waitforevent();
			System.out.println("----------" + event + "----------");
			
			switch (event) {
			case network_layer_ready:
				if (buffer_cnt >= MAX_SEQ) {
					Thread.sleep(1000);
				}else if (input_file.id == input_file.totframe) {
					//最后一帧已发送完毕，只发送确认帧，用于确定自己接收完成
					
					System.out.println("发送纯确认帧");
					System.out.println("发送帧的seq:" + ack_expected);
					System.out.println("发送帧的ack:" + next_frame_to_send);
					System.out.println("当前期待帧:" + frame_expected);
					Frame pure_ack = new Frame();
					pure_ack.seq = next_frame_to_send;
					pure_ack.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1);
					pure_ack.pure_ack = 1;
					pure_ack.len = 0;
					pure_ack.index = -1;
					buffer[next_frame_to_send] = pure_ack;
					sender.senddata(pure_ack);
					next_frame_to_send = inc(next_frame_to_send);
					buffer_cnt++;
					
					finish_send = true;
				}else {
					//发送下一数据帧
					System.out.println("发送帧的ack:" + ack_expected);
					System.out.println("发送帧的seq:" + next_frame_to_send);
					System.out.println("当前期待帧:" + frame_expected);
					buffer[next_frame_to_send] = input_file.frames[input_file.id];
					buffer_cnt++;
					input_file.id++;
					senddata(buffer[next_frame_to_send]);
					next_frame_to_send = inc(next_frame_to_send);
				}
				break;
			
			case frame_arrival:
				Frame cur_frame = received.poll();
				System.out.println("接收帧的seq:"+cur_frame.seq);
				System.out.println("接收帧的ack:"+cur_frame.ack);
				System.out.println("当前期待帧:"+frame_expected);
				
				//是当前需要接收帧
				if (cur_frame.seq == frame_expected) {
					//判断是否为第一帧，使用第一帧获得完整数据帧数
					if (acknum == 0) {
						int len = 0;
						for (int i = 3; i >= 0; i--) {
							int tmp = 0;
							tmp = (cur_frame.data[i] & 0xff) << (8*i);
							len += tmp;
						}
						file_len = len;
						frame_expected = inc(frame_expected);
						acknum++;
						System.out.println("待接收数据总长度为：" + file_len);
					}
					//正常数据帧接收
					else {
						frame_expected = inc(frame_expected);
						output_file.getData(cur_frame);
						acknum++;
//						System.out.println("acknum:"+acknum);
//						System.out.println("fillen:"+file_len);
					}
					
					//接受完成设置接受完成标记为true
					if(acknum == file_len) {
						finish_receive = true;
					}
					
				}
				
				//对方已接收该帧，停止对应帧的计时器
				while(between(ack_expected, cur_frame.ack, next_frame_to_send)) {
					buffer_cnt--;
					mytimers[ack_expected].timerStop();
					ack_expected = inc(ack_expected);
				}
				
				break;
				
			case cksum_error:
				break;
				
			case timeout:
				next_frame_to_send = ack_expected;
				System.out.println("超时重发");
				for(int i=1;i<=buffer_cnt;i++) {
					System.out.println("--------------------");
//					System.out.printf("ack_expected:%d  next_frame:%d  frame_expected:%d\n", ack_expected,next_frame_to_send,frame_expected);
					senddata(buffer[next_frame_to_send]);
//					System.out.println("");
					next_frame_to_send = inc(next_frame_to_send);
				}
				settimeout(2);//timeoutflag = false
				break;
				
			case pure_ack:
				//纯确认帧接收，不包含数据
				Frame pure_frame = received.poll();
				System.out.println("接收帧的seq:"+pure_frame.seq);
				System.out.println("接收帧的ack:"+pure_frame.ack);
				System.out.println("当前期待帧:"+frame_expected);
				if(pure_frame.seq == frame_expected) {
					frame_expected = inc(frame_expected);
					finish_send = true;
				}
				
				//对方已接收该帧，停止对应帧的计时器
				while(between(ack_expected, pure_frame.ack, next_frame_to_send)) {
					buffer_cnt--;
					mytimers[ack_expected].timerStop();
					ack_expected = inc(ack_expected);
				}
				
				
			default:
				break;
			}
			
			
		}
		
		System.out.println("----------" + "传输并接收完成！" + "----------");
	}
	
	//正常帧的传输
	public void senddata(Frame frame) throws IOException {
		System.out.println("当前帧数据中下标：" + frame.index);
//		System.out.println("当前帧的全局下标：" + total_frame);
		Frame s = new Frame();
		s.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1);
		s.seq = next_frame_to_send;
		s.pure_ack = frame.pure_ack;
		s.len = frame.len;
		System.arraycopy(frame.data, 0, s.data, 0, frame.len);
		System.out.println("正在发送帧seq:"+s.seq);
		System.out.println("正在发送帧ack:"+s.ack);
		
		//模拟丢失，不发送帧，只启动计时器
		if((lost_offset + total_frame) % filter_lost == 0 && s.pure_ack == 0){
			System.out.println("模拟丢失");
			mytimers[next_frame_to_send] = new Timer();
			mytimers[next_frame_to_send].start();
			total_frame++;
			finish_send = false;
			return;
		}
		//模拟校验出错，修改检验值
		else if((error_offset + total_frame) % filter_error == 0) {
			System.out.println("模拟出错");
			Frame error_frame = new Frame();
			error_frame = getframetosend(s);
			error_frame.data[s.len-1] = 
					(byte) ((error_frame.data[s.len-1] + 1) & 0xff);
			sender.senddata(error_frame);
			mytimers[next_frame_to_send] = new Timer();
			mytimers[next_frame_to_send].start();
			total_frame++;
			finish_send = false;
			return;
		}
		//正常传输
		else {
			sender.senddata(getframetosend(s));
			mytimers[next_frame_to_send] = new Timer();
			mytimers[next_frame_to_send].start();
			total_frame++;
		}
		
	}
	
	//第一帧长度帧的传输
	public void sendlen() throws IOException {
		int cur = input_file.totframe + 1;//包括第一帧长度帧，加1
		Frame first_frame = new Frame();
		first_frame.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1);
		first_frame.seq = next_frame_to_send;
		first_frame.pure_ack = 0;
		first_frame.len = 4;
		first_frame.index = -1;
//		System.out.println(cur);
		
		for(int i = 0; i < 4; i++) {
			first_frame.data[i] = (byte)(cur & 0xff);
			cur = cur >> 8;
		}
//		System.out.printf("%x %x %x %x\n", first_frame.data[3],first_frame.data[2],first_frame.data[1],first_frame.data[0]);
		
		buffer[next_frame_to_send] = first_frame;	
		//cur_file.index += 1;
		senddata(buffer[next_frame_to_send]);
		next_frame_to_send = inc(next_frame_to_send);
		buffer_cnt++;
	}
	
}
