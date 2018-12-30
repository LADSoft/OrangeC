
/* Test vectors for complex math functions.

   S. L. Moshier
   February, 1997  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "complex.h"
#if 0
int signbit (double);
#endif
int isnan (double);

#define PI 3.141592653589793238463E0
#define PIO2 1.570796326794896619231E0
#define PIO4 7.853981633974483096157E-1
#define SQRT2 1.414213562373095048802E0
#define SQRTH 7.071067811865475244008E-1
#define MACHEP 1.11022302462515654042E-16

/* Functions of one variable.  */

struct oneargument
  {
    char *name;			/* Name of the function. */
    double complex (*func) (double complex);
    double complex arg1;
    double complex answer;
    int thresh;			/* Error report tolerance threshold. */
  };


struct oneargument test1[] =
{
  {"csin", csin,   PIO2+0.0*I, 1.0 + 0.0*I, 0},
  {"csin", csin,  -PIO2+0.0*I, -1.0 + 0.0*I, 0},
  {"ccos", ccos,    0.0+0.0*I, 1.0 + 0.0*I, 0},
  {"ccos", ccos,     PI+0.0*I, -1.0 + 0.0*I, 0},
  {"csin", csin,  (0.25*PI)+0.0*I, SQRTH + 0.0*I, 2},
  {"ccos", ccos,  (0.25*PI)+0.0*I, SQRTH + 0.0*I, 2},
  {"cacos", cacos,  0.0+0.0*I, PIO2+0.0*I, 0},
  {"cacos", cacos,  1.0+0.0*I, 0.0+0.0*I, 0},
  {"cacosh", cacosh,  0.0+0.0*I, 0.0 + PIO2*I, 0},
  {"casinh", casinh,  0.0+0.0*I, 0.0 + 0.0*I, 0},
  {"catanh", catanh,  0.0+0.0*I, 0.0 + 0.0*I, 0},
  {"ccosh", ccosh, 0.0+0.0*I, 1.0+0.0*I, 0},
  {"ccosh", ccosh, 0.0+PI*I, -1.0+0.0*I, 0},
  {"csinh", csinh, 0.0+0.0*I, 0.0+0.0*I, 0},
  {"csinh", csinh, 0.0+PIO2*I, 0.0+1.0*I, 0},
  {"ctanh", ctanh, 0.0+0.0*I, 0.0+0.0*I, 0},
  {"cexp", cexp,   0.0+0.0*I, 1.0+0.0*I, 0},
  {"cexp", cexp,   0.0+PI*I, -1.0+0.0*I, 2},
  {"clog", clog,   1.0+0.0*I, 0.0+0.0*I, 0},
  {"clog", clog,  -1.0+0.0*I, 0.0+PI*I, 0},
  {"clog", clog,   0.0+1.0*I, 0.0+PIO2*I, 0},
  {"clog", clog,   0.0-1.0*I, 0.0-PIO2*I, 0},
  {"csqrt", csqrt, 0.0+0.0*I, 0.0+0.0*I, 0},
  {"csqrt", csqrt, 0.0+2.0*I, 1.0+1.0*I, 0},
  {"csqrt", csqrt, 1.0+0.0*I, 1.0+0.0*I, 0},
  {"csqrt", csqrt, 3.0+4.0*I, 2.0+1.0*I, 0},
  {"csqrt", csqrt, 3.0-4.0*I, 2.0-1.0*I, 0},
  {"csqrt", csqrt, -3.0-4.0*I, 1.0-2.0*I, 0},
  {"csqrt", csqrt, -3.0+4.0*I, 1.0+2.0*I, 0},
  {0, 0, 0.0, 0.0, 0},
};

/* Functions of two variables.  */

struct twoarguments
  {
    char *name;			/* Name of the function. */
    double complex (*func) (double complex, double complex);
    double complex arg1;
    double complex arg2;
    double complex answer;
    int thresh;
  };

struct twoarguments test2[] =
{
  {"cpow", cpow,  0.0+0.0*I, 1.0+0.0*I,       0.0, 0},
  {"cpow", cpow,  2.0+0.0*I, 3.0+0.0*I, 8.0+0.0*I, 0},
  {"cpow", cpow,  1.0+1.0*I, 2.0+0.0*I, 0.0+2.0*I, 3},
  {"cpow", cpow,  1.0-1.0*I, 2.0+0.0*I, 0.0-2.0*I, 3},
  {"cpow", cpow,  0.0+1.0*I, 2.0+0.0*I,      -1.0, 2},
  {"cpow", cpow, -1.0+0.0*I, 0.5+0.0*I, 0.0+1.0*I, 2},
  {0, 0, 0.0, 0.0, 0.0, 0},
};


