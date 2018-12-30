/*							clogl.c
 *
 *	Complex natural logarithm
 *
 *
 *
 * SYNOPSIS:
 *
 * long double complex clogl();
 * long double complex z, w;
 *
 * w = clogl( z );
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
static void cchshl ( long double x, long double *c, long double *s );
static long double redupil ( long double x );
static long double ctansl ( long double complex z );
long double fabsl (long double);
long double sqrtl (long double);
long double logl (long double);
long double expl (long double);
long double atan2l (long double, long double);
long double sinhl (long double);
long double coshl (long double);
long double sinl (long double);
long double cosl (long double);
long double asinl (long double);
long double powl (long double, long double);
long double cabsl (long double complex);
long double complex csqrtl (long double complex);
#else
static void cchshl();
static long double redupil();
static long double ctansl();
long double cabsl(), fabsl(), sqrtl();
lnog double logl(), expl(), atan2l(), coshl(), sinhl();
long double asinl(), sinl(), cosl();
long double complex csqrtl ();
long double powl();
#endif

extern long double MAXNUML, MACHEPL, PIL, PIO2L;

long double complex
clogl( long double complex z )
{
long double complex w;
long double p, rr;

/*rr = sqrt( z->r * z->r  +  z->i * z->i );*/
p = cabsl(z);
p = logl(p);
rr = atan2l( cimag(z), creal(z) );
w = p + rr * I;
return (w);
}
/*							cexpl()
 *
 *	Complex exponential function
 *
 *
 *
 * SYNOPSIS:
 *
 * long double complex cexpl();
 * long double complex z, w;
 *
 * w = cexpl( z );
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

long double complex
cexpl( long double complex z )
{
long double complex w;
long double r;

r = expl( creal(z) );
w = r * cosl( (long double) cimag(z) ) +  (r * sinl( (long double) cimag(z) )) * I;
return (w);
}
/*							csinl()
 *
 *	Complex circular sine
 *
 *
 *
 * SYNOPSIS:
 *
 * long double complex csinl();
 * long double complex z, w;
 *
 * w = csinl( z );
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
 *    DEC       -10,+10      8400       5.3e-17     1.3e-17
 *    IEEE      -10,+10     30000       3.8e-16     1.0e-16
 * Also tested by csin(casin(z)) = z.
 *
 */

long double complex
csinl( long double complex z )
{
long double complex w;
long double ch, sh;

cchshl( cimag(z), &ch, &sh );
w = sinl( creal(z) ) * ch + (cosl( creal(z) ) * sh) * I;
return (w);
}



/* calculate cosh and sinh */

static void
cchshl( long double x, long double *c, long double *s )
{
long double e, ei;

if( fabsl(x) <= 0.5L )
	{
	*c = coshl(x);
	*s = sinhl(x);
	}
else
	{
	e = expl(x);
	ei = 0.5L/e;
	e = 0.5L * e;
	*s = e - ei;
	*c = e + ei;
	}
}

/*							ccosl()
 *
 *	Complex circular cosine
 *
 *
 *
 * SYNOPSIS:
 *
 * long double complex ccosl();
 * long double complex z, w;
 *
 * w = ccosl( z );
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

long double complex
ccosl( long double complex z )
{
long double complex w;
long double ch, sh;

cchshl( cimag(z), &ch, &sh );
w = cosl( creal(z) ) * ch + (-sinl( creal(z) ) * sh) * I;
return (w);
}
/*							ctanl()
 *
 *	Complex circular tangent
 *
 *
 *
 * SYNOPSIS:
 *
 * long double complex ctanl();
 * long double complex z, w;
 *
 * w = ctanl( z );
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
 *    DEC       -10,+10      5200       7.1e-17     1.6e-17
 *    IEEE      -10,+10     30000       7.2e-16     1.2e-16
 * Also tested by ctan * ccot = 1 and catan(ctan(z))  =  z.
 */

long double complex
ctanl( long double complex z )
{
long double complex w;
long double d, x, y;

x = creal(z);
y = cimag(z);
d = cosl( 2.0L * x ) + coshl( 2.0L * y );

if( fabsl(d) < 0.25L )
  {
    d = fabsl(d);
	d = ctansl(z);
  }
if( d == 0.0L )
	{
	mtherr( "ctan", OVERFLOW );
	w = MAXNUML +  MAXNUML * I;
	return (w);
	}

w = sinl( 2.0L * x ) / d + (sinhl( 2.0L * y ) / d) * I;
return (w);
}
/*							ccotl()
 *
 *	Complex circular cotangent
 *
 *
 *
 * SYNOPSIS:
 *
 * long double complex ccotl();
 * long double complex z, w;
 *
 * w = ccotl( z );
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

long double complex
ccotl( long double complex z )
{
long double complex w;
long double d;

d = coshl(2.0L * cimag(z)) - cosl(2.0L * creal(z));

if( fabsl(d) < 0.25L )
	d = ctansl(z);

if( d == 0.0L )
	{
	mtherr( "ccot", OVERFLOW );
	w = MAXNUML + MAXNUML * I;
	return (w);
	}

w = sinl( 2.0L * creal(z) ) / d + (-sinhl( 2.0L * cimag(z) ) / d) * I;
return (w);
}

/* Program to subtract nearest integer multiple of PI */
/* extended precision value of PI: */
#ifdef UNK
static long double DP1 = 3.14159265358979323829596852490908531763125L;
static long double DP2 = 1.6667485837041756656403424829301998703007e-19L;
static long double DP3 = 1.8830410776607851167459095484560349402753e-39L;
#endif

