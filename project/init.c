#include "init.h"
#include "socket.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LISTEN 256

static int conf_file_analysis(char * ip, unsigned short int * pport, int conf_type)
{
	int fd;
	if(conf_type == SERVER_CONF)
		fd = open("./server.conf", O_RDONLY);
	else if(conf_type == CLIENT_CONF)
		fd = open("./client.conf", O_RDONLY);
	else
	{
		fprintf(stderr, "The argument conf_type is wrong\n");
		return -1;
	}
	
	char buf[1024];
	char port[6];
	int lenth;
	while((lenth = read(fd, buf, 1024)) > 0)
	{
		for(int i=0; i<lenth; ) 
		{
			if(buf[i] == '#')
			{
				i++;
				while(buf[i] != '\n')
					i++;
				i++;
			}
			else if(buf[i] == '\n' || buf[i] == ' ')
				i++;
			else if(buf[i] == 'i')
			{
				i++;
				int j = 0;
				if(buf[i++]=='p' && buf[i++]=='=' && buf[i++]=='\"')
				{
					for(j=0; buf[i]!='\"' && j<15; j++, i++)
					{
						ip[j] = buf[i];
					}
					ip[j] = '\0';
				}
				i++;
			}
			else if(buf[i] == 'p')
			{
				i++;
				int j = 0;
				if(buf[i++]=='o' && buf[i++]=='r' && buf[i++]=='t' && buf[i++]=='=')
				{
					for(j=0; buf[i]!='\n'&&buf[i]!=' ' && j<5; j++, i++)
					{
						port[j] = buf[i];
					}
				}
				port[j] = '\0';
				i++;
				*pport = (unsigned short int)atoi(port);
			}
			else
			{
				fprintf(stderr, "Analysis network configuration file failed!\n");
				return -1;
			}
		}
	}
	return 0;
}

#ifdef SOCK_FOR_CLIENT

int client_init()
{
	int sockfd = Socket(SOCK_STREAM);
	struct sockaddr_in seraddr;
	unsigned short int port;
	char ip[16] = {0};
	if(!conf_file_analysis(ip, &port, CLIENT_CONF) == 0)
	{
		exit(1);
	}
	struct_sockaddr_init(&seraddr, port, ip);
	Connect(sockfd, &seraddr);	
	return sockfd;
}

#else

int server_init()
{
	int sockfd = Socket(SOCK_STREAM);
	struct sockaddr_in myaddr;
	unsigned short int port;
	char ip[16] = {0};
	if(!conf_file_analysis(ip, &port, SERVER_CONF) == 0)
	{
		exit(1);
	}
	struct_sockaddr_init(&myaddr, port, ip);
	//myaddr.sin_addr.s_addr = INADDR_ANY;
	Bind(sockfd, &myaddr);
	Listen(sockfd, MAX_LISTEN);	
	return sockfd;
}

#endif
