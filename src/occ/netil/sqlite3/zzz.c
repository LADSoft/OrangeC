#include <stdio.h>
#include <stdarg.h>

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