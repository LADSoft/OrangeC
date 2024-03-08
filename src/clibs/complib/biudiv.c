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
/*
 * Code in this module was derived from the 'bigdigits' source code by David Ireland
 * Original licensing is as follows:
 */
/***** BEGIN LICENSE BLOCK *****
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2001-16 David Ireland, D.I. Management Services Pty Limited
 * <http://www.di-mgt.com.au/bigdigits.html>. All rights reserved.
 *
 ***** END LICENSE BLOCK *****/
#include "bi.h"

int __stdcall _RTL_FUNC ___biadd(UNDERLYING_TYPE* ans, UNDERLYING_TYPE* left, UNDERLYING_TYPE* right, int bits);
int __stdcall _RTL_FUNC ___bicompareu(UNDERLYING_TYPE* left, UNDERLYING_TYPE* right, int bits);
int __stdcall _RTL_FUNC ___bilsh(UNDERLYING_TYPE* ans, UNDERLYING_TYPE* left, int bits, int shift);
void __stdcall _RTL_FUNC ___biursh(UNDERLYING_TYPE* ans, UNDERLYING_TYPE* left, int bits, int shift);


int __mdiv(unsigned u[2], unsigned v, unsigned *q, unsigned *r);
void __mmul(unsigned a, unsigned b, unsigned* high, unsigned* low);

static unsigned multSub(unsigned wn, unsigned w[], const unsigned v[], unsigned q, size_t n)
{ /*	Compute w = w - qv
      where w = (WnW[n-1]...W[0])
      return modified Wn.
  */
    unsigned k, high, low;

    if (q == 0)
        return wn;

    k = 0;

    for (int i = 0; i < n; i++)
    {
        __mmul(q, v[i], &high, &low);
        w[i] -= k;
        if (w[i] > 0xffffffff - k)
            k = 1;
        else
            k = 0;
        w[i] -= low;
        if (w[i] > 0xffffffff - low)
            k++;
        k += high;
    }

    // handle high-order digit
    wn -= k;

    return wn;
}

static int QhatTooBig(unsigned qhat, unsigned rhat, unsigned vn2, unsigned ujn2)
{ /*	Returns true if Qhat is too big */
    unsigned high, low;

    __mmul(qhat, vn2, &high, &low);
    if (high < rhat)
        return 0;
    else if (high > rhat)
        return 1;
    else if (low > ujn2)
        return 1;

    return 0;
}

static UNDERLYING_TYPE  shortDiv(UNDERLYING_TYPE  q[], const UNDERLYING_TYPE  u[], UNDERLYING_TYPE  v, size_t ndigits)
{
    /*	Calculates quotient q = u div v
        Returns remainder r = u mod v
        where q, u are multiprecision integers of ndigits each
        and r, v are single precision digits.
        Ref: Knuth Vol 2 Ch 4.3.1 Exercise 16 p625
    */
    UNDERLYING_TYPE t[2], r;
    int shift;
    UNDERLYING_TYPE  bitmask, overflow, *uu;

    if (ndigits == 0)
        return 0;
    if (v == 0)
        return 0; /* Divide by zero error */

    /*	Normalise first */
    /*	Requires high bit of V
        to be set, so find most signif. bit then shift left,
        i.e. d = 2^shift, u' = u * d, v' = v * d.
    */
    for (shift = 0, bitmask = 0x80000000 ; shift < UTBITS && !(v & bitmask); shift++, bitmask >>= 1);

    v <<= shift;
    overflow = ___bilsh(q, u, ndigits * UTBITS, shift);
    uu = q;

    /* Step S1 - modified for extra digit. */
    r = overflow; /* New digit Un */
    for (int j = ndigits-1; j >=0; j--)
    {
        /* Step S2. */
        t[1] = r;
        t[0] = uu[j];
        overflow = __mdiv(t, v, &q[j], &r);
    }

    /* Unnormalise */
    r >>= shift;

    return r;
}

