/**
* @author 袁瑞泽
* @version 创建时间：2020年4月22日 下午4:08:41
* 类说明：距离矢量（DV）算法
*/

import java.io.*;
import java.util.ArrayList;
import java.util.Scanner;

class node{
    int dis;
    int next_hop ;
    Boolean neighbor = false;
}

public class Dv{
    static node[][] mp = new node[100][100];
    static int tot;//节点数

    public static void readFile(String path) {
		try {
			FileReader fileReader = new FileReader(path);
			Scanner sc = new Scanner(fileReader);
			tot = sc.nextInt();
			for (int i = 0; i < tot; i++) {
				for (int j = 0; j < tot; j++) {
					int x = sc.nextInt();
					node tmp = new node();
					tmp.dis = x;
					tmp.next_hop = j;
					if (x < 16) {
						tmp.neighbor = true;
					}
					mp[i][j] = tmp;
				}
			}
			sc.close();
			
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}
    
    //通过y更新x
    public static int updatepair(int x, int y) {
		int cnt = 0;
		
		for (int i = 0; i < tot; i++) {
			if (mp[x][i].next_hop == y) {
				int dis = mp[x][y].dis + mp[y][i].dis;
				if (dis > 16) {
					dis = 16;
				}
				if (dis != mp[x][i].dis) {
					System.out.println("update " + getname(x) + " to " + getname(i)
					+ " through " + getname(y));
					mp[x][i].dis = dis;
					mp[x][i].next_hop = y;
					cnt++;
				}
			}else if (mp[x][y].neighbor) {
				int dis = mp[x][y].dis + mp[y][i].dis;
				if (dis > 16) {
					dis = 16;
				}
				if (dis < mp[x][i].dis) {
					System.out.println("update " + getname(x) + " to " + getname(i)
					+ " through " + getname(y));
					mp[x][i].dis = dis;
					mp[x][i].next_hop = y;
					cnt++;
				}
			}
		}
		
		return cnt;
	}
    
    public static void update() {
		boolean isupdate = true;
		int cnt = 0;
		while (isupdate) {
			isupdate = false;
			for (int i = 0; i < tot; i++) {
				for (int j = 0; j < tot; j++) {
					//判断i，j是否为邻居
					if (i == j) {
						continue;
					}
					int tmp = updatepair(i, j);
					if (tmp > 0) {
						isupdate = true;
						cnt += tmp;
					}
				}
			}
		}
		System.out.println("update " + cnt + " times");
		
	}
    
    public static void output(int x) {
		System.out.println("router : " + getname(x));
		System.out.println("destination	cost		next-hop");
		for (int i = 0; i < tot; i++) {
			System.out.println(getname(i) + "		" + mp[x][i].dis + "		" + getname(mp[x][i].next_hop));
		}
	}
    
    public static void change() throws IOException {
    	System.out.println("请输入修改的源路由器，目标路由器，及修改距离：");
    	BufferedReader bf = new BufferedReader(new InputStreamReader(System.in));
		String ststr = bf.readLine();
		String[] tmp = ststr.split(" ");
		ststr = tmp[0];
		String enstr = tmp[1];
		int st = ststr.charAt(0) - 'A';
		int en = enstr.charAt(0) - 'A';
		int dis = Integer.parseInt(tmp[2]);
		mp[en][st].dis = mp[st][en].dis = dis;
	}
    
    public static void main(String[] args) throws NumberFormatException, IOException {
		readFile("in.txt");
		System.out.println("初始路由表");
		for (int i = 0; i < tot; i++) {
			output(i);
		}
		System.out.println("更新后路由表");
		update();
		for (int i = 0; i < tot; i++) {
			output(i);
		}
		int flag = 1;
		BufferedReader bf = new BufferedReader(new InputStreamReader(System.in));
		while (flag != 0) {
			System.out.println("请输入操作编号：");
			System.out.println("0：退出");
			System.out.println("1：更新网络拓扑");
			System.out.println("2：输出指定路由器表");
			System.out.println("3：更新路由器表");
			System.out.println("4：输出所有路由器表");
			
			String input = bf.readLine();
			flag = Integer.parseInt(input);
			
			switch (flag) {
			case 0:
				break;
			case 1:
				change();
				break;
			case 2:
				System.out.println("请输入需要查看的路由器编号：");
				String str = bf.readLine();
				int id = str.charAt(0) - 'A';
				output(id);
				break;
			case 3:
				System.out.println();
				update();
				break;
			case 4:
				for (int i = 0; i < tot; i++) {
					output(i);
				}
				break;
				
			default:
				break;
			}
		}
		
	}
    
    public static String getname(int x) {
		return Character.toString((char) ('A' + x));
	}

}

