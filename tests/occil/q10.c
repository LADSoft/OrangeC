#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
enum aa { r1, r2, r3, r4 };
void (*bb)(int);
void CALLBACK r(int a, int b,  int c, int d, long long e, int f, int g, int h, int i)
{
    printf("%d %d %d %d %lld %d %d %d %d\n",a,b,c,d,e,f,g,h,i);
    printf("bye");
}
void *testfunc(void *);
main(int argc, char **argv)
{
    testfunc(r);
    r(1,2,3,4,5,6,7,8,9);
    printf("hi");
}