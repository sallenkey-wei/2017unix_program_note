#include <stdio.h>
#include <sys/types.h>
#include <unistd.h> 
#include <errno.h>
#include <fcntl.h>
#include <string.h>

int main()
{
	int fd;
	fd = open("test", O_WRONLY);
	if(fd<0)
	{
		perror("open failed\n");
		return -1;
	}

	int flag = 0;
	while(1)
	{
		//加锁
		struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_whence = SEEK_SET;//加锁起始位置偏移的基准点，这里设置为文件头
		lock.l_start = 0;//相对于基准点的偏移量
		lock.l_len = 0;//设置为0,表示从起始点到文件尾全部加锁
		int ret = fcntl(fd, F_SETLKW, &lock);

		char buf[100];
		if(!flag)
		{
			strcpy(buf, "hello world\n");
		}
		else
		{
			strcpy(buf, "fuck worrld\n");
		}
		flag = !flag;

		printf("write..\n");
		ret = write(fd, buf, strlen(buf));
		if(ret <= 0)
		{
			printf("write failed\n");
		}
		//解锁
		lock.l_type = F_UNLCK;
		lock.l_whence = SEEK_SET;
		lock.l_start = 0;
		lock.l_len = 0;

		sleep(1);
		fcntl(fd, F_SETLK, &lock);
		sleep(1);
	}
	close(fd);
}
