/*							lgam()
 *
 *	Natural loglarithm of gamma function
 *
 *
 *
 * SYNOPSIS:
 *
 * long double x, y, __lgamma_r();
 * int* sgngam;
 * y = __lgamma_r( x, sgngam );
 * 
 * long double x, y, lgamma();
 * y = lgamma( x);
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the base e (2.718...) loglarithm of the absolute
 * value of the gamma function of the argument. In the reentrant
 * version, the sign (+1 or -1) of the gamma function is returned
 * in the variable referenced by sgngam.
 *
 * For arguments greater than 13, the loglarithm of the gamma
 * function is approximated by the loglarithmic version of
 * Stirling's formula usinlg a polynomial approximation of
 * degree 4. Arguments between -33 and +33 are reduced by
 * recurrence to the interval [2,3] of a rational approximation.
 * The cosecant reflection formula is employed for arguments
 * less than -33.
 *
 * Arguments greater than MAXLGM return MAXNUM and an error
 * message.  MAXLGM = 2.035093e36 for DEC
 * arithmetic or 2.556348e305 for IEEE arithmetic.
 *
 *
 *
 * ACCURACY:
 *
 *
 * arithmetic      domain        # trials     peak         rms
 *    DEC     0, 3                  7000     5.2e-17     1.3e-17
 *    DEC     2.718, 2.035e36       5000     3.9e-17     9.9e-18
 *    IEEE    0, 3                 28000     5.4e-16     1.1e-16
 *    IEEE    2.718, 2.556e305     40000     3.5e-16     8.3e-17
 * The error criterion was relative when the function magnitude
 * was greater than one but absolute when it was less than one.
 *
 * The following test used the relative error criterion, though
 * at certain points the relative error could be much higher than
 * indicated.
 *    IEEE    -200, -4             10000     4.8e-16     1.3e-16
 *
 */

/*
 * Cephes Math Library Release 2.8:  June, 2000
 * Copyright 1984, 1987, 1989, 1992, 2000 by Stephen L. Moshier
 */

/*
 * 26-11-2002 Modified for mingw.
 * Danny Smith 
 */

#include "math.h"
#include "errno.h"
#define _SET_ERRNO(x)
#define mtherr(x,y) { printf("x"); _SET_ERRNO(y); }
#define PI        3.14159265358979323846L
#define MAXNUM 1e100
#ifndef __MINGW32__
//#include "mconf.h"
#define UNK
#define ANSIPROT
#ifdef XXXX
#ifdef ANSIPROT
extern long double powl ( long double, double );
extern long double logl ( long double );
extern long double expl ( long double );
extern long double sinl ( long double );
extern long double polevll ( long double, void *, int );
extern long double p1evll ( long double, void *, int );
extern long double floorl ( long double );
extern long double fabsl ( long double );
extern int isnan ( long double );
extern int isfinite ( long double );
#else
long double powl(), logl(), expl(), sinl(), polevll(), p1evll(), floorl(), fabsl();
int isnan(), isfinite();
#endif
#endif
#ifdef INFINITIES
extern long double INFINITY;
#endif
#ifdef NANS
extern long double NAN;
#endif
#else /* __MINGW32__ */
#include "cephes_mconf.h"
#endif /* __MINGW32__ */


/* A[]: Stirling's formula explansion of logl gamma
 * B[], C[]: logl gamma function between 2 and 3
 */
#ifdef UNK
static long double A[] = {
 8.11614167470508450300E-4L,
-5.95061904284301438324E-4L,
 7.93650340457716943945E-4L,
-2.77777777730099687205E-3L,
 8.33333333333331927722E-2L
};
static long double B[] = {
-1.37825152569120859100E3L,
-3.88016315134637840924E4L,
-3.31612992738871184744E5L,
-1.16237097492762307383E6L,
-1.72173700820839662146E6L,
-8.53555664245765465627E5L
};
static long double C[] = {
/* 1.00000000000000000000E0, */
-3.51815701436523470549E2L,
-1.70642106651881159223E4L,
-2.20528590553854454839E5L,
-1.13933444367982507207E6L,
-2.53252307177582951285E6L,
-2.01889141433532773231E6L
};
/* logl( sqrt( 2*pi ) ) */
static long double LS2PI  =  0.91893853320467274178L;
#define MAXLGM 2.556348e305L
static long double LOGPI = 1.14472988584940017414L;
#endif