#ifdef DEC
DEC not supported
#endif

#ifdef IBMPC
static unsigned short P1[] = {0xc234,0x2168,0xdaa2,0xc90f,0x4000,XPD};
static unsigned short P2[] = {0x1cd1,0x80dc,0x628b,0xc4c6,0x3fc0,XPD};
static unsigned short P3[] = {0x31d0,0x299f,0x3822,0xa409,0x3f7e,XPD};
#define DP1 *(long double *)P1
#define DP2 *(long double *)P2
#define DP3 *(long double *)P3
#endif

#ifdef MIEEE
static unsigned short P1[] = {
0x4000,0x0000,0xc90f,0xdaa2,0x2168,0xc234
};
static unsigned short P2[] = {
0x3fc0,0x0000,0xc4c60,0x628b,0x80dc,0x1cd1
};
static unsigned short P3[] = {
0x3f7e,0x0000,0xa409,0x3822,0x299f,0x31d0
};
#define DP1 *(long double *)P1
#define DP2 *(long double *)P2
#define DP3 *(long double *)P3
#endif

static long double
redupil(x)
     long double x;
{
long double t;
long i;

t = x/PIL;
if( t >= 0.0L )
	t += 0.5L;
else
	t -= 0.5L;

i = t;	/* the multiple */
t = i;
t = ((x - t * DP1) - t * DP2) - t * DP3;
return(t);
}

/*  Taylor series expansion for cosh(2y) - cos(2x)	*/

static long double
ctansl(long double complex z)
{
long double f, x, x2, y, y2, rn, t;
long double d;

x = fabsl( 2.0L * creal(z) );
y = fabsl( 2.0L * cimag(z) );

x = redupil(x);

x = x * x;
y = y * y;
x2 = 1.0L;
y2 = 1.0L;
f = 1.0L;
rn = 0.0L;
d = 0.0L;
do
	{
	rn += 1.0L;
	f *= rn;
	rn += 1.0L;
	f *= rn;
	x2 *= x;
	y2 *= y;
	t = y2 + x2;
	t /= f;
	d += t;

	rn += 1.0L;
	f *= rn;
	rn += 1.0L;
	f *= rn;
	x2 *= x;
	y2 *= y;
	t = y2 - x2;
	t /= f;
	d += t;
	}
while( fabsl(t/d) > MACHEPL );
return(d);
}
/*							casinl()
 *
 *	Complex circular arc sine
 *
 *
 *
 * SYNOPSIS:
 *
 * long double complex casinl();
 * long double complex z, w;
 *
 * w = casinl( z );
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
 *    DEC       -10,+10     10100       2.1e-15     3.4e-16
 *    IEEE      -10,+10     30000       2.2e-14     2.7e-15
 * Larger relative error can be observed for z near zero.
 * Also tested by csin(casin(z)) = z.
 */

long double complex
casinl( long double complex z )
{
long double complex w;
long double x, y, b;
static long double complex ca, ct, zz, z2;

x = creal(z);
y = cimag(z);

if( y == 0.0L )
	{
	if( fabsl(x) > 1.0L )
		{
		w = PIO2L + 0.0L * I;
		mtherr( "casinl", DOMAIN );
		}
	else
		{
		w = asinl(x) + 0.0L * I;
		}
	return (w);
	}

/* Power series expansion */

b = cabsl(z);
if( b < 0.125L )
{
 long double complex sum;
 long double n, cn;

z2 = (x - y) * (x + y) + (2.0L * x * y) * I;
cn = 1.0L;
n = 1.0L;
ca = x + y * I;
sum = x + y * I;
do
	{
	  ct = z2 * ca;
	  ca = ct;

	cn *= n;
	n += 1.0L;
	cn /= n;
	n += 1.0L;
	b = cn/n;

	ct *= b;
	sum += ct;
	b = cabsl(ct);
	}
while( b > MACHEPL );
w = sum;
return w;
}



  ca = x + y * I;
  ct = ca * I;	/* iz */
	/* sqrt( 1 - z*z) */
/* cmul( &ca, &ca, &zz ) */
	/*x * x  -  y * y */
  zz = (x - y) * (x + y) + (2.0L * x * y) * I;
  zz = 1.0L - creal(zz) - cimag(zz) * I;
  z2 = csqrtl (zz);

  zz = ct + z2;
  zz = clogl (zz);
  /* multiply by 1/i = -i */
  w = zz * (-1.0L * I);
  return (w);
}
/*							cacosl()
 *
 *	Complex circular arc cosine
 *
 *
 *
 * SYNOPSIS:
 *
 * long double complex cacosl();
 * long double complex z, w;
 *
 * w = cacosl( z );
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

long double complex
cacosl( long double complex z )
{
long double complex w;

w = casinl( z );
w = (PIO2L  -  creal(w)) - cimag(w) * I;
return (w);
}
/*							catanl()
 *
 *	Complex circular arc tangent
 *
 *
 *
 * SYNOPSIS:
 *
 * long double complex catanl();
 * long double complex z, w;
 *
 * w = catanl( z );
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
 *    DEC       -10,+10      5900       1.3e-16     7.8e-18
 *    IEEE      -10,+10     30000       2.3e-15     8.5e-17
 * The check catan( ctan(z) )  =  z, with |x| and |y| < PI/2,
 * had peak relative error 1.5e-16, rms relative error
 * 2.9e-17.  See also clog().
 */

