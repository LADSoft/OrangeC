#include <complex.h>
#include <math.h>
#include <stdio.h>
int main()
{
    double x,y;
    double complex result;
#ifdef XXXXX
    for (x = -10; x < 10; x += 0.5)
        for (y=-10; y < 10; y += 0.5) {
            result = ccosh(x + I *y);
            printf("%f %f %f %f %f %f\n",x,y,(double)cosh(x)*cos(y),(double)creal(result),(double)sinh(x)*sin(y),(double)cimag(result));
        }
    for (x = -10; x < 10; x += 0.5)
        for (y=-10; y < 10; y += 0.5) {
            result = csinh(x + I *y);
            printf("%f %f %f %f %f %f\n",x,y,(double)sinh(x)*cos(y),(double)creal(result),(double)cosh(x)*sin(y),(double)cimag(result));
        }
#endif
    for (x = -10; x < 10; x += 0.5)
        for (y=-10; y < 10; y += 0.5) {
            double d = coshl( 2.0L * x ) + cosl( 2.0L * y );
            result = ctanh(x + I *y);
            printf("%f %f %f %f %f %f\n",x,y,(double)sinh(2.0*x)/d,(double)creal(result),(double)sin(2.0*y)/d,(double)cimag(result));
        }
}