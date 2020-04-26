#include <iostream>
#include "IpAddress.h"
#include <vector>
#include <fstream>
#include "RouterTable.h"
using namespace std;

string addr, dest;
RouterTable routerTable;

const char conf[] = "ip.conf";
int main()
{
    ifstream in(conf);
    in >> addr;
    while (in >> addr >> dest)
    {
        routerTable.addRouter(IpAddress::fromstring(addr,dest));
    }
    cout<<"Please input the destination IP Address: ";
    cin>>addr;
    routerTable.showDetail();
    IpAddress destinationAddress=routerTable.match(addr);
    cout<<"The next destination router/interface is:\t"<<destinationAddress.getDestination()<<endl;
}