#include "linklist.h"
#include "comtype.h"
#include <stdlib.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "pwdmgmt.h"

extern pthread_mutex_t mutex;

Online_node * Create_list(Online_node * phead)
{
	if(phead == NULL)
	{
		phead = (Online_node *)malloc(sizeof(Online_node));
		phead->next = NULL;
	}
	return phead;
}

void add_list(Online_node * phead, Online_node *ptemp)
{
	pthread_mutex_lock(&mutex);
	Online_node * pnode = (Online_node *)malloc(sizeof(Online_node));
	*pnode = *ptemp; 
	pnode->next = phead->next;
	phead->next = pnode;
	pthread_mutex_unlock(&mutex);
}

void del_list(Online_node * phead, int fd_id, int flag)
{
	Online_node * p;
	pthread_mutex_lock(&mutex);
	if(phead != NULL)
	{
		if(flag == BY_SOCKFD)
		{
			while(phead->next != NULL)
			{
				if(phead->next->sockfd == fd_id)
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
		else if(flag == BY_ID)
		{
			while(phead->next != NULL)
			{
				if(phead->next->id == fd_id)
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
	}
	pthread_mutex_unlock(&mutex);
}

Online_node * search_list(Online_node * phead, int fd_id, int flag)
{
	pthread_mutex_lock(&mutex);
	if(phead != NULL)
	{
		if(flag == BY_SOCKFD)
		{
			while(phead->next != NULL)
			{
				if(phead->next->sockfd == fd_id)
				{
					pthread_mutex_unlock(&mutex);
					return phead->next;
				}
				else
					phead = phead->next;
			}
		}
		else if(flag == BY_ID)
		{
			while(phead->next != NULL)
			{
				if(phead->next->id == fd_id)
				{
					pthread_mutex_unlock(&mutex);
					return phead->next;
				}
				else
					phead = phead->next;
			}
		}
	}
	pthread_mutex_unlock(&mutex);
	return NULL;
}

void show_list(Online_node * POnline)
{
	system("clear");
	printf("\033[47;34m%-20.20s\33[0m", "id");
	printf("\033[47;34m%-20.20s\33[0m", "ip_address");
	printf("\033[47;34m%-20.20s\33[0m", "port_nu");
	printf("\n");
	char buf[20];
	while(POnline->next != NULL)
	{
		POnline = POnline->next;
		printf("%-20d", POnline->id);
		printf("%-20.20s", inet_ntop(AF_INET, &(POnline->addr.sin_addr.s_addr), buf, 20));
		printf("%-20d", ntohs(POnline->addr.sin_port));
		printf("\n");
	}
}

