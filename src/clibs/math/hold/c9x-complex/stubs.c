
/* These functions are, or pretend to be, from the cephes library.  */

#include <stdio.h>
#include "mconf.h"
int dprec() {return 0;}
int ldprec() {return 0;}

double MINLOG = -170.0;
double MAXLOG = +170.0;
double PI = 3.14159265358979323846;
double PIO2 = 1.570796326794896619;
double MAXNUM = 1.0e308;
double MACHEP = 1.1e-16;
double INFINITY = 1.0/0.0;
long double PIL = 3.141592653589793238462643383279502884197169L;
long double PIO2L = 1.570796326794896619231321691639751442098585L;

#if 1
/* For Intel x86 or Motorola 68881 */
/* almost 2^16384 */
long double MAXNUML = 1.189731495357231765021263853E4932L;
/* 2^-64 */
long double MACHEPL = 5.42101086242752217003726400434970855712890625E-20L;
#else
/* For IEEE quad precision */
/* (1 - 2^-113) 2^16384 */
long double MAXNUML = 1.189731495357231765085759326628007016196469e4932L;
/* 2^-113 */
long double MACHEPL = 9.629649721936179265279889712924636592690508e-35L;
#endif

int sprec() {return 0;}
float PIF = 3.14159265358979323846F;
float PIO2F =  1.570796326794896619F;
float MAXNUMF = 1.0e38F;
float MACHEPF = 3.0e-8F;

int
mtherr( str, val )
char *str;
int val;
{
  printf ("Math error %s, value %d\n", str, val);
  return val;
}

static int sx = 1;
static int sy = 10000;
static int sz = 3000;

static union {
 double d;
 unsigned short s[4];
} unkans;

static int ranwh()
{
int r, s;

r = sx/177;
s = sx - 177 * r;
sx = 171 * s - 2 * r;
if( sx < 0 )
  sx += 30269;

r = sy/176;
s = sy - 176 * r;
sy = 172 * s - 35 * r;
if( sy < 0 )
  sy += 30307;

r = sz/178;
s = sz - 178 * r;
sz = 170 * s - 63 * r;
if( sz < 0 )
  sz += 30323;
return 0;
}


int drand( a )
double *a;
{
unsigned short r;
#ifdef DEC
unsigned short s, t;
#endif

/* This algorithm of Wichmann and Hill computes a floating point
 * result:
 */
ranwh();
unkans.d = sx/30269.0  +  sy/30307.0  +  sz/30323.0;
r = unkans.d;
unkans.d -= r;
unkans.d += 1.0;

/* if UNK option, do nothing further.
 * Otherwise, make a random 16 bit integer
 * to overwrite the least significant word
 * of unkans.
 */
#ifdef UNK
/* do nothing */
#else
ranwh();
r = sx * sy + sz;
#endif
#ifdef DEC
/* To make the numbers as similar as possible
 * in all arithmetics, the random integer has
 * to be inserted 3 bits higher up in a DEC number.
 * An alternative would be put it 3 bits lower down
 * in all the other number types.
 */
s = unkans.s[2];
t = s & 07;/* save these bits to put in at the bottom */
s &= 0177770;
s |= (r >> 13) & 07;
unkans.s[2] = s;
t |= r << 3;
unkans.s[3] = t;
#endif

#ifdef IBMPC
unkans.s[0] = r;
#endif

#ifdef MIEEE
unkans.s[3] = r;
#endif

*a = unkans.d;
return 0;
}
