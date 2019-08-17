#include <stdio.h>
#undef stderr
#undef stdin
#undef stdout
FILE *__iob_func();
#define stdin __iob_func()
#define stdout (__iob_func()+1)
#define stderr (__iob_func()+2)

#define RAND_MASK ((rNToGo == 1) ? 1 : 0)

int rNToGo;

void zz(int aa)
{
    printf("%d\n", 1 << aa);
}
int main(int argc, char *argv[])
{
    argc ^= (unsigned)RAND_MASK;
    return argc;
}