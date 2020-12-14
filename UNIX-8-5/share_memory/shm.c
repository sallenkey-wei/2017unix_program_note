#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/sem.h>

int main()
{
	//创建共享内存，大小1024， 键值0x1000,权限0777
	int shmid = shmget(0x1000, 1024, IPC_CREAT|0777);

	//创建信号量，键值0x1002， 个数1， 优先级
	int semid = semget(0x1002, 1, IPC_CREAT|0777);//第二个参数代表新建信号量的个数

	void * p = shmat(shmid, NULL, 0);//获取共享内存的地址
	strcpy((char *)p, "weiyuyin");
	
	struct sembuf sops;//这里可以建立一个数组，sops[n];数组中的每个元素
	//代表对信号量的一次操作，
	sops.sem_num = 0;//代表对第0个信号量进行操作
	sops.sem_op = 1;//代表生产一个资源，如果为-2代表消费两个资源，如果为0则等待该信号量的数值变为0
	sops.sem_flg = 0;//0为默认值，表示以阻塞的方式运行

	//对信号量进行操作，第二个参数是一个结构体数组，结构体存储着对哪个信号量进行操作，进行何种操作，以及操作的标志
	//第一个参数为信号量的键值，第三个参数为总共进行几步操作，即第二个参数数组的长度
	semop(semid, &sops, 1);
}
