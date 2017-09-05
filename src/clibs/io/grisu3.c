#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#pragma pack(0)
typedef struct FP
{
    unsigned long long m;
    int x;
} FP;

FP cached[] = {
#include "grisuacache.h"
};

FP tenpowers[] = {
{ 0x8000000000000000ULL, -63 },
{ 0xa000000000000000ULL, -60 },
{ 0xc800000000000000ULL, -57 },
{ 0xfa00000000000000ULL, -54 },
{ 0x9c40000000000000ULL, -50 },
{ 0xc350000000000000ULL, -47 },
{ 0xf424000000000000ULL, -44 },
{ 0x9896800000000000ULL, -40 },
{ 0xbebc200000000000ULL, -37 },
} ;
#pragma pack()
#define ONE_OVER_LOG2_10 0.3010299956639811
static int get_k(int e, int alpha, int gamma) 
{ 
    return ceil((alpha-e+ 63) * ONE_OVER_LOG2_10); 
} 
static FP multiply(FP x, FP y);
static FP get_cached(int k)
{
    int k1 = k < 0 ? k/9-1 : k/9;
    int k2 = k < 0 ? k - k1 * 9 : k- k1*9;
    FP rv = multiply(cached[k1+33], tenpowers[k2]);
    while ((long long)rv.m > 0)
        rv.m = rv.m << 1, rv.x--;
    return rv;
}
static FP multiply(FP x, FP y) 
{ 
    unsigned long long a,b,c,d,ac,bc,ad,bd,tmp; 
    FP r; 
    const unsigned long long M32 = 0xFFFFFFFF; 
    a = x.m >> 32; 
    b = x.m & M32; 
    c = y.m >> 32; 
    d = y.m & M32; 
    ac = a*c; bc = b*c; ad = a*d; bd = b*d; 
    tmp = (bd>>32) + (ad&M32) + (bc&M32); 
    tmp += 1U << 31; 
    r.m = ac + (ad>>32) + (bc>>32) + (tmp>>32); 
    r.x = x.x + y.x + 64; 
    return r; 
} 
#define TEN9 1000000000
static void tobuf(FP Mp, FP delta, char *buf, int *len, int *K)
{
	int divv;
	int d,kappa;
	FP one;
	one.m = 1ULL << - Mp.x;
	one.x = Mp.x;
	unsigned part1 =  Mp.m >> - one.x;
	unsigned long long part2 = Mp.m & (one.m-1);
	*len = 0; 
	divv = TEN9;
	
	for (kappa = 10; kappa > 0; kappa--)
	{
		div_t aa = div(part1,divv);
		if (aa.quot || *len)
			buf[(*len)++]= '0' + aa.quot;
		part1 = aa.rem;
		divv /= 10;
		if ((((unsigned long long)part1) << -one.x) + part2 < delta.m)
		{
			*K -= kappa-1;
			break;
		}
	}
	if (kappa == 0)
	{
		do {
			part2 *= 10;
			int d = part2 >> -one.x;
			if (d || *len)
				buf[(*len)++]= '0' + d;
			part2 &= one.m-1;
			kappa--;
			delta.m *= 10;
		}
		while (part2 > delta.m);
		*K -= kappa;
	}
}
static int roundAndWeed(char *buffer, int len, FP W, unsigned long long delta, unsigned long long rest, unsigned long long tenkappa)
{
	unsigned long long Wminus = W.m;
	unsigned long long Wplus = W.m;
	Wminus += 1 << 11;
	Wplus -= 1 << 11;
	while (rest < Wplus && delta - rest >= tenkappa &&
			(rest + tenkappa< Wplus || Wplus-rest >= rest - tenkappa- Wplus))
			{
				buffer[len-1]--;
				rest += tenkappa;
			}
	if (rest < Wminus && delta - rest >= tenkappa &&
			(rest + tenkappa< Wminus || Wminus-rest >= rest - tenkappa- Wminus))
		return 0;
	return (2 << 11) <= rest && rest <= delta- (4 << 11);
}
static int grisu3(double v, char* buffer) 
{ 
    union {
        double a;
        unsigned long long q;
    } p;
    p.a = v;
    
    FP mminus, mplus, m; 
    m.m = mminus.m = mplus.m =  p.q & 0x000fffffffffffffLL;
    m.x = mminus.x = mplus.x = (p.q >> 52) & 0x7ff;
    if (m.x == 0)
        return INT_MIN;
    if (mminus.x > 1)
    {
        m.m = mminus.m = mplus.m |= 0x0010000000000000LL;
    }
	mminus.m <<=1 ;
	mminus.x--;
	mplus.m <<=1;
	mplus.x--;
	mminus.m--;
	mplus.m++;
    FP wminus = mminus, wplus = mplus, w = m;
	while ((long long)wminus.m > 0)
		wminus.m <<= 1, wminus.x--;
	while ((long long)wplus.m > 0)
		wplus.m <<= 1, wplus.x--;
	while ((long long)w.m > 0)
		w.m <<= 1, w.x--;
	wminus.x -= 1023 + 52;
	wplus.x -= 1023 + 52;
	w.x -= 1023 + 52;
    const int alpha = -59;
    const int gamma = -32;
    int mk = get_k(wplus.x+64, alpha, gamma);
	FP fmk = get_cached(mk);
    FP Mminus = multiply(wminus, fmk);
    FP Mplus = multiply(wplus, fmk);
	FP W = multiply(w, fmk);
	Mminus.m-= 1 << 11;
	Mplus.m+= 1 << 11;	
	while (Mminus.x < Mplus.x)
		Mminus.m >>=1, Mminus.x++;
 	FP delta = Mplus;
	delta.m -= Mminus.m;
		
	int K = mk;
	int len;
	tobuf(Mplus, delta, buffer, &len, &K);
	buffer[len] = 0;
	FP WpW = Mplus;
	while (WpW.x < W.x)
		WpW.m >>=1, WpW.x++;
	WpW.m -= W.m;

	
	FP g = fmk;
	while (g.x < Mplus.x)
		g.m >>=1, g.x++;
	while (Mplus.x < g.x)
		Mplus.m >>=1, Mplus.x++;
	unsigned long long rest = Mplus.m - g.m;
	if (roundAndWeed(buffer, len, WpW, delta.m, rest, g.m))
	{
        return K;
	}
	else
	{
        return INT_MAX;
	}
}
int fextractdouble(double fmant, int *fexp, int *fsign, unsigned char *buf)
{
    *fsign = fmant < 0 ? -1 : 1;
    fmant = fabs(fmant);
    int exp = grisu3(fmant, (char *)buf);
    if (exp == INT_MIN)
    {
        *fexp = 0;
        buf[0] = '0';
        buf[1] = 0;
        return 1;
    }
    else if (exp == INT_MAX)
    {
        *fexp = exp;
        return exp;
    }
    else
    {
        exp = strlen((char *)buf) - exp-1;
        *fexp = exp;
        return 0;
    }
}
