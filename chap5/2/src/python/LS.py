
from configure import configure
from dijkstra import Graph
import numpy as np
if __name__ == '__main__':
    conf=configure()
    graph=Graph(graph=conf.dis)
    num=graph.VertexNum
    table=np.zeros(shape=[num,num],dtype=int)

    for i in range(num):
        for j in range(num):
            dist,path=graph.Dijkstra(i,j)
            Path = []
            for t in range(len(path)):
                Path.append(graph.labels[path[len(path) - 1 - t]])
            if (len(Path)>1):
                table[i][j]=Path[1]
            else:
                table[i][j]=Path[0]
            print('从节点{}到节点{}的最短路径为：\n{}\n最短路径长度为：{}'.format(i, j, Path, dist))
    # print(table)
    for i in range(num):
        print('节点{}的路由表为：'.format(i))
        print('-------------------------------------------------------')
        for j in range(num):
            if i==j:
                continue
            else:
                print('到节点{}\t转发给节点{}'.format(j,table[i][j]))
        print('-------------------------------------------------------')
