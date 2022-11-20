#include <stdio.h>
int main(int argc)
{

	int i = 0;
	void *arr[3] = { &&one, &&two, &&three };
loop:
     goto *arr[i++];
one:
	printf("hello, world\n");
     goto loop;
two:
     printf("ok now\n");
     goto loop;
three:
	return 0;
}