/*
 * network.c
 *
 *  Created on: Mar 6, 2014
 *      Author: alex
 */


#include "network.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

char* get_ip_address(char* ifname){

	int sock, i;
    struct ifreq ifreqs[20];
    struct ifconf ic;

    ic.ifc_len = sizeof ifreqs;
    ic.ifc_req = ifreqs;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    if (ioctl(sock, SIOCGIFCONF, &ic) < 0) {
        perror("SIOCGIFCONF");
        exit(1);
    }

    for (i = 0; i < ic.ifc_len/sizeof(struct ifreq); ++i){
    	if(!strcmp(ifreqs[i].ifr_name,ifname)){
    		close(sock);
    		return inet_ntoa(((struct sockaddr_in*)&ifreqs[i].ifr_addr)->sin_addr);
    	}
    }

    close(sock);

    return "0.0.0.0";
}

uint16_t in_cksum(uint16_t *addr, unsigned len);

#define	DEFDATALEN	(64-ICMP_MINLEN)	/* default data length */
#define	MAXIPLEN	60
#define	MAXICMPLEN	76
#define	MAXPACKET	(65536 - 60 - ICMP_MINLEN)/* max packet size */

int ping(char* target)
{

        int s, i, cc, packlen, datalen = DEFDATALEN;
	struct hostent *hp;
	struct sockaddr_in to, from;
	//struct protoent	*proto;
	//struct ip *ip;
	u_char *packet, outpack[MAXPACKET];
	char hnamebuf[MAXHOSTNAMELEN];
	char *hostname;
	struct icmp *icp;
	int ret, fromlen, hlen;
	fd_set rfds;
	struct timeval tv;
	int retval;
	struct timeval start, end;
	int /*start_t, */end_t =0;
	int cont = 1;

	to.sin_family = AF_INET;

	// try to convert as dotted decimal address, else if that fails assume it's a hostname
	to.sin_addr.s_addr = inet_addr(target);
	if (to.sin_addr.s_addr != (u_int)-1)
		hostname = target;
	else
	{
		hp = gethostbyname(target);
		if (!hp)
		{
			printf("unknown host %s",target);
			return 0;
		}
		to.sin_family = hp->h_addrtype;
		bcopy(hp->h_addr, (caddr_t)&to.sin_addr, hp->h_length);
		strncpy(hnamebuf, hp->h_name, sizeof(hnamebuf) - 1);
		hostname = hnamebuf;
	}
	packlen = datalen + MAXIPLEN + MAXICMPLEN;
	if ( (packet = (u_char *)malloc((u_int)packlen)) == NULL)
	{
		printf("malloc error\n");
		return 0;
	}

/*
	if ( (proto = getprotobyname("icmp")) == NULL)
	{
		cerr << "unknown protocol icmp" << endl;
		return -1;
	}
*/
	if ( (s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		perror("socket");	/* probably not running as superuser */
		return 0;
	}

	icp = (struct icmp *)outpack;
	icp->icmp_type = ICMP_ECHO;
	icp->icmp_code = 0;
	icp->icmp_cksum = 0;
	icp->icmp_seq = 12345;	/* seq and id must be reflected */
	icp->icmp_id = getpid();


	cc = datalen + ICMP_MINLEN;
	icp->icmp_cksum = in_cksum((unsigned short *)icp,cc);

	gettimeofday(&start, NULL);

	i = sendto(s, (char *)outpack, cc, 0, (struct sockaddr*)&to, (socklen_t)sizeof(struct sockaddr_in));
	if (i < 0 || i != cc)
	{
		if (i < 0)
			perror("sendto error");
		printf("wrote %s %d  chars, ret %d ",hostname,cc,i);
	}

	// Watch stdin (fd 0) to see when it has input.
	FD_ZERO(&rfds);
	FD_SET(s, &rfds);
	// Wait up to one seconds.
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	while(cont)
	{
		retval = select(s+1, &rfds, NULL, NULL, &tv);
		if (retval == -1)
		{
			perror("select()");
			return 0;
		}
		else if (retval)
		{
			fromlen = sizeof(struct sockaddr_in);
			if ( (ret = recvfrom(s, (char *)packet, packlen, 0,(struct sockaddr *)&from, (socklen_t*)&fromlen)) < 0)
			{
				//printf("recvfrom error");
				return 0;
			}

			// Check the IP header
			//ip = (struct ip *)((char*)packet);
			hlen = sizeof( struct ip );
			if (ret < (hlen + ICMP_MINLEN))
			{
				//printf("packet too short (%d bytes) from %s",ret,hostname);
				return 0;
			}

			// Now the ICMP part
			icp = (struct icmp *)(packet + hlen);
			if (icp->icmp_type == ICMP_ECHOREPLY)
			{
				//printf("Recv: echo reply");
				if (icp->icmp_seq != 12345)
				{
					//printf("received sequence # %d",icp->icmp_seq);
					continue;
				}
				if (icp->icmp_id != getpid())
				{
					//printf("received id %d",icp->icmp_id);
					continue;
				}
				cont = 0;
			}
			else
			{
				//printf("Recv: not an echo reply");
				continue;
			}

			gettimeofday(&end, NULL);
			end_t = 1000000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);

			if(end_t < 1)
				end_t = 1;

			//printf("Elapsed time = %d usec",end_t);
			return end_t;
		}
		else
		{
			//printf("No data within one seconds.\n");
			return 0;
		}
	}
	return 0;
}

uint16_t in_cksum(uint16_t *addr, unsigned len)
{
  uint16_t answer = 0;
  /*
   * Our algorithm is simple, using a 32 bit accumulator (sum), we add
   * sequential 16 bit words to it, and at the end, fold back all the
   * carry bits from the top 16 bits into the lower 16 bits.
   */
  uint32_t sum = 0;
  while (len > 1)  {
    sum += *addr++;
    len -= 2;
  }

  // mop up an odd byte, if necessary
  if (len == 1) {
    *(unsigned char *)&answer = *(unsigned char *)addr ;
    sum += answer;
  }

  // add back carry outs from top 16 bits to low 16 bits
  sum = (sum >> 16) + (sum & 0xffff); // add high 16 to low 16
  sum += (sum >> 16); // add carry
  answer = ~sum; // truncate to 16 bits
  return answer;
}
