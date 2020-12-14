#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

pthread_mutex_t mutex;
char buf[1024];

void * fun1(void *str)
{
	printf("on fun1\n");
	pthread_mutex_lock(&mutex);
	strcpy(buf, "yisheng");
	pthread_mutex_unlock(&mutex);
	return NULL;
}

void * fun(void *str)
{
	sleep(1);
	printf("on fun lock\n");
	pthread_mutex_lock(&mutex);
	puts(buf);
	pthread_mutex_unlock(&mutex);
	return NULL;
}

int main()
{
	pthread_mutex_init(&mutex, NULL);
	pthread_t thread, thread1;
	int num = 10;
	pthread_create(&thread, NULL, fun, (void *)&num);
	pthread_create(&thread1, NULL, fun1, (void *)&num);
	pthread_join(thread, NULL);
	pthread_join(thread1, NULL);
	printf("on main thread\n");
	return 0;
}
