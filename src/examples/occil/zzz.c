#include <stdio.h>
#include <stdarg.h>

enum myenum
{
    one = 45, two = 55, three = 65
} qqq;
void myprintf(char *f, ...)
{
    int accum = 0;
    va_list ap;
    va_start (ap, f);
    vprintf(f, ap);
}
void mygets(char *buf, int n)
{
    fgets(buf, n, stdin);
}
struct aa
{
    int i;
    char b;
    int j;
} qq = {100,99,98};
struct aa *getqq(void)
{
    return &qq;
}