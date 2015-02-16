/*
	Software License Agreement (BSD License)
	
	Copyright (c) 1997-2008, David Lindauer, (LADSoft).
	All rights reserved.
	
	Redistribution and use of this software in source and binary forms, with or without modification, are
	permitted provided that the following conditions are met:
	
	* Redistributions of source code must retain the above
	  copyright notice, this list of conditions and the
	  following disclaimer.
	
	* Redistributions in binary form must reproduce the above
	  copyright notice, this list of conditions and the
	  following disclaimer in the documentation and/or other
	  materials provided with the distribution.
	
	* Neither the name of LADSoft nor the names of its
	  contributors may be used to endorse or promote products
	  derived from this software without specific prior
	  written permission of LADSoft.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
	PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
	TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <errno.h>
#include <stdlib.h>
#include <wctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <time.h>
#include <locale.h>
#include <float.h>
#include "_locale.h"
#include "libp.h"
#include "_lfloat.h"

static unsigned floating_infinity = 0x7f800000;
static unsigned floating_nan = 0x7fc00000;

#define nextchar { *ch = fgetwc(fil); \
                    (*chars)++; \
                    if (!count) return val; \
                    else count--; }
                    
extern long double __tensexp(int n);
long double __xwcstod(FILE *fil, int count, int *ch, int *chars, long double max, int exp2, int exp10, int full)
{
	int sign = 0;
	long double val = 0.0;
	int exp=0;
    int radix = 10;
    char *bpos = fil->curp;
    NUMERIC_DATA *nd = __locale_data[LC_NUMERIC];

    count--;    

    while (isspace(*ch)) nextchar;
        
    if (*ch == '-') {
		sign++;
        nextchar;
	}
    else if (*ch == '+') {
        nextchar;
	}
    if (*ch == '0') {
        nextchar;
        if (*(ch) == 'x' || *(ch) == 'X')  {
            radix = 2;
            nextchar;
        }
    } else if (full && *ch == 'i' || *ch == 'I') {
        int i;
        nextchar;
         if (*ch != 'n' && *ch != 'N') {
            fil->curp = bpos;
            return 0;
         }
        nextchar;
         if (*ch != 'f' && *ch != 'F') {
            fil->curp = bpos; // fil->buffer;
            return 0;
         }
         nextchar ;
         bpos = fil->curp ;
         for (i=0; i < 5; i++) {
            if ("inity"[i] != (*ch | 0x20)) {
                fil->curp = bpos;
                break ;
            }
            nextchar ;
         }
         if (sign)
            return - *(float *)&floating_infinity;
         else
            return *(float *)&floating_infinity;
    } else if (full && *ch == 'n' || *ch == 'N') {
        int shift = 19;
        unsigned nan = floating_nan, nan_significand = 0;
        nextchar;
         if (*ch != 'a' && *ch != 'A') {
            fil->curp = bpos;
            return 0;
         }
        nextchar;
         if (*ch != 'n' && *ch != 'N') {
            fil->curp = bpos;
            return 0;
         }
         nextchar;
         bpos = fil->curp;
         if (*ch == '(') {
            nextchar;
            while (isxdigit(*ch) && shift >= 3) {
               int n = *ch;
               nextchar ;
               if (n >= 'a')
                   n = n - 'a' + 10;
               else if (n >= 'A')
                   n = n - 'A' + 10;
               else
                   n = n - '0';
               nan_significand |= n << shift;
               shift -= 4;
            }
            if (*ch == ')') {
              nan |= nan_significand;
              nextchar;
            }
            else
               fil->curp = bpos;
         }
         val = *(float *)&nan;
    	   if (sign)
    		val = -val;
         return val;
    } else if (!isdigit(*ch) && *ch != nd->decimal_point[0]) {
        fil->curp = bpos;
        return 0;
    }
    if (radix == 2) {
        unsigned LLONG_TYPE i = 0;
        int k = 0;
        long double rval = 0;
        while (isxdigit(*ch)) {
            i = i * 16;
            if (*ch <= '9')
                i += *ch - '0';
            else if (*ch >= 'a')
                i += *ch - 'a' + 10;
            else
                i += *ch - 'A' + 10;
            if (++k == sizeof(i) * 2)
            {
                rval *= 2 << (k * 4);
                rval += i;
                k = 0;
                i = 0;
            }
            nextchar;
        }
        rval *= 2 << (k * 4);
        rval += i;
        if (*ch == nd->decimal_point[0]) {
            int pow = 0;
            k = 0;
            i = 0;
            nextchar;
            while (isxdigit(*ch)) {
                i = i * 16;
                if (*ch <= '9')
                    i += *ch - '0';
                else if (*ch >= 'a')
                    i += *ch - 'a' + 10;
                else
                    i += *ch - 'A' + 10;
                pow--;
                if (++k == sizeof(i) * 2)
                {
                    rval += ((double)i) / (2 << ((4 * -pow)-1));
                    k = 0;
                    i = 0;
                }
                nextchar;
            }
            rval += ((double)i) / (2 << ((4 * -pow)-1));
        }
    	if (sign)
      {
    		val = - val;
            rval = - rval;
      }
        if (*ch == 'p' || *ch == 'P') {
    		sign = 0;
            nextchar;
            if (*ch == '-') {
    			sign++;
                nextchar;
    		}
            else if (*ch == '+') {
                nextchar;
    		}
            if (!isdigit(*ch)) {
                fil->curp = bpos;
                return 0;
            }
            while(isdigit(*ch)) {
    			exp*= 10;
                exp += (*ch-'0');
                nextchar;
    		}
    		if (!sign && exp > exp2) {
                goto rangeerr;
            }
		if (sign)
            rval /= (2 << exp);
        else
            rval *= (2 << exp);
    	}
        val = rval;
    } else {
        unsigned LLONG_TYPE i = 0;
        int k = 0;
        long double rval = 0;
        while (isdigit(*ch)) {
            i = i * 10 + (*ch - '0');
            if (++k == (int)(sizeof(i) * M_LN2 / M_LN10))
            {
                rval *= __tensexp(k);
                rval += i;
                k = 0;
                i = 0;
            }
            nextchar;
        }
        rval *= __tensexp(k);
        rval += i;
        if (*ch == nd->decimal_point[0]) {
            int pow = 0;
            k = 0;
            i = 0;
            nextchar;
            while(isdigit(*ch)) {
                i = i * 10 + (*ch - '0');
                pow--;
                if (++k == (int)(sizeof(i) * CHAR_BIT * M_LN2 / M_LN10))
                {
                    rval += ((double)i) * __tensexp(pow);
                    k = 0;
                    i = 0;
                }
                nextchar ;
    	    }
            rval += ((double)i) * __tensexp(pow);
    	}
    	if (sign)
      {
    		val = - val;
            rval = - rval;
      }
        if (*ch == 'e' || *ch == 'E') {
    		sign = 0;
            nextchar;
            if (*ch == '-') {
    			sign++;
                nextchar;
    		}
            else if (*ch == '+') {
                nextchar;
    		}
            if (!isdigit(*ch)) {
                fil->curp = bpos;
                return 0;
            }
            while(isdigit(*ch)) {
    			exp*= 10;
                exp += (*ch-'0');
                nextchar;
    		}
    		if (!sign && exp > exp10) {
                goto rangeerr;
            }
		if (sign)
            rval /= __tensexp(exp);
        else
            rval *= __tensexp(exp);
    	}
        val = rval;
    }
    if (val > max || val < - max) {
rangeerr:
        errno = ERANGE;
        if (max == FLT_MAX)
            return copysignf(HUGE_VALF,val);
        else if (max == DBL_MAX)
            return copysign(HUGE_VAL,val);
        else
            return copysignl(HUGE_VALL,val);
    }
	return val;
}
static long double __wcstod(const wchar_t *buf, wchar_t **endptr, int width,
        long double max,int exp2,int exp10, int full)
{
   long double rv ;
   int ch,chars ;
   FILE fil ;
   memset(&fil,0,sizeof(fil)) ;
   fil.level = (wcslen(buf) + sizeof(wchar_t)) * sizeof(wchar_t);
   fil.flags = _F_IN | _F_READ | _F_BUFFEREDSTRING ;
   fil.bsize = wcslen(buf) * sizeof(wchar_t);
   fil.buffer = fil.curp = buf ;
   fil.token = FILTOK ;
   ch = fgetwc(&fil) ;
   rv = __xwcstod(&fil,width,&ch,&chars,max,exp2,exp10,full) ;
   if (endptr) {
      *endptr = fil.curp;
      if (fil.curp != fil.buffer)
        (*endptr)--;
   }
   return rv ;
}
long double  _RTL_FUNC wcstold(const wchar_t *restrict string, wchar_t **restrict endptr)
{
   return(__wcstod(string,endptr,256,LDBL_MAX, LDBL_MAX_EXP, LDBL_MAX_10_EXP,1));
}
long double  _RTL_FUNC _wcstold(const wchar_t *restrict string, wchar_t **restrict endptr)
{
    return wcstold(string,endptr);
}
double  _RTL_FUNC wcstod(const wchar_t *restrict string, wchar_t **restrict endptr)
{
   return((double)__wcstod(string,endptr,256,DBL_MAX, DBL_MAX_EXP, DBL_MAX_10_EXP,1));
}
float _RTL_FUNC wcstof(const wchar_t *restrict string, wchar_t ** restrict endptr)
{
   return((float)__wcstod(string,endptr,256,FLT_MAX, FLT_MAX_EXP, FLT_MAX_10_EXP,1));
}
double  _RTL_FUNC wtof(const wchar_t *restrict string)
{
   return((double)__wcstod(string,0,256,DBL_MAX, DBL_MAX_EXP, DBL_MAX_10_EXP,1));
}
long double  _RTL_FUNC _wtold(const wchar_t *restrict string)
{
   return(__wcstod(string,0,256,LDBL_MAX, LDBL_MAX_EXP, LDBL_MAX_10_EXP,1));
}
