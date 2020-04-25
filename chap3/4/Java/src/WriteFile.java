import java.io.FileOutputStream;
import java.io.IOException;

/**
* @author 袁瑞泽
* @version 创建时间：2020年4月25日 上午10:11:40
* 类说明:输出帧类
*/
public class WriteFile {
	private static final int MAX_LEN = 1024*1024;
	public static byte[] rec = new byte[MAX_LEN];
	private int cur_len = 0;
	private String outname;
	
	public WriteFile(String out_file) {
		outname = out_file;
	}
	
	//将当前帧的数据附加到写出缓冲区后面
	public void getData(Frame cur_frame) {
		System.arraycopy(cur_frame.data, 0, rec, cur_len, cur_frame.len);
		cur_len += cur_frame.len;
	}
	
	//将全部接收内容输出到文件
	public void writeOut() throws IOException{
		FileOutputStream fileOutputStream = new FileOutputStream(outname);
		fileOutputStream.write(rec,0,cur_len);
		fileOutputStream.close();
	}
}
