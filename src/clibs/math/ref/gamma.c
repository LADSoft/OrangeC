#include <math.h>
#include <stdio.h>
#define ITMAX 100
#define EPS 3.0e-7
#define FPMIN 1.0e-30 
float gammln(float xx)
{
    double x,y,tmp,ser;
    static double cof[6] = { 76.18009172947146, -86.50532032941677,
                             24.01409824083091, -1.231739572450155,
                             0.1208650973866179e-2,-0.5395239384953e-5};
    int j;
    y=x=xx;
    tmp=x+5.5 ;
    tmp -= (x+0.5)*log(tmp);
    ser=1.000000000190015;
    for(j=0;j<=5;++j) ser += cof[j]/++y ;
    return -tmp+log(2.5066282746310005*ser/x);
}

float gser(float a, float x)
{
    int n;
    float sum,del,ap, gln;
    gln = gammln(a);
    if (x <= 0.0) {
        if (x < 0.0) printf("domain error");
        return 0.0;
    } else {
        ap=a;
        del=sum=1.0/a;
        for (n=1;n<=ITMAX;n++) {
            ++ap;
            del *= x/ap;
            sum += del;
            if (fabs(del) < fabs(sum)*EPS) {
                return sum*exp(-x+a*log(x)-(gln));
            }
        }
        printf("range err");
        return 0.0;
    }
}
float gcf(float a, float x)
{
    int i;
    float an,b,c,d,del,h, gln;
    gln=gammln(a);
    b=x+1.0-a; 
    c=1.0/FPMIN;
    d=1.0/b;
    h=d;
    for (i=1;i<=ITMAX;i++) {
        an = -i*(i-a);
        b += 2.0;
        d=an*d+b;
        if (fabs(d) < FPMIN) 
            d=FPMIN;
        c=b+an/c;
        if (fabs(c) < FPMIN) 
            c=FPMIN;
        d=1.0/d;
        del=d*c;
        h *= del;
        if (fabs(del-1.0) < EPS) 
            break;
    }
    if (i > ITMAX) printf("range err");
    return exp(-x+a*log(x)-(gln))*h;
}
float gammp(float a, float x)
{
    if (x < 0.0 || a <= 0.0) 
        printf("domain err");
    if (x < (a+1.0)) { 
        return gser(a,x);
    } else { 
        return 1.0 - gcf(a,x);
    }
}
float gammq(float a, float x)
{
    if (x < 0.0 || a <= 0.0) printf("Invalid arguments in routine gammq");
    if (x < (a+1.0)) {
        return 1.0 - gser(a,x);
    } else {
        return gcf(a,x);
    }
}
float erff(float x)
{
    return x < 0.0 ? -gammp(0.5,x*x) : gammp(0.5,x*x);
}
float erffc(float x)
{
    return x < 0.0 ? 1.0+gammp(0.5,x*x) : gammq(0.5,x*x);
}
main()
{
    double i;
    for (i=-10; i < 10; i+=.1)
        printf("%f %f %f\n",i, erff(i), erffc(i));
        printf("%.20f",gammln(0.5));
    return 0;
}