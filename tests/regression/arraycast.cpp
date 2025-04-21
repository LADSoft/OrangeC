#include <stdio.h>
template <int s>
void two(char (&aa)[s])
{
	printf("%d:%s\n", s, aa);			
}
void one(char *aa)
{
	printf("%s\n", aa);
	char (&bb)[256] = (char (&)[256]) *aa;
	printf("%s\n", bb);
	two(bb);
}

int main()
{
	char aa[256] = " hi dave";
	one(aa);
}