/* Print complex value X in hex.  */

void
pchex (void *pc)
{
  unsigned int *pi;
  int k;

  pi = (unsigned int *) pc;
  for (k = 0; k < 4; k++)
    printf ("%08x ", *pi++);
  printf ("\n");
}


volatile double complex x1;
volatile double complex x2;
volatile double complex cy;
volatile double complex answer;
volatile double x;
volatile double y;

int
main ()
{
  int i, nerrors;
  double complex (*fun1) (double complex);
  double complex (*fun2) (double complex, double complex);
  double complex e;

  nerrors = 0;
  for (i = 0; ; i++)
    {
      if (test1[i].name == 0 || test1[i].func == 0)
	break;
      x1 = test1[i].arg1;
      fun1 = test1[i].func;
      cy = (*(fun1)) (x1);
      answer = test1[i].answer;
      if (memcmp ((void *) &cy, (void *) &answer, sizeof (double complex))
	  == 0)
	continue; /* answer is ok */

#if 0
      /* Check sign bits, in case of minus zero.  */
      if (signbit (creal (cy)) != signbit (creal (answer)))
	{
	  /* If they are both NaN's, forget about the sign bits.  */
	  if (isnan (creal (cy)) && isnan (creal (answer)))
	      goto srealok;
	  printf ("%d Sign bit of real part differs.\n", i);
	  goto err1;
	}
  srealok:
      if (signbit (cimag (cy)) != signbit (cimag (answer)))
	{
	  /* If they are both NaN's, forget about the sign bits.  */
	  if (isnan (cimag (cy)) && isnan (cimag (answer)))
	      goto simagok;
	  printf ("%d Sign bit of imaginary part differs.\n", i);
	  goto err1;
	}
  simagok:
#endif

      if (cy != answer)
	{
	  e = cy - answer;
	  if (answer != 0.0)
	    e = e / answer;
	  if (cabs(e) > test1[i].thresh * MACHEP)
	    {
#if 0
  err1:
#endif
	      printf (
 "%d %s (%.16e,%.16e) = (%.16e,%.16e)\n    should be. (%.16e,%.16e)\n",
  i, test1[i].name, creal(x1), cimag(x1),
 creal(cy), cimag(cy), creal(answer), cimag(answer));
	      pchex ((void *) &x1);
	      pchex ((void *) &cy);
	      pchex ((void *) &answer);
	      nerrors += 1;
	    }
	}
    }

  for (i = 0; ; i++)
    {
      if (test2[i].name == 0 || test2[i].func == 0)
	break;
      x1 = test2[i].arg1;
      x2 = test2[i].arg2;
      fun2 = test2[i].func;
      cy = (*(fun2)) (x1, x2);
      answer = test2[i].answer;
      if (memcmp ((void *) &cy, (void *) &answer, sizeof (double complex))
	  == 0)
	continue; /* answer is ok */
#if 0
      if (signbit (creal (cy)) != signbit (creal (answer)))
	{
	  printf ("%d Sign bit of real part differs.\n", i);
	  goto err1;
	}
      if (signbit (cimag (cy)) != signbit (cimag (answer)))
	{
	  printf ("%d Sign bit of imaginary part differs.\n", i);
	  goto err1;
	}
#endif
      if (cy != answer)
	{
	  e = cy - answer;
	  if (answer != 0.0)
	    e = e / answer;
	  /*
	  if (e < 0)
	    e = -e;
	    */
	  if (cabs(e) > test2[i].thresh * MACHEP)
	    {
	      printf ("%s ((%.16e,%.16e), (%.16e,%.16e))",
	      test2[i].name, creal(x1), cimag(x1), creal(x2), cimag(x2));
	      printf (" = (%.16e,%.16e), should be (%.16e,%.16e)\n",
		      creal(cy), cimag(cy), creal(answer), cimag(answer));
	      nerrors += 1;
	    }
	}
    }

  printf ("%d errors\n", nerrors);
  exit (0);
}
