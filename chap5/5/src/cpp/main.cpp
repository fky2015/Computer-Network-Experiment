#include <iostream>
#include "IpAddress.h"
#include <vector>
#include <fstream>
#include "RouterTable.h"
using namespace std;
const char conf[] = "ip.conf";
string addr, dest;
RouterTable routerTable;


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