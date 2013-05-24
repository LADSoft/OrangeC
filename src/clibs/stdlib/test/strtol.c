#include <stdlib.h>
#include <errno.h>

void testi(char *str,int radix)
{
    char *endptr;
    long long aa ;
    printf("\n:%s:%d\n",str,radix);
    errno = 0;
    aa = strtol(str,&endptr,radix);
    printf("strtol: %d %lld \"%s\"\n",errno,aa,endptr);
    errno = 0;
    aa = strtoll(str,&endptr,radix);
    printf("strtoll: %d %lld \"%s\"\n",errno,aa,endptr);
    errno = 0;
    aa = strtoul(str,&endptr,radix);
    printf("strtoul: %d %lld \"%s\"\n",errno,aa,endptr);
    errno = 0;
    aa = strtoull(str,&endptr,radix);
    printf("strtoull: %d %lld \"%s\"\n",errno,aa,endptr);
}

int main()
{
    testi("p",0);
    testi("0xppp",16);
    testi("+p",0);
    testi("-p",0);
    testi("44",0);
    testi("44",16);
    testi("-44",16);
    testi("099",0);
    testi("099",10);
    testi("077",0);
    testi("077",8);
    testi("077",10);
    testi("0x44",0);
    testi("0x44",10);
    testi("0x44",8);
    testi("0x44",16);
    testi("-077",0);
    testi("-077",8);
    testi("-077",10);
    testi("-099",0);
    testi("-099",10);
    testi("-0x44",0);
    testi("-0x44",10);
    testi("-0x44",8);
    testi("-0x44",16);
    testi("0x123456789A",0);
    testi("-0x123456789A",0);
}