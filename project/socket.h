#ifndef _SOCKET_H
#define _SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "comtype.h"

int Socket(int type);
void Bind(int sockfd, const struct sockaddr_in * sockaddr);
void struct_sockaddr_init(struct sockaddr_in * paddr, int port, char * ip);
void Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr_in * addr);
void Connect(int sockfd, const struct sockaddr_in * paddr);
int Write(int sockfd, int optype, void * buf, uint32_t lenth);
int Write_file(int sockfd, int optype, char filename[], int src_id, int dest_id, int lenth, void * buf);
int Read(int sockfd, int * optype, void ** pbuf);
#ifdef SOCK_FOR_CLIENT
void Write_client(int sockfd, int optype, void * buf, uint32_t lenth);
void Write_file_client(int sockfd, int optype, char filename[], int src_id, int dest_id, int lenth, void * buf);
void Read_client(int sockfd, int * optype, void ** pbuf);
void Getsockname(int sockfd, struct sockaddr_in * addr);
void Online_mine_init(int sockfd);
#else
int Write_server(int sockfd, int optype, void * buf, uint32_t lenth);
int Write_file_server(int sockfd, int optype, char filename[], int src_id, int dest_id, int lenth, void * buf);
int Read_server(int sockfd, int * optype, void ** pbuf);
#endif

#endif
