import java.nio.charset.Charset;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Scanner;

/**
 * MD5
 */
public class MD5 {

    public static byte[] toHex (String a) {
        byte[] res = new byte[a.length()];

        for (int i = 0; i < a.length(); i++) {
            char nowCh = a.charAt(i);
            res[i] = (byte) nowCh;
        }
        return res;
    }

    public static void main(String[] args) throws NoSuchAlgorithmException {
        MessageDigest m = MessageDigest.getInstance("MD5");
        System.out.println("please enter the password");
        Scanner in = new Scanner(System.in);
        String psw = in.nextLine();
        System.out.println(String.format("the password is :%s", psw));
        String rdmNum = Integer.toString((int) (Math.random() * 1000000000));
        System.out.println(String.format("the random number is %s", rdmNum));
        in.close();
        
        String challenge = psw + rdmNum;
        m.update(toHex(challenge));
        byte[] res = m.digest();
        System.out.print("The MD5 is: ");
        for (byte b : res) {
            System.out.print(String.format("%x", b));
        }
        System.out.println("");
    }
}