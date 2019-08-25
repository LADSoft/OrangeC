#include <stdio.h>
#include <string.h>

extern char *ss()
{
}
main()
{
	printf("%s\n", ss());
	printf("%d %d %d %d %d %d %d %d\n", -129, -128,-127, 0, 126, 127, 128, 129);
}
static char *ss()
{
return "hi";
}