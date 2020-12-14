#include "server_action.h"
#include <stdio.h>
#include <mysql/mysql.h>
#include "mmsql.h"
#include "linklist.h"
#include <stdlib.h>
#include "socket.h"
#include "epoll.h"
#include "comtype.h"
#include <unistd.h>
#include <string.h>

extern Online_node * POnline;
extern int epfd;

void analysis_receive(int confd, int optype, void * buf)
{
	switch(optype)
	{
		case OP_REGISTER:
			register_action(confd, (USER *)buf);
			break;
		case OP_LOGIN:
			login_action(confd, (USER *)buf);
			break;
		case OP_HEARTBEAT:
			heartbeat_action(confd, *((int *)buf));
			break;
		case OP_OFFLINE:
			offline_action(confd, *((int *)buf));//下线处理函数
			free(buf);
			break;
		case OP_ADD_FRIEND:
			add_friend_action(confd, (int *)buf);
			break;
		case OP_ALLOW_ADDF:
			allow_addf_action(confd, (int *)buf);
			break;

	}
}

void register_action(int confd, USER * user)
{
	char sql_cmd[1024];
	MYSQL * mysql_conn;
	MYSQL_RES * res;
	MYSQL_ROW row;
	int id;
	write_to_sql(user);
	sprintf(sql_cmd, "select * from user where id=(select max(id) from user)");
	if(!mysql_go(sql_cmd, &res, &mysql_conn))
	{
		row = mysql_fetch_row(res);
		id = atoi(row[0]);
		/*
		 * Create the friends table
		 * */
		sprintf(sql_cmd, "create table id%d(id int(6) not null primary key)", id);
		if(mysql_go(sql_cmd, &res, &mysql_conn))
		{
			id = -1;
		}
	}
	else
	{
		id = -1;
	}
	Write_server(confd, OP_REGISTER_SUCCESS, &id, sizeof(int));
	mysql_free_result(res);
	mysql_close(mysql_conn);
	free(user);
}

void login_action(int confd, USER * user)
{
	char sql_cmd[1024];
	MYSQL * mysql_conn;
	MYSQL_RES * res;
	MYSQL_ROW row;
	int flag;
	sprintf(sql_cmd, "select * from user where id=%d", user->id);
	if(!mysql_go(sql_cmd, &res, &mysql_conn))
	{
		row = mysql_fetch_row(res);
		if(row == NULL || strcmp(row[2], user->password))
		{
			flag = -1;
		}
		else if(search_list(POnline, user->id, BY_ID) == NULL)//以下是登录成功要做的操作，更新服务器的在线用户链表
			//向所有好友发送自己上线信息，向自己发送所有在线好友信息
		{
			Online_node temp;
			temp.id = user->id;
			temp.sockfd = confd;
			int lenth = sizeof(struct sockaddr_in);
			if(getpeername(confd, (struct sockaddr *)&(temp.addr), &lenth) < 0)
			{
				perror("getpeername failed");
				offline_action(confd, -1);//下线处理函数
				free(user);
				mysql_free_result(res);
				mysql_close(mysql_conn);
				return;
			}
			flag = 0;	
			/*
			 * update linklist
			 * */
			add_list(POnline, &temp);
			/*
			 * send the login succeed flag to client.
			 * */
			Write_server(confd, OP_LOGIN_FEEDBACK, &flag, sizeof(int));
show_list(POnline);
			/*以下操作是从新登录用户对应的数据库好友表中循环取出好友
			 *每取出一个好友就检测服务器在线用户链表中是否有该好友
			 *如果有，则向该好友发送新用户在线信息，向新用户发送该
			 *好友的在线信息, 没有就进入下次循环
			 * */
			int id;
			Online_node * ponline;
			sprintf(sql_cmd, "select * from id%d", temp.id);
			if(!mysql_go(sql_cmd, &res, &mysql_conn))
			{
				while((row = mysql_fetch_row(res)) != NULL)
				{
					id = atoi(row[0]);
					ponline = search_list(POnline, id, BY_ID);
					if(ponline != NULL)
					{
						if(-1 == Write_server(ponline->sockfd, OP_ONLINE, &temp, sizeof(Online_node)))	
						{
							printf("error1\n");
							continue;
						}
						if(-1 == Write_server(confd, OP_ONLINE, ponline, sizeof(Online_node)))
						{
							printf("error2\n");
							break;
						}
					}
				}
			}
			else//这种情况是登录成功后，服务器更新客户端好友出错,采取下线操作
			{
				offline_action(confd, user->id);//下线处理函数
			}
			/*
			 * 检测数据库idfriendxxxxxx是否有好友请求（待写）
			 *
			 * */
			free(user);
			mysql_free_result(res);
			mysql_close(mysql_conn);
			return;
		}
		else
		{
			flag = -3;//重复登录

		}
	}
	else
	{
		flag = -2;
	}
	Write_server(confd, OP_LOGIN_FEEDBACK, &flag, sizeof(int));
	mysql_free_result(res);
	mysql_close(mysql_conn);
	free(user);
}

