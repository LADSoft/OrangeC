#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
char *a = "hello";
enum zz { x,y,z};
void (*xx)(char *, ...);
void *xx1 = MessageBoxA;
#define aa2 ((void (*WINAPI)(enum zz, char *, char *, int))(xx1))
void s(char *an, va_list ss)
{
    vprintf(an, ss);
}
void r(char *an, ...)
{
    va_list aa;
    va_start(aa, an);
    s(an, aa);
    vprintf(an, aa);

}
main()
{
    aa2(x,"hi","hi",0);
    xx = r;
    xx("hi %d %d",4,5);
}