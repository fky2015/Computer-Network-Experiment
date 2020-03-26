import gnu.io.*;

import java.io.IOException;
import java.util.List;
import java.util.Scanner;

public class Reader {

    public static void main(String args[]) 
    		throws IOException, NoSuchPortException, PortInUseException, UnsupportedCommOperationException {
    	System.loadLibrary("rxtxParallel");
    	System.loadLibrary("rxtxSerial");
    	
    	System.out.println("可用的串口有：");
    	List<String>portList = SerialCom.getPortNamList();
    	for (String string : portList) {
			System.out.println(string);
		}
    	
    	SerialPort serialPort;
    	
    	Scanner scanner = new Scanner(System.in);
    	while(true) {
    		System.out.println("请选择连接接口：");
    		int index = scanner.nextInt();
    		if (index <= portList.size()) {
				serialPort = SerialCom.openPort(portList.get(index - 1));
				break;
			}
    		else {
				System.out.println("请输入正确的编号：");
			}
    	}
    	
    	
    	while (true) {
                byte[] bytes = SerialCom.readData(serialPort);
                if (bytes != null) {
                     System.out.println("接收到数据：" + new String(bytes));
				}
		}
	}
}

