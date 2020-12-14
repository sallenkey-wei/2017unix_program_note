#include <stdio.h>
#include <signal.h>
#include <unistd.h>

typedef void (* sighandler_t)(int);
void func(int sig)
{
	printf("sign_id = %d \n", sig);
	printf("on func\n");
}

int main()
{
	sighandler_t p;
	p = signal(2, func);
	raise(2);//向自己的进程发送信号
	while(1)
	{
		putchar('.');
		fflush(stdout);
		sleep(1);
	}
}
