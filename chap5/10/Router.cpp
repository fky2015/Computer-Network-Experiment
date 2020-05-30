/*
 * Copyright (c) 2005 - 2006
 *		CACE Technologies, Davis, CA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. Neither the name of CACE Technologies nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

 /*
  * This simple program implements a user-level bridge.
  * It opens two adapters specified by the user and starts two threads.
  * The first thread receives packets from adapter 1 and sends them down to
  * adapter 2. The second thread does the same, but in the opposite
  * direction.
  */


#pragma warning(disable:4996)
#define WIN32
#ifndef _XKEYCHECK_H
#define _XKEYCHECK_H
#endif
#pragma comment(lib, "Packet")
#pragma comment(lib, "wpcap")
#pragma comment(lib, "WS2_32")
long long counter = 0;
#include <pcap.h>
#include <signal.h>
#include <Packet32.h>
#include <ntddndis.h>
#include <string>
#include <remote-ext.h>
#include <iostream>
#include <iomanip> 
#include <cstdio>
#include <time.h>
#include <map>
#include <fstream>
using namespace std;
#define threshold 1024*1024
/* IP帧格式 */
typedef struct ip_header {
	u_char ver_ihl;				//Version (4 bits) + Internet header length (4 bits)
	u_char tos;					//Type of service
	u_short tlen;				//Total length
	u_short identification;		//Identification
	u_short flags_fo;			//Flags (3 bits) + Fragment offset (13 bits)
	u_char ttl;					//Time to live
	u_char proto;				//Protocol
	u_short crc;				//Header checksum
	u_char saddr[4];			//Source address
	u_char daddr[4];			//Destination address
	u_int op_pad;				//Option + Padding
} ip_header;

char buf1[20], buf2[20];

typedef struct MAC_Address {
	u_char address[6];
	bool operator < (const MAC_Address& b) const {
		for (int i = 0; i < 6; i++)
			if (address[i] < b.address[i])
				return true;
		return false;
	}
	bool operator == (const MAC_Address& b) const {
		for (int i = 0; i < 6; i++)
			if (address[i] != b.address[i])
				return false;
		return true;
	}
} macAddr;

/* 以太网数据链路层帧格式 */
typedef struct mac_header {
	MAC_Address dest;
	MAC_Address src;
	u_char type[2];
} mac_header;
#define MAX_SIZE 500
struct sendtable {
	MAC_Address addr;
	char *interface_description;
	pcap_t *send_interface;
}SendTable[MAX_SIZE];

int table_size = 0;
struct routertable {
	unsigned int dest;
	unsigned int mask;
	unsigned int next;
	bool direct;
} RouterTable[MAX_SIZE];


int router_table_size = 0;
#undef MAX_SIZE

map<unsigned int, mac_header> arp_map;

#include <string>
#include <sstream>
using namespace std;

MAC_Address local1_mac, local2_mac, next_mac, pc_mac;

void loadARPTable(const char * fileName) {
	ifstream infile;
	infile.open(fileName);
	string line;
	char _;
	
	{
		getline(infile, line);
		istringstream iss(line);
		MAC_Address&a = pc_mac;
		int t = 6, x;
		for (int i = 0; i < 6; i++)
		{
			iss >> hex >> x >> _;
			a.address[i] = x;
		}
	}
	{
		getline(infile, line);
		istringstream iss(line);
		MAC_Address &a = local1_mac;
		int t = 6, x;
		for (int i = 0; i < 6; i++)
		{
			iss >> hex >> x >> _;
			a.address[i] = x;
		}
	}
	{
		getline(infile, line);
		istringstream iss(line);
		MAC_Address &a= local2_mac;
		int t = 6, x;
		for (int i = 0; i < 6; i++)
		{
			iss >> hex >> x >> _;
			a.address[i] = x;
		}
	}
	{
		getline(infile, line);
		cout <<"ff"<< line << endl;
		istringstream iss(line);
		MAC_Address &a = next_mac;
		int t = 6, x;
		for (int i = 0; i < 6; i++)
		{
			iss >> hex >> x >> _;
			a.address[i] = x;
		}
	}
}

