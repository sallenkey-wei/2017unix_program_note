#include "menushow.h"
#include "menumgmt.h"
#include "pwdmgmt.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "socket.h"
#include "comtype.h"

extern Online_node mine;

void wel_Iface_mgmt(int sockfd)
{
	char ifg;
again:
	//display the welcome interface
	wel_Iface_show();
	ifg = getchar();
	//empty the stdin buff
	stdin_clr();
	//let user choose their own option
	switch(ifg)
	{
		case 'L':
			Login_Iface_mgmt(sockfd);
			break;
		case 'R':
			Register_Iface_mgmt(sockfd);
			break;
		case 'E':
			exit(0);
			break;
		default:
			Inputerr_Iface_mgmt();
			break;
	}
	if(ifg != 'L')
	{
		goto again;
	}
	system("clear");
}

void Login_Iface_mgmt(int sockfd)
{
	//Used to keep Username and password
	char RxData[USERNAME_LEN+1], Pwd[PWD_LEN+1];
again:
	Login_Iface_show();
	
	MOVEUP(5);
	printf("\t\t\033[47;34m    ID:             \t\t\t\t\33[0m");
	MOVELEFT(35);	
	
	//receive id 
	data_input(RxData, USERNAME_LEN+1);

	MOVEDOWN(1);
	printf("\t\t\033[47;34m    Password:       \t\t\t\t\33[0m");
	MOVELEFT(35);

	//receive password
	password_input(Pwd, PWD_LEN+1);
	
	USER user;
	user.id = atoi(RxData);
	strcpy(user.password, Pwd);
	Write_client(sockfd, OP_LOGIN, &user, sizeof(USER));
	int optype;
	void * buf;
	//read(sockfd, &packheader, sizeof(packheader));
	Read_client(sockfd, &optype, &buf);
	if(optype == OP_LOGIN_FEEDBACK)
	{
		if(*((int *)buf) == 0)
		{
			mine.id = user.id;
			system("clear");
			printf("\tlogin succeed!\n");
			//mygetch();
			system("clear");
		}
		else if(*((int *)buf) == -1)
		{
			Faillogin_Iface_mgmt();
			goto again;
		}
		else if(*((int *)buf) == -2)
		{
			fprintf(stderr, "login server database occur error!\n");
			mygetch();
			exit(1);
		}
		else if(*((int *)buf) == -3)
		{
			fprintf(stderr, "\n\n\talready online\n");
			mygetch();
			goto again;
		}
		free(buf);
	}
	else
	{
		fprintf(stderr, "login on server occur error!\n");
		mygetch();
		exit(1);
	}
}

void Register_Iface_mgmt(int sockfd)
{
	//Used to keep Username and password
	char RxData[USERNAME_LEN+1], Pwd[PWD_LEN+1];
again:
	Register_Iface_show();
	
	MOVEUP(5);
	printf("\t\t\033[47;34m    Username:       \t\t\t\t\33[0m");
	MOVELEFT(35);	
	
	//receive username
	data_input(RxData, USERNAME_LEN+1);

	MOVEDOWN(1);
	printf("\t\t\033[47;34m    Password:       \t\t\t\t\33[0m");
	MOVELEFT(35);

	//receive password
	password_input(Pwd, PWD_LEN+1);
	USER user;
	strcpy(user.username, RxData);
	strcpy(user.password, Pwd);
	Write_client(sockfd, OP_REGISTER, &user, sizeof(user));
	int optype;
	int * buf;
	Read_client(sockfd, &optype, (void **)&buf);
	if(optype == OP_REGISTER_SUCCESS)
	{
		if(*((int *)buf) == -1)
		{
			fprintf(stderr, "register failed, server database occur error!\n");
			mygetch();
			exit(1);
		}
		else
		{
			system("clear");
			printf("your id is %d\n", *buf);
		}
		free(buf);
	}
	else
	{
		fprintf(stderr, "register on server occur error!\n");
		mygetch();
		exit(1);
	}
	mygetch();
	system("clear");
}

char opera_Iface_mgmt()
{
	char ifg;
	opera_Iface_show();
	ifg = getchar();
	stdin_clr();
	return ifg;
}

/*this function is used to manage the login failed interface*/
void Faillogin_Iface_mgmt()
{
	Faillogin_Iface_show();
	mygetch();
}

/*this function is used to manage the illegal input interface*/
void Inputerr_Iface_mgmt() 	
{
	Inputerr_Iface_show();
	mygetch();
}

