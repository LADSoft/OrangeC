#include <complex.h>
#include <math.h>
#include <stdio.h>
int main()
{
    double x,y;
    double complex result,aresult;
//    asm int3;
//    x = atan2(10,.1);
//    printf("%f\n",x);
    result = ctan(1.5 + 0 * I);
    aresult = catan(result) ;
    printf("%f %f %.20f %.20f %.20f %.20f\n",0.1,0.1,(double)creal(result),(double)cimag(result),(double)creal(aresult),(double)cimag(aresult));
    for (x = -10; x < 10; x += 0.5)
        for (y=-10; y < 10; y += 0.5) {
            result = ccos(x + I *y);
            aresult = cacos(result);
            printf("%f %f %f %f %f %f\n",x,y,(double)creal(result),(double)cimag(result),(double)creal(aresult),(double)cimag(aresult));
        }
    for (x = -10; x < 10; x += 0.5)
        for (y=-10; y < 10; y += 0.5) {
            result = csin(x + I *y);
            aresult = casin(result);
            printf("%f %f %f %f %f %f\n",x,y,(double)creal(result),(double)cimag(result),(double)creal(aresult),(double)cimag(aresult));
        }
    for (x = -10; x < 10; x += 0.5)
        for (y=-10; y < 10; y += 0.5) {
            result = ctan(x + I *y);
            aresult = catan(result);
            printf("%f %f %f %f %f %f\n",x,y,(double)creal(result),(double)cimag(result),(double)creal(aresult),(double)cimag(aresult));
        }
}
