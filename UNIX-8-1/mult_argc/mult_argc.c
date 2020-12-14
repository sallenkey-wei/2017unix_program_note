#include <stdio.h>
#include <stdarg.h>

void func(int a, int n, ...);
//除了第一个参数，后面的都是不定参数，不定参数从右至左依次进栈
//栈为va_list类型，自行定义

int main()
{
	func(10, 5,2,4,2,1);
}

void func(int a, int n, ...)
{
	va_list list;
	n--;	
	n = 0;
	//n = 100;
	va_start(list, n);//指明栈中有几个元素,第二个参数必须是n，即最右侧
	//的有名参数(last named argument)，n的数值表示list中有几个元素,经测试，
	//使用以下循环加va_arg()的方式取出栈中的值，n值对其无影响,真正决定的
	//是循环的次数
	for(int i=0; i<4; i++)
	{
		printf("第%d个参数为%d\n", i+1, va_arg(list, int));
	}
	va_end(list);
}
