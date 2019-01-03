/*
C----------------------------------------------------------------------
C  Program to test CSQRT
C
C  Accuracy tests are based on the identity
C
C            z = sqrt(z*z)
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
C         ABS, CMPLX (or DCMPLX), LOG, REAL (or DBLE), SQRT
C
C  The program as given here must be modified before compiling.
C   If a single (double) precision version is desired, change all
C   occurrences of CS (CD) in columns 1 and 2 to blanks.  For a
C   calibration version, change all occurrences of CS and of CC
C   in columns 1 and 2 to blanks.
C
C  Latest revision - November 15, 1990
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
  /* SCALE = BETA ** I1; */
#ifdef TDOUBLE
  SCALE = POW (BETA, (double) I1);
#endif
#ifdef TSINGLE
  SCALE = POW (BETA, (float) I1);
#endif
  AX = ZERO;
  BX = TEN;
  AY = AX;
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
  /*      DO 300 J = 1, 2*/
  for (J = 1; J <= 2; J++)
    {
      RESET ();
      DELX = (BX - AX) / XN;
      DELY = BY - AY;
      XL = AX;
      /*         DO 200 I = 1, N */
      for (I = 1; I <= N; I++)
	{
	  X = DELX * REN (I1) + XL;
	  Y = DELY * REN (I1);
/*
C-----------------------------------------------------------------
C  Carefully purify arguments and evaluate identities
C-----------------------------------------------------------------
*/
	  Z = X * SCALE;
	  W = Z + X;
	  X = W - Z;
	  Z = Y * SCALE;
	  W = Z + Y;
	  Y = W - Z;
	  CZZ = CONVC (X, Y);
	  Z = X * X - Y * Y;
	  W = X * Y;
	  /* For TABLAT, the function argument must be named CX.  */
	  CX = CONVC (Z, W + W);
	  if (J == 1)
	    CZ = FSQRT (CX);
	  else
	    {
	      CZ = -FSQRT (CX);
#if 0
	      /* This did not seem right in the Fortran program.
		 The question is, Is CZ a square root of CX?
		 Fix so TABLAT compares the square of CZ with CX.  */
	      CZZ = CX;
	      CX = CZ;
	      CZ = CZ * CZ;
#endif
	    }
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
      if (J == 1)
	printf (" Test of SQRT(Z*Z) vs Z\n");
      else
	printf (" Test of -SQRT(Z*Z) vs Z\n");

      REPORT (IOUT);
      BX = -BX * BX;
      BY = -BX;
      /*  300 CONTINUE */
    }
/*
C-----------------------------------------------------------------
C  Special tests
C-----------------------------------------------------------------
*/
  printf (" Test of special arguments\n");
  CX = CONVC (ONE, ZERO);
  CZ = FSQRT (CX);
  printf (" SQRT(1.0,0.0) = (%15.7E,%15.7E)\n", creal (CZ), cimag (CZ));
  printf (" Tests near extreme arguments.  These should not\n");
  printf (" but may\n trigger error messages\n");
  CX = CONVC (XMIN, XMIN);
  printf (" SQRT will be called with the argument (%15.4E,%15.4E)\n",
	  creal (CX), cimag (CX));
  CZ = FSQRT (CX);
  printf (" SQRT(XMIN,XMIN) = SQRT(%15.7E,%15.7E) = (%15.7E,%15.7E)\n",
	  creal (CX), cimag (CX), creal (CZ), cimag (CZ));
  CX = CONVC (XMAX, XMAX);
  printf (" SQRT will be called with the argument (%15.4E,%15.4E)\n",
	  creal (CX), cimag (CX));
  CZ = FSQRT (CX);
  printf (" SQRT(XMAX,XMAX) = SQRT(%15.7E,%15.7E) = (%15.7E,%15.7E)\n",
	  creal (CX), cimag (CX), creal (CZ), cimag (CZ));
  printf (" This concludes the tests\n");
  exit (0);
}

/*
C---------------------------------------------------------------------
*/
/*
C--------- Last card of CSQRT Test Program ----------
*/

#include "tablat.c"
