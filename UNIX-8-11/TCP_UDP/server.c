#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/epoll.h>

struct client_attr
{
	struct sockaddr_in cliaddr; 
	int id;
};

typedef struct client_node
{
	struct client_attr cliattr;
	int sockfd;
	struct client_node * next;
}C_node;

C_node * phead = NULL;

C_node * Create_list(C_node * phead);
void add_list(C_node * phead, C_node *pnode);
void write_cli();
void del_list(C_node * phead, int fd);
int bind_listen();

int main()
{
	int sockfd = bind_listen();
	C_node cnode;
	int lenth = sizeof(struct sockaddr_in);
	phead = Create_list(phead);
	
	//将sockfd添加到epfd，文件描述符集合中去
	int epfd = epoll_create(256);
	struct epoll_event even, evens[20];
	even.events = EPOLLIN | EPOLLET;
	even.data.fd = sockfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &even);
	char buf[1024] = {0};
	int ret;
	while(1)
	{
		ret = epoll_wait(epfd, evens, 20, -1);//等待时间设为-1,表示无穷大，即一直等待
		if(ret < 0)
		{
			perror("epoll_wait failed");
			exit(0);
		}
		else if(ret == 0)
		{
			printf("time out\n");
		}
		else
		{
			for(int i=0; i<ret; i++)
			{
				if(evens[i].data.fd == sockfd)
				{
					printf("request for connect\n");
					cnode.sockfd = accept(sockfd, (struct sockaddr *)&cnode.cliattr.cliaddr, &lenth);
					even.data.fd = cnode.sockfd;
					even.events = EPOLLIN | EPOLLET;
					epoll_ctl(epfd, EPOLL_CTL_ADD, cnode.sockfd, &even);
					add_list(phead, &cnode);
					write(cnode.sockfd, phead->next, sizeof(C_node));
					write_cli();
					continue;
				}
				else if(evens[i].events & EPOLLIN)
				{
					int ret1;
					int fd = evens[i].data.fd; 
					//用read函数检测client是否断开
					ret1 = read(fd, buf, 1024);
					if(ret1 < 0)
					{
						perror("read failed");
						even.data.fd = fd;
						even.events = EPOLLIN | EPOLLET;
						epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &even);
						del_list(phead, fd);
						write_cli();
					}
					else if(ret1 == 0)
					{
						printf("client disconnect!\n");
						even.data.fd = fd;
						even.events = EPOLLIN | EPOLLET;
						epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &even);
						del_list(phead, fd);
						write_cli();
					}
				}
			}
		}
	}
}

C_node * Create_list(C_node * phead)
{
	if(phead == NULL)
	{
		phead = (C_node *)malloc(sizeof(C_node));
		phead->next = NULL;
	}
	return phead;
}

void add_list(C_node * phead, C_node *ptemp)
{
	C_node * pnode = (C_node *)malloc(sizeof(C_node));
	*pnode = *ptemp; 
	if(phead->next == NULL)
	{
		pnode->cliattr.id = 1001;
	}
	else
	{
		pnode->cliattr.id = phead->next->cliattr.id + 1;
	}
	pnode->next = phead->next;
	phead->next = pnode;
}

void del_list(C_node * phead, int fd)
{
	C_node * p;
	if(phead != NULL)
		while(phead->next != NULL)
		{
			if(phead->next->sockfd == fd)
			{
				p = phead->next;
				phead->next = phead->next->next;	
				free(p);
				break;
			}
			else
				phead = phead->next;
		}
}

void write_cli()
{
	C_node * head, *head2;
	head2 = head = phead->next;	
	while(head2 != NULL)
	{
		while(head != NULL)
		{
			write(head2->sockfd, head, sizeof(C_node));
			head = head->next;
		}
		head = phead->next;
		head2 = head2->next;
	}
}

int bind_listen()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in myaddr;
	myaddr.sin_port = htons(8080);
	myaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "192.168.88.130", &(myaddr.sin_addr));
	int ret = bind(sockfd, (struct sockaddr *)&myaddr, sizeof(struct sockaddr));	
	if(ret < 0)
	{
		perror("bind failed");
	}
	ret = listen(sockfd, 20);
	if(ret < 0)
	{
		perror("listen failed");
	}
	return sockfd;
}
