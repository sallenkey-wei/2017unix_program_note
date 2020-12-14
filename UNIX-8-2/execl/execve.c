/*程序功能是用自己编译的程序env替换当前进程，使用execve()函数是可以
 * 指定环境变量，所以将环境变量改变为"weiyuyin", "sallenkey" 后，新的进程
 * 运行env，env的功能正好是打印环境变量，所以程序最终输出的结果是weiyuyin
 * sallenkey
 * 本程序依赖于自己的写的程序env*/
#include <stdio.h>
#include <unistd.h>

/*execve(const char *file, char *const argv[], char *const envp[]); man 2 execve*/
int main()
{
	/*都需要用NULL结束*/
	char *arg[] = {"env", NULL};
	char * env[] = {"weiyuyin", "sallenkey", NULL};
	printf("before ecvpe\n");

	/*env 是自己编译的程序，功能是打印当前环境变量，execve()函数第三个参数
	 * 就是指定替换当前进程的进程的环境变量*/
	execve("env", arg, env);
	printf("after ecvpe\n");
}
