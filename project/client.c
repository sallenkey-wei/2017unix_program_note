#include <stdio.h>
#include "socket.h"
#include <unistd.h>
#include <string.h>
#include "init.h"
#include <pthread.h>
#include "comtype.h"
#include "linklist.h"
#include <stdlib.h>
#include "pwdmgmt.h"
#include "menushow.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

//链表的互斥锁
pthread_mutex_t mutex;
//iniface标志位的互斥锁
pthread_mutex_t mutex_iniface;
//行计数互斥锁
pthread_mutex_t mutex_count;

//在线用户链表
Online_node * POnline;
//client自己的信息
Online_node mine;
//client的udpfd
int mine_udpfd;
//是否在聊天界面
int iniface = 0;
//行计数
int row_count = 15;

void * pthread_tcp(void * argv);
void * pthread_udp(void * argv);
void analysis_receive(int tcpfd, int optype, void * buf);
void online_action(int tcpfd, Online_node * pnode);
void offline_action(int tcpfd, int * id);
void allow_disconnect_action();
void add_friend_action(int tcpfd, int id[]);
void allow_addf_action(int tcpfd, int id[]);
void send_message_mgmt();

int main()
{
	int tcpfd = client_init();
	Online_mine_init(tcpfd);
	pthread_mutex_init(&mutex, NULL);
	POnline = Create_list(POnline);
	wel_Iface_mgmt(tcpfd);

	pthread_t thread_tcp;
	pthread_create(&thread_tcp, NULL, pthread_tcp, (void *)&tcpfd);

	int udpfd = Socket(SOCK_DGRAM);	
	mine_udpfd = udpfd;
	struct sockaddr_in udpaddr;
	udpaddr.sin_family = AF_INET;
	udpaddr.sin_addr = mine.addr.sin_addr;
	udpaddr.sin_port = mine.addr.sin_port;	
	Bind(udpfd, &udpaddr);

	//printf("id = %d, \n", mine.id);
	//int port = ntohs(mine.addr.sin_port);
	//printf("port = %d\n", port);
	//char buf[20];
	//printf("ip = %s\n", inet_ntop(AF_INET, &(mine.addr.sin_addr.s_addr), buf, 20));

	pthread_t thread_udp;
	pthread_create(&thread_udp, NULL, pthread_udp, (void *)&udpfd);
	while(1)
	{
		char result = opera_Iface_mgmt();
		if(result == 'P')
		{
			show_list(POnline);
			mygetch();
		}
		else if(result == 'A')
		{
			system("clear");
			printf("please input the other side id:\n");
			int id;
			char RxData[7];
			data_input(RxData, 7);
			id = atoi(RxData);

			int send_id[2];
			send_id[0] = mine.id;
			send_id[1] = id;
			if(send_id[0] != send_id[1])
			{
				Write_client(tcpfd, OP_ADD_FRIEND, &send_id, 2*sizeof(int));
			}
			else
			{
				fprintf(stderr, "Can not make yourself as a friend!\n");
				mygetch();
			}
		}
		else if(result == 'S')
		{
			send_message_mgmt();

		}
		else if(result == 'E')
		{
			Write_client(tcpfd, OP_OFFLINE, &mine.id, sizeof(int));
		}

	}
}
void send_message_mgmt()
{
	char ifg;
	select_send_show();
	ifg = getchar();
	stdin_clr();
	if(ifg == 'S')
	{
		system("clear");
		printf("please input the friend's id:");
		char buf[7];
		data_input(buf, 7);
		int id = atoi(buf);
		send_message_Iface();
		while(1)
		{
			Online_node * pfriend = search_list(POnline, id, BY_ID);
			if(pfriend == NULL)
			{
				system("clear");
				printf("id%d not exist or not online\n", id);
				mygetch();
				pthread_mutex_lock(&mutex_iniface);
				iniface = 0;;
				pthread_mutex_unlock(&mutex_iniface);
				return;
			}
			pthread_mutex_lock(&mutex_iniface);
			iniface = 1;;
			pthread_mutex_unlock(&mutex_iniface);
			char RxData[76];
			if(data_input_msg(RxData, 76) != 0)
			{
				pthread_mutex_lock(&mutex_iniface);
				iniface = 0;;
				pthread_mutex_unlock(&mutex_iniface);
				return;
			}
			else
			{
				struct sockaddr_in friend_addr;
				friend_addr.sin_port = pfriend->addr.sin_port;
				friend_addr.sin_family = AF_INET;
				friend_addr.sin_addr = pfriend->addr.sin_addr;
				UDP_HEADER * pudp_header = (UDP_HEADER *)malloc(sizeof(UDP_HEADER)+strlen(RxData)+1);
				pudp_header->lenth = strlen(RxData)+1;
				pudp_header->id = mine.id;
				strcpy(pudp_header->data, RxData);
				int ret = sendto(mine_udpfd, pudp_header,sizeof(UDP_HEADER)+strlen(RxData)+1, 0, (struct sockaddr *)&friend_addr, sizeof(struct sockaddr_in));
				free(pudp_header);
				if(ret < 0)
					return;

				pthread_mutex_lock(&mutex_count);
				if(row_count <= 0)
				{
					row_count = 15;
					send_message_Iface();
				}
				MOVEUP(row_count+2);
				row_count--;
				printf("mine:%s\n", RxData);	
				MOVEDOWN(row_count+1);
				for(int i=0; i<75;i++)
				{
					printf(" ");
				}
				MOVEUP(1);
				printf("\n");
				pthread_mutex_unlock(&mutex_count);
			}
		}

	}
	else if(ifg == 'M')
	{
		system("clear");
		printf("please input receiver's num :");
		char buf[4];
		data_input(buf, 4);
		printf("input the receviver's id by order:\n");
		int num = atoi(buf);
		int id[256];
		char buf2[7];
		for(int i = 0; i < num; i++)
		{
			data_input(buf2, 7);
			id[i] = atoi(buf2);
		}
		send_message_Iface();
		while(1)
		{
			pthread_mutex_lock(&mutex_iniface);
			iniface = 1;;
			pthread_mutex_unlock(&mutex_iniface);
			char RxData[76];
			if(data_input_msg(RxData, 76) != 0)
			{
				pthread_mutex_lock(&mutex_iniface);
				iniface = 0;;
				pthread_mutex_unlock(&mutex_iniface);
				return;
			}
			else
			{
				struct sockaddr_in friend_addr;
				Online_node * ptemp;
				UDP_HEADER * pudp_header = (UDP_HEADER *)malloc(sizeof(UDP_HEADER)+strlen(RxData)+1);
				pudp_header->lenth = strlen(RxData)+1;
				pudp_header->id = mine.id;
				strcpy(pudp_header->data, RxData);
				for(int i = 0; i < num; i++)
				{
					ptemp = search_list(POnline, id[i], BY_ID);
					if(ptemp == NULL)
						continue;
					friend_addr.sin_port = ptemp->addr.sin_port;
					friend_addr.sin_family = AF_INET;
					friend_addr.sin_addr = ptemp->addr.sin_addr;
					int ret = sendto(mine_udpfd, pudp_header,sizeof(UDP_HEADER)+strlen(RxData)+1, 0, (struct sockaddr *)&friend_addr, sizeof(struct sockaddr_in));
					if(ret < 0)
						continue;
				}
				free(pudp_header);

				pthread_mutex_lock(&mutex_count);
				if(row_count <= 0)
				{
					row_count = 15;
					send_message_Iface();
				}
				MOVEUP(row_count+2);
				row_count--;
				printf("mine:%s\n", RxData);	
				MOVEDOWN(row_count+1);
				for(int i=0; i<75;i++)
				{
					printf(" ");
				}
				MOVEUP(1);
				printf("\n");
				pthread_mutex_unlock(&mutex_count);
			}
		}
	}
	else if(ifg == 'A')
	{
		system("clear");
		send_message_Iface();
		while(1)
		{
			pthread_mutex_lock(&mutex_iniface);
			iniface = 1;;
			pthread_mutex_unlock(&mutex_iniface);
			char RxData[76];
			if(data_input_msg(RxData, 76) != 0)
			{
				pthread_mutex_lock(&mutex_iniface);
				iniface = 0;;
				pthread_mutex_unlock(&mutex_iniface);
				return;
			}
			else
			{
				struct sockaddr_in friend_addr;
				UDP_HEADER * pudp_header = (UDP_HEADER *)malloc(sizeof(UDP_HEADER)+strlen(RxData)+1);
				pudp_header->lenth = strlen(RxData)+1;
				pudp_header->id = mine.id;
				strcpy(pudp_header->data, RxData);
				//send to all the friends
				pthread_mutex_lock(&mutex);
				Online_node * ptemp = POnline->next;
				while(ptemp != NULL)
				{
					friend_addr.sin_port = ptemp->addr.sin_port;
					friend_addr.sin_family = AF_INET;
					friend_addr.sin_addr = ptemp->addr.sin_addr;
					int ret = sendto(mine_udpfd, pudp_header,sizeof(UDP_HEADER)+strlen(RxData)+1, 0, (struct sockaddr *)&friend_addr, sizeof(struct sockaddr_in));
					if(ret < 0)
						return;
					ptemp = ptemp->next;
				}
				pthread_mutex_unlock(&mutex);
				free(pudp_header);

				pthread_mutex_lock(&mutex_count);
				if(row_count <= 0)
				{
					row_count = 15;
					send_message_Iface();
				}
				MOVEUP(row_count+2);
				row_count--;
				printf("mine:%s\n", RxData);	
				MOVEDOWN(row_count+1);
				for(int i=0; i<75;i++)
				{
					printf(" ");
				}
				MOVEUP(1);
				printf("\n");
				pthread_mutex_unlock(&mutex_count);
			}
		}
	}
}

