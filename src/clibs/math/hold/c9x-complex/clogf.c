/*							clogf.c
 *
 *	Complex natural logarithm
 *
 *
 *
 * SYNOPSIS:
 *
 * void clogf();
 * cmplxf z, w;
 *
 * clogf( &z, &w );
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
 *    IEEE      -10,+10     30000       1.9e-6       6.2e-8
 *
 * Larger relative error can be observed for z near 1 +i0.
 * In IEEE arithmetic the peak absolute error is 3.1e-7.
 *
 */

#include "complex.h"
#include "mconf.h"
extern float MAXNUMF, MACHEPF, PIF, PIO2F;
#ifdef ANSIC
float cabsf(float complex);
float sqrtf(float), logf(float);
float atan2f(float, float);
float expf(float), sinf(float), cosf(float);
float coshf(float), sinhf(float), asinf(float);
float ctansf(float complex);
float redupif(float);
static void cchshf( float, float *, float * );
/* void caddf( cmplxf *, cmplxf *, cmplxf * ); */
float complex csqrtf( float complex );
float powf (float, float);
#else
float cabsf(), sqrtf(), logf(), atan2f();
float expf(), sinf(), cosf();
float coshf(), sinhf(), asinf();
float ctansf(), redupif();
static void cchshf()
float complex csqrtf();
float powf();
/*  caddf(); */
#endif

#define fabsf(x) ( (x) < 0 ? -(x) : (x) )

float complex
clogf( float complex z )
{
float complex w;
float p, rr, x, y;

x = creal(z);
y = cimag(z);
rr = atan2f( y, x );
p = cabsf(z);
p = logf(p);
w = p + rr * I;
return (w);
}
/*							cexpf()
 *
 *	Complex exponential function
 *
 *
 *
 * SYNOPSIS:
 *
 * void cexpf();
 * cmplxf z, w;
 *
 * cexpf( &z, &w );
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
 *    IEEE      -10,+10     30000       1.4e-7      4.5e-8
 *
 */

float complex
cexpf( float complex z )
{
float complex w;
float r;

r = expf( creal(z) );
w = r * cosf( cimag(z) ) +  r * sinf( cimag(z) ) * I;
return (w);
}
/*							csinf()
 *
 *	Complex circular sine
 *
 *
 *
 * SYNOPSIS:
 *
 * void csinf();
 * cmplxf z, w;
 *
 * csinf( &z, &w );
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
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       1.9e-7      5.5e-8
 *
 */

float complex
csinf( float complex z )
{
float complex w;
float ch, sh;

cchshf( (float) cimag(z), &ch, &sh );
w = sinf( creal(z) ) * ch  + (cosf( creal(z) ) * sh) * I;
return (w);
}



/* calculate cosh and sinh */

#ifdef ANSIC
static void
cchshf( float xx, float *c, float *s )
#else
void cchshf( xx, c, s )
double xx;
float *c, *s;
#endif
{
float x, e, ei;

x = xx;
if( fabsf(x) <= 0.5f )
	{
	*c = coshf(x);
	*s = sinhf(x);
	}
else
	{
	e = expf(x);
	ei = 0.5f/e;
	e = 0.5f * e;
	*s = e - ei;
	*c = e + ei;
	}
}

/*							ccosf()
 *
 *	Complex circular cosine
 *
 *
 *
 * SYNOPSIS:
 *
 * void ccosf();
 * cmplxf z, w;
 *
 * ccosf( &z, &w );
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
 *    IEEE      -10,+10     30000       1.8e-7       5.5e-8
 */

float complex
ccosf( float complex z )
{
float complex w;
float ch, sh;

cchshf( cimag(z), &ch, &sh );
w = cosf( creal(z) ) * ch + ( -sinf( creal(z) ) * sh) * I;
return (w);
}
/*							ctanf()
 *
 *	Complex circular tangent
 *
 *
 *
 * SYNOPSIS:
 *
 * void ctanf();
 * cmplxf z, w;
 *
 * ctanf( &z, &w );
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
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       3.3e-7       5.1e-8
 */

float complex
ctanf( float complex z )
{
float complex w;
float d;

d = cosf( 2.0f * creal(z) ) + coshf( 2.0f * cimag(z) );

if( fabsf(d) < 0.25f )
	d = ctansf(z);

if( d == 0.0f )
	{
	mtherr( "ctanf", OVERFLOW );
	w = MAXNUMF + MAXNUMF * I;
	return (w);
	}
w = sinf (2.0f * creal(z)) / d + (sinhf (2.0f * cimag(z)) / d) * I;
return (w);
}
/*							ccotf()
 *
 *	Complex circular cotangent
 *
 *
 *
 * SYNOPSIS:
 *
 * void ccotf();
 * cmplxf z, w;
 *
 * ccotf( &z, &w );
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
 *    IEEE      -10,+10     30000       3.6e-7       5.7e-8
 * Also tested by ctan * ccot = 1 + i0.
 */

