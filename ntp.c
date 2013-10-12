#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

char destip[3][64] = { {"114.80.81.1"},{"122.226.192.4"},{"218.75.4.130"}};



int main()
{
	int sockfd ;

	sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	
	if(sockfd < 0)
	{
		perror("no socket");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
