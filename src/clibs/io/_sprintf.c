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
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>
#include <float.h>
#include "_locale.h"
#include "libp.h"
#include "_lfloat.h"
#include "malloc.h"
#include <stdlib.h>

int _e_min_exp_digit = 2;  /* min exp digits for %e (stdc, MS is 3) */

int fextractdouble(double fmant, int *fexp, int *fsign, unsigned char *buf);   

#ifdef _i386_
#define USE_FLOAT
#endif

#define NUM_SIZE 100
#define PUTCH(x)  fputc(x, __stream)
#define PUTSTR(x) fputs(x, __stream)

int fnd(unsigned char *buf, int index)
{
  if (index >= strlen(buf))
      return 0;
  return buf[index] - '0';
}

long double __tensexp(int n)
{
    int table[] = {
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
    1000000000,

    };
    long double rval = 1.0;
    if (n < 0)
    {
        n = - n;
        while (n >= sizeof(table)/sizeof(table[0]))
        {
            rval /= table[sizeof(table)/sizeof(table[0])-1];
            n -= sizeof(table)/sizeof(table[0])-1;
        }
        return rval/table[n];
    }
    else
    {
        while (n >= sizeof(table)/sizeof(table[0]))
        {
            rval *= table[sizeof(table)/sizeof(table[0])-1];
            n -= sizeof(table)/sizeof(table[0])-1;
        }
        return rval*table[n];
    }
}                    

