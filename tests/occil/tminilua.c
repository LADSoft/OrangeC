#include <stdio.h>

int myfunc()
{
	printf("hello");
	printf("hi");
}
main(int argc, char **argv)
{
	printf("%c\n", "hello"[1]);
	(void)((argc == 1) && myfunc());
}