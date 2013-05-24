#include <math.h>
#include <limits.h>
#include <stdio.h>

int main()
{
    float i ;
    printf("%f %f\n",(double)lroundf(1.5),(double)roundl(-1.5));

    for (i=-2; i <= 2; i+=.1)
        printf("%f %f %f %f %f %f %f\n",(double)i,(double)roundl(i),(double)ceill(i),(double)floorl(i),(double)truncl(i),(double)nearbyintl(i),(double)rintl(i));
    for (i= -10; i < 10; i+=.2)
        printf("%f %d %lld %d %lld\n",i,lrintl(i),llrintl(i),lroundl(i),llroundl(i));
    i = UINT_MAX * 10.0;
    printf("%f %d %lld %d %lld",i,lrintl(i),llrintl(i),lroundl(i),llroundl(i));
    i = LLONG_MAX * 10.0;
    printf("%f %d %lld %d %lld",i,lrintl(i),llrintl(i),lroundl(i),llroundl(i));
}