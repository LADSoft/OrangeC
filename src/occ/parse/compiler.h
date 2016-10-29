/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
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
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
/* the long long type is 8 bytes...  if neither long long nor the long type is 8 byte in the compiler
 * this is compiled with, things will still work, however long long constants will be limited in size
 * and then automatically sign-extended out to long-long at runtime if necessary when used.  Also
 * some floating point conversions will have a more limited integer portion.
 */
#ifdef USE_LONGLONG
#ifdef BORLAND
    #define ULLONG_TYPE unsigned __int64
    #define LLONG_TYPE __int64
    #define LLONG_MIN _I64_MIN
    #define LLONG_MAX _I64_MAX
    #define ULLONG_MAX ((_I64_MAX << 1) + 1)
#else
    #define ULLONG_TYPE unsigned long long
    #define LLONG_TYPE long long
#endif
    #define LLONG_FORMAT_SPECIFIER "%lld"
#else 
    #define ULLONG_TYPE unsigned long 
    #define LLONG_TYPE long
    #define ULLONG_MAX ULONG_MAX
    #define LLONG_FORMAT_SPECIFIER "%ld"
#endif 

typedef unsigned BITINT;


typedef unsigned long ADDRESS;
typedef unsigned LCHAR;
typedef unsigned char BOOLEAN;
typedef unsigned char UBYTE;

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <float.h>
#include "ccerr.h"
#include "floating.h"
#include "osutil.h"
#include "pp.h"
#include "iexpr.h"
#include "c.h" 
#include "iopt.h"
#include "beinterf.h"

#include "compiler.p"

#define M_LN2       0.693147180559945309417
#define M_LN10      2.30258509299404568402
