#ifndef SERVER_ACTION_H
#define SERVER_ACTION_H

#include "comtype.h"

void analysis_receive(int confd, int optype, void * buf);
void login_action(int confd, USER * user);
void register_action(int confd, USER * user);
void heartbeat_action(int confd, int id);
void offline_action(int confd, int id);
void add_friend_action(int confd, int id[]);
void allow_addf_action(int confd, int id[]);

#endif
