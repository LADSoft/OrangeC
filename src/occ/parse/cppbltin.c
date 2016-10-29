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
/* declare in select has multiple vars */
#include "compiler.h"
#include "rtti.h"
extern INCLUDES *includes;
extern TYPE stdint;
extern ARCH_ASM *chosenAssembler;

static unsigned char *cppbuiltin = (unsigned char *)"void * operator new(unsigned size); "
    "void * operator new[](unsigned size); " 
    "void * operator new(unsigned size, void *ptr) noexcept; " 
    "void * operator new[](unsigned size, void *ptr) noexcept; " 
    "void   operator delete  (void *) noexcept; " 
	"void   operator delete[](void *) noexcept; "
    "void * __dynamic_cast(void *, void *, void *, void *); " 
    "void * __lambdaCall(void *); "
    "void * __lambdaCallS(void *); "
    "void * __lambdaPtrCall(void *, void *); "
    "void * __lambdaPtrCallS(void *, void *); "
    "void * __GetTypeInfo(void *, void *); " 
    "void _ThrowException(void *,void *,int,void*,void *); "
	"void _RethrowException(void *); "
    "void _CatchCleanup(void *); " 
    "void _InitializeException(void *, void *); "
    "void _RundownException(); "
    "void __arrCall(void *, void *, void *, int, int); "
    "int __is_constructible(...); "
    "int __is_convertible_to(...); "
    "namespace std { "
    "class type_info; "
    "} "
    "typedef std::type_info typeinfo; "
    "extern \"C\" {"
    "int __builtin_ctz(unsigned x);"
    "int __builtin_ctzl(unsigned long x);"
    "int __builtin_ctzll(unsigned long long x);"
    "int __builtin_clz(unsigned x);"
    "int __builtin_clzl(unsigned long x);"
    "int __builtin_clzll(unsigned long long x);"
    "int __builtin_popcount(unsigned x);"
    "int __builtin_popcountl(unsigned long x);"
    "int __builtin_popcountll(unsigned long long x);"
    "float __builtin_huge_valf();"
    "double __builtin_huge_val();"
    "long double __builtin_huge_vall();"
    "float __builtin_nanf(const char *x);"
    "double __builtin_nan(const char *x);"
    "long double __builtin_nanl(const char *x);"
    "float __builtin_nansf(const char *x);"
    "double __builtin_nans(const char *x);"
    "long double __builtin_nansl(const char *x);"
    "}"
    ;
TYPE stdXC = { 
    bt_struct, sizeof(XCTAB)
};
void ParseBuiltins(void)
{
    LEXEME *lex;
    FILE *handle = includes->handle;
    unsigned char *p = includes->lptr;
    includes->handle = NULL;
    if (cparams.prm_cplusplus)
    {
        includes->lptr = cppbuiltin;
        lex = getsym();
        if (lex)
        {
            while ((lex = declare(lex, NULL, NULL, sc_global, lk_none, NULL, TRUE, FALSE, FALSE, FALSE, ac_public)) != NULL) ;
        }
    }
    if (chosenAssembler->bltins)
    {
        includes->lptr = chosenAssembler->bltins;
        lex = getsym();
        if (lex)
        {
            while ((lex = declare(lex, NULL, NULL, sc_global, lk_none, NULL, TRUE, FALSE, FALSE, FALSE, ac_public)) != NULL) ;
        }
    }
    includes->handle = handle;
    includes->lptr = p;
    includes->line = 0;
    if (cparams.prm_cplusplus)
    {
        stdXC.syms = CreateHashTable(1);
        stdXC.sp = makeID(sc_type, &stdXC, NULL, "$$XCTYPE");
        stdXC.alignment = getAlign(sc_auto, &stdint);
    }
}