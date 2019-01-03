/*
C----------------------------------------------------------------------
C  Subroutine to tabulate the results of tests for complex
C  functions.
C
C  Generic Fortran subprograms required:
C
C         ABS, AIMAG (or DIMAG), REAL (or DBLE)
C
C  Latest revision - October 11, 1990
C
C  Author - W. J. Cody
C           Argonne National Laboratory
C
C----------------------------------------------------------------------
C  Declare named common
C----------------------------------------------------------------------
*/

void
TABLAT ()
{
#ifdef TDOUBLE
  double complex CW;
  double ZZ;
#endif
#ifdef TSINGLE
  float complex CW;
  float ZZ;
#endif

  /*
C-----------------------------------------------------------------
C Handle vector error
C-----------------------------------------------------------------
*/
  if (CZ != CZERO)
    CW = (CZ - CZZ) / CZ;
  else
    CW = ONE;

  W = FABS (CW);
  if (W < ZERO)
    K1 = K1 + 1;
  else
    {
      if (W > ZERO)
	K3 = K3 + 1;
    }

  if (W > R6)
    {
      R6 = W;
      CX1 = CX;
      if (PFLAG)
	CY1 = CV;
      CF1 = CZ;
    }
  R7 = R7 + W * W;
  /*
C-----------------------------------------------------------------
C Handle error in real component
C-----------------------------------------------------------------
*/
  if (RFLAG)
    {
      Z = CONVR (CZ);
      ZZ = CONVR (CZZ);
      if (Z != ZERO)
	W = (Z - ZZ) / Z;
      else
	W = ONE;

      if (W < ZERO)
	KX1 = KX1 + 1;
      else
	{
	  if (W > ZERO)
	    KX3 = KX3 + 1;
	}
      W = ABS (W);
      if (W > RX6)
	{
	  RX6 = W;
	  CX2 = CX;
	  if (PFLAG)
	    CY2 = CV;
	  CF2 = CZ;
	}
      RX7 = RX7 + W * W;
    }
  /*
C-----------------------------------------------------------------
C Handle error in imaginary component
C-----------------------------------------------------------------
*/
  if (IFLAG)
    {
      Z = CONVI (CZ);
      ZZ = CONVI (CZZ);
      if (Z != ZERO)
	W = (Z - ZZ) / Z;
      else
	W = ONE;

      if (W < ZERO)
	KY1 = KY1 + 1;
      else
	{
	  if (W > ZERO)
	    KY3 = KY3 + 1;
	}
      W = ABS (W);
      if (W > RY6)
	{
	  RY6 = W;
	  CX3 = CX;
	  if (PFLAG)
	    CY3 = CV;
	  CF3 = CZ;
	}
      RY7 = RY7 + W * W;
    }
}

/*
C---------- Last line of TABLAT ----------
*/
/*
C----------------------------------------------------------------------
C  Subroutine to print the results of tests for complex functions.
C
C  Generic Fortran subprograms required:
C
C         ABS, LOG, MAX, SQRT
C
C  Latest revision - October 11, 1990
C
C  Author - W. J. Cody
C           Argonne National Laboratory
C
C----------------------------------------------------------------------
C  Declare named common
C----------------------------------------------------------------------
*/
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
extern double ALL9, ZERO;
#endif
#ifdef TSINGLE
extern float ALL9, ZERO;
#endif

/*
C----------------------------------------------------------------------
*/
void
REPORT (int IOUT)
{
  printf ("%7d random arguments were tested from the box\n", N);
  printf ("      (%15.4E,%15.4E)      (%15.4E,%15.4E)\n", AX, BX, AY, BY);
  /*
C-----------------------------------------------------------------
C Report vector error
C-----------------------------------------------------------------
*/
  printf (" First, report the vector error\n");
  K2 = N - K3 - K1;
  R7 = SQRT (R7 / XN);
  printf (" Test function was larger %6d times\n", K1);
  printf ("                   agreed %6d times, and\n", K2);
  printf ("              was smaller %6d times.\n", K3);

  printf (" There are %4d base %4d\n", IT, IBETA);
  printf (" significant digits in a floating-point number.\n");
  if (R6 != ZERO)
    W = LOG (ABS (R6)) / ALBETA;
  else
    W = ALL9;

  if (!PFLAG)
    {
      printf (" The maximum relative error of %15.4E = %4d ** %7.2f\n",
	      R6, IBETA, W);
      printf ("    occurred for Z = (%17.6E,%17.6E)\n", creal (CX1), cimag (CX1));
      printf ("    with a value of (%17.6E,%17.6E\n", creal (CF1), cimag (CF1));
    }
  else
    {
      printf (" The maximum relative error of %15.4E = %4d ** %7.2f\n",
	      R6, IBETA, W);
      printf (" occurred for Z = (%17.6E,%17.6E)\n", creal (CX1), cimag (CX1));
      printf ("             and W = (%17.6E,%17.6E)\n", creal (CY1), cimag (CY1));
      printf ("    with a value of (%17.6E,%17.6E)\n", creal (CF1), cimag (CF1));
    }
  W = MAX (AIT + W, ZERO);
  printf (" The estimated loss of base %4d\n", IBETA);
  printf (" significant digits is %7.2f\n", W);
  if (R7 != ZERO)
    W = LOG (ABS (R7)) / ALBETA;
  else
    W = ALL9;

  printf (" The root mean square relative error was %15.4E\n", R7);
  printf (" = %4d ** %7.2f\n", IBETA, W);
  W = MAX (AIT + W, ZERO);
  printf (" The estimated loss of base %4d\n", IBETA);
  printf (" significant digits is %7.2f\n", W);
  /*
C-----------------------------------------------------------------
C Report error in real component
C-----------------------------------------------------------------
*/
  if (RFLAG)
    {
      printf (" Then, report the error in the real component.\n");
      KX2 = N - KX3 - KX1;
      RX7 = SQRT (RX7 / XN);
      printf (" Test function was larger %6d times\n", KX1);
      printf ("                   agreed %6d times, and\n", KX2);
      printf ("              was smaller %6d times.\n", KX3);

      printf (" There are %4d base %4d\n", IT, IBETA);
      printf (" significant digits in a floating-point number.\n");
      if (RX6 != ZERO)
	W = LOG (ABS (RX6)) / ALBETA;
      else
	W = ALL9;

      if (!PFLAG)
	{
	  printf (" The maximum relative error of %15.4E = %4d ** %7.2f\n",
		  RX6, IBETA, W);
	  printf ("    occurred for Z = (%17.6E,%17.6E)\n", creal (CX2), cimag (CX2));
	  printf ("    with a value of (%17.6E,%17.6E\n", creal (CF2), cimag (CF2));
	}
      else
	{
	  printf (" The maximum relative error of %15.4E = %4d ** %7.2f\n",
		  RX6, IBETA, W);
	  printf (" occurred for Z = (%17.6E,%17.6E)\n", creal (CX2), cimag (CX2));
	  printf ("             and W = (%17.6E,%17.6E)\n", creal (CY2), cimag (CY2));
	  printf ("    with a value of (%17.6E,%17.6E)\n", creal (CF2), cimag (CF2));
	}
      W = MAX (AIT + W, ZERO);
      printf (" The estimated loss of base %4d\n", IBETA);
      printf (" significant digits is %7.2f\n", W);
      if (RX7 != ZERO)
	W = LOG (ABS (RX7)) / ALBETA;
      else
	W = ALL9;

      printf (" The root mean square relative error was %15.4E\n", RX7);
      printf (" = %4d ** %7.2f\n", IBETA, W);
      W = MAX (AIT + W, ZERO);
      printf (" The estimated loss of base %4d\n", IBETA);
      printf (" significant digits is %7.2f\n", W);
    }
  /*
C-----------------------------------------------------------------
C Report error in imaginary component
C-----------------------------------------------------------------
*/
  if (IFLAG)
    {
      printf (" Then, report the error in the imaginary component.\n");
      KY2 = N - KY3 - KY1;
      RY7 = SQRT (RY7 / XN);
      printf (" Test function was larger %6d times\n", KY1);
      printf ("                   agreed %6d times, and\n", KY2);
      printf ("              was smaller %6d times.\n", KY3);

      printf (" There are %4d base %4d\n", IT, IBETA);
      printf (" significant digits in a floating-point number.\n");
      if (RY6 != ZERO)
	W = LOG (ABS (RY6)) / ALBETA;
      else
	W = ALL9;

      if (!PFLAG)
	{
	  printf (" The maximum relative error of %15.4E = %4d ** %7.2f\n",
		  RY6, IBETA, W);
	  printf ("    occurred for Z = (%17.6E,%17.6E)\n", creal (CX3), cimag (CX3));
	  printf ("    with a value of (%17.6E,%17.6E\n", creal (CF3), cimag (CF3));
	}
      else
	{
	  printf (" The maximum relative error of %15.4E = %4d ** %7.2f\n",
		  RY6, IBETA, W);
	  printf (" occurred for Z = (%17.6E,%17.6E)\n", creal (CX3), cimag (CX3));
	  printf ("             and W = (%17.6E,%17.6E)\n", creal (CY3), cimag (CY3));
	  printf ("    with a value of (%17.6E,%17.6E)\n", creal (CF3), cimag (CF3));
	}
      W = MAX (AIT + W, ZERO);
      printf (" The estimated loss of base %4d\n", IBETA);
      printf (" significant digits is %7.2f\n", W);
      if (RY7 != ZERO)
	W = LOG (ABS (RY7)) / ALBETA;
      else
	W = ALL9;

      printf (" The root mean square relative error was %15.4E\n", RY7);
      printf (" = %4d ** %7.2f\n", IBETA, W);

      W = MAX (AIT + W, ZERO);
      printf (" The estimated loss of base %4d\n", IBETA);
      printf (" significant digits is %7.2f\n", W);

    }
}

/*
C-----------------------------------------------------------------
*/
/*
C---------- Last line of REPORT ----------
*/
/*
C----------------------------------------------------------------------
C  Subroutine to reset the paramaters for tests for complex
C  functions.
C
C  Latest revision - October 15, 1990
C
C  Author - W. J. Cody
C           Argonne National Laboratory
C
*/

void
RESET ()
{

  K1 = 0;
  K3 = 0;
  KX1 = 0;
  KX3 = 0;
  KY1 = 0;
  KY3 = 0;
  CX1 = CZERO;
  CX2 = CZERO;
  CX3 = CZERO;
  CY1 = CZERO;
  CY2 = CZERO;
  CY3 = CZERO;
  CF1 = CZERO;
  CF2 = CZERO;
  CF3 = CZERO;
  R6 = ZERO;
  R7 = ZERO;
  RX6 = ZERO;
  RX7 = ZERO;
  RY6 = ZERO;
  RY7 = ZERO;
};

/*
C---------- Last line of RESET ----------
*/
