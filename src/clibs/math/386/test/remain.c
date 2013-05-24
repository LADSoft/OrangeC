#include <math.h>
#include <limits.h>
#include <stdio.h>

int main()
{
    float i ;
    int expx;
    printf("%f\n",(double)fmodl(-7,7));
    for (i=-10; i < 10; i++)
        printf("%f %f %f %f\n",i,(double)fmodl(i,7),(double)fmodl(i,-7),(double)remainderl(i,7));
    for (i=-700; i < 700; i += 1) {
        float nn = (double)remquol(i,7,&expx);
        printf("%f %f %d\n",i,nn,expx);
    }
    fmod(10,0); remainderl(10,0);remquol(10,0,&expx);
    
    printf("%f %f %f %f\n",copysign(4000,-5),copysign(-4000,-5), copysign(4000,5), copysign(-4000,5));
    printf("%f %f %f %f\n",copysignf(4000,-5),copysignf(-4000,-5), copysignf(4000,5), copysignf(-4000,5));
    printf("%lf %lf %lf %lf\n",copysignl(4000,-5),copysignl(-4000,-5), copysignl(4000,5), copysignl(-4000,5));
}