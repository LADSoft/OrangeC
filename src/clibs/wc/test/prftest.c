#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    wchar_t s[100];
    int i;
    wchar_t *t = itow(-513,s,10);
    double a ;
    struct tm *tm;
    time_t x = time(0);
    swprintf(s,100,L"%10d:%f:%ls:%s",4527,4.52,L"HI DAVE","\xe0\x84\x85\xdf\xbf");
    for (i=0; i < wcslen(s); i++)
        printf("%04x ",s[i]);
    printf("\n");
}