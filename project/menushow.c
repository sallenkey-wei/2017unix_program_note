/*本文件主要负责打印所有字符界面，主要用printf
 函数*/
#include "menushow.h"
#include <stdio.h>
#include <stdlib.h>

void wel_Iface_show()
{
	system("clear");
	printf("\n\n\n");
	
	printf("\t\t\033[47;34m\t                              \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t     Welcome For Coming!!!    \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t                              \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t       L: Login               \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t                              \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t       R: Register            \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t                              \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t       E: exit                \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t                              \t\t\33[0m\n");
	printf("\t\t\t        :");
}

void send_message_Iface()
{
	system("clear");
	int i;
	for(i = 0; i < 15; ++i)
	{
		printf("\n");
	}
	for(i = 0; i < 80; ++i)
	{
		printf("=");
	}
	printf("\n");
}

void Login_Iface_show()
{
	system("clear");

	printf("\n\n\n");
	printf("\t\t\033[47;34m                                \t\t\33[0m\n");
	printf("\t\t\033[47;34m    Login                       \t\t\33[0m\n");
	printf("\t\t\033[47;34m                                \t\t\33[0m\n");
	printf("\t\t\033[47;34m                                \t\t\33[0m\n");
	printf("\t\t\033[47;34m    ID:                         \t\t\33[0m\n");
	printf("\t\t\033[47;34m                                \t\t\33[0m\n");
	printf("\t\t\033[47;34m    Password:                   \t\t\33[0m\n");
	printf("\t\t\033[47;34m                                \t\t\33[0m\n");
	printf("\t\t\033[47;34m                                \t\t\33[0m\n");
}

void Register_Iface_show()
{

	system("clear");

	printf("\n\n\n");
	printf("\n\n\n");
	printf("\t\t\033[47;34m                                \t\t\33[0m\n");
	printf("\t\t\033[47;34m    Register                    \t\t\33[0m\n");
	printf("\t\t\033[47;34m                                \t\t\33[0m\n");
	printf("\t\t\033[47;34m                                \t\t\33[0m\n");
	printf("\t\t\033[47;34m    ID:                         \t\t\33[0m\n");
	printf("\t\t\033[47;34m                                \t\t\33[0m\n");
	printf("\t\t\033[47;34m    Password:                   \t\t\33[0m\n");
	printf("\t\t\033[47;34m                                \t\t\33[0m\n");
	printf("\t\t\033[47;34m                                \t\t\33[0m\n");
}

void opera_Iface_show()
{
	system("clear");
	printf("\n\n\n");
	
	printf("\t\t\033[47;34m\t                              \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t     Operations               \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t                              \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t       A: add friends         \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t                              \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t       S: send message        \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t                              \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t       P: print online friends\t\t\33[0m\n");
	printf("\t\t\033[47;34m\t                              \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t       E: exit                \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t                              \t\t\33[0m\n");
	printf("\t\t\t        :");
}

void select_send_show()
{
	system("clear");
	printf("\n\n\n");
	
	printf("\t\t\033[47;34m\t                              \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t     Selection                \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t                              \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t       S: single mode         \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t                              \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t       M: Multi mode          \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t                              \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t       A: All                 \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t                              \t\t\33[0m\n");
	printf("\t\t\t        :");
}
void Faillogin_Iface_show()
{

	system("clear");

	printf("\n\n\n\n\n\n");
	printf("\t\t\033[47;34m\t                              \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t                              \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t  ID not exist or             \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t  password wrong!!            \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t  please try again!           \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t  put any keys to continue... \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t                              \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t                              \t\t\33[0m");
	MOVELEFT(35);	
}

void Inputerr_Iface_show()
{

	system("clear");

	printf("\n\n\n\n\n\n");
	printf("\t\t\033[47;34m\t                              \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t                              \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t  Illegal Input               \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t  please try again!           \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t                              \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t  put any keys to continue... \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t                              \t\t\33[0m\n");
	printf("\t\t\033[47;34m\t                              \t\t\33[0m");
	MOVELEFT(35);	
}

