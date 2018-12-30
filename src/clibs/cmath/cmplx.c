/*							cmplx.c
 *
 *	Complex number arithmetic
 *      This version is for C9X.
 *
 *
 *
 * SYNOPSIS:
 *
 * typedef struct {
 *      double r;     real part
 *      double i;     imaginary part
 *     }cmplx;
 *
 * cmplx a, b, c;
 *
 * c = cadd( a, b );     c = b + a
 * c = csub( a, b );     c = b - a
 * c = cmul( a, b );     c = b * a
 * c = cdiv( a, b );     c = b / a
 * c = cneg( a );           c = -a
 * cmov( b, c );        c = b
 *
 *
 *
 * DESCRIPTION:
 *
 * Addition:
 *    c.r  =  b.r + a.r
 *    c.i  =  b.i + a.i
 *
 * Subtraction:
 *    c.r  =  b.r - a.r
 *    c.i  =  b.i - a.i
 *
 * Multiplication:
 *    c.r  =  b.r * a.r  -  b.i * a.i
 *    c.i  =  b.r * a.i  +  b.i * a.r
 *
 * Division:
 *    d    =  a.r * a.r  +  a.i * a.i
 *    c.r  = (b.r * a.r  + b.i * a.i)/d
 *    c.i  = (b.i * a.r  -  b.r * a.i)/d
 * ACCURACY:
 *
 * In DEC arithmetic, the test (1/z) * z = 1 had peak relative
 * error 3.1e-17, rms 1.2e-17.  The test (y/z) * (z/y) = 1 had
 * peak relative error 8.3e-17, rms 2.1e-17.
 *
 * Tests in the rectangle {-10,+10}:
 *                      Relative error:
 * arithmetic   function  # trials      peak         rms
 *    DEC        cadd       10000       1.4e-17     3.4e-18
 *    IEEE       cadd      100000       1.1e-16     2.7e-17
 *    DEC        csub       10000       1.4e-17     4.5e-18
 *    IEEE       csub      100000       1.1e-16     3.4e-17
 *    DEC        cmul        3000       2.3e-17     8.7e-18
 *    IEEE       cmul      100000       2.1e-16     6.9e-17
 *    DEC        cdiv       18000       4.9e-17     1.3e-17
 *    IEEE       cdiv      100000       3.7e-16     1.1e-16
 */
/*				cmplx.c
 * complex number arithmetic
 */


/*
Cephes Math Library Release 2.3:  March, 1995
Copyright 1984, 1995 by Stephen L. Moshier
*/


#include "complex.h"
#include "mconf.h"

#ifndef ANSIPROT
double fabs(), cabs(), sqrt(), atan2(), cos(), sin();
double sqrt(), frexp(), ldexp();
#endif

int isnan();

extern double MAXNUM, MACHEP, PI, PIO2, INFINITY;
double complex czero = 0.0;
double complex cone = 1.0;

/*	c = b + a	*/

double complex
cadd( a, b )
     double complex a, b;
{
  return (creal (b) + creal (a) + (cimag (b) + cimag (a)) * I);
}


/*	c = b - a	*/

double complex
csub( a, b )
     double complex a, b;
{
  return (creal (b) - creal (a) + (cimag (b) - cimag (a)) * I);
}

/*	c = b * a */

double complex
cmul( a, b )
     double complex a, b;
{
  return ((creal (b) * creal (a) - cimag (b) * cimag (a))
	  + (creal (b) * cimag (a) + cimag (b) * creal (a)) * I);
}



/*	c = b / a */

double complex
cdiv( a, b )
     double complex a, b;
{
  double y, p, q, w;


  y = creal (a) * creal (a) + cimag (a) * cimag (a);
  p = creal (b) * creal (a) + cimag (b) * cimag (a);
  q = cimag (b) * creal (a) - creal (b) * cimag (a);

  if( y < 1.0 )
    {
      w = MAXNUM * y;
      if ((fabs(p) > w) || (fabs(q) > w) || (y == 0.0))
	{
	  mtherr( "cdiv", OVERFLOW );
	  return (MAXNUM + MAXNUM * I);
	}
    }
  return (p/y + (q/y) * I);
}

/*							cabs()
 *
 *	Complex absolute value
 *
 *
 *
 * SYNOPSIS:
 *
 * double cabs();
 * double complex z;
 * double a;
 *
 * a = cabs( z );
 *
 *
 *
 * DESCRIPTION:
 *
 *
 * If z = x + iy
 *
 * then
 *
 *       a = sqrt( x^2 + y^2 ).
 * 
 * Overflow and underflow are avoided by testing the magnitudes
 * of x and y before squaring.  If either is outside half of
 * the floating point full scale range, both are rescaled.
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -30,+30     30000       3.2e-17     9.2e-18
 *    IEEE      -10,+10    100000       2.7e-16     6.9e-17
 */


