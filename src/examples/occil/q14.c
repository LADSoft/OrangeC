#include <stdio.h>
#undef putchar

struct nn
{
    int r,s,t;
} bb = {3, 4, 5};

main()
{
    printf("%s", "hi");
    printf("%d", bb.t);
    printf("%d", bb.t);
    putchar('a'); 
    putchar('b');     
//    for (int i=0; i < 10; i++)
//    putchar('b');     
//        printf("%d\n", i);
    return 0;
}