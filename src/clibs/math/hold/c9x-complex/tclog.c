/*
C----------------------------------------------------------------------
C  Program to test CLOG
C
C  Accuracy tests are based on the identity
C
C            log(z) = log(z*z)/2
C     and
C            log(z) = log(17z/16) - log(17/16)
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
C         CMPLX (or DCMPLX), LOG, REAL (or DBLE), SQRT
C
C  The program as given here must be modified before compiling.
C   If a single (double) precision version is desired, change all
C   occurrences of CS (CD) in columns 1 and 2 to blanks.  For a
C   calibration version, change all occurrences of CS and of CC
C   in columns 1 and 2 to blanks.
C
C  Latest revision - November 20, 1990
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
int IEXP, IOUT, I1, J;
#ifdef TDOUBLE
double SCALE;
#endif
#ifdef TSINGLE
float SCALE;
#endif
/*
C----------------------------------------------------------------------
C  Mathematical constants
C----------------------------------------------------------------------
*/
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
  I1 = IT / 2 + 4;
  /*      SCALE = BETA ** I1; */
#ifdef TDOUBLE
  SCALE = POW (BETA, (double) I1);
#endif
#ifdef TSINGLE
  SCALE = POW (BETA, (float) I1);
#endif

  AX = ONE + ONE;
  BX = TEN;
  AY = ZERO;
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
  /*      DO 300 J = 1, 3 */
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
	  /*
C-----------------------------------------------------------------
C  Purify argument and evaluate identities
C-----------------------------------------------------------------
*/
	  Z = X * SCALE;
	  W = Z + X;
	  X = W - Z;
	  Z = Y * SCALE;
	  W = Z + Y;
	  Y = W - Z;
	  CX = CONVC (X, Y);
	  Z = X * X - Y * Y;
	  W = X * Y;
	  CY = CONVC (Z, W + W);
	  CZ = FLOG (CX);
	  CZZ = FLOG (CY);
	  CZZ = CZZ * HALF;
	  /*
C-----------------------------------------------------------------
C  Accumulate results
C-----------------------------------------------------------------
*/
	  TABLAT ();
	  XL = XL + DELX;
	  /*  200    CONTINUE */
	}
      /*
C-----------------------------------------------------------------
C  Process and output statistics
C-----------------------------------------------------------------
*/
      printf (" Test of LOG(Z) vs LOG(Z*Z)/2\n");
      REPORT (IOUT);
      if (J == 1)
	{
	  AX = THOUS;
	  BX = AX + AX;
	  AY = -AX;
	  BY = -BX - BX;
	}
      else
	{
	  AX = EPS;
	  BX = HALF * HALF;
	  AY = -AX;
	  BY = -BX;
	}
      /*  300 CONTINUE*/
    }
  /*
C-----------------------------------------------------------------
C  Special tests
C-----------------------------------------------------------------
*/
  printf (" Special tests\n");
  printf (" The identity  LOG(Z) = -LOG(1/Z)  will be tested\n");
  printf ("               Z                      LOG(Z) + LOG(1/Z)\n");
  /*      DO 320 I = 1, 5*/
  for (I = 1; I <= 5; I++)
    {
      X = REN (I1) * TEN;
      Y = REN (I1) * TEN;
      CX = CONVC (X, Y);
      CY = ONE / CX;
      CZ = FLOG (CX) + FLOG (CY);
      printf ("(%15.7E,%15.7E) (%15.7E,%15.7E)\n",
	      creal (CX), cimag (CX), creal (CZ), cimag (CZ));
      /*  320 CONTINUE*/
    }
  printf (" Test of special arguments\n");
  CX = CONVC (ONE, ZERO);
  CZ = FLOG (CX);
  printf (" LOG(1.0,0.0) = (%15.7E,%15.7E)\n", creal (CZ), cimag (CZ));
  printf (" Tests near extreme arguments.  These should not\n");
  printf (" but may\n trigger error messages\n");
  CX = CONVC (XMIN, XMIN);
  printf (" LOG will be called with the argument ");
  printf (" (%15.4E,%15.4E)\n", creal (CX), cimag (CX));
  CZ = FLOG (CX);
  printf (" LOG(XMIN,XMIN) = LOG(%15.7E,%15.7E) =\n", creal (CX), cimag (CX));
  printf ("         (%15.7E,%15.7E)\n", creal (CZ), cimag (CZ));
  CX = CONVC (XMAX, XMAX);
  printf (" LOG will be called with the argument ");
  printf (" (%15.4E,%15.4E)\n", creal (CX), cimag (CX));
  CZ = FLOG (CX);
  printf (" LOG(XMAX,XMAX) = LOG(%15.7E,%15.7E) =\n", creal (CX), cimag (CX));
  printf ("         (%15.7E,%15.7E)\n", creal (CZ), cimag (CZ));

  /*
C-----------------------------------------------------------------
C  Test of error returns
C-----------------------------------------------------------------
*/
  printf (" Test of error returns\n");
  CX = CONVC (ZERO, ZERO);
  printf (" LOG will be called with the argument (%15.4E,%15.4E)\n",
	  creal (CX), cimag (CX));
  printf (" This should trigger an error message\n");
  CZ = FLOG (CX);
  printf (" LOG returned the value (%15.4E,%15.4E)\n", creal (CZ), cimag (CZ));

  printf (" This concludes the tests\n");
  exit (0);
}

/*
C---------------------------------------------------------------------
*/
/*
C---------- Last card of CLOG Test Program ----------
*/

#include "tablat.c"
