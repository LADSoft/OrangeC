#include <math.h>

int main()
{
    float i ;
    
    for (i=-27; i <= 27.1; i+=0.1)
        printf("%f %f\n",(double)sqrtl(i),(double)cbrtl(i));
        
    printf("%f %f\n",(double)pow10l(4),(double)pow10l(-1));
    printf("%f %f %f %f %f %f\n",(double)powl(2,8),(double)powl(3,3),(double)powl(0,-5),(double)powl(-5,4),(double)powl(-5,3),(double)powl(-5,.25));
}