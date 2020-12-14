#ifndef _MMSQL_H_
#define _MMSQL_H_

#include "comtype.h"
#include <mysql/mysql.h>

MYSQL * mysql_get_conn(void);
void write_to_sql(USER *);
int mysql_go(char * sqlcmd, MYSQL_RES ** pres, MYSQL ** conn);

#endif

