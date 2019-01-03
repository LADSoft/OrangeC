#include <stdio.h>
#include "complex.h"

/* Sigh, we can't use the letter I.  */
#undef I

#define MAX(x,y) ((x)>(y)?(x):(y))

int I, IBETA, IEXP, IRND, IT, I1, J, K1, K2, K3;
int MAXEXP, MINEXP, N, NEGEP, NGRD;

double A, AIT, ALBETA, B, BETA, DEL, DELX, DELY, EPS, EPSNEG;
double R6, R7, W, X, XL, XMAX, XMIN, XN;
double X1, Y, Z;
double complex CX, CY;

/*
C----------------------------------------------------------------------
C  Mathematical constants
C----------------------------------------------------------------------
*/

int IOUT = 6;
double ZERO = 0.0;
double ONE16 = 0.0625;
double HALF = 0.5;
double ONE = 1.0;
double ONEP6 = 1.625;
double THREE = 3.0;
double FOUR = 4.0;
double FIVE = 5.0;
double EIGHT = 8.0;
double TEN = 10.0;
double ELEVEN = 11.0;
double TWELVE = 12.0;
double SIXTEN = 16.0;
double TWENTY = 20.0;
double ALL9 = -999.0E0;
double THOUS = 1000.0;

/*
C----------------------------------------------------------------------
C  Statement functions for conversion between integer and float
C----------------------------------------------------------------------
*/
double
CONV (int NDNUM)
{
  return ((double) NDNUM);
}

double complex
CONVC (double X, double Y)
{
  return (X + Y * _Imaginary_I);
}

double complex CZERO = 0.0 + 0.0 * _Imaginary_I;

double 
CONVR (double complex ZDUM)
{
  return (creal (ZDUM));
}

double 
CONVI (double complex ZDUM)
{
  return (cimag (ZDUM));
}


double floor (double);

double 
AINT (double x)
{
  return (floor (x + 0.5));
}

/* Random number between 0 and 1.  */

double
REN (K)
{
  double x;
  extern int drand();

  drand (&x);
  x = x - 1.0;
  return x;
}

extern double log (double);
extern double fabs (double);
extern double sqrt (double);
double exp (double);
extern double cos (double);
extern double sin (double);
extern double pow (double, double);
#define LOG log
#define ABS fabs
#define SQRT sqrt
#define EXP exp
#define COS cos
#define SIN sin
#define POW pow

/* Functions under test */
extern double cabs (double complex);
#define FABS cabs
double complex cexp (double complex);
#define FEXP cexp
double complex clog (double complex);
#define FLOG clog
double complex cpow (double complex, double complex);
#define FPOW cpow
extern double complex csin (double complex);
#define FSIN csin
extern double complex ccos (double complex);
#define FCOS ccos
extern double complex csqrt (double complex);
#define FSQRT csqrt

#define TRUE 1
#define FALSE 0

     void RESET ();
     void TABLAT ();
     void REPORT ();

#define IT_VAL 53
#define XMIN_VAL 1.0e-308
#define XMAX_VAL 1.0e308
