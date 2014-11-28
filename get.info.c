#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
void cbs_sender_service(void *arg){

	int socket = *(int *)arg;
	printf("%s %d. pthread start.socket=%d\n",__FUNCTION__,__LINE__,socket);
}
int main(int argc,char **argv){
	pthread_t pid1,pid2,pid3,pid4;
	int ss = 99;
    int ret = pthread_create(&pid1,NULL,&cbs_sender_service,&ss);
	pthread_detach(thread_id)
	pthread_join(pid1,NULL);
	return 0;
}
int get_ip2(int argc,char **argv)
{
	int     sockfd;
	unsigned char *ptr;
	struct arpreq arpreq;
	struct sockaddr_in *sin;
	struct sockaddr_storage ss;
	char addr[INET_ADDRSTRLEN+1];

	memset(addr, 0, INET_ADDRSTRLEN+1);
	memset(&ss, 0, sizeof(ss));
	if (argc != 2) {
		printf("USAGE arpg  ip_addr/n");
		return -1;
	}
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		perror("socket error");
		return -1;
	}
	sin = (struct sockaddr_in *) &ss;
	sin->sin_family = AF_INET;
	if (inet_pton(AF_INET, argv[1], &(sin->sin_addr)) <= 0) {
		perror("inet_pton error");
		return -1;
	}
	sin = (struct sockaddr_in *) &arpreq.arp_pa;
	memcpy(sin, &ss, sizeof(struct sockaddr_in));
	strcpy(arpreq.arp_dev, "wlan0");
	arpreq.arp_ha.sa_family = AF_UNSPEC;
	if (ioctl(sockfd, SIOCGARP, &arpreq) < 0) {
		perror("ioctl SIOCGARP: ");
		return -1;
	}
	char buf[60];
	ptr = (unsigned char *)arpreq.arp_ha.sa_data;
	sprintf(buf,"%x:%x:%x:%x:%x:%x", *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5));
	printf("%s\n",buf);
	return 0;
}


int getpeermac( int sockfd)
{
	int ret =0;
	char buf[60];
	struct arpreq arpreq;
	struct sockaddr_in dstadd_in;
    socklen_t  len = sizeof( struct sockaddr_in );
    memset( &arpreq, 0, sizeof( struct arpreq ));
	memset( &dstadd_in, 0, sizeof( struct sockaddr_in ));
	if( getpeername( sockfd, (struct sockaddr*)&dstadd_in, &len ) < 0 )
		printf("getpeername()");
    else
	{
		memcpy( &arpreq.arp_pa, &dstadd_in, sizeof( struct sockaddr_in ));
		strcpy(arpreq.arp_dev, "eth1");
		arpreq.arp_pa.sa_family = AF_INET;
		arpreq.arp_ha.sa_family = AF_UNSPEC;
		if( ioctl( sockfd, SIOCGARP, &arpreq ) < 0 )
			printf("ioctl SIOCGARP");
		else
		{
			unsigned char* ptr = (unsigned char *)arpreq.arp_ha.sa_data;
            ret = sprintf(buf, "%02x%02x%02x%02x%02x%02x", *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5));
		}
	}
	printf("%s\n",buf);
	return ret;
}

int getpeeraddr(int sockfd)
{
	char peeraddrstr[60];
	char peerip[18];
	struct sockaddr_in peeraddr;
	socklen_t len = sizeof(peeraddr);//must set len. or it will return 0.0.0.0

	int ret = getpeername(sockfd, (struct sockaddr *)&peeraddr, &len);
	if (ret < 0)
		return 0;
	sprintf(peeraddrstr, "peer address: %s:%d\n\n", inet_ntop(AF_INET, &peeraddr.sin_addr, peerip, sizeof(peerip)), ntohs(peeraddr.sin_port));
	printf("%s\n",peeraddrstr);
	///////////////////////////
	//struct sockaddr_in sa;
	//int len = sizeof(sa);
	//if(!getpeername(sockfd, (struct sockaddr *)&sa, &len))
	{
		printf( "对方IP：%s ", inet_ntoa(peeraddr.sin_addr));
		printf( "对方PORT：%d ", ntohs(peeraddr.sin_port));
	}

	return 0;
}

int get_mac2(void) 
{
	int sockfd; 
	struct ifreq struReq; 
	sockfd = socket(PF_INET,SOCK_STREAM,0); 
	memset(&struReq,0,sizeof(struReq)); 

	strncpy(struReq.ifr_name, "eth0", sizeof(struReq.ifr_name)); 

	ioctl(sockfd,SIOCGIFHWADDR,&struReq); 
	//printf("%s\n", (char *)ether_ntoa(struReq.ifr_hwaddr.sa_data)); 
	unsigned char *u = ( unsigned char * )&struReq.ifr_hwaddr.sa_data;
	if ( u[0] + u[1] + u[2] + u[3] + u[4] + u[5] )
	{
		printf( "HW Address: %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
			u[0], u[1], u[2], u[3], u[4], u[5] );
	}
	
	strncpy(struReq.ifr_name, "wlan0", sizeof(struReq.ifr_name)); 

	ioctl(sockfd,SIOCGIFHWADDR,&struReq); 
	//printf("%s\n", (char *)ether_ntoa(struReq.ifr_hwaddr.sa_data)); 
	u = ( unsigned char * )&struReq.ifr_hwaddr.sa_data;
	if ( u[0] + u[1] + u[2] + u[3] + u[4] + u[5] )
	{
		printf( "HW Address: %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
			u[0], u[1], u[2], u[3], u[4], u[5] );
	}
	close(sockfd); 
}

int get_mac ()
{
#define inaddrr( x ) ( *( struct in_addr * )&ifr->x[ sizeof( sa.sin_port ) ] )
#define IFRSIZE      ( ( int )( size * sizeof( struct ifreq ) ) )

	unsigned char *    u;
	int                sockfd, size = 1;
	struct ifreq *     ifr;
	struct ifconf      ifc;
	struct sockaddr_in sa;

	if ( 0 > ( sockfd = socket( AF_INET, SOCK_DGRAM, IPPROTO_IP ) ) )
	{
		fprintf( stderr, "Cannot open socket.\n" );
		exit( EXIT_FAILURE );
	}
		ifc.ifc_req = NULL;
	do{
		++size;
		/* realloc buffer size until no overflow occurs  */
		if ( NULL == ( ifc.ifc_req = realloc( ifc.ifc_req, IFRSIZE ) ) )
		{
			fprintf( stderr, "Out of memory.\n" );
			exit( EXIT_FAILURE );
		}
			ifc.ifc_len = IFRSIZE;
		if ( ioctl( sockfd, SIOCGIFCONF, &ifc ) )
		{
			perror( "ioctl SIOCFIFCONF" );
			exit( EXIT_FAILURE );
		}
	} while ( IFRSIZE <= ifc.ifc_len );
    ifr = ifc.ifc_req;
	for ( ; ( char * )ifr < ( char * )ifc.ifc_req + ifc.ifc_len; ++ifr )
	{
		if ( ifr->ifr_addr.sa_data == ( ifr + 1 )->ifr_addr.sa_data )
		{
			continue;  /* duplicate, skip it */
		}
		if ( ioctl( sockfd, SIOCGIFFLAGS, ifr ) )
		{
			continue;  /* failed to get flags, skip it */
		}
		printf( "Interface:  %s\n", ifr->ifr_name );
		if ( 0 == ioctl( sockfd, SIOCGIFHWADDR, ifr ) )
		{
			switch ( ifr->ifr_hwaddr.sa_family )
			{
			case ARPHRD_NETROM:
			case ARPHRD_ETHER:
			case ARPHRD_PPP:
			case ARPHRD_EETHER:
			case ARPHRD_IEEE802:
				break;
			default:
				printf( "\n" );
				continue;
			}
			u = ( unsigned char * )&ifr->ifr_addr.sa_data;
			if ( u[0] + u[1] + u[2] + u[3] + u[4] + u[5] )
			{
				printf( "HW Address: %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
						u[0], u[1], u[2], u[3], u[4], u[5] );
			}
		}
		printf( "\n" );
	}  /* end of for */
    close( sockfd );
    return( EXIT_SUCCESS );
}


int get_ip()
{
	#define PORT 7778
	#define MAXDATASIZE 1024
	
	struct sockaddr_in user_addr,my_addr;
	char my_ip[13];
	int socket_fd;
	int so_broadcast=1;
	char buf[MAXDATASIZE];

	socklen_t size;

	my_addr.sin_family=AF_INET;
	my_addr.sin_port=htons(PORT);
	my_addr.sin_addr.s_addr=inet_addr("255.255.255.255");


	user_addr.sin_family=AF_INET;
	user_addr.sin_port=htons(PORT);
	user_addr.sin_addr.s_addr=htonl(INADDR_ANY);


	if((socket_fd=(socket(AF_INET,SOCK_DGRAM,0)))==-1) {
		perror("socket");
		exit(1);
	}
	setsockopt(socket_fd,SOL_SOCKET,SO_BROADCAST,&so_broadcast,sizeof(so_broadcast));//??socket发送的数据具有广播特性

	if((bind(socket_fd,(struct sockaddr *)&user_addr, sizeof(struct sockaddr)))==-1) {
		perror("bind");
		exit(1);
	}

	strcpy(buf,"broadcast test");
	sendto(socket_fd,buf,strlen(buf),0,(struct sockaddr *)&my_addr,sizeof(my_addr));
	size=sizeof(user_addr);
	recvfrom(socket_fd,buf,MAXDATASIZE,0,(struct sockaddr *)&user_addr,&size);

	strcpy(my_ip,inet_ntoa(user_addr.sin_addr));
	printf("my_ip:%s\n",inet_ntoa(user_addr.sin_addr));
}


