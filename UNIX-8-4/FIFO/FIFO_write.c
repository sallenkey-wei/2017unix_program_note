#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

int main()
{
	umask(0);
	int ret = mkfifo("./test", 0664);
	if(ret < 0 && errno != EEXIST)
	{
		perror("creat FIFO failed:");
	}
	int fd;
	fd = open("./test", O_WRONLY);
	if(fd < 0)
	{
		printf("open failed\n");
		return -1;
	}
	char buf[1024] = {0};
	while(1)
	{
		//signal(EPIPE, SIG_IGN);//经测试本函数无法忽略该信号
		
		printf("please input the buf:");
		scanf("%s", buf);
		ret = write(fd, buf, strlen(buf)+1);//当fd对应管道另一端
		//关闭时，在调用write向管道写数据会导致程序直接退出，
		//此时程序收到一个EPIPE信号，详见man以下语句都不会执行
		if(ret < 0)
		{
			perror("FIFO write failed");
			close(fd);
			return -1;
		}
		else 
			printf("write successed\n");
	}
}

