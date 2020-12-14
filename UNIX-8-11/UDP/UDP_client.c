#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

int main()
{
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	char *buf = "hello";
	struct sockaddr_in seraddr;
	seraddr.sin_port = htons(8080);
	seraddr.sin_family = AF_INET;
	inet_pton(AF_INET, "192.168.1.99",&(seraddr.sin_addr));
	sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&seraddr, sizeof(struct sockaddr_in));
	close(sockfd);
}
