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

bool dbgtypes::typecompare::operator()(const TYPE* left, const TYPE* right) const
{
    if (left->type == bt_typedef)
        if (right->type != bt_typedef)
            return true;
    if (right->type == bt_typedef)
        if (left->type != bt_typedef)
            return false;
    while (left->type == bt_typedef)
        left = left->btp;
    while (right->type == bt_typedef)
        right = right->btp;
    if (isconst(left) && !isconst(right))
        return true;
    else if (isconst(left) == isconst(right))
    {
        if (isvolatile(left) && !isvolatile(right))
            return true;
        else if (isvolatile(left) == isvolatile(right))
        {
            left = basetype(left);
            right = basetype(right);
            if (left->type < right->type)
                return true;
            else if (left->type == right->type)
            {
                switch (left->type)
                {
                    case bt_func:
                    case bt_ifunc:
                        if (operator()(left->btp, right->btp))
                            return true;
                        else if (!operator()(right->btp, left->btp))
                        {
                            if (!!left->syms != !!right->syms)
                            {
                                if (!left->syms)
                                    return true;
                            }
                            else if (left->syms && right->syms && left->syms->table && right->syms->table)
                            {
                                const SYMLIST* hr1 = left->syms->table[0];
                                const SYMLIST* hr2 = right->syms->table[0];
                                while (hr1 && hr2)
                                {
                                    SYMBOL* sym1 = (SYMBOL*)hr1->p;
                                    SYMBOL* sym2 = (SYMBOL*)hr2->p;
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
                    case bt_struct:
                    case bt_union:
                    case bt_class:
                    case bt_enum:
                        if (strcmp(left->sp->name, right->sp->name) < 0)
                            return true;
                        break;
                    case bt_pointer:
                        if (left->array && !right->array)
                            return true;
                        else if (left->array != right->array)
                            return false;
                        // fallthrough
                    case bt_lref:
                    case bt_rref:
                        if (operator()(left->btp, right->btp))
                            return true;
                        break;
                    default:
                        break;
                }
            }
        }
    }
    return false;
}

ObjType* dbgtypes::Put(TYPE* tp)
{
    auto val = Lookup(tp);
    if (val)
        return val;
    if (tp->type == bt_any || tp->type == bt_aggregate)
    {
        val = factory.MakeType((ObjType::eType)42);
    }
    else if (tp->type == bt_typedef)
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
void dbgtypes::OutputTypedef(SYMBOL* sym) { Put(sym->tp); }
ObjType* dbgtypes::Lookup(TYPE* tp)
{

    auto it = hash.find(tp);
    if (it != hash.end())
        return it->second;
    return nullptr;
}
ObjType* dbgtypes::BasicType(TYPE* tp)
{
    static int basicTypes[] = {35, 34, 40, 40, 48, 41, 46, 49, 45, 0,  42, 42, 47, 50, 50,
                               43, 51, 44, 52, 72, 73, 74, 80, 81, 82, 88, 89, 90, 32};
    static int pointedTypes[] = {0,       0,       40 + 16, 40 + 16, 48 + 16, 41 + 16, 46 + 16, 49 + 16, 45 + 16, 0,
                                 42 + 16, 42 + 16, 47 + 16, 50 + 16, 50 + 16, 43 + 16, 51 + 16, 44 + 16, 52 + 16, 72 + 16,
                                 73 + 16, 74 + 16, 80 + 16, 81 + 16, 82 + 16, 88 + 16, 89 + 16, 90 + 16, 33};
    int n = 0;
    TYPE* tp1 = basetype(tp);
    if (tp1->type <= bt_void)
    {
        n = basicTypes[tp1->type];
    }
    else if (tp1->type == bt_pointer && !tp1->array && !tp1->vla && !tp1->bits)
    {
        TYPE* tp2 = basetype(tp1->btp);
        if (tp2->type <= bt_void)
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
void dbgtypes::StructFields(ObjType::eType sel, ObjType* val, int sz, SYMBOL* parent, SYMLIST* hr)
{
    int index = val->GetIndex();
    int i = 0;
    if (parent->baseClasses)
    {
        BASECLASS* bc = parent->baseClasses;
        while (bc)
        {
            SYMBOL* sym = (SYMBOL*)bc->cls;
            // we are setting sym->offset here for use later in this function
            if (bc->isvirtual)
            {
                VBASEENTRY* vbase = parent->vbaseEntries;
                while (vbase)
                {
                    if (vbase->cls == bc->cls || sameTemplate(vbase->cls->tp, bc->cls->tp))
                    {
                        sym->offset = vbase->pointerOffset;
                        break;
                    }
                    vbase = vbase->next;
                }
                TYPE* tpl = (TYPE*)Alloc(sizeof(TYPE));
                tpl->type = bt_pointer;
                tpl->size = getSize(bt_pointer);
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
            bc = bc->next;
        }
    }
    while (hr)
    {
        SYMBOL* sym = hr->p;
        if (!istype(sym) && sym->tp->type != bt_aggregate)
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
void dbgtypes::EnumFields(ObjType* val, ObjType* base, int sz, SYMLIST* hr)
{
    int index = val->GetIndex();
    int i = 0;
    while (hr)
    {
        SYMBOL* sym = hr->p;
        ObjField* field = factory.MakeField(sym->name, base, sym->value.i, index);
        val->SetConstVal(sym->value.i);
        val->Add(field);
        if ((++i % 16) == 0)
        {
            index = factory.GetIndexManager()->NextType();
        }
        hr = hr->next;
    }
}
ObjType* dbgtypes::Function(TYPE* tp)
{
    ObjFunction* val = nullptr;
    ObjType* rv = Put(basetype(tp)->btp);
    int v = 0;
    if (basetype(tp)->sp)
        switch (basetype(tp)->sp->storage_class)
        {
            case sc_virtual:
            case sc_member:
            case sc_mutable:
                v = 4;  // has a this pointer
                break;
            default:
                switch (basetype(tp)->sp->linkage)
                {
                    default:
                    case lk_cdecl:
                        v = 1;
                        break;
                    case lk_stdcall:
                        v = 2;
                        break;
                    case lk_pascal:
                        v = 3;
                        break;
                    case lk_fastcall:
                        v = 4;
                        break;
                }
                break;
        }
    else
        v = 1;
    if (isstructured(basetype(tp)->btp))
        v |= 32;  // structured return value
    val = factory.MakeFunction("", rv);
    val->SetLinkage((ObjFunction::eLinkage)v);
    if (basetype(tp)->syms)
    {
        SYMLIST* hr = basetype(tp)->syms->table[0];
        while (hr)
        {
            SYMBOL* s = hr->p;
            val->Add(Put(s->tp));
            hr = hr->next;
        }
    }
    return val;
}
ObjType* dbgtypes::ExtendedType(TYPE* tp)
{
    tp = basetype(tp);
    ObjType* val = nullptr;
    if (tp->type == bt_pointer)
    {
        val = Put(tp->btp);

        if (tp->vla)
        {
            val = factory.MakeType(ObjType::eVla, val);
        }
        else if (tp->array)
        {
            val = factory.MakeType(ObjType::eArray, val);
            val->SetSize(tp->size);
            val->SetTop(tp->size / basetype(tp)->btp->size);
        }
        else
        {
            val = factory.MakeType(ObjType::ePointer, val);
            val->SetSize(tp->size);
        }
    }
    else if (tp->type == bt_lref)
    {
        val = Put(tp->btp);
        val = factory.MakeType(ObjType::eLRef, val);
    }
    else if (tp->type == bt_rref)
    {
        val = Put(tp->btp);
        val = factory.MakeType(ObjType::eRRef, val);
    }
    else if (isfunction(tp))
    {
        val = Function(tp);
    }
    else
    {
        if (tp->hasbits)
        {
            val = Lookup(tp);
            if (!val)
                val = BasicType(tp);
            val = factory.MakeType(ObjType::eBitField, val);
            val->SetSize(tp->size);
            val->SetStartBit(tp->startbit);
            val->SetBitCount(tp->bits);
        }
        else if (isstructured(tp))
        {
            ObjType::eType sel;
            tp = basetype(tp)->sp->tp;  // find instantiated version in case of C++ struct
            if (tp->type == bt_union)
            {
                sel = ObjType::eUnion;
            }
            else
            {
                sel = ObjType::eStruct;
            }
            val = factory.MakeType(sel);
            val->SetSize(basetype(tp)->size);
            hash[tp] = val;  // for self-referencing
            if (tp->syms)
                StructFields(sel, val, tp->size, tp->sp, tp->syms->table[0]);
            else
                StructFields(sel, val, tp->size, tp->sp, nullptr);
            val = TypeName(val, tp->sp->decoratedName);
        }
        else if (tp->type == bt_ellipse)
        {
            // ellipse results in no debug info
        }
        else if (tp->type == bt_templateselector)
        {
            val = factory.MakeType((ObjType::eType)42);
        }
        else  if (tp->type == bt_memberptr)
            val = Put(&stdint); // fixme
        else// enum
        {
            ObjType* base;
            if (tp->type == bt_enum)
                if (tp->btp)
                    base = Put(tp->btp);
                else
                    base = Put(&stdvoid);
            else
                base = Put(tp);

            val = factory.MakeType(ObjType::eEnum);
            val->SetSize(basetype(tp)->size);
            if (tp->syms)
                EnumFields(val, base, tp->size, tp->syms->table[0]);
            else
                EnumFields(val, base, tp->size, nullptr);
            val = TypeName(val, tp->sp->decoratedName);
        }
    }
    return val;                                                                                 
}