float complex
ccotf( float complex z )
{
float complex w;
float d;


d = coshf(2.0f * cimag(z)) - cosf(2.0f * creal(z));

if( fabsf(d) < 0.25f )
	d = ctansf(z);

if( d == 0.0f )
	{
	mtherr( "ccotf", OVERFLOW );
	w = MAXNUMF + MAXNUMF * I;
	return (w);
	}
w = sinf (2.0f * creal(z)) / d - (sinhf (2.0f * cimag(z)) / d) * I;
return (w);
}

/* Program to subtract nearest integer multiple of PI */
/* extended precision value of PI: */

static double DP1 =  3.140625;
static double DP2 =  9.67502593994140625E-4;
static double DP3 =  1.509957990978376432E-7;


#ifdef ANSIC
float redupif(float xx)
#else
float redupif(xx)
double xx;
#endif
{
float x, t;
long i;

x = xx;
t = x/PIF;
if( t >= 0.0 )
	t += 0.5;
else
	t -= 0.5;

i = t;	/* the multiple */
t = i;
t = ((x - t * DP1) - t * DP2) - t * DP3;
return(t);
}

/*  Taylor series expansion for cosh(2y) - cos(2x)	*/

float
ctansf(float complex z)
{
float f, x, x2, y, y2, rn, t, d;

x = fabsf( 2.0f * creal(z) );
y = fabsf( 2.0f * cimag(z) );

x = redupif(x);

x = x * x;
y = y * y;
x2 = 1.0f;
y2 = 1.0f;
f = 1.0f;
rn = 0.0f;
d = 0.0f;
do
	{
	rn += 1.0f;
	f *= rn;
	rn += 1.0f;
	f *= rn;
	x2 *= x;
	y2 *= y;
	t = y2 + x2;
	t /= f;
	d += t;

	rn += 1.0f;
	f *= rn;
	rn += 1.0f;
	f *= rn;
	x2 *= x;
	y2 *= y;
	t = y2 - x2;
	t /= f;
	d += t;
	}
while( fabsf(t/d) > MACHEPF );
return(d);
}
/*							casinf()
 *
 *	Complex circular arc sine
 *
 *
 *
 * SYNOPSIS:
 *
 * void casinf();
 * cmplxf z, w;
 *
 * casinf( &z, &w );
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
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       1.1e-5      1.5e-6
 * Larger relative error can be observed for z near zero.
 *
 */

float complex
casinf( float complex z )
{
float complex w;
float x, y;
static float complex ca, ct, zz, z2;
/*
float cn, n;
static float a, b, s, t, u, v, y2;
static cmplxf sum;
*/

x = creal(z);
y = cimag(z);

if( y == 0.0f )
	{
	if( fabsf(x) > 1.0f )
		{
		w = PIO2F + 0.0f * I;
		mtherr( "casinf", DOMAIN );
		}
	else
		{
		w = asinf (x) + 0.0f * I;
		}
	return (w);
	}

/* Power series expansion */
/*
b = cabsf(z);
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
	b = fabsf(ct.r) + fabsf(ct.i);
	}
while( b > MACHEPF );
w->r = sum.r;
w->i = sum.i;
return;
}
*/


  ca = x + y * I;
  ct = ca * I;	/* iz */
	/* sqrt( 1 - z*z) */
/* cmul( &ca, &ca, &zz ) */
	/*x * x  -  y * y */
  zz = (x - y) * (x + y) + (2.0f * x * y) * I;
  zz = 1.0f - creal(zz) - cimag(zz) * I;
  z2 = csqrtf (zz);

  zz = ct + z2;
  zz = clogf (zz);
  /* multiply by 1/i = -i */
  w = zz * (-1.0f * I);
  return (w);
}
/*							cacosf()
 *
 *	Complex circular arc cosine
 *
 *
 *
 * SYNOPSIS:
 *
 * void cacosf();
 * cmplxf z, w;
 *
 * cacosf( &z, &w );
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
 *    IEEE      -10,+10     30000       9.2e-6       1.2e-6
 *
 */

