/*
C----------------------------------------------------------------------
C  Program to test CEXP
C
C  Accuracy tests are based on the identity
C
C            exp(z) = exp(z-w)*exp(w)
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
C              be deleted provided the following four
C              parameters are assigned the values indicated:
C
C              IBETA  - the radix of the floating point system;
C              IT     - the number of base-IBETA digits in the
C                       significand of a floating-point number;
C              XMIN   - the smallest non-vanishing floating-point
C                       power of the radix;
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
C         AIMAG (or DIMAG), AINT, CMPLX (or DCMPLX), EXP,
C
C         LOG, REAL (or DBLE), SQRT
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
double AX, AY, BX, BY, RX6, RX7, RY6, RY7, R6, R7, XN;
double complex CF1, CF2, CF3, CV, CX, CX1, CX2, CX3, CY1, CY2, CY3, CZ,
  CZZ;
#endif

#ifdef TSINGLE
float AX, AY, BX, BY, RX6, RX7, RY6, RY7, R6, R7, XN;
float complex CF1, CF2, CF3, CV, CX, CX1, CX2, CX3, CY1, CY2, CY3, CZ,
  CZZ;
#endif

/*
C----------------------------------------------------------------------
C  Declare local variables
C----------------------------------------------------------------------
*/


/*
C----------------------------------------------------------------------
C  Mathematical constants
C----------------------------------------------------------------------
*/
#ifdef TDOUBLE
double complex EXPDEL = 6.2416044877018563681e-2
+ 6.6487597751003112768e-2 * _Imaginary_I;
double complex CDEL = 0.0625 + 0.0625 * _Imaginary_I;
#endif

