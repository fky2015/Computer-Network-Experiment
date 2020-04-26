/**
* @author 袁瑞泽
* @version 创建时间：2020年4月24日 下午3:26:24
* 类说明:传输数据读入
*/
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;

public class ReadFile {
	public int MAX_LEN = 1024;
	public int MAX_FRAME = 1024;
	public Frame[] frames = new Frame[MAX_FRAME];
	public int id = 0;
	public int totframe = 0;
	
	public void readfile(String filename) {
		InputStream inputStream = null;
		byte[] tmp = new byte[MAX_LEN - 5];//后5位保留给校验ack等值
		int bytecnt = 0;
		int framecnt = 0;
		try {
			inputStream = new FileInputStream(filename);
			while((bytecnt = inputStream.read(tmp)) != -1)
			{	
//				System.out.write(tmp, 0, bytecnt);
				frames[framecnt] = new Frame();
				System.arraycopy(tmp, 0, frames[framecnt].data, 0, bytecnt);
				frames[framecnt].len = bytecnt;
				frames[framecnt].index = framecnt;
				framecnt++;
//				System.out.println(framecnt);
			}
			totframe = framecnt;
			System.out.println("待发送的总数据帧数:" + totframe);
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			if (inputStream != null) {
				try {
					inputStream.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
		
	}
	
}
