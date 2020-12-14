/*函数signal()用于接收信号，第一个参数为要捕获信号的
 *数值，第二个参数为要订阅的信号处理函数 */
/*类似单片机的中断，和中断处理函数，当外部进程发送3信号时
 * 进程停止正文部分执行，保存现场，转去执行func()函数，执行完毕
 * 恢复现场，接着执行正文部分。*/
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>

void func(int sig)
{
	printf("on func signal is %d\n", sig);
	exit(1);
}

int main()
{
	int pid;
	pid = fork();
	if(pid == 0)
	{
		signal(3, func);
		while(1)
		{
			printf("on child process\n");
			sleep(1);
		}
	}
	else if(pid > 0)
	{
		int i = 0;
		while(1)
		{
			printf("on father process\n");
			sleep(1);
			if(i>2)
			{
				kill(pid, 3);
				break;
			}
			i++;
		}
	}
}
