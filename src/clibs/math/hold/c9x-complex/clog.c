/*							clog.c
 *
 *	Complex natural logarithm
 *
 *
 *
 * SYNOPSIS:
 *
 * double complex clog();
 * double complex z, w;
 *
 * w = clog (z);
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns complex logarithm to the base e (2.718...) of
 * the complex argument x.
 *
 * If z = x + iy, r = sqrt( x**2 + y**2 ),
 * then
 *       w = log(r) + i arctan(y/x).
 * 
 * The arctangent ranges from -PI to +PI.
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10      7000       8.5e-17     1.9e-17
 *    IEEE      -10,+10     30000       5.0e-15     1.1e-16
 *
 * Larger relative error can be observed for z near 1 +i0.
 * In IEEE arithmetic the peak absolute error is 5.2e-16, rms
 * absolute error 1.0e-16.
 */

#include "complex.h"
#include "mconf.h"

#ifdef ANSIPROT
static void cchsh ( double x, double *c, double *s );
static double redupi ( double x );
static double ctans ( double complex z );
#else
static void cchsh();
static double redupi();
static double ctans();
double cabs(), fabs(), sqrt();
double log(), exp(), atan2(), cosh(), sinh();
double asin(), sin(), cos();
#endif

extern double MAXNUM, MACHEP, PI, PIO2;

double complex
clog (z)
     double complex z;
{
  double complex w;
  double p, rr;

  /*rr = sqrt( z->r * z->r  +  z->i * z->i );*/
  rr = cabs(z);
  p = log(rr);
  rr = atan2 (cimag (z), creal (z));
  w = p + rr * I;
  return (w);
}
/*							cexp()
 *
 *	Complex exponential function
 *
 *
 *
 * SYNOPSIS:
 *
 * double complex cexp ();
 * double complex z, w;
 *
 * w = cexp (z);
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the exponential of the complex argument z
 * into the complex result w.
 *
 * If
 *     z = x + iy,
 *     r = exp(x),
 *
 * then
 *
 *     w = r cos y + i r sin y.
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10      8700       3.7e-17     1.1e-17
 *    IEEE      -10,+10     30000       3.0e-16     8.7e-17
 *
 */

double complex
cexp(z)
     double complex z;
{
  double complex w;
  double r, x, y;

  x = creal (z);
  y = cimag (z);
  r = exp (x);
  w = r * cos (y) + r * sin (y) * I;
  return (w);
}
/*							csin()
 *
 *	Complex circular sine
 *
 *
 *
 * SYNOPSIS:
 *
 * double complex csin();
 * double complex z, w;
 *
 * w = csin (z);
 *
 *
 *
 * DESCRIPTION:
 *
 * If
 *     z = x + iy,
 *
 * then
 *
 *     w = sin x  cosh y  +  i cos x sinh y.
 *
 * csin(z) = -i csinh(iz).
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10      8400       5.3e-17     1.3e-17
 *    IEEE      -10,+10     30000       3.8e-16     1.0e-16
 * Also tested by csin(casin(z)) = z.
 *
 */

double complex
csin (z)
     double complex z;
{
  double complex w;
  double ch, sh;

  cchsh( cimag (z), &ch, &sh );
  w = sin (creal(z)) * ch + (cos (creal(z)) * sh) * I;
  return (w);
}



/* calculate cosh and sinh */

static void
cchsh( x, c, s )
     double x, *c, *s;
{
  double e, ei;

  if (fabs(x) <= 0.5)
    {
      *c = cosh(x);
      *s = sinh(x);
    }
  else
    {
      e = exp(x);
      ei = 0.5/e;
      e = 0.5 * e;
      *s = e - ei;
      *c = e + ei;
    }
}

