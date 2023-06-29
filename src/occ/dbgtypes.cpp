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

#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include "be.h"

#include "dbgtypes.h"
#include "ObjFactory.h"
#include "ObjType.h"
#include "ObjFile.h"
#include "OptUtils.h"
#include "memory.h"
#define DEBUG_VERSION 4.0
namespace occx86
{

bool dbgtypes::typecompare::operator()(const Optimizer::SimpleType* left, const Optimizer::SimpleType* right) const
{
    if (left->istypedef)
        if (!right->istypedef)
            return true;
    if (right->istypedef)
        if (!left->istypedef)
            return false;
    left = left;
    right = right;
    if (left->type < right->type)
        return true;
    else if (left->type == right->type)
    {
        switch (left->type)
        {
            case Optimizer::st_func:
                if (operator()(left->btp, right->btp))
                    return true;
                else if (!operator()(right->btp, left->btp))
                {
                    if (!!left->sp->syms != !!right->sp->syms)
                    {
                        if (!left->sp->syms)
                            return true;
                    }
                    else if (left->sp->syms && right->sp->syms && left->sp->syms && right->sp->syms)
                    {
                        const Optimizer::LIST* hr1 = left->sp->syms;
                        const Optimizer::LIST* hr2 = right->sp->syms;
                        while (hr1 && hr2)
                        {
                            Optimizer::SimpleSymbol* sym1 = (Optimizer::SimpleSymbol*)hr1->data;
                            Optimizer::SimpleSymbol* sym2 = (Optimizer::SimpleSymbol*)hr2->data;
                            if (operator()(sym1->tp, sym2->tp))
                                return true;
                            else if (operator()(sym2->tp, (sym1->tp)))
                                return false;
                            hr1 = hr1->next;
                            hr2 = hr2->next;
                        }
                        if (hr2)
                            return true;
                    }
                }
                break;
            case Optimizer::st_struct:
            case Optimizer::st_union:
            case Optimizer::st_enum:
                if (strcmp(left->sp->name, right->sp->name) < 0)
                    return true;
                break;
            case Optimizer::st_pointer:
                if (left->isarray && !right->isarray)
                    return true;
                else if (left->isarray != right->isarray)
                    return false;
                // fallthrough
            case Optimizer::st_lref:
            case Optimizer::st_rref:
                if (operator()(left->btp, right->btp))
                    return true;
                break;
            default:
                if (left->sizeFromType < right->sizeFromType)
                    return true;
                break;
        }
    }
    return false;
}

ObjType* dbgtypes::Put(Optimizer::SimpleType* tp, bool istypedef)
{
    auto val = Lookup(tp);
    if (val)
        return val;
    if (tp->type == Optimizer::st_any || tp->type == Optimizer::st_aggregate)
    {
        val = factory.MakeType((ObjType::eType)42);
    }
    else if (tp->istypedef && !istypedef)
    {
        val = Put(tp, true);

        val = factory.MakeType(ObjType::eTypeDef, val);
    }
    else
    {
        if (!tp->bits)
            val = BasicType(tp);
        if (val == nullptr)
        {
            val = ExtendedType(tp);
        }
    }
    fi->Add(val);

    hash[tp] = val;
    return val;
}

void dbgtypes::OutputTypedef(struct Optimizer::SimpleSymbol* sym)
{
    auto val = Put(sym->tp, true);
    const char* nm = sym->outputName;
    if (nm[0] == '_')
        nm++;
    fi->Add(factory.MakeType(nm, ObjType::eNone, val, val->GetIndex()));
}
ObjType* dbgtypes::Lookup(Optimizer::SimpleType* tp)
{

    auto it = hash.find(tp);
    if (it != hash.end())
        return it->second;
    return nullptr;
}
ObjType* dbgtypes::BasicType(Optimizer::SimpleType* tp)
{
    static int basicTypesUnsigned[] = {0, 35, 34, 48, 49, 49, 49, 50, 50, 51, 50, 52, 50, 0,
                                       0, 0,  0,  0,  72, 73, 74, 80, 81, 82, 88, 89, 90};
    static int basicTypesSigned[] = {0, 35, 34, 40, 41, 41, 41, 42, 42, 43, 42, 44, 42, 0,
                                     0, 0,  0,  0,  72, 73, 74, 80, 81, 82, 88, 89, 90};
    static int pointedAddition[] = {0, 0, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 0,
                                    0, 0, 0,  0,  16, 16, 16, 16, 16, 16, 16, 16, 16};
    int n = 0;
    Optimizer::SimpleType* tp1 = tp;
    if (tp1->type <= Optimizer::st_void)
    {
        int add = 0;
        if (tp1->type == Optimizer::st_void)
            n = 32;
        else
        {
            if (tp1->type == Optimizer::st_pointer && !tp1->isarray && !tp1->isvla && !tp1->bits)
            {
                tp1 = tp1->btp;
                if (tp1->type < Optimizer::st_void && tp1->type != Optimizer::st_pointer)
                    add = pointedAddition[tp1->sizeFromType >= 0 ? tp1->sizeFromType : -tp1->sizeFromType];
            }
            if (tp1->type == Optimizer::st_void)  // pointer to void
                n = 33;
            else if (tp1->type < Optimizer::st_pointer)
            {
                if (tp1->sizeFromType < 0)
                    n = basicTypesSigned[-tp1->sizeFromType] + add;
                else
                    n = basicTypesUnsigned[tp1->sizeFromType] + add;
            }
        }
    }
    if (n)
        return factory.MakeType((ObjType::eType)n);
    return nullptr;
}
ObjType* dbgtypes::TypeName(ObjType* val, const char* nm)
{
    if (nm[0] == '_')
        nm++;
    fi->Add(val);
    return factory.MakeType(nm, ObjType::eNone, val, val->GetIndex());
}
bool istype(Optimizer::SimpleSymbol* sym)
{
    return sym->storage_class == Optimizer::scc_type || sym->storage_class == Optimizer::scc_typedef;
}
void dbgtypes::StructFields(ObjType::eType sel, ObjType* val, int sz, Optimizer::SimpleSymbol* parent, Optimizer::LIST* hr)
{
    int index = val->GetIndex();
    int i = 0;
    if (parent->baseClasses)
    {
        for (Optimizer::BaseList* bc = parent->baseClasses; bc; bc = bc->next)
        {
            Optimizer::SimpleSymbol* sym = bc->sym;
            // we are setting sym->offset here for use later in this function
            if (sym->vbase)
            {
                Optimizer::SimpleType* tpl = Allocate<Optimizer::SimpleType>();
                tpl->type = Optimizer::st_pointer;
                tpl->size = Optimizer::sizeFromISZ(ISZ_ADDR);
                tpl->sizeFromType = ISZ_ADDR;
                tpl->btp = sym->tp;
                ObjType* base = Put(tpl);
                ObjField* field = factory.MakeField(sym->name, base, -1, index);
                val->Add(field);
            }
            else
            {
                ObjType* base = Put(sym->tp);
                ObjField* field = factory.MakeField(sym->name, base, bc->offset, index);
                val->Add(field);
            }
            if ((++i % 16) == 0)
            {
                index = factory.GetIndexManager()->NextType();
            }
        }
    }
    while (hr)
    {
        Optimizer::SimpleSymbol* sym = (Optimizer::SimpleSymbol*)hr->data;
        // this bears looking at, not sure why parameters are in this list
        if (!istype(sym) && sym->tp->type != Optimizer::st_aggregate && sym->storage_class != Optimizer::scc_parameter)
        {
            ObjType* base = Put(sym->tp);
            ObjField* field = factory.MakeField(sym->name, base, sym->offset, index);
            val->Add(field);
        }
        if ((++i % 16) == 0)
        {
            index = factory.GetIndexManager()->NextType();
        }
        hr = hr->next;
    }
}
void dbgtypes::EnumFields(ObjType* val, ObjType* base, int sz, Optimizer::LIST* hr)
{
    int index = val->GetIndex();
    int i = 0;
    while (hr)
    {
        Optimizer::SimpleSymbol* sym = (Optimizer::SimpleSymbol*)hr->data;
        ObjField* field = factory.MakeField(sym->name, base, sym->i, index);
        val->SetConstVal(sym->i);
        val->Add(field);
        if ((++i % 16) == 0)
        {
            index = factory.GetIndexManager()->NextType();
        }
        hr = hr->next;
    }
}
ObjType* dbgtypes::Function(Optimizer::SimpleType* tp)
{
    ObjFunction* val = nullptr;
    ObjType* rv = Put(tp->btp);
    int v = 0;
    if (tp->sp)
        switch (tp->sp->storage_class)
        {
            case Optimizer::scc_virtual:
            case Optimizer::scc_member:
            case Optimizer::scc_mutable:
                v = 4;  // has a this pointer
                break;
            default:
                if (tp->sp->iscdecl)
                    v = 1;
                else if (tp->sp->isstdcall)
                    v = 2;
                else if (tp->sp->ispascal)
                    v = 3;
                else if (tp->sp->isfastcall)
                    v = 5;
                else
                    v = 1;  // cdecl is default
                break;
        }
    else
    {
        v = 1;
    }
    if (tp->btp->type == Optimizer::st_struct || tp->btp->type == Optimizer::st_union)
        v |= 32;  // structured return value
    val = factory.MakeFunction("", rv);
    val->SetLinkage((ObjFunction::eLinkage)v);
    hash[tp] = val;  // for self referencing
    if (tp->sp->syms)
    {
        for (auto hr = tp->sp->syms; hr; hr = hr->next)
        {
            Optimizer::SimpleSymbol* s = (Optimizer::SimpleSymbol*)hr->data;
            val->Add(Put(s->tp));
        }
    }
    return val;
}
ObjType* dbgtypes::ExtendedType(Optimizer::SimpleType* tp)
{
    ObjType* val = nullptr;
    if (tp->type == Optimizer::st_pointer)
    {
        val = Put(tp->btp);

        if (tp->isvla)
        {
            val = factory.MakeType(ObjType::eVla, val);
        }
        else if (tp->isarray)
        {
            val = factory.MakeType(ObjType::eArray, val);
            val->SetSize(tp->size);
            if (tp->btp->size == 0)
                val->SetTop(tp->size);
            else
                val->SetTop(tp->size / tp->btp->size);
        }
        else
        {
            val = factory.MakeType(ObjType::ePointer, val);
            val->SetSize(tp->size);
        }
    }
    else if (tp->type == Optimizer::st_lref)
    {
        val = Put(tp->btp);
        val = factory.MakeType(ObjType::eLRef, val);
    }
    else if (tp->type == Optimizer::st_rref)
    {
        val = Put(tp->btp);
        val = factory.MakeType(ObjType::eRRef, val);
    }
    else if (tp->type == Optimizer::st_func)
    {
        val = Function(tp);
    }
    else
    {
        if (tp->bits)
        {
            val = Lookup(tp);
            if (!val)
                val = BasicType(tp);
            val = factory.MakeType(ObjType::eBitField, val);
            val->SetSize(tp->size);
            val->SetStartBit(tp->startbit);
            val->SetBitCount(tp->bits);
        }
        else if (tp->type == Optimizer::st_struct || tp->type == Optimizer::st_union)
        {
            auto tpo = tp;
            ObjType::eType sel;
            tp = tp->sp->tp;  // find instantiated version in case of C++ struct
            // uninstantiated templates resolve to Optimizer::st_i
            if (tp->type != Optimizer::st_struct && tp->type != Optimizer::st_union)
                return Put(tp);
            if (tp->type == Optimizer::st_union)
            {
                sel = ObjType::eUnion;
            }
            else
            {
                sel = ObjType::eStruct;
            }
            val = factory.MakeType(sel);
            val->SetSize(tp->size);
            hash[tpo] = val;  // for self-referencing
            if (tp->sp->syms)
                StructFields(sel, val, tp->size, tp->sp, tp->sp->syms);
            else
                StructFields(sel, val, tp->size, tp->sp, nullptr);
            val = TypeName(val, tp->sp->outputName);
        }
        else if (tp->type == Optimizer::st_ellipse)
        {
            // ellipse results in no debug info
        }
        else if (tp->type == Optimizer::st_memberptr)
        {
            static Optimizer::SimpleType intType;
            intType.type = Optimizer::st_i;
            intType.size = Optimizer::sizeFromISZ(-ISZ_UINT);
            intType.sizeFromType = -ISZ_UINT;
            val = Put(&intType);  // fixme
        }
        else  // enum
        {
            ObjType* base;
            if (tp->type == Optimizer::st_enum)
                if (tp->btp)
                {
                    base = Put(tp->btp);
                }
                else
                {
                    static Optimizer::SimpleType voidType;
                    voidType.type = Optimizer::st_void;
                    voidType.size = 0;
                    voidType.sizeFromType = 0;
                    base = Put(&voidType);
                }
            else
                base = Put(tp);

            val = factory.MakeType(ObjType::eEnum);
            val->SetSize(tp->size);
            EnumFields(val, base, tp->size, tp->sp->syms);
            val = TypeName(val, tp->sp->outputName);
        }
    }
    return val;
}
}  // namespace occx86