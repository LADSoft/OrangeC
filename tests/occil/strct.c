#include <stdio.h>
struct aa
{
	char a;
	int b;
	struct xx
	{
		int c,d;
	} rr;
} aa[] = { {1,2,{3}},{4,5,{6}}, {7,8,{9}}} ;


main()
{
    for (int i=0; i < sizeof(aa)/sizeof(aa[0]); i++)
	printf("%d\n", aa[i].a);




}