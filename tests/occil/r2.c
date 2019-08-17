#include <stdio.h>
int qq;
extern int qq1;
int zz1(void);
int zz(void)
{
    int b = qq1;
    printf("hi%d\n", 43);
    fputs("hi", stdout);
    zz1();
}
struct aa
{
    int a,b;
} r = {4,5};