void * pthread_tcp(void * argv)
{
	int tcpfd = *((int *)argv);
	int optype;
	void * buf;
	while(1)
	{
		Read_client(tcpfd, &optype, &buf);
		analysis_receive(tcpfd, optype, buf);
	}
}

void * pthread_udp(void * argv)
{
	int udpfd = *((int *)argv);
	UDP_HEADER * pudp_header;
	struct sockaddr_in src_addr;
	int lenth = sizeof(struct sockaddr_in);
	char buf[256];
	while(1)
	{
		int ret = recvfrom(udpfd, &buf, 256, 0, (struct sockaddr *)&src_addr, &lenth);
		if(-1 == ret)
		{
			perror("revfrom failed");
			exit(1);
		}
		else
		{
			pudp_header = (UDP_HEADER *)buf;
			char * message = pudp_header->data;
			int id = pudp_header->id;
			int lenth = pudp_header->lenth;
			int flag;
			Online_node * psrc = search_list(POnline, pudp_header->id, BY_ID);
			if(psrc != NULL && psrc->addr.sin_port == src_addr.sin_port)
			{
				pthread_mutex_lock(&mutex_iniface);
				flag = iniface;
				pthread_mutex_unlock(&mutex_iniface);
				if(!flag)
				{
					MOVEDOWN(1);
					for(int i=0; i<80; i++)
						printf(" ");
					printf("\n");
					MOVEUP(3);
					printf("\n");

					MOVEDOWN(1);
					printf("receive id:%d's message:%s\n", id, message);
					MOVEUP(3);
					printf("\n");
					continue;
				}
				pthread_mutex_lock(&mutex_count);
				if(row_count <= 0)
				{
					row_count = 15;
					send_message_Iface();
				}
				MOVEUP(row_count+1);
				row_count --;
				printf("id%d:%s\n", id, message);	
				MOVEDOWN(row_count+1);
				for(int i=0; i<75;i++)
				{
					printf(" ");
				}
				MOVEUP(1);
				printf("\n");
				pthread_mutex_unlock(&mutex_count);
			}
		}
	}
}
void analysis_receive(int tcpfd, int optype, void * buf)
{
	switch(optype)
	{
		case OP_ONLINE:
			//printf("optype = OP_ONLINE\n");
			online_action(tcpfd, (Online_node *)buf);
			break;
		case OP_ALLOW_DISCONNECT:
			//printf("optype = OP_ALLOW_DISCONNECT\n");
			allow_disconnect_action();
			break;
		case OP_OFFLINE:
			//printf("optype = OP_OFFLINE\n");
			offline_action(tcpfd, (int *)buf);
			break;
		case OP_ADD_FRIEND:
			//printf("optype = OP_ADD_FRIEND\n");
			add_friend_action(tcpfd, (int *)buf);
			break;
		case OP_ALLOW_ADDF:
			//printf("optype = OP_ALLOW_ADDF\n");
			allow_addf_action(tcpfd, (int *)buf);
			break;
		default:
			printf("recv unknown optype = %d\n", optype);
	}
}

