#include <stdio.h>
#include "pwdmgmt.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	umask(0);
	int ret = mkfifo("./fifo_id", 0664);
	if(ret < 0 &&errno != EEXIST)
	{
		perror("create fifo failed");
		exit(1);
	}
	ret = mkfifo("./fifo_result", 0644);
	if(ret < 0 &&errno != EEXIST)
	{
		perror("create fifo failed");
		exit(1);
	}
	int fd_id, fd_result;
	fd_id = open("./fifo_id", O_RDONLY);
	if(fd_id < 0)
	{
		perror("open fifo failed");
		exit(1);
	}
	fd_result = open("./fifo_result", O_WRONLY);
	if(fd_result < 0)
	{
		perror("open fifo failed");
		exit(1);
	}
	int id;
	ret = read(fd_id, &id, sizeof(int));
	if(ret < 0)
	{
		perror("write fifo failed");
		exit(1);
	}
	printf("id: %d request a friend, Y/N\n", id);
	char result = mygetch();
	ret = write(fd_result, &result, sizeof(char));
	if(ret < 0)
	{
		perror("read fifo failed");
		exit(1);
	}
}