unsigned int BLEndianUint32(unsigned int value)
{
	return ((value & 0x000000FF) << 24) | ((value & 0x0000FF00) << 8) | ((value & 0x00FF0000) >> 8) | ((value & 0xFF000000) >> 24);
}

void printRouterTable() {
	cout << "current router table:\n";
	for (int i = 0; i < router_table_size; i++) {
		unsigned int dest = BLEndianUint32(RouterTable[i].dest);
		unsigned int mask = BLEndianUint32(RouterTable[i].mask);

		unsigned int next = BLEndianUint32(RouterTable[i].next);
		bool direct = RouterTable[i].direct;
		if (direct) {

			cout << "dest: " << inet_ntoa(*(in_addr *)&dest) << " mask: ";
			cout << inet_ntoa(*(in_addr *)&mask) << " direct" << endl;
		}
		else {
			cout << "dest: " << inet_ntoa(*(in_addr *)&dest);
			cout << " mask: " << inet_ntoa(*(in_addr *)&mask);
			cout << " next: " << inet_ntoa(*(in_addr *)&next) << " static" << endl;
		}

	}
}

void printRouterEntry(int i) {
	unsigned int dest = BLEndianUint32(RouterTable[i].dest);
	unsigned int mask = BLEndianUint32(RouterTable[i].mask);

	unsigned int next = BLEndianUint32(RouterTable[i].next);
	bool direct = RouterTable[i].direct;
	if (direct) {

		cout << "dest: " << inet_ntoa(*(in_addr *)&dest) << " mask: ";
		cout << inet_ntoa(*(in_addr *)&mask) << " direct" << endl;
	}
	else {
		cout << "dest: " << inet_ntoa(*(in_addr *)&dest);
		cout << " mask: " << inet_ntoa(*(in_addr *)&mask);
		cout << " next: " << inet_ntoa(*(in_addr *)&next) << " static" << endl;
	}
}

void loadRouterTable(routertable *t, const char * fileName) {
	ifstream infile;
	unsigned short a, b, c, d;
	u_char _;
	int mask_num;
	infile.open(fileName);
	string line;
	while (getline(infile, line)) {
		cout << line << endl;
		if (line.find("direct") != string::npos) {
			istringstream iss(line);
			// direct
			iss >> a >> _ >> b >> _ >> c >> _ >> d >> _ >> mask_num;
			char tmp[4] = {};
			tmp[0] = a;
			tmp[1] = b;
			tmp[2] = c;
			tmp[3] = d;
			t[router_table_size].dest = BLEndianUint32 (*(int *)tmp);
			t[router_table_size].mask = UINT_MAX << (32 - mask_num);
			t[router_table_size].direct = true;
			router_table_size++;
		}
		else {
			istringstream iss(line);
			iss >> a >> _ >> b >> _ >> c >> _ >> d >> _ >> mask_num;
			char tmp[4] = {};
			tmp[0] = a;
			tmp[1] = b;
			tmp[2] = c;
			tmp[3] = d;
			t[router_table_size].dest = BLEndianUint32( *(int *)tmp);
			t[router_table_size].mask = UINT_MAX << (32 - mask_num);
			iss >> a >> _ >> b >> _ >> c >> _ >> d;
			printf("%d %d %d %d\n", a, b, c, d);
			tmp[0] = a;
			tmp[1] = b;
			tmp[2] = c;
			tmp[3] = d;
			t[router_table_size].next = BLEndianUint32( *(int *)tmp);
			
			t[router_table_size].direct = false;
			router_table_size++;
			// static
		}
		printRouterTable();
		// printf("dest ip %d\n", t[router_table_size - 1].dest);
	}
}

