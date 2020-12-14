#ifndef _COMTYPE_H
#define _COMTYPE_H

#include "menumgmt.h"
#include <stdint.h>
#include <netinet/in.h>		/* To define `struct in_addr'.  */
#include <time.h>

//server can receive the following optypes
#define OP_REGISTER 0			//USER
#define OP_LOGIN 1				//USER
#define OP_HEARTBEAT 2			//int(id)

//client can receive the following optypes

// the other package_header is add 
#define OP_FILE 3				//FILE_HEADER + uint8_t[]
#define OP_ONLINE 4				//Online_node
#define OP_ALLOW_DISCONNECT 5	//null
#define OP_REGISTER_SUCCESS 6	//int(receive a id)
#define OP_LOGIN_FEEDBACK 7		//int(flag)0成功，-1失败，-2服务器数据库操作失败
//server and client common optypes
#define OP_OFFLINE 8			//int(user.id) id==-1表示在epoll中，但不在在线用户链表中
#define OP_ADD_FRIEND 9			//int[2](id)
#define OP_ALLOW_ADDF 10		//int[3](id, allow or not)第三位int[2]
								//0表示拒绝，1表示同意，2表示不在线或不存在
								//3表示服务器数据库操作失败, 4表示已经是好友

typedef struct packet_header
{
	int optype;
	int lenth;
	char data[0];
}PACK_HEADER;

//upd channel is only for client to receive message.
//the id is the message source's id, the lenth is the lenth for message.
typedef struct udp_packet_header
{
	int id;
	int lenth;
	char data[0];
}UDP_HEADER;

//server and client common optypes
#define OP_COMPLETE 0
#define OP_NOCOMPLETE 1
#define OP_RECEIVED 2
//client may receive the following optypes
#define OP_ID_NOT_EXIT 3
typedef struct file_header
{
	int optype;
	char filename[20];
	int id[2];
	int lenth;
	char data[0];
}FILE_HEADER;

typedef struct user
{
	int id;
	char username[USERNAME_LEN];
	char password[PWD_LEN];
}USER;

typedef struct client_online
{
	int id;
	int sockfd;
	time_t time;
	struct sockaddr_in addr;
	struct client_online * next;
}Online_node;

#endif
