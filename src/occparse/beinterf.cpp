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
 * C configuration when we are using ICODE code generator output
 */
#include <cstdio>
#include <cstring>
#include <cctype>
#include "compiler.h"
#include "PreProcessor.h"
#include "Utils.h"
#include "config.h"
#include "occparse.h"
#include "types.h"

namespace Parser
{

int needsAtomicLockFromType(Type* tp)
{
    Optimizer::ARCH_SIZING* p = Optimizer::chosenAssembler->arch->type_needsLock;
    switch (tp->BaseType()->type)
    {
        case BasicType::char8_t_:
            return 0;
        case BasicType::char16_t_:
            return 0;
        case BasicType::char32_t_:
            return 0;
        case BasicType::bit_:
        case BasicType::void_:
            return 0;
        case BasicType::char_:
        case BasicType::signed_char_:
        case BasicType::unsigned_char_:
            return p->a_char;
        case BasicType::bool_:
            return p->a_bool;
        case BasicType::short_:
        case BasicType::unsigned_short_:
            return p->a_short;
        case BasicType::wchar_t_:
            return p->a_wchar_t;
        case BasicType::long_:
        case BasicType::unsigned_long_:
            return p->a_long;
        case BasicType::long_long_:
        case BasicType::unsigned_long_long_:
            return p->a_longlong;
        case BasicType::int_:
        case BasicType::unsigned_:
        case BasicType::inative_:
        case BasicType::unative_:
            return p->a_int;
        case BasicType::enum_:
            return p->a_enum;
        case BasicType::pointer_:
            return p->a_addr;
        case BasicType::any_:
            return p->a_addr;
        case BasicType::seg_:
            return p->a_farseg;
        case BasicType::far_:
            return p->a_farptr;
        case BasicType::memberptr_:
             return p->a_memberptr;
        case BasicType::float_:
        case BasicType::float__imaginary_:
            return p->a_float;
        case BasicType::double_:
        case BasicType::double__imaginary_:
            return p->a_double;
        case BasicType::long_double_:
        case BasicType::long_double_imaginary_:
            return p->a_longdouble;
        case BasicType::float__complex_:
            return 1;
        case BasicType::double__complex_:
            return 1;
        case BasicType::long_double_complex_:
            return 1;
        case BasicType::bitint_:
        case BasicType::unsigned_bitint_:
            return 1;
        case BasicType::class_:
        case BasicType::struct_:
        case BasicType::union_:
        default:
            if ((Optimizer::chosenAssembler->arch->isLockFreeSize >= tp->BaseType()->size &&
                 (tp->BaseType()->size & (tp->BaseType()->size - 1)) == 0))
                return 0;
            return 1;
    }
}
static int basesize(Optimizer::ARCH_SIZING* p, Type* tp)
{
    tp = tp->BaseType();
    switch (tp->type)
    {
        case BasicType::char8_t_:
            return 1;
        case BasicType::char16_t_:
            return 2;
        case BasicType::char32_t_:
            return 4;
        case BasicType::bit_:
        case BasicType::void_:
            return 0;
        case BasicType::char_:
        case BasicType::unsigned_char_:
        case BasicType::signed_char_:
            return p->a_char;
        case BasicType::bool_:
            return p->a_bool;
        case BasicType::short_:
        case BasicType::unsigned_short_:
            return p->a_short;
        case BasicType::wchar_t_:
            return p->a_wchar_t;
        case BasicType::long_:
        case BasicType::unsigned_long_:
            return p->a_long;
        case BasicType::long_long_:
        case BasicType::unsigned_long_long_:
            return p->a_longlong;
        case BasicType::int_:
        case BasicType::unsigned_:
        case BasicType::inative_:
        case BasicType::unative_:
            return p->a_int;
        case BasicType::enum_:
            return p->a_enum;
        case BasicType::pointer_:
            if (tp->array && !tp->vla)
                return basesize(p, tp->btp);
            else
                return p->a_addr;
        case BasicType::any_:
            return p->a_addr;
        case BasicType::seg_:
            return p->a_farseg;
        case BasicType::far_:
            return p->a_farptr;
        case BasicType::memberptr_:
            return p->a_memberptr;
        case BasicType::float_:
        case BasicType::float__imaginary_:
            return p->a_float;
        case BasicType::double_:
        case BasicType::double__imaginary_:
            return p->a_double;
        case BasicType::long_double_:
        case BasicType::long_double_imaginary_:
            return p->a_longdouble;
        case BasicType::float__complex_:
            return (p->a_float + p->a_fcomplexpad) * 2;
        case BasicType::double__complex_:
            return (p->a_double + p->a_rcomplexpad) * 2;
        case BasicType::long_double_complex_:
            return (p->a_longdouble + p->a_lrcomplexpad) * 2;
        case BasicType::string_:
            return p->a_addr;
        case BasicType::object_:
            return p->a_addr;
        case BasicType::class_:
        case BasicType::struct_:
        case BasicType::union_:
            if (p->a_struct)
                return p->a_struct;
            else
                return tp->sp ? (tp->sp->sb->attribs.inheritable.structAlign ? tp->sp->sb->attribs.inheritable.structAlign : 1) : 0;
        default:
            /*            diag("basesize: unknown type");*/
            return 1;
    }
}
int getSize(BasicType type)
{
    Type tp;
    memset(&tp, 0, sizeof(tp));
    tp.type = type; /* other fields don't matter, we never call this for structured types*/
    tp.rootType = &tp;
    return basesize(Optimizer::chosenAssembler->arch->type_sizes, &tp);
}
int getBaseAlign(BasicType type)
{
    Type tp;
    if (type == BasicType::auto_)
        type = BasicType::struct_;
    tp.type = type; /* other fields don't matter, we never call this for structured types*/
    tp.array = tp.vla = false;
    tp.rootType = &tp;
    return basesize(Optimizer::chosenAssembler->arch->type_align, &tp);
}
int getMaxAlign()
{
    return Optimizer::chosenAssembler->arch->type_align->a_maxalign;
}
long getautoval(long val)
{
    if (Optimizer::chosenAssembler->arch->spgrowsup)
        return -val;
    else
        return val;
}
int funcvaluesize(int val)
{
    if (Optimizer::chosenAssembler->arch->param_offs)
        return (Optimizer::chosenAssembler->arch->param_offs(val));
    return 0;
}
int alignment(StorageClass sc, Type* tp)
{
    (void)sc;
    return basesize(Optimizer::chosenAssembler->arch->type_align, tp);
}
int getAlign(StorageClass sc, Type* tp)
{
    if (tp->alignment)
        return tp->alignment;
    while (tp->array)
        tp = tp->BaseType()->btp;
    int align = basesize(Optimizer::chosenAssembler->arch->type_align, tp);
    if (sc != StorageClass::auto_)
    {
        int pack = preProcessor->GetPack();
        if (pack < align)
            align = pack;
    }
    if (Optimizer::chosenAssembler->arch->align)
        align = Optimizer::chosenAssembler->arch->align(align);
    if (tp->IsStructured() && tp->BaseType()->sp->sb->attribs.inheritable.structAlign > align)
        align = tp->BaseType()->sp->sb->attribs.inheritable.structAlign;
    return align;
}
const char* getUsageText(void) { return Optimizer::chosenAssembler->usage_text; }
const char* getHelpText(void) { return Optimizer::chosenAssembler->help_text; }
KEYWORD* GetProcKeywords(void) { return (KEYWORD*)Optimizer::chosenAssembler->keywords; }
}  // namespace Parser