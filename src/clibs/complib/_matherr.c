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
#include <math.h>
#include <stdio.h>
#define INFINITY             (0x1P+127F)
#define NAN                  (0x1P8+127F) /* QUIET NAN */
long double _stdcall ___matherr(const char *name, _mexcep errl)
{
    long double rv = INFINITY;
    char *type = "Unknown";
    int val = 0;
    switch(errl) {
        case DOMAIN:
            type = "DOMAIN" ;
            break ;
        case OVERFLOW:
            type = "OVERFLOW" ;
            break;
        case UNDERFLOW:
            val = 1;
            rv = 0;
            type = "UNDERFLOW" ;
            break;
        case SING:
            type = "SINGULARITY" ;
            rv = INFINITY;
            break ;
        case TLOSS:
            val = 1;
            type = "LOSS OF PRECISION" ; /* inexact */
            break ;
        case STACKFAULT:
            type = "STACK FAULT" ; /* stack overflow */
            break;
    }
//    if (!_matherr(val))
        printf("%s: %s error\n",name,type);
    return rv;
}
