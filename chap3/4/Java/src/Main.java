import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Properties;
import java.util.Scanner;

/**
* @author 袁瑞泽
* @version 创建时间：2020年4月25日 下午4:31:33
* 类说明:主函数类
*/
public class Main {
	
	static int sendport;
	static int recport;
	static int filter_lost;
	static int filter_error;
	static String inputfile;
	static String outputfile;
	
	public static void readdata(int num) {
		Properties properties = new Properties();
		try {
			properties.load(new FileInputStream("data" + num + ".properties"));
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		sendport = Integer.parseInt(properties.getProperty("sendport"));
		recport = Integer.parseInt(properties.getProperty("recport"));
		filter_error = Integer.parseInt(properties.getProperty("filtererror"));
		filter_lost = Integer.parseInt(properties.getProperty("filterlost"));
		inputfile = properties.getProperty("inputfile");
		outputfile = properties.getProperty("outputfile");
	}
	
	public static void main(String[] args) throws IOException, InterruptedException {
		System.out.println("请选择客户端编号1/2：");
		Scanner scanner = new Scanner(System.in);
		int num = Integer.parseInt(scanner.nextLine());
		scanner.close();
		
		readdata(num);
		
		GBNProtocol protocol = new GBNProtocol(recport, sendport, outputfile);
		protocol.filter_error = filter_error;
		protocol.filter_lost = filter_error;
		protocol.execute(recport, sendport, inputfile);
		
	}
}
