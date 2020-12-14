#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
	int fd = open("./test", O_RDONLY);
	char buf[100];
	read(fd, buf, 100);
	printf("%s\n", buf);	
}
