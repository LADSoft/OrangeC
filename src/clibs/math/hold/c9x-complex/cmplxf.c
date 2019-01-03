/*							cmplxf.c
 *
 *	Complex number arithmetic
 *
 *
 *
 * SYNOPSIS:
 *
 * typedef struct {
 *      float r;     real part
 *      float i;     imaginary part
 *     }cmplxf;
 *
 * cmplxf *a, *b, *c;
 *
 * caddf( a, b, c );     c = b + a
 * csubf( a, b, c );     c = b - a
 * cmulf( a, b, c );     c = b * a
 * cdivf( a, b, c );     c = b / a
 * cnegf( c );           c = -c
 * cmovf( b, c );        c = b
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
 *    IEEE       cadd       30000       5.9e-8      2.6e-8
 *    IEEE       csub       30000       6.0e-8      2.6e-8
 *    IEEE       cmul       30000       1.1e-7      3.7e-8
 *    IEEE       cdiv       30000       2.1e-7      5.7e-8
 */
/*				cmplx.c
 * complex number arithmetic
 */


/*
Cephes Math Library Release 2.1:  December, 1988
Copyright 1984, 1987, 1988 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/


#include "complex.h"
#include "mconf.h"
extern float MAXNUMF, MACHEPF, PIF, PIO2F;
#define fabsf(x) ( (x) < 0 ? -(x) : (x) )
#ifdef ANSIC
float sqrtf(float), frexpf(float, int *);
float ldexpf(float, int);
float cabsf(float complex);
float atan2f(float, float);
float cosf(float);
float sinf(float);
#else
float sqrtf(), frexpf(), ldexpf();
float cabsf(), atan2f(), cosf(), sinf();
#endif
/*
typedef struct
	{
	float r;
	float i;
	}cmplxf;
*/
float complex czerof = 0.0f + 0.0f * I;
float complex conef = 1.0f + 0.0f * I;

/*	c = b + a	*/

float complex
caddf( float complex a, float complex b )
{
  return (creal (b) + creal (a) + (cimag (b) + cimag (a)) * I);
}


/*	c = b - a	*/

float complex
csubf( float complex a, float complex b )
{
  return (creal (b) - creal (a) + (cimag (b) - cimag (a)) * I);
}

/*	c = b * a */

float complex
cmulf( float complex a, float complex b )
{
  return ((creal (b) * creal (a) - cimag (b) * cimag (a))
	  + (creal (b) * cimag (a) + cimag (b) * creal (a)) * I);
}



/*	c = b / a */

float complex
cdivf( float complex a, float complex b )
{
float y, p, q, w;

  y = creal (a) * creal (a) + cimag (a) * cimag (a);
  p = creal (b) * creal (a) + cimag (b) * cimag (a);
  q = cimag (b) * creal (a) - creal (b) * cimag (a);

  if( y < 1.0f )
	{
	w = MAXNUMF * y;
	if( (fabsf(p) > w) || (fabsf(q) > w) || (y == 0.0f) )
		{
		  mtherr( "cdivf", OVERFLOW );
		  return (MAXNUMF + MAXNUMF * I);
		}
	}
  return (p/y + (q/y) * I);
}

/*							cabsf()
 *
 *	Complex absolute value
 *
 *
 *
 * SYNOPSIS:
 *
 * float cabsf();
 * cmplxf z;
 * float a;
 *
 * a = cabsf( &z );
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
 *    IEEE      -10,+10     30000       1.2e-7      3.4e-8
 */


/*
Cephes Math Library Release 2.1:  January, 1989
Copyright 1984, 1987, 1989 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/


/*
typedef struct
	{
	float r;
	float i;
	}cmplxf;
*/
/* square root of max and min numbers */
#define SMAX  1.3043817825332782216E+19
#define SMIN  7.6664670834168704053E-20
#define PREC 12
#define MAXEXPF 128


#define SMAXT (2.0 * SMAX)
#define SMINT (0.5 * SMIN)

float
cabsf( float complex z )
{
float x, y, b, re, im;
int ex, ey, e;

re = fabsf( (float) creal(z) );
im = fabsf( (float) cimag(z) );

if( re == 0.0f )
  return( im );
if( im == 0.0 )
  return( re );

/* Get the exponents of the numbers */
x = frexpf( re, &ex );
y = frexpf( im, &ey );

/* Check if one number is tiny compared to the other */
e = ex - ey;
if( e > PREC )
	return( re );
if( e < -PREC )
	return( im );

/* Find approximate exponent e of the geometric mean. */
e = (ex + ey) >> 1;

/* Rescale so mean is about 1 */
x = ldexpf( re, -e );
y = ldexpf( im, -e );
		
/* Hypotenuse of the right triangle */
b = sqrtf( x * x  +  y * y );

/* Compute the exponent of the answer. */
y = frexpf( b, &ey );
ey = e + ey;

/* Check it for overflow and underflow. */
if( ey > MAXEXPF )
	{
	mtherr( "cabsf", OVERFLOW );
	return( MAXNUMF );
	}
if( ey < -MAXEXPF )
	return(0.0);

/* Undo the scaling */
b = ldexpf( b, e );
return( b );
}
/*							csqrtf()
 *
 *	Complex square root
 *
 *
 *
 * SYNOPSIS:
 *
 * float complex csqrtf();
 * float complex z, w;
 *
 * w = csqrtf( z );
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
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10    1,000,000    1.8e-7       3.5e-8
 *
 */


float complex
csqrtf( float complex z )
{
float complex w;
float x, y, r, t, scale;

x = creal(z);
y = cimag(z);

if( y == 0.0f )
	{
	if( x < 0.0f )
	  {
	    w = 0.0f + sqrtf(-x) * I;
	    return (w);
	  }
	else if (x == 0.0f)
	  {
	    return (0.0f + y * I);
	  }
	else
	  {
	    w = sqrtf(x) + y * I;
	    return (w);
	  }
	}

if( x == 0.0f )
	{
	r = fabsf(y);
	r = sqrtf(0.5f*r);
	if( y > 0 )
	  w = r + r * I;
	else
	  w = r - r * I;
	return (w);
	}

 /* Rescale to avoid internal overflow or underflow.  */
 if ((fabsf(x) > 4.0f) || (fabsf(y) > 4.0f))
   {
     x *= 0.25f;
     y *= 0.25f;
     scale = 2.0f;
   }
 else
   {
#if 1
     x *= 6.7108864e7f; /* 2^26 */
     y *= 6.7108864e7f;
     scale = 1.220703125e-4f; /* 2^-13 */
#else
     x *= 4.0f;
     y *= 4.0f;
     scale = 0.5f;
#endif
   }
  w = x + y * I;
  r = cabsf(w);
  if( x > 0 )
    {
      t = sqrtf( 0.5f * r + 0.5f * x );
      r = scale * fabsf( (0.5f * y) / t );
      t *= scale;
    }
  else
    {
      r = sqrtf( 0.5f * r - 0.5f * x );
      t = scale * fabsf( (0.5f * y) / r );
      r *= scale;
    }

  if (y < 0)
    w = t - r * I;
  else
    w = t + r * I;
  return (w);
}

