/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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

/* declare in select has multiple vars */
#include "compiler.h"
#include "rtti.h"
#include "config.h"
#include "initbackend.h"
#include "declare.h"
#include "lex.h"
#include "symtab.h"
#include "beinterf.h"

namespace Parser
{
static std::string cppbuiltin =
    "void * operator new(unsigned size); "
    "void __rtllinkage * operator new[](unsigned size); "
    "void __rtllinkage * operator new(unsigned size, void *ptr) noexcept; "
    "void __rtllinkage * operator new[](unsigned size, void *ptr) noexcept; "
    "void __rtllinkage   operator delete  (void *) noexcept; "
    "void __rtllinkage   operator delete[](void *) noexcept; "
    "void __rtllinkage * __dynamic_cast(void *, void *, void *, void *); "
    "void __rtllinkage * __lambdaCall(void *); "
    "void __rtllinkage * __lambdaCallS(void *); "
    "void __rtllinkage * __lambdaPtrCall(void *, void *); "
    "void __rtllinkage * __lambdaPtrCallS(void *, void *); "
    "void __rtllinkage * __GetTypeInfo(void *, void *); "
    "void __rtllinkage _ThrowException(void *,void *,int,void*,void *); "
    "void __rtllinkage _RethrowException(void *); "
    "void __rtllinkage _CatchCleanup(void *); "
    "void __rtllinkage _InitializeException(void *, void *); "
    "void __rtllinkage _RundownException(); "
    "void __rtllinkage __arrCall(void *, void *, void *, int, int); "
    "int __rtllinkage __is_constructible(...); "
    "int __rtllinkage __is_convertible_to(...); "
    "namespace std { "
    "class type_info; "
    "} "
    "typedef std::type_info typeinfo; "
    "extern \"C\" {"
    "int __rtllinkage __builtin_ctzll(unsigned long long x);"
    "int __rtllinkage __builtin_clzll(unsigned long long x);"
    "int __rtllinkage __builtin_popcount(unsigned x);"
    "int __rtllinkage __builtin_popcountl(unsigned long x);"
    "int __rtllinkage __builtin_popcountll(unsigned long long x);"
    "float __rtllinkage __builtin_huge_valf();"
    "double __rtllinkage __builtin_huge_val();"
    "long double __rtllinkage __builtin_huge_vall();"
    "float __rtllinkage __builtin_nanf(const char *x);"
    "double __rtllinkage __builtin_nan(const char *x);"
    "long double __rtllinkage __builtin_nanl(const char *x);"
    "float __rtllinkage __builtin_nansf(const char *x);"
    "double __rtllinkage __builtin_nans(const char *x);"
    "long double __rtllinkage __builtin_nansl(const char *x);"
    "}";
TYPE stdXC = {bt_struct, XCTAB_SIZE, 0, &stdXC};
void ParseBuiltins(void)
{
    LEXEME* lex;
    if (Optimizer::cparams.prm_cplusplus)
    {
        SetAlternateParse(true, cppbuiltin);
        lex = getsym();
        if (lex)
        {
            while ((lex = declare(lex, nullptr, nullptr, sc_global, lk_none, nullptr, true, false, false, ac_public)) != nullptr)
                ;
        }
        SetAlternateParse(false, "");
    }
    if (Optimizer::chosenAssembler->bltins)
    {
        std::string temp = Optimizer::chosenAssembler->bltins;
        SetAlternateParse(true, temp);
        lex = getsym();
        if (lex)
        {
            while ((lex = declare(lex, nullptr, nullptr, sc_global, lk_none, nullptr, true, false, false, ac_public)) != nullptr)
                ;
        }
        SetAlternateParse(false, "");
    }
    if (Optimizer::cparams.prm_cplusplus)
    {
        stdXC.syms = CreateHashTable(1);
        stdXC.sp = makeID(sc_type, &stdXC, nullptr, "$$XCTYPE");
        stdXC.sp->sb->decoratedName = stdXC.sp->name;
        stdXC.alignment = getAlign(sc_auto, &stdint);
    }
}
}  // namespace Parser