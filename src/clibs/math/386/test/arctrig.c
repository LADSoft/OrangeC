#include <math.h>


int main()
{
    float i ;
    for (i= -1.0; i < 1.1; i+=.1)
        printf("%f %f\n",(double)acosl(cosl(i)),(double)asinl(sinl(i)));
    for (i= -10.0; i < 10.1; i+=.1)
        printf("%f %f\n",(double)atanl(i),(double)atan2l(1.0,i));
    printf("%f\n",(double)atan2l(0.0,0.0));
}