/*							ccos()
 *
 *	Complex circular cosine
 *
 *
 *
 * SYNOPSIS:
 *
 * double complex ccos();
 * double complex z, w;
 *
 * w = ccos (z);
 *
 *
 *
 * DESCRIPTION:
 *
 * If
 *     z = x + iy,
 *
 * then
 *
 *     w = cos x  cosh y  -  i sin x sinh y.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10      8400       4.5e-17     1.3e-17
 *    IEEE      -10,+10     30000       3.8e-16     1.0e-16
 */

double complex
ccos (z)
     double complex z;
{
  double complex w;
  double ch, sh;

  cchsh( cimag(z), &ch, &sh );
  w = cos(creal (z)) * ch - (sin (creal (z)) * sh) * I;
  return (w);
}
/*							ctan()
 *
 *	Complex circular tangent
 *
 *
 *
 * SYNOPSIS:
 *
 * double complex ctan();
 * double complex z, w;
 *
 * w = ctan (z);
 *
 *
 *
 * DESCRIPTION:
 *
 * If
 *     z = x + iy,
 *
 * then
 *
 *           sin 2x  +  i sinh 2y
 *     w  =  --------------------.
 *            cos 2x  +  cosh 2y
 *
 * On the real axis the denominator is zero at odd multiples
 * of PI/2.  The denominator is evaluated by its Taylor
 * series near these points.
 *
 * ctan(z) = -i ctanh(iz).
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10      5200       7.1e-17     1.6e-17
 *    IEEE      -10,+10     30000       7.2e-16     1.2e-16
 * Also tested by ctan * ccot = 1 and catan(ctan(z))  =  z.
 */

double complex
ctan (z)
     double complex z;
{
  double complex w;
  double d;

  d = cos (2.0 * creal (z)) + cosh (2.0 * cimag (z));

  if (fabs(d) < 0.25)
    d = ctans (z);

  if (d == 0.0 )
    {
      mtherr ("ctan", OVERFLOW);
      w = MAXNUM + MAXNUM * I;
      return (w);
    }

  w = sin (2.0 * creal(z)) / d + (sinh (2.0 * cimag(z)) / d) * I;
  return (w);
}
/*							ccot()
 *
 *	Complex circular cotangent
 *
 *
 *
 * SYNOPSIS:
 *
 * double complex ccot();
 * double complex z, w;
 *
 * w = ccot (z);
 *
 *
 *
 * DESCRIPTION:
 *
 * If
 *     z = x + iy,
 *
 * then
 *
 *           sin 2x  -  i sinh 2y
 *     w  =  --------------------.
 *            cosh 2y  -  cos 2x
 *
 * On the real axis, the denominator has zeros at even
 * multiples of PI/2.  Near these points it is evaluated
 * by a Taylor series.
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10      3000       6.5e-17     1.6e-17
 *    IEEE      -10,+10     30000       9.2e-16     1.2e-16
 * Also tested by ctan * ccot = 1 + i0.
 */

double complex
ccot (z)
     double complex z;
{
  double complex w;
  double d;

  d = cosh (2.0 * cimag (z)) - cos (2.0 * creal(z));

  if (fabs(d) < 0.25)
    d = ctans (z);

  if (d == 0.0)
    {
      mtherr ("ccot", OVERFLOW);
      w = MAXNUM + MAXNUM * I;
      return (w);
    }

  w = sin (2.0 * creal(z)) / d - (sinh (2.0 * cimag(z)) / d) * I;
  return w;
}

/* Program to subtract nearest integer multiple of PI */
/* extended precision value of PI: */
#ifdef UNK
static double DP1 = 3.14159265160560607910E0;
static double DP2 = 1.98418714791870343106E-9;
static double DP3 = 1.14423774522196636802E-17;
#endif

#ifdef DEC
static unsigned short P1[] = {0040511,0007732,0120000,0000000,};
static unsigned short P2[] = {0031010,0055060,0100000,0000000,};
static unsigned short P3[] = {0022123,0011431,0105056,0001560,};
#define DP1 *(double *)P1
#define DP2 *(double *)P2
#define DP3 *(double *)P3
#endif

