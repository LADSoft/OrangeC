#include <complex.h>
#include <math.h>
#include <stdio.h>
int main()
{
    double x,y;
    double complex result;
    for (x = -10; x < 10; x += 0.5)
        for (y=-10; y < 10; y += 0.5) {
            result = ccos(x + I *y);
            printf("%f %f %f %f %f %f\n",x,y,(double)cos(x)*cosh(y),(double)creal(result),(double)-sin(x)*sinh(y),(double)cimag(result));
        }
    for (x = -10; x < 10; x += 0.5)
        for (y=-10; y < 10; y += 0.5) {
            result = csin(x + I *y);
            printf("%f %f %f %f %f %f\n",x,y,(double)sin(x)*cosh(y),(double)creal(result),(double)cos(x)*sinh(y),(double)cimag(result));
        }
    for (x = -10; x < 10; x += 0.5)
        for (y=-10; y < 10; y += 0.5) {
            double d = cosl( 2.0L * x ) + coshl( 2.0L * y );
            result = ctan(x + I *y);
            printf("%f %f %f %f %f %f\n",x,y,(double)sin(2.0*x)/d,(double)creal(result),(double)sinh(2.0*y)/d,(double)cimag(result));
        }
}