/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

#include <wctype.h>
#include <wchar.h>
#include <math.h>
#include <time.h>
#include <locale.h>
#include <_locale.h>
#include <limits.h>
#include <stdio.h>
#include "malloc.h"
#define LLONG_TYPE long long

extern int _e_min_exp_digit; /* min exp digits for %e (stdc is 2, MS is 3) */

#ifdef _i386_
#    define USE_FLOAT
#endif

#define NUM_SIZE 100

static inline void PUTCH(wchar_t x, FILE* stream)
{
    if (stream->flags & _F_BUFFEREDSTRING)
    {
        fputwc_unlocked(x, stream);
    }
    else
    {
        char buf[32];
        int n = wcrtomb(buf, x, (mbstate_t*)stream->extended->mbstate);
        if (n > 0)
        {
            buf[n] = '\0';
            fputs_unlocked(buf, stream);
        }
    }
}
static inline void PUTSTR(wchar_t* x, FILE* stream)
{
    while (*x)
        PUTCH(*x++, stream);
}

extern int fnd(unsigned char* buf, int index);
extern int fextractdouble(long double* fmant, int* fexp, int* fsign, unsigned char* buf);

static wchar_t* getnum(wchar_t* string, LLONG_TYPE num, int radix, int lc, int unsign)
{
    int i, sz = 0;
    unsigned LLONG_TYPE unum;
    for (i = 0; i < NUM_SIZE - 1; i++)
        string[i] = '0';
    string[NUM_SIZE - 1] = 0;
    if (num < 0 && !unsign)
        unum = -num;
    else
        unum = num;
    i = NUM_SIZE - 2;
    while (unum)
    {
        string[i] = (wchar_t)((unum % radix) + '0');
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
    if (sz == 0)
        return &string[NUM_SIZE - 2];

    return &string[i + 1];
}
static void justifiedOutput(FILE* __stream, wchar_t* buf, int ljustify, int width, int pad, int* restrict written)
{
    int xx = 0;
    (*written) += width;
    if (wcslen(buf) > width)
    {
        xx = buf[width];
        buf[width] = 0;
    }
    if (ljustify)
    {
        int l;
        PUTSTR(buf, __stream);
        for (l = wcslen(buf); l < width; l++)
            PUTCH(pad, __stream);
    }
    else
    {
        int l;
        for (l = wcslen(buf); l < width; l++)
            PUTCH(pad, __stream);
        PUTSTR(buf, __stream);
    }
    if (xx)
    {
        buf[width] = xx;
    }
}
wchar_t* __wonetostr(FILE* __stream, const wchar_t* restrict format, void* restrict arg, int* restrict count, int* restrict written)
{
    LLONG_TYPE c;
    int ch;
    wchar_t* sz;
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
    wchar_t *fbuf, *fbp, *xxx;
    int type, fbufsize;
    NUMERIC_DATA* nd = __locale_data[LC_NUMERIC];
    wchar_t locbuf[NUM_SIZE];
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
    if (iswdigit(*format))
    {
        width = 0;
        while (iswdigit(*format))
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
            while (iswdigit(*format))
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
    fbuf = alloca(fbufsize * sizeof(wchar_t));
    switch (type)
    {
        case '%':
            PUTCH('%', __stream);
            (*written)++;
            break;
        case 'c':
            if (width < 1)
                width = 1;
            (*count)++;
            ch = *(int*)arg;
            if (!ljustify && width > 1)
                justifiedOutput(__stream, "", 0, width - 1, ' ', written);
            PUTCH(ch, __stream);
            (*written)++;
            if (ljustify && width > 1)
                justifiedOutput(__stream, "", 1, width - 1, ' ', written);
            break;
        case 'a':
            if (mode == 'L')
            {
                (*count) += 3;
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
                fbp += wcslen(fbp);
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
            wcscpy(fbp, sz);
            i = wcslen(fbuf);
            if (width < i)
                width = i;

            justifiedOutput(__stream, fbuf, ljustify, width, ' ', written);
            break;
        case 'g':
            isg = 1;
            goto dofloat;
        case 'e':
            isf = 0;
            goto dofloat;
        case 'f':
        dofloat:
            if (mode == 'L')
            {
                (*count) += 3;
                fmant = *(long double*)arg;
            }
            else
            {
                (*count) += 2;
                fmant = *(double*)arg;
            }
#ifndef USE_FLOAT
            PUTSTR(L"(FP not linked)", __stream);
            (*written) += wcslen(L"(FP not linked)");
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
                    wcscpy(fbp, L"nan");
                else
                    wcscpy(fbp, L"NAN");
#    if 0 /* Display of first 5 digits of significand */
                fbp += 3;
                f = fmant;
                d = *(int *)&f;
                d >>= 3 ;
                d &= 0xfffff ;
                sz = getnum(locbuf,d,16,lc,1);
                if (wcslen(sz) < 5)
                    sz -= 5 - wcslen(sz);
                *fbp++ = '(';
                wcscpy(fbp,sz);
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
                    wcscpy(fbp, L"inf");
                else
                    wcscpy(fbp, L"INF");
            }
            else
            {
                int bcdIndex = 0, isZero;
                unsigned char bcdBuf[100];
                isZero = fextractdouble(&fmant, &fexp, &fsign, bcdBuf);
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
                        wcscpy(xxx, xxx + 1); /* nope, blit it out */
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
                    if (isg && !prefixed && wcschr(fbuf, nd->decimal_point[0]))
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
                        if (wcslen(sz) < (isg ? 2 : _e_min_exp_digit))
                            sz -= (isg ? 2 : _e_min_exp_digit) - wcslen(sz);
                        wcscpy(fbp, sz);
                        fbp += wcslen(fbp);
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
							wcscpy(fbuf, fbuf + 1);
					}
#    endif
                }
            }
            i = wcslen(fbuf);
            if (width < i)
                width = i;
            justifiedOutput(__stream, fbuf, ljustify, width, ' ', written);

#endif
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
        {
            int hold = 0;
            (*count)++;
            xxx = *(wchar_t**)arg;
            if (xxx == NULL)
            {
                xxx = L"(null)";
                lc = 1;
            }
            else if (!lc)
            {
                char* yyy = xxx;
                int n = mbsrtowcs(NULL, &yyy, 10000, (mbstate_t*)__stream->extended->mbstate);
                if (n > 0)
                {
                    yyy = malloc(n + 1);
                    if (!yyy)
                        return format + wcslen(format);
                    mbsrtowcs(yyy, &xxx, 10000, (mbstate_t*)__stream->extended->mbstate);
                    xxx = yyy;
                }
            }
            fxs = wcslen(xxx);
            if (prec > 0 && prec < fxs)
            {
                hold = xxx[fxs];
                xxx[fxs] = 0;
                fxs = prec;
            }
            if (width < fxs)
                width = fxs;
            justifiedOutput(__stream, xxx, ljustify, width, ' ', written);
            if (hold)
                xxx[fxs] = hold;
        }
            if (!lc)
                free(xxx);
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
            if (prec == 0 && wcslen(sz) == 1 && sz[0] == '0')
                sz++;
            else if (wcslen(sz) < prec)
                sz -= prec - wcslen(sz);
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
            temp = wcslen(sz);
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
                if (leadzero)
                {
                    PUTCH(signch, __stream);
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
                    PUTCH('0', __stream);
                    (*written)++;
                }
                else if ((type == 'x') && c)
                {
                    PUTCH('0', __stream);
                    PUTCH('X', __stream);
                    (*written) += 2;
                }
            justifiedOutput(__stream, sz, ljustify, width, leadzero ? '0' : ' ', written);
            break;
        default:
            PUTCH(format[-1], __stream);
            (*written)++;
            break;
    }
    return (format);
}
