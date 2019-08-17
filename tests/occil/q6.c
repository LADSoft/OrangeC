#include <stdio.h>
#include <stdlib.h>
char data[] = "zyxwdcbaAB";

int cmpFunc(void const *left, void const *right)
{
    const char *l = left;
    const char *r = right;
    if (*l < *r)
        return -1;
    else 
        return *l > *r;
}
int (*p)(int) = putchar;
int (* __unmanaged q)(void *, unsigned, unsigned, int (*)(const void *, const void *)) = qsort;
void * xx(void *t)
{
    if (t)
        return t;
    return NULL;
}
int main()
{
    p('@');
    q(data,10,1,cmpFunc);
    printf("%s\n", data);
    xx(putchar);
}
