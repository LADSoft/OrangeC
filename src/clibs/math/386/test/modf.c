#include <math.h>

int main()
{
    float i ;
    long double j ;
    i = modfl(1.77,&j);
    printf("%f %f\n",i,(long double)j);
    i = modfl(-1.77,&j);
    printf("%f %f\n",i,(long double)j);
    i = modfl(1E37F,&j);
    printf("%f %f\n",i,(long double)j);
}