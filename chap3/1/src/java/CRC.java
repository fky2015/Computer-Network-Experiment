public class CRC {
	int gen = 0x1021;
	int crc = 0x0000;
	
	public String fixlength(String string, int len)
	{
	    String nwString = "";
		for(int i=0; i<len - string.length(); i++)
	    {
	    	nwString = nwString + "0";
	    }
		return nwString + string;
	}

	public int CRC16(byte[] data) {
		crc = 0x0000;
		for (int i = 0; i < data.length; i++) {
			// 每次读取一个字节
			crc ^= (data[i] << 8);
			for (int j = 0; j < 8; j++) {
				/**
				 * 依次判断crc的低8位 如果为1，则左移1位后异或生成式 否则左移1位
				 */
				if ((crc & 0x8000) != 0) {
					crc = (crc << 1) ^ gen;
				} else {
					crc = crc << 1;
				}
			}
		}
		// 取crc最后16位作为余数
		return (crc & (0xffff));
	}

	public byte[] change(String string) {
		int len = string.length() / 8;
		byte[] ret = new byte[len];
		//倒序存入，低位存储高位数据
		for (int i = 0; i < len; i++) {
			int num = Integer.parseInt(string.substring(0, 8), 2);
			ret[i] = (byte) (num & 0xff);
			string = string.substring(8);
		}
		return ret;
	}
	
	public String append(String string, int res) {
		return string + fixlength(Integer.toBinaryString(res), 16);
	}

}
