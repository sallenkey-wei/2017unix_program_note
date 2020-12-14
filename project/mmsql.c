#include "mmsql.h"
#include <stdlib.h>
#include <stdio.h>
#include "comtype.h"

//连接数据库
MYSQL * mysql_get_conn(void)
{
	MYSQL * mysql_conn;
	mysql_conn = mysql_init(NULL);
	char * mysql_server = "localhost";
	char * mysql_user = "root";
	char * mysql_password = "jkl";
	char * mysql_database = "user";
	if(!mysql_real_connect(mysql_conn, mysql_server, mysql_user, 
				mysql_password, mysql_database, 0, NULL, 0))
	{
		fprintf(stderr, "%s\n", mysql_error(mysql_conn));
		return NULL;
	}
	return mysql_conn;
}

//执行命令操作数据库，返回结果在pres中,不需要返回结果，
int mysql_go(char * sqlcmd, MYSQL_RES ** pres, MYSQL ** conn)
{
	if(conn==NULL)
	{
		printf("conn is not null!\n");
		return -1;
	}
	MYSQL * mysql_conn = NULL; 
	MYSQL_RES * res;
	mysql_conn = mysql_get_conn();
	if(mysql_conn == NULL)
		exit(0);
	if(mysql_query(mysql_conn, sqlcmd))
	{
		fprintf(stderr, "%s\n", mysql_error(mysql_conn));
		mysql_close(mysql_conn);
		return -1;
	}
	res = mysql_use_result(mysql_conn);
	if(pres != NULL)
		*pres = res;
	//mysql_close(mysql_conn);//这里不能关，关闭之后pres指向的内存
	//会释放，导致拆分结果错误,所以本函数将mysql_conn传出去，等
	//外部操作完毕，后再关闭
	*conn = mysql_conn;
	return 0;
}

void write_to_sql(USER * user)
{
	MYSQL * mysql_conn = NULL;
	mysql_conn = mysql_get_conn();
	if(mysql_conn == NULL)
		exit(0);

	char sql_cmd[1024];
	sprintf(sql_cmd, "insert into user(username, password) values\
			(\'%s\',\'%s\')", user->username, user->password);
	mysql_query(mysql_conn, sql_cmd);
	mysql_close(mysql_conn);
}
