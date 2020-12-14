#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include "stu.h"
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

void signal_fun(int argv);

void register_fun()
{
	struct msgbuf mb;
	printf("please input your name:\n");
	scanf("%s", mb.stu.name);
	while(getchar()!='\n')
		continue;
	printf("please input your password\n");
	scanf("%s", mb.stu.password);
	while(getchar() != '\n')
		continue;
	mb.stu.id = getpid();
	mb.type = 1;
	int msgid = msgget(0x1000, IPC_CREAT|0777);	
	msgsnd(msgid, &mb, sizeof(Stu), 0);

	int shmid = shmget(0x10000, sizeof(int), IPC_CREAT|0777);
	int semid = semget(0x1000, 1, IPC_CREAT|0777);
	void *p = shmat(shmid, NULL, 0);	
	struct sembuf sop;
	sop.sem_num = 0;
	sop.sem_op = -1;
	sop.sem_flg = 0;
	
	//semop(int semid, struct sembuf *sops, size_t nsops);
	semop(semid, &sop, 1);
	printf("your id is  %d\n", *((int *)p));
	//shmdt(p);
	//semctl(semid, IPC_RMID, 0);
}

void login_fun()
{
	Stu  stu;
	struct msgbuf mb;
	printf("please input your id:\n");
	scanf("%d", &stu.id);
	while(getchar()!='\n')
		continue;
	printf("please input your password:\n");
	scanf("%s", stu.password);
	while(getchar()!='\n')
		continue;
	umask(0);
	int ret = mkfifo("./fifo", 0664);
	if(ret < 0 && errno!=EEXIST)
	{
		perror("mkfifo error");
		return;
	}
	int fifo_fd = open("./fifo", O_WRONLY);
	if(fifo_fd < 0)
	{
		perror("open fifo failed");
		return;
	}
	ret = write(fifo_fd, &stu, sizeof(stu));
	if(ret <= 0)
	{
		perror("write fifo failed");
		return;
	}
	int msgid = msgget(0x2000, O_CREAT|0777);
	if((msgrcv(msgid, &mb, sizeof(mb.stu), 2, 0)) < 0)
		perror("msgrcv failed");
	if(mb.stu.id == -1)
	{
		printf("id is not exsit!\n");
	}
	else if(mb.stu.id == -2)
	{
		printf("password is wrong!\n");
	}
	else
	{
		printf("id = %d, your name is %s score is %f\n", mb.stu.id, mb.stu.name, mb.stu.score);
	}
}

void menu()
{
	int flag;
	while(1)
	{
		printf("1:login\n");
		printf("2:register\n");
		scanf("%d", &flag);
		while(getchar()!='\n')
			continue;
		switch(flag)
		{
			case 1:
				login_fun();
				break;
			case 2:
				register_fun();
				break;
		}
	}
}

int main()
{
	
	signal(1, signal_fun);
	menu();		
}

void signal_fun(int argv)
{
	printf("register succeed!\n");
}
