#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
/*
上海怡胜 
本程序要求
   1.mysql安装 
   2.创建数据库yisheng_data
   3.创建表 stus
		+------+--------+------+
		| id   | name   | age  |
		+------+--------+------+
		|    1 | aaa    |   10 |
		+------+--------+------+
   4. mysql 登录的用户和密码 跟程序中的一致。可修改程序中的全局变量 mysql_user mysql_password。
*/	

MYSQL *mysql_conn = NULL;
char mysql_server[]     = "localhost";//mysql 数据库主机
char mysql_user[]       = "root";//mysql 管理登录用户
char mysql_password[]   = "123456";//mysql 管理登录密码
char mysql_database[]   = "yisheng_data";//使用的数据库。


struct stu
{
	int num;
	char name[32];
	int age;
};


MYSQL * mysql_get_conn()
{
	
	if(mysql_conn != NULL)
		return mysql_conn;

	//初始化，创建数据操作符
	mysql_conn = mysql_init(NULL);

	//链接数据库
	if (!mysql_real_connect
	    (mysql_conn, mysql_server, mysql_user, mysql_password, mysql_database, 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(mysql_conn));
		return NULL;
	}

	return mysql_conn;
}


int  mysql_go(char *sqlcmd,MYSQL_RES **pres)
{
	
	MYSQL *conn;
	MYSQL_RES *res;
	conn = mysql_get_conn();

	//执行 指定数据库
	if (mysql_query(conn, sqlcmd)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		return -1;
	}	
	//获取 sql语句执行的结果
	res = mysql_use_result(conn);
	
	if(pres != NULL)
		*pres = res;
	return 0;
}

int stu_add()
{
	struct stu s;

	char sql_cmd[1024];

	printf("Please input stu(num name):\n");

	scanf("%d %s %d",&s.num,s.name,&s.age);

	sprintf(sql_cmd,"insert into stus values(%d,'%s',%d)",s.num,s.name,s.age);
	printf("%s\n",sql_cmd);
	mysql_go(sql_cmd,NULL);
	return 0;
}

int stu_del()
{
	struct stu s;

	char sql_cmd[1024];

	printf("Please input id of stu that you want delete:\n");

	scanf("%d",&s.num);

	sprintf(sql_cmd,"delete from stus where id=%d",s.num);
	printf("%s\n",sql_cmd);
	mysql_go(sql_cmd,NULL);
	return 0;
}


void stu_show()
{
	struct stu s;
	MYSQL_RES *res;
	MYSQL_ROW row;
	int fnum = 0,i;	

	char sql_cmd[1024];


	sprintf(sql_cmd,"select * from stus");
	mysql_go(sql_cmd,&res);

	fnum = mysql_num_fields(res);//返回值为列数。
	
	//一行一行的获取数据，依次向后
	while ((row = mysql_fetch_row(res)) != NULL) {
		s.num = atoi(row[0]);
		strcpy(s.name,row[1]);
		s.age = atoi(row[2]);

		printf("stu:%d %s %d\n",s.num,s.name,s.age);
	}

	
	mysql_free_result(res);

}



int main()
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	int fnum = 0,sel;	
	int ret;
	int flag = 1;


	ret = mysql_go("use yisheng_data",NULL);
	if(ret == -1)
	{
		printf("use data ret error\n");
	}
	
	while(flag)
	{
		printf("1.add stu.\n");
		printf("2.show stu.\n");
		printf("3.delete stu\n");
		printf("4.exit.\n");

		printf("Please input selct num:\n");
		scanf("%d",&sel);

		switch(sel)
		{
			case 1:
				stu_add();
				break;
			case 2:
				stu_show();
				break;
			case 3:
				stu_del();
				break;
			case 4:
				flag = 0;
				break;
		}	
	}
	//关闭描述符
	mysql_close(mysql_get_conn());

	printf("finish! \n");
	return 0;
}