float complex
cacosf( float complex z )
{
float complex w;

w = casinf( z );
w = (PIO2F  -  creal (w)) - cimag (w) * I;
return (w);
}
/*							catanf()
 *
 *	Complex circular arc tangent
 *
 *
 *
 * SYNOPSIS:
 *
 * float complex catanf();
 * float complex z, w;
 *
 * w = catanf( z );
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
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000        2.3e-6      5.2e-8
 *
 */

float complex
catanf( float complex z )
{
float complex w;
float a, t, x, x2, y;

x = creal (z);
y = cimag (z);

if( (x == 0.0f) && (y > 1.0f) )
	goto ovrf;

x2 = x * x;
a = 1.0f - x2 - (y * y);
if( a == 0.0f )
	goto ovrf;

t = 0.5f * atan2f( 2.0f * x, a );
w = redupif( t );

t = y - 1.0f;
a = x2 + (t * t);
if( a == 0.0f )
	goto ovrf;

t = y + 1.0f;
a = (x2 + (t * t))/a;
w = w + (0.25f * logf (a)) * I;
return (w);

ovrf:
mtherr( "catanf", OVERFLOW );
w = MAXNUMF + MAXNUMF * I;
return (w);
}


/*							csinhf
 *
 *	Complex hyperbolic sine
 *
 *
 *
 * SYNOPSIS:
 *
 * float complex csinhf();
 * float complex z, w;
 *
 * w = csinhf (z);
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

float complex
csinhf (float complex z)
{
  float complex w;
  float x, y;

  x = creal(z);
  y = cimag(z);
  w = sinhf (x) * cosf (y)  +  (coshf (x) * sinf (y)) * I;
  return (w);
}


/*							casinhf
 *
 *	Complex inverse hyperbolic sine
 *
 *
 *
 * SYNOPSIS:
 *
 * float complex casinhf();
 * float complex z, w;
 *
 * w = casinhf (z);
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

float complex
casinhf (float complex z)
{
  float complex w;

  w = -1.0f * I * casinf (z * I);
  return (w);
}


/*							ccoshf
 *
 *	Complex hyperbolic cosine
 *
 *
 *
 * SYNOPSIS:
 *
 * float complex ccoshf();
 * float complex z, w;
 *
 * w = ccoshf (z);
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

float complex
ccoshf (float complex z)
{
  float complex w;
  float x, y;

  x = creal(z);
  y = cimag(z);
  w = coshf (x) * cosf (y)  +  (sinhf (x) * sinf (y)) * I;
  return (w);
}



/*							cacoshf
 *
 *	Complex inverse hyperbolic cosine
 *
 *
 *
 * SYNOPSIS:
 *
 * float complex cacoshf();
 * float complex z, w;
 *
 * w = cacoshf (z);
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

float complex
cacoshf (float complex z)
{
  float complex w;

  w = I * cacosf (z);
  return (w);
}


/*							ctanhf
 *
 *	Complex hyperbolic tangent
 *
 *
 *
 * SYNOPSIS:
 *
 * float complex ctanhf();
 * float complex z, w;
 *
 * w = ctanhf (z);
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

float complex
ctanhf (float complex z)
{
  float complex w;
  float x, y, d;

  x = creal(z);
  y = cimag(z);
  d = coshf (2.0f * x) + cosf (2.0f * y);
  w = sinhf (2.0f * x) / d  +  (sinf (2.0f * y) / d) * I;
  return (w);
}


/*							catanhf
 *
 *	Complex inverse hyperbolic tangent
 *
 *
 *
 * SYNOPSIS:
 *
 * float complex catanhf();
 * float complex z, w;
 *
 * w = catanhf (z);
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

float complex
catanhf (float complex z)
{
  float complex w;

  w = -1.0f * I * catanf (z * I);
  return (w);
}

/*							cpowf
 *
 *	Complex power function
 *
 *
 *
 * SYNOPSIS:
 *
 * float complex cpowf();
 * float complex a, z, w;
 *
 * w = cpowf (a, z);
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

float complex
cpowf (float complex a, float complex z)
{
  float complex w;
  float x, y, r, theta, absa, arga;

  x = creal (z);
  y = cimag (z);
  absa = cabsf (a);
  if (absa == 0.0f)
    {
      return (0.0f + 0.0f * I);
    }
  arga = cargf (a);
  r = powf (absa, x);
  theta = x * arga;
  if (y != 0.0f)
    {
      r = r * expf (-y * arga);
      theta = theta + y * logf (absa);
    }
  w = r * cosf (theta) + (r * sinf (theta)) * I;
  return (w);
}
