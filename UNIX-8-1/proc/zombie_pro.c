#include <stdio.h> 
#include <unistd.h>

int main()
{
	int pid = fork();
	if(pid == 0)
	{
	}
	else if(pid > 0)
	{
		while(1);
	}

	return 0;
}
