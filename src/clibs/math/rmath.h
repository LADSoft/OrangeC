/*
 *  AUTHOR
 *    Catherine Loader, catherine@research.bell-labs.com.
 *    October 23, 2000.
 *
 *  Merge in to R:
 *	Copyright (C) 2000, The R Core Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, a copy is available at
 *  https://www.R-project.org/Licenses/
 *
 */
#define ML_POSINF	(1.0 / 0.0)
#define ML_NEGINF	((-1.0) / 0.0)
#define ML_NAN		(0.0 / 0.0)

#define ML_ERR_return_NAN { mtherr("", ME_DOMAIN); return ML_NAN; }
	
#define ML_VALID(x)	(!ISNAN(x))

#define ME_NONE		0
/*	no error */
#define ME_DOMAIN	1
/*	argument out of domain */
#define ME_RANGE	7
/*	value out of range */
#define ME_NOCONV	8
/*	process did not converge */
#define ME_PRECISION	6
/*	does not have "full" precision */
#define ME_UNDERFLOW	4
/*	and underflow occured (important for IEEE)*/


int  __chebyshev_init(double *dos, int nos, double eta);
double  __chebyshev_eval(double x, const double *a, const int n);
double __lgammafn_sign(double x, int *sgn);
double __gammafn(double x);
double lgamma(double x);
double __lgammacor(double x);
double __stirlerr(double n);
