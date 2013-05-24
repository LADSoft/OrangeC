
#include <math.h>
#include <limits.h>
#include <stdio.h>

int main()
{
    float i ;
    printf("%d %d %d %d %d\n",isgreater(4,5),isgreater(5,4),isgreater(0x1.8P+16384L,4), isgreater(1,NAN), isgreater(5,5));
    printf("%d %d %d %d %d\n",isless(4,5),isless(5,4),isless(0x1.8P+16384L,4), isless(1,0x1.8P+16384L), isless(5,5));
    printf("%d %d %d %d %d\n",isgreaterequal(4,5),isgreaterequal(5,4),isgreaterequal(0x1.8P+16384L,4), isgreaterequal(1,0x1.8P+16384L), isgreaterequal(5,5));
    printf("%d %d %d %d %d\n",islessequal(4,5),islessequal(5,4),islessequal(0x1.8P+16384L,4), islessequal(1,0x1.8P+16384L), islessequal(5,5));
    printf("%d %d %d %d %d\n",isunordered(4,5),isunordered(5,4),isunordered(0x1.8P+16384L,4), isunordered(1,0x1.8P+16384L), isunordered(5,5));
    printf("%f %f %f\n",fmin(4,5),fmin(5,4), fmin(4,4));
    printf("%f %f %f\n",fmax(4,5),fmax(5,4), fmax(4,4));
    printf("%f %f %f\n",fdim(4,5),fdim(5,4), fdim(4,4));
    printf("%f %f %f\n",fmaf(4,5,3),fma(4,5,3),(double)fmal(4,5,3));
    int a,b,c;
#ifndef XXXXX
    *(float *)&a = nexttowardf(20,10);
    *(float *)&b = nexttowardf(20,30);
    *(float *)&c = 20;
    printf("%x %x %x\n",a,b,c);
    *(float *)&a = nexttowardf(0,-30);
    *(float *)&b = nexttowardf(0,30);
    *(float *)&c = 0;
    printf("%x %x %x\n",a,b,c);
    *(float *)&a = nexttowardf(0x1.fffffeP+127,-30);
    *(float *)&b = nexttowardf(0x1.fffffeP+127,0x1.ffffffeP+129L);
    *(float *)&c = 0x1.ffffffeP+127;
    printf("%x %x %x\n",a,b,c);
    *(float *)&a = nexttowardf(20,20);
    *(float *)&c = 20;
    printf("%x %x\n",a,c);
#endif
#ifndef XXXXX
    long long a1,b1,c1;
    *(double *)&a1 = nexttoward(20,10);
    *(double *)&b1 = nexttoward(20,30);
    *(double *)&c1 = 20;
    printf("%llx %llx %llx\n",a1,b1,c1);
    *(double *)&a1 = nexttoward(0,-30);
    *(double *)&b1 = nexttoward(0,30);
    *(double *)&c1 = 0;
    printf("%llx %llx %llx\n",a1,b1,c1);
    *(double *)&a1 = nexttoward(20,20);
    *(double *)&c1 = 20;
    printf("%llx %llx\n",a1,c1);
#endif
    char a2[12],b2[12],c2[12];
    *(long double *)&a2 = nexttowardl(20,10);
    *(long double *)&b2 = nexttowardl(20,30);
    *(long double *)&c2 = 20;
    printf("%04x%016llx %04x%016llx %04x%016llx\n",
        (int)*(short *)(a2+8),*(long long *)(a2),
        (int)*(short *)(b2+8),*(long long *)(b2),
        (int)*(short *)(c2+8),*(long long *)(c2));
    *(long double *)&a2 = nexttowardl(0,-30);
    *(long double *)&b2 = nexttowardl(0,30);
    *(long double *)&c2 = 20;
    printf("%04x%016llx %04x%016llx %04x%016llx\n",
        (int)*(short *)(a2+8),*(long long *)(a2),
        (int)*(short *)(b2+8),*(long long *)(b2),
        (int)*(short *)(c2+8),*(long long *)(c2));
    *(long double *)&a2 = nexttowardl(20,20);
    *(long double *)&b2 = nexttowardl(20,20);
    *(long double *)&c2 = 20;
    printf("%04x%016llx %04x%016llx %04x%016llx\n",
        (int)*(short *)(a2+8),*(long long *)(a2),
        (int)*(short *)(b2+8),*(long long *)(b2),
        (int)*(short *)(c2+8),*(long long *)(c2));

}