#include <stdio.h>
#include <stddef.h> // for offsetof
struct aa
{
	int b,c ;
};
struct bb
{
	int d,e;
};

struct xx : aa, bb
{
	int x,y;
};

typedef struct {
	int r,s,t;
} AA;

struct zz {
	double g;
	struct qq
        {
		int a,b,c;
	} vv;
};
main()
{
	printf("%d %d %d %d %d\n", offsetof(struct aa, c), offsetof(xx, y), offsetof(xx,d), offsetof(AA,s), offsetof(zz, vv.b));
}