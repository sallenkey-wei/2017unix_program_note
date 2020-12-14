#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex, mutex2;
int buf;

void * func(void * str)
{
	printf("on func thread\n");
	printf("func thread: %s\n", (char *)str);

	/*这里上锁顺序与main线程中不同，造成死锁*/
	pthread_mutex_lock(&mutex);
	sleep(1);
	pthread_mutex_lock(&mutex2);
	buf = 1;
	pthread_mutex_unlock(&mutex);
	pthread_mutex_unlock(&mutex2);
	
	//return (void *)str;
	pthread_exit((void *)str);
}

int main()
{
	pthread_t thread;
	pthread_mutex_init(&mutex, NULL);
	pthread_create(&thread, NULL, func, (void *)"hello world");

	/*这里上锁的顺序和func线程的上锁顺序不同，导致死锁发声*/
	pthread_mutex_lock(&mutex2);
	sleep(1);
	pthread_mutex_lock(&mutex);
	printf("buf = %d \n", buf);
	pthread_mutex_unlock(&mutex);
	pthread_mutex_unlock(&mutex2);


	char * p;
	pthread_join(thread, (void *)&p);
	printf("main thread :%s\n", p);

}
