#include <stdio.h>
int main()
{
    // Should compile pre-C2x
    _Bool bool = 1;
    printf("%hhd\n", bool);
}