
#include<string.h>
#include <unistd.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include"../fun.h"
Ponline head=NULL;
void recv_from_server(int sockfd);
void show_online_link();
void free_link();



void free_link()
{
	Ponline p=NULL;
	while(head!=NULL)
	{
		p=head->next;
		free(head);
		head=p;
	}
}
void recv_from_server(int sockfd)
{
	int len;
	read(sockfd,&len,4);
	free_link();
	Ponline temp=NULL;
	for(int i=0;i<len;i++)
	{
		temp=(Ponline)malloc(sizeof(ONLINE));
		read(sockfd,temp,sizeof(ONLINE));
		temp->next=head;
		head=temp;
	}
	show_online_link();
}

void show_online_link()
{
	Ponline p=head;
	char buf[20]={0};
	printf("id\tconfd\tport\tip\n");
	while(p!=NULL)
	{
		printf("%d\t%d\t%d\t%s\n",p->id,p->confd,ntohs(p->caddr.sin_port),\
				inet_ntop(AF_INET,&(p->caddr.sin_addr.s_addr),buf,20));
		p=p->next;
	}

}

void *fun(void *arg)
{
	int sockfd=*((int *)arg);
	
	while(1)
	{
		recv_from_server(sockfd);
	}
}

Ponline  search_node(int id)
{
	Ponline p=head;
	while(p!=NULL)
	{
		if(p->id==id)
		{
			return p;
		}
		p=p->next;
	}
	return NULL;
}

void * fun1(void * arg)
{
	int udpfd=*((int *)arg);
	int id;
	char buf[1024]={0};
	Ponline p=NULL;
	while(1)
	{
		printf("请输入你要发送的人的id:");
		scanf("%d",&id);
		p=search_node(id);
		if(p==NULL)
		{
			printf("你输入的人不存在\n");
			continue;
		}
		printf("请输入你要发送的信息:");
		scanf("%s",buf);
		sendto(udpfd,buf,strlen(buf)+1,0,(struct sockaddr *)&p->caddr,sizeof(p->caddr));
	}
}
int main()
{
	int sockfd=socket(AF_INET,SOCK_STREAM,0);
	printf("sockfd=%d\n",sockfd);		
	struct sockaddr_in seraddr;
	seraddr.sin_family=AF_INET;
	seraddr.sin_port=htons(9000);
	inet_pton(AF_INET,"192.168.1.222",&(seraddr.sin_addr.s_addr));

	int ret=connect(sockfd,(struct sockaddr *) &seraddr,sizeof(seraddr));
	if(ret<0)
	{
		perror("connect faild");
		exit(0);
	}
	struct sockaddr_in myaddr;
	read(sockfd,&myaddr,sizeof(myaddr));
	printf("my port is %d\n",ntohs(myaddr.sin_port));

	pthread_t pthread,pthread1;
	pthread_create(&pthread,NULL,fun,&sockfd);//用于更新在线用户链表
	

	int UDPfd;
	UDPfd=socket(AF_INET,SOCK_DGRAM,0);
	//int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
	//int opt=1;
	//setsockopt(UDPfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	ret=bind(UDPfd,(struct sockaddr *)&myaddr,sizeof(myaddr));
	if(ret<0)
	{
		printf("udp bind failed\n");
		return -1;
	}
	pthread_create(&pthread1,NULL,fun1,&UDPfd);
	char buf[1024]={0};
	struct sockaddr_in  udpclient;
	int len=sizeof(udpclient);
	while(1)
	{
		recvfrom(UDPfd,buf,1024,0,(struct sockaddr *)&udpclient,&len);
		printf("buf=%s\n",buf);
	}
	
	close(sockfd);//结束
	return 0;
}
