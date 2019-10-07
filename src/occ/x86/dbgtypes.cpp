/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "be.h"

#include "dbgtypes.h"
#include "ObjFactory.h"
#include "ObjType.h"
#include "ObjFile.h"

extern TYPE stdvoid;
extern TYPE stdint;
#define DEBUG_VERSION 4.0

bool dbgtypes::typecompare::operator()(const SimpleType* left, const SimpleType* right) const
{
    if (left->type == st_typedef)
        if (right->type != st_typedef)
            return true;
    if (right->type == st_typedef)
        if (left->type != st_typedef)
            return false;
    while (left->type == st_typedef)
        left = left->btp;
    while (right->type == st_typedef)
        right = right->btp;
    left = left;
    right = right;
    if (left->type < right->type)
        return true;
    else if (left->type == right->type)
    {
        switch (left->type)
        {
            case st_func:
                if (operator()(left->btp, right->btp))
                    return true;
                else if (!operator()(right->btp, left->btp))
                {
                    if (!!left->syms != !!right->syms)
                    {
                        if (!left->syms)
                            return true;
                    }
                    else if (left->syms && right->syms && left->syms && right->syms)
                    {
                        const LIST* hr1 = left->syms;
                        const LIST* hr2 = right->syms;
                        while (hr1 && hr2)
                        {
                            SimpleSymbol* sym1 = (SimpleSymbol*)hr1->data;
                            SimpleSymbol* sym2 = (SimpleSymbol*)hr2->data;
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
            case st_struct:
            case st_union:
            case st_enum:
                if (strcmp(left->sp->name, right->sp->name) < 0)
                    return true;
                break;
            case st_pointer:
                if (left->isarray && !right->isarray)
                    return true;
                else if (left->isarray != right->isarray)
                    return false;
                // fallthrough
            case st_lref:
            case st_rref:
                if (operator()(left->btp, right->btp))
                    return true;
                break;
            default:
                break;
        }
    }
    return false;
}

ObjType* dbgtypes::Put(SimpleType* tp)
{
    auto val = Lookup(tp);
    if (val)
        return val;
    if (tp->type == st_any || tp->type == st_aggregate)
    {
        val = factory.MakeType((ObjType::eType)42);
    }
    else if (tp->type == st_typedef)
    {
        if (!tp->sp->templateLevel)
        {
            val = Put(tp->btp);

            val = factory.MakeType(ObjType::eTypeDef, val);
        }
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
void dbgtypes::OutputTypedef(struct SimpleSymbol* sym) { Put(sym->tp); }
ObjType* dbgtypes::Lookup(SimpleType* tp)
{

    auto it = hash.find(tp);
    if (it != hash.end())
        return it->second;
    return nullptr;
}
ObjType* dbgtypes::BasicType(SimpleType* tp)
{
    static int basicTypes[] = {35, 34, 40, 40, 48, 41, 46, 49, 45, 0,  42, 42, 47, 50, 50,
                               43, 51, 44, 52, 72, 73, 74, 80, 81, 82, 88, 89, 90, 32};
    static int pointedTypes[] = {0,       0,       40 + 16, 40 + 16, 48 + 16, 41 + 16, 46 + 16, 49 + 16, 45 + 16, 0,
                                 42 + 16, 42 + 16, 47 + 16, 50 + 16, 50 + 16, 43 + 16, 51 + 16, 44 + 16, 52 + 16, 72 + 16,
                                 73 + 16, 74 + 16, 80 + 16, 81 + 16, 82 + 16, 88 + 16, 89 + 16, 90 + 16, 33};
    int n = 0;
    SimpleType* tp1 = tp;
    if (tp1->type <= st_void)
    {
        n = basicTypes[tp1->type];
    }
    else if (tp1->type == st_pointer && !tp1->isarray && !tp1->isvla && !tp1->bits)
    {
        SimpleType* tp2 = tp1->btp;
        if (tp2->type <= st_void)
        {
            n = pointedTypes[tp2->type];
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
bool istype(SimpleSymbol* sym)
{
    return sym->storage_class == sc_type || sym->storage_class == sc_typedef;
}
void dbgtypes::StructFields(ObjType::eType sel, ObjType* val, int sz, SimpleSymbol* parent, LIST* hr)
{
    int index = val->GetIndex();
    int i = 0;
    if (parent->baseClasses)
    {
        for (LIST *bc = parent->baseClasses; bc; bc = bc->next)
        { 
            SimpleSymbol* sym = (SimpleSymbol*)bc->data;
            // we are setting sym->offset here for use later in this function
            if (sym->vbase)
            {
                SimpleType* tpl = (SimpleType*)Alloc(sizeof(SimpleType));
                tpl->type = st_pointer;
                tpl->size = getSize(bt_pointer);
                tpl ->sizeFromType = ISZ_ADDR;
                tpl->btp = sym->tp;
                ObjType* base = Put(tpl);
                ObjField* field = factory.MakeField(sym->name, base, -1, index);
                val->Add(field);
            }
            else
            {
                ObjType* base = Put(sym->tp);
                ObjField* field = factory.MakeField(sym->name, base, sym->offset, index);
                val->Add(field);
            }
            if ((++i % 16) == 0)
            {
                index = factory.GetIndexManager()->NextType();
            }
            bc = bc->next;
        }
    }
    while (hr)
    {
        SimpleSymbol* sym = (SimpleSymbol *)hr->data;
        if (!istype(sym) && sym->tp->type != st_aggregate)
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
void dbgtypes::EnumFields(ObjType* val, ObjType* base, int sz, LIST* hr)
{
    int index = val->GetIndex();
    int i = 0;
    while (hr)
    {
        SimpleSymbol* sym = (SimpleSymbol*)hr->data;
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
ObjType* dbgtypes::Function(SimpleType* tp)
{
    ObjFunction* val = nullptr;
    ObjType* rv = Put(tp->btp);
    int v = 0;
    if (tp->sp)
        switch (tp->sp->storage_class)
        {
            case sc_virtual:
            case sc_member:
            case sc_mutable:
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
                    v = 1; // cdecl is default
                break;
        }
    else
    {
        v = 1;
    }
    if (tp->btp->type == st_struct || tp->btp->type == st_union)
        v |= 32;  // structured return value
    val = factory.MakeFunction("", rv);
    val->SetLinkage((ObjFunction::eLinkage)v);
    hash[tp] = val; // for self referencing
    if (tp->syms)
    {
        for (auto hr = tp->syms; hr; hr = hr->next)
        {
            SimpleSymbol *s = (SimpleSymbol*)hr->data;
            val->Add(Put(s->tp));
        }
    }
    return val;
}
ObjType* dbgtypes::ExtendedType(SimpleType* tp)
{
    ObjType* val = nullptr;
    if (tp->type == st_pointer)
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
            val->SetTop(tp->size / tp->btp->size);
        }
        else
        {
            val = factory.MakeType(ObjType::ePointer, val);
            val->SetSize(tp->size);
        }
    }
    else if (tp->type == st_lref)
    {
        val = Put(tp->btp);
        val = factory.MakeType(ObjType::eLRef, val);
    }
    else if (tp->type == st_rref)
    {
        val = Put(tp->btp);
        val = factory.MakeType(ObjType::eRRef, val);
    }
    else if (tp->type == st_func)
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
        else if (tp->type == st_struct)
        {
            ObjType::eType sel;
            tp = tp->sp->tp;  // find instantiated version in case of C++ struct
            if (tp->type == st_union)
            {
                sel = ObjType::eUnion;
            }
            else
            {
                sel = ObjType::eStruct;
            }
            val = factory.MakeType(sel);
            val->SetSize(tp->size);
            hash[tp] = val;  // for self-referencing
            if (tp->syms)
                StructFields(sel, val, tp->size, tp->sp, tp->syms);
            else
                StructFields(sel, val, tp->size, tp->sp, nullptr);
            val = TypeName(val, tp->sp->decoratedName);
        }
        else if (tp->type == st_ellipse)
        {
            // ellipse results in no debug info
        }
        else if (tp->type == st_memberptr)
        {
            static SimpleType intType;
            intType.type = st_i;
            intType.size = getSize(bt_int);
            intType.sizeFromType = -ISZ_UINT;
            val = Put(&intType);  // fixme
        }
        else                     // enum
        {
            ObjType* base;
            if (tp->type == st_enum)
                if (tp->btp)
                {
                    base = Put(tp->btp);
                }
                else
                {
                    static SimpleType voidType;
                    voidType.type = st_void;
                    voidType.size = 0;
                    voidType.sizeFromType = 0;
                    base = Put(&voidType);
                }
            else
                base = Put(tp);

            val = factory.MakeType(ObjType::eEnum);
            val->SetSize(tp->size);
            EnumFields(val, base, tp->size, tp->syms);
            val = TypeName(val, tp->sp->decoratedName);
        }
    }
    return val;
}
