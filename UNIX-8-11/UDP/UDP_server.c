#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

int main()
{
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in seraddr;
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(8080);
	inet_pton(AF_INET, "192.168.1.99", &seraddr.sin_addr);
	int ret = bind(sockfd, (struct sockaddr *)&seraddr, sizeof(struct sockaddr_in));
	if(ret < 0)
	{
		perror("bind failed");
		return -1;
	}
	char buf[1024]={0};
	struct sockaddr_in src_addr;
	int len = sizeof(struct sockaddr_in);
	ret = recvfrom(sockfd, buf, 1024, 0, (struct sockaddr *)&src_addr, &len);
	if(ret < 0)
	{
		perror("recieve failed");
	}
	else if(ret == 0)
	{
		printf("receive ret=0\n");
	}
	else
	{
		printf("buf = %s\n", buf);
	}
	close(sockfd);	
}
