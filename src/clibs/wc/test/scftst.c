#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    wchar_t s[100],c1;
    char t[100],c2[100];
    int n=29,i;
    
    memset(c2,0,sizeof(c2));
    swscanf(L"\x5445\x2425 456 \x5445hidave \x5445hidave",L"%lc %c %d %ls %s",&c1,c2,&n,s,t);
    for (i=0; i < wcslen(s); i++)
        printf("%04x ",s[i]);
    printf("\n");
    for (i=0; i < strlen(t); i++)
        printf("%02x ",(unsigned char)t[i]);
    printf("\n%d:%x\n",n,c1);
    for (i=0; i < strlen(c2); i++)
        printf("%02x ",(unsigned char)c2[i]);
    printf("\nhi\n");
    swscanf(L"abdce",L"%l[abc]",s);
   for (i=0; i < wcslen(s); i++)
        printf("%04x ",(unsigned short)s[i]);
     printf("\n");
    swscanf(L"\x5467zyabdce",L"%l[^abc]",s);
   for (i=0; i < wcslen(s); i++)
        printf("%04x ",(unsigned short)s[i]);
     printf("\n");
}