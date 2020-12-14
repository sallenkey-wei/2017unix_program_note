/*通过无名管道父进程写入，子进程读入, 交换数据
 * 无名管道只能用于有亲缘关系的进程，因为fork()
 * 函数创建的子进程可以拷贝所有属于父进程资源,
 * 所以文件描述符所对应的管道文件的文件表项也是
 * 一样的，所以可以交换数据,且无名管道文件不同于
 * FIFO,无名管道存储在内存中，所以读写速度快于FIFO，
 * FIFO管道文件存储在磁盘上，*/
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int main()
{
	int fds[2];
	int ret = pipe(fds);
	if(ret < 0)
		printf("create pipe failed\n");
	pipe(fds);
	pid_t pid;
	pid = fork();
	if(pid == 0)
	{
		char buf[1024]={0}; 
		close(fds[1]);//关闭写文件描述符
		sleep(5);
		while(1)
		{
			printf("on child process ");
			ret = read(fds[0], buf, 1024);
			printf("ret = %d\n", ret);
			if(ret < 0)
			{
				perror("read failed\n");
			}
			else if(ret == 0)
			{
				printf("pipe closed\n");
				close(fds[0]);
				break;
			}
			else
			{
				printf("read from pipe: ");
				//printf("%s\n", buf);
				//char * p = buf;
				int i = 0;
				while(i<112)	
				{
					printf("%c",buf[i]);
					++i;
				}
				printf("\n");
			}
		}
	}
	else if(pid > 0)
	{
		close(fds[0]);//关闭读文件描述符
		char buf[1024];
		while(1)
		{
			printf("on father process please input the buf:");
			scanf("%s", buf);
			ret = write(fds[1], buf, strlen(buf)+1);
			if(ret < 0)
			{
				printf("write failed\n");
			}
			else
			{
				printf("write buf to child process sucess\n");
			}
			//close(fds[1]);
			//break;
			//sleep(1);
		}
	}
	while(1);
}
