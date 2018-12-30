#include <stdio.h>
#include "complex.h"

/* Sigh, we can't use the letter I.  */
#undef I

#define MAX(x,y) ((x)>(y)?(x):(y))

int I, IBETA, IEXP, IRND, IT, I1, J, K1, K2, K3;
int MAXEXP, MINEXP, N, NEGEP, NGRD;

float A, AIT, ALBETA, B, BETA, DEL, DELX, DELY, EPS, EPSNEG;
float R6, R7, W, X, XL, XMAX, XMIN, XN;
float X1, Y, Z;
float complex CX, CY;

/*
C----------------------------------------------------------------------
C  Mathematical constants
C----------------------------------------------------------------------
*/

int IOUT = 6;
float ZERO = 0.0f;
float ONE16 = 0.0625f;
float HALF = 0.5f;
float ONE = 1.0f;
float ONEP6 = 1.625f;
float THREE = 3.0f;
float FOUR = 4.0f;
float FIVE = 5.0f;
float EIGHT = 8.0f;
float TEN = 10.0f;
float ELEVEN = 11.0f;
float TWELVE = 12.0f;
float SIXTEN = 16.0f;
float TWENTY = 20.0f;
float ALL9 = -999.0E0f;
float THOUS = 1000.0f;

/*
C----------------------------------------------------------------------
C  Statement functions for conversion between integer and float
C----------------------------------------------------------------------
*/
float
CONV (int NDNUM)
{
  return ((float) NDNUM);
}

float complex
CONVC (float X, float Y)
{
  return (X + Y * _Imaginary_I);
}

float complex CZERO = 0.0f + 0.0f * _Imaginary_I;

float 
CONVR (float complex ZDUM)
{
  return (crealf (ZDUM));
}

float 
CONVI (float complex ZDUM)
{
  return (cimagf (ZDUM));
}


float floorf (float);

float 
AINT (float x)
{
  return (floorf (x + 0.5f));
}

/* Random number between 0 and 1.  */

float
REN (K)
{
  double x;
  extern int drand();

  drand (&x);
  x = x - 1.0;
  return ((float) x);
}

extern float logf (float);
extern float fabsf (float);
extern float sqrtf (float);
float expf (float);
extern float cosf (float);
extern float sinf (float);
extern float powf (float, float);
#define LOG logf
#define ABS fabsf
#define SQRT sqrtf
#define EXP expf
#define COS cosf
#define SIN sinf
#define POW powf

/* Functions under test */
extern float cabsf (float complex);
#define FABS cabsf
float complex cexpf (float complex);
#define FEXP cexpf
float complex clogf (float complex);
#define FLOG clogf
float complex cpowf (float complex, float complex);
#define FPOW cpowf
extern float complex csinf (float complex);
#define FSIN csinf
extern float complex ccosf (float complex);
#define FCOS ccosf
extern float complex csqrtf (float complex);
#define FSQRT csqrtf

#define TRUE 1
#define FALSE 0

     void RESET ();
     void TABLAT ();
     void REPORT ();

#define IT_VAL 24
#define XMIN_VAL 7.0e-38
#define XMAX_VAL 1.0e38
