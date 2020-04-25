import org.junit.Test;

/**
* @author 袁瑞泽
* @version 创建时间：2020年4月24日 下午3:56:09
* 类说明:循环冗余校验
*/
public class CRC {
	int gen = 0x1021;
	int crc = 0x0000;
	
	public int CRC16(byte[] data, int len) {
		crc = 0x0000;
		for (int i = 0; i < len; i++) {
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
	
	@Test
	public void test() {
		byte[] data = new byte[3];
		data[0] = 0x1;
//		System.out.println(Integer.toHexString(41));
		data[1] = 4129 >> 8;
		data[2] = 4129 & 0xff;
 		System.out.println(CRC16(data, 3));
	}

}
