#include <stdio.h> 
#include <unistd.h>
#include <sys/types.h>

int main(void)
{
	int pid = fork();
	if(pid == 0)
	{
		printf("children process\n");
		printf("child's pid = %d \n", getpid());	
		printf("father's pid = %d \n", getppid());
		sleep(3);
		printf("end sleep father pid = %d \n", getppid());
		return 0;

	}
	else if(pid > 0)
	{
		sleep(1);
		printf("father process\n");
		printf("child's id %d\n", pid);
		printf("father's pid = %d \n", getpid());
		return 0;
	}
}
