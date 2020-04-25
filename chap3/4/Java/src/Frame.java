/**
* @author 袁瑞泽
* @version 创建时间：2020年4月24日 下午3:29:32
* 类说明:传输帧
*/
public class Frame {
	private static int MAX_LEN = 1024; //帧的最大长度
	public int seq;					//帧的下标
	public int ack;					//确认的下标
	public int pure_ack;			//是否为纯确认帧
	public byte[] data;				//数据
	public int index;				//在数据中的下标
	public int crc;					
	public int len;					//实际数据的长度
	public Frame() {
		seq = 0;
		ack = 0;
		pure_ack = 0;
		index = 0;
		crc = 0;
		len = 0;
		data = new byte[MAX_LEN];
	}
}
