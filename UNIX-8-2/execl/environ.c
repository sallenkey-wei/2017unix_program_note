#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char * argv[])
{
	extern char ** environ;
	char ** env = environ;
	while(*env)
	{
		puts(*env);
		env++;
	}

}
