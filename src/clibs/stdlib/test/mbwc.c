#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    wchar_t s[] = { 0x21,0x600,0x7ff,0x800,0xffff,0 };
    unsigned char buf[256],*p=buf;
    wchar_t q[256],*r=q;
    int i,l;
    memset(buf,0xaa,sizeof(buf));
    for (i=0; i < sizeof(s)/sizeof(wchar_t); i++)
        p += wctomb(p,s[i]);
    for (i=0; i < strlen(buf); i++)
        printf("%02x ",buf[i]);


    for (i=0; i <strlen(buf); i++) {
        int zz = mbtowc(r,buf+i,1);
        printf("::%d\n",zz);
        if (zz > 0) {
            r++;
        }
    }
    for (i=0; i <wcslen(q); i++)
        printf("%04x ",q[i]);
    
    wmemset(q,0,sizeof(q)/sizeof(wchar_t));
    r = q ;
    for (i=0; i <strlen(buf); i++) {
        int zz = mbtowc(r,buf+i,MB_CUR_MAX);
        printf("::%d\n",zz);
        if (zz > 0) {
            r++;
            i += zz-1;
        }
    }
    for (i=0; i <wcslen(q); i++)
        printf("%04x ",q[i]);
    printf("\n");
    p = buf; 
    r = s;
    l = wcstombs(p,r,256);
    for (i=0; i < l+1; i++)
        printf("%02x ",buf[i]);
    printf("\n");
    p = buf;
    r = q;
    l = mbstowcs(r,p,256);
    for (i=0; i < l+1; i++)
        printf("%04x ",q[i]);

}