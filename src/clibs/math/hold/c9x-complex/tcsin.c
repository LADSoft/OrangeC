/*
C---------------------------------------------------------------------
C  Program to test CSIN/CCOS
C
C  Accuracy tests are based on the identities
C
C            sin(z) = sin(z-w)*cos(w) + cos(z-w)*sin(w)
C     and
C            cos(z) = cos(z-w)*cos(w) - sin(z-w)*sin(w)
C
C  Data required:
C
C     None
C
C  Subprograms required from this package:
C
C     MACHAR - An environmental inquiry program providing
C              information on the floating point arithmetic
C              system.  Note that the call to MACHAR can
C              be deleted provided the following five
C              parameters are assigned the values indicated:
C
C              IBETA  - the radix of the floating point system;
C              IT     - the number of base-IBETA digits in the
C                       significand of a floating-point number;
C              XMAX   - the largest finite floating-point no.
C
C     REN(K) - a function subprogram returning random real
C              numbers uniformly distributed over (0,1)
C
C     RESET, TABLAT, REPORT - programs to report results.
C
C
C  Generic Fortran subprograms required:
C
C         ABS, AIMAG (or DIMAG), CMPLX (or DCMPLX), COS,
C
C         LOG, MAX, REAL (or DBLE), SIN
C
C
C  Latest revision - October 31, 1990
C
C  Author - W. J. Cody
C           Argonne National Laboratory
C
C----------------------------------------------------------------------
C  Declare named common
C----------------------------------------------------------------------
*/

#include "celefunt.h"

int IFLAG, RFLAG, PFLAG;
int KX1, KX2, KX3, KY1, KY2, KY3;
#ifdef TDOUBLE
double AX, AY, BX, BY, RX6, RX7, RY6, RY7;
double complex CF1, CF2, CF3, CV, CX, CX1, CX2, CX3, CY1, CY2, CY3, CZ,
  CZZ;
#endif
#ifdef TSINGLE
float AX, AY, BX, BY, RX6, RX7, RY6, RY7;
float complex CF1, CF2, CF3, CV, CX, CX1, CX2, CX3, CY1, CY2, CY3, CZ,
  CZZ;
#endif
/*
C----------------------------------------------------------------------
C  Declare local variables
C----------------------------------------------------------------------
*/
#ifdef TDOUBLE
double ALXMAX, BETAP, C;
double complex CC, CS;
#endif
#ifdef TSINGLE
float ALXMAX, BETAP, C;
float complex CC, CS;
#endif
/*
C----------------------------------------------------------------------
C  Mathematical constants
C----------------------------------------------------------------------
*/

#ifdef TDOUBLE
double complex CDEL = 0.0625 + 0.0625 * _Imaginary_I;
/* sin(CDEL) */
double complex SINDEL
= 6.2581348413276935585E-2 + 6.2418588008436587236E-2 * _Imaginary_I;
/* 1 - cos(CDEL) */
double complex COSDEL
= -2.5431314180235545803E-6 - 3.9062493377261771826E-3 * _Imaginary_I;

/* sin(11+12.5i) */
double complex SIN11A = -134167.0E0 + 593.0E0 * _Imaginary_I;
double complex SIN11B
= -3.2928849557976510994E-1 + 7.8989452897413630729E-1 * _Imaginary_I;

/* sin(22+12.5i) */
double complex SIN22A = -1187.0E0 - 134163.0E0 * _Imaginary_I;
double complex SIN22B
= -5.6815858848429427210E-1 - 3.8738909081835398666E-1 * _Imaginary_I;
#endif

#ifdef TSINGLE
float complex CDEL = 0.0625f + 0.0625f * _Imaginary_I;
float complex SINDEL
= 6.2581348413276935585E-2f + 6.2418588008436587236E-2f * _Imaginary_I;
float complex COSDEL
= -2.5431314180235545803E-6f - 3.9062493377261771826E-3f * _Imaginary_I;
float complex SIN11A = -134167.0E0f + 593.0E0f * _Imaginary_I;
float complex SIN11B
= -3.2928849557976510994E-1f + 7.8989452897413630729E-1f * _Imaginary_I;
float complex SIN22A = -1187.0E0f - 134163.0E0f * _Imaginary_I;
float complex SIN22B
= -5.6815858848429427210E-1f - 3.8738909081835398666E-1f * _Imaginary_I;
#endif

