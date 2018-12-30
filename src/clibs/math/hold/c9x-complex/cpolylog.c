/*                                                      cpolylog.c

 *      Polylogarithms
 *
 *
 *
 * SYNOPSIS:
 *
 * double complex x, y, polylog();
 * int n;
 *
 * y = polylog( n, x );
 *
 *
 * The polylogarithm of order n is defined by the series
 *
 *
 *              inf   k
 *               -   x
 *  Li (x)  =    >   ---  .
 *    n          -     n
 *              k=1   k
 *
 *
 *  For x = 1,
 *
 *               inf
 *                -    1
 *   Li (1)  =    >   ---   =  Riemann zeta function (n)  .
 *     n          -     n
 *               k=1   k
 *
 *
 *  When n = 2, the function is the dilogarithm, related to Spence's integral:
 *
 *                 x                      1-x
 *                 -                        -
 *                | |  -ln(1-t)            | |  ln t
 *   Li (x)  =    |    -------- dt    =    |    ------ dt    =   spence(1-x) .
 *     2        | |       t              | |    1 - t
 *               -                        -
 *                0                        1
 *
 *
 *
 *  References:
 *
 *  Lewin, L., _Polylogarithms and Associated Functions_,
 *  North Holland, 1981.
 *
 *  Lewin, L., ed., _Structural Properties of Polylogarithms_,
 *  American Mathematical Society, 1991.
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain   n   # trials      peak         rms
 *    IEEE     -10, 10   2      5000      1.2e-15     1.9e-16
 *    IEEE     -10, 10   3      5000      1.3e-15     2.3e-16
 *    IEEE     -10, 10   4      5000      2.1e-15     2.5e-16
 *    IEEE     -10, 10   5      5000      1.8e-15     3.0e-16
 *
 */

/*
Cephes Math Library Release 2.8:  July, 1999
Copyright 1999 by Stephen L. Moshier
*/

#include <complex.h>

extern double PI;

#ifdef __STDC__
extern double powi (double, int);
extern double fac (int);
extern double zetac (double);
extern double complex clog (double complex);
double fabs (double);
static double complex invformula (int, double complex);
#else
extern double zetac (), powi (), fabs (), fac ();
extern double complex clog ();
static double complex invformula ();
#endif

extern double MACHEP;

double complex
  cpolylog (n, x)
     int n;
     double complex x;
{
  double complex h, k, p, s, u, z;
  double ah, as;
  int i, j;

  if (n == -1)
    {
      p = 1.0 - x;
      u = x / p;
      s = u * u + u;
      return s;
    }

  if (n == 0)
    {
      s = x / (1.0 - x);
      return s;
    }

/* This recurrence provides formulas for n < 2.

   d                 1
   --   Li (x)  =   ---  Li   (x)  .
   dx     n          x     n-1

 */

  if (n == 1)
    {
      s = -clog (1.0 - x);
      return s;
    }

  /* Argument +1 */
  if (x == (1.0 + 0.0 * I) && n > 1)
    {
      s = zetac ((double) n) + 1.0 + 0.0 * I;
      return s;
    }

  /* Argument -1.
                        1-n
     Li (-z)  = - (1 - 2   ) Li (z)
       n                       n
   */
  if (x == (-1.0 + 0.0 * I) && n > 1)
    {
      /* Li_n(1) = zeta(n) */
      s = zetac ((double) n) + 1.0 + 0.0 * I;
      s = s * (powi (2.0, 1 - n) - 1.0);
      return s;
    }

  /* Use inversion formula for large x.  */
  ah = fabs (creal (x)) + fabs (cimag (x));
  if (ah > 3.0)
    return (invformula (n, x));

  if ((fabs (creal (x)) + fabs (cimag (x))) < 0.75)
    goto pseries;


/*  The expansion in powers of log(x) is especially useful when
   x is near 1.

   See also the pari gp calculator.

                     inf                  j
                      -    z(n-j) (log(x))
   polylog(n,x)  =    >   -----------------
                      -           j!
                     j=0

   where

   z(j) = Riemann zeta function (j), j != 1

                           n-1
                            -
   z(1) =  -log(-log(x)) +  >  1/k
                            -
                           k=1
 */

  z = clog (x);
  h = -clog (-z);
  for (i = 1; i < n; i++)
    h = h + 1.0 / i;
  p = 1.0;
  s = zetac ((double) n) + 1.0;
  for (j = 1; j <= n + 1; j++)
    {
      p = p * z / j;
      if (j == n - 1)
	s = s + h * p;
      else
	s = s + (zetac ((double) (n - j)) + 1.0) * p;
    }
  j = n + 3;
  z = z * z;
  for (;;)
    {
      p = p * z / ((j - 1) * j);
      h = (zetac ((double) (n - j)) + 1.0);
      h = h * p;
      s = s + h;
      ah = fabs (creal (h)) + fabs (cimag (h));
      as = fabs (creal (s)) + fabs (cimag (s));
      if (ah / as < MACHEP)
	break;
      j += 2;
    }
  return s;


pseries:


  s = 0.0;
  ah = fabs (creal (x)) + fabs (cimag (x));
  if (ah < 1.0e-6)
    goto smseries;

  p = x * x * x;
  k = 3.0;
  do
    {
      p = p * x;
      k += 1.0;
      h = p / powi (k, n);
      s = s + h;
      ah = fabs (creal (h)) + fabs (cimag (h));
      as = fabs (creal (s)) + fabs (cimag (s));
    }
  while (ah / as > 1.1e-16);

smseries:
  s += x * x * x / powi (3.0, n);
  s += x * x / powi (2.0, n);
  s += x;
  return s;
}


/*  Inversion formula:
 *                                                   [n/2]   n-2r
 *                n                  1     n           -  log    (z)
 *  Li (-z) + (-1)  Li (-1/z)  =  - --- log (z)  +  2  >  ----------- Li  (-1)
 *    n               n              n!                -   (n - 2r)!    2r
 *                                                    r=1
 */
static double complex
  invformula (n, x)
     int n;
     double complex x;
{
  double complex p, q, s, w, m1;
  int r, j;

  m1 = -1.0 + 0.0 * I;
  w = clog (-x);
  s = 0.0 + 0.0 * I;
  for (r = 1; r <= n / 2; r++)
    {
      j = 2 * r;
      p = cpolylog (j, m1);
      j = n - j;
      if (j == 0)
	{
	  s = s + p;
	  break;
	}
      q = (double) j + 0.0 * I;
      q = cpow (w, q) * (creal (p) / fac (j));
      s = s + q;
    }
  s = 2.0 * s;

  q = cpolylog (n, 1.0 / x);
  if (n & 1)
    q = -q;
  s = s - q;

  s = s - cpow (w, (double complex) n) / fac (n);
  return s;
}
