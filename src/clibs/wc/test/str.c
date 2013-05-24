#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
int main()
{
    wchar_t *b;
    wchar_t s[100];
printf("%d\n",wcsicmp(L"HI DAVE",L"hi dave"));
printf("%d\n",wcsicmp(L"HI DAVE",L"hi dave3"));
printf("%d\n",wcsicmp(L"HI DAVE3",L"hi dave"));
printf("%d\n",wcsnicmp(L"HI DAVE3",L"hi dave",20));
printf("%d\n",wcsnicmp(L"HI DAVE3",L"hi dave",3));
printf("%d\n",wcscmp(L"HI DAVE",L"HI DAVE"));
printf("%d\n",wcscmp(L"HI DAVE",L"HI DAVE3"));
printf("%d\n",wcscmp(L"HI DAVE3",L"HI DAVE"));
printf("%d\n",wcsncmp(L"HI DAVE3",L"HI DAVE",20));
printf("%d\n",wcsncmp(L"HI DAVE3",L"HI DAVE",3));
    b= wcsupr(wcscpy(s,L"hi dave"));
    printf("%ls/%ls\n",b,s);
    wmemset(s,0,100);
    b= wcslwr(wcsncpy(s,L"HI DAVE",5));
    printf("%ls/%ls\n",b,s);
    printf("%d\n",wcsspn(L"abcdefg",L"abc"));
    printf("%d\n",wcscspn(L"abcdefg",L"efg"));
    printf("%ls\n",wcsncat(s,L"go home",5));
    printf("%ls\n",wcscat(s,L"bye now"));
    printf("%d\n",wcslen(L"hi"));
    printf("%ls\n",wcschr(L"KLKL",'K'));
    printf("%ls\n",wcsrchr(L"KLKL",'K'));
    printf("%ls\n",wcsstr(L"HI DAVE",L"DA"));
    printf("%d\n",wcpcpy(s,L"HI")-s);
    printf("%d\n",wcpncpy(s,L"HI dave",4)-s);
    printf("%d\n",wcpncpy(s,L"HI dave",20)-s);
    wmemset(s,0,100);
    wmemset(s,0x55,10);
    printf("%x %x\n",s[9],s[10]);
    wmemcpy(s,L"hi",2);
    printf("%x %x\n",s[1],s[2]);
    printf("%ls\n",wmemchr(L"hijkl",L'j',3));
    printf("%d\n",wmemchr(L"hijkl",L'j',2));
    printf("%d\n",wmemcmp(L"hi3",L"hi",2));
    printf("%d\n",wmemcmp(L"hi3",L"hi",3));
    printf("%d\n",wmemcmp(L"hi3",L"hi4",3));
    wmemset(s,'h',10);
    wmemset(s+10,'l',10);
    s[20] = 0;
    wmemmove(s +5, s,10);
    printf("%ls",s);
    wmemset(s,'h',10);
    wmemset(s+10,'l',10);
    s[20] = 0;
    wmemmove(s , s+5,10);
    printf("%ls",s);
    printf("%ls",wcspbrk(L"abcdefghijklmnop",L"12345i67890"));
    wchar_t *rv = wcstok(L":::/go home:::/why not:::/aaa",L":/",&b);
    printf("%ls\n",rv);
    while (1) {
        wchar_t *rv = wcstok(0,L":/",&b);
        if (!rv)
            break;
        printf("%ls\n",rv);
    }
}