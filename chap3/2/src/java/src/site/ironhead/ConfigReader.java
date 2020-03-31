package site.ironhead;

import java.io.IOException;
import java.io.InputStream;
import java.util.Enumeration;
import java.util.Properties;


public class ConfigReader {
    Properties properties;
    ConfigReader(String path) throws IOException{
        try (InputStream inputstream = this.getClass().getClassLoader().getResourceAsStream(path)) {
//            System.out.println("Loading the config: " + path);
            this.properties = new Properties();
            assert inputstream != null;
            properties.load(inputstream);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void PrintConfig(){
        Enumeration enumeration=properties.propertyNames();//取得配置文件里所有的key值
        while(enumeration.hasMoreElements()){
            String key=(String) enumeration.nextElement();
            System.out.println(key+"："+properties.getProperty(key));//输出key值
        }
    }

    public String GetValue(String key){
        return this.properties.getProperty(key);
    }

}
