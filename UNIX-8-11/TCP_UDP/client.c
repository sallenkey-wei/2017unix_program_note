#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <string.h>

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
int myid = 0;
C_node mycnode;

int socket_init();
void add_list();
C_node * Create_list(C_node * phead);
void output(C_node * phead);
void destroy_list(C_node *phead);
void * server_con(void * argv);
void receive_from_server(int sockfd);
void * send_cli(void * argv);
C_node *search(C_node * phead, int id);

int main()
{
	int sockfd = socket_init();
	int ret = read(sockfd, &(mycnode), sizeof(C_node));//mycnode,myid全局变量
	if(ret < 0)
	{
		perror("read failed");
		exit(0);
	}
	else if(ret == 0)
	{
		printf("server is closed\n");
		exit(0);
	}
	else
	{
		myid = mycnode.cliattr.id;
	}
	//output own info
	char buf[16];
	inet_ntop(AF_INET, &(mycnode.cliattr.cliaddr.sin_addr.s_addr), buf, 16);
	int sin_port = ntohs(mycnode.cliattr.cliaddr.sin_port);
	printf("myid = %d, address=%s, port=%d\n", myid, buf, sin_port);

	pthread_t thread;
	pthread_create(&thread, NULL, server_con, &sockfd);

	int sockfd1 = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in myaddr = mycnode.cliattr.cliaddr;


	ret = bind(sockfd1, (struct sockaddr *)&myaddr, sizeof(myaddr));
	if(ret < 0)
	{
		perror("bind failed");
		exit(0);
	}
	pthread_t thread2;
	pthread_create(&thread2, NULL, send_cli, &sockfd1);

	char buf1[1024];
	struct sockaddr_in srcaddr;
	int len = sizeof(struct sockaddr_in);
	while(1)
	{
		ret = recvfrom(sockfd1, buf1, 1024, 0, (struct sockaddr *)&srcaddr, &len);
		if(ret < 0)
		{
			perror("recv failed");
		}
		else
			printf("recv message: %s\n\n", buf1);
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

void add_list(C_node * phead, C_node * ptemp)
{
	C_node * pnode = (C_node *)malloc(sizeof(C_node));
	*pnode = *ptemp;
	pnode->next = phead->next;
	phead->next = pnode;
}

void output(C_node * phead)
{
	char buf[16] = {0};
	printf("online id as follow:\n");
	while(phead->next != NULL)
	{
		inet_ntop(AF_INET,&(phead->next->cliattr.cliaddr.sin_addr.s_addr),buf, 16);
		printf("id = %d, address = %s, sin_port = %d\n",phead->next->cliattr.id, buf, 
				ntohs(phead->next->cliattr.cliaddr.sin_port));
		phead = phead->next;
	}
	printf("\n");
}

void destroy_list(C_node *phead)
{
	C_node * pbackup = phead;
	C_node * temp;
	if(phead != NULL)
	{
		phead = phead->next;
		while(phead != NULL)
		{
			temp = phead->next;
			free(phead);
			phead = temp;
		}
		pbackup->next = NULL;
	}
}

C_node * search(C_node * phead, int id)
{
	phead = phead->next;
	while(phead != NULL)
	{
		if(phead->cliattr.id == id)
			break;
		phead = phead->next;
	}
	return phead;
}

int socket_init()
{
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in seraddr;
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(8080);
	inet_pton(AF_INET, "192.168.88.130", &seraddr.sin_addr);
	int ret;
	ret = connect(sockfd, (struct sockaddr *)&seraddr, sizeof(seraddr));
	if(ret < 0)
	{
		perror("connect failed");
		exit(0);
	}
	return sockfd;
}

void * server_con(void * argv)
{
	int sockfd = *((int *)argv);

	phead = Create_list(phead);
	while(1)
	{
		receive_from_server(sockfd);
		output(phead);
	}
	pthread_exit(NULL);
}

void receive_from_server(int sockfd)
{
	C_node cnode;
	int ret;
	int flag = 1;
	while(1)
	{
		ret = read(sockfd, &(cnode), sizeof(C_node));	
		if(ret < 0)
		{
			perror("read failed");
			break;
		}
		else if(ret == 0)
		{
			printf("server is closed\n");
			break;
		}
		else
		{
			if(flag)
			{
				flag = 0;
				destroy_list(phead);
			}
			add_list(phead, &cnode);
			if(cnode.next == NULL)
				break;
		}
	}
}

void * send_cli(void * argv)
{
	int sockfd = *((int *)argv);
	int id;
	C_node * ptmp;
	char buf[1024];
	int ret;
	while(1)
	{
		printf("please input id the other side:\n");
		scanf("%d", &id);
		while(getchar() != '\n')
			continue;
		ptmp = search(phead, id);
		if(ptmp == NULL)
		{
			printf("the id is not online\n\n");
			continue;
		}
		printf("please input message you'll send:\n");
		scanf("%s", buf);
		while(getchar() != '\n')
			continue;
		ret = sendto(sockfd, buf, strlen(buf)+1, 0, (struct sockaddr *)&(ptmp->cliattr.cliaddr), 
				sizeof(struct sockaddr_in));
		if(ret < 0)
			perror("send failed");
	}
		
}
