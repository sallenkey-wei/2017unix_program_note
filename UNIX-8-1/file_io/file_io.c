#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(void)
{
	int fd = open("./test", O_CREAT|O_RDWR, 0664);
	char buf[1024] = 
	 "hello world, I'm stupid, I'm sb\n";
	int count = write(fd, buf, strlen(buf));	
	char buf2[1024];
	lseek(fd, 0, SEEK_SET);
	int count2 = read(fd, buf2, count);
	printf("%s\n", buf2);
}
