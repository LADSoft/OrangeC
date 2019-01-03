/*
C      ALGORITHM 714, COLLECTED ALGORITHMS FROM ACM.
C      THIS WORK PUBLISHED IN TRANSACTIONS ON MATHEMATICAL SOFTWARE,
C      VOL. 19, NO. 1, MARCH, 1993, PP. 1-21.
C----------------------------------------------------------------------
C  Program to test CABS
C
C  Accuracy tests are based on the identities
C
C            cabs((3x,4x)) = 5x
C     and
C            cabs((5x,12x)) = 13x
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
C              be deleted provided the following six
C              parameters are assigned the values indicated:
C
C              IBETA  - the radix of the floating point system;
C              IT     - the number of base-IBETA digits in the
C                       significand of a floating-point number;
C              XMIN   - The smallest non-vanishing floating-point
C                       power of the radix;
C              XMAX   - The largest finite floating-point no.
C
C     REN(K) - a function subprogram returning random real
C              numbers uniformly distributed over (0,1)
C
C
C  Generic Fortran subprograms required:
C
C     ABS, LOG, MAX, REAL (or DBLE), SQRT
C
C  The program as given here must be modified before compiling.
C   If a single (double) precision version is desired, change all
C   occurrences of CS (CD) in columns 1 and 2 to blanks.  For a
C   calibration version, change all occurrences of CS and of CC
C   in columns 1 and 2 to blanks.
C
C  Latest revision - November 7, 1990
C
C  Author - W. J. Cody
C           Argonne National Laboratory
C
C----------------------------------------------------------------------
*/

#include "celefunt.h"

/*
C----------------------------------------------------------------------
*/
int
main ()
{
  /*
     CALL MACHAR(IBETA,IT,IRND,NGRD,MACHEP,NEGEP,IEXP,MINEXP, MAXEXP,EPS,EPSNEG,XMIN,XMAX)
  */
  IBETA = 2;
  IT = IT_VAL;
  XMIN = XMIN_VAL;
  XMAX = XMAX_VAL;
  BETA = CONV (IBETA);
  ALBETA = LOG (BETA);
  AIT = CONV (IT);
  A = ONE;
  B = TWENTY;
  N = 2000;
  XN = CONV (N);
  DEL = (B - A) / XN;
  I1 = 0;
  /*
C-----------------------------------------------------------------
C  Random argument accuracy tests
C-----------------------------------------------------------------
*/
  /*      DO 300 J = 1, 2 */
  for (J = 1; J <= 2; J++)
    {
      K1 = 0;
      K3 = 0;
      X1 = ZERO;
      R6 = ZERO;
      R7 = ZERO;
      XL = A;
      /* DO 200 I = 1, N*/
      for (I = 1; I <= N; I++)
	{
	  X = DEL * REN (I1) + XL;
	  if (J == 1)
	    {
	      Y = X * EIGHT;
	      Z = X + Y;
	      X = Z - Y;
	      Y = X * FIVE;
	      CX = CONVC (THREE * X, FOUR * X);
	    }
	  else
	    {
	      Y = X * (SIXTEN + SIXTEN);
	      Z = X + Y;
	      X = Z - Y;
	      Y = X * TWELVE + X;
	      CX = CONVC (FIVE * X, TWELVE * X);
	    }
	  Z = FABS (CX);
	  W = (Z - Y) / Z;
	  /*
C-----------------------------------------------------------------------
C  Accumulate results
C-----------------------------------------------------------------------
*/
	  if (W > ZERO)
	    K1 = K1 + 1;
	  else
	    {
	      if (W < ZERO)
		K3 = K3 + 1;
	    }
	  W = ABS (W);
	  if (W > R6)
	    {
	      R6 = W;
	      X1 = X;
	    }
	  R7 = R7 + W * W;
	  XL = XL + DEL;
	  /*  200    CONTINUE */
	}
      /*
C-----------------------------------------------------------------------
C  Process and output statistics
C-----------------------------------------------------------------------
*/
      K2 = N - K1 - K3;
      R7 = SQRT (R7 / XN);
      if (J == 1)
	printf ("Test of CABS((3X,4X)) vs 5X\n");
      else
	printf ("\n\n");
      printf (" Test of CABS((5X,12X)) vs 13X\n");
      printf (" %d random values of X were drawn from the interval \n", N);
      printf ("      (%15.4E,%15.4E)\n", A, B);
      printf (" CABS was larger %6d times,\n", K1);
      printf ("             agreed %6d times, and\n", K2);
      printf ("        was smaller %6d times\n", K3);
      printf (" There are %4d base %4d\n", IT, IBETA);
      printf (" significant digits in a floating-point number\n");
      if (R6 != ZERO)
	W = LOG (ABS (R6)) / ALBETA;
      else
	W = ALL9;

      printf (" The maximum relative error of %15.4E = %4d ** ", R6, IBETA);
      printf ("  %7.2f/4X occurred for X = %17.6E\n", W, X1);

      W = MAX (AIT + W, ZERO);
      printf (" The root mean square relative error was %15.4E\n", R7);
      printf (" = %4d ** %7.2f\n", IBETA, W);

      if (R7 != ZERO)
	W = LOG (ABS (R7)) / ALBETA;
      else
	W = ALL9;

      printf (" The root mean square relative error was %15.4E\n", R7);
      printf (" = %4d ** %7.2f\n", IBETA, W);

      W = MAX (AIT + W, ZERO);
      printf (" The estimated loss of base %4d\n", IBETA);
      printf (" significant digits is %7.2f\n", W);
      /*  300 CONTINUE */
    }
  /*
C-----------------------------------------------------------------
C  Special tests
C-----------------------------------------------------------------
*/
  printf ("\n Test of special arguments\n");
  X = XMIN;
  CX = CONVC (THREE * X, FOUR * X);
  Y = FIVE * X;
  printf (" Test of |(3*XMIN,4*XMIN)| vs 5*XMIN.");
  printf (" This should not trigger an error message\n");
  printf ("XMIN = %15.4E\n", XMIN);
  Z = FABS (CX);
  printf (" CABS((3*XMIN,4*XMIN)) = %15.7E\n", Z);
  printf (" 5*XMIN                = %15.7E\n", Y);
  /*
C-----------------------------------------------------------------
C  Test of error returns
C-----------------------------------------------------------------
*/
  printf (" Test of error returns\n");
  X = XMAX / SIXTEN;
  CX = CONVC (FIVE * X, TWELVE * X);
  printf (" CABS will be called with the argument (%15.4E,%15.4E)\n",
	  creal (CX), cimag (CX));
  printf (" This should not trigger an error message\n");
  Z = FABS (CX);
  printf (" CABS returned the value %15.4E\n\n\n", Z);
  X = XMAX / FOUR;
  CX = CONVC (THREE * X, XMAX);
  printf (" CABS will be called with the argument (%15.4E,%15.4E)\n",
	  creal (CX), cimag (CX));
  printf (" This should trigger an error message\n\n");
  Z = FABS (CX);
  printf (" CABS returned the value %15.4E\n\n\n", Z);
  printf (" This concludes the tests\n");
  exit (0);
}

/*
C---------- Last card of CABS Test Program ----------
*/
