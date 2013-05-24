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
    for (i=0;i < wcslen(t); i++)
        printf("%04x ",s[i]);
    printf("\n");    

    a = wcstod(L"4.5E3",0);
    printf("%f\n",a);    
    i = wcstol(L"0x4578",0,0);
    printf("%x\n",i);    

    printf("%d\n",wcscoll(L"bbbc",L"bbba"));    
    printf("%d\n",wcscoll(L"bbbc",L"bbbc"));    
    printf("%d\n",wcscoll(L"bbb",L"bbbc"));    
    t = wcsdup(L"hi dave");
    for (i=0; i < wcslen(t); i++)
        printf("%04x ",t[i]);
    printf("\n");
    tm = localtime(&x);
    wcsftime(s,100,L"%H:%M:%S",tm);
    for (i=0; i < wcslen(s); i++)
        printf("%04x ",s[i]);
    printf("\n");
    wcsxfrm(s,L"hi dave",100);        
    for (i=0; i < wcslen(s); i++)
        printf("%04x ",s[i]);
    printf("\n");
}