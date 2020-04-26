package ARQ;

/**
 * frame
 */
public class frame {

    static public int MAX_DATA_BYTE = 8;
    static public int BASE_BYTE = 5;
    static public int MAX_BYTE = MAX_DATA_BYTE + BASE_BYTE;
    static public int HEAD_LENGTH = 3;

    public byte[] data;
    public byte seq;
    public byte type;
    public byte length;
    public byte[] checkSum;

    public byte[] toByte() {
        byte[] res = new byte[BASE_BYTE + this.length];
        int count = 0;

        res[count++] = this.type;
        res[count++] = this.seq;
        res[count++] = this.length;

        for (byte b : this.data) {
            res[count++] = b;
        }

        this.checkSum = this.createCRC();
        res[count++] = this.checkSum[0];
        res[count++] = this.checkSum[1];

        return res;
    }

    // CRC-CCITT (XModem)
    public byte[] createCRC() {
        // CRC-CCITT: 0x1021 = x16 + x12 + x5 + 1
        byte[] res = new byte[] { 0, 0 };
        int genPoly = 0x00001021;

        int now = 0x00000000;
        if (this.data.length == 0) {
            return res;
        }
        now = now | this.data[0];
        if (this.length >= 2) {
            now = now << 8;
            now = now | this.data[1];
        }

        for (int i = 2; i < this.length; i++) {
            for (int j = 7; j >= 0; j--) {
                if ((now & 0x00008000) != 0) {
                    now = (now << 1) | ((this.data[i] >> j) % 2);
                    now = now ^ genPoly;
                } else {
                    now = (now << 1) | ((this.data[i] >> j) % 2);
                }
            }
        }

        for (int i = 0; i < 16; i++) {
            if ((now & 0x00008000) != 0) {
                now = now << 1;
                now = now ^ genPoly;
            } else {
                now = now << 1;
            }
        }
        
        res[0] = (byte) (now >> 8);
        res[1] = (byte) now;
        return res;
    }

    //convert sending message to frame
    public frame(byte type, byte seq, byte len, byte[] data) {
        this.data = new byte[len];
        this.checkSum = new byte[2];
        this.type = type;
        this.seq = seq;
        this.length = len;

        for (int i = 0; i < this.length; i++) {
            this.data[i] = data[i];
        }

        this.checkSum = this.createCRC();
    }

    //convert receiving message to frame
    public frame(byte[] info) {
        this.length = info[2];
        this.data = new byte[this.length];
        this.checkSum = new byte[2];
        this.type = info[0];
        this.seq = info[1];

        for (int i = 0; i < this.length; i++) {
            this.data[i] = info[i + 3];
        }

        this.checkSum[0] = info[HEAD_LENGTH + this.length];
        this.checkSum[1] = info[HEAD_LENGTH + this.length + 1];
    }

    @Override
    public String toString() {
        return String.format("type:%d, seq:%d, length:%d, checksum:%d,%d\n", type, seq, length, checkSum[0], checkSum[1]);
    }

    public static void main(String[] args) {
        byte[] data = new byte[] {0b0110_1111, 0b0101_1010};
        frame f = new frame((byte) 1, (byte) 1, (byte) 2, data);
        System.out.println(String.format("%x %x\n", f.checkSum[0], f.checkSum[1]));
        
    }
}