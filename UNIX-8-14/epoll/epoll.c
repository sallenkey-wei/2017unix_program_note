#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/epoll.h>

int main()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in myaddr;
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(8080);
	inet_pton(AF_INET, "192.168.88.130", &(myaddr.sin_addr.s_addr));
	int ret;
	ret = bind(sockfd, (struct sockaddr *)&myaddr, sizeof(struct sockaddr_in));
	if(ret < 0)
	{
		perror("bind failed");
		exit(0);
	}
	listen(sockfd, 20);

	int epfd = epoll_create(100);
	//添加socket文件描述符到监视列表中
	struct epoll_event even, evens[20];
	even.data.fd = sockfd;
	even.events = EPOLLIN|EPOLLET;
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &even);
	if(ret < 0)
	{
		perror("epoll_add failed");
		exit(0);
	}
	while(1)
	{
		ret = epoll_wait(epfd, evens, 20, 1000);//阻塞状态维持1000ms
		if(ret == 0)
		{
			printf(".");
			fflush(stdout);
			continue;
		}
		else if(ret < 0)
		{
			perror("epoll_wait failed");
			exit(0);
		}
		for(int i=0; i<ret; i++)
		{
			if(evens[i].data.fd == sockfd)
			{
				printf("client requset connect\n");
				struct sockaddr_in cliaddr;
				int len = sizeof(struct sockaddr_in);
				int confd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
				even.events = EPOLLIN|EPOLLET;
				even.data.fd = confd;
				epoll_ctl(epfd, EPOLL_CTL_ADD, confd, &even);
			}
			else if(evens[i].events & EPOLLIN)
			{
				char buf[1024] = {0};
				int ret1;
				int fd = evens[i].data.fd;
				ret1 = read(fd, buf, 1024);
				if(ret1<0)
				{
					perror("read failed");
					even.events = EPOLLIN|EPOLLET;
					even.data.fd = fd;
					epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &even);
				}
				else if(ret1 == 0)
				{
					printf("client disconnect\n");
					even.events = EPOLLIN|EPOLLET;
					even.data.fd = fd;
					epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &even);
				}
				else
					printf("buf = %s\n", buf);
			}
		}
	}
}
