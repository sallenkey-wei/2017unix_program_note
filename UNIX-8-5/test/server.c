#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <mysql/mysql.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include "mmsql.h"
#include "stu.h"
#include <string.h>
#include <signal.h>

static void  * register_action(void *);

int main()
{
	pthread_t thread;
	pthread_create(&thread, NULL, register_action, NULL);
	struct msgbuf mb; 
	mb.type = 2;
	int msgid = msgget(0x2000, IPC_CREAT|0777);
	int ret = mkfifo("./fifo", 0664);	
	if(ret < 0 && errno != EEXIST)
	{
		perror("create fifo failed");
		return 0;
	}
	int fifo_fd = open("./fifo", O_RDONLY);
	if(fifo_fd < 0)
	{
		perror("open fifo failed");
		return 0;
	}
	while(1)
	{
		ret = read(fifo_fd, &mb.stu, sizeof(mb.stu));	
		if(ret <= 0)
		{
			break;
		}
		char buf[1024];
		MYSQL * mysql_conn;
		MYSQL_RES * res;
		MYSQL_ROW row;
		sprintf(buf, "select * from student where id=%d", mb.stu.id);
		mysql_go(buf, &res, &mysql_conn);
		row = mysql_fetch_row(res);
		int len = sizeof(struct msgbuf) - sizeof(long);
		//使用消息队列发送数据是要特别注意，本次向程序中写入的数据
		//若没有来得及读出，只要消息队列不删除，将一直存储在队列中
		//下次重启程序还会读的到,所以要注意服务器发送一个数据，客户端
		//要读一个，如果不同步，就会导致错误,就如第47行，如果不加break
		//则从管道读入数据失败时（例如客户端异常关闭）,会想队列写入一个
		//无用的信息，下次客户端重启时，就会读到这个无用的信息，导致错误
		//所以加上一个break可以使这个数据传输配套，客户端发送一个登录信息
		//服务器接收后，通过队列回传一个信息,如果服务器接受失败，则不发送
		//任何消息到队列中，影响下次通信，
		//一定要注意，当初这里调了好久，
		if(row == NULL)
		{
			mb.stu.id = -1;
			if(msgsnd(msgid, &mb, len, 0)<0)
				perror("msgsnd faild");
		}
		else if(strcmp(row[2],mb.stu.password))
		{
			mb.stu.id = -2;
			if(msgsnd(msgid, &mb, len, 0)<0)
				perror("msgsnd failed");
		}
		else
		{
			strcpy(mb.stu.name, row[1]);
			mb.stu.id = atoi(row[0]);
			mb.stu.score = 0;
			if(msgsnd(msgid, &mb, len, 0)<0)
				perror("msgsnd failed");
		}
	}
}


static void * register_action(void * argv)
{
	struct msgbuf mb;
	//use message_queue to receive data from client
	struct sembuf sop;
	sop.sem_num = 0;
	sop.sem_op = 1;
	sop.sem_flg = 0;

	char sql_cmd[1024];
	MYSQL * mysql_conn;
	MYSQL_RES * res;
	MYSQL_ROW row;

	while(1)
	{
		int msgid = msgget(0x1000, IPC_CREAT|0777); 
		//use share memory to return the id of student
		int shmid = shmget(0x10000, sizeof(int), IPC_CREAT|0777);
		int * shmemory = (int *)shmat(shmid, NULL, 0);
		//use semaphore 
		int semid = semget(0x1000, 1, IPC_CREAT|0777);
		msgrcv(msgid, &mb, sizeof(Stu), 1, 0);
		msgctl(msgid, IPC_RMID, 0);
		int pid = mb.stu.id;
		write_to_sql(&mb);

		sprintf(sql_cmd, "select * from student where id=(select max(id) from student)");

		//mysql_conn参数用于接受未关闭的链接，等返回结果拆分完毕，才能关闭
		if(!mysql_go(sql_cmd, &res, &mysql_conn))
		{	
			kill(pid, 1);
			row = mysql_fetch_row(res);	
			*shmemory = atoi(row[0]);
			printf("id = %s, name = %s\n", row[0], row[1]);

			mysql_free_result(res);
			mysql_close(mysql_conn);
			
			//int semop(int semid, struct sembuf * sops, unsigned nsops);
			semop(semid, &sop, 1);
		}
	}
	pthread_exit(NULL);
}
