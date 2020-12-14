#include <unistd.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include"../fun.h"
Ponline add_online_node(Ponline end,int confd,struct sockaddr_in caddr);
void show_online_link();
void write_to_client();
int link_length();
Ponline head=NULL;
Ponline add_online_node(Ponline end,int confd,struct sockaddr_in caddr)
{
	Ponline temp=(Ponline)malloc(sizeof(ONLINE));
	temp->confd=confd;
	temp->caddr=caddr;
	temp->next=NULL;
	if(end==NULL)
	{
		temp->id=1001;
		head=temp;
		return temp;
	}
	else
	{
		temp->id=end->id+1;
		end->next=temp;
		return temp;
	}
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

void write_to_client()
{
	Ponline p=head;
	Ponline q=NULL;
	int len=link_length();
	while(p!=NULL)
	{
		q=head;
		write(p->confd,&len,4);
		while(q!=NULL)
		{
			write(p->confd,q,sizeof(ONLINE));
			q=q->next;
		}

		p=p->next;
	}

}

int link_length()
{
	Ponline p=head;
	int len=0;
	while(p!=NULL)
	{
		len++;
		p=p->next;
	}
	return len;
}
void * fun(void *arg)
{
	int confd= *((int *)arg);
	char buf[1024]={0};
	memset(buf,0,1024);
	int ret;
	while(1)
	{
		ret=read(confd,buf,1024);//返回值等于0表示链接断开
		if(ret<0)
		{
			perror("read failed\n");
			//exit(0);
			break;
		}
		if(ret==0)
		{
			printf("客户端已经关闭\n");
			break;
		}
		printf("buf=%s\n",buf);

	}
	close(confd);
}
int main()
{
	int sockfd;
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in myaddr,caddr;
	myaddr.sin_family=AF_INET;
	myaddr.sin_port=htons(9000);
	inet_pton(AF_INET,"192.168.1.222",&myaddr.sin_addr.s_addr);
	int ret=bind(sockfd,(struct sockaddr *)&myaddr,sizeof(myaddr));
	if(ret<0)
	{
		perror("bind failed:");
		exit(0);
	}
	ret=listen(sockfd,5);
	if(ret<0)
	{
		perror("listen failed:");
		exit(0);
	}
	int len=sizeof(caddr);
	int confd;
	Ponline end=NULL;
	while(1)
	{
		confd=accept(sockfd,(struct sockaddr *)&caddr,&len);
		if(confd<0)
		{
			perror("connect failed\n");
			exit(0);
		}
		write(confd,&caddr,sizeof(caddr));
		printf("有人请求链接，创建线程处理读操作\n");
		end=add_online_node(end,confd,caddr);//添加节点
		show_online_link();	
		write_to_client();
		pthread_t pthread;
		pthread_create(&pthread,NULL,fun,(void *)&confd);
	}

	close(sockfd);
}
