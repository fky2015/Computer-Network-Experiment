/***************************************************
 * file:     testpcap1.c
 * Date:     Thu Mar 08 17:14:36 MST 2001
 * Author:   Martin Casado
 * Location: LAX Airport (hehe)
 *
 * Simple single packet capture program
 *****************************************************/
#include <arpa/inet.h>
#include <errno.h>
#include <net/ethernet.h>
#include <netinet/if_ether.h> /* includes net/ethernet.h */
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <pcap.h> /* if this gives you an error try pcap/pcap.h */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define ETHERNET_HEAD_SIZE 14
#define TCP_HEAD_SIZE 20
#define IP_ADDR_SIZE 4
#define print(format, args...) printf(" " format "\n", args);

char buffer[20000] = {};
int buffer_offset = 0;

int http_finished() {

    char *body_start;
    if (!(body_start = strstr(buffer, "\r\n\r\n")))
        return 0;

    body_start += 4;
    int content_length = 0;
    char *content_l;
    if (!(content_l = strstr(buffer, "Content-Length:")))
        return 1;
    content_l += strlen("Content-Length:");
    sscanf(content_l, "%d", &content_length);
    if (buffer + buffer_offset - body_start == content_length) {
        return 1;
    } else {
        printf("size: %d, body_start: %d\n", buffer_offset,
               body_start - buffer);
        return 0;
    }
}

void http_analysis(const u_char *packet, int offset, int size,
                   int has_http_content, int push_flag) {
    const u_char *ptr = packet + offset;

    if (has_http_content) {
        memcpy(buffer + buffer_offset, ptr, size);
        buffer_offset += size;
        /*puts("=============================");*/
        /*puts(buffer);*/
        /*puts("=============================");*/
    }
    if (http_finished()) {
        // end
        char *ptr1 = strstr(buffer, "\r\n");
        char *ptr2 = strstr(buffer, "\r\n\r\n");
        char *ptr3 = buffer + offset;

        puts("\n\nHTTP");

        print("Request Line:\n%.*s", ptr1 - buffer, buffer);
        puts("");
        print("Header:\n%.*s", ptr2 - ptr1 - 2, ptr1 + 2);
        puts("");
        print("BODY:\n%.*s", ptr3 - ptr2 - 4, ptr2 + 4);

        memset(buffer, 0, sizeof(buffer));
        buffer_offset = 0;
    }
}
void tcp_analysis(const u_char *packet, int ip_header_size, int total_len) {
    struct tcphdr *tcph =
        (struct tcphdr *)(packet + ETHERNET_HEAD_SIZE + ip_header_size);

    print(" Src Port: %d", ntohs(tcph->th_sport));
    print(" Dest Port: %d", ntohs(tcph->th_dport));
    print(" Sequence Number: %x", ntohs(tcph->th_seq));
    print(" Acknowledgement Number: %d", ntohs(tcph->th_ack));
    print(" Data Offset: %x", tcph->th_off);
    print(" Flag: %x", tcph->th_flags);
    print(" Window Size: %x", ntohs(tcph->th_flags));
    print(" Checksum: %x", ntohs(tcph->th_sum));
    print(" Urgent Pointer: %x", ntohs(tcph->th_urp));

    // Application Layer
    http_analysis(
        packet, ETHERNET_HEAD_SIZE + ip_header_size + TCP_HEAD_SIZE,
        total_len - ETHERNET_HEAD_SIZE - ip_header_size - TCP_HEAD_SIZE,
        total_len - ETHERNET_HEAD_SIZE - ip_header_size - TCP_HEAD_SIZE > 0 &&
            (tcph->th_flags & 0x10) && (!(tcph->th_flags & TH_SYN)),
        tcph->th_flags & TH_PUSH);
}

void udp_analysis(const u_char *packet, int ip_header_size, int total_len) {
    struct udphdr *udphd =
        (struct udphdr *)(packet + ETHERNET_HEAD_SIZE + ip_header_size);

    print("Src Port: %d", ntohs(udphd->uh_sport));
    print("Dest Port: %d", ntohs(udphd->uh_dport));
    print("UDP Length: %d", ntohs(udphd->uh_ulen));
    print("Checksum: %d", ntohs(udphd->uh_sum));
}

void icmp_analysis(const u_char *packet, int ip_header_size, int total_len) {
    struct icmphdr *icmphd =
        (struct icmphdr *)(packet + ETHERNET_HEAD_SIZE + ip_header_size);

    u_char icmp_type = icmphd->type;
    switch (icmp_type) {
    case ICMP_ECHOREPLY:
        puts("ICMP Echo Reply");
        break;
    case ICMP_INFO_REQUEST:
        puts("ICMP Request");
        break;
    case ICMP_INFO_REPLY:
        puts("ICMP Response");
        break;
    case ICMP_TIME_EXCEEDED:
        puts("ICMP TIME_EXCEEDED");
        break;
        // ...
    }
    print("ICMP Code: %d", icmphd->code);
    print("ICMP checksum: %d", icmphd->checksum);
}

