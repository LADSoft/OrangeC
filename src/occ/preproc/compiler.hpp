/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
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
    #define ULLONG_TYPE unsigned long long
    #define LLONG_TYPE long long
#else 
    #define ULLONG_TYPE unsigned long 
    #define LLONG_TYPE long
    #define ULLONG_MAX ULONG_MAX
#endif 

typedef unsigned long ADDRESS;
typedef unsigned short LCHAR;
typedef unsigned char BOOL;
typedef unsigned char BYTE;

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <float.h>
#include "ccerr.h"
#include "osutil.h"
#include "pp.h"

typedef struct {
    int  prm_maxerr;        /* number of errors at which the compiler stops*/
    BOOL prm_optimize;
    BOOL prm_quiet;         /* no warnings/errors on console */
    BOOL prm_warning;       /* display warnings */
    BOOL prm_extwarning;    /* display extended warnings */
    BOOL prm_diag;          /* display diagnostics/ memory usage */
    BOOL prm_ansi;          /* use ansi restrictions */
    BOOL prm_cmangle;       /* add underscore to names */
    BOOL prm_c99;           /* C99 mode */
    BOOL prm_cplusplus;     /* C++ mode */
    BOOL prm_xcept;         /* generate RTTI in C++ mode */
    BOOL prm_icdfile;       /* dump intermediate code peep list to file */
    BOOL prm_asmfile;       /* assembly language output */
    BOOL prm_compileonly;   /* don't run postprocess routines */
    BOOL prm_debug;         /* generate debug info */
    BOOL prm_listfile;      /* generate list file */
    BOOL prm_cppfile;       /* generate preprocessor file */
    BOOL prm_errfile;       /* generate error file */
    BOOL prm_browse;        /* generate browse information */
    BOOL prm_trigraph;      /* parse trigraphs */
    BOOL prm_oldfor;        /* old C++ for scoping */
    BOOL prm_stackcheck;    /* generate stack check code */
    BOOL prm_profiler;      /* generate profiler insets */
    BOOL prm_mergestrings;  /* merge strings and other constants */
    BOOL prm_revbits;       /* use reverse of the default bit ordering */
    BOOL prm_lines;         /* put line info into assembly files */
    BOOL prm_bss;           /* Use a BSS */
    BOOL prm_intrinsic;     /* look for intrinsics */
    BOOL prm_smartframes;   /* omit frame pointer initialization as possible */
    BOOL prm_farkeyword;    /* honor the 'far' and 'near' keywords */
    BOOL prm_linkreg;       /* use a frame pointer */
    BOOL prm_charisunsigned; /* char type is unsigned */
    BOOL prm_assemble;		/* do assembler mode */
} COMPILER_PARAMS;

typedef struct sym
{
    char *name;
} SYMBOL;
#include "pp.p"

#define imax(x, y) ((x) > (y) ? (x) : (y))


