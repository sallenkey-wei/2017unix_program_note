#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

void * receive_fun(void * temp);

int main()
{
	//步骤一
	//open a socket file and access the file descriptor
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	//步骤二
	//绑定该服务器的ip和端口号到socket文件的文件描述符
	struct sockaddr_in myaddr;
	struct sockaddr_in cliaddr;
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(8080);
	//参数1:ipv4
	//参数2：地址族
	//参数3：地址族大小
	inet_pton(AF_INET, "192.168.1.99", &myaddr.sin_addr.s_addr);
	int ret = bind(sockfd, (struct sockaddr *)&myaddr, sizeof(myaddr));
	if(ret < 0)
	{
		perror("bind failed");
		exit(0);
	}
	
	//步骤三：开始监听
	//参数1：套接字文件
	//参数2：等待链接的队列大小
	ret = listen(sockfd, 20);
	if(ret < 0)
	{
		perror("listen failed");
		exit(0);
	}
	//printf("end of listen\n");
	
	int len = sizeof(cliaddr);
	//步骤四：接受请求,返回一个新的socket文件描述符
	//接收客户端的访问
	//到此阻塞，等待客户端的访问
	int confd; 
	while(1)
	{
		confd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
		if(confd < 0)
		{
			perror("connect failed");
			//exit(0);
		}
		pthread_t pthid;
		pthread_create(&pthid, NULL, receive_fun, (void *)&confd);
	}
}

void * receive_fun(void * temp)
{
	int confd = *((int *)temp);
	char buf[1024];
	int ret;
	while(1)
	{
		ret = read(confd, buf, 1024);
		if(ret < 0)
		{
			perror("read failed");
			//exit(0);
		}
		else if(ret == 0)
		{
			printf("客户端链接已断开\n");
			break;
		}
		else
		{
			printf("ret = %d\n", ret);
			printf("buf = %s\n", buf);
			char * snd = "服务器已收到";
			ret = write(confd, snd, strlen(snd));
			if(ret < 0)
			{
				perror("write failed");
			}
		}
	}
	close(confd);
	pthread_exit(NULL);
}