/*
C----------------------------------------------------------------------
C  Statement functions for conversion between integer or complex
C  and float
C----------------------------------------------------------------------
*/
/*
C----------------------------------------------------------------------
*/
int
main ()
{
  /*
      CALL MACHAR(IBETA,IT,IRND,NGRD,MACHEP,NEGEP,IEXP,MINEXP,
     1            MAXEXP,EPS,EPSNEG,XMIN,XMAX)
     */
  IBETA = 2;
  IT = IT_VAL;
  XMIN = XMIN_VAL;
  XMAX = XMAX_VAL;
  BETA = CONV (IBETA);
  ALBETA = LOG (BETA);
  AIT = CONV (IT);
  AX = ONE / SIXTEN;
  BX = ONE;
  AY = ZERO;
  BY = ONE;
  N = 2000;
  XN = CONV (N);
  I1 = 0;
  IFLAG = FALSE;
  RFLAG = TRUE;
  PFLAG = FALSE;
  /*
C-----------------------------------------------------------------
C  Random argument accuracy tests
C-----------------------------------------------------------------
*/
  /*    DO 300 J = 1, 3 */
  for (J = 1; J <= 3; J++)
    {
      RESET ();
      DELX = (BX - AX) / XN;
      DELY = (BY - AY);
      XL = AX;
      /*         DO 200 I = 1, N */
      for (I = 1; I <= N; I++)
	{
	  X = DELX * REN (I1) + XL;
	  Y = DELY * REN (I1) + AY;
	  CX = CONVC (X, Y);
	  /*
C----------------------------------------------------------------
C  Carefully purify arguments and evaluate identities
C----------------------------------------------------------------
*/
	  CY = CX - CDEL;
	  CS = FSIN (CY);
	  CC = FCOS (CY);
	  if (J != 3)
	    {
	      CZ = FSIN (CX);
	      CZZ = CS + (CS * COSDEL + CC * SINDEL);
	    }
	  else
	    {
	      CZ = FCOS (CX);
	      CZZ = CC + (CC * COSDEL - CS * SINDEL);
	    }
	  /*
C-----------------------------------------------------------------
C  Accumulate results
C-----------------------------------------------------------------
*/
	  TABLAT ();
	  XL = XL + DELX;
	  /*  200    CONTINUE*/
	}
      /*
C---------------------------------------------------------------------
C  Process and output statistics
C---------------------------------------------------------------------
*/
      if (J != 3)
	printf (" Test of SIN(X) vs SIN(W+D), W = X-D, D = (1/16,1/16)\n");
      else
	printf (" Test of COS(X) vs COS(W+D), W = X-D, D = (1/16,1/16)\n");

      REPORT (IOUT);
      if (J == 1)
	{
	  AX = SIXTEN;
	  BX = AX + ONE;
	}
      AY = AX;
      BY = BX;
      IFLAG = TRUE;
      /*  300 CONTINUE*/
    }
  /*
C-----------------------------------------------------------------
C  Special tests
C-----------------------------------------------------------------
*/
  printf (" Special Tests\n");
  /* C = ONE / BETA ** (IT/2) */
#ifdef TDOUBLE
  C = ONE / POW (BETA, (double) (IT / 2));
#endif
#ifdef TSINGLE
  C = ONE / POW (BETA, (float) (IT / 2));
#endif

  X = ELEVEN;
  Y = X + ONE + HALF;
  X = X + X;
  CX = CONVC (X + C, Y);
  CY = CONVC (X - C, Y);
  CZ = (FSIN (CX) - FSIN (CY)) / (C + C);
  printf (" If (%9.1f,%7.1f) is not almost (-134163,1188),\n",
	  creal (CZ), cimag (CZ));
  printf (" SIN has the wrong period.\n");

  printf (" The identity   SIN(-Z) = -SIN(Z)   will be tested.\n");
  printf ("                X            F(Z) + F(-Z)\n");
  /*      DO 320 I = 1, 5 */
  for (I = 1; I <= 5; I++)
    {
      X = REN (I1) * TEN;
      Y = REN (I1) * TEN;
      CX = CONVC (X, Y);
      CZ = FSIN (CX) + FSIN (-CX);
      printf ("(%15.7E,%15.7E) (%15.7E,%15.7E)\n",
	      creal (CX), cimag (CX), creal (CZ), cimag (CZ));
      /*  320 CONTINUE */
    }
  printf (" Test the significance in the real or imaginary\n");
  printf (" components near\n");
  printf (" the zeros of the real sin or cos.\n");
  X = ELEVEN;
  Y = X + ONE + HALF;
  CX = CONVC (X, Y);
  CZ = FSIN (CX);
  CS = (CZ - SIN11A) - SIN11B;
  C = CONVI (CS) / CONVI (CZ);
  if (C != ZERO)
    W = LOG (ABS (C)) / ALBETA + AIT;
  else
    W = ZERO;

  W = MAX (W, ZERO);
  printf (" SIN(%15.7E,%15.7E) = (%15.7E,%15.7E)\n",
	  creal (CX), cimag (CX), creal (CZ), cimag (CZ));
  printf (" The estimated loss of base %4d digits in the\n", IBETA);
  printf (" imaginary component is %7.2f\n", W);

  if (W < ONE)
    printf (" This is acceptable\n");
  else
    printf (" This is too large\n");

  X = X + X;
  CX = CONVC (X, Y);
  CZ = FSIN (CX);
  CS = (CZ - SIN22A) - SIN22B;
  C = CONVR (CS) / CONVR (CZ);
  if (C != ZERO)
    W = LOG (ABS (C)) / ALBETA + AIT;
  else
    W = ZERO;

  W = MAX (W, ZERO);
  printf (" SIN(%15.7E,%15.7E) = (%15.7E,%15.7E)\n",
	  creal (CX), cimag (CX), creal (CZ), cimag (CZ));
  printf (" The estimated loss of base %4d digits in the\n", IBETA);
  printf (" real component is %7.2f\n", W);
  if (W < ONE)
    printf (" This is acceptable\n");
  else
    printf (" This is too large\n");
  /*
C-----------------------------------------------------------------
C  Test of error returns
C-----------------------------------------------------------------
*/
  printf (" Test of error returns\n");
  ALXMAX = LOG (XMAX) - HALF;
  CX = CONVC (ONE, ALXMAX);
  printf (" SIN will be called with the argument (%15.4E,%15.4E)\n",
	  creal (CX), cimag (CX));
  printf (" This should not trigger an error message\n");
  CY = FSIN (CX);
  printf (" SIN returned the value (%15.4E,%15.4E)\n", creal (CY), cimag (CY));
  /*BETAP = BETA ** IT */
#ifdef TDOUBLE
  BETAP = POW (BETA, (double) IT);
#endif
#ifdef TSINGLE
  BETAP = POW (BETA, (float) IT);
#endif
  CX = CONVC (BETAP, ONE);
  printf (" SIN will be called with the argument (%15.4E,%15.4E)\n",
	  creal (CX), cimag (CX));
  printf (" This should trigger an error message\n");
  CY = FSIN (CX);
  printf (" SIN returned the value (%15.4E,%15.4E)\n", creal (CY), cimag (CY));
  CX = CONVC (ONE, BETAP);
  printf (" SIN will be called with the argument (%15.4E,%15.4E)\n",
	  creal (CX), cimag (CX));
  printf (" This should trigger an error message\n");
  CY = FSIN (CX);
  printf (" SIN returned the value (%15.4E,%15.4E)\n", creal (CY), cimag (CY));
  printf (" This concludes the tests\n");
  exit (0);
}

/*
C----------------------------------------------------------------
*/
/*
C---------- Last card of CSIN/CCOS Test Program ----------
*/
#include "tablat.c"