/*
Cephes Math Library Release 2.1:  January, 1989
Copyright 1984, 1987, 1989 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/


#ifdef UNK
#define PREC 27
#define MAXEXPD 1024
#define MINEXPD -1077
#endif
#ifdef DEC
#define PREC 29
#define MAXEXPD 128
#define MINEXPD -128
#endif
#ifdef IBMPC
#define PREC 27
#define MAXEXPD 1024
#define MINEXPD -1077
#endif
#ifdef MIEEE
#define PREC 27
#define MAXEXPD 1024
#define MINEXPD -1077
#endif

#if 1
double
cabs( z )
     double complex z;
{
  double x, y, b, re, im;
  int ex, ey, e;

#ifdef INFINITIES
/* Note, cabs(INFINITY,NAN) = INFINITY. */
  if(creal (z) == INFINITY || cimag (z) == INFINITY
     || creal (z) == -INFINITY || cimag (z) == -INFINITY )
    return( INFINITY );
#endif

#ifdef NANS
  if (isnan(creal(z)))
    return (creal(z));
  if(isnan(cimag(z)))
    return(cimag(z));
#endif

  re = fabs (creal(z));
  im = fabs (cimag(z));

  if (re == 0.0)
    return (im);
  if (im == 0.0)
    return (re);

  /* Get the exponents of the numbers */
  x = frexp( re, &ex );
  y = frexp( im, &ey );

  /* Check if one number is tiny compared to the other */
  e = ex - ey;
  if (e > PREC)
    return (re);
  if (e < -PREC)
    return (im);

  /* Find approximate exponent e of the geometric mean. */
  e = (ex + ey) >> 1;

  /* Rescale so mean is about 1 */
  x = ldexp( re, -e );
  y = ldexp( im, -e );
		
  /* Hypotenuse of the right triangle */
  b = sqrt( x * x  +  y * y );

  /* Compute the exponent of the answer. */
  y = frexp( b, &ey );
  ey = e + ey;

  /* Check it for overflow and underflow. */
  if (ey > MAXEXPD)
    {
      mtherr ("cabs", OVERFLOW);
      return (INFINITY);
    }
  if (ey < MINEXPD)
    return (0.0);

  /* Undo the scaling */
  b = ldexp (b, e);
  return (b);
}
#endif /* 1 */
/*							csqrt()
 *
 *	Complex square root
 *
 *
 *
 * SYNOPSIS:
 *
 * double complex csqrt();
 * double complex z, w;
 *
 * w = csqrt (z);
 *
 *
 *
 * DESCRIPTION:
 *
 *
 * If z = x + iy,  r = |z|, then
 *
 *                       1/2
 * Re w  =  [ (r + x)/2 ]   ,
 *
 *                       1/2
 * Im w  =  [ (r - x)/2 ]   .
 *
 * Cancellation error in r-x or r+x is avoided by using the
 * identity  2 Re w Im w  =  y.
 *
 * Note that -w is also a square root of z.  The root chosen
 * is always in the right half plane and Im w has the same sign as y.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    DEC       -10,+10     25000       3.2e-17     9.6e-18
 *    IEEE      -10,+10   1,000,000     2.9e-16     6.1e-17
 *
 */


double complex
csqrt (z)
     double complex z;
{
  double complex w;
  double x, y, r, t, scale;

  x = creal (z);
  y = cimag (z);

  if (y == 0.0)
    {
      if (x == 0.0)
	{
	  w = 0.0 + y * I;
	}
      else
	{
	  r = fabs (x);
	  r = sqrt (r);
	  if (x < 0.0)
	    {
	      w = 0.0 + r * I;
	    }
	  else
	    {
	      w = r + y * I;
	    }
	}
      return (w);
    }
  if (x == 0.0)
    {
      r = fabs (y);
      r = sqrt (0.5*r);
      if (y > 0)
	w = r + r * I;
      else
	w = r - r * I;
      return (w);
    }
 /* Rescale to avoid internal overflow or underflow.  */
 if ((fabs(x) > 4.0) || (fabs(y) > 4.0))
   {
     x *= 0.25;
     y *= 0.25;
     scale = 2.0;
   }
 else
   {
#if 1
     x *= 1.8014398509481984e16;  /* 2^54 */
     y *= 1.8014398509481984e16;
     scale = 7.450580596923828125e-9; /* 2^-27 */
#else
     x *= 4.0;
     y *= 4.0;
     scale = 0.5;
#endif
   }
  w = x + y * I;
  r = cabs(w);
  if( x > 0 )
    {
      t = sqrt( 0.5 * r + 0.5 * x );
      r = scale * fabs( (0.5 * y) / t );
      t *= scale;
    }
  else
    {
      r = sqrt( 0.5 * r - 0.5 * x );
      t = scale * fabs( (0.5 * y) / r );
      r *= scale;
    }
  if (y < 0)
    w = t - r * I;
  else
    w = t + r * I;
  return (w);
}


double
hypot( x, y )
     double x, y;
{
  double complex z;

  z = x + y * I;
return (cabs(z));
}
