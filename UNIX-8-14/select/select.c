#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/select.h>
#include <stdlib.h>


int main()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	int ret;
	struct sockaddr_in myaddr, caddr;
	char buf[1024] = {0};
	myaddr.sin_port = htons(8080);
	myaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "192.168.88.130", &(myaddr.sin_addr.s_addr)); 
	ret = bind(sockfd, (struct sockaddr *)&myaddr, sizeof(myaddr));

	listen(sockfd, 20);

	fd_set set, reset;//select函数会改变文件描述符集合，所以定义
	//两个集合，做好备份
	FD_ZERO(&set);
	FD_SET(sockfd, &set);
	struct timeval time;
	int max_fds = sockfd;
	int len = sizeof(struct sockaddr_in);

	while(1)
	{
		time.tv_sec = 2;
		time.tv_usec = 0;
		reset = set;
		ret = select(max_fds+1, &reset, NULL, NULL, &time);
		printf("ret = %d\n", ret);
		if(ret == 0)
		{
			printf(".");	
			fflush(stdout);
			continue;
		}
		else if(ret < 0)
		{
			perror("select failed");
			exit(0);
		}
		if(FD_ISSET(sockfd, &reset))
		{
			printf("有人请求链接\n");
			int confd = accept(sockfd, (struct sockaddr *)&caddr, &len);
			if(confd < 0)
			{
				perror("connect failed");
				exit(0);
			}
			FD_SET(confd, &set);
			if(confd > max_fds)
				max_fds = confd;
		}
		for(int fd=0; fd<max_fds+1; fd++)
		{
			if(fd == sockfd)
				continue;
			else if(!FD_ISSET(fd, &reset))
				continue;
			int ret1 = read(fd, buf, 1024);
			if(ret1 < 0)
			{
				perror("read failed");
				FD_CLR(fd, &set);
				continue;
			}
			else if(ret1 == 0)
			{
				printf("client disconnect\n");
				FD_CLR(fd, &set);
				continue;
			}
			else
				printf("buf=%s\n", buf);
		}
	}
}