void heartbeat_action(int confd, int id)
{
}

void offline_action(int confd, int id)//下线处理函数
{
	//从epoll监视队列中删除
	struct epoll_event event;
	event.events = EPOLLET | EPOLLIN;	
	event.data.fd = confd;
	epoll_ctl(epfd, EPOLL_CTL_DEL, confd, &event);
	
	/*offline_action函数在发生读写错误时也会被调用，客户端运行后
	 *未登录成功，退出，或客户端只进行了注册就退出了这时只需删除
	 *监视队列中的confd即可，在线用户链表中根本没有此confd对应的
	 *节点，所以直接return, 读写函数中发生错误时会先检索在线用户
	 *如果不存在该节点，就把id置为-1，若存在则说明不是上述情况，
	 *id置为检索到的值
	 * */
	if(id < 0)
	{
		close(confd);
		return ;
	}
	/*删除在线用户链表中该下线用户对应的节点*/
	del_list(POnline, id, BY_ID);
show_list(POnline);
	
	/*向其所有的在线好友发送其下线消息*/
	char sql_cmd[1024];
	MYSQL * mysql_conn;
	MYSQL_RES * res;
	MYSQL_ROW row;
	Online_node * ponline;
	int friend_id;
	sprintf(sql_cmd, "select * from id%d", id);
	if(!mysql_go(sql_cmd, &res, &mysql_conn))
	{
		while((row = mysql_fetch_row(res)) != NULL)
		{
			friend_id = atoi(row[0]);
			ponline = search_list(POnline, friend_id, BY_ID);
			if(ponline != NULL)
			{
				Write_server(ponline->sockfd, OP_OFFLINE, &id, sizeof(int));	
			}
		}
	}
	//发送允许下线消息
	Write_server(confd, OP_ALLOW_DISCONNECT, NULL, 0);
	//关闭文件描述符
	mysql_free_result(res);
	mysql_close(mysql_conn);
	close(confd);
}

void add_friend_action(int confd, int id[])
{
	int src_id = id[0];
	int dst_id = id[1];
	Online_node * pdst = search_list(POnline, dst_id, BY_ID);
	
	char sql_cmd[1024];
	MYSQL * mysql_conn;
	MYSQL_RES * res;
	MYSQL_ROW row;
	sprintf(sql_cmd, "select * from id%d where id=%d", src_id, dst_id);
	int ret = mysql_go(sql_cmd, &res, &mysql_conn);
	row = mysql_fetch_row(res);
	if(ret != 0)
	{
		int id_send[3] = {id[0], id[1], 3};
		Write_server(confd, OP_ALLOW_ADDF, id_send, 3*sizeof(int));
		free(id);
		return;

	}
	if(pdst == NULL && row == NULL)
	{
		int id_send[3] = {id[0], id[1], 2};
		Write_server(confd, OP_ALLOW_ADDF, id_send, 3*sizeof(int));
	}
	else if(row != NULL)
	{
		int id_send[3] = {id[0], id[1], 4};
		Write_server(confd, OP_ALLOW_ADDF, id_send, 3*sizeof(int));
	}
	else
	{
		Write_server(pdst->sockfd, OP_ADD_FRIEND, id, 2*sizeof(int));
	}
	free(id);
}

void allow_addf_action(int confd, int id[])
{
	int src_id = id[0];
	int dst_id = id[1];
	Online_node * psrc = search_list(POnline, src_id, BY_ID);
	Online_node * pdst = search_list(POnline, dst_id, BY_ID);
	if(id[2] == 1)
	{
		char sql_cmd[1024];
		MYSQL * mysql_conn;
		MYSQL_RES * res;
		MYSQL_ROW row;
		sprintf(sql_cmd, "insert into id%d values(%d)", src_id, dst_id);
		int ret = mysql_go(sql_cmd, &res, &mysql_conn);
		sprintf(sql_cmd, "insert into id%d values(%d)", dst_id, src_id);
		int ret2 = mysql_go(sql_cmd, &res, &mysql_conn);
		if(!ret && !ret2)
		{
			if(psrc != NULL)
			{
				Write_server(psrc->sockfd, OP_ALLOW_ADDF, id, 3*sizeof(int));
			}
			if(psrc != NULL && pdst != NULL)
			{
				Write_server(psrc->sockfd, OP_ONLINE, pdst, sizeof(Online_node));
				Write_server(pdst->sockfd, OP_ONLINE, psrc, sizeof(Online_node));
			}
		}
		else
		{
			if(psrc != NULL)
			{
				id[2] = 3;
				Write_server(psrc->sockfd, OP_ALLOW_ADDF, id, 3*sizeof(int));
			}
		}
		mysql_free_result(res);
		mysql_close(mysql_conn);
	}
	else if(id[2] == 0)
	{
		if(psrc != NULL)
			Write_server(psrc->sockfd, OP_ALLOW_ADDF, id, 3*sizeof(int));
	}
	free(id);
}
