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
/* Divide z / w ... */
long double complex _stdcall __CPLXDIV(long double complex z, long double complex w)
{
    #pragma STDC FP_CONTRACT OFF
    long double a, b, c, d, logbw, denom, x, y;
    int ilogbw = 0;
    a = creall(z); b = cimagl(z);
    c = creall(w); d = cimagl(w);
    logbw = logbl(fmaxl(fabsl(c), fabsl(d)));
    if (isfinite(logbw)) {
        ilogbw = (int)logbw;
        c = scalbnl(c, -ilogbw);
        d = scalbnl(d, -ilogbw);
    }
    denom = c * c + d * d;
    x = scalbnl((a * c + b * d) / denom, -ilogbw);
    y = scalbnl((b * c - a * d) / denom, -ilogbw);
    /*
     * Recover infinities and zeros that computed
     * as NaN+iNaN; the only cases are non-zero/zero,
     * infinite/finite, and finite/infinite, ...
     */
    if (isnan(x) && isnan(y)) {
        if ((denom == 0.0) && (!isnan(a) || !isnan(b))) {
            x = copysignl(INFINITY, c) * a;
            y = copysignl(INFINITY, c) * b;
        }
        else if ((isinf(a) || isinf(b)) && isfinite(c) && isfinite(d)) {
            a = copysignl(isinf(a) ? 1.0 : 0.0, a);
            b = copysignl(isinf(b) ? 1.0 : 0.0, b);
            x = INFINITY * ( a * c + b * d );
            y = INFINITY * ( b * c - a * d );
        }
        else if (isinf(logbw) && isfinite(a) && isfinite(b)) {
            c = copysignl(isinf(c) ? 1.0 : 0.0, c);
            d = copysignl(isinf(d) ? 1.0 : 0.0, d);
            x = 0.0 * ( a * c + b * d );
            y = 0.0 * ( b * c - a * d );
        }
    }
    return x + I * y;
}
