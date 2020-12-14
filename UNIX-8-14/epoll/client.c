#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>


int main()
{
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in seraddr;
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(8080);
	inet_pton(AF_INET, "192.168.88.130", &seraddr.sin_addr);
	int ret;
	ret = connect(sockfd, (struct sockaddr *)&seraddr, sizeof(seraddr));
	if(ret < 0)
	{
		perror("connect failed");
		exit(0);
	}
	char buf[1024];
	while(1)
	{
		printf("please input your message:");
		scanf("%s", buf);
		write(sockfd, buf, strlen(buf)+1);
	}

}

