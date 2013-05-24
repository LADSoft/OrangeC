#include <Stdlib.h>
#include <math.h>

void test(char *str)
{
    long double f ;
    char *endptr;
    
    printf("\n:%s:\n",str);
    errno = 0;
    f = strtof(str,&endptr);
    printf("%le:%d %s\n",f,errno,endptr);
    errno = 0;
    f = strtod(str,&endptr);
    printf("%le:%d %s\n",f,errno,endptr);
    errno = 0;
    f = strtold(str,&endptr);
    printf("%le:%d %s\n",f,errno,endptr);
}
int main()
{
    test("4.4e10aaa");
    test("-4.4e+10aaa");
    test("4.4e-10aaa");
    test("0x1fz");
    test("0x1.8P+4");
    test("-0x1.8p4z");
    test("0x1.8p-4");
    test("0x1.8zz");
    test("440aaa");
    test("1e40");
    test("1e400");
    printf("%le",HUGE_VALL);
}
