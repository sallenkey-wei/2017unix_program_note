#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in seraddr;
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(8080);
	inet_pton(AF_INET, "192.168.1.99", &(seraddr.sin_addr));
	int lenth = sizeof(struct sockaddr_in);
	int ret = connect(sockfd, (struct sockaddr *)&seraddr, lenth);	
	if(ret < 0)
	{
		perror("connect failed");
		exit(0);
	}
	pid_t pid = fork();
	if(pid == 0)
	{
		char buf[1024];
		while(1)
		{
			printf("please input data:");
			fgets(buf, 1024, stdin);
			ret = write(sockfd, buf, strlen(buf)+1);
			if(ret < 0)
			{
				perror("write failed");
			}
			else
			{
				printf("write succeed!\n");
			}
		}
	}
	else if(pid > 0)
	{
		char buf[1024] = {0};
		while(1)
		{
			ret = read(sockfd, buf, 1024);
			if(ret < 0)
			{
				perror("read failed");
			}
			else if(ret == 0)
			{
				printf("server is disconnect!\n");
				exit(0);
			}
			else
			{
				printf("Data receive: ");
				printf("%s\n", buf);
			}
		}
	}
}
