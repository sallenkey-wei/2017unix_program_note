#ifndef _MMSQL_H_
#define _MMSQL_H_

#include "stu.h"
#include <mysql/mysql.h>

MYSQL * mysql_get_conn(void);
void write_to_sql(struct msgbuf *);
int mysql_go(char * sqlcmd, MYSQL_RES ** pres, MYSQL ** conn);

#endif

