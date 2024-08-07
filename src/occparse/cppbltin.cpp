/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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
 * 
 */

/* declare in select has multiple vars */
#include "compiler.h"
#include "rtti.h"
#include "config.h"
#include "initbackend.h"
#include "declare.h"
#include "lex.h"
#include "beinterf.h"
#include "libcxx.h"
#include "ccerr.h"
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "symtab.h"
#include "stmt.h"
#include "types.h"

namespace Parser
{

Type stdXC = {BasicType::struct_, XCTAB_SIZE, 0, &stdXC};
void ParseBuiltins(void)
{
    static std::string cppbuiltin =
        "void * operator new(unsigned); "
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
        "[[noreturn]]void __rtllinkage _ThrowException(void *,void *,int,void*,void *); "
        "[[noreturn]]void __rtllinkage _RethrowException(void *); "
        "void __rtllinkage _CatchCleanup(void *); "
        "void __rtllinkage __arrCall(void *, void *, void *, int, int); "
        "extern \"C\" bool __rtllinkage __static_guard(void *); "
        "extern \"C\" void __rtllinkage atexit(void (*__func)(void)); "
        "namespace std { "
        "class type_info; "
        "} "
        "typedef std::type_info typeinfo; "
        "class __make_integer_seq_cls; "
        "template <template <class T, T...> class tpl, class _Tp, _Tp _Ep> using __make_integer_seq = "
        "__make_integer_seq_cls::type; "
        "class __type_pack_element_cls; "
        "template <size_t _Idx, class ..._Types> using __type_pack_element = __type_pack_element_cls::type; "
        "extern \"C\" {"
        "[[noreturn]] void __rtllinkage __builtin_unreachable(); "
        "constexpr int __rtllinkage __builtin_ctz(unsigned x);"
        "constexpr int __rtllinkage __builtin_ctzl(unsigned long x);"
        "constexpr int __rtllinkage __builtin_ctzll(unsigned long long x);"
        "constexpr int __rtllinkage __builtin_clz(unsigned x);"
        "constexpr int __rtllinkage __builtin_clzl(unsigned long x);"
        "constexpr int __rtllinkage __builtin_clzll(unsigned long long x);"
        "constexpr int __rtllinkage __builtin_popcount(unsigned x);"
        "constexpr int __rtllinkage __builtin_popcountl(unsigned long x);"
        "constexpr int __rtllinkage __builtin_popcountll(unsigned long long x);"
        "constexpr float __rtllinkage __builtin_huge_valf();"
        "constexpr double __rtllinkage __builtin_huge_val();"
        "constexpr long double __rtllinkage __builtin_huge_vall();"
        "constexpr float __rtllinkage __builtin_nanf(const char *x);"
        "constexpr double __rtllinkage __builtin_nan(const char *x);"
        "constexpr long double __rtllinkage __builtin_nanl(const char *x);"
        "constexpr float __rtllinkage __builtin_nansf(const char *x);"
        "constexpr double __rtllinkage __builtin_nans(const char *x);"
        "constexpr long double __rtllinkage __builtin_nansl(const char *x);"
        // this next is for libcpp version c++17...   we are simplifying and assuming that on all architectures,
        // an int and everything below it is always lock free.    Since this does the comparison based on type size
        // it will also catch longs on those architectures where sizeof(long) == sizeof(int)
        "constexpr bool __atomic_always_lock_free(int x, int y) { return x <= " +
        std::to_string(getSize(BasicType::unsigned_)) +
        "; };"
        "}";
    static std::string cbuiltin =
        "bool __stdcall __rtllinkage ___ckdadd(void*, int, void*, int, void*, int);"
        "bool __stdcall __rtllinkage ___ckdmul(void*, int, void*, int, void*, int);"
        "bool __stdcall __rtllinkage ___ckdsub(void*, int, void*, int, void*, int);";
    LexList* lex;

    if (Optimizer::cparams.prm_cplusplus)
    {
        libcxx_builtins();
        SetAlternateParse(true, cppbuiltin);
        lex = getsym();
        if (lex)
        {
            while ((lex = declare(lex, nullptr, nullptr, StorageClass::global_, Linkage::none_, emptyBlockdata, true, false, false, AccessLevel::public_)) != nullptr)
                ;
        }
        SetAlternateParse(false, "");
    }
    else if (Optimizer::cparams.c_dialect >= Dialect::c2x)
    {
        SetAlternateParse(true, cbuiltin);
        lex = getsym();
        if (lex)
        {
            while ((lex = declare(lex, nullptr, nullptr, StorageClass::global_, Linkage::none_, emptyBlockdata, true, false, false, AccessLevel::public_)) != nullptr)
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
            while ((lex = declare(lex, nullptr, nullptr, StorageClass::global_, Linkage::none_, emptyBlockdata, true, false, false, AccessLevel::public_)) != nullptr)
                ;
        }
        SetAlternateParse(false, "");
    }
    if (Optimizer::cparams.prm_cplusplus)
    {
        stdXC.syms = symbols.CreateSymbolTable();
        stdXC.sp = makeID(StorageClass::type_, &stdXC, nullptr, "$$XCTYPE");
        stdXC.sp->sb->decoratedName = stdXC.sp->name;
    }
}
}  // namespace Parser