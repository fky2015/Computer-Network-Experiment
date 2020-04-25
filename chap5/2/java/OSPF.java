import java.io.File;
import java.util.Scanner;
import java.util.Arrays;

class OSPF {

  static int state[][] = new int[5][5];
  static int result[][] = new int[5][5];

  static final int n = 5;

  static void spf(int current) {
    int current_state[] = { 99, 99, 99, 99, 99 };
    boolean S[] = new boolean[n];
    S[current] = true;
    current_state[current] = 0;
    int c = current;
    for (int i = 0; i < n; i++) {
      if (state[current][i] != 99)
        result[current][i] = i;
    }
    for (int i = 0; i < n - 1; i++) {
      int min_index = -1;
      int min_dist = 99;
      for (int j = 0; j < n; j++) {
        if (current_state[c] + state[c][j] < current_state[j]) {
          current_state[j] = current_state[c] + state[c][j];
          if (c != current)
            result[current][j] = result[current][c];
        }
        if (S[j] == false && current_state[j] < min_dist) {
          min_dist = current_state[j];
          min_index = j;
        }
      }

      // print
      System.out.println(Arrays.toString(current_state));
      S[min_index] = true;
      c = min_index;

    }

  }

  public static void main(String[] args) throws Exception {

    File file = new File("config");
    Scanner sc = new Scanner(file);

    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 5; j++) {
        state[i][j] = sc.nextInt();
        result[i][j] = 99;
      }
      result[i][i] = 0;
      System.out.println(Integer.toString(i) + "号节点状态:");
      System.out.println(Arrays.toString(state[i]));
    }

    for (int i = 0; i < n; i++) {
      System.out.println("计算" + Integer.toString(i) + "的路由表");
      spf(i);
    }

    for (int i = 0; i < n; i++) {
      System.out.printf("%d 的路由表:\n", i);
      for (int j = 0; j < n; j++) {
        System.out.printf("目的网络: %d,下一跳: %d\n", j, result[i][j]);
      }
    }

    sc.close();

  }
}
