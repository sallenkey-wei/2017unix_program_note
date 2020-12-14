#ifndef _STU_H
#define _STU_H

typedef struct student
{
	int id;
	char name[20];
	char password[20];
	float score;
}Stu;

struct msgbuf
{
	long type;
	Stu stu;
};

#endif

