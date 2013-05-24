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
/*
    Copied from the C99 standard
*/
#include <math.h>
#include <complex.h>

double complex _stdcall __CPLXMUL(long double complex z, long double complex w)
{
    #pragma STDC FP_CONTRACT OFF
    long double a, b, c, d, ac, bd, ad, bc, x, y;
    a = creall(z); b = cimagl(z);
    c = creall(w); d = cimagl(w);
    ac = a * c; bd = b * d;
    ad = a * d; bc = b * c;
    x = ac - bd;
    y = ad + bc;
    /* Risinfecover infinities that computed as NaN+iNaN ... */
    if (isnan(x) && isnan(y)) {
        int recalc = 0;
        if ( isinf(a) || isinf(b) ) { /* z is infinite */
            /* "Box" the infinity ... */
            a = copysignl(isinf(a) ? 1.0 : 0.0, a);
            b = copysignl(isinf(b) ? 1.0 : 0.0, b);
            /* Change NaNs in the other factor to 0 ... */
            if (isnan(c)) c = copysignl(0.0, c);
            if (isnan(d)) d = copysignl(0.0, d);
            recalc = 1;
        }
        if ( isinf(c) || isinf(d) ) { /* w is infinite */
            /* "Box" the infinity ... */
            c = copysignl(isinf(c) ? 1.0 : 0.0, c);
            d = copysignl(isinf(d) ? 1.0 : 0.0, d);
            /* Change NaNs in the other factor to 0 ... */
            if (isnan(a)) a = copysignl(0.0, a);
            if (isnan(b)) b = copysignl(0.0, b);
            recalc = 1;
        }
        if (!recalc) {
            /* *Recover infinities from overflow cases ... */
            if (isinf(ac) || isinf(bd) || isinf(ad) || isinf(bc)) {
                /* Change all NaNs to 0 ... */
                if (isnan(a)) a = copysignl(0.0, a);
                if (isnan(b)) b = copysignl(0.0, b);
                if (isnan(c)) c = copysignl(0.0, c);
                if (isnan(d)) d = copysignl(0.0, d);
                recalc = 1;
            }
        }
        if (recalc) {
            x = INFINITY * ( a * c - b * d );
            y = INFINITY * ( a * d + b * c );
        }
    }
    return x + I * y;
}