﻿/*
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

#ifndef _XKEYCHECK_H
#define _XKEYCHECK_H
#endif
#pragma comment(lib, "Packet")
#pragma comment(lib, "wpcap")
#pragma comment(lib, "WS2_32")

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

typedef struct MAC_Address{
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
#undef MAX_SIZE
int table_size = 0;
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
CRITICAL_SECTION print_cs,table_cs;

/* Thread handlers. Global because we wait on the threads from the CTRL+C handler */
HANDLE threads[2];

/* This global variable tells the forwarder threads they must terminate */
volatile int kill_forwaders = 0;

string mactostr(MAC_Address macaddr) {
	char str[20];
	
	sprintf(str,"%02X-%02X-%02X-%02X-%02X-%02X", macaddr.address[0], macaddr.address[1], macaddr.address[2], macaddr.address[3], macaddr.address[4], macaddr.address[5]);
	//cout << str << endl;
	return string(str);
}

/*******************************************************************/

int main()
{
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

	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
	{
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}

	/* Print the list */
	for (d = alldevs; d; d = d->next)
	{
		printf("%d. ", ++i);
		if (d->description)
			printf("%s\n", d->description);
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
	couple0.input_adapter = adhandle1;
	couple0.output_adapter = adhandle2;
	couple1.input_adapter = adhandle2;
	couple1.output_adapter = adhandle1;

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
			EnterCriticalSection(&print_cs);
			cout << "----------------------------------------------------------" << endl;
			cout << "the data frame is from " << ad_couple->name << endl;
			cout << "the source MAC address is " << mac_src << endl;
			cout << "the destination MAC address is " << mac_dest << endl;
		
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
			cout << "the SendTable is:" << endl;
			for (int i = 1; i <= table_size; i++)
			{
				cout << mactostr(SendTable[i].addr) << ": " << SendTable[i].interface_description << endl;
			}
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
				if (tmp.second==ad_couple->input_adapter)
				{
					EnterCriticalSection(&print_cs);
					cout << "Discard the packet" << endl;
					LeaveCriticalSection(&print_cs);
				}
				else {
					EnterCriticalSection(&print_cs);
					cout << "Transport the packet to "<<tmp.first << endl;
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
				cout << "Flooding the packet" <<endl;
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