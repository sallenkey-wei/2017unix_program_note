#include <stdio.h>
#include "socket.h"
#include "init.h"
#include "comtype.h"
#include "epoll.h"
#include <sys/epoll.h>
#include <pthread.h>
#include "linklist.h"
#include "server_action.h"

static int epoll_init(int sockfd);
void * pthread_receive(void * argv);

Online_node * POnline;
pthread_mutex_t mutex;
int epfd;

int main()
{
	int sockfd = server_init();
	epfd = epoll_init(sockfd);
	pthread_mutex_init(&mutex ,NULL);
	POnline = Create_list(POnline);
	struct epoll_event events[256], event;
	while(1)
	{
		int ret = epoll_wait(epfd, events, 256, -1);
		for(int i = 0; i < ret; i++)
		{
			if(events[i].data.fd == sockfd)	
			{
				struct sockaddr_in addr;
				int confd = Accept(sockfd, &addr);
				event.events = EPOLLET | EPOLLIN;	
				event.data.fd = confd;
				epoll_ctl(epfd, EPOLL_CTL_ADD, confd, &event);
			}
			else if(events[i].events & EPOLLIN)
			{
				pthread_t thread;
				pthread_create(&thread, NULL, pthread_receive, &(events[i].data.fd));
			}
		}
	}
}

static int epoll_init(int sockfd)
{
	int epfd = Epoll_creat();	
	struct epoll_event event;
	event.data.fd = sockfd;
	event.events = EPOLLIN|EPOLLET;
	Epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);
	return epfd;
}

void * pthread_receive(void * argv)
{
	int confd = *((int *)argv);
	int optype = -1;	
	void * buf;
	//Read_server(confd, &optype, &buf);

	if(Read_server(confd, &optype, &buf) != -1)
	{
		analysis_receive(confd, optype, buf);
	}
}

