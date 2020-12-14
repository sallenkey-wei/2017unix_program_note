#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void * thread_fun(void * Ppara)
{
	pthread_t thread;
	thread = pthread_self();
	printf("fun1\n");
	printf("fun2\n");
	printf("fun3\n");
	int oldtype;	
	/*该函数用来设置pthread_cancel()函数的退出类型，如果不设置为
	 * 该模式(PTHERAD_CANCEL_ASYNCHRONOUS), 会导致进程取消后继续
	 * 执行    printf("fun4\n");  语句*/
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype); 
	pthread_cancel(thread);
	printf("fun4\n");
	printf("fun5\n");
}

int main(void)
{
	pthread_t thread;
	pthread_create(&thread, NULL, thread_fun, NULL);
	printf("新建线程号为:%lu\n", (unsigned long)thread);
	pthread_join(thread, NULL);//等待创建线程结束
}

