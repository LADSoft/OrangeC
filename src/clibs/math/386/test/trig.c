#include <math.h>


int main()
{
    float i ;
    for (i= -M_PI; i < M_PI; i+=M_PI/100)
        printf("%f %f %f\n",cos(i),sin(i),tan(i));
    cos(1E36);
    sin(1E36);
    tan(1E36);
}