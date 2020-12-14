#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int main()
{
	//open a socket file
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("sockfd = %d\n", sockfd);

	//connect the socketfd to the specified ip, 
	//and the specified protocol
	struct sockaddr_in cliaddr;
	cliaddr.sin_family = AF_INET;//ipv4 protocol
	cliaddr.sin_port = htons(8080);//port num 9000
	//specify ip address
	inet_pton(AF_INET, "192.168.1.99", &(cliaddr.sin_addr.s_addr));

	//connect to the server
	int ret = connect(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
	if(ret < 0)
	{
		perror("connect failed:");
		exit(0);
	}
	char buf[1024];
	strcpy(buf, "holy shit hhhhhhhhhhh");
	
	while(1)
	{
		ret = write(sockfd, buf, strlen(buf));
		printf("write success!\n");
		sleep(1);
		if(ret < 0)
		{
			perror("write failed");
		}
	}
	close(sockfd);
}