#ifdef IBMPC
static unsigned short P1[] = {0x0000,0x5400,0x21fb,0x4009};
static unsigned short P2[] = {0x0000,0x1000,0x0b46,0x3e21};
static unsigned short P3[] = {0xc06e,0x3145,0x6263,0x3c6a};
#define DP1 *(double *)P1
#define DP2 *(double *)P2
#define DP3 *(double *)P3
#endif

#ifdef MIEEE
static unsigned short P1[] = {
0x4009,0x21fb,0x5400,0x0000
};
static unsigned short P2[] = {
0x3e21,0x0b46,0x1000,0x0000
};
static unsigned short P3[] = {
0x3c6a,0x6263,0x3145,0xc06e
};
#define DP1 *(double *)P1
#define DP2 *(double *)P2
#define DP3 *(double *)P3
#endif

static double
redupi(x)
     double x;
{
  double t;
  long i;

  t = x/PI;
  if( t >= 0.0 )
    t += 0.5;
  else
    t -= 0.5;

  i = t;	/* the multiple */
  t = i;
  t = ((x - t * DP1) - t * DP2) - t * DP3;
  return (t);
}

/*  Taylor series expansion for cosh(2y) - cos(2x)	*/

static double
ctans (z)
     double complex z;
{
  double f, x, x2, y, y2, rn, t;
  double d;

  x = fabs (2.0 * creal (z));
  y = fabs (2.0 * cimag(z));

  x = redupi(x);

  x = x * x;
  y = y * y;
  x2 = 1.0;
  y2 = 1.0;
  f = 1.0;
  rn = 0.0;
  d = 0.0;
  do
    {
      rn += 1.0;
      f *= rn;
      rn += 1.0;
      f *= rn;
      x2 *= x;
      y2 *= y;
      t = y2 + x2;
      t /= f;
      d += t;

      rn += 1.0;
      f *= rn;
      rn += 1.0;
      f *= rn;
      x2 *= x;
      y2 *= y;
      t = y2 - x2;
      t /= f;
      d += t;
    }
  while (fabs(t/d) > MACHEP);
  return (d);
}
/*							casin()
 *
 *	Complex circular arc sine
 *
 *
 *
 * SYNOPSIS:
 *
 * double complex casin();
 * double complex z, w;
 *
 * w = casin (z);
 *
 *
 *
 * DESCRIPTION:
 *
 * Inverse complex sine:
 *
 *                               2
 * w = -i clog( iz + csqrt( 1 - z ) ).
 *
 * casin(z) = -i casinh(iz)
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10     10100       2.1e-15     3.4e-16
 *    IEEE      -10,+10     30000       2.2e-14     2.7e-15
 * Larger relative error can be observed for z near zero.
 * Also tested by csin(casin(z)) = z.
 */

double complex
casin (z)
     double complex z;
{
  double complex w;
  static double complex ca, ct, zz, z2;
  double x, y;

  x = creal (z);
  y = cimag (z);

  if (y == 0.0)
    {
      if (fabs(x) > 1.0)
	{
	  w = PIO2 + 0.0 * I;
	  mtherr ("casin", DOMAIN);
	}
      else
	{
	  w = asin (x) + 0.0 * I;
	}
      return (w);
    }

/* Power series expansion */
/*
b = cabs(z);
if( b < 0.125 )
{
z2.r = (x - y) * (x + y);
z2.i = 2.0 * x * y;

cn = 1.0;
n = 1.0;
ca.r = x;
ca.i = y;
sum.r = x;
sum.i = y;
do
	{
	ct.r = z2.r * ca.r  -  z2.i * ca.i;
	ct.i = z2.r * ca.i  +  z2.i * ca.r;
	ca.r = ct.r;
	ca.i = ct.i;

	cn *= n;
	n += 1.0;
	cn /= n;
	n += 1.0;
	b = cn/n;

	ct.r *= b;
	ct.i *= b;
	sum.r += ct.r;
	sum.i += ct.i;
	b = fabs(ct.r) + fabs(ct.i);
	}
while( b > MACHEP );
w->r = sum.r;
w->i = sum.i;
return;
}
*/


  ca = x + y * I;
  ct = ca * I;
	/* sqrt( 1 - z*z) */
  /* cmul( &ca, &ca, &zz ) */
  /*x * x  -  y * y */
  zz = (x - y) * (x + y) + (2.0 * x * y) * I;

  zz = 1.0 - creal(zz) - cimag(zz) * I;
  z2 = csqrt (zz);

  zz = ct + z2;
  zz = clog (zz);
  /* multiply by 1/i = -i */
  w = zz * (-1.0 * I);
  return (w);
}
/*							cacos()
 *
 *	Complex circular arc cosine
 *
 *
 *
 * SYNOPSIS:
 *
 * double complex cacos();
 * double complex z, w;
 *
 * w = cacos (z);
 *
 *
 *
 * DESCRIPTION:
 *
 *
 * w = arccos z  =  PI/2 - arcsin z.
 *
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10      5200      1.6e-15      2.8e-16
 *    IEEE      -10,+10     30000      1.8e-14      2.2e-15
 */

double complex
cacos (z)
     double complex z;
{
  double complex w;

  w = casin (z);
  w = (PIO2  -  creal (w)) - cimag (w) * I;
  return (w);
}
/*							catan()
 *
 *	Complex circular arc tangent
 *
 *
 *
 * SYNOPSIS:
 *
 * double complex catan();
 * double complex z, w;
 *
 * w = catan (z);
 *
 *
 *
 * DESCRIPTION:
 *
 * If
 *     z = x + iy,
 *
 * then
 *          1       (    2x     )
 * Re w  =  - arctan(-----------)  +  k PI
 *          2       (     2    2)
 *                  (1 - x  - y )
 *
 *               ( 2         2)
 *          1    (x  +  (y+1) )
 * Im w  =  - log(------------)
 *          4    ( 2         2)
 *               (x  +  (y-1) )
 *
 * Where k is an arbitrary integer.
 *
 * catan(z) = -i catanh(iz).
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10      5900       1.3e-16     7.8e-18
 *    IEEE      -10,+10     30000       2.3e-15     8.5e-17
 * The check catan( ctan(z) )  =  z, with |x| and |y| < PI/2,
 * had peak relative error 1.5e-16, rms relative error
 * 2.9e-17.  See also clog().
 */

double complex
catan (z)
     double complex z;
{
  double complex w;
  double a, t, x, x2, y;

  x = creal (z);
  y = cimag (z);

  if ((x == 0.0) && (y > 1.0))
    goto ovrf;

  x2 = x * x;
  a = 1.0 - x2 - (y * y);
  if (a == 0.0)
    goto ovrf;

  t = 0.5 * atan2 (2.0 * x, a);
  w = redupi (t);

  t = y - 1.0;
  a = x2 + (t * t);
  if (a == 0.0)
    goto ovrf;

  t = y + 1.0;
  a = (x2 + (t * t))/a;
  w = w + (0.25 * log (a)) * I;
  return (w);

ovrf:
  mtherr ("catan", OVERFLOW);
  w = MAXNUM + MAXNUM * I;
  return (w);
}


/*							csinh
 *
 *	Complex hyperbolic sine
 *
 *
 *
 * SYNOPSIS:
 *
 * double complex csinh();
 * double complex z, w;
 *
 * w = csinh (z);
 *
 * DESCRIPTION:
 *
 * csinh z = (cexp(z) - cexp(-z))/2
 *         = sinh x * cos y  +  i cosh x * sin y .
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       3.1e-16     8.2e-17
 *
 */

double complex
csinh (z)
     double complex z;
{
  double complex w;
  double x, y;

  x = creal(z);
  y = cimag(z);
  w = sinh (x) * cos (y)  +  (cosh (x) * sin (y)) * I;
  return (w);
}


/*							casinh
 *
 *	Complex inverse hyperbolic sine
 *
 *
 *
 * SYNOPSIS:
 *
 * double complex casinh();
 * double complex z, w;
 *
 * w = casinh (z);
 *
 *
 *
 * DESCRIPTION:
 *
 * casinh z = -i casin iz .
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       1.8e-14     2.6e-15
 *
 */

double complex
casinh (z)
     double complex z;
{
  double complex w;

  w = -1.0 * I * casin (z * I);
  return (w);
}


/*							ccosh
 *
 *	Complex hyperbolic cosine
 *
 *
 *
 * SYNOPSIS:
 *
 * double complex ccosh();
 * double complex z, w;
 *
 * w = ccosh (z);
 *
 *
 *
 * DESCRIPTION:
 *
 * ccosh(z) = cosh x  cos y + i sinh x sin y .
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       2.9e-16     8.1e-17
 *
 */

double complex
ccosh (z)
     double complex z;
{
  double complex w;
  double x, y;

  x = creal(z);
  y = cimag(z);
  w = cosh (x) * cos (y)  +  (sinh (x) * sin (y)) * I;
  return (w);
}



/*							cacosh
 *
 *	Complex inverse hyperbolic cosine
 *
 *
 *
 * SYNOPSIS:
 *
 * double complex cacosh();
 * double complex z, w;
 *
 * w = cacosh (z);
 *
 *
 *
 * DESCRIPTION:
 *
 * acosh z = i acos z .
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       1.6e-14     2.1e-15
 *
 */

double complex
cacosh (z)
     double complex z;
{
  double complex w;

  w = I * cacos (z);
  return (w);
}


/*							ctanh
 *
 *	Complex hyperbolic tangent
 *
 *
 *
 * SYNOPSIS:
 *
 * double complex ctanh();
 * double complex z, w;
 *
 * w = ctanh (z);
 *
 *
 *
 * DESCRIPTION:
 *
 * tanh z = (sinh 2x  +  i sin 2y) / (cosh 2x + cos 2y) .
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       1.7e-14     2.4e-16
 *
 */

double complex
ctanh (z)
     double complex z;
{
  double complex w;
  double x, y, d;

  x = creal(z);
  y = cimag(z);
  d = cosh (2.0 * x) + cos (2.0 * y);
  w = sinh (2.0 * x) / d  +  (sin (2.0 * y) / d) * I;
  return (w);
}


/*							catanh
 *
 *	Complex inverse hyperbolic tangent
 *
 *
 *
 * SYNOPSIS:
 *
 * double complex catanh();
 * double complex z, w;
 *
 * w = catanh (z);
 *
 *
 *
 * DESCRIPTION:
 *
 * Inverse tanh, equal to  -i catan (iz);
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       2.3e-16     6.2e-17
 *
 */

double complex
catanh (z)
     double complex z;
{
  double complex w;

  w = -1.0 * I * catan (z * I);
  return (w);
}

/*							cpow
 *
 *	Complex power function
 *
 *
 *
 * SYNOPSIS:
 *
 * double complex cpow();
 * double complex a, z, w;
 *
 * w = cpow (a, z);
 *
 *
 *
 * DESCRIPTION:
 *
 * Raises complex A to the complex Zth power.
 * Definition is per AMS55 # 4.2.8,
 * analytically equivalent to cpow(a,z) = cexp(z clog(a)).
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       9.4e-15     1.5e-15
 *
 */

double complex
cpow (a, z)
     double complex a, z;
{
  double complex w;
  double x, y, r, theta, absa, arga;

  x = creal (z);
  y = cimag (z);
  absa = cabs (a);
  if (absa == 0.0)
    {
      return (0.0 + 0.0 * I);
    }
  arga = carg (a);
  r = pow (absa, x);
  theta = x * arga;
  if (y != 0.0)
    {
      r = r * exp (-y * arga);
      theta = theta + y * log (absa);
    }
  w = r * cos (theta) + (r * sin (theta)) * I;
  return (w);
}
