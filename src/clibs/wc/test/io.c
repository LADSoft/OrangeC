#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    int c,i;
    wchar_t s[100];
    FILE *fil = fopen("hi.txt","w+");
    printf("%d\n",fwide(fil,0));
    fputwc(L'c',fil);
    fputws(L"hi dave\n",fil);
    fwprintf(fil,L"%ls\n",L"geez");
    printf("%d\n",fwide(fil,0));
    printf("%d\n",fputc('z',fil));
    printf("%d\n",fwide(fil,-1));
    printf("%d\n",fputc('y',fil));
    printf("%d\n",fputc('y',fil));
    rewind(fil);
    printf("%d\n",fwide(fil,1));
    printf("%04x\n",fgetwc(fil));
    printf("%04x\n",fgetwc(fil));
    printf(":%x: ",fgetws(s,100,fil));
    for (i=0; i <wcslen(s); i++)
        printf("%04x ",s[i]);
    printf("\n");
    fwscanf(fil,L"%ls ",s);
    for (i=0; i <wcslen(s); i++)
        printf("%04x ",s[i]);
    printf("\n");
    printf("%d\n",fwide(fil,-1));
    printf("%02x\n",fgetc(fil));        
    fclose(fil);
}