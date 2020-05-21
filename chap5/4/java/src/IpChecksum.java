import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.math.BigInteger;
import java.util.Properties;

/**
* @author 袁瑞泽
* @version 创建时间：2020年4月24日 上午8:48:57
* 类说明:IP首部校验和的计算程序
*/
public class IpChecksum {
	static String ipheader;
	public static void readdata() {
		Properties properties = new Properties();
		try {
			properties.load(new FileInputStream("data.properties"));
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		ipheader = properties.getProperty("IPHeader");
	}
	
	public static byte[] StringToBytes(String s) {
	    int len = s.length();
	    byte[] bytes = new byte[len / 2];
	    for (int i = 0; i < len; i += 2) {
	        // 两位一组，表示一个字节,把这样表示的16进制字符串，还原成一个字节
	        bytes[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4) + 
	        		Character.digit(s.charAt(i + 1), 16));
	    }
	    return bytes;
	}
	
	public static String BytesToString(byte[] bytes) {
	    if (bytes == null){
	        return null;
	    }
	    char[] hexArray = "0123456789ABCDEF".toCharArray();
	    char[] hexChars = new char[bytes.length * 2];
	    for (int j = 0; j < bytes.length; j++) {
	        int v = bytes[j] & 0xFF;
	        hexChars[j * 2] = hexArray[v >> 4];
	        hexChars[j * 2 + 1] = hexArray[v & 0x0F];
	    }
	    return new String(hexChars);
	}
	
	public static String CalChecksum(String ipheader) {
		byte[] checksum = new byte[2];
		
		byte[] ipbytes = StringToBytes(ipheader);
		
		long sum = 0;
		//16位为一组，求和
		for (int i = 0; i < 20; i++) {
			//原第11,12个字节校验和
			if (i == 10 || i == 11) {
//				System.out.println(ipbytes[i]);
				continue;
			}
			long tmp = 0;
			if (ipbytes[i] < 0) {
				tmp = 256 + ipbytes[i];
			}
			else {
				tmp = ipbytes[i];
			}
			if (i % 2 == 0) {
				tmp *= 16*16;
			}
			sum += tmp;
		}
		
		//结果大于2字节
		sum = (sum >> 16) + (sum & 0xffff);
		sum = ~sum;
		
		checksum[0] = (byte) (sum >> 8);
	    checksum[1] = (byte) (sum & 0xff);
		
		return BytesToString(checksum);
	}
	
	public static long StringToDecimal(String str, int st, int en) {
		long ans = 0;
		for (int i = st; i <= en; i++) {
			ans *= 16;
			if (str.charAt(i) >= 'A' && str.charAt(i) <= 'Z') {
				ans += str.charAt(i) - 'A' + 10;
			}else if (str.charAt(i) >= 'a' && str.charAt(i) <= 'z') {
				ans += str.charAt(i) - 'a' + 10;
			}else {
				ans += str.charAt(i) - '0';
			}
		}
		return ans;
	}
	
	public static void showheader(String ipheader) {
		
		System.out.println("Version:" + StringToDecimal(ipheader, 0, 0));
		System.out.println("Header Length:" + StringToDecimal(ipheader, 1, 1) * 4
							+ " bytes (" + StringToDecimal(ipheader, 1, 1) + ")");
		System.out.println("Differentiated Services Field:" 
							+ StringToDecimal(ipheader, 2, 3));
		System.out.println("Total Length:" + StringToDecimal(ipheader, 4, 7));
		System.out.println("Identification:" + StringToDecimal(ipheader, 8, 11));
		long tmp = StringToDecimal(ipheader, 12, 15);
		System.out.println("Reserverd bit:" + (tmp >> 15));
		System.out.println("Don't fragment:" + (tmp >> 14));
		System.out.println("More fragments:" + (tmp >> 13));
		System.out.println("Fragment offset:" + (tmp & 0x1fff));
		System.out.println("Time to live:" + StringToDecimal(ipheader, 16, 17));
		System.out.println("Protocol:" + StringToDecimal(ipheader, 18, 19));
		System.out.println("Header checksum:" + StringToDecimal(ipheader, 20, 23));
		System.out.println("Source:" + StringToDecimal(ipheader, 24, 25) 
									+ "." + StringToDecimal(ipheader, 26, 27)
									+ "." + StringToDecimal(ipheader, 28, 29)
									+ "." + StringToDecimal(ipheader, 30, 31));
		System.out.println("Destination:" + StringToDecimal(ipheader, 32, 33) 
									+ "." + StringToDecimal(ipheader, 34, 35)
									+ "." + StringToDecimal(ipheader, 36, 37)
									+ "." + StringToDecimal(ipheader, 38, 39));
	}
	
	public static void main(String[] args) {
		readdata();
		System.out.println("IP Header = " + ipheader);
		
		showheader(ipheader);
		
		String string = CalChecksum(ipheader);
		System.out.println("Calculate checksum = " + string);
		
		if (string.equals(ipheader.substring(20, 24).toUpperCase())) {
			System.out.println("校验正确");
		}else {
			System.out.println("校验出错");
		}
	}
}
