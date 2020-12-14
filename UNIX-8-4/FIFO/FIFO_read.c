#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main()
{
	int ret = mkfifo("./test", 0664);
	if(ret < 0 && errno != EEXIST)
	{
		perror("FIFO file create failed");
	}
	int fd = open("./test", O_RDONLY);
	if(fd < 0)
	{
		printf("open failed\n");
		return -1;
	}
	char buf[1024];
	while(1)
	{
		ret = read(fd, buf, 1024);	
		if(ret < 0)
		{
			perror("FIFO read failed");
			close(fd);
			return -1;
		}
		else if(ret == 0)
		{
			printf("FIFO broken\n");
			return -1;
		}
		else
			printf("read buf:%s\n", buf);
	}
}
