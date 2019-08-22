#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

struct aa
{
    int a,b,c;
};
struct aa one = { 1, 2, 3 };
struct aa two = { 4,5,6 };

static int aa();

static int aa() { return 4; }

void zz(struct aa q, char r[])
{
    printf("%s", r);
    printf("%d\n", q.b);
    q.b = 5;
    printf("%d\n", q.b);
}

struct aa zz1()
{
    return two;
}
main(int argc, char **argv)
{
    printf(":%d\n", one.b);
    zz(one,"hi");
    printf(":%d\n", one.b);
    struct aa nn = zz1();
    printf("$%d\n", nn.c); 
}