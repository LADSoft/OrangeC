#include <complex.h>
#include <math.h>
#include <stdio.h>
long double complex
acpowl (long double complex a, long double complex z)
{
  long double complex w;
  long double x, y, r, theta, absa, arga;

  x = creal (z);
  y = cimag (z);
  absa = cabsl (a);
  if (absa == 0.0L)
    {
      return (0.0L + 0.0L * I);
    }
  arga = cargl (a);
  r = powl (absa, x);
  theta = x * arga;
  if (y != 0.0L)
    {
      r = r * expl (-y * arga);
      theta = theta + y * logl (absa);
    }
  w = r * cosl (theta) + (r * sinl (theta)) * I;
  return (w);
}
int main()
{
    double x,y;
    double complex result,aresult;
#ifdef XXXXX
    for (x = -4; x < 4; x += 0.5)
        for (y=-4; y < 4; y += 0.5) {
            result = cexp(x + I *y);
            aresult = clog(result);
            printf("%f %f %f %f %f %f\n",x,y,(double)creal(result),(double)cimag(result),(double)creal(aresult),(double)cimag(aresult));
        }
    for (x = -4; x < 4; x += 0.5)
        for (y=-4; y < 4; y += 0.5) {
            double resultd= carg(x + I *y);
            double aresultd = atan2(y,x);
            printf("%f %f %f %f\n",x,y,resultd,aresultd);
        }
#endif
    for (x = -4; x < 4; x += 0.5)
        for (y=-4; y < 4; y += 0.5) {
            result = cpow(x + I *y,y + x * I);
            aresult = acpowl(x + I * y, y + x * I);
            printf("%f %f %f %f %f %f\n",x,y,(double)creal(result),(double)cimag(result),(double)creal(aresult),(double)cimag(aresult));
        }
}