#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main()
{
	int fd;
	pid_t pid;
	pid = fork();
	if(pid==0)
	{
		fd = open("test", O_RDWR);//不要把open()函数放入fork()函数
								  //之前,否则会导致子进程和父进程中
								  //fd文件描述符指向同一个文件表项
								  //子父进程同一文件的偏移量offeset
								  //混乱，每个进程应该有对文件的单独的
								  //文件表项
		sleep(1);
		char buf[1024];
		int cur = lseek(fd, 0, SEEK_END);
		lseek(fd, 0, SEEK_SET);
		int ret = read(fd, buf, cur);
		printf(" child ret = %d buf = %s\n", ret, buf);
	}
	else if(pid>0)
	{
		fd = open("test", O_RDWR);
		char buf[] = "hello world";
		int ret = write(fd, buf, strlen(buf));
		printf("father ret = %d buf = %s\n", ret, buf);
		sleep(3);
	}
}
