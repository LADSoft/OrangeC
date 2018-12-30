/*							cmplxl.c
 *
 *	Complex number arithmetic
 *
 *
 *
 * SYNOPSIS:
 *
 * typedef struct {
 *      long double r;     real part
 *      long double i;     imaginary part
 *     }cmplxl;
 *
 * cmplxl *a, *b, *c;
 *
 * caddl( a, b, c );     c = b + a
 * csubl( a, b, c );     c = b - a
 * cmull( a, b, c );     c = b * a
 * cdivl( a, b, c );     c = b / a
 * cnegl( c );           c = -c
 * cmovl( b, c );        c = b
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
 *    IEEE       caddl     100000       5.4e-20     2.4e-20
 *    IEEE       csubl     100000       5.4e-20     2.3e-20
 *    IEEE       cmull     100000       1.1e-19     3.3e-20
 *    IEEE       cdivl     100000       2.1e-19     5.2e-20
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
long double fabsl(), cabsl(), sqrtl(), atan2l(), cosl(), sinl();
long double sqrtl(), frexpl(), ldexpl();
int isnanl();
#else
long double fabsl (long double);
long double sqrtl (long double);
long double frexpl (long double, int *);
long double ldexpl (long double, int);
long double cabsl (long double complex);
int isnanl (long double);
#endif


extern long double MAXNUML, MACHEPL, PIL, PIO2L, NANL;
#define INFINITYL MAXNUML

/*
typedef struct
	{
	long double r;
	long double i;
	}cmplxl;
*/
long double complex czerol = 0.0L + 0.0L * I;
long double complex conel = 1.0L + 0.0L * I;


/*	c = b + a	*/

long double complex
caddl( long double complex a, long double complex b )
{
  return (creal (b) + creal (a) + (cimag (b) + cimag (a)) * I);
}


/*	c = b - a	*/

long double complex
csubl( long double complex a, long double complex b )
{
  return (creal (b) - creal (a) + (cimag (b) - cimag (a)) * I);
}

/*	c = b * a */

long double complex
cmull( long double complex a, long double complex b )
{
  return ((creal (b) * creal (a) - cimag (b) * cimag (a))
	  + (creal (b) * cimag (a) + cimag (b) * creal (a)) * I);
}



/*	c = b / a */

long double complex
cdivl( long double complex a, long double complex b )
{
long double y, p, q, w;


  y = creal (a) * creal (a) + cimag (a) * cimag (a);
  p = creal (b) * creal (a) + cimag (b) * cimag (a);
  q = cimag (b) * creal (a) - creal (b) * cimag (a);

if( y < 1.0L )
	{
	w = MAXNUML * y;
	if( (fabsl(p) > w) || (fabsl(q) > w) || (y == 0.0L) )
		{
		mtherr( "cdivl", OVERFLOW );
		return (INFINITYL + INFINITYL * I);
		}
	}
return (p/y + (q/y) * I);
}

/*							cabsl()
 *
 *	Complex absolute value
 *
 *
 *
 * SYNOPSIS:
 *
 * long double cabsl();
 * long double complex cmplxl z;
 * long double a;
 *
 * a = cabsl( z );
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
 *       a = sqrt( x**2 + y**2 ).
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
 *    IEEE      -10,+10    100000       1.0e-19     2.8e-20
 */


/*
Cephes Math Library Release 2.1:  January, 1989
Copyright 1984, 1987, 1989 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/


/*
typedef struct
	{
	long double r;
	long double i;
	}cmplxl;
*/

#ifdef UNK
#define PRECL 32
#define MAXEXPL 16384
#define MINEXPL -16384
#endif
#ifdef IBMPC
#define PRECL 32
#define MAXEXPL 16384
#define MINEXPL -16384
#endif
#ifdef MIEEE
#define PRECL 32
#define MAXEXPL 16384
#define MINEXPL -16384
#endif


long double
cabsl( long double complex z )
{
long double x, y, b, re, im;
int ex, ey, e;

x = creal(z);
y = cimag(z);

#ifdef INFINITIES
/* Note, cabs(INFINITY,NAN) = INFINITY. */
if( x == INFINITYL || y == INFINITYL
   || x == -INFINITYL || y == -INFINITYL )
  return( INFINITYL );
#endif

#ifdef NANS
if( isnanl(x) )
  return(x);
if( isnanl(y) )
  return(y);
#endif

re = fabsl( x );
im = fabsl( y );

if( re == 0.0L )
	return( im );
if( im == 0.0L )
	return( re );

/* Get the exponents of the numbers */
x = frexpl( re, &ex );
y = frexpl( im, &ey );

/* Check if one number is tiny compared to the other */
e = ex - ey;
if( e > PRECL )
	return( re );
if( e < -PRECL )
	return( im );

/* Find approximate exponent e of the geometric mean. */
e = (ex + ey) >> 1;

/* Rescale so mean is about 1 */
x = ldexpl( re, -e );
y = ldexpl( im, -e );
		
/* Hypotenuse of the right triangle */
b = sqrtl( x * x  +  y * y );

/* Compute the exponent of the answer. */
y = frexpl( b, &ey );
ey = e + ey;

/* Check it for overflow and underflow. */
if( ey > MAXEXPL )
	{
	mtherr( "cabsl", OVERFLOW );
	return( INFINITYL );
	}
if( ey < MINEXPL )
	return(0.0L);

/* Undo the scaling */
b = ldexpl( b, e );
return( b );
}
/*							csqrtl()
 *
 *	Complex square root
 *
 *
 *
 * SYNOPSIS:
 *
 * long double complex csqrtl();
 * long double complex z, w;
 *
 * w = csqrtl( z );
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
 *    IEEE      -10,+10     500000      1.1e-19     3.0e-20
 *
 */


long double complex
csqrtl( long double complex z )
{
long double complex w;
long double x, y, r, t, scale;

x = creal(z);
y = cimag(z);

if( y == 0.0L )
	{
	if( x < 0.0L )
		{
		w = 0.0L + sqrtl(-x) * I;
		return (w);
		}
	else
		{
		w = sqrtl(x) + 0.0L * I;
		return (w);
		}
	}


if( x == 0.0L )
	{
	r = fabsl(y);
	r = sqrtl(0.5L*r);
	if( y > 0.0L )
	  w = r + r * I;
	else
	  w = r - r * I;
	return (w);
	}
/* Rescale to avoid internal overflow or underflow.  */
if ((fabsl(x) > 4.0L) || (fabsl(y) > 4.0L))
  {
    x *= 0.25L;
    y *= 0.25L;
    scale = 2.0L;
  }
else
  {
#if 1
    x *= 7.3786976294838206464e19;  /* 2^66 */
    y *= 7.3786976294838206464e19;
    scale = 1.16415321826934814453125e-10;  /* 2^-33 */
#else
    x *= 4.0L;
    y *= 4.0L;
    scale = 0.5L;
#endif
  }
 w = x + y * I;
 r = cabsl(w);
if( x > 0 )
	{
	t = sqrtl( 0.5L * r + 0.5L * x );
	r = scale * fabsl( (0.5L * y) / t );
	t *= scale;
	}
else
	{
	r = sqrtl( 0.5L * r - 0.5L * x );
	t = scale * fabsl( (0.5L * y) / r );
	r *= scale;
	}
if (y < 0)
	w = t - r * I;
else
	w = t + r * I;
  return (w);
}


long double hypotl( x, y )
long double x, y;
{
long double complex z;

z = x + y * I;
return( cabsl(z) );
}
