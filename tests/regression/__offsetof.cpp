#include <stdio.h>
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
	printf("%d %d %d %d %d\n", __offsetof(struct aa, c), __offsetof(xx, y), __offsetof(xx,d), __offsetof(AA,s), __offsetof(zz, vv.b));
}