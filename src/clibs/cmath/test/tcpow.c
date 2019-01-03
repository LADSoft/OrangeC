/*
C----------------------------------------------------------------------
C  Program to test complex power (exponentiation)
C
C  Accuracy tests are based on the identity
C
C            z = z ** (1,0)
C
C            z * z = Z ** (2,0)
C     and
C            z ** w = (z*z) ** (w/2)
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
C              MAXEXP - the smallest integer power of FLOAT(IBETA)
C                       that causes overflow;
C              XMIN   - the smallest non-vanishing floating-point
C                       power of the radix.
C
C     REN(K) - a function subprogram returning random real
C              numbers uniformly distributed over (0,1)
C
C     RESET, TABLAT, REPORT - programs to report results.
C
C
C  Generic Fortran subprograms required:
C
C         ABS, CMPLX (or DCMPLX), LOG, REAL (or DBLE)
C
C  Latest revision - November 29, 1990
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
#ifdef TDOUBLE
double complex CONE = 1.0 + 0.0 * _Imaginary_I;
double complex CTWO = 2.0 + 0.0 * _Imaginary_I;
double C1 = 0.95e0;
double C2 = 1.05e0;
#endif
#ifdef TSINGLE
float complex CONE = 1.0f + 0.0f * _Imaginary_I;
float complex CTWO = 2.0f + 0.0f * _Imaginary_I;
float C1 = 0.95e0f;
float C2 = 1.05e0f;
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
  I1 = IT / 2 + 4;
  /*      SCALE = BETA ** I1 */
#ifdef TDOUBLE
  SCALE = POW (BETA, (double) I1);
#endif
#ifdef TSINGLE
  SCALE = POW (BETA, (float) I1);
#endif
  AX = ONE;
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
C  Carefully purify arguments and evaluate identities
C-----------------------------------------------------------------
*/
	  CX = CONVC (X, Y);
	  if (J == 1)
	    {
	      CX = CONVC (X, Y);
	      CZ = FPOW (CX, CONE);
	      CZZ = CX;
	    }
	  else if (J == 2)
	    {
	    label100:
	      W = ABS (Y / X);
	      if ((W > C1) && (W < C2))
		{
		  Y = DELY * REN (I1) + AY;
		  goto label100;
		}
	      CX = CONVC (X, Y);
	      CZ = CX * SCALE;
	      CY = CX + CZ;
	      CX = CY - CZ;
	      CZ = FPOW (CX, CTWO);
	      CZZ = CX * CX;
	    }
	  else if (J == 3)
	    {
	      CX = CONVC (X, Y);
	      X = DELY * REN (I1) + AX;
	      Y = DELY * REN (I1) + AY;
	      CZ = CX * SCALE;
	      CY = CX + CZ;
	      CX = CY - CZ;
	      CY = CONVC (X, Y);
	      CV = CY;
	      CZ = FPOW (CX, CY);
	      CZZ = FPOW (CX * CX, CY * HALF);
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
	printf (" Test of Z ** (1,0) vs Z\n");
      else if (J == 2)
	printf (" Test of Z ** (2,0) vs Z*Z\n");
      else
	{
	  printf (" Test of Z**W vs (Z*Z) ** (W/2)\n");
	  printf (" *****************************************************\n");
	  printf (" * Very large MRE values are the norm for this test; *\n");
	  printf (" * the frequency of exact agreement is meaningful.   *\n");
	  printf (" *****************************************************\n");

	}
      REPORT (IOUT);
      if (J == 2)
	{
	  PFLAG = TRUE;
	  AX = FOUR;
	  AY = AX;
	}
      /*  300 CONTINUE */
    }
  /*
C-----------------------------------------------------------------
C  Special tests
C-----------------------------------------------------------------
*/
  printf (" Special tests\n");
  printf (" The identity  Z**W = (1/Z)**(-W)  will be tested.\n");
  printf ("          Z           W          (Z**W - (1/Z)**(-W))/CABS(Z**W)\n");

  /*      DO 320 I = 1, 5 */
  for (I = 1; I <= 5; I++)
    {
      X = REN (I1) * TEN;
      Y = REN (I1) * TEN;
      CX = CONVC (X, Y);
      X = REN (I1) * TEN;
      Y = REN (I1) * TEN;
      CZ = CONVC (X, Y);
      CY = ONE / CX;
      CZZ = (FPOW (CX, CZ) - FPOW (CY, -CZ)) / FABS (FPOW (CX, CZ));
      printf ("(%11.4E,%11.4E) (%11.4E,%11.4E) , (%15.7E,%15.7E)\n",
	      creal (CX), cimag (CX), creal (CZ), cimag (CZ), creal (CZZ),
	      cimag (CZZ));
      /*  320 CONTINUE*/
    }
  printf (" Test near an extreme argument.  This should not\n");
  printf (" but may\n trigger an error message\n");
  Y = BETA * XMIN;
  CX = CONVC (BETA, Y);
  W = CONV (MAXEXP - 1);
  CY = CONVC (W, ZERO);
  CZ = FPOW (CX, CY);
  X = BETA;
  /*      DO 330 I = 3, MAXEXP */
  for (I = 3; I <= MAXEXP; I++)
    {
      X = X * BETA;
      /*  330 CONTINUE*/
    }
  CZZ = X * CONVC (ONE, XMIN * W);
  printf (" (BETA,BETA*XMIN)**(MAXEXP-1,0)       = \n");
  printf ("(%15.7E,%15.7E)\n", creal (CZ), cimag (CZ));
  printf (" BETA**(MAXEXP-1)*(1,(MAXEXP-1)*XMIN) = ");
  printf ("(%15.7E,%15.7E)\n", creal (CZZ), cimag (CZZ));
  CZ = CZ - CZZ;
  printf (" Difference                           = (%15.7E,%15.7E)\n",
	  creal (CZ), cimag (CZ));
  /*
C-----------------------------------------------------------------
C  Test of error returns
C-----------------------------------------------------------------
*/
  printf (" Test of error returns\n");
  printf (" Test of (0,0) ** (0,0)\n");
  printf (" This should trigger an error message\n");
  CX = CONVC (ZERO, ZERO);
  CZ = FPOW (CX, CX);
  printf (" Test returned the value (%15.4E,%15.4E)\n", creal (CZ), cimag (CZ));
  printf (" This concludes the tests\n");
  exit (0);
}

/*
C---------------------------------------------------------------------
*/
/*
C---------- Last card of CPOW Test Program ----------
*/
#include "tablat.c"
