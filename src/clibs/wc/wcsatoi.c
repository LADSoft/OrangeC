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
#include <stdio.h>
#include <String.h>
#include <wctype.h>
#include <limits.h>
#include <time.h>
#include <errno.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

#define nextchar { *ch = fgetwc(fil); \
                    (*chars)++; \
                    if (!count) return val; \
                    else count--; };

LLONG_TYPE __xwcstoimax(FILE *fil, int count, int *ch, int *chars,
        int radix, unsigned LLONG_TYPE max, int full, int errmode)
{
   int sign = 0;
   unsigned LLONG_TYPE val = 0;
   unsigned LLONG_TYPE imax ;
   int lim ;
    int temp;
    int err = 0;    
    count--;
    if (radix > 36 || radix < 0)
        return 0;

    while (iswspace(*ch)) nextchar;

    if (*ch == '-') {
        sign++;
        nextchar;
    }
    else if (*ch == '+') 
        nextchar;
    if (*ch == '0') {
      if (!radix) {
        radix = 8;
        nextchar;
        if (*ch == 'x' || *ch == 'X') {
           radix = 16;
           nextchar;
           if (!iswxdigit(*ch)) {
             return 0;   
           }
        }
      } else if (radix == 16) {
         nextchar; 
         if (*(ch) == 'x' || *(ch) == 'X') {
            nextchar;
            if (!iswxdigit(*ch))
                return 0;
         }
      }
    }
    if (!iswalnum(*ch))
        return 0;
    if (!radix)
      radix = 10 ;
    lim = (ULLONG_MAX % radix) ;
    imax = ULLONG_MAX / radix ;
    while(iswalnum(*ch)) {
        unsigned temp = towupper((wchar_t)*ch)- '0';
        if (temp >= 10)
            temp -= 7;
        if (temp >= radix)
                break ;
        else if (val > imax || val == imax && temp > lim) {
                err = 1;
        }
        val*= radix;
        val += temp;
        nextchar;
        if (*ch == EOF)
            break;
    }
    switch (errmode)
    {
        case STRT_ERR_RET_CV:
            if (sign)
                val = - val;
            break;
        case STRT_ERR_RET_SIGNED:
            if (err || val > max && (!sign || val != max))
            {
                errno = ERANGE;
                if (sign)
                    val = -(max+1);
                else
                    val = max;
            }
            else if (sign)
                val = - val;
            break;
        case STRT_ERR_RET_UNSIGNED:
            if (err || val > max)
            {
                errno = ERANGE;
                val = max;
            }
            else if (sign)
                val = - val;
            break;
    }
    return val;
}
static LLONG_TYPE __xwcstol(const wchar_t *buf, int width, wchar_t **endptr,
        int radix, unsigned LLONG_TYPE max, int full, int errmode)
{
   LLONG_TYPE rv ;
   int ch ;
   FILE fil ;
   int chars = 0;
   memset(&fil,0,sizeof(fil)) ;
   fil.level = wcslen(buf)*sizeof(wchar_t) + sizeof(wchar_t) ;
   fil.flags = _F_IN | _F_READ | _F_BUFFEREDSTRING ;
   fil.bsize = wcslen(buf) ;
   fil.buffer = fil.curp = buf ;
   fil.token = FILTOK ;
   ch = fgetwc(&fil) ;
   rv = __xwcstoimax(&fil,width,&ch,& chars,radix,max,full, errmode) ;
   if (endptr) {
      *endptr = fil.curp;
      if (fil.curp != fil.buffer)
        (*endptr)--;
   }
   return rv ;
}
LLONG_TYPE _RTL_FUNC wcstoimax(const wchar_t * restrict nptr, 
                              wchar_t ** restrict endptr, int base)
{
    return __xwcstol(nptr,INT_MAX,endptr,base, ULLONG_MAX,1, STRT_ERR_RET_SIGNED);  
} 
unsigned LLONG_TYPE _RTL_FUNC wcstoumax(const wchar_t * restrict nptr, 
                               wchar_t ** restrict endptr, int base)
{
    return __xwcstol(nptr,INT_MAX,endptr,base, ULLONG_MAX,1, STRT_ERR_RET_UNSIGNED);
}
LLONG_TYPE _RTL_FUNC wcstoll(const wchar_t * restrict nptr, 
                              wchar_t ** restrict endptr, int base)
{
    return __xwcstol(nptr,INT_MAX,endptr,base, ULLONG_MAX,1, STRT_ERR_RET_SIGNED); 
} 
unsigned LLONG_TYPE _RTL_FUNC wcstoull(const wchar_t * restrict nptr, 
                               wchar_t ** restrict endptr, int base)
{
    return __xwcstol(nptr,INT_MAX,endptr,base, ULLONG_MAX,1, STRT_ERR_RET_UNSIGNED);                             
}
long  _RTL_FUNC wcstol(const wchar_t * restrict s, wchar_t ** restrict endptr , int radix)
{
   return (long) __xwcstol(s,INT_MAX,endptr,radix,ULONG_MAX,1, STRT_ERR_RET_SIGNED) ;
}
unsigned long  _RTL_FUNC wcstoul(const wchar_t * restrict s, wchar_t ** restrict endptr , int radix)
{
   return (unsigned long)__xwcstol(s,INT_MAX,endptr,radix,ULONG_MAX,1, STRT_ERR_RET_UNSIGNED) ;
}