void ip_analysis(const u_char *packet, int total_len) {
    u_char protocol;

    struct iphdr *iph = (struct iphdr *)(packet + ETHERNET_HEAD_SIZE);
    print("Version: %d", iph->version);
    print("Header Length: %d", iph->ihl);
    print("Type of Service: %x", iph->tos);
    print("Total length: %d", ntohs(iph->tot_len));
    print("Indentification: %x", ntohs(iph->id));
    print("Offset: %d", ntohs(iph->frag_off));
    print("TTL: %d", iph->ttl);
    print("Protocal: %d", iph->protocol);
    print("CheckSum: %d", ntohs(iph->check));

    struct in_addr ip_addr;
    ip_addr.s_addr = iph->saddr;
    print("Src ip: %s", inet_ntoa(ip_addr));
    ip_addr.s_addr = iph->daddr;
    print("Dest ip: %s", inet_ntoa(ip_addr));

    // Transport Layer
    switch (iph->protocol) {
    case IPPROTO_TCP:
        puts("TCP protocol!");
        tcp_analysis(packet, iph->ihl * 4, total_len);
        break;
    case IPPROTO_ICMP:
        puts("ICMP protocol!");
        icmp_analysis(packet, iph->ihl * 4, total_len);
        break;
    case IPPROTO_UDP:
        puts("UDP protocal!");
        udp_analysis(packet, iph->ihl * 4, total_len);
        break;
    }
}

void arp_analysis(const u_char *packet, int total_len) {}

int main(int argc, char **argv) {
    int i;
    char *dev;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *descr;
    bpf_u_int32 mask;
    bpf_u_int32 net;
    struct bpf_program fp;
    char filter_exp[] = "udp";
    const u_char *packet;
    struct pcap_pkthdr hdr;    /* pcap.h */
    struct ether_header *eptr; /* net/ethernet.h */

    u_char *ptr; /* printing out hardware header info */

    pcap_if_t *alldevsp;

    int dev_i;
    if (pcap_findalldevs(&alldevsp, errbuf) != 0) {
        printf("%s\n", errbuf);
        exit(1);

    } else {
        int i = 0;
        for (pcap_if_t *pdev = alldevsp; pdev; pdev = pdev->next)
            printf("#%d: %s %s\n", i++, pdev->name,
                   pdev->description ? pdev->description : "");
        puts("\n please input dev choose:");
        scanf("%d", &dev_i);
        if (dev_i >= 0 && dev_i < i) {
            pcap_if_t *pdev = alldevsp;
            while (pdev && dev_i--) {
                pdev = pdev->next;
            }
            dev = pdev->name;
        } else {
            fprintf(stderr, "no such device\n");
            exit(0);
        }
    }

    /* grab a device to peak into... */
    /*dev = pcap_lookupdev(errbuf);*/

    /*if (dev == NULL) {*/
    /*printf("%s\n", errbuf);*/
    /*exit(1);*/
    /*}*/

    printf("DEV: %s\n", dev);

    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Couldn't get netmask for dev\n");
        net = 0;
        mask = 0;
    }

    /* open the device for sniffing.

       pcap_t *pcap_open_live(char *device,int snaplen, int prmisc,int to_ms,
       char *ebuf)

       snaplen - maximum size of packets to capture in bytes
       promisc - set card in promiscuous mode?
       to_ms   - time to wait for packets in miliseconds before read
       times out
       errbuf  - if something happens, place error string here

       Note if you change "prmisc" param to anything other than zero, you will
       get all packets your device sees, whether they are intendeed for you or
       not!! Be sure you know the rules of the network you are running on
       before you set your card in promiscuous mode!!     */

    descr = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);

    if (descr == NULL) {
        printf("pcap_open_live(): %s\n", errbuf);
        exit(1);
    }

    if (pcap_compile(descr, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp,
                pcap_geterr(descr));
        return (2);
    }

    if (pcap_setfilter(descr, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp,
                pcap_geterr(descr));
        return (2);
    }

    /*
       grab a packet from descr (yay!)
       u_char *pcap_next(pcap_t *p,struct pcap_pkthdr *h)
       so just pass in the descriptor we got from
       our call to pcap_open_live and an allocated
       struct pcap_pkthdr                                 */

    while (packet = pcap_next(descr, &hdr)) {

        if (packet == NULL) { /* dinna work *sob* */
            printf("Didn't grab packet\n");
            exit(1);
        }

        /*  struct pcap_pkthdr {
            struct timeval ts;   time stamp
            bpf_u_int32 caplen;  length of portion present
            bpf_u_int32;         lebgth this packet (off wire)
            }
         */

        printf("Grabbed packet of length %d\n", hdr.len);
        // printf("Recieved at ..... %s\n",ctime((const
        // time_t*)&hdr.ts.tv_sec));
        printf("Ethernet address length is %d\n", ETHER_HDR_LEN);

        /* lets start with the ether header... */
        eptr = (struct ether_header *)packet;

        ptr = eptr->ether_dhost;
        i = ETHER_ADDR_LEN;
        printf(" Destination Address:  ");
        do {
            printf("%s%x", (i == ETHER_ADDR_LEN) ? " " : ":", *ptr++);
        } while (--i > 0);
        printf("\n");

        ptr = eptr->ether_shost;
        i = ETHER_ADDR_LEN;
        printf(" Source Address:  ");
        do {
            printf("%s%x", (i == ETHER_ADDR_LEN) ? " " : ":", *ptr++);
        } while (--i > 0);
        printf("\n");

        // network layer
        if (ntohs(eptr->ether_type) == ETHERTYPE_IP) {
            printf("Ethernet type hex:%x dec:%d is an IP packet\n",
                   ntohs(eptr->ether_type), ntohs(eptr->ether_type));
            ip_analysis(packet, hdr.len);
        } else if (ntohs(eptr->ether_type) == ETHERTYPE_ARP) {
            printf("Ethernet type hex:%x dec:%d is an ARP packet\n",
                   ntohs(eptr->ether_type), ntohs(eptr->ether_type));
            arp_analysis(packet, hdr.len);
        } else {
            printf("Ethernet type %x not IP", ntohs(eptr->ether_type));
            exit(1);
        }
    };

    pcap_freealldevs(alldevsp);
    return 0;
}