#ifdef DEC
static const unsigned short A[] = {
0035524,0141201,0034633,0031405,
0135433,0176755,0126007,0045030,
0035520,0006371,0003342,0172730,
0136066,0005540,0132605,0026407,
0037252,0125252,0125252,0125132
};
static const unsigned short B[] = {
0142654,0044014,0077633,0035410,
0144027,0110641,0125335,0144760,
0144641,0165637,0142204,0047447,
0145215,0162027,0146246,0155211,
0145322,0026110,0010317,0110130,
0145120,0061472,0120300,0025363
};
static const unsigned short C[] = {
/*0040200,0000000,0000000,0000000*/
0142257,0164150,0163630,0112622,
0143605,0050153,0156116,0135272,
0144527,0056045,0145642,0062332,
0145213,0012063,0106250,0001025,
0145432,0111254,0044577,0115142,
0145366,0071133,0050217,0005122
};
/* logl( sqrt( 2*pi ) ) */
static const unsigned short LS2P[] = {040153,037616,041445,0172645,};
#define LS2PI *(long double *)LS2P
#define MAXLGM 2.035093e36
static const unsigned short LPI[4] = {
0040222,0103202,0043475,0006750,
};
#define LOGPI *(long double *)LPI

#endif

#ifdef IBMPC
static const unsigned short A[] = {
0x6661,0x2733,0x9850,0x3f4a,
0xe943,0xb580,0x7fbd,0xbf43,
0x5ebb,0x20dc,0x019f,0x3f4a,
0xa5a1,0x16b0,0xc16c,0xbf66,
0x554b,0x5555,0x5555,0x3fb5
};
static const unsigned short B[] = {
0x6761,0x8ff3,0x8901,0xc095,
0xb93e,0x355b,0xf234,0xc0e2,
0x89e5,0xf890,0x3d73,0xc114,
0xdb51,0xf994,0xbc82,0xc131,
0xf20b,0x0219,0x4589,0xc13a,
0x055e,0x5418,0x0c67,0xc12a
};
static const unsigned short C[] = {
/*0x0000,0x0000,0x0000,0x3ff0,*/
0x12b2,0x1cf3,0xfd0d,0xc075,
0xd757,0x7b89,0xaa0d,0xc0d0,
0x4c9b,0xb974,0xeb84,0xc10a,
0x0043,0x7195,0x6286,0xc131,
0xf34c,0x892f,0x5255,0xc143,
0xe14a,0x6a11,0xce4b,0xc13e
};
/* logl( sqrt( 2*pi ) ) */
static const unsigned short LS2P[] = {
0xbeb5,0xc864,0x67f1,0x3fed
};
#define LS2PI *(long double *)LS2P
#define MAXLGM 2.556348e305
static const unsigned short LPI[4] = {
0xa1bd,0x48e7,0x50d0,0x3ff2,
};
#define LOGPI *(long double *)LPI
#endif

#ifdef MIEEE
static const unsigned short A[] = {
0x3f4a,0x9850,0x2733,0x6661,
0xbf43,0x7fbd,0xb580,0xe943,
0x3f4a,0x019f,0x20dc,0x5ebb,
0xbf66,0xc16c,0x16b0,0xa5a1,
0x3fb5,0x5555,0x5555,0x554b
};
static const unsigned short B[] = {
0xc095,0x8901,0x8ff3,0x6761,
0xc0e2,0xf234,0x355b,0xb93e,
0xc114,0x3d73,0xf890,0x89e5,
0xc131,0xbc82,0xf994,0xdb51,
0xc13a,0x4589,0x0219,0xf20b,
0xc12a,0x0c67,0x5418,0x055e
};
static const unsigned short C[] = {
0xc075,0xfd0d,0x1cf3,0x12b2,
0xc0d0,0xaa0d,0x7b89,0xd757,
0xc10a,0xeb84,0xb974,0x4c9b,
0xc131,0x6286,0x7195,0x0043,
0xc143,0x5255,0x892f,0xf34c,
0xc13e,0xce4b,0x6a11,0xe14a
};
/* logl( sqrt( 2*pi ) ) */
static const unsigned short LS2P[] = {
0x3fed,0x67f1,0xc864,0xbeb5
};
#define LS2PI *(long double *)LS2P
#define MAXLGM 2.556348e305
static unsigned short LPI[4] = {
0x3ff2,0x50d0,0x48e7,0xa1bd,
};
#define LOGPI *(long double *)LPI
#endif


/* Logarithm of gamma function */
/* Reentrant version */ 

long double __lgamma_r(long double x, int* sgngam)
{
long double p, q, u, w, z;
int i;

*sgngam = 1;
#ifdef NANS
if( isnan(x) )
    return(x);
#endif

#ifdef INFINITIES
if( !isfinite(x) )
    return(INFINITY);
#endif

if( x < -34.0L )
    {
    q = -x;
    w = __lgamma_r(q, sgngam); /* note this modifies sgngam! */
    p = floorl(q);
    if( p == q )
        {
lgsinlg:
        _SET_ERRNO(EDOM);
        mtherr( "lgam", SING );
#ifdef INFINITIES
        return (INFINITY);
#else
        return (MAXNUM);
#endif
        }
    i = p;
    if( (i & 1) == 0 )
        *sgngam = -1;
    else
        *sgngam = 1;
    z = q - p;
    if( z > 0.5L )
        {
        p += 1.0L;
        z = p - q;
        }
    z = q * sinl( PI * z );
    if( z == 0.0L )
        goto lgsinlg;
/*	z = logl(PI) - logl( z ) - w;*/
    z = LOGPI - logl( z ) - w;
    return( z );
    }

if( x < 13.0L )
    {
    z = 1.0L;
    p = 0.0L;
    u = x;
    while( u >= 3.0L )
        {
        p -= 1.0L;
        u = x + p;
        z *= u;
        }
    while( u < 2.0L )
        {
        if( u == 0.0L )
            goto lgsinlg;
        z /= u;
        p += 1.0L;
        u = x + p;
        }
    if( z < 0.0L )
        {
        *sgngam = -1;
        z = -z;
        }
    else
        *sgngam = 1;
    if( u == 2.0L )
        return( logl(z) );
    p -= 2.0L;
    x = x + p;
    p = x * polevll( x, B, 5 ) / p1evll( x, C, 6);
    return( logl(z) + p );
    }

if( x > MAXLGM )
    {
    _SET_ERRNO(ERANGE);
    mtherr( "lgamma", OVERFLOW );
#ifdef INFINITIES
    return( *sgngam * INFINITY );
#else
    return( *sgngam * MAXNUM );
#endif
    }

q = ( x - 0.5L ) * logl(x) - x + LS2PI;
if( x > 1.0e8L )
    return( q );

p = 1.0/(x*x);
if( x >= 1000.0L )
    q += ((   7.9365079365079365079365e-4L * p
        - 2.7777777777777777777778e-3L) *p
        + 0.0833333333333333333333L) / x;
else
    q += polevll( p, A, 4 ) / x;
return( q );
}

/* This is the C99 version */

long double lgamma(long double x)
{
  int local_sgngam=0;
  return (__lgamma_r(x, &local_sgngam));
}

main()
{
    printf("%40.20Le", lgamma(0.5));
}