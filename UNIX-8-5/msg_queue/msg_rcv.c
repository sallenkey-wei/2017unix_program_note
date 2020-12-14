#include <stdio.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>

struct DATA
{
	int id;
	char name[20];
};

struct msgbuf
{
	long type;//该参数指定消息类型，对应msgrcv()第4个参数,多个进程使用
	//同一个消息队列时，用于区分不同进程发送的消息
	struct DATA data;
};

int main()
{
	//创建消息队列，如果消息队列已存在则返回消息队列的id 
	int msgid = msgget(0x100, IPC_CREAT|0777);
	
	//发送消息
	struct msgbuf mb;
	msgrcv(msgid, &mb, sizeof(struct DATA), 1, 0);
	printf("id = %d \n", mb.data.id);
	printf("name = %s\n", mb.data.name);
}

