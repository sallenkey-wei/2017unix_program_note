#include "socket.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include "comtype.h"
#include <string.h>
#include <unistd.h>
#include "linklist.h"

int Socket(int type)
{
	int sockfd = socket(AF_INET, type, 0);
	if(sockfd < 0)
	{
		perror("sockfd access failed");
		exit(1);
	}
	int opt = 1;
	//设置端口可重复绑定
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))<0)
	{
		perror("setsockopt reuseaddr failed");
		exit(1);
	}
	return sockfd;
}

//初始化sockaddr结构体
void struct_sockaddr_init(struct sockaddr_in * paddr, int port, char * ip)
{
	paddr->sin_port = htons(port);
	paddr->sin_family = AF_INET;
	int ret = inet_pton(AF_INET, ip, &(paddr->sin_addr));
	if(ret == 0)
	{
		fprintf(stderr, "ip address: \"%s\" is wrong\n", ip);
	}
	else if(ret == -1)
		perror("inet_pton argument af is wrong");
}

void Bind(int sockfd, const struct sockaddr_in * sockaddr)
{
	if(bind(sockfd, (struct sockaddr *)sockaddr, sizeof(struct sockaddr_in)) < 0)
	{
		perror("bind failed");
		exit(1);
	}
}

void Listen(int sockfd, int backlog)
{
	if(listen(sockfd, backlog) < 0)
	{
		perror("listen failed");
		exit(1);
	}
}	

int Accept(int sockfd, struct sockaddr_in * addr)
{
	int lenth = sizeof(struct sockaddr_in);
	int confd = accept(sockfd, (struct sockaddr *)addr, &lenth);
	if(confd < 0)
	{
		perror("accept failed");
		exit(1);
	}
	return confd;
}

void Connect(int sockfd, const struct sockaddr_in * paddr)
{
	if(connect(sockfd, (struct sockaddr *)paddr, sizeof(struct sockaddr_in)))
	{
		perror("connect failed");
		exit(1);
	}
}

int Write(int sockfd, int optype, void * buf, uint32_t lenth)
{
	PACK_HEADER * PDataPackage = (PACK_HEADER *)malloc(lenth + sizeof(PACK_HEADER));
	PDataPackage->optype = optype;
	PDataPackage->lenth = lenth;

	memcpy((void *)PDataPackage->data, buf, lenth);

	int ret = write(sockfd, (void *)PDataPackage, lenth + sizeof(PACK_HEADER));
	
	free(PDataPackage);
	if(ret < 0)
	{
		perror("write failed");
		return -1;
	}
	if(ret == 0)
	{
		return -1;
	}
	return 0;
}

int Write_file(int sockfd, int optype, char filename[], int src_id, int dest_id, int lenth, void * buf)
{
	PACK_HEADER * PDataPackage = (PACK_HEADER *)malloc(lenth + sizeof(PACK_HEADER) + sizeof(FILE_HEADER));
	PDataPackage->optype = OP_FILE; 
	PDataPackage->lenth = lenth + sizeof(FILE_HEADER); 
	FILE_HEADER * fileheader = (FILE_HEADER *)(PDataPackage->data);
	fileheader->optype = optype;
	strcpy(fileheader->filename, filename);
	fileheader->id[0] = src_id;
	fileheader->id[1] = dest_id;
	fileheader->lenth = lenth;
	for (int i = 0; i < lenth; i++)
	{
		((uint8_t *)(fileheader->data))[i] = ((uint8_t *)(buf))[i];
	}
	int ret = write(sockfd, (void *)PDataPackage, lenth + sizeof(PACK_HEADER) + sizeof(FILE_HEADER));
	free(PDataPackage);
	if(ret < 0)
	{
		perror("write failed");
		return -1;
	}
	return 0;
}

int Read(int sockfd, int * optype, void ** pbuf)
{
	PACK_HEADER packheader;
	int ret = read(sockfd, &packheader, sizeof(PACK_HEADER));	
	if(ret < 0)
	{
		perror("read failed");
		return -1;
	}
	else if(ret == 0)
	{
		return -1;
	}
	*optype = packheader.optype;
	if(packheader.lenth > 0)
	{
		*pbuf = (void *)malloc(packheader.lenth);
		ret = read(sockfd, *pbuf, packheader.lenth);
		if(ret < 0)
		{
			perror("read failed");
		return -1;
		}
		else if(ret == 0)
		{
			return -1;
		}
	}
	return 0;
}

#ifdef SOCK_FOR_CLIENT

void Write_client(int sockfd, int optype, void * buf, uint32_t lenth)
{
	int ret = Write(sockfd, optype, buf, lenth);
	if(ret < 0)
	{
		fprintf(stderr, "server connect occur error!\n");
		exit(1);
	}	
}

void Write_file_client(int sockfd, int optype, char filename[], int src_id, int dest_id, int lenth, void * buf)
{
	int ret = Write_file(sockfd, optype, filename, src_id, dest_id, lenth, buf);
	if(ret < 0)
	{
		fprintf(stderr, "server connect occur error!\n");
		exit(1);
	}
		
}

void Read_client(int sockfd, int * optype, void ** pbuf)
{
	int ret = Read(sockfd, optype, pbuf);
	if(ret < 0)
	{
		fprintf(stderr, "when call read_client func server connect occur error!\n");
		exit(1);
	}
}

void Getsockname(int sockfd, struct sockaddr_in * addr)
{
	int lenth = sizeof(struct sockaddr);
	int ret = getsockname(sockfd, (struct sockaddr *)addr, &lenth);
	if(ret < 0)
	{
		perror("getsockname failed");
		exit(1);
	}
}

extern Online_node mine;

void Online_mine_init(int sockfd)
{
	Getsockname(sockfd, &(mine.addr));	
	mine.sockfd = sockfd;
}

#else

extern Online_node * POnline;
void offline_action(int confd, int id);

int Write_server(int sockfd, int optype, void * buf, uint32_t lenth)
{
	int ret = Write(sockfd, optype, buf, lenth);
	if(ret < 0)
	{
		/*
		 * delete the sockfd information
		 * */
		Online_node * ptemp = search_list(POnline, sockfd, BY_SOCKFD);
		int id;
		if(ptemp == NULL)
			id = -1;
		else
			id = ptemp->id;
		offline_action(sockfd, id);
	}	
	return ret;
}

int Write_file_server(int sockfd, int optype, char filename[], int src_id, int dest_id, int lenth, void * buf)
{
	int ret = Write_file(sockfd, optype, filename, src_id, dest_id, lenth, buf);
	if(ret < 0)
	{
		/*
		 *delete the sockfd information
		 * */
		Online_node * ptemp = search_list(POnline, sockfd, BY_SOCKFD);
		int id;
		if(ptemp == NULL)
			id = -1;
		else
			id = ptemp->id;
		offline_action(sockfd, id);
	}
	return ret;
		
}

int Read_server(int sockfd, int * optype, void ** pbuf)
{
	int ret = Read(sockfd, optype, pbuf);
	if(ret < 0)
	{
		/*
		 *delete sockfd information
		 * */
		Online_node * ptemp = search_list(POnline, sockfd, BY_SOCKFD);
		int id;
		if(ptemp == NULL)
			id = -1;
		else
			id = ptemp->id;
		offline_action(sockfd, id);
	}
	return ret;
}

#endif
