package site.ironhead;

import java.io.IOException;

public class ByteStuffing {
    private static String ESC="10";
    private String flag;
    private String src;
    private String dest;
    ByteStuffing(String path) throws IOException {
        ConfigReader cfg=new ConfigReader(path);
        cfg.PrintConfig();
        this.flag=cfg.GetValue("FlagString");
        this.src=cfg.GetValue("InfoString");
        this.dest=stuffing(this.src);
    }

    ByteStuffing() throws IOException {
        ConfigReader cfg=new ConfigReader("config/byteconfig.properties");
        cfg.PrintConfig();
        this.flag=cfg.GetValue("FlagString");
        this.src=cfg.GetValue("InfoString");
        this.dest=stuffing(this.src);
    }
    public void showInfo()
    {
        System.out.printf("帧起始标志: %s \t 帧数据信息: %s \t 帧结束标志: %s\n",this.flag,this.src,this.flag);
    }

    public String encode(String data){
        String code="";
        assert data.length() %2 == 0;
        for (int i=0;i<data.length();i+=2) {
            String tmp=data.substring(i,i+2);
            if (tmp.equals(ESC) || tmp.equals(this.flag)) {
                code+=ESC;
            }
            code+=tmp;
        }
        return code;
    }

    public String decode(String data){
        String code="";
        assert data.length() %2 == 0;
        for (int i=0;i<data.length();i+=2) {
            String tmp=data.substring(i,i+2);
            if (tmp.equals(ESC)) {
                i+=2;
                tmp=data.substring(i,i+2);
            }
            code+=tmp;
        }
        return code;
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
