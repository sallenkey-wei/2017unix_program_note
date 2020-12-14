/*exec系列函数可以将当前进程执行镜像替换成新的镜像，包括包括代码段，所有内存资源，cpu寄存器信息等
 * exec系列函数包括两种系列execl带l的，使用不定参数，使用不定参数传递要执行命令的string 表示
 * execv带v的，使用字符串数组传递要执行命令的字符串表示
 * 除非函数执行失败，否则不会返回当前进程*/
/*int execl(const char *path, const char *arg, ...);
 *int execlp(const char *file, const char *arg, ...);
 *int execle(const char *path, const char *arg, ..., char * const envp[]);

 *int execv(const char *path, char *const argv[]);
 *int execvp(const char *file, char *const argv[]);
 *int execve(const char *file, char *const argv[], char *const envp[]);
 *                                                                         */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(void)
{
	int pid = fork();
	if(pid == 0)
	{
		printf("in child's process\n");
		printf("before execl\n");

		/*会自动在/bin中搜索cat文件(命令),采用不定参数，而且必须用null作为参数的结束 */ 
		int ret = execlp("cat", "cat", "execl.c", NULL);//将当前进程替换成系统命令"cat execl.c",进程放弃执行此行命令之下的代码 
		//转而去显示execl.c文件中的内容

		/*要人为指定路径，否则出错*/
		//int ret = execl("/bin/cat", "cat", "execl.c", NULL);
		if(ret < 0)
			perror("failed");
		printf("end of execl\n");
		return 0;
	}
	else if(pid > 0)
	{
		printf("in father's process\n");
		printf("before execl\n");
		//int ret = execl("/bin/cat", "cat", "execl.c", NULL);
		/*execlp()和execl()两函数的功能差别，exelp()函数可以自动在环境变量指定的路径中搜索要执行的文件*/
		char *argv[] = {"cat", "execl.c", NULL};
		int ret = execv("/bin/cat", argv);
		if(ret < 0)
			perror("failed");
		printf("end of execl\n");
		return 0;
	}
}