void online_action(int tcpfd, Online_node * pnode)
{
	add_list(POnline, pnode);
	free(pnode);
}

void offline_action(int tcpfd, int * id)
{
	del_list(POnline, *id, BY_ID);
	free(id);  
}

void allow_disconnect_action()
{
	//printf("recv allow disconnect \n");
	system("clear");
	exit(0);
}

void add_friend_action(int tcpfd, int id[])
{
	//打开另一个终端，获取是否同意加好友
	system("gnome-terminal -e ./allow_addf");
	umask(0);
	int ret = mkfifo("./fifo_id", 0664);
	if(ret < 0 &&errno != EEXIST)
	{
		perror("create fifo failed");
		exit(1);
	}
	ret = mkfifo("./fifo_result", 0644);
	if(ret < 0 &&errno != EEXIST)
	{
		perror("create fifo failed");
		exit(1);
	}
	int fd_id, fd_result;
	fd_id = open("./fifo_id", O_WRONLY);
	if(fd_id < 0)
	{
		perror("open fifo failed");
		exit(1);
	}
	fd_result = open("./fifo_result", O_RDONLY);
	if(fd_result < 0)
	{
		perror("open fifo failed");
		exit(1);
	}
	ret = write(fd_id, &id[0], sizeof(int));
	if(ret < 0)
	{
		perror("write fifo failed");
		exit(1);
	}
	char result;
	ret = read(fd_result, &result, sizeof(char));
	if(ret < 0)
	{
		perror("read fifo failed");
		exit(1);
	}


	int id_send[3];
	id_send[0] = id[0];
	id_send[1] = id[1];
	free(id);
	if(result == 'y' || result == 'Y')
	{
		id_send[2] = 1;
		Write_client(tcpfd, OP_ALLOW_ADDF, id_send, 3*sizeof(int));
	}
	else
	{
		id_send[2] = 0;
		Write_client(tcpfd, OP_ALLOW_ADDF, id_send, 3*sizeof(int));
	}
}

void allow_addf_action(int tcpfd, int id[])
{
	if(id[0] == mine.id)
	{
		sleep(1);
		if(id[2] == 0)
		{
			MOVEDOWN(1);
			MOVELEFT(16);
			printf("id:%d refuse your request\n", id[1]);
			MOVEUP(3);
			printf("\n");
		}
		else if(id[2] == 1)
		{
			MOVEDOWN(1);
			MOVELEFT(16);
			printf("add id:%d as your friend succeed\n", id[1]);
			MOVEUP(3);
			printf("\n");
		}
		else if(id[2] == 2)
		{
			MOVEDOWN(1);
			MOVELEFT(16);
			printf("id:%d is not online or not exist\n", id[1]);
			MOVEUP(3);
			printf("\n");
		}
		else if(id[2] == 3)
		{
			MOVEDOWN(1);
			MOVELEFT(16);
			printf("server database operating failed\n");
			MOVEUP(3);
			printf("\n");
		}
		else if(id[2] == 4)
		{
			MOVEDOWN(1);
			MOVELEFT(16);
			printf("Can not add existing friend id:%d again\n", id[1]);
			MOVEUP(3);
			printf("\n");
		}
	}
	free(id);
}