/* Storage data structure used to pass parameters to the threads */
typedef struct _in_out_adapters
{
	char *name;		/* Some simple state information */
	pcap_t *input_adapter;
	pcap_t *output_adapter;
}in_out_adapters;

/* Prototypes */
DWORD WINAPI CaptureAndForwardThread(LPVOID lpParameter);
void ctrlc_handler(int sig);

/* This prevents the two threads to mess-up when they do printfs */
CRITICAL_SECTION print_cs, table_cs;

/* Thread handlers. Global because we wait on the threads from the CTRL+C handler */
HANDLE threads[2];

/* This global variable tells the forwarder threads they must terminate */
volatile int kill_forwaders = 0;

string mactostr(MAC_Address macaddr) {
	char str[20];

	sprintf(str, "%02X-%02X-%02X-%02X-%02X-%02X", macaddr.address[0], macaddr.address[1], macaddr.address[2], macaddr.address[3], macaddr.address[4], macaddr.address[5]);
	//cout << str << endl;
	return string(str);
}

char *iptos(u_long in);
char* ip6tos(struct sockaddr *sockaddr, char *address, int addrlen);


/* From tcptraceroute, convert a numeric IP address to a string */
#define IPTOSBUFFERS    12
char *iptos(u_long in)
{
	static char output[IPTOSBUFFERS][3 * 4 + 3 + 1];
	static short which;
	u_char *p;

	p = (u_char *)&in;
	which = (which + 1 == IPTOSBUFFERS ? 0 : which + 1);
	_snprintf_s(output[which], sizeof(output[which]), sizeof(output[which]), "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
	return output[which];
}

char* ip6tos(struct sockaddr *sockaddr, char *address, int addrlen)
{
	socklen_t sockaddrlen;

#ifdef WIN32
	sockaddrlen = sizeof(struct sockaddr_in6);
#else
	sockaddrlen = sizeof(struct sockaddr_storage);
#endif


	if (getnameinfo(sockaddr,
			sockaddrlen,
			address,
			addrlen,
			NULL,
			0,
			NI_NUMERICHOST) != 0) address = NULL;

	return address;
}


/* Print all the available information on the given interface */
void ifprint(pcap_if_t *d)
{
	pcap_addr_t *a;
	char ip6str[128];

	/* Name */
	printf("%s\n", d->name);

	/* Description */
	if (d->description)
		printf("\tDescription: %s\n", d->description);

	/* Loopback Address*/
	printf("\tLoopback: %s\n", (d->flags & PCAP_IF_LOOPBACK) ? "yes" : "no");

	/* IP addresses */
	for (a = d->addresses; a; a = a->next) {
		printf("\tAddress Family: #%d\n", a->addr->sa_family);

		switch (a->addr->sa_family)
		{
		case AF_INET:
			printf("\tAddress Family Name: AF_INET\n");
			if (a->addr)
				printf("\tAddress: %s\n", iptos(((struct sockaddr_in *)a->addr)->sin_addr.s_addr));
			if (a->netmask)
				printf("\tNetmask: %s\n", iptos(((struct sockaddr_in *)a->netmask)->sin_addr.s_addr));
			if (a->broadaddr)
				printf("\tBroadcast Address: %s\n", iptos(((struct sockaddr_in *)a->broadaddr)->sin_addr.s_addr));
			if (a->dstaddr)
				printf("\tDestination Address: %s\n", iptos(((struct sockaddr_in *)a->dstaddr)->sin_addr.s_addr));
			break;

		case AF_INET6:
			printf("\tAddress Family Name: AF_INET6\n");
			if (a->addr)
				printf("\tAddress: %s\n", ip6tos(a->addr, ip6str, sizeof(ip6str)));
			break;

		default:
			printf("\tAddress Family Name: Unknown\n");
			break;
		}
	}
	printf("\n");
}

/*******************************************************************/
char config_r[40], config_a[40];
int main()
{
	int z = 0;
	cin >> z;
	getchar();
	sprintf(config_r,"config%d.txt", z);
	sprintf(config_a, "mac%d.txt", z);
	loadRouterTable(RouterTable, config_r);
	loadARPTable(config_a);
	pcap_if_t *alldevs;
	pcap_if_t *d;
	int inum1, inum2;
	int i = 0;
	pcap_t *adhandle1, *adhandle2;
	char errbuf[PCAP_ERRBUF_SIZE];
	u_int netmask1, netmask2;
	char packet_filter[256];
	struct bpf_program fcode;
	in_out_adapters couple0, couple1;

	/*
	 * Retrieve the device list
	 */

	if (pcap_findalldevs_ex((char *)"rpcap:/", NULL, &alldevs, errbuf) == -1)
	{
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}

	/* Print the list */
	for (d = alldevs; d; d = d->next)
	{
		printf("%d. ", ++i);
		if (d->description)
			ifprint(d);
			//printf("%s %s\n", d->description);
		else
			printf("<unknown adapter>\n");
	}

	if (i == 0)
	{
		printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
		return -1;
	}


	/*
	 * Get input from the user
	 */

	 /* Get the filter*/
	printf("\nSpecify filter (hit return for no filter):");

	fgets(packet_filter, sizeof(packet_filter), stdin);

	/* Get the first interface number*/
	printf("\nEnter the number of the first interface to use (1-%d):", i);
	scanf_s("%d", &inum1);

	if (inum1 < 1 || inum1 > i)
	{
		printf("\nInterface number out of range.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* Get the second interface number*/
	printf("Enter the number of the first interface to use (1-%d):", i);
	scanf_s("%d", &inum2);

	if (inum2 < 1 || inum2 > i)
	{
		printf("\nInterface number out of range.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	if (inum1 == inum2)
	{
		printf("\nCannot bridge packets on the same interface.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}


	/*
	 * Open the specified couple of adapters
	 */

	 /* Jump to the first selected adapter */
	for (d = alldevs, i = 0; i < inum1 - 1; d = d->next, i++);

	/*
	 * Open the first adapter.
	 * *NOTICE* the flags we are using, they are important for the behavior of the prgram:
	 *	- PCAP_OPENFLAG_PROMISCUOUS: tells the adapter to go in promiscuous mode.
	 *    This means that we are capturing all the traffic, not only the one to or from
	 *    this machine.
	 *	- PCAP_OPENFLAG_NOCAPTURE_LOCAL: prevents the adapter from capturing again the packets
	 *	  transmitted by itself. This avoids annoying loops.
	 *	- PCAP_OPENFLAG_MAX_RESPONSIVENESS: configures the adapter to provide minimum latency,
	 *	  at the cost of higher CPU usage.
	 */
	couple0.name = d->description;
	if ((adhandle1 = pcap_open(d->name,						// name of the device
				   65536,							// portion of the packet to capture. 
											      // 65536 grants that the whole packet will be captured on every link layer.
				   PCAP_OPENFLAG_PROMISCUOUS |	// flags. We specify that we don't want to capture loopback packets, and that the driver should deliver us the packets as fast as possible
				   PCAP_OPENFLAG_NOCAPTURE_LOCAL |
				   PCAP_OPENFLAG_MAX_RESPONSIVENESS,
				   500,							// read timeout
				   NULL,							// remote authentication
				   errbuf							// error buffer
	)) == NULL)
	{
		fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n", d->description);
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	if (d->addresses != NULL)
	{
		/* Retrieve the mask of the first address of the interface */
		netmask1 = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	}
	else
	{
		/* If the interface is without addresses we suppose to be in a C class network */
		netmask1 = 0xffffff;
	}

	/* Jump to the second selected adapter */
	for (d = alldevs, i = 0; i < inum2 - 1; d = d->next, i++);
	couple1.name = d->description;
	/* Open the second adapter */
	if ((adhandle2 = pcap_open(d->name,						// name of the device
				   65536,							// portion of the packet to capture. 
											      // 65536 grants that the whole packet will be captured on every link layer.
				   PCAP_OPENFLAG_PROMISCUOUS |	// flags. We specify that we don't want to capture loopback packets, and that the driver should deliver us the packets as fast as possible
				   PCAP_OPENFLAG_NOCAPTURE_LOCAL |
				   PCAP_OPENFLAG_MAX_RESPONSIVENESS,
				   500,							// read timeout
				   NULL,							// remote authentication
				   errbuf							// error buffer
	)) == NULL)
	{
		fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n", d->description);
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	if (d->addresses != NULL)
	{
		/* Retrieve the mask of the first address of the interface */
		netmask2 = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	}
	else
	{
		/* If the interface is without addresses we suppose to be in a C class network */
		netmask2 = 0xffffff;
	}


	/*
	 * Compile and set the filters
	 */

	 /* compile the filter for the first adapter */
	if (pcap_compile(adhandle1, &fcode, packet_filter, 1, netmask1) < 0)
	{
		fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");

		/* Close the adapters */
		pcap_close(adhandle1);
		pcap_close(adhandle2);

		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* set the filter for the first adapter*/
	if (pcap_setfilter(adhandle1, &fcode) < 0)
	{
		fprintf(stderr, "\nError setting the filter.\n");

		/* Close the adapters */
		pcap_close(adhandle1);
		pcap_close(adhandle2);

		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* compile the filter for the second adapter */
	if (pcap_compile(adhandle2, &fcode, packet_filter, 1, netmask2) < 0)
	{
		fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");

		/* Close the adapters */
		pcap_close(adhandle1);
		pcap_close(adhandle2);

		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* set the filter for the second adapter*/
	if (pcap_setfilter(adhandle2, &fcode) < 0)
	{
		fprintf(stderr, "\nError setting the filter.\n");

		/* Close the adapters */
		pcap_close(adhandle1);
		pcap_close(adhandle2);

		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* At this point, we don't need the device list any more. Free it */
	pcap_freealldevs(alldevs);

	/*
	 * Start the threads that will forward the packets
	 */

	 /* Initialize the critical section that will be used by the threads for console output */
	InitializeCriticalSection(&print_cs);
	InitializeCriticalSection(&table_cs);
	/* Init input parameters of the threads */
	sprintf(buf1, "%d -> %d", inum1, inum2);
	couple0.input_adapter = adhandle1;
	couple0.output_adapter = adhandle2;
	couple0.name = buf1;
	sprintf(buf2, "%d -> %d", inum2, inum1);
	couple1.input_adapter = adhandle2;
	couple1.output_adapter = adhandle1;
	couple1.name = buf2;
	/* Start first thread */
	if ((threads[0] = CreateThread(
		NULL,
		0,
		CaptureAndForwardThread,
		&couple0,
		0,
		NULL)) == NULL)
	{
		fprintf(stderr, "error creating the first forward thread");

		/* Close the adapters */
		pcap_close(adhandle1);
		pcap_close(adhandle2);

		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* Start second thread */
	if ((threads[1] = CreateThread(
		NULL,
		0,
		CaptureAndForwardThread,
		&couple1,
		0,
		NULL)) == NULL)
	{
		fprintf(stderr, "error creating the second forward thread");

		/* Kill the first thread. Not very gentle at all...*/
		TerminateThread(threads[0], 0);

		/* Close the adapters */
		pcap_close(adhandle1);
		pcap_close(adhandle2);

		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/*
	 * Install a CTRL+C handler that will do the cleanups on exit
	 */
	signal(SIGINT, ctrlc_handler);

	/*
	 * Done!
	 * Wait for the Greek calends...
	 */
	printf("\nStart bridging the two adapters...\n");
	Sleep(INFINITE);
	return 0;
}

/*******************************************************************
 * Forwarding thread.
 * Gets the packets from the input adapter and sends them to the output one.
 *******************************************************************/
DWORD WINAPI CaptureAndForwardThread(LPVOID lpParameter)
{
	struct pcap_pkthdr *header;
	const u_char *pkt_data;
	mac_header *mh;
	int res = 0;
	in_out_adapters* ad_couple = (in_out_adapters*)lpParameter;
	unsigned int n_fwd = 0;

	/*
	 * Loop receiving packets from the first input adapter
	 */

	while ((!kill_forwaders) && (res = pcap_next_ex(ad_couple->input_adapter, &header, &pkt_data)) >= 0)
	{
		if (header->len <= 0) continue;
		mh = (mac_header *)pkt_data;
		string mac_dest = mactostr(mh->dest), mac_src = mactostr(mh->src);
		if (res != 0)	/* Note: res=0 means "read timeout elapsed"*/
		{
			/*
			 * Print something, just to show when we have activity.
			 * BEWARE: acquiring a critical section and printing strings with printf
			 * is something inefficient that you seriously want to avoid in your packet loop!
			 * However, since this is a *sample program*, we privilege visual output to efficiency.
			 */
			 // start to extract the mac package.
                         // so we can extract ip further more.
			ip_header * iphdr = (ip_header *)((char *)pkt_data + 14);
			EnterCriticalSection(&print_cs);
			struct in_addr addr1;
			struct in_addr addr2;
			memcpy(&addr1, &iphdr->saddr, 4);
			memcpy(&addr2, &iphdr->daddr, 4);
			cout << counter << " ----------------------------------------------------------" << endl;
			cout << "the data frame is from " << ad_couple->name << endl;
			cout << "the source MAC address is " << mac_src << endl;
			cout << "the destination MAC address is " << mac_dest << endl;
			cout << "ip src is " << inet_ntoa(addr1) << endl;
			cout << "ip dest is " << inet_ntoa(addr2)<< endl;


			int found_entry = -1;
			unsigned dest_ip = BLEndianUint32( *(unsigned int *)(iphdr->daddr));

			printRouterTable();
			for (int i = 0; i < router_table_size; i++)
			{
				if ((dest_ip &RouterTable[i].mask)== (RouterTable[i].dest & RouterTable[i].mask)) {
					printf("find match router table %d\n", i);
					printRouterEntry(i);
					found_entry = i;
					
					
					if (RouterTable[found_entry].direct == false) {

						// regenerate mac 
						memcpy(&mh->dest, next_mac.address, 6);
						memcpy(&mh->src, local2_mac.address, 6);
					}
					else {
						// regenerate mac 
						memcpy(&mh->dest, pc_mac.address, 6);
						memcpy(&mh->src, local1_mac.address, 6);
					}
					
					// calculate checksum
					char * header = (char *)iphdr;
					int tempck = 0;
					for (int i = 0; i < 10; i++) {
						if (i == 5)
							continue;
						tempck += *(unsigned short*)(header + i * 2);
					}
					while (tempck >= (1 << 16)) {
						tempck = (((1 << 16) - 1) & tempck) + (tempck >> 16);
					}
					iphdr->ttl -= 1;
					unsigned short checksum = ~tempck;
					cout << "new checksum is: " << checksum << endl;
					iphdr->crc = checksum;
				}
			}



			counter++;
			EnterCriticalSection(&table_cs);
			int existed = 0;
			for (int i = 1; i <= table_size; i++)
			{
				if (SendTable[i].addr == mh->src)
				{
					existed = i;
					break;
				}
			}
			if (existed)
			{
				SendTable[existed].interface_description = ad_couple->name;
				SendTable[existed].send_interface = ad_couple->input_adapter;
				cout << "BackWard Learning Completed: Update the Table" << endl;
			}
			else
			{
				table_size++;
				SendTable[table_size].addr = mh->src;
				SendTable[table_size].interface_description = ad_couple->name;
				SendTable[table_size].send_interface = ad_couple->input_adapter;
				cout << "BackWard Learning Completed: Add new MAC Address" << endl;
			}
			//cout << "the SendTable is:" << endl;
			//for (int i = 1; i <= table_size; i++)
			//{
			//	cout << mactostr(SendTable[i].addr) << ": " << SendTable[i].interface_description << endl;
			//}
			LeaveCriticalSection(&table_cs);
			cout << endl;
			LeaveCriticalSection(&print_cs);
			//cout << "hello" << endl;

			/*
			 * Send the just received packet to the output adaper
			 */
			pair<char *, pcap_t *> tmp;
			//cout << "hello" << endl;
			int found = 0;
			EnterCriticalSection(&table_cs);
			//int found = SendTable.count(mh->dest);
			//cout << "hello" << endl;
			//if (found)
		//	tmp = SendTable[mh->dest];
			for (int i = 1; i <= table_size; i++)
			{
				if (SendTable[i].addr == mh->dest)
				{
					found = 1;
					tmp.first = SendTable[i].interface_description;
					tmp.second = SendTable[i].send_interface;
				}
			}
			LeaveCriticalSection(&table_cs);
			//cout << "hello" << endl;
			if (found)
			{
				if (tmp.second == ad_couple->input_adapter)
				{
					EnterCriticalSection(&print_cs);
					cout << "Discard the packet" << endl;
					LeaveCriticalSection(&print_cs);
				}
				else {
					EnterCriticalSection(&print_cs);
					cout << "Transport the packet to " << tmp.first << endl;
					LeaveCriticalSection(&print_cs);
					if (pcap_sendpacket(tmp.second, pkt_data, header->caplen) != 0)
					{
						EnterCriticalSection(&print_cs);

						printf("Error sending a %u bytes packets on interface %s: %s\n",
						       header->caplen,
						       ad_couple->name,
						       pcap_geterr(tmp.second));

						LeaveCriticalSection(&print_cs);
					}
					else
					{
						n_fwd++;
					}
				}
			}
			else
			{
				EnterCriticalSection(&print_cs);
				cout << "Flooding the packet" << endl;
				LeaveCriticalSection(&print_cs);
				if (pcap_sendpacket(ad_couple->output_adapter, pkt_data, header->caplen) != 0)
				{
					EnterCriticalSection(&print_cs);

					printf("Error sending a %u bytes packets on interface %s: %s\n",
					       header->caplen,
					       ad_couple->name,
					       pcap_geterr(ad_couple->output_adapter));

					LeaveCriticalSection(&print_cs);
				}
				else
				{
					n_fwd++;
				}
			}
			EnterCriticalSection(&print_cs);
			cout << "----------------------------------------------------------" << endl;
			LeaveCriticalSection(&print_cs);
		}
	}

	/*
	 * We're out of the main loop. Check the reason.
	 */
	if (res < 0)
	{
		EnterCriticalSection(&print_cs);

		printf("Error capturing the packets: %s\n", pcap_geterr(ad_couple->input_adapter));
		fflush(stdout);

		LeaveCriticalSection(&print_cs);
	}
	else
	{
		EnterCriticalSection(&print_cs);

		printf("End of bridging on interface %s. Forwarded packets:%u\n",
		       ad_couple->name,
		       n_fwd);
		fflush(stdout);

		LeaveCriticalSection(&print_cs);
	}

	return 0;
}

/*******************************************************************
 * CTRL+C hanlder.
 * We order the threads to die and then we patiently wait for their
 * suicide.
 *******************************************************************/
void ctrlc_handler(int sig)
{
	/*
	 * unused variable
	 */
	(VOID)(sig);

	kill_forwaders = 1;

	WaitForMultipleObjects(2,
			       threads,
			       TRUE,		/* Wait for all the handles */
			       5000);		/* Timeout */

	exit(0);
}