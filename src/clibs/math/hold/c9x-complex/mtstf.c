/* Complex variable version.  */
/*   mtstf.c
 Consistency tests for math functions.
 To get strict rounding rules on a 386 or 68000 computer,
 define SETPREC to 1.

 The following are typical results for
 IEEE single precision arithmetic.

Consistency test of complex math functions.
Max and rms relative errors for 10000 random arguments.
x =   csqrtf( csquaref(x) ):  max = 1.19E-07   rms = 2.38E-08
x =    csinf(   casinf(x) ):  max = 2.23E-06   rms = 1.55E-07
x =   catanf(    ctanf(x) ):  max = 2.37E-06   rms = 1.18E-07
x =    ccosf(   cacosf(x) ):  max = 1.64E-05   rms = 2.51E-07
x =   csinhf(  casinhf(x) ):  max = 6.96E-06   rms = 1.71E-07
x =   ctanhf(  catanhf(x) ):  max = 2.25E-06   rms = 1.03E-07
x =   ccoshf(  cacoshf(x) ):  max = 6.72E-06   rms = 1.99E-07
x =    cexpf(    clogf(x) ):  max = 4.00E-07 A rms = 1.63E-07 A
x = cpowf( cpowf(x,a),1/a ):  max = 8.65E-06   rms = 2.30E-07
cgammaf(x) = exp(clgamf(x)):  max = 5.70E-06 A rms = 7.67E-07 A
*/

/*
Cephes Math Library Release 2.7: April, 1998
Copyright 1984-1998 by Stephen L. Moshier
*/


#include <stdio.h>
#include <stdlib.h>
#include "complex.h"
#include "mconf.h"

#ifndef NTRIALS
#define NTRIALS 100000
#endif

#define SETPREC 1
#define STRTST 0

#define WTRIALS (NTRIALS/5)

#ifndef ANSIPROT
float fabsf(), sqrtf(), cbrtf(), expf(), logf();
float exp10f(), log10f(), tanf(), atanf();
float sinf(), asinf(), cosf(), acosf(), powf();
float tanhf(), atanhf(), sinhf(), asinhf(), coshf(), acoshf();
float cabsf();
float complex csqrtf(), ccbrtf(), cexpf(), clogf();
float complex ctanf(), catanf();
float complex csinf(), casinf(), ccosf(), cacosf(), cpowf();
float complex ctanhf(), catanhf(), csinhf(), casinhf(), ccoshf(), cacoshf();
float complex cgammaf(), clgamf();
#else
float expf(float);
float sqrtf(float);
#endif

#if SETPREC
int sprec();
#endif

int drand();
/* void exit(); */
/* int printf(); */


/* Provide inverses for square root and cube root: */
float complex
csquaref(float complex x)
{
  return( x * x );
}

float complex
ccube(float complex x)
{
  return( x * x * x );
}

/* lookup table for each function */
struct fundef
	{
	char *nam1;		/* the function */
	float complex (*name )();
	char *nam2;		/* its inverse  */
	float complex (*inv )();
	int nargs;		/* number of function arguments */
	int tstyp;		/* type code of the function */
	long ctrl;		/* relative error flag */
	float arg1w;		/* width of domain for 1st arg */
	float arg1l;		/* lower bound domain 1st arg */
	long arg1f;		/* flags, e.g. integer arg */
	float arg2w;		/* same info for args 2, 3, 4 */
	float arg2l;
	long arg2f;
/*
	double arg3w;
	double arg3l;
	long arg3f;
	double arg4w;
	double arg4l;
	long arg4f;
*/
	};


/* fundef.ctrl bits: */
#define RELERR 1

/* fundef.tstyp  test types: */
#define POWER 1 
#define ELLIP 2 
#define GAMMA 3
#define WRONK1 4
#define WRONK2 5
#define WRONK3 6
#define SQRT 7

/* fundef.argNf  argument flag bits: */
#define INT 2
#define EXPSCAL 4

extern float MINLOGF;
extern float MAXLOGF;
extern float PIF;
extern float PIO2F;
/*
define MINLOG -170.0
define MAXLOG +170.0
define PI 3.14159265358979323846
define PIO2 1.570796326794896619
*/

#define NTESTS 10
struct fundef defs[NTESTS] = {
{"csquaref",csquaref,  "  csqrtf",  csqrtf, 1, SQRT, 1, 20.0,  -10.0, EXPSCAL,
0.0, 0.0, 0},
{"  casinf",  casinf,   "   csinf",   csinf, 1, 0, 1,   3.14,      -1.57,  0,
0.0, 0.0, 0},
{"   ctanf",   ctanf,   "  catanf",  catanf, 1, 0, 1,  3.14,  -1.57,  0,
0.0, 0.0, 0},
{"  cacosf",  cacosf,   "   ccosf",   ccosf, 1, 0, 1,   3.14,      -1.57,  0,
0.0, 0.0, 0},
{" casinhf", casinhf,   "  csinhf",  csinhf, 1, 0, 1, 3.14,   -1.57,  0,
0.0, 0.0, 0},
{" catanhf", catanhf,   "  ctanhf",  ctanhf, 1, 0, 1, 3.14,   -1.57,  0,
0.0, 0.0, 0},
{" cacoshf", cacoshf,   "  ccoshf",  ccoshf, 1, 0, 1, 3.14,   -1.57,  0,
0.0, 0.0, 0},
{"   clogf",   clogf,   "   cexpf",   cexpf, 1, 0, 0, 340.0,    -170.0,  0,
0.0, 0.0, 0},
{"cpowf",      cpowf,     "cpowf",   cpowf, 2, POWER, 1, 2.0, 0.1,   0,
2.0, -1.0, 0},
{"clgamf",   clgamf,   "cgammaf",  cgammaf, 1, GAMMA, 0, 40.0, -20.0,   0,
0.0, 0.0, 0},

#if 0
{"  cube",   cube,   "  cbrt",   cbrt, 1, 0, 1, 2002.0, -1001.0, 0,
0.0, 0.0, 0},
#endif
};

static char *headrs[] = {
"x = %s( %s(x) ): ",
"x = %s( %s(x,a),1/a ): ",	/* power */
"Legendre %s, %s: ",		/* ellip */
"%s(x) = exp(%s(x)): ",		/* gamma */
"Wronksian of %s, %s: ",
"Wronksian of %s, %s: ",
"Wronksian of %s, %s: ",
"z = %s( %s(z) ): ",
};
 
static float complex yy1;
static float complex y2;
static float complex y3;
static float complex y4;
static float complex a;
static float complex x;
static float complex xsave;
static float complex y;
static float complex z;
static float complex e;
static float de;
static float max;
static float rmsa;
static float rms;
static float ave;
static double dr, di;

