import java.io.FileInputStream;
import java.io.IOException;
import java.util.Properties;

public class Port {
	
	public String serialPortName;
	public int baudRate;
	public int dataBits;
	public int parity;
	public int stopBits;
	
	public Port(String portName) throws IOException {
		this.serialPortName = portName;
		Properties properties = new Properties();
		properties.loadFromXML(new FileInputStream("./config.xml"));
		this.baudRate = Integer.parseInt((String) properties.get("BaudRate"));
		this.dataBits = Integer.parseInt((String) properties.get("DataBits"));
		this.stopBits = Integer.parseInt((String) properties.get("StopBits"));
		this.parity = Integer.parseInt((String) properties.get("Parity"));
	}
	
}
