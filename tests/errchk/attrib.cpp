#include <stdio.h>

__attribute__((deprecated)) int a()
{
}
__attribute__((__deprecated__)) int b()
{
}
[[deprecated]]  int c()
{
}


main()
{
	a();
	b();
	c();
}