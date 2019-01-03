/* Complex variable version.  */
/*   mtstf.c
 Consistency tests for math functions.
 To get strict rounding rules on a 386 or 68000 computer,
 define SETPREC to 1.

 The following are typical results for IEEE 80-bit long double
 precision arithmetic:

Consistency test of complex math functions.
Max and rms relative errors for 10000 random arguments.
z =   csqrtl( csquarel(z) ):  max = 1.08e-19   rms = 2.55e-20
x =    csinl(   casinl(x) ):  max = 6.53e-19   rms = 1.35e-19
x =   catanl(    ctanl(x) ):  max = 1.32e-18   rms = 1.04e-19
x =    ccosl(   cacosl(x) ):  max = 2.28e-18   rms = 1.58e-19
x =   csinhl(  casinhl(x) ):  max = 7.64e-19   rms = 1.33e-19
x =   ctanhl(  catanhl(x) ):  max = 9.93e-19   rms = 8.91e-20
x =   ccoshl(  cacoshl(x) ):  max = 4.10e-18   rms = 1.62e-19
x =    cexpl(    clogl(x) ):  max = 3.63e-19 A rms = 1.45e-19 A
x = cpowl( cpowl(x,a),1/a ):  max = 1.07e-17   rms = 2.21e-19
cgammal(x) = exp(clgaml(x)):  max = 1.37e-17 A rms = 1.86e-18 A

... or this, for 128-bit long double precision:

Consistency test of complex math functions.
Max and rms relative errors for 10000 random arguments.
z =   csqrtl( csquarel(z) ):  max = 1.92e-34   rms = 4.56e-35
x =    csinl(   casinl(x) ):  max = 1.68e-33   rms = 2.39e-34
x =   catanl(    ctanl(x) ):  max = 3.73e-33   rms = 1.93e-34
x =    ccosl(   cacosl(x) ):  max = 4.11e-33   rms = 2.81e-34
x =   csinhl(  casinhl(x) ):  max = 1.45e-33   rms = 2.25e-34
x =   ctanhl(  catanhl(x) ):  max = 1.08e-33   rms = 1.57e-34
x =   ccoshl(  cacoshl(x) ):  max = 4.92e-33   rms = 2.69e-34
x =    cexpl(    clogl(x) ):  max = 7.68e-34 A rms = 2.66e-34 A
x = cpowl( cpowl(x,a),1/a ):  max = 1.54e-32   rms = 4.10e-34
(200 trials:)
cgammal(x) = exp(clgaml(x)):  max = 3.66e-32 A rms = 1.10e-32 A
*/

/*
Cephes Math Library Release 2.7:  December, 1998
Copyright 1984 - 1998 by Stephen L. Moshier
*/


#include <stdio.h>
#include <stdlib.h>
#include "complex.h"
#include "mconf.h"

#ifndef NTRIALS
#ifdef LD128BITS
#define NTRIALS 200
#else
#define NTRIALS 10000
#endif
#endif

#define SETPREC 1
#define STRTST 0

#define WTRIALS (NTRIALS/5)

#ifndef ANSIPROT
long double fabsl(), sqrtl(), cbrtl(), expl(), logl();
long double exp10l(), log10l(), tanl(), atanl();
long double sinl(), asinl(), cosl(), acosl(), powl();
long double tanhl(), atanhl(), sinhl(), asinhl(), coshl(), acoshl();
long double cabsl();
long double complex csqrtl(), ccbrtl(), cexpl(), clogl();
long double complex ctanl(), catanl();
long double complex csinl(), casinl(), ccosl(), cacosl(), cpowl();
long double complex ctanhl(), catanhl(), csinhl(), casinhl();
long double complex ccoshl(), cacoshl();
long double complex cgammal(), clgaml();
#else
long double expl(long double);
long double sqrtl(long double);
#endif

#if SETPREC
int ldprec();
#endif

int drand();
/* void exit(); */
/* int printf(); */


/* Provide inverses for square root and cube root: */
long double complex
csquarel(long double complex x)
{
  return( x * x );
}

long double complex
ccubel(long double complex x)
{
  return( x * x * x );
}

/* lookup table for each function */
struct fundef
	{
	char *nam1;		/* the function */
	long double complex (*name )();
	char *nam2;		/* its inverse  */
	long double complex (*inv )();
	int nargs;		/* number of function arguments */
	int tstyp;		/* type code of the function */
	long ctrl;		/* relative error flag */
	long double arg1w;		/* width of domain for 1st arg */
	long double arg1l;		/* lower bound domain 1st arg */
	long arg1f;		/* flags, e.g. integer arg */
	long double arg2w;		/* same info for args 2, 3, 4 */
	long double arg2l;
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

extern long double MINLOGL;
extern long double MAXLOGL;
extern long double PIL;
extern long double PIO2L;
/*
define MINLOG -170.0
define MAXLOG +170.0
define PI 3.14159265358979323846
define PIO2 1.570796326794896619
*/

#define NTESTS 10
struct fundef defs[NTESTS] = {
{"csquarel",csquarel,  "  csqrtl",  csqrtl, 1, SQRT, 1, 20.0,  -10.0, EXPSCAL,
0.0, 0.0, 0},
{"  casinl",  casinl,   "   csinl",   csinl, 1, 0, 1,   3.14,      -1.57,  0,
0.0, 0.0, 0},
{"   ctanl",   ctanl,   "  catanl",  catanl, 1, 0, 1,  3.14,  -1.57,  0,
0.0, 0.0, 0},
{"  cacosl",  cacosl,   "   ccosl",   ccosl, 1, 0, 1,   3.14,      -1.57,  0,
0.0, 0.0, 0},
{" casinhl", casinhl,   "  csinhl",  csinhl, 1, 0, 1, 3.14,   -1.57,  0,
0.0, 0.0, 0},
{" catanhl", catanhl,   "  ctanhl",  ctanhl, 1, 0, 1, 3.14,   -1.57,  0,
0.0, 0.0, 0},
{" cacoshl", cacoshl,   "  ccoshl",  ccoshl, 1, 0, 1, 3.14,   -1.57,  0,
0.0, 0.0, 0},
{"   clogl",   clogl,   "   cexpl",   cexpl, 1, 0, 0, 340.0,    -170.0,  0,
0.0, 0.0, 0},
{"cpowl",      cpowl,     "cpowl",   cpowl, 2, POWER, 1, 2.0, 0.1,   0,
2.0, -1.0, 0},
{"clgaml",   clgaml,    "cgammal", cgammal, 1, GAMMA, 0, 42.0, -21.0,   0,
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
 
static long double complex yy1;
static long double complex y2;
static long double complex y3;
static long double complex y4;
static long double complex a;
static long double complex x;
static long double complex xsave;
static long double complex y;
static long double complex z;
static long double complex e;
static long double de;
static long double max;
static long double rmsa;
static long double rms;
static long double ave;
static double dr, di;

int main()
{
long double complex (*fun )();
long double complex (*ifun )();
struct fundef *d;
int i, k, itst;
int m, ntr;

#if SETPREC
ldprec();  /* set coprocessor precision */
#endif
ntr = NTRIALS;
printf( "Consistency test of complex math functions.\n" );
printf( "Max and rms relative errors for %d random arguments.\n",
	ntr );

#if 0
/* Initialize machine dependent parameters: */
defs[1].arg1w = PIL;
defs[1].arg1l = -PIL/2.0L;
#endif
/* Microsoft C has trouble with denormal numbers. */
#if 0
defs[3].arg1w = MAXLOGL;
defs[3].arg1l = -MAXLOGL/2.0L;
defs[4].arg1w = 2.0L*MAXLOGL;
defs[4].arg1l = -MAXLOGL;
#endif
#if 0
defs[6].arg1w = 2.0L*MAXLOGL;
defs[6].arg1l = -MAXLOGL;
defs[7].arg1w = MAXLOGL;
defs[7].arg1l = 0.0L;
#endif

/* Outer loop, on the test number: */

for( itst=STRTST; itst<NTESTS; itst++ )
{
d = &defs[itst];
k = 0;
m = 0;
max = 0.0L;
rmsa = 0.0L;
ave = 0.0L;
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
a = (long double) dr + (long double) di * I;

if( d->arg2f & EXPSCAL )
	{
	a = expl((long double) dr) + I * expl((long double) di);
	drand( &dr );
	drand( &di );
	y2 = (long double) dr + (long double) di * I;
	a -= 1.0e-13L * a * y2;
	}
if( d->arg2f & INT )
	{
	k = cabsl(a) + 0.25f;
	a = k;
	}

case 1:
drand( &dr );
drand( &di );
dr = d->arg1w *  ( dr - 1.0 )  +  d->arg1l;
di = d->arg1w *  ( di - 1.0 )  +  d->arg1l;
x = (long double) dr + (long double) di * I;
if( d->arg1f & EXPSCAL )
	{
	x = expl((long double) dr) + I * expl((long double) di);
	drand( &dr );
	drand( &di );
	a = (long double) dr + (long double) di * I;
	x += 1.0e-13L * x * a;
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
		y2 = ( *(fun) )(1.0L-x);
		y3 = ( *(ifun) )(x);
		y4 = ( *(ifun) )(1.0L-x);
		break;

		case GAMMA:
		z = cgammal(x);
		y = cexpl(clgaml(x));
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
			y2 = (*fun)( a+1.0L, x );
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
			y = (*ifun)( z, 1.0L/a );
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
	e = (y2*y3 - yy1*y4) - 2.0L/(PIL*x); /* Jn, Yn */
	break;

	case WRONK2:
	e = (y2*y3 + yy1*y4) - 1.0L/x; /* In, Kn */
	break;
	
	case ELLIP:
	e = (yy1-y3)*y4 + y3*y2 - PIO2L;
	break;

	case SQRT:
	  /* Square root could have either sign.  */
	de = cabsl ((y - x) / x);
	if (de > 1.5L)
	  {
	    /* Try -y. */
	    dr = cabsl ((-y - x) / x);
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
	if( cabsl(x) > 1.0L )
		e /= x;
	}

de = cabsl(e);

statistics:

ave +=de;
/* absolute value of error */
if( de < 0 )
	de = -de;

/* peak detect the error */
if( de > max )
	{
	max = de;

	if( de > 4.0e-16L )
		{
#if 0
		printf(
   "x (%.6Le,%.6Le) z (%.6Le,%.6Le) y (%.6Le,%.6Le) max %.4Le\n",
		       creall(xsave), cimagl(xsave), creall(z), cimagl(z),
		       creall(y), cimagl(y), max);
#else
                printf(
   "x (%.6e,%.6e) z (%.6e,%.6e) y (%.6e,%.6e) max %.4e\n",
                       (double) creall(xsave), (double) cimagl(xsave),
                       (double) creall(z), (double) cimagl(z),
                       (double) creall(y), (double) cimagl(y),
                       (double) max);
#endif
		if( d->tstyp == POWER )
			{
			printf( "a (%.6Le,%.6Le)\n", creall(a), cimagl(a) );
			}
		if( d->tstyp >= WRONK1 )
			{
		printf(
 "yy1 (%.4Le,%.4Le) y2 (%.4Le,%.4Le) y3 (%.4Le,%.4Le) y4 (%.4Le,%.4Le) k %d x (%.4Le,%.4Le)\n",
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
de *= 1.0e16L;	/* adjust range */
rmsa += de * de;	/* accumulate the square of the error */
}

/* report after NTRIALS trials */
rms = 1.0e-16L * sqrtl( rmsa/m );
if(d->ctrl & RELERR)
	printf(" max = %.2Le   rms = %.2Le\n", max, rms );
else
	printf(" max = %.2Le A rms = %.2Le A\n", max, rms );
} /* loop on itst */

exit(0);
}
