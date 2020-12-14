/*功能：创建新的线程，并向线程中传递参数，并取得线程的返回值
 * */
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
/*int pthread_create(pthread_t *restrict thread,
				const pthread_attr_t *restrict attr,
				void *(*start_routine)(void*), 
				void *restrict arg);*/

typedef struct student
{
	int id;
	char name[10];
}Stu;

void * thread_fun(void * Ppara)
{
	int param = *((int*)Ppara);
	printf("传入的参数为：%d\n", param);
	
	pthread_t pthread_id = pthread_self();
	printf("线程号为:%lu\n", (unsigned long)pthread_id);
	
	Stu *pstu1 = (Stu *)malloc(sizeof(Stu));	
	strcpy(pstu1->name, "xiaoming");
	pstu1->id = 1001;

	pthread_exit((void*)pstu1);//退出当前线程,并将返回值void*返回

	printf("end of pthread exit1\n");
}

int main(void)
{
	pthread_t thread;
	int num = 10;

	pthread_create(&thread, NULL, thread_fun, (void *)&num);
	printf("新建线程号为:%lu\n", (unsigned long)thread);

	Stu *pstu1 = NULL;

	/*等待线程thread结束，并取其返回值*/
	pthread_join(thread, (void **)&pstu1);

	printf("id=%d\n", pstu1->id);
	printf("name=%s\n", pstu1->name);
}

