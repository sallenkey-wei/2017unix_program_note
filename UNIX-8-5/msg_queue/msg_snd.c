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
	long type;
	struct DATA data;
};

int main()
{
	//创建消息队列，如果消息队列已存在则返回消息队列的id 
	int msgid = msgget(0x100, IPC_CREAT|0777);
	
	//发送消息
	struct msgbuf mb = {1, 1001, "yisheng"};
	msgsnd(msgid, &mb, sizeof(struct DATA), 0);
}

