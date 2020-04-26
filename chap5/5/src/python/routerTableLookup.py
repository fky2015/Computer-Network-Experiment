class RouterTableLookup:

    def __init__(self, filename):
        self.plataddrlist = []
        self.addrlist = []
        self.masklist = []
        self.nexthoclist = []
        self.filename = filename
        self.prefix_len = 0
        self.nexthoc = len(self.addrlist)
        self.checklist = []

    def print_router_table(self):

        print("\n\n当前路由表信息: ")

        f = open(self.filename)
        lines = f.readlines()

        for line in lines:
            print(line, end="")

            if line == "RoutingTable=\n":
                continue

            self.plataddrlist.append((line.split()[0].split('/')[0]).split('.'))
            self.masklist.append(bin(int(line.split()[0].split('/')[1])))
            self.nexthoclist.append(line.split()[1])

        for i in range(len(self.plataddrlist)):
            addr = "0b"
            for j in range(len(self.plataddrlist[i])):
                addr = addr + bin(int(self.plataddrlist[i][j]))[2:]

            addr = addr.ljust(32, '0')
            # print(addr)

            self.addrlist.append(addr)


    def get_matchlist(self, strdest):

        destl = strdest.split('.')
        bindest = "0b"

        for e in destl:
            bindest += bin(int(e))[2:]

        bindest = bindest.ljust(32, '0')

        dest = int(bindest, 2)

        # print("dest:{}".format(bindest))

        for i in range(len(self.addrlist)):
            if int(int(self.masklist[i], 2)) == 0:
                self.checklist.append("True")

            elif int(self.addrlist[i], 2) ^ dest >= 2 ** (32 - int(self.masklist[i], 2)):
                self.checklist.append("False")
            else:
                if int(self.masklist[i], 2) > self.prefix_len:
                    self.prefix_len = int(self.masklist[i], 2)
                    self.nexthoc = i
                self.checklist.append("True")

    def print_match(self):
        print("\n\n匹配情况：")
        f = open(".conf")
        lines = f.readlines()

        for i in range(len(lines)):
            if i == 0:
                continue
            line = lines[i].split('\n')[0].ljust(30) + self.checklist[i - 1]
            print(line)

        print("\n\n该数据报的下一跳: {}".format(self.nexthoclist[self.nexthoc]))


if __name__ == '__main__':

    strdest = input("请输入IP数据报的目的IP地址: ")

    rtl = RouterTableLookup(".conf")

    rtl.print_router_table()
    rtl.get_matchlist(strdest)
    rtl.print_match()
