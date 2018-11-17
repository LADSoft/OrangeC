#include <stdio.h>
int main()
{
    printf("%d\n", __builtin_ctz(12));
    printf("%d\n", __builtin_clz(12));
    printf("%d\n", __builtin_ctzl(12));
    printf("%d\n", __builtin_clzl(12));
    return 0;
}