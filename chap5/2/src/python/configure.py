import numpy as np
class configure:
    def __init__(self,url='distance.conf'):
        self.n=0
        self.dis=[]
        with open(url,'r') as f:
            for line in f.readlines():
                self.n+=1
                dist=[]
                for i in line.strip().split(' '):
                    if i=='' or i=='99':
                        dist.append(float('inf'))
                    else:
                        dist.append(float(i))
                self.dis.append(dist)
        self.dis=np.array(self.dis)

    def table(self):
        return self.dis

    def printState(self):
        for i in self.dis:
            for j in i:
                print(j,end='\t')
            print()



