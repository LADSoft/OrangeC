#include <complex.h>
#include <math.h>
#include <stdio.h>
int main()
{
    double x,y;
    double complex result,aresult;
    result = catanh(-1.0 + 0 * I);
#ifdef XXXXX
    for (x = -10; x < 10; x += 0.5)
        for (y=-10; y < 10; y += 0.5) {
            result = cacosh(x + I *y);
            aresult = I * cacos(x + I * y);
            printf("%f %f %f %f %f %f\n",x,y,(double)creal(result),(double)cimag(result),(double)creal(aresult),(double)cimag(aresult));
        }
    for (x = -10; x < 10; x += 0.5)
        for (y=-10; y < 10; y += 0.5) {
            result = casinh(x + I *y);
            aresult = x + y * I;
            aresult = I * aresult;
            aresult =  -I *casin(aresult) ;
            printf("%f %f %f %f %f %f\n",x,y,(double)creal(result),(double)cimag(result),(double)creal(aresult),(double)cimag(aresult));
        }
#endif
    for (x = -10; x < 10; x += 0.5)
        for (y=-10; y < 10; y += 0.5) {
            result = catanh(x + I * y);
            aresult = x + y * I;
            aresult = I * aresult;
            aresult =  -I *catan(aresult) ;
            printf("%f %f %f %f %f %f\n",x,y,(double)creal(result),(double)cimag(result),(double)creal(aresult),(double)cimag(aresult));
        }
}