#ifdef TSINGLE
float complex EXPDEL = 6.2416044877018563681e-2f
+ 6.6487597751003112768e-2f * _Imaginary_I;
float complex CDEL = 0.0625f + 0.0625f * _Imaginary_I;
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
  AX = ZERO;
  BX = ONE;
  AY = ONE16;
  BY = BX;
  N = 2000;
  XN = CONV (N);
  I1 = 0;
  RFLAG = TRUE;
  IFLAG = TRUE;
  PFLAG = FALSE;
  /*
C---------------------------------------------------------------------
C  Random argument accuracy tests
C---------------------------------------------------------------------
*/
  /* DO 300 J = 1, 3 */
  for (J = 1; J <= 3; J++)
    {
      RESET ();
      DELX = (BX - AX) / XN;
      DELY = (BY - AY);
      XL = AX;
      /* DO 200 I = 1, N */
      for (I = 1; I <= N; I++)
	{
	  X = DELX * REN (I1) + XL;
	  Y = DELY * REN (I1) + AY;
	  /*
C-----------------------------------------------------------------
C  Carefully purify arguments and evaluate identities
C-----------------------------------------------------------------
*/
	  CX = CONVC (X, Y);
	  CY = CX - CDEL;
	  CZ = FEXP (CX);
	  CZZ = FEXP (CY);
	  CZZ = CZZ + CZZ * EXPDEL;
	  /*
C-----------------------------------------------------------------
C  Accumulate results
C-----------------------------------------------------------------
*/
	  TABLAT ();
	  XL = XL + DELX;
	}
      /*  200    CONTINUE */
      /*
C-----------------------------------------------------------------
C  Process and output statistics
C-----------------------------------------------------------------
*/
      printf (" Test of EXP(Z) vs EXP(Z-DEL)EXP(DEL), DEL = \n");
      printf ("(1/16,1/16)\n");

      REPORT (IOUT);
      if (J == 1)
	{
	  AX = ONEP6;
	  BX = THREE;
	}
      else
	{
	  AX = SIXTEN;
	  BX = AX + ONE;
	}
      AY = AX;
      BY = BX;
      /*  300 CONTINUE */
    }
  /*
C---------------------------------------------------------------------
C  Special tests
C---------------------------------------------------------------------
*/
  printf (" Special tests\n");
  printf (" The identity  EXP(Z)*EXP(-Z) = 1.0  will be tested.\n");
  printf ("        Z                        F(Z)*F(-Z) - 1\n");

  /* DO 320 I = 1, 5 */
  for (I = 1; I <= 5; I++)
    {
      X = REN (I1) * BETA;
      Y = REN (I1) * BETA;
      CX = CONVC (X, Y);
      CY = -CX;
      CZ = FEXP (CX) * FEXP (CY) - ONE;
      printf ("(%15.7E,%15.7E) (%15.7E,%15.7E)\n",
	      creal (CX), cimag (CX), creal (CZ), cimag (CZ));
      /*  320 CONTINUE*/
    }
  printf (" Test of special arguments\n");
  CX = CONVC (ZERO, ZERO);
  CZ = FEXP (CX) - ONE;
  printf (" EXP(0.0,0.0) - 1.0E0 = (%15.7E,%15.7E)\n",
	  creal (CZ), cimag (CZ));
  printf (" Test cos(y), sin(y), and exp(x)\n");
  CX = CONVC (ZERO, TEN);
  CZ = FEXP (CX);
  X = COS (TEN);
  Y = SIN (TEN);
  W = (CONVR (CZ) - X) / X;
  printf (" [REAL(EXP(0,10)) - COS(10)]/COS(10) = %15.7E\n", W);

  W = (CONVI (CZ) - Y) / Y;
  printf (" [IMAG(EXP(0,10)) - SIN(10)]/SIN(10) = %15.7E\n", W);
  CX = CONVC (TEN, ZERO);
  CZ = FEXP (CX);
  X = EXP (TEN);
  W = (CONVR (CZ) - X) / X;
  printf (" [REAL(EXP(10,0)) - EXP(10)]/EXP(10) = %15.7E\n", W);
  printf (" Tests near extreme arguments\n");
  printf (" The real part of the first result below may underflow.\n");
  X = AINT (LOG (XMIN));
  CX = CONVC (X, ONE);
  CZ = FEXP (CX);
  printf (" EXP(%13.6E,%13.6E) = (%13.6E,%13.6E)\n",
	  creal (CX), cimag (CX), creal (CZ), cimag (CZ));
  Y = SQRT (XMAX);
  X = AINT (LOG (XMAX));
  CX = CONVC (X, ONE);
  CZ = FEXP (CX);
  printf (" EXP(%13.6E,%13.6E) = (%13.6E,%13.6E)\n",
	  creal (CX), cimag (CX), creal (CZ), cimag (CZ));
  CX = CONVC (X * HALF, TEN);
  CY = CX * HALF;
  CZ = FEXP (CX);
  CZZ = FEXP (CY);
  CZZ = CZZ * CZZ;
  printf (" If EXP(%13.6E,%13.6E) = (%13.6E,%13.6E)\n",
	  creal (CX), cimag (CX), creal (CZ), cimag (CZ));
  printf (" is not about\n EXP(%13.6E,%13.6E)**2 = \n",
	  creal (CY), cimag (CY));
  printf (" (%13.6E,%13.6E) there is an\n argument reduction error\n",
	  creal (CZZ), cimag (CZZ));

  /*
C---------------------------------------------------------------------
C  Test of error returns
C---------------------------------------------------------------------
*/
  printf (" Test of error returns\n");
  X = -ONE / SQRT (XMIN);
  CX = CONVC (X, ONE);
  printf (" EXP will be called with the argument (%15.4E,%15.4E)\n",
	  creal (CX), cimag (CX));
  printf (" The real component of the argument is so negative\n");
  printf (" that the real exponential should underflow to zero.\n");
  printf (" Thus, the result should be (0,0).\n");
  CZ = FEXP (CX);
  printf (" EXP returned the value (%15.4E,%15.4E)\n",
	  creal (CZ), cimag (CZ));
  Y = SQRT (XMAX);
  CX = CONVC (TEN, Y);
  printf (" EXP will be called with the argument (%15.4E,%15.4E)\n",
	  creal (CX), cimag (CX));
  printf (" The imaginary component of the argument is too large\n");
  printf (" for the sin and cos computations to make sense.\n");
  CZ = FEXP (CX);
  printf (" EXP returned the value (%15.4E,%15.4E)\n",
	  creal (CZ), cimag (CZ));
  CX = CONVC (-X, -ONE);
  printf (" EXP will be called with the argument (%15.4E,%15.4E)\n",
	  creal (CX), cimag (CX));
  printf (" The real component of the argument is so large\n");
  printf (" that the real exponential should overflow.  The\n");
  printf (" result should be (inf,inf) with an error return.\n");
  CZ = FEXP (CX);
  printf (" EXP returned the value (%15.4E,%15.4E)\n",
	  creal (CZ), cimag (CZ));
  printf (" This concludes the tests\n");
  exit (0);
}

/*
C---------------------------------------------------------------------
*/
/*
C---------- Last card of CEXP Test Program ----------
*/

#include "tablat.c"
