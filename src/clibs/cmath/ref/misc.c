#include <complex.h>
#include <math.h>
#include <stdio.h>
int main()
{
    double x,y;
    double complex result;
    result = conj(4 + 5 * I);
    printf("%f %f\n",creal(result),cimag(result));
    result = conj(4 - 5 * I);
    printf("%f %f\n",creal(result),cimag(result));
    result = cproj(4 - 5 * I);
    printf("%f %f\n",creal(result),cimag(result));
    result = cproj(-INFINITY + 1 * I);
    printf("%f %f\n",creal(result),cimag(result));
    result = cproj(-1 + INFINITY * I);
    printf("%f %f\n",creal(result),cimag(result));
}