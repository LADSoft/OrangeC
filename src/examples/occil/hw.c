#include <stdio.h>
int a = 4, b = 5;
main(int argc, char **argv)
{
    fprintf(stderr, "hello world");
    fprintf(stderr,":%d", a + b);
//    return 0;
}