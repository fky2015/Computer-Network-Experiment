#include <iostream>
#include <pcap.h>
#include <string>
#include <cstring>
#include <vector>
#include <map>



int get_devs()
{

    vector<pcap_if_t *> tmp;
    if (pcap_findalldevs())
}
int main()
{
    char errBuf[PCAP_ERRBUF_SIZE],*device;
    pcap_if_t *devs,*d;
    pcap_findalldevs(&devs,errBuf);

    int i=0;
    for (d = devs; d; d = d->next )
    {
        printf("%d. %s",++i,d->name);
        if (d->description)
            printf("(%s)\n",d->description);
        else
            printf("(No description avaliable)\n");
    }

    return 0;
}
