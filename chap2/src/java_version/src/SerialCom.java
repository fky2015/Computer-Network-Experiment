import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;

import gnu.io.CommPort;
import gnu.io.CommPortIdentifier;
import gnu.io.NoSuchPortException;
import gnu.io.PortInUseException;
import gnu.io.SerialPort;
import gnu.io.UnsupportedCommOperationException;

public class SerialCom {
	
	//获取串口列表
	public static List<String> getPortNamList() {
		List<String> portName = new ArrayList<>();
		Enumeration<CommPortIdentifier> portListEnumeration = 
				CommPortIdentifier.getPortIdentifiers();
		while (portListEnumeration.hasMoreElements()) {
			String commPortIdentifier = portListEnumeration.nextElement().getName();
			portName.add(commPortIdentifier);
		}
		return portName;
	}
	
	//建立串口连接
	public static SerialPort openPort(String portName) 
			throws IOException, NoSuchPortException, 
			PortInUseException, UnsupportedCommOperationException {
		Port port = new Port(portName);
		CommPortIdentifier portIdentifier = 
				CommPortIdentifier.getPortIdentifier(port.serialPortName);
		CommPort commPort = portIdentifier.open(port.serialPortName, 2000);
		
		if (commPort instanceof SerialPort) {
			SerialPort serialPort = (SerialPort) commPort;
			serialPort.setSerialPortParams(
					port.baudRate, 
					port.dataBits, 
					port.stopBits, 
					port.parity);
			System.out.println("Connect success!");
			System.out.println("Device : " + portName);
			System.out.println("Baudrate : " + port.baudRate);
			System.out.println("Databits : " + port.dataBits);
			System.out.println("Stopbits : " + port.stopBits);
			System.out.println("Parity : " + port.parity);
			return serialPort;
		}else {
			throw new NoSuchPortException();
		}
	}
	
	//关闭串口
	public static void closePort(SerialPort serialPort) {
		if (serialPort != null) {
			serialPort.close();
		}
	}
	
	//写数据
	public static void writeData(SerialPort serialPort, byte[] data) throws IOException {
		OutputStream oStream = null;
		try {
			oStream = serialPort.getOutputStream();
			oStream.write(data);
			oStream.flush();
		} finally {
			if (oStream != null) {
				oStream.close();
			}
		}
	}
	
	public static byte[] bytemerge(byte[] byte_1, byte[] byte_2) {
		byte[] byte_3 = new byte[byte_1.length + byte_2.length];
		System.arraycopy(byte_1, 0, byte_3, 0, byte_1.length);
		System.arraycopy(byte_2, 0, byte_3, byte_1.length, byte_2.length);
		return byte_3;
	}
	
	//读数据
	public static byte[] readData(SerialPort serialPort) throws IOException {
		InputStream iStream = null;
		byte[] data = null;
		try {
			iStream = serialPort.getInputStream();
			int length = iStream.available();
					
			while (length != 0) {
				byte[] data1 = new byte[length];
				iStream.read(data1);
				length = iStream.available();
				if (data != null) {
					data = bytemerge(data, data1);
				}else {
					data = data1;
				}
			}
		} finally {
			if (iStream != null) {
				iStream.close();
			}
		}
		return data;
	}
	
}
