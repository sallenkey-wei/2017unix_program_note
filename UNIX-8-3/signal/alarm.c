/*本例程运行效果是程序运行5秒后打印第一个点，然后每隔1秒，打印一个点*/
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void func(int signal)
{
	printf(".");
	fflush(stdout);
	alarm(1);
}

int main()
{
	signal(SIGALRM, func);
	alarm(5);//过5秒钟向本进程发送SIGALRM信号
	while(1)
		;
}