long double complex
catanl( long double complex z )
{
long double complex w;
long double a, t, x, x2, y;

x = creal(z);
y = cimag(z);

if( (x == 0.0L) && (y > 1.0L) )
	goto ovrf;

x2 = x * x;
a = 1.0L - x2 - (y * y);
if( a == 0.0L )
	goto ovrf;

t = atan2l( 2.0L * x, a ) * 0.5L;
w = redupil( t );

t = y - 1.0L;
a = x2 + (t * t);
if( a == 0.0L )
	goto ovrf;

t = y + 1.0L;
a = (x2 + (t * t))/a;
w = w + (0.25L * logl(a)) * I;
return (w);

ovrf:
mtherr( "catanl", OVERFLOW );
w = MAXNUML + MAXNUML * I;
return (w);
}


/*							csinhl
 *
 *	Complex hyperbolic sine
 *
 *
 *
 * SYNOPSIS:
 *
 * long double complex csinhl();
 * long double complex z, w;
 *
 * w = csinhl (z);
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

long double complex
csinhl (long double complex z)
{
  long double complex w;
  long double x, y;

  x = creal(z);
  y = cimag(z);
  w = sinhl (x) * cosl (y)  +  (coshl (x) * sinl (y)) * I;
  return (w);
}


/*							casinhl
 *
 *	Complex inverse hyperbolic sine
 *
 *
 *
 * SYNOPSIS:
 *
 * long double complex casinhf();
 * long double complex z, w;
 *
 * w = casinhl (z);
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

long double complex
casinhl (long double complex z)
{
  long double complex w;

  w = -1.0L * I * casinl (z * I);
  return (w);
}


/*							ccoshl
 *
 *	Complex hyperbolic cosine
 *
 *
 *
 * SYNOPSIS:
 *
 * long double complex ccoshl();
 * long double complex z, w;
 *
 * w = ccoshl (z);
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

long double complex
ccoshl (long double complex z)
{
  long double complex w;
  long double x, y;

  x = creal(z);
  y = cimag(z);
  w = coshl (x) * cosl (y)  +  (sinhl (x) * sinl (y)) * I;
  return (w);
}



/*							cacoshl
 *
 *	Complex inverse hyperbolic cosine
 *
 *
 *
 * SYNOPSIS:
 *
 * long double complex cacoshl();
 * long double complex z, w;
 *
 * w = cacoshl (z);
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

long double complex
cacoshl (long double complex z)
{
  long double complex w;

  w = I * cacosl (z);
  return (w);
}


/*							ctanhl
 *
 *	Complex hyperbolic tangent
 *
 *
 *
 * SYNOPSIS:
 *
 * long double complex ctanhl();
 * long double complex z, w;
 *
 * w = ctanhl (z);
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

long double complex
ctanhl (long double complex z)
{
  long double complex w;
  long double x, y, d;

  x = creal(z);
  y = cimag(z);
  d = coshl (2.0L * x) + cosl (2.0L * y);
  w = sinhl (2.0L * x) / d  +  (sinl (2.0L * y) / d) * I;
  return (w);
}


/*							catanhl
 *
 *	Complex inverse hyperbolic tangent
 *
 *
 *
 * SYNOPSIS:
 *
 * long double complex catanhl();
 * long double complex z, w;
 *
 * w = catanhl (z);
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

long double complex
catanhl (long double complex z)
{
  long double complex w;

  w = -1.0L * I * catanl (z * I);
  return (w);
}

/*							cpowl
 *
 *	Complex power function
 *
 *
 *
 * SYNOPSIS:
 *
 * long double complex cpowl();
 * long double complex a, z, w;
 *
 * w = cpowl (a, z);
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

long double complex
cpowl (long double complex a, long double complex z)
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
