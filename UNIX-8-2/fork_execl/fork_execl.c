/*fork()函数功能是创建一个和当前进程资源完全一样的进程
 * execl()函数功能是用新的进程替换当前进程，
 * 所以利用fork()函数和execl()函数的组合可以实现并行执行
 * 两个不同的进程*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

int main()
{
	int pid = fork();
	if(pid == 0)
	{
		sleep(1);
		printf("on child process\n");
		int pre = execl("/bin/ls", "ls", "-al", NULL);
		if (pre < 0)
			perror("failed");
		return 0;
	}
	else if(pid > 0)
	{
		printf("on father process\n");
		printf("子进程id是%d\n", pid);
		system("ls -al");
		int status;
		printf("结束子进程是%d\n",wait(&status));//wait函数等待子进程结束
		
	}
}
