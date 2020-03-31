import java.io.FileNotFoundException;
import java.io.IOException;

public class TestCRC {
	
	public static String fixlength(String string, int len)
	{
	    String nwString = "";
		for(int i=0; i<len - string.length(); i++)
	    {
	    	nwString = nwString + "0";
	    }
		return nwString + string;
	}
	
	public static void main(String[] args) throws FileNotFoundException, IOException {
		GetData getData = new GetData();
		getData.readdata();
		CRC crc = new CRC();
		int len = getData.send.length();
		
		int res = crc.CRC16(crc.change(getData.send));
		int res2 = crc.CRC16(crc.change(getData.receive));
		String solveSend = crc.append(getData.send, res);
		String solveReceive = crc.append(getData.receive, res);
		int res3 = crc.CRC16(crc.change(solveReceive));

		System.out.println("发送的二进制比特串：" + getData.send);
		System.out.println("CRC-CCITT生成式对应二进制比特串：" + '1' + fixlength(Integer.toBinaryString(crc.gen), 16));
		System.out.println("计算得到循环冗余校验码CRC-Code：" + fixlength(Integer.toBinaryString(res), 16));
		System.out.println("带校验和的发送帧：" + fixlength(solveSend, len + 16));
		
		System.out.println();
		System.out.println("接收的二进制比特串：" + getData.receive);
		System.out.println("计算得到循环冗余校验码CRC-Code：" + fixlength(Integer.toBinaryString(res2), 16));
		System.out.println("计算得到的余数：" + fixlength(Integer.toBinaryString(res3), 16));

		if (res3 == 0) {
			System.out.println("校验无错");
		} else {
			System.out.println("校验出错");
		}

	}
}
