#include <stdio.h>
#include <termios.h>
#include "pwdmgmt.h"
#include <unistd.h>
#include <string.h>
#include "menushow.h" 

void stdin_clr()
{
	while(getchar()!='\n')
		continue;
}
//该函数用来实现conio.h文件中getch函数的功能
int mygetch()
{
	struct termios oldt, newt;
	int ch;
	tcgetattr(STDIN_FILENO,&oldt);
	newt=oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return ch;
}

//该函数功能是逐个读入字符串并在屏幕上显示‘*’,实现密码获取
void password_input(char * password, int len)
{
	int ch, i = 0;
	while((ch=mygetch()) != '\r' && ch!='\n' && ch!='\t')//不同的系统回车键返回值不同
	{
		if(ch == '\b' || ch == 127)//不同系统BackSpace返回值不同
		{
			if(i > 0)
			{
				putchar('\b');
				putchar(' ');
				putchar('\b');
				--i;
			}
		}
		else if(i < len-1)
		{
			password[i++] = ch;
			putchar('*');
		}
	}
	printf("\n");
	password[i] = '\0';
	return;
}
void data_input(char data[], int len)
{
	int ch, i = 0;
	while((ch=mygetch()) != '\r' && ch!='\n' && ch!='\t')//不同的系统回车键返回值不同
	{
		if(ch == '\b' || ch == 127)//不同系统返回值不同
		{
			if(i > 0)
			{
				putchar('\b');
				putchar(' ');
				putchar('\b');
				--i;
			}
		}
		else if(i < len-1)
		{
			data[i++] = ch;
			putchar(ch);
		}
	}
	printf("\n");
	data[i] = '\0';
	return;

}

int data_input_msg(char data[], int len)
{
	int ch, i = 0;
	while((ch=mygetch()) != '\r' && ch!='\n' && ch!='\t')//不同的系统回车键返回值不同
	{
		if(ch == 27)
		{
			return -1;
		}
		if(ch == '\b' || ch == 127)//不同系统返回值不同
		{
			if(i > 0)
			{
				putchar('\b');
				putchar(' ');
				putchar('\b');
				--i;
			}
		}
		else if(i < len-1)
		{
			data[i++] = ch;
			putchar(ch);
		}
	}
	printf("\n");
	data[i] = '\0';
	return 0;
}
