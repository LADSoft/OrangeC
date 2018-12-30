/* Complex variable version.  */
/*   mtst.c
 Consistency tests for math functions.
 To get strict rounding rules on a 386 or 68000 computer,
 define SETPREC to 1.

 The following are typical results for
 IEEE double precision arithmetic.

Consistency test of complex math functions.
Max and rms relative errors for 10000 random arguments.
z =   csqrt( csquare(z) ):  max = 2.22E-16   rms = 2.57E-17
z =    csin(   casin(z) ):  max = 6.60E-15   rms = 2.94E-16
z =   catan(    ctan(z) ):  max = 3.95E-15   rms = 2.16E-16
z =    ccos(   cacos(z) ):  max = 1.01E-14   rms = 3.50E-16
z =   csinh(  casinh(z) ):  max = 5.56E-15   rms = 2.83E-16
z =   ctanh(  catanh(z) ):  max = 2.72E-15   rms = 2.00E-16
z =   ccosh(  cacosh(z) ):  max = 8.57E-15   rms = 3.52E-16
z =    cexp(    clog(z) ):  max = 7.86E-16 A rms = 3.03E-16 A
z = cpow( cpow(z,a),1/a ):  max = 1.27E-14   rms = 4.43E-16
cgamma(z) = exp(clgam(z)):  max = 2.45E-14 A rms = 3.45E-15 A
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
#define NTRIALS 10000
#endif

#define SETPREC 1
#define STRTST 0

#define WTRIALS (NTRIALS/5)

#ifndef ANSIPROT
double fabs(), sqrt(), cbrt(), exp(), log();
double exp10(), log10(), tan(), atan();
double sin(), asin(), cos(), acos(), pow();
double tanh(), atanh(), sinh(), asinh(), cosh(), acosh();
double cabs();
double complex csqrt(), ccbrt(), cexp(), clog();
double complex ctan(), catan();
double complex csin(), casin(), ccos(), cacos(), cpow();
double complex ctanh(), catanh(), csinh(), casinh(), ccosh(), cacosh();
double complex cgamma(), clgam();
#endif

#if SETPREC
int dprec();
#endif

int drand();
/* void exit(); */
/* int printf(); */


/* Provide inverses for square root and cube root: */
double complex
csquare(x)
     double complex x;
{
  return( x * x );
}

double complex
ccube(x)
     double complex x;
{
  return( x * x * x );
}

/* lookup table for each function */
struct fundef
	{
	char *nam1;		/* the function */
	double complex (*name )();
	char *nam2;		/* its inverse  */
	double complex (*inv )();
	int nargs;		/* number of function arguments */
	int tstyp;		/* type code of the function */
	long ctrl;		/* relative error flag */
	double arg1w;		/* width of domain for 1st arg */
	double arg1l;		/* lower bound domain 1st arg */
	long arg1f;		/* flags, e.g. integer arg */
	double arg2w;		/* same info for args 2, 3, 4 */
	double arg2l;
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

extern double MINLOG;
extern double MAXLOG;
extern double PI;
extern double PIO2;
/*
define MINLOG -170.0
define MAXLOG +170.0
define PI 3.14159265358979323846
define PIO2 1.570796326794896619
*/

#define NTESTS 10
struct fundef defs[NTESTS] = {
{"csquare",csquare,  "  csqrt",  csqrt, 1, SQRT, 1, 170.0,    -85.0, EXPSCAL,
0.0, 0.0, 0},
{"  casin",  casin,   "   csin",   csin, 1, 0, 1,   3.14,      -1.57,  0,
0.0, 0.0, 0},
{"   ctan",   ctan,   "  catan",  catan, 1, 0, 1,  3.14,  -1.57,  0,
0.0, 0.0, 0},
{"  cacos",  cacos,   "   ccos",   ccos, 1, 0, 1,   3.14,      -1.57,  0,
0.0, 0.0, 0},
{" casinh", casinh,   "  csinh",  csinh, 1, 0, 1, 3.14,   -1.57,  0,
0.0, 0.0, 0},
{" catanh", catanh,   "  ctanh",  ctanh, 1, 0, 1, 3.14,   -1.57,  0,
0.0, 0.0, 0},
{" cacosh", cacosh,   "  ccosh",  ccosh, 1, 0, 1, 3.14,   -1.57,  0,
0.0, 0.0, 0},
{"   clog",   clog,   "   cexp",   cexp, 1, 0, 0, 340.0,    -170.0,  0,
0.0, 0.0, 0},
{"cpow",      cpow,     "cpow",   cpow, 2, POWER, 1, 2.0, 0.1,   0,
2.0, -1.0, 0},
{"clgam",    clgam,   "cgamma",  cgamma, 1, GAMMA, 0, 42.0, -21.0,   0,
0.0, 0.0, 0},

#if 0
{"  cube",   cube,   "  cbrt",   cbrt, 1, 0, 1, 2002.0, -1001.0, 0,
0.0, 0.0, 0},
#endif
};

static char *headrs[] = {
"z = %s( %s(z) ): ",
"z = %s( %s(z,a),1/a ): ",	/* power */
"Legendre %s, %s: ",		/* ellip */
"%s(z) = exp(%s(z)): ",		/* gamma */
"Wronksian of %s, %s: ",
"Wronksian of %s, %s: ",
"Wronksian of %s, %s: ",
"z = %s( %s(z) ): ",
};
 
static double complex yy1;
static double complex y2;
static double complex y3;
static double complex y4;
static double complex a;
static double complex x;
static double complex y;
static double complex z;
static double complex e;
static double de = 0.0;
static double max = 0.0;
static double rmsa = 0.0;
static double rms = 0.0;
static double ave = 0.0;
static double dr, di;

int main()
{
double complex (*fun )();
double complex (*ifun )();
struct fundef *d;
int i, k, itst;
int m, ntr;

#if SETPREC
dprec();  /* set coprocessor precision */
#endif
ntr = NTRIALS;
printf( "Consistency test of complex math functions.\n" );
printf( "Max and rms relative errors for %d random arguments.\n",
	ntr );

#if 0
/* Initialize machine dependent parameters: */
defs[1].arg1w = PI;
defs[1].arg1l = -PI/2.0;
#endif
/* Microsoft C has trouble with denormal numbers. */
#if 0
defs[3].arg1w = MAXLOG;
defs[3].arg1l = -MAXLOG/2.0;
defs[4].arg1w = 2*MAXLOG;
defs[4].arg1l = -MAXLOG;
#endif
#if 0
defs[6].arg1w = 2.0*MAXLOG;
defs[6].arg1l = -MAXLOG;
defs[7].arg1w = MAXLOG;
defs[7].arg1l = 0.0;
#endif

/* Outer loop, on the test number: */

for( itst=STRTST; itst<NTESTS; itst++ )
{
d = &defs[itst];
k = 0;
m = 0;
max = 0.0;
rmsa = 0.0;
ave = 0.0;
fun = d->name;
ifun = d->inv;

/* Error criterion is controlled by RELERR now.  Letter "A" in the
   result display indicates absolute error criterion.  */
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
a = dr + di * I;

if( d->arg2f & EXPSCAL )
	{
	a = exp(dr) + I * exp(di);
	drand( &dr );
	drand( &di );
	y2 = dr + di * I;
	a -= 1.0e-13 * a * y2;
	}
if( d->arg2f & INT )
	{
	k = cabs(a) + 0.25;
	a = k;
	}

case 1:
drand( &dr );
drand( &di );
dr = d->arg1w *  ( dr - 1.0 )  +  d->arg1l;
di = d->arg1w *  ( di - 1.0 )  +  d->arg1l;
x = dr + di * I;
if( d->arg1f & EXPSCAL )
	{
	x = exp(dr) + I * exp(di);
	drand( &dr );
	drand( &di );
	a = dr + di * I;
	x += 1.0e-13 * x * a;
	}
}


/* compute function under test */
switch( d->nargs )
	{
	case 1:
	switch( d->tstyp )
		{
		case ELLIP:
		yy1 = ( *(fun) )(x);
		y2 = ( *(fun) )(1.0-x);
		y3 = ( *(ifun) )(x);
		y4 = ( *(ifun) )(1.0-x);
		break;

		case GAMMA:
		z = cgamma(x);
		y = cexp(clgam(x));
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
			y2 = (*fun)( a+1.0, x );
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
			y = (*ifun)( z, 1.0/a );
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
	e = (y2*y3 - yy1*y4) - 2.0/(PI*x); /* Jn, Yn */
	break;

	case WRONK2:
	e = (y2*y3 + yy1*y4) - 1.0/x; /* In, Kn */
	break;
	
	case ELLIP:
	e = (yy1-y3)*y4 + y3*y2 - PIO2;
	break;

	case SQRT:
	  /* Square root could have either sign.  */
	de = cabs ((y - x) / x);
	if (de > 1.5)
	  {
	    /* Try -y. */
	    dr = cabs ((-y - x) / x);
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
	if( cabs(x) > 1.0 )
		e /= x;
	}
de = cabs(e);

statistics:

ave +=de;
/* absolute value of error */
if( de < 0 )
	de = -de;

/* peak detect the error */
if( de > max )
	{
	max = de;

	if( de > 1.0e-10 )
		{
		printf("x (%.16E,%.16E) z (%.16E,%.16E) y (%.16E,%.16E) max %.4E\n",
		       creal(x), cimag(x), creal(z), cimag(z),
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
 creal(y3), cimag(y3), creal(y4), cimag(y4), k, creal(x), cimag(x) );
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
de *= 1.0e16;	/* adjust range */
rmsa += de * de;	/* accumulate the square of the error */
}

/* report after NTRIALS trials */
rms = 1.0e-16 * sqrt( rmsa/m );
if(d->ctrl & RELERR)
	printf(" max = %.2E   rms = %.2E\n", max, rms );
else
	printf(" max = %.2E A rms = %.2E A\n", max, rms );
} /* loop on itst */

exit(0);
}
