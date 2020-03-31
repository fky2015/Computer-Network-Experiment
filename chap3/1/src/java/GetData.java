import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Properties;

public class GetData {
	
	public String send;
	public String receive;
	
	public void readdata() throws FileNotFoundException, IOException {
		Properties properties = new Properties();
		properties.load(new FileInputStream("data.properties"));
		send = properties.getProperty("InfoString1");
		receive = properties.getProperty("InfoString2");
	}
	
}
