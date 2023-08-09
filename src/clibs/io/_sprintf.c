/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
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
#include <limits.h>
#include "malloc.h"
#include <stdlib.h>

int _e_min_exp_digit = 2; /* min exp digits for %e (stdc, MS is 3) */

char* dtoa(double d, int mode, int ndigits, int* decpt, int* sign, char** rve);

int fextractdouble(double fmant, int* fexp, int* fsign, unsigned char* buf)
{
    *fsign = fmant < 0 ? -1 : 1;
    fmant = fabsl(fmant);
    int temp;
    char* s = dtoa(fmant, 2, 80, fexp, &temp, NULL);
    (*fexp)--;
    strcpy(buf, s);
    freedtoa(s);
    return fmant == 0.0;
}

#ifdef _i386_
#    define USE_FLOAT
#endif

#define NUM_SIZE 100
#define PUTCH(x) fputc_unlocked(x, __stream)
#define PUTSTR(x) fputs_unlocked(x, __stream)

int fnd(unsigned char* buf, int index)
{
    if (index >= strlen(buf))
        return 0;
    return buf[index] - '0';
}

static char* getnum(char* string, LLONG_TYPE num, int radix, int lc, int unsign)
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
    int i, sz = 0;
    unsigned LLONG_TYPE unum;
    string[NUM_SIZE - 2] = '0';
    string[NUM_SIZE - 1] = 0;
    if (num < 0 && !unsign)
        unum = (unsigned LLONG_TYPE) - num;
    else
        unum = num;
    i = NUM_SIZE - 2;
    switch (radix)
    {
        case 10:
            while (unum >= 100)
            {
                /*
                        // lldiv_t doesn't work for large unsigneds...
                                lldiv_t aa = lldiv(unum, 100);
                                int val = ((int)aa.rem) * 2;
                */
                int val = (int)(unum % 100) * 2;
                unum = unum / 100;
                string[i--] = digitsArray[val + 1];
                string[i--] = digitsArray[val];
                sz += 2;
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
            while (unum)
            {
                string[i--] = (char)((unum % 8) + '0');
                unum /= 8;
                sz++;
            }
            break;
        case 2:
            while (unum)
            {
                string[i--] = (char)((unum % 2) + '0');
                unum /= 2;
                sz++;
            }
            break;
        case 16:
            while (unum)
            {
                string[i] = (char)((unum % 16) + '0');
                if (string[i] > '9')
                {
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
            while (unum)
            {
                string[i] = (char)((unum % radix) + '0');
                if (string[i] > '9')
                {
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
        return &string[NUM_SIZE - 2];

    return &string[i + 1];
}
static void PutPrefix(FILE* __stream, int prefixChar)
{
    if (prefixChar)
    {
         PUTCH('0');
         if (prefixChar != '0')
            PUTCH(prefixChar);
    }
}
static void justifiedOutput(FILE* __stream, char* buf, int ljustify, int swidth, int width, int pad, int* restrict written, int prefixChar)
{
    (*written) += width;
    if (swidth > width)
        swidth = width;
    int n = 0;
    if (prefixChar == '0')
    {
        n = 1;
    }
    else if (prefixChar == 'x' || prefixChar == 'X' || prefixChar == 'b' || prefixChar == 'B')
    {
        n = 2;
    }
    if (ljustify)
    {
        int l;
        PutPrefix(__stream, prefixChar);
        for (l = 0; l < swidth; l++)
            PUTCH(buf[l]);
        for (l = swidth+n; l < width; l++)
            PUTCH(pad);
    }
    else
    {
        int l;
        if (pad == '0')
            PutPrefix(__stream, prefixChar);
        for (l = swidth+n; l < width; l++)
            PUTCH(pad);
        if (pad != '0')
            PutPrefix(__stream, prefixChar);
        for (l = 0; l < swidth; l++)
            PUTCH(buf[l]);
    }
}
char* __onetostr(FILE* __stream, const char* restrict format, void* restrict arg, int* restrict count, int* restrict written)
{
    LLONG_TYPE c;
    int ch;
    char* sz;
    int fxs;
    int temp, signch;
    int* p;
    int lc = 0x20;
    int looping = 1;
    int issigned = 0, ljustify = 0, spaced = 0, prefixed = 0;
    int leadzero = 0;
    int width = 0;
    int prec = -1;
    int mode = 0;
    int i;
    int isf = 1, isg = 0;
    long double fmant;
    int fsign, fexp;
    char *fbuf, *fbp, *xxx;
    int type, fbufsize;
    NUMERIC_DATA* nd = __locale_data[LC_NUMERIC];
    char locbuf[NUM_SIZE];
    while (looping)
        switch (*format)
        {
            case '+':
                issigned = 1;
                format++;
                break;
            case '-':
                ljustify = 1;
                format++;
                break;
            case ' ':
                spaced = 1;
                format++;
                break;
            case '#':
                prefixed = 1;
                format++;
                break;
            case '0':
                leadzero = 1;
                format++;
                break;
            default:
                looping = 0;
        }
    if (ljustify)
        leadzero = 0;
    if (issigned)
        spaced = 0;
    if (isdigit(*format))
    {
        width = 0;
        while (isdigit(*format))
        {
            width *= 10;
            width += *format++ - '0';
        }
    }
    else if (*format == '*')
    {
        width = *((int*)arg)++;
        if (width < 0)
        {
            width = -width;
            ljustify = 1;
        }
        (*count)++;
        format++;
    }
    if (*format == nd->decimal_point[0])
    {
        format++;
        if (*format == '*')
        {
            prec = *((int*)arg)++;
            if (prec < 0)
                prec = -1;
            (*count)++;
            format++;
        }
        else
        {
            prec = 0;
            while (isdigit(*format))
            {
                prec *= 10;
                prec += *format++ - '0';
            }
        }
    }
    if (*format == 'h' || *format == 'l')
    {
        mode = *format++;
        if (mode != 'h')
        {
            if (*format == 'l')
            {
                mode = '1';
                format++;
            }
        }
        else if (*format == 'h')
        {
            mode = 'c';
            format++;
        }
    }
    else if (*format == 'I' && *(format + 1) == '6' && *(format + 2) == '4')
    {
        mode = '1';
        format += 3;
    }
    else if (*format == 'j')  // intmax_t
        mode = '1', format++;
    else if (*format == 'L')
        mode = *format++;
    else if (*format == 'z' || *format == 't') /* size_t && ptrdiff_t */
        format++;
    type = *format++;
    if (!(type & 0x20))
    {
        lc = 0;
        type |= 0x20;
    }
    fbufsize = width + prec + 128;
    if (fbufsize < 4096)
        fbufsize = 4096;
    fbuf = alloca(fbufsize);
    switch (type)
    {
        case '%':
            PUTCH('%');
            (*written)++;
            break;
        case 'c':
            if (mode == 'l')
            {
                mbstate_t st;
                int l;
                memset(&st, 0, sizeof(st));
                l = wcrtomb(fbuf, (wchar_t) * (wint_t*)arg, &st);
                (*count)++;
                if (width < l)
                    width = l;
                fbuf[l] = 0;
                justifiedOutput(__stream, fbuf, ljustify, strlen(fbuf), width, ' ', written, 0);
            }
            else
            {
                if (width < 1)
                    width = 1;
                (*count)++;
                ch = *(int*)arg;
                if (!ljustify && width > 1)
                    justifiedOutput(__stream, "", 0, 0, width - 1, ' ', written, 0);
                PUTCH(ch);
                (*written)++;
                if (ljustify && width > 1)
                    justifiedOutput(__stream, "", 1, 0, width - 1, ' ', written, 0);
            }
            break;
        case 'a':
            if (mode == 'L')
            {
                (*count) += 2;
                fmant = *(long double*)arg;
            }
            else
            {
                (*count) += 2;
                fmant = *(double*)arg;
            }
            c = *(unsigned long long*)&fmant;
            fbp = fbuf;
            if (isnan(fmant))
                goto donan;
            if (isinf(fmant))
                goto doinf;
            fsign = signbit(fmant) ? -1 : 1;
            if (issigned || spaced || fsign < 0)
                if (fsign < 0)
                    *fbp++ = '-';
                else if (spaced)
                    *fbp++ = ' ';
                else
                    *fbp++ = '+';
            *fbp++ = '0';
            *fbp++ = 'X' + lc;
            xxx = fbp;
            fexp = 0;
            *fbp = (char)(c >> 63) & 1;
            c <<= 1;
            while (c && *fbp == 0)
            { /* Skip leading 0s, not true 0 */
                *fbp = (char)(c >> 63) & 1;
                c <<= 1;
                fexp--;
            }
            *fbp++ += '0';
            if (prefixed || prec != 0)
                *fbp++ = nd->decimal_point[0];
            while (c && prec)
            {
                temp = ((char)(c >> 60) & 15) + '0';
                if (temp > '9')
                    temp += 7 + lc;
                *fbp++ = temp;
                c <<= 4;
                prec--;
            }
            while (prec > 0)
            {
                *fbp++ = '0';
                prec--;
            }
            /* Rounding */
            if (c < 0)
            {
                *fbp-- = 0;
                while (fbp != fbuf && *fbp != ('X' + lc))
                {
                    if (*fbp == nd->decimal_point[0])
                        fbp--;
                    (*fbp)++;
                    if (*fbp == 'g' || *fbp == 'G')
                        *fbp-- = '0';
                    else
                    {
                        if (*fbp == '9' + 1)
                            *fbp = 'A' + lc;
                        break;
                    }
                }
                fbp += strlen(fbp);
                if (*xxx == '2')
                { /* Adjust leading digit */
                    *xxx = '1';
                    fexp++;
                }
            }
            // get rid of decimal point, when it is last char
            if (!prefixed && fbp[-1] == '.')
                *(--fbp) = 0;
            *fbp++ = 'P' + lc;
            c = *(short*)(((char*)&fmant) + 8);
            c &= 0x7fff;
            if (c)
                c -= 0x3fff; /* Normal */
            else if (fexp)
                c -= 0x3ffe; /* Subnormal */
            c += fexp;
            if (c < 0)
            {
                c = -c;
                *fbp++ = '-';
            }
            else
                *fbp++ = '+';
            sz = getnum(locbuf, c, 10, 0, 1);
            strcpy(fbp, sz);
            i = strlen(fbuf);
            if (width < i)
                width = i;

            justifiedOutput(__stream, fbuf, ljustify, strlen(fbuf), width, ' ', written, 0);
            break;
        case 'g':
            isg = 1;
            goto dofloat;
        case 'e':
            isf = 0;
            goto dofloat;
        case 'f':
        {
        dofloat:
            if (mode == 'L')
            {
                (*count) += 2;
                fmant = *(long double*)arg;
            }
            else
            {
                (*count) += 2;
                fmant = *(double*)arg;
            }
#ifndef USE_FLOAT
            PUTSTR("(FP not linked)");
            (*written) += strlen("(FP not linked)");
#else
            fbp = fbuf;
            if (isnan(fmant))
            {

                float f;
                int d;
            donan:
                fsign = signbit(fmant) ? -1 : 1;
                if (issigned || spaced || fsign < 0)
                    if (fsign < 0)
                        *fbp++ = '-';
                    else if (spaced)
                        *fbp++ = ' ';
                    else
                        *fbp++ = '+';
                if (lc)
                    strcpy(fbp, "nan");
                else
                    strcpy(fbp, "NAN");
#    if 0 /* Display of first 5 digits of significand */
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
#    endif
            }
            else if (isinf(fmant))
            {
            doinf:
                fsign = signbit(fmant) ? -1 : 1;
                if (issigned || spaced || fsign < 0)
                    if (fsign < 0)
                        *fbp++ = '-';
                    else if (spaced)
                        *fbp++ = ' ';
                    else
                        *fbp++ = '+';
                if (lc)
                    strcpy(fbp, "inf");
                else
                    strcpy(fbp, "INF");
            }
            else
            {
                int bcdIndex = 0, isZero;
                unsigned char bcdBuf[100];
                int exp;

                isZero = fextractdouble(*(double*)arg, &fexp, &fsign, bcdBuf);
                /* sign */
                if (issigned || spaced || fsign < 0)
                    if (fsign < 0)
                        *fbp++ = '-';
                    else if (spaced)
                        *fbp++ = ' ';
                    else
                        *fbp++ = '+';

                {
                    if (fexp > fbufsize - 1 || fexp < -(fbufsize - 1))
                    {
                        isf = 0;
                    }
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
                        if (fexp >= 0 && fexp == prec - 1)
                        {
                            int i = 0;
                            do
                            {
                                all9 = fnd(bcdBuf, i++) == 9;
                            } while (all9 && i < prec);
                        }
                        isf = fexp < prec && fexp > -5 && !all9;
                    }

                    xxx = fbp;    /* pointer to the roundup digit */
                    *fbp++ = '0'; /* in case of round from 9.99999999.... */

                    /* mantissa */
                    if (isf)
                    {
                        int digits = prec;
                        if (fexp < 0)
                        {
                            *fbp++ = '0';
                            if (prefixed || prec > 0)
                                *fbp++ = nd->decimal_point[0];
                            {
                                int use_fnd = 0, sd_found = 0;
                                if (!isg && fexp < -prec - 1)
                                {
                                    fexp = -prec - 1;
                                }
                                else
                                    use_fnd = 1;
                                for (i = fexp; i < -1 && digits; i++)
                                {
                                    *fbp++ = '0';
                                    if (!isg)
                                        digits--;
                                }
                                while (digits)
                                {
                                    *fbp++ = fnd(bcdBuf, bcdIndex++) + '0';
                                    if (!sd_found && isg && *(fbp - 1) != '0')
                                        sd_found++;
                                    if (!isg)
                                        digits--;
                                    if (sd_found)
                                        digits--;
                                }
                                if (use_fnd)
                                    i = fnd(bcdBuf, bcdIndex++);
                                else
                                    i = 0;
                            }
                        }
                        else
                        {
                            *fbp++ = fnd(bcdBuf, bcdIndex++) + '0';
                            if (isg && *(fbp - 1) != '0')
                                digits--;
                            while (fexp-- > 0 && (digits || !isg))
                            {
                                *fbp++ = fnd(bcdBuf, bcdIndex++) + '0';
                                if (isg)
                                    digits--;
                            }
                            if (!isg && prec > 0 || isg && digits || prefixed)
                                *fbp++ = nd->decimal_point[0];
                            while (digits--)
                                *fbp++ = fnd(bcdBuf, bcdIndex++) + '0';
                            i = fnd(bcdBuf, bcdIndex++);
                        }
                    }
                    else
                    {
                        int digits = prec;
                        *fbp++ = fnd(bcdBuf, bcdIndex++) + '0';
                        if (isg && *(fbp - 1) != '0')
                            digits--;
                        if (!isZero)
                            while (!isg && *(fbp - 1) == '0')
                            {
                                fexp--;
                                *(fbp - 1) = fnd(bcdBuf, bcdIndex++) + '0';
                            }
                        if (!isg && prec > 0 || isg && digits || prefixed)
                            *fbp++ = nd->decimal_point[0];
                        while (digits--)
                            *fbp++ = fnd(bcdBuf, bcdIndex++) + '0';
                        i = fnd(bcdBuf, bcdIndex++);
                    }

                    /* Rounding */
                    if (i > 4)
                    {
                        char* fbp2 = fbp - 1;
                        while (fbp2 >= fbuf)
                        {
                            if (*fbp2 < '0')
                            {
                                fbp2--;
                                continue;
                            }
                            (*fbp2)++;
                            if (*fbp2 != 0x3a)
                                break;
                            (*fbp2--) = '0';
                        }
                    }

                    /* check for round out of first digit */
                    *fbp = 0;
                    if (*xxx == '0')
                    {
                        strcpy(xxx, xxx + 1); /* nope, blit it out */
                        fbp--;
                    }
                    else
                    {
                        if (!isf)
                        {
                            *(xxx + 1) = nd->decimal_point[0]; /* yes, readjust decimal */
                            *(xxx + 2) = '0';
                            if (!isg)
                                fbp--; /* fix prec */
                            fexp++;    /* fix exponent */
                        }
                    }
                    /* zerowipe */
                    *fbp = 0;
                    if (isg && !prefixed && strchr(fbuf, nd->decimal_point[0]))
                        while (*(fbp - 1) == '0')
                            fbp--;

                    // get rid of decimal point in G mode, when it is last char
                    if (isg && !prefixed)
                    {
                        if (fbp[-1] == '.')
                            *(--fbp) = 0;
                    }
                    /* exponent */
                    if (!isf)
                    {
                        *fbp++ = 'E' + lc;
                        if (fexp < 0)
                        {
                            fexp = -fexp;
                            *fbp++ = '-';
                        }
                        else
                            *fbp++ = '+';
                        sz = getnum(locbuf, fexp, 10, 0, 0);
                        if (strlen(sz) < (isg ? 2 : _e_min_exp_digit))
                        {
                            int n = (isg ? 2 : _e_min_exp_digit) - strlen(sz);
                            for (int i = 0; i < n; i++)
                                *(--sz) = '0';
                        }
                        strcpy(fbp, sz);
                        fbp += strlen(fbp);
                    }
                    *fbp++ = 0;
                    /* if -0.000000..., wipe out the minus */
#    if 0 /* Removed to display signed 0 */
					if (fbuf[0] == '-')
					{
						char *fpx = fbuf + 1;
						while (*fpx && *fpx == '.' || *fpx == '0')
							fpx++;
						if (!*fpx)
							strcpy(fbuf, fbuf + 1);
					}
#    endif
                }
            }
            i = strlen(fbuf);
            if (width < i)
                width = i;
            justifiedOutput(__stream, fbuf, ljustify, strlen(fbuf), width, ' ', written, 0);

#endif
        }
        break;
        case 'n':
            (*count)++;
            p = *(int**)arg;
            if (mode == '1')
                *(LLONG_TYPE*)p = (int)(*written);
            else if (mode == 'l')
                *(long*)p = (int)(*written);
            else if (mode == 'h')
                *(short*)p = (int)(*written);
            else if (mode == 'c')
                *(char*)p = (int)(*written);
            else
                *(int*)p = (int)(*written);
            break;
        case 's':
            if (!lc)
                mode = 'l';
            if (mode == 'l')
            {
                char* p;
                mbstate_t st;
                wchar_t* ss;
                int l = 0, next = 0, inc = -1;
                memset(&st, 0, sizeof(st));
                (*count)++;
                ss = *(wchar_t**)arg;
                if (ss == NULL)
                    ss = L"(null)";
                p = ss;
                if (prec < 0)
                    prec = INT_MAX;
                while (l + next <= prec)
                {
                    l += next;
                    inc++;
                    if (!*ss)
                        break;
                    next = wcrtomb(fbuf, *ss++, &st);
                }
                if (width < l)
                    width = l;
                if (l)
                {
                    if (!ljustify)
                        justifiedOutput(__stream, "", 0, 0, width - l, ' ', written, 0);
                    memset(&st, 0, sizeof(st));
                    ss = p;
                    for (i = 0; i < inc; i++)
                    {
                        int l = wcrtomb(fbuf, *ss++, &st);
                        fbuf[l] = 0;
                        PUTSTR(fbuf);
                        (*written) += strlen(fbuf);
                    }
                    if (ljustify)
                        justifiedOutput(__stream, "", 1, 0, width - l, ' ', written, 0);
                }
            }
            else
            {
                int hold = 0;
                (*count)++;
                xxx = *(char**)arg;
                if (xxx == NULL)
                    xxx = "(null)";
                int width1 = strlen(xxx);
                if (prec >= 0 && width1 > prec)
                    width1 = prec;
                if (width < width1)
                    width = width1;
                if (prec < 0)
                    prec = width;
                hold = strlen(xxx);
                if (hold < prec)
                    prec = hold;
                justifiedOutput(__stream, xxx, ljustify, prec, width, ' ', written, 0);
            }
            break;
        case 'p':
            leadzero = 1;
            prefixed = 1;
            type = 'x';
            if (!width)
                width = 8;
        case 'x':
        case 'u':
        case 'o':
        case 'd':
        case 'i':
        case 'b':
            if (prec == -1)
                prec = 1;
            (*count)++;
            if (mode == '1' || mode == 'L')
            {
                c = *(LLONG_TYPE*)arg;
                (*count)++;
            }
            else if (mode == 'l')
            {
                c = *(long*)arg;
                if (type != 'd' && type != 'i')
                    c = (unsigned long)c;
            }
            else
            {
                c = *(int*)arg;
                if (type != 'd' && type != 'i')
                    c = (unsigned)c;
            }
            if (mode == 'h')
            {
                c = (short)c;
                if (type != 'd' && type != 'i')
                    c = (unsigned short)c;
            }
            else if (mode == 'c')
            {
                c = (char)c;
                if (type != 'd' && type != 'i')
                    c = (unsigned char)c;
            }
            sz = getnum(locbuf, c, (type == 'o') ? 8 : (type == 'x') ? 16 : (type == 'b') ? 2 : 10, lc,
                        type == 'x' || type == 'o' || type == 'u' || type == 'b');
            if (prec == 0 && strlen(sz) == 1 && sz[0] == '0')
                sz++;
            else if (strlen(sz) < prec)
                for (int i = strlen(sz); i < prec; i++)
                    *--sz = '0';
            signch = -1;
            if (type != 'x' && type != 'u' && type != 'o' && type != 'b')
                if (issigned)
                {
                    if (c < 0)
                        signch = '-';
                    else
                        signch = '+';
                }
                else if (spaced)
                {
                    if (c < 0)
                        signch = '-';
                    else
                        signch = ' ';
                }
                else
                {
                    if (c < 0)
                    {
                        signch = '-';
                    }
                }
            temp = strlen(sz);
            if (signch != -1 && !leadzero)
                temp++;
            if (prefixed)
                if ((type == 'o' || type == 'b') && sz[0] != '0')
                    temp++;
                else if ((type == 'x') && c)
                    temp += 2;
            if (temp > width)
                width = temp;
            if (signch != -1)
            {
                *(--sz) = signch;
                temp++;
            }
            justifiedOutput(__stream, sz, ljustify, strlen(sz), width, leadzero ? '0' : ' ', written, prefixed ? (type & 0xdf) + lc : 0);
            break;
        default:
            PUTCH(format[-1]);
            (*written)++;
            break;
    }
    return (format);
}
