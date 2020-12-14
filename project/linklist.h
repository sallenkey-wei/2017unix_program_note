#ifndef _LINKLIST_H
#define _LINKLIST_H

#include "comtype.h"

#define BY_ID 0
#define BY_SOCKFD 1

Online_node * Create_list(Online_node * phead);
void add_list(Online_node * phead, Online_node *pnode);
void del_list(Online_node * phead, int fd_id, int flag);
Online_node * search_list(Online_node * phead, int fd_id, int flag);
void show_list(Online_node * POnline);

#endif
