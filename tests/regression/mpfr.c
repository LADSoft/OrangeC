#include <stdio.h>

#ifdef DLL
//------------------------------------------------------------------------------------
char *hi ="hello %d\n";

_Thread_local long long padding;

_Thread_local char **one = &hi;

void cc(int a, char *msg)
{
    printf(msg, a);
}
_export void expfunc(int a)
{
	cc(a, *one);
}
//--------------------------------------------------------------------------------------
#else

struct aa
{
	int r,s,t;
} ss[2]= {4,5,6,7,8,9};

typedef struct aa zz[1];

zz rr = {1,2,3};


void test(zz* m)
{
	zz * q = m+1;
	printf("%d\n", q[0][0].t);
        printf("%d\n", rr[0].t);
}


_import int expfunc(int);

main()
{
	expfunc(543);
	test(ss);
}
#endif