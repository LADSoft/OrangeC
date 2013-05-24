#include <math.h>


int main()
{
    float i ;
    for (i= -2; i < 2; i+=0.1)
    {
        printf("atanh %f %f %f\n",i,(double)tanhl(i),(double)atanhl(tanhl(i)));
        printf("acosh %f %f %f\n",i,(double)coshl(i),(double)acoshl(coshl(i)));
        printf("asinh %f %f %f\n",i,(double)sinhl(i),(double)asinhl(sinhl(i)));
    }
    atanhl(1); atanhl(-1); atanhl(10); acoshl(0);
}