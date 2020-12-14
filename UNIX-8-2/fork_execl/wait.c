/*wait()函数用来等待子进程结束，如果有多个子进程，则只要有一个
 * 子进程结束，父进程就会结束，其他未结束的子进程变为孤儿进程
 * waitpid()函数可以等待指定pid的子进程结束*/
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
		sleep(2);
		printf("on child process\n");
		//获取当前进程的进程号
		printf("子进程的id是%d\n", getpid());
		int pre = execl("/bin/ls", "ls", "-al", NULL);
		if (pre < 0)
			perror("failed");
		return 0;
	}
	else if(pid > 0)
	{
		int pid2 = fork();
		if(pid2 == 0)
		{
			sleep(1);
			printf("on child2 process\n");
			printf("子进程2的id是%d\n", getpid());
			return 0;
		}
		else if(pid2 > 0)
		{
			printf("on father process\n");
			printf("子进程id是%d\n", pid);
			system("ls -al");
			int status;
			printf("子进程2结束pid=%d\n", waitpid(pid2, &status, 0));
			printf("结束子进程是%d\n",wait(&status));//wait函数等待子进程结束
			printf("子进程是否正常退出 ：%d\n",WIFEXITED(status));
			printf("子进程退出返回值为：%d\n", WEXITSTATUS(status));
		}
	}
}
