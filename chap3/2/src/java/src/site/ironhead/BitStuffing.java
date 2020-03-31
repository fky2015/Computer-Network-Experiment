package site.ironhead;

import java.io.IOException;
import java.security.PublicKey;

public class BitStuffing {
    private String flag;
    private String src;
    private String dest;
    BitStuffing(String path) throws IOException {
        ConfigReader cfg=new ConfigReader(path);
        this.flag=cfg.GetValue("FlagString");
        this.src=cfg.GetValue("InfoString");
        this.dest=stuffing(this.src);
    }

    BitStuffing() throws IOException {
        ConfigReader cfg=new ConfigReader("config/bitconfig.properties");
        this.flag=cfg.GetValue("FlagString");
        this.src=cfg.GetValue("InfoString");
        this.dest=stuffing(this.src);
    }

    public void showInfo()
    {
        System.out.printf("帧起始标志: %s \t 帧数据信息: %s \t 帧结束标志: %s\n",this.flag,this.src,this.flag);
    }

    public static String encode(String data){
        return data.replace("11111","111110");
    }

    public  static String decode(String data){
        return data.replace("111110","11111");
    }

    public String enframe(String data){
        return this.flag+data+this.flag;
    }

    public String deframe(String data){
        return data.substring(flag.length(),data.length()-flag.length());
    }

    public String stuffing(String data){
        return enframe(encode(data));
    }

    public String stuffing(){
        return this.dest;
    }

    public String destuffing(String data){
        return decode(deframe(data));
    }

    public String destuffing(){
        return destuffing(this.dest);
    }

}
