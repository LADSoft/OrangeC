#include <stdio.h>
static union
{
	float b;
	int c;
};
int main()
{
static union
{
	float b1;
	int c1;
};
union
{
	float b2;
	int c2;
};
	b = b1 = b2 = 4.0;
	printf("%x %x %x\n", c, c1, c2);
}
