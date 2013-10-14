#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <time.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

char destip[4][64] = { {"202.120.2.101"},{"122.226.192.4"},{"218.75.4.130"},{"210.72.145.44"}};

#define uint8   unsigned char 
#define uint32	unsigned long 

typedef struct __NTP_PACKET_
{
	uint8 	li_vn_mode 		;
	uint8 	stratum 		;
	uint8 	poll 			;
	uint8  	precision  		;
	uint32 	root_delay   	;
	uint32 	root_dispersion ;
	uint8 	ref_id[4]		;
	uint32	reftimestamphigh;
	uint32 	reftimestamplow	;
	uint32 	oritimestamphigh;
	uint32	oritimestamplow	;
	uint32	recvtimestamphigh;
	uint32 	recvtimestamplow;
	uint32 	trantimestamphigh;
	uint32 	trantimestamplow ;
}NtpPacket ;

int main()
{
	int sockfd ;
	NtpPacket ntpsend;
	NtpPacket ntprecv;
	struct tm tmtime ; 
	time_t localtime ,nettime;
	struct sockaddr_in server ;
	struct sockaddr_in client ;
	fd_set readfd ;
	struct timeval timeout ;
	int ret = 0 ;
	int i = 0 ;
	socklen_t addrlen = sizeof(struct sockaddr);
	time_t timetemp ;

	timetemp = time(NULL);
	printf("today time : %x  ---- %s.\n",timetemp,ctime(&timetemp));

/*
	bzero(&tmtime,sizeof(struct tm));
	tmtime.tm_year = 70; 
	tmtime.tm_mday  = 1 ;
	tmtime.tm_wday = 4 ;
*/

	sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	
	if(sockfd < 0)
	{
		perror("no socket");
		exit(EXIT_FAILURE);
	}

	printf("sockfd : %d.\n",sockfd);

	for(i = 0 ; i < 4 ;  i ++)
	{

		bzero(&server , sizeof(server));
		server.sin_family = AF_INET ;
		server.sin_port = htons(123);
		server.sin_addr.s_addr = inet_addr(destip[i]);
/*
		bzero(sendbuf,sizeof(sendbuf));
		sendbuf[0] = 0x1b ;
		time(&localtime);
		localtime = htonl(localtime + 0x83AA7E80);
		memcpy(sendbuf + 40 , &localtime,sizeof(localtime));
*/

		bzero(&ntpsend,sizeof(ntpsend));
		ntpsend.li_vn_mode = 0x1b ;
		ntpsend.oritimestamphigh = htonl(time(NULL) + 0x83AA7E80);  //本地时间格式
		

		ret = sendto(sockfd,&ntpsend,sizeof(ntpsend),0,(struct sockaddr *)&server,sizeof(struct sockaddr));

		if(ret < 0)
		{
			perror("Fail to send to ");
			exit(EXIT_FAILURE);
		}


		FD_ZERO(&readfd);
		FD_SET(sockfd,&readfd);
		timeout.tv_sec = 10;
		timeout.tv_usec =  0;

		ret = select(sockfd + 1 , &readfd , NULL,NULL ,&timeout) ;

		if(ret < 0)
		{
			perror("Falt to select  or timeout ");
			goto CONTINUE ;
		}

		if(ret == 0)
		{
			printf("ip:[%s] -time out .\n",destip[i]);
			goto CONTINUE ;
		}


		bzero(&ntprecv,sizeof(ntprecv));
		ret = recvfrom(sockfd ,&ntprecv,sizeof(ntprecv),0,(struct sockaddr *)&client,&addrlen);

		if(ret <=  0)
		{
			perror("Fail to recvfrom ");
			goto CONTINUE ;
		}
/*
		memcpy(&nettime,sendbuf + 40 , 4);
		nettime = nettime - 0x83AA7E80;
*/


		addrlen = sizeof(struct sockaddr);

		ntprecv.root_delay 			= ntohl(ntprecv.root_delay);
		ntprecv.root_dispersion 	= ntohl(ntprecv.root_dispersion);
		ntprecv.reftimestamphigh	= ntohl(ntprecv.reftimestamphigh);
		ntprecv.reftimestamplow 	= ntohl(ntprecv.reftimestamplow);
		ntprecv.oritimestamphigh 	= ntohl(ntprecv.oritimestamphigh);
		ntprecv.oritimestamplow 	= ntohl(ntprecv.oritimestamplow);
		ntprecv.recvtimestamphigh 	= ntohl(ntprecv.recvtimestamphigh);
		ntprecv.recvtimestamplow 	= ntohl(ntprecv.recvtimestamplow);
		ntprecv.trantimestamphigh 	= ntohl(ntprecv.trantimestamphigh);
		ntprecv.trantimestamplow 	= ntohl(ntprecv.trantimestamplow);

		printf("ntprecv[li-vn-mode]       : %x  .\n",ntprecv.li_vn_mode);
		printf("ntprecv[root_delay]       : %lu .\n",ntprecv.root_delay);
		printf("ntprecv[root_dispersion]  : %lu .\n",ntprecv.root_dispersion);
		printf("ntprecv[reftimestamphigh] : %lu .\n",ntprecv.reftimestamphigh);
		printf("ntprecv[reftimestamplow]  : %lu .\n",ntprecv.reftimestamplow);
		printf("ntprecv[oritimestamphigh] : %lu .\n",ntprecv.oritimestamphigh);
		printf("ntprecv[oritimestamplow]  : %lu .\n",ntprecv.oritimestamplow);
		printf("ntprecv[recvtimestamphigh]: %lu .\n",ntprecv.recvtimestamphigh);	//到达服务器的时间
		printf("ntprecv[recvtimestamplow] : %lu .\n",ntprecv.recvtimestamplow);	
		printf("ntprecv[trantimestamphigh]: %lu .\n",ntprecv.trantimestamphigh);  	//离开服务器的时间
		printf("ntprecv[trantimestamplow] : %lu .\n",ntprecv.trantimestamplow);

		ntprecv.recvtimestamphigh -= 0x83AA7E80 ;  //1900 - 1970 之间的时间秒数
		printf("server recvtimestamphigh time : %s:%lu.\n",ctime(&(ntprecv.recvtimestamphigh)),ntprecv.recvtimestamplow);

		ntprecv.trantimestamphigh -= 0x83AA7E80 ;  //1900 - 1970 之间的时间秒数
		printf("server transtimes time : %s:%lu.\n",ctime(&(ntprecv.trantimestamphigh)),ntprecv.trantimestamplow);

CONTINUE:
		usleep(1000);
	}

	close(sockfd);

	exit(EXIT_SUCCESS);
}
