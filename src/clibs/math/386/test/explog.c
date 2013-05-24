#include <math.h>


int main()
{
    float i ;
    for (i= -100; i < 100; i+=0.1)
    {
        int exp;
        double aa = frexpl(i,&exp);
        printf("%f %d %f %f\n",i,exp,aa,(double)(aa*exp2l(exp)));
    }
    for (i=-10; i < 10; i+=1)
        printf("%f\n",(double)ldexpl(i,(int)i));
        {
    int expx;
    double aa = frexpl(0,&expx);
    printf("%d %f %f\n",expx,aa,(double)(aa*exp2l(i)));
    printf("%f\n",ldexpl(5.0,22000));
    printf("%d\n",(int)log10l(-.1));
    for (i=0.1; i < 20; i+=.1)
        printf("%f %f %f %f %f %f\n",(double)i,(double)logl(i),(double)log1pl(i-1),(double)
        expl(logl(i)),(double)log10l(i),(double)pow10l(log10l(i)));
        }
}