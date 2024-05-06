
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

struct aa
{
	int a,b;
};
struct aa nn = {1,2 };
struct aa rr = {3,4 };
struct aa ss = {5,6 };
struct aa tt = {7,8 }; 
struct aa uu = {7,8 }; 
struct aa *mm = &nn;

int main(int argc, char *argv[])
{
	printf("%d %d\n", nn.a, nn.b);
//	(0, nn) = rr;
	printf("%d %d\n", nn.a, nn.b);
	*(0, &nn) = ss;
	printf("%d %d\n", nn.a, nn.b);
	*(0, mm) = tt;
	printf("%d %d\n", nn.a, nn.b);

	*&nn = uu;
	*mm = uu;
}