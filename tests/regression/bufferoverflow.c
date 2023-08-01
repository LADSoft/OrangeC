#include <Stdio.h>

void aa()
{
	char buf[256], buf2[256];
	for (int i=0; i < 257; i++)
                buf[i] = 0;
	for (int i=0; i < 257; i++)
                buf2[i] = 0;
}
main()
{
	aa();
}