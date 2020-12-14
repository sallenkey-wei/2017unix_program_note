#include <stdio.h>

int main()
{
	int *(* a)[2] ;
	int c = 1, d = 2, e = 3;
	int * b[3][2] = {&c, &d, &e};
	a = b;
//	printf("%d, %d, %d", **a, **(a+1), **(a+2));
}
