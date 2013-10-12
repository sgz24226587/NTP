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
	char sendbuf[300];
	struct tm tmtime ; 
	time_t localtime ,nettime;
	struct sockaddr_in server ;
	struct sockaddr_in client ;
	fd_set readfd ;
	struct timeval timeout ;
	int ret = 0 ;
	int i = 0 ;
	socklen_t addrlen = sizeof(struct sockaddr);


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

		bzero(sendbuf,sizeof(sendbuf));
		sendbuf[0] = 0x1b ;
		time(&localtime);
		localtime = htonl(localtime + 0x83AA7E80);
		memcpy(sendbuf + 40 , &localtime,sizeof(localtime));

		ret = sendto(sockfd,sendbuf,48,0,(struct sockaddr *)&server,sizeof(struct sockaddr));

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


		bzero(sendbuf,sizeof(sendbuf));
		ret = recvfrom(sockfd ,sendbuf,sizeof(sendbuf),0,(struct sockaddr *)&client,&addrlen);

		if(ret <=  0)
		{
			perror("Fail to recvfrom ");
			goto CONTINUE ;
		}

		memcpy(&nettime,sendbuf + 40 , 4);
		nettime = nettime - 0x83AA7E80;

		printf("net time : %s ", ctime(&nettime));
		addrlen = sizeof(struct sockaddr);


CONTINUE:
		usleep(1000);
	}

	close(sockfd);

	exit(EXIT_SUCCESS);
}
