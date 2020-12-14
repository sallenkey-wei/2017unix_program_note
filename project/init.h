#ifndef _INIT_H
#define _INIT_H

#define SERVER_CONF 0
#define CLIENT_CONF 1

static int conf_file_analysis(char * ip, unsigned short int * pport, int conf_type);

#ifdef SOCK_FOR_CLIENT
int client_init();
#else
int server_init();
#endif

#endif
