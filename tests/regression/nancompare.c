#include <stdio.h>
#include <math.h>

void test(double d)
{
	printf("%f\n", d);
	if (d == d)
		printf("1");
	else
		printf("0");
	if (3.0 < d)
		printf("1");
	else
		printf("0");
	if (3.0 > d)
		printf("1");
	else
		printf("0");
	printf("%d", d == d);
	printf("%d", 3.0 < d);
	printf("%d\n", 3.0 > d);
}
int main()
{
	test(nan("0"));
	test(4.0);
}