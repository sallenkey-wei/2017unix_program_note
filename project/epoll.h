#ifndef _EPOLL_H
#define _EPOLL_H

#include <sys/epoll.h>

int Epoll_creat();
void Epoll_ctl(int epfd, int op, int fd, struct epoll_event * event);
int Epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout);

#endif