int fextract(long double *fmant, int *fexp, int *fsign, unsigned char *buf)
{
    int count = 0;
    unsigned char *q = buf;
    long double rval = *fmant;
    int exp, type, i;
    *fsign = rval < 0 ? -1 : 1;
    rval = fabsl(rval);
    if (rval)
    {
        exp = (int)log10l(rval);
        rval /= __tensexp(exp);
        if (rval >= 10)
        {
            *q++ = '1';
            rval-= 10;
            exp++;
        }
        else if (rval < 1)
        {
            rval = rval * 10;
            exp--;
        }
        while (rval != 0 && count < LDBL_DIG)
        {
            int val;
            val = rval;
            rval -= val;
            *q++ = val + '0';     
            rval = rval * 10;
            count++;
        }
    }
    else
    {
        
        exp = 0;
        *q++ = '0';
    }        
    *q = 0;
    *fexp = exp;
    return exp == 0;
}
static char * getnum(char *string, LLONG_TYPE num,int radix,int lc,int unsign)
{
    static const char digitsArray[] =
    "00010203040506070809"
    "10111213141516171819"
    "20212223242526272829"
    "30313233343536373839"
    "40414243444546474849"
    "50515253545556575859"
    "60616263646566676869"
    "70717273747576777879"
    "80818283848586878889"
    "90919293949596979899";
	int i,sz=0;
   unsigned LLONG_TYPE unum;
    string[NUM_SIZE-2] = '0';
	string[NUM_SIZE-1] = 0;
	if (num < 0 && !unsign)
		unum = - num;
	else
		unum = num;
	i = NUM_SIZE-2;
    switch (radix)
    {
        case 10:
        	while (unum >= 100) {
                lldiv_t aa = lldiv(unum, 100);
                int val = ((int)aa.rem) * 2;
                unum = aa.quot;
                string[i--] = digitsArray[val + 1];
                string[i--] = digitsArray[val];
        		sz+=2;
        	}
            if (unum > 9)
            {
                int val = ((int)unum) * 2;
                string[i--] = digitsArray[val + 1];
                string[i--] = digitsArray[val];
                sz += 2;
            }
            else
            {
                string[i--] = '0' + ((int)unum);
                sz++;
            }
            break;
        case 8:
        	while (unum) {
           		string[i--] = (char)((unum % 8)+ '0');
        		unum /= 8;
        		sz++;
        	}
            break;
        case 2:
        	while (unum) {
           		string[i--] = (char)((unum % 2)+ '0');
        		unum /= 2;
        		sz++;
        	}
            break;
        case 16:
        	while (unum) {
           		string[i] = (char)((unum % 16)+ '0');
        		if (string[i] > '9') {
        			string[i] += 7;
        			if (lc)
        				string[i] += 32;
        		}
                i--;
        		unum /= 16;
        		sz++;
        	}
            break;
        default:
        	while (unum) {
           		string[i] = (char)((unum % radix)+ '0');
        		if (string[i] > '9') {
        			string[i] += 7;
        			if (lc)
        				string[i] += 32;
        		}
        		i--;
        		unum /= radix;
        		sz++;
        	}
            break;
    }
	if (sz == 0)
		return &string[NUM_SIZE - 2] ;
        
	return &string[i+1];
}
static void justifiedOutput(FILE *__stream, char *buf, int ljustify, int width, 
							int pad, int *restrict written)
{
	int xx = 0;
	(*written) += width;
	if (strlen(buf) > width)
	{
		xx = buf[width];
		buf[width] = 0;
	}
	if (ljustify)
	{
		int l;
		PUTSTR(buf);
		for (l=strlen(buf); l < width; l++)
			PUTCH(pad);
	}
	else
	{
		int l;
		for (l=strlen(buf); l < width; l++)
			PUTCH(pad);
		PUTSTR(buf);
	}
	if (xx)
	{
		buf[width] = xx;
	}
}
char *__onetostr(FILE *__stream, const char *restrict format, void *restrict arg,
        int *restrict count, int *restrict written)
{
   LLONG_TYPE c ;
   int ch ;
   char * sz;
   int fxs;
   int temp, signch;
	int *p;
    int lc = 0x20;
	int looping = 1;
	int issigned = 0, ljustify = 0, spaced = 0, prefixed = 0;
	int	leadzero = 0;
	int width = 0;
	int prec = -1;
	int mode = 0;
	int i;
	int isf=1, isg=0;
	long double fmant;
	int fsign,fexp;
	char *fbuf, *fbp, *xxx;
   int type, fbufsize;
    NUMERIC_DATA *nd = __locale_data[LC_NUMERIC];
	char locbuf[NUM_SIZE];
	while (looping) 
		switch (*format) {
			case '+':	issigned = 1;
					format++;
					break;
			case '-':	ljustify = 1;
					format++;
					break;
			case ' ': spaced = 1;
					format++;
					break;
			case '#': prefixed = 1;
					format++;
					break;
			case '0':	leadzero = 1;
					format++;
					break;
			default:
					looping = 0;
		}
    if (ljustify)
        leadzero = 0;
    if (issigned)
        spaced = 0;
	if (isdigit(*format)) {
		width = 0;
		while (isdigit(*format)) {
			width *= 10;
			width += *format++ - '0';
		}
	}
	else if (*format == '*') {
		width = *((int *)arg)++;
        if (width < 0) {
            width = -width;
            ljustify = 1;
        }
		(*count)++;
		format++;
	}
	if (*format == nd->decimal_point[0]) {
		format++;
        if (*format == '*') {
            prec = *((int *)arg)++;
            if (prec <0)
                prec=-1;
            (*count)++ ;
            format++;
        } else {
    		prec = 0;
	    	while (isdigit(*format)) {
		    	prec *= 10;
			    prec += *format++ - '0';
            }
		}
	}
   if (*format == 'h' || *format == 'l') {
		mode = *format++;
      if (mode !='h') {
        if (*format == 'l') {
            mode = '1' ;
            format++ ;
        }
      } else if (*format == 'h') {
        mode = 'c';
        format++;
      }
   }
   else if (*format == 'I' && *(format + 1) == '6' && *(format + 2) == '4') {
     mode = '1';
     format += 3;
   }
    else if (*format == 'j') // intmax_t
        mode = '1', format++;
    else if (*format == 'L')
        mode = *format++;
    else if (*format == 'z' || *format == 't') /* size_t && ptrdiff_t */
        format++;
   type = *format++ ;
   if (! (type & 0x20)) {
     lc = 0;
     type |= 0x20;
   }
   fbufsize = width + prec + 128;
   if (fbufsize < 4096)
       fbufsize = 4096;
   fbuf = alloca(fbufsize);
   switch (type) {
		case '%':
			PUTCH('%');
			(*written)++;
			break;
		case 'c':
            if (mode == 'l') {
                mbstate_t st;
                int l;
                memset(&st,0,sizeof(st));
                l = wcrtomb(fbuf,(wchar_t)*(wint_t *)arg,&st);
                (*count)++;
                if (width <l)
                    width = l;
                fbuf[l] = 0;
				justifiedOutput(__stream, fbuf, ljustify, width, ' ', written);
            } else {
                if (width < 1)
                    width = 1;
		    	(*count)++;
                ch = *(int *)arg;
				if (!ljustify && width > 1)
					justifiedOutput(__stream, "", 0, width-1, ' ', written);
				PUTCH(ch);
				(*written)++;
				if (ljustify && width > 1)
					justifiedOutput(__stream, "", 1, width-1, ' ', written);
            }
			break;
        case 'a':
            if (mode == 'L') {
                (*count)+=3 ;
				fmant = *(long double *)arg;
			}
			else {
				(*count)+=2;
				fmant = *(double *)arg;
			}
            c = *(unsigned long long *) &fmant;
			fbp = fbuf;
            if (isnan(fmant))
                goto donan ;
            if (isinf(fmant))
                goto doinf;
            fsign = signbit(fmant) ? -1 : 1;
			if (issigned || spaced || fsign < 0)
				if (fsign < 0)
					*fbp++='-';
				else if (spaced)
                    *fbp++ = ' ';
                else
					*fbp++='+';
            *fbp++ = '0' ;
            *fbp++ = 'X' + lc;
            xxx = fbp;
            fexp = 0;
            *fbp = (char)(c >> 63) & 1;
            c <<= 1;
            while (c && *fbp == 0) {  /* Skip leading 0s, not true 0 */
                *fbp = (char)(c >> 63) & 1;
                c <<= 1;
                fexp--;
            }
            *fbp++ += '0';
            if (prefixed || prec != 0)
                *fbp++ = nd->decimal_point[0] ;
            while (c && prec) {
                temp = ((char)(c >> 60) & 15) + '0';
                if (temp > '9')
                    temp += 7 + lc;
                *fbp++ = temp;
                c <<= 4 ;
                prec--;
            }
            while (prec > 0) {
                *fbp++ = '0';
                prec--;
            }
            /* Rounding */
            if (c < 0) {
                *fbp-- = 0;
                while (fbp != fbuf && *fbp != ('X' + lc)) {
                    if (*fbp == nd->decimal_point[0])
                        fbp--;
                    (*fbp)++;
                    if (*fbp =='g' || *fbp == 'G')
                        *fbp-- = '0';
                    else {
                        if (*fbp == '9'+1)
                            *fbp = 'A' + lc;
                        break ;
                    }
                }
                fbp += strlen(fbp);
                if (*xxx == '2') {  /* Adjust leading digit */
                    *xxx = '1';
                    fexp++;
                }
            }
		  // get rid of decimal point, when it is last char
		  if (!prefixed && fbp[-1] == '.')
		    *(--fbp) = 0;
            *fbp++ = 'P' + lc ;
            c = *(short *)(((char *)&fmant)+ 8);
            c &= 0x7fff;
            if (c) c -= 0x3fff;  /* Normal */
            else if (fexp) c -= 0x3ffe;  /* Subnormal */
            c += fexp;
            if (c < 0) {
                c = - c;
                *fbp++ = '-';
            } else
                *fbp++ = '+';
            sz = getnum(locbuf,c,10,0,1);
            strcpy(fbp,sz);
			i = strlen(fbuf);
            if (width < i)
                width = i;
                
			justifiedOutput(__stream, fbuf, ljustify, width, ' ', written);
            break ;            
		case 'g':
			isg = 1;
			goto dofloat;
		case 'e':
			isf = 0;
			goto dofloat;
		case 'f':
        {
dofloat:
             if (mode == 'L') {
                (*count)+=3 ;
				fmant = *(long double *)arg;
			 }
			 else {
				(*count)+=2;
				fmant = *(double *)arg;
			 }
#ifndef USE_FLOAT
			PUTSTR("(FP not linked)");
			(*written)+=strlen("(FP not linked)");
#else
    		fbp = fbuf;
            if (isnan(fmant)) {

                float f ;
                int d ;
donan:               
                fsign = signbit(fmant) ? -1 : 1;
                if (issigned || spaced || fsign < 0)
                    if (fsign < 0)
                        *fbp++='-';
                    else if (spaced)
                        *fbp++ = ' ';
                    else
                        *fbp++='+';
                if (lc)
    				strcpy(fbp,"nan");
                else
                    strcpy(fbp,"NAN");
#if 0  /* Display of first 5 digits of significand */
                fbp += 3;
                f = fmant;
                d = *(int *)&f;
                d >>= 3 ;
                d &= 0xfffff ;
                sz = getnum(locbuf,d,16,lc,1);
                if (strlen(sz) < 5)
                    sz -= 5 - strlen(sz);
                *fbp++ = '(';
                strcpy(fbp,sz);
                fbp += 5;
                *fbp++ = ')';
                *fbp = 0;
#endif
            } else if (isinf(fmant)) {
doinf:
                fsign = signbit(fmant) ? -1 : 1;
                if (issigned || spaced || fsign < 0)
                    if (fsign < 0)
                        *fbp++='-';
                    else if (spaced)
                        *fbp++ = ' ';
                    else
                        *fbp++='+';
                if (lc)
    				strcpy(fbp,"inf");
                else
                    strcpy(fbp,"INF");
            } else {
                int bcdIndex = 0, isZero;
                unsigned char bcdBuf[32];
                int exp;
                
                if (mode == 'L')
     			    isZero = fextract(&fmant,&fexp,&fsign, bcdBuf);
                else
                {
                    isZero = fextractdouble(*(double *)arg, &fexp, &fsign, bcdBuf);
                    if (fexp == INT_MAX)
         			    isZero = fextract(&fmant,&fexp,&fsign, bcdBuf);
                }
				/* sign */
				if (issigned || spaced || fsign < 0)
					if (fsign < 0)
						*fbp++='-';
					else if (spaced)
                        *fbp++ = ' ';
                    else
						*fbp++='+';

                {
					if (fexp > fbufsize-1 || fexp < -(fbufsize-1))
					{
						isf = 0;
					}
					/* adjust for negative exponents, I was too lazy to do this in ASM */
/*
					if (fexp < 0) {
                        bcdIndex++;
						fexp--;
					}
*/

                    if (isg) 
                    {
                        if (prec == 0)
                            prec = 1;
                    }
					if (prec == -1)
						prec = 6;

					/* decide on g format */
                    if (isg)
					{
                        int all9 = 0;
                        if (fexp >= 0 && fexp == prec-1)
                        {
                            int i = 0;
                            do {
                                all9 = fnd(bcdBuf, i++) == 9;
                            } while (all9 && i < prec);
                        }
                        isf = fexp < prec && fexp > -5 && !all9;
					}

					xxx = fbp;		/* pointer to the roundup digit */
					*fbp++ = '0';						/* in case of round from 9.99999999.... */

					/* mantissa */
					if (isf) {
                        int digits = prec;
						if (fexp < 0) {
							*fbp++ = '0';
                            if (prefixed || prec > 0)
      							*fbp++ = nd->decimal_point[0];
                                {
									int use_fnd = 0, sd_found = 0;
    		    					if (!isg && fexp < -prec-1)
									{
	    		    					fexp = -prec-1;
									}
									else
										use_fnd = 1;
		    		    			for (i=fexp; i < -1 && digits; i++)
			    		    		{ *fbp++ = '0'; if (!isg) digits--; }
				    		    	while (digits)
									{
					    		    	*fbp++ = fnd(bcdBuf,bcdIndex++) + '0';
								if (!sd_found && isg && *(fbp - 1) != '0') sd_found++;
								if (!isg) digits--;
								if (sd_found) digits--;
									}
									if (use_fnd)
										i = fnd(bcdBuf,bcdIndex++);
									else
										i = 0;
                                }
						}
						else {
							*fbp++ = fnd(bcdBuf,bcdIndex++) + '0';
							if (isg && *(fbp - 1) != '0') digits --;
							while (fexp-- > 0 && (digits || !isg)) {
								*fbp++ = fnd(bcdBuf,bcdIndex++) + '0';
                                if (isg) digits--;
							}
                            if (!isg && prec > 0 || isg && digits || prefixed)
    							*fbp++ = nd->decimal_point[0];
							while (digits--)
								*fbp++ = fnd(bcdBuf,bcdIndex++) + '0';
							i = fnd(bcdBuf,bcdIndex++);
						}
					}
					else {
                        int digits = prec;
						*fbp++ = fnd(bcdBuf,bcdIndex++) + '0';
                        if (isg && *(fbp - 1) != '0') digits--;
						if (!isZero)
							while (!isg && *(fbp-1) == '0')
							{
								fexp--;
								*(fbp-1) = fnd(bcdBuf, bcdIndex++) + '0';
							}
                        if (!isg && prec > 0 || isg && digits || prefixed)
   							*fbp++ = nd->decimal_point[0];
						while (digits--)
							*fbp++ = fnd(bcdBuf,bcdIndex++) + '0';
						i = fnd(bcdBuf,bcdIndex++);
					}
		
					/* Rounding */
					if (i >4) {
						char *fbp2=fbp-1;
						while (fbp2 >= fbuf) {
							if (*fbp2 < '0') {
								fbp2--;
								continue;
							}
							(*fbp2)++;
							if (*fbp2 != 0x3a)
								break;
							(*fbp2--)='0';
						}
					}

					/* check for round out of first digit */
					*fbp = 0;
					if (*xxx == '0') {
						strcpy(xxx,xxx+1);	/* nope, blit it out */
						fbp--;
					}
					else {
						if (!isf) {
							*(xxx+1) = nd->decimal_point[0];	/* yes, readjust decimal */
							*(xxx+2) = '0';
							if (!isg)
								fbp--;	/* fix prec */
							fexp++;	/* fix exponent */
						}
						
					}
					/* zerowipe */
                    *fbp = 0;
                    if (isg && !prefixed && strchr(fbuf,nd->decimal_point[0]))
    					while (*(fbp-1) == '0')
	    					fbp--;
    
					// get rid of decimal point in G mode, when it is last char
					if (isg && !prefixed)
					{
						if (fbp[-1] == '.')
							*(--fbp) = 0;
					}
					/* exponent */
					if (!isf) {
						*fbp++ = 'E' + lc;
						if (fexp < 0) {
							fexp=-fexp;
							*fbp++='-';
						}
						else
							*fbp++='+';
                        sz = getnum(locbuf,fexp,10,0,0);
                        if (strlen(sz) < (isg ? 2 : _e_min_exp_digit))
                        {
                            int n = (isg ? 2 : _e_min_exp_digit) - strlen(sz);
                            for (int i=0; i < n; i++)
                                *(--sz) = '0';
                        }
                        strcpy(fbp,sz);
                        fbp += strlen(fbp);
					}
					*fbp++ = 0;
					/* if -0.000000..., wipe out the minus */
#if 0  /* Removed to display signed 0 */
					if (fbuf[0] == '-')
					{
						char *fpx = fbuf + 1;
						while (*fpx && *fpx == '.' || *fpx == '0')
							fpx++;
						if (!*fpx)
							strcpy(fbuf, fbuf + 1);
					}
#endif
				}
			}
			i = strlen(fbuf);
            if (width < i)
                width = i;
			justifiedOutput(__stream, fbuf, ljustify, width, ' ', written);
               
#endif
        }
			break;
		case 'n':
			(*count)++;
			p = *(int **)arg;
         if (mode == '1')
			*(LLONG_TYPE *)p = (int)(*written);
         else if (mode == 'l')
			*(long *)p = (int)(*written);
         else if (mode == 'h')
			*(short *)p = (int)(*written);
         else if (mode == 'c')
			*(char *)p = (int)(*written);
         else
			*(int *)p = (int)(*written);
			break;
		case 's':
            if (mode == 'l') {
                char *p;
                mbstate_t st;
                wchar_t *ss ;
                int l=0,next = 0, inc = -1;
                memset(&st,0,sizeof(st));
                (*count)++;
                ss = *(wchar_t **)arg;
                if (ss == NULL)
                    ss = L"(null)";
                p = ss;
                if (prec < 0)
                    prec = INT_MAX;
                while (l + next <= prec) {
                    l += next ;
                    inc++;
                    if (!*ss)
                        break;
                    next = wcrtomb(fbuf,*ss++,&st);
                }
                if (width < l)
                    width = l;
				if (l)
				{
					if (!ljustify)
						justifiedOutput(__stream, "", 0, width-l, ' ', written);
		            memset(&st,0,sizeof(st));
                        ss = p;
        	        for (i=0; i < inc; i++) {
            	        int l = wcrtomb(fbuf,*ss++,&st);
						fbuf[l] = 0;
						PUTSTR(fbuf);
						(*written) += strlen(fbuf);
                	}
					if (ljustify)
						justifiedOutput(__stream, "", 1, width-l, ' ', written);
				}
            } else {
				int hold = 0;
			    (*count)++;
                xxx = *(char **)arg ;
                if (xxx == NULL)
                    xxx = "(null)";
    			fxs = strlen(xxx);
    			if (prec > 0 && prec < fxs)
				{
					hold = xxx[fxs];
					xxx[fxs] = 0;
    				fxs = prec;
				}
                if (width < fxs)
                    width = fxs ;
				justifiedOutput(__stream, xxx, ljustify, width, ' ', written);
				if (hold)
					xxx[fxs] = hold;
            }
			break;
        case 'p':
         leadzero = 1 ;
         prefixed = 1 ;
         type = 'x';
         if (!width)
            width = 8 ;
		case 'x':
		case 'u':
		case 'o':
        case 'd':
        case 'i':
        case 'b':
            if (prec == -1)
                prec = 1;
			(*count)++;
         if (mode == '1' || mode == 'L') {
            c = *(LLONG_TYPE *)arg ;
            (*count)++ ;
         } else if (mode == 'l') {
            c = *(long *)arg;
            if (type != 'd' && type != 'i')
                c = (unsigned long)c;
         } else {
            c = *(int *)arg ;
            if (type != 'd' && type != 'i')
                c = (unsigned)c;
         }
         if (mode == 'h') {
            c = (short)c ;
            if (type != 'd' && type != 'i')
                c = (unsigned short)c;
         } else if (mode == 'c') {
            c = (char)c;
            if (type != 'd' && type != 'i')
                c = (unsigned char)c;
         }
			sz = getnum(locbuf,c,(type == 'o') ? 8 : (type == 'x') ? 16 : (type == 'b') ? 2 : 10,
                        lc,type == 'x' || type == 'o' || type == 'u' || type == 'b');
            if (prec == 0 && strlen(sz) == 1 && sz[0] == '0')
                sz++;
            else if (strlen(sz) < prec)
                sz -= prec - strlen(sz);
            signch = -1 ;
            if (type != 'x' && type != 'u' && type != 'o' && type != 'b')
    			if (issigned) {
	    			if (c < 0) 
		    			signch = '-';
			    	else
				    	signch = '+';
    			}
	    		else if (spaced) {
		    		if (c < 0) 
			    		signch = '-';
				    else
					    signch = ' ';
			    }
			    else {
				    if (c < 0) {
					    signch = '-';
				    }
			    }
            temp = strlen(sz) ;
            if (signch != -1 && !leadzero)
                temp++;
            if (prefixed)
                if ((type == 'o' || type == 'b') && sz[0] != '0')
                    temp++;
                else if ((type == 'x') && c)
                    temp += 2 ;
            if (temp > width)
                width = temp;
            if (signch != -1)
			{
				if (leadzero)
				{
					PUTCH(signch);
					(*written)++;
				}
				else
				{
					*(--sz) = signch;
					temp++;
				}
			}
            if (prefixed)
                if ((type == 'o' || type == 'b') && sz[0] != '0')
				{
					PUTCH('0');
					(*written)++;
				}
                else if ((type == 'x') && c)
				{
					PUTCH('0');
					PUTCH('X');
					(*written)+=2;
				}
			justifiedOutput(__stream, sz, ljustify, width, leadzero ? '0' : ' ', written);
			break;
		default:
			PUTCH(format[-1]);
			(*written)++;
			break;
	}
	return(format);
}
