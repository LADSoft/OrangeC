#include <Stdio.h>
struct ss
{
    int a;
    int b:10;
    int c:4;
    int d:14;
} qq = { 1,2,3,4 };


main()
{
    printf("%d %d %d %d\n", qq.a, qq.b, qq.c, qq.d);
    qq.a = 17;
    qq.b = qq.d + qq.c + 12;
    qq.c++;
    ++qq.d;
    qq.d += qq.c;
    printf("%d %d %d %d\n", qq.a, qq.b, qq.c, qq.d);
}