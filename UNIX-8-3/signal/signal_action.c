#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

void func(int sig)
{
	printf("recv sig = %d\n", sig);
}
//int sigaction(int signum, const struct sigaction *act,
				 //struct sigaction *oldact);

int main()
{
	struct sigaction action;
	sigemptyset(&action.sa_mask);//清空信号集合
	action.sa_flags = SA_SIGINFO;//改变信号的调用方式
	//void handler(int signo);
	void handler(int signo, siginfo_t * info, void * context);
	action.sa_handler = func;
	sigaction(2, &action, NULL);
	while(1)
	{
		printf(".");
		fflush(stdout);
		sleep(1);
	}
}
