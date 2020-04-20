import numpy as np
#构造有向Graph
class Graph:
    def __init__(self, graph, labels=None):  #labels为标点名称
        if labels is None:
            labels = [i for i in range(graph.shape[0])]
        self.Arcs=graph
        self.VertexNum=graph.shape[0]
        self.labels=labels

    def Dijkstra(self,Vertex,EndNode):  #Vertex为源点，EndNode为终点
        Dist=[[] for i in range(self.VertexNum)] #存储源点到每一个终点的最短路径的长度
        Path=[[] for i in range(self.VertexNum)] #存储每一条最短路径中倒数第二个顶点的下标
        flag=[[] for i in range(self.VertexNum)] #记录每一个顶点是否求得最短路径
        index=0
        #初始化
        while index<self.VertexNum:
            Dist[index]=self.Arcs[Vertex][index]
            flag[index]=0
            if self.Arcs[Vertex][index]<float('inf'):  #正无穷
                Path[index]=Vertex
            else:
                Path[index]=-1 #表示从顶点Vertex到index无路径
            index+=1
        flag[Vertex]=1
        Path[Vertex]=0
        Dist[Vertex]=0
        index=1
        while index<self.VertexNum:
            MinDist=float('inf')
            j=0
            while j<self.VertexNum:
                if flag[j]==0 and Dist[j]<MinDist:
                    tVertex=j  #tVertex为目前从V-S集合中找出的距离源点Vertex最断路径的顶点
                    MinDist=Dist[j]
                j+=1
            flag[tVertex]=1
            EndVertex=0
            MinDist=float('inf') #表示无穷大，若两点间的距离小于MinDist说明两点间有路径
            #更新Dist列表，符合思想中第三条
            while EndVertex<self.VertexNum:
                if flag[EndVertex]==0:
                    if self.Arcs[tVertex][EndVertex]<MinDist and Dist[
                        tVertex]+self.Arcs[tVertex][EndVertex]<Dist[EndVertex]:
                        Dist[EndVertex]=Dist[tVertex]+self.Arcs[tVertex][EndVertex]
                        Path[EndVertex]=tVertex
                EndVertex+=1
            index+=1
        vertex_endnode_path=[] #存储从源点到终点的最短路径
        return Dist[EndNode],start_end_Path(Path,Vertex,EndNode,vertex_endnode_path)
    #根据本文上述定义的Path递归求路径
def start_end_Path(Path,start,endnode,path):
    if start==endnode:
        path.append(start)
    else:
        path.append(endnode)
        start_end_Path(Path,start,Path[endnode],path)
    return path

if __name__=='__main__':
    #float('inf')表示无穷
    graph=np.array([[0,6,5,float('inf'),float('inf'),float('inf')],
                    [float('inf'),0,2,8,float('inf'),float('inf')],
                    [float('inf'),float('inf'),0,float('inf'),3,float('inf')],
                    [float('inf'),float('inf'),7,0,float('inf'),9],
                    [float('inf'),float('inf'),float('inf'),float('inf'),0,9],
                    [float('inf'),float('inf'),float('inf'),float('inf'),0]])
    G=Graph(graph,labels=['a','b','c','d','e','f'])
    start=input('请输入源点')
    endnode=input('请输入终点')
    dist,path=G.Dijkstra(G.labels.index(start),G.labels.index(endnode))
    Path=[]
    for i in range(len(path)):
        Path.append(G.labels[path[len(path)-1-i]])
    print('从顶点{}到顶点{}的最短路径为：\n{}\n最短路径长度为：{}'.format(start,endnode,Path,dist))