static int bidiv(UNDERLYING_TYPE* q, UNDERLYING_TYPE* r, UNDERLYING_TYPE *u, UNDERLYING_TYPE* v, int words)
{
/*	
    Computes quotient q = u / v and remainder r = u mod v
    Ref: Knuth Vol 2 Ch 4.3.1 p 272 Algorithm D.
*/
    int shift;
    int n = 0, m = 0;
    UNDERLYING_TYPE  bitmask, overflow;
    UNDERLYING_TYPE  qhat, rhat, t[2];
    UNDERLYING_TYPE  *uu, *ww;
    int qhatOK, cmp;

    /* Clear q and r */
    memset(q, 0, words * sizeof(UNDERLYING_TYPE));
    memset(r, 0, words * sizeof(UNDERLYING_TYPE));

    /* Work out exact sizes of u and v */
    for (int i= words; i > 0 && !n; i--)
       if (v[i-1]) n = i;
    for (int i= words; i > 0 && !m; i--)
       if (u[i-1]) m = i;
    m -= n;

    /* Catch special cases */
    if (n == 0)
        return -1; /* Error: divide by zero */

    if (n == 1)
    { /* Use short division instead */
        r[0] = shortDiv(q, u, v[0], words);
        return 0;
    }

    if (m < 0)
    { /* v > u, so just set q = 0 and r = u */
        memcpy(r, u, words * sizeof(UNDERLYING_TYPE));
        return 0;
    }

    if (m == 0)
    { /* u and v are the same length */
        cmp = ___bicompareu(u, v, n * UTBITS);
        if (cmp < 0)
        { /* v > u, as above */
        memcpy(r, u, words * sizeof(UNDERLYING_TYPE));
            return 0;
        }
        else if (cmp == 0)
        { /* v == u, so set q = 1 and r = 0 */
            q[0] = 1;
            return 0;
        }
    }

    /*	In Knuth notation, we have:
        Given
        u = (Um+n-1 ... U1U0)
        v = (Vn-1 ... V1V0)
        Compute
        q = u/v = (QmQm-1 ... Q0)
        r = u mod v = (Rn-1 ... R1R0)
    */

    /*	Step D1. Normalise */
    /*	Requires high bit of Vn-1
        to be set, so find most signif. bit then shift left,
        i.e. d = 2^shift, u' = u * d, v' = v * d.
    */
    for (shift = 0, bitmask = 0x80000000 ; shift < UTBITS && !(v[n-1] & bitmask); shift++, bitmask >>= 1);

    /* Normalise v in situ - NB only shift non-zero digits */
    ___bilsh(v, v, n * UTBITS, shift);

    /* Copy normalised dividend u*d into r */
    overflow = ___bilsh(r, u, (m + n) * UTBITS, shift);
    uu = r; /* Use ptr to keep notation constant */
    t[0] = overflow; /* Extra digit Um+n */

    /* Step D2. Initialise j. Set j = m */
    for (int j = m; j >= 0; j--)
    {
        /* Step D3. Set Qhat = [(b.Uj+n + Uj+n-1)/Vn-1]
           and Rhat = remainder */
        qhatOK = 0;
        t[1] = t[0]; /* This is Uj+n */
        t[0] = uu[j + n - 1];
        overflow = __mdiv(t, v[n-1], &qhat, &rhat);
        /* Test Qhat */
        if (overflow)
        { /* Qhat == b so set Qhat = b - 1 */
            qhat = 0xffffffff;
            rhat = uu[j + n - 1];
            rhat += v[n - 1];
            if (rhat < v[n - 1]) /* Rhat >= b, so no re-test */
                qhatOK = 1;
        }
        /* [VERSION 2: Added extra test "qhat && "] */
        if (qhat && !qhatOK && QhatTooBig(qhat, rhat, v[n - 2], uu[j + n - 2]))
        { /* If Qhat.Vn-2 > b.Rhat + Uj+n-2
             decrease Qhat by one, increase Rhat by Vn-1
          */
            qhat--;
            rhat += v[n - 1];
            /* Repeat this test if Rhat < b */
            if (!(rhat < v[n - 1]))
                if (QhatTooBig(qhat, rhat, v[n - 2], uu[j + n - 2]))
                    qhat--;
        }
        /* Step D4. Multiply and subtract */
        ww = &uu[j];
        overflow = multSub(t[1], ww, v, qhat, (size_t)n);

        /* Step D5. Test remainder. Set Qj = Qhat */
        q[j] = qhat;
        if (overflow)
        { /* Step D6. Add back if D4 was negative */
            q[j]--;
            overflow = ___biadd(ww, ww, v, n * UTBITS);
        }

        t[0] = uu[j + n - 1]; /* Uj+n on next round */
    } /* Step D7. Loop on j */

    /* Clear high digits in uu */
    memset(uu +n, 0, m *sizeof(UNDERLYING_TYPE));

    /* Step D8. Unnormalise. */

    ___biursh(r, r, n *UTBITS, shift);
    ___biursh(v, v, n *UTBITS, shift);

    return 0;

}
void __stdcall _RTL_FUNC ___biudiv(UNDERLYING_TYPE* ans, UNDERLYING_TYPE* left, UNDERLYING_TYPE* right, int bits)
{
    const int words = WORDS(bits);
    MASK(left, bits);
    MASK(right, bits);
    UNDERLYING_TYPE* r = alloca(words * sizeof(UNDERLYING_TYPE));
    bidiv(ans, r, left, right, words);
}
void __stdcall _RTL_FUNC ___biumod(UNDERLYING_TYPE* ans, UNDERLYING_TYPE* left, UNDERLYING_TYPE* right, int bits)
{
    const int words = WORDS(bits);
    MASK(left, bits);
    MASK(right, bits);
    UNDERLYING_TYPE* q = alloca(words * sizeof(UNDERLYING_TYPE));
    bidiv(q, ans, left, right, words);
}