int main()
{
float complex (*fun )();
float complex (*ifun )();
struct fundef *d;
int i, k, itst;
int m, ntr;

#if SETPREC
sprec();  /* set coprocessor precision */
#endif
ntr = NTRIALS;
printf( "Consistency test of complex math functions.\n" );
printf( "Max and rms relative errors for %d random arguments.\n",
	ntr );

#if 0
/* Initialize machine dependent parameters: */
defs[1].arg1w = PIF;
defs[1].arg1l = -PIF/2.0f;
#endif
/* Microsoft C has trouble with denormal numbers. */
#if 0
defs[3].arg1w = MAXLOGF;
defs[3].arg1l = -MAXLOGF/2.0f;
defs[4].arg1w = 2.0f*MAXLOGF;
defs[4].arg1l = -MAXLOGF;
#endif
#if 0
defs[6].arg1w = 2.0f*MAXLOGF;
defs[6].arg1l = -MAXLOGF;
defs[7].arg1w = MAXLOGF;
defs[7].arg1l = 0.0f;
#endif

/* Outer loop, on the test number: */

for( itst=STRTST; itst<NTESTS; itst++ )
{
d = &defs[itst];
k = 0;
m = 0;
max = 0.0f;
rmsa = 0.0f;
ave = 0.0f;
fun = d->name;
ifun = d->inv;

#if 0
/* Absolute error criterion starts with gamma function
 * (put all such at end of table)
 */
if( d->tstyp == GAMMA )
	printf( "Absolute error criterion (but relative if >1):\n" );
#endif

/* Smaller number of trials for Wronksians
 * (put them at end of list)
 */
if( d->tstyp == WRONK1 )
	{
	ntr = WTRIALS;
	printf( "Absolute error and only %d trials:\n", ntr );
	}

printf( headrs[d->tstyp], d->nam2, d->nam1 );

for( i=0; i<ntr; i++ )
{
m++;

/* make random number(s) in desired range(s) */
switch( d->nargs )
{

default:
goto illegn;
	
case 2:
drand( &dr );
drand( &di );
dr = d->arg2w *  ( dr - 1.0 )  +  d->arg2l;
di = d->arg2w *  ( di - 1.0 )  +  d->arg2l;
a = (float) dr + (float) di * I;

if( d->arg2f & EXPSCAL )
	{
	a = expf(dr) + I * expf(di);
	drand( &dr );
	drand( &di );
	y2 = (float) dr + (float) di * I;
	a -= 1.0e-13f * a * y2;
	}
if( d->arg2f & INT )
	{
	k = cabsf(a) + 0.25f;
	a = k;
	}

case 1:
drand( &dr );
drand( &di );
dr = d->arg1w *  ( dr - 1.0 )  +  d->arg1l;
di = d->arg1w *  ( di - 1.0 )  +  d->arg1l;
x = (float) dr + (float) di * I;
if( d->arg1f & EXPSCAL )
	{
	x = expf(dr) + I * expf(di);
	drand( &dr );
	drand( &di );
	a = (float) dr + (float) di * I;
	x += 1.0e-13f * x * a;
	}
}

/* compute function under test */
xsave = x;
switch( d->nargs )
	{
	case 1:
	switch( d->tstyp )
		{
		case ELLIP:
		yy1 = ( *(fun) )(x);
		y2 = ( *(fun) )(1.0f-x);
		y3 = ( *(ifun) )(x);
		y4 = ( *(ifun) )(1.0f-x);
		break;

		case GAMMA:
		z = cgammaf(x);
		y = cexpf(clgamf(x));
		x = z;  /* This breaks the printout of large errors.  */
		break;

		default:
		z = ( *(fun) )(x);
		y = ( *(ifun) )(z);
		}
	break;
	
	case 2:
	if( d->arg2f & INT )
		{
		switch( d->tstyp )
			{
			case WRONK1:
			yy1 = (*fun)( k, x ); /* jn */
			y2 = (*fun)( k+1, x );
			y3 = (*ifun)( k, x ); /* yn */
			y4 = (*ifun)( k+1, x );	
			break;

			case WRONK2:
			yy1 = (*fun)( a, x ); /* iv */
			y2 = (*fun)( a+1.0f, x );
			y3 = (*ifun)( k, x ); /* kn */	
			y4 = (*ifun)( k+1, x );	
			break;

			default:
			z = (*fun)( k, x );
			y = (*ifun)( k, z );
			}
		}
	else
		{
		if( d->tstyp == POWER )
			{
			z = (*fun)( x, a );
			y = (*ifun)( z, 1.0f/a );
			}
		else
			{
			z = (*fun)( a, x );
			y = (*ifun)( a, z );
			}
		}
	break;


	default:
illegn:
	printf( "Illegal nargs= %d", d->nargs );
	exit(1);
	}	

switch( d->tstyp )
	{
	case WRONK1:
	e = (y2*y3 - yy1*y4) - 2.0f/(PIF*x); /* Jn, Yn */
	break;

	case WRONK2:
	e = (y2*y3 + yy1*y4) - 1.0f/x; /* In, Kn */
	break;
	
	case ELLIP:
	e = (yy1-y3)*y4 + y3*y2 - PIO2F;
	break;

	case SQRT:
	  /* Square root could have either sign.  */
	de = cabsf ((y - x) / x);
	if (de > 1.5f)
	  {
	    /* Try -y. */
	    dr = cabsf ((-y - x) / x);
	    if (dr < de)
	      {
		y = -y;
		de = dr;
	      }
	  }
	goto statistics;

	default:
	e = y - x;
	break;
	}

if( d->ctrl & RELERR )
	e /= x;
else
	{
	if( cabsf(x) > 1.0f )
		e /= x;
	}

de = cabsf(e);

statistics:

ave +=de;
/* absolute value of error */
if( de < 0 )
	de = -de;

/* peak detect the error */
if( de > max )
	{
	max = de;

	if( de > 1.0e-4f )
		{
		printf("x (%.9E,%.9E) z (%.9E,%.9E) y (%.9E,%.9E) max %.4E\n",
		       creal(xsave), cimag(xsave), creal(z), cimag(z),
		       creal(y), cimag(y), max);
		if( d->tstyp == POWER )
			{
			printf( "a (%.6E,%.6E)\n", creal(a), cimag(a) );
			}
		if( d->tstyp == WRONK1 )
			{
		printf(
 "yy1 (%.4E,%.4E) y2 (%.4E,%.4E) y3 (%.4E,%.4E) y4 (%.4E,%.4E) k %d x (%.4E,%.4E)\n",
 creal(yy1), cimag(yy1), creal(y2), cimag(y2),
 creal(y3), cimag(y3), creal(y4), cimag(y4), k, creal(xsave), cimag(xsave) );
			}
		}

/*
	printf("%.8E %.8E %.4E %6ld \n", x, y, max, n);
	printf("%d %.8E %.8E %.4E %6ld \n", k, x, y, max, n);
	printf("%.6E %.6E %.6E %.4E %6ld \n", a, x, y, max, n);
	printf("%.6E %.6E %.6E %.6E %.4E %6ld \n", a, b, x, y, max, n);
	printf("%.4E %.4E %.4E %.4E %.4E %.4E %6ld \n",
		a, b, c, x, y, max, n);
*/
	}

/* accumulate rms error	*/
de *= 1.0e8f;	/* adjust range */
rmsa += de * de;	/* accumulate the square of the error */
}

/* report after NTRIALS trials */
rms = 1.0e-8f * sqrtf( rmsa/m );
if(d->ctrl & RELERR)
	printf(" max = %.2E   rms = %.2E\n", max, rms );
else
	printf(" max = %.2E A rms = %.2E A\n", max, rms );
} /* loop on itst */

exit(0);
}
