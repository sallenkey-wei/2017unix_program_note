#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define MAX_QUEUE 20

int sockfds[2] = {1,1};

void * pthread_action(void *);

int main()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in myaddr;  
	myaddr.sin_port = htons(8080);
	myaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "192.168.1.99", &(myaddr.sin_addr));
    //int bind(int sockfd, const struct sockaddr *addr,
	//              socklen_t addrlen
	int ret = bind(sockfd, (struct sockaddr *)&myaddr, sizeof(myaddr));
	if(ret < 0)
	{
		perror("bind failed");
		exit(0);
	}
	ret = listen(sockfd, MAX_QUEUE);
	if(ret < 0)
	{
		perror("listen failed");
		exit(0);
	}
	
	struct sockaddr_in client_addr;
	socklen_t lenth = sizeof(client_addr);
	int newsockfd;
	int i = 0;
	while(1)
	{
		newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &lenth);
		sockfds[i%2] = newsockfd;
		i++;
		if(ret == -1)
		{
			perror("accept failed");
		}
		pthread_t pthid;
		pthread_create(&pthid, NULL, pthread_action, &newsockfd);
	}
}

void * pthread_action(void * temp)
{
	int sockfd = *((int *)temp);
	int ret;
	char buf[1024];
	while(1)
	{
		ret = read(sockfd, buf, 1024);
		if(ret < 0)
		{
			perror("read failed");
		}
		else if(ret == 0)
		{
			printf("client is disconnect");
			if(sockfds[0] != sockfd)
				sockfds[1] = 1;
			else
				sockfds[0] = 1;
			break;
		}
		else
		{
			if(sockfds[0] != sockfd)
				write(sockfds[0], buf, strlen(buf)+1);
			else
				write(sockfds[1], buf, strlen(buf)+1);
		}
	}
}

