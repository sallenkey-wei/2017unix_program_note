#include "epoll.h"
#include <sys/epoll.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

int Epoll_creat()
{
	int epfd = epoll_create1(0);
	if(epfd == -1)
	{
		perror("epoll create failed");
		exit(1);
	}
	return epfd;
}

void Epoll_ctl(int epfd, int op, int fd, struct epoll_event * event)
{
	int ret = epoll_ctl(epfd, op, fd, event);
	if(ret < 0)
	{
		perror("epoll ctl failed");	
		exit(1);
	}
}

int Epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout)
{
	int ret = epoll_wait(epfd, events, maxevents, timeout);
	if(ret < 0)
	{
		perror("epoll wait failed");
		exit(1);
	}
	return ret;
}
