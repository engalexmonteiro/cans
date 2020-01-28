/*
 * network.h
 *
 *  Created on: Mar 6, 2014
 *      Author: alex
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/time.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
//#include <netinet/ip_var.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
//#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct nic_info{
	int type;
	char ifname[10];
	char hostname[30];
	char mac_adress[50];
	char ip_adress[15];
	char mask[15];
	char gateway_address[15];
	char dns_address1[15];
	char dns_address2[15];
} nic_info;

char *getlocalipaddress(char* iface);

char* get_ip_address(char* ifname);

int getroute();

int ping(char* target);

#endif /* NETWORK_H_ */
