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
#include <malloc.h>
#include <cstring>
#include <climits>
#include "ioptimizer.h"
#include "beinterfdefs.h"
#include "ialias.h"
#include "iblock.h"
#include "iflow.h"
#include "iloop.h"
#include "ilazy.h"
#include "ildata.h"
#include "OptUtils.h"
#include "output.h"
#include "iout.h"
#include "ilocal.h"
#include "memory.h"
#include "ioptutil.h"
#include "optmain.h"
#include "FNV_hash.h"
/* This is a partial implementation of the VLLPA algorithm in
 * Practical and Accurate Low-Level Pointer Analysis
 * Bolei Guo, Matthew J. Bridges, Spyridon Triantafyllis
 * Guilherme Ottoni, Easwaran Raman, David I. August
 *
 * Their implementation was designed to work on assembly language
 * code; in this implementation we are actually working with the intermediate
 * code so there is no vagary between arrays and other things, and we have
 * partial type information so we know what is a pointer and what is not to make
 * things just a tad cleaner.
 *
 * This only does the intraprocedural part - this compiler does not do
 * inter-procedural optimizations.
 *
 * a limitation of this implementation is it does not handle block assignments
 * or structures passed by value.
 */
namespace Optimizer
{
int cachedTempCount;
BITINT* uivBytes;
BITINT* processBits;

static bool changed;
static ALIASLIST* parmList;
static int processCount;

#pragma pack(1)
struct ptrint
{
    void* ptr;
    int intval;
};
#pragma pack()

static std::unordered_map < ptrint*, ALIASADDRESS*, OrangeC::Utils::fnv1a32_binary<sizeof(ptrint)>,
    OrangeC::Utils::bin_eql<sizeof(ptrint)>> addresses;
static std::unordered_map<IMODE**, std::list<ALIASNAME*>, OrangeC::Utils::fnv1a32_binary<sizeof(IMODE*)>,
    OrangeC::Utils::bin_eql<sizeof(IMODE*)>> mem;
static std::unordered_map<ptrint*, UIVHash*, OrangeC::Utils::fnv1a32_binary<sizeof(ptrint)>,
                          OrangeC::Utils::bin_eql<sizeof(ptrint)>> names;
static std::unordered_map<ALIASNAME**, ADDRBYNAME*, OrangeC::Utils::fnv1a32_binary<sizeof(ALIASNAME*)>,
                          OrangeC::Utils::bin_eql<sizeof(ALIASNAME*)>> addrNames;
static void ResetProcessed(void);
static void GatherInds(BITINT* p, int n, ALIASLIST* al);
void AliasInit(void)
{
    int i;
    for (i = 0; i < tempCount; i++)
    {
        tempInfo[i]->pointsto = nullptr;
        tempInfo[i]->modifiedBy = nullptr;
    }
    addresses.clear();
    names.clear();
    mem.clear();
    addrNames.clear();
    parmList = nullptr;
    uivBytes = nullptr;
    cachedTempCount = tempCount;
    processBits = nullptr;
    processCount = 0;
    changed = false;
}
void AliasRundown(void) { aFree(); }
static void PrintOffs(struct UIVOffset* offs)
{
    if (offs)
    {
        PrintOffs(offs->next);
        oprintf(icdFile, "@%d", offs->offset);
    }
}
static void PrintName(ALIASNAME* name, int offs)
{
    oprintf(icdFile, "(");
    if (!name)
    {
        oprintf(icdFile, "stub");
    }
    else if (name->byUIV)
    {
        putamode(nullptr, name->v.uiv->im);
        PrintOffs(name->v.uiv->offset);
    }
    else
    {
        putamode(nullptr, name->v.name);
    }
    oprintf(icdFile, ",%d)", offs);
}
static void PrintTemps(BITINT* modifiedBy)
{
    int i;
    if (modifiedBy)
    {
        oprintf(icdFile, "[");
        for (i = 1; i < termCount; i++)
            if (isset(modifiedBy, i))
                oprintf(icdFile, "T%d ", termMapUp[i]);
        oprintf(icdFile, "]");
    }
}
static void DumpAliases(void)
{
    oprintf(icdFile, "function: %s\n", currentFunction->name);
    int i;
    oprintf(icdFile, "Alias Dump:\n");
    for (auto aab : addresses)
    {
        ALIASADDRESS* aa = aab.second;
        ALIASLIST* al;
        ALIASADDRESS* aa1 = aa;
        while (aa1->merge)
            aa1 = aa1->merge;
        al = aa1->pointsto;
        PrintName(aa->name, aa->offset);
        oprintf(icdFile, ": ");
        while (al)
        {
            PrintName(al->address->name, al->address->offset);
            oprintf(icdFile, " ");
            al = al->next;
        }
        PrintTemps(aa1->modifiedBy);
        oprintf(icdFile, "\n");
    }
    for (i = 0; i < cachedTempCount; i++)
    {
        if (tempInfo[i]->pointsto)
        {
            ALIASLIST* al = tempInfo[i]->pointsto;
            oprintf(icdFile, "T%d:", i);
            while (al)
            {
                PrintName(al->address->name, al->address->offset);
                oprintf(icdFile, " ");
                al = al->next;
            }
            PrintTemps(tempInfo[i]->modifiedBy);
            oprintf(icdFile, "\n");
        }
    }
    {
        ALIASLIST* al = parmList;
        oprintf(icdFile, "UIV: ");
        while (al)
        {
            ALIASADDRESS* aa1 = al->address;
            while (aa1->merge)
                aa1 = aa1->merge;
            PrintName(aa1->name, aa1->offset);
            oprintf(icdFile, " ");
            al = al->next;
        }
        PrintTemps(uivBytes);
    }
}
static ALIASNAME* LookupMem(IMODE* im)
{
    switch (im->offset->type)
    {
        case se_global:
        case se_pc:
        case se_auto:
        case se_threadlocal:
            if (im->offset->sp->imvalue)
                im = im->offset->sp->imvalue;
            break;
        default:
            break;
    }
    auto it = mem.find(&im);
    if (it != mem.end())
    {
        for (auto p : it->second)
        {
            if ((p->byUIV == false && p->v.name == im) ||
                (p->byUIV == true && p->v.uiv->im == im && p->v.uiv->offset == nullptr))
            {
                return p;
            }
        }
    }
    else
    {
        IMODE** im2 = Allocate<IMODE*>();
        *im2 = im;
        mem[im2] = std::list<ALIASNAME*>();
        it = mem.find(im2);
    }
    auto p = Allocate<ALIASNAME>();
    p->v.name = im;
    switch (im->offset->type)
    {
        case se_auto:
        case se_global:
            p->v.uiv = aAllocate<UIV>();
            p->v.uiv->im = im;
            p->byUIV = true;
            break;
        default:
            break;
    }
    it->second.push_back(p);
    return p;
}
static void AliasUnion(ALIASLIST** dest, ALIASLIST* src)
{
    while (src)
    {
        ALIASLIST** q = dest;
        ALIASNAME* nm2 = src->address->name;
        IMODE* im2;
        if (nm2->byUIV)
            im2 = nm2->v.uiv->im;
        else
            im2 = nm2->v.name;
        ALIASLIST* q1 = *q;
        while (q1)
        {
            ALIASNAME* nm1 = q1->address->name;
            IMODE* im1;
            // we don't check the offset here because of the rule if the same
            // name is used with different offsets it is assumed to be an array.
            if (nm1 == nm2)
                break;
            if (nm1->byUIV)
                im1 = nm1->v.uiv->im;
            else
                im1 = nm1->v.name;
            if (im1 == im2)
                break;
            q1 = q1->next;
        }
        if (!*q)
        {
            ALIASLIST* al = aAllocate<ALIASLIST>();
            al->address = src->address;
            *q = al;
            changed = true;
        }
        src = src->next;
    }
}
static void AliasUnionParm(ALIASLIST** dest, ALIASLIST* src)
{
    while (src)
    {
        ALIASLIST** q = dest;
        while (*q)
        {
            // we don't check the offset here because of the rule if the same
            // name is used with different offsets it is assumed to be an array.
            if ((*q)->address->name == src->address->name)
                break;
            q = &(*q)->next;
        }
        if (!*q)
        {
            ALIASLIST* al = aAllocate<ALIASLIST>();
            al->address = src->address;
            *q = al;
            changed = true;
        }
        src = src->next;
    }
}
static ALIASNAME* LookupAliasName(ALIASNAME* name, int offset)
{
    ptrint str;
    str.ptr = name;
    str.intval = offset;
    auto it = names.find(&str);
    if (it != names.end())
        return it->second->result;
    ptrint* mystr = Allocate<ptrint>();
    memcpy(mystr, &str, sizeof(ptrint));

    UIVHash* uiv;
    uiv = aAllocate<UIVHash>();
    uiv->name = name;
    uiv->offset = offset;
    ALIASNAME* result;
    result = aAllocate<ALIASNAME>();
    result->byUIV = true;
    result->v.uiv = aAllocate<UIV>();
    if (name->byUIV)
    {
        *result->v.uiv = *name->v.uiv;
        result->v.uiv->alias = nullptr;
    }
    else
    {
        result->v.uiv->im = name->v.name;
    }
    result->v.uiv->offset = aAllocate<UIVOffset>();
    result->v.uiv->offset->offset = offset;
    if (name->byUIV)
        result->v.uiv->offset->next = name->v.uiv->offset;
    uiv->result = result;
    names[mystr] = uiv;
    return result;
}
static ALIASNAME* GetAliasName(ALIASNAME* name, int offset)
{
    ptrint str;
    str.ptr = name;
    str.intval = offset;
    auto it = names.find(&str);
    if (it != names.end())
        return it->second->result;
    return nullptr;
}
static ALIASADDRESS* LookupAddress(ALIASNAME* name, int offset)
{
    ptrint str;
    str.ptr = name;
    str.intval = offset;
    IMODE* im;
    LIST* li;
    auto it = addresses.find(&str);
    if (it != addresses.end())
        return it->second;
    ALIASADDRESS* addr;
    addr = aAllocate<ALIASADDRESS>();
    addr->name = name;
    addr->offset = offset;
    ptrint* mystr = Allocate<ptrint>();
    memcpy(mystr, &str, sizeof(ptrint));
    addresses[mystr] = addr;
    if (addr->name->byUIV)
    {
        im = addr->name->v.uiv->im;
    }
    else
    {
        im = addr->name->v.name;
    }
    switch (im->offset->type)
    {
        case se_auto:
            //			if (im->offset->sp->storage_class != scc_parameter)
            break;
        case se_global: {
            ALIASLIST* l = aAllocate<ALIASLIST>();
            l->address = addr;
            AliasUnion(&parmList, l);
        }
        break;
        default:
            break;
    }
    li = aAllocate<LIST>();
    li->data = addr;
    li->next = name->addresses;
    name->addresses = li;

    auto it1 = addrNames.find(&name);
    if (it1 == addrNames.end())
    {
        auto q = aAllocate<ADDRBYNAME>();
        q->name = name;
        ALIASNAME** name1 = Allocate<ALIASNAME*>();
        *name1 = name;
        addrNames[name1] = q;
        ALIASLIST* ali = aAllocate<ALIASLIST>();
        ali->address = addr;
        ali->next = q->addresses;
        q->addresses = ali;
    }
    else
    {
        ALIASLIST* ali = aAllocate<ALIASLIST>();
        ali->address = addr;
        ali->next = it1->second->addresses;
        it1->second->addresses = ali;
    }
    return addr;
}
static ALIASADDRESS* GetAddress(ALIASNAME* name, int offset)
{
    ptrint str;
    str.ptr = name;
    str.intval = offset;
    auto it = addresses.find(&str);
    if (it != addresses.end())
        return it->second;
    return nullptr;
}
static void CreateMem(IMODE* im)
{
    ALIASNAME* p;
    if (im->offset->type != se_pc && im->offset->type != se_sub)
    {
        if (im->mode == i_immed)
        {
            if (!im->offset->sp->imvalue)
            {
                // make one in the case of global addresses that aren't used
                // directly
                IMODE* ap2 = Allocate<IMODE>();
                ap2->offset = im->offset;
                ap2->mode = i_direct;
                ap2->size = ISZ_ADDR;
                im->offset->sp->imvalue = ap2;
            }
            p = LookupMem(im->offset->sp->imvalue);
        }
        else
        {
            ALIASADDRESS* aa;
            p = LookupMem(im);
            p = LookupAliasName(p, 0);
        }
        if (im->size == ISZ_ADDR || im->offset->type == se_global)
        {
            ALIASADDRESS* aa;
            aa = LookupAddress(p, 0);
            if (!aa->pointsto)
            {
                ALIASNAME* an = LookupAliasName(p, 0);
                aa->pointsto = aAllocate<ALIASLIST>();
                aa->pointsto->address = LookupAddress(an, 0);
            }
        }
    }
}
static void Createaddresses(void)
{
    QUAD* head = intermed_head;
    while (head)
    {
        if (head->dc.opcode != i_assnblock && head->dc.opcode != i_clrblock)
            if (head->dc.opcode != i_label && head->dc.opcode != i_passthrough && !head->ignoreMe)
            {
                if (head->ans && !(head->temps & TEMP_ANS) && head->ans->mode != i_immed)
                {
                    CreateMem(head->ans);
                }
                if (head->dc.left && !(head->temps & TEMP_LEFT))
                {
                    // fixme...
                    if (head->dc.left->mode == i_direct ||
                        (!isarithmeticconst(head->dc.left->offset) && head->dc.left->offset->type != se_labcon &&
                         head->dc.left->offset->type != se_add))
                        CreateMem(head->dc.left);
                }
                if (head->dc.right && !(head->temps & TEMP_RIGHT))
                {
                    // fixme...
                    if (head->dc.right->mode == i_direct ||
                        (!isarithmeticconst(head->dc.right->offset) && head->dc.right->offset->type != se_labcon &&
                         head->dc.right->offset->type != se_add))
                        CreateMem(head->dc.right);
                }
            }
        head = head->fwd;
    }
}
static bool IntersectsUIV(ALIASLIST* list)
{
    while (list)
    {
        if (list->address->name->byUIV)
            return true;
        list = list->next;
    }
    return false;
}
static void HandlePhi(QUAD* head)
{
    if (tempInfo[head->dc.v.phi->T0]->enode->sp->imvalue->size == ISZ_ADDR)
    {
        struct _phiblock* pb = head->dc.v.phi->temps;
        ALIASLIST* l = nullptr;
        bool xchanged = changed;
        while (pb)
        {
            AliasUnion(&l, tempInfo[pb->Tn]->pointsto);
            pb = pb->next;
        }
        changed = xchanged;
        tempInfo[head->dc.v.phi->T0]->pointsto = l;
    }
}
static void HandleAssn(QUAD* head)
{
    if (head->ans == head->dc.left)
        return;
    if (head->ans->mode == i_ind)
    {
        if (head->temps & TEMP_LEFT)
        {
            // ind, temp
            ALIASLIST* addr;
            ALIASLIST* src = tempInfo[head->dc.left->offset->sp->i]->pointsto;
            addr = tempInfo[head->ans->offset->sp->i]->pointsto;
            while (addr)
            {
                AliasUnion(&addr->address->pointsto, src);
                addr = addr->next;
            }
        }
        else if (head->dc.left->mode == i_immed && head->dc.left->size == ISZ_ADDR && head->dc.left->offset->type != se_labcon)
        {
            // ind, immed
            ALIASLIST* addr;
            ALIASNAME* an = LookupMem(head->ans->offset->sp->imvalue);
            ALIASADDRESS* aa;
            if (head->ans->mode == i_direct)
                an = LookupAliasName(an, 0);
            aa = LookupAddress(an, 0);
            addr = tempInfo[head->ans->offset->sp->i]->pointsto;
            while (addr)
            {
                AliasUnion(&addr->address->pointsto, aa->pointsto);
                addr = addr->next;
            }
        }
    }
    else if (head->dc.left->mode == i_ind && (head->temps & TEMP_ANS))
    {
        // temp, ind
        ALIASLIST* result = nullptr;
        ALIASLIST* addr = tempInfo[head->dc.left->offset->sp->i]->pointsto;
        bool xchanged = changed;
        while (addr)
        {
            if (addr->address->name->byUIV)
            {
                if (!IntersectsUIV(addr->address->pointsto))
                {
                    ALIASNAME* an = LookupAliasName(addr->address->name, addr->address->offset);
                    ALIASADDRESS* aa = LookupAddress(an, 0);
                    ALIASLIST* al = aAllocate<ALIASLIST>();
                    al->address = aa;
                    AliasUnion(&addr->address->pointsto, al);
                }
            }
            AliasUnion(&result, addr->address->pointsto);
            addr = addr->next;
        }
        changed = xchanged;
        tempInfo[head->ans->offset->sp->i]->pointsto = result;
    }
    else if (head->ans->size == ISZ_ADDR)
    {
        if (!(head->temps & TEMP_ANS) && !head->ans->retval)
        {
            if (head->temps & TEMP_LEFT)
            {
                // mem, temp
                ALIASLIST* al;
                ALIASNAME* an = LookupMem(head->ans);
                ALIASADDRESS* aa;
                an = LookupAliasName(an, 0);
                aa = LookupAddress(an, 0);
                AliasUnion(&aa->pointsto, tempInfo[head->dc.left->offset->sp->i]->pointsto);
            }
            else if (head->dc.left->mode == i_immed && head->dc.left->size == ISZ_ADDR && head->dc.left->offset->type != se_labcon)
            {
                // mem, immed
                ALIASNAME* an2 = LookupMem(head->dc.left);
                ALIASADDRESS* aa2 = LookupAddress(an2, 0);
                if (head->ans->offset->sp->imvalue)
                {
                    ALIASNAME* an = LookupMem(head->ans->offset->sp->imvalue);
                    ALIASADDRESS* aa;
                    ALIASLIST* al = aAllocate<ALIASLIST>();
                    al->address = aa2;
                    if (head->ans->mode == i_direct)
                        an = LookupAliasName(an, 0);
                    aa = LookupAddress(an, 0);
                    AliasUnion(&aa->pointsto, al);
                }
            }
        }
        else if (head->temps & TEMP_ANS)
        {
            if (head->dc.left->mode == i_immed && head->dc.left->size == ISZ_ADDR && head->dc.left->offset->type != se_labcon &&
                !isintconst(head->dc.left->offset))
            {
                // temp, immed
                bool xchanged = changed;
                ALIASNAME* an = LookupMem(head->dc.left);
                ALIASADDRESS* aa = LookupAddress(an, 0);
                ALIASLIST* al = aAllocate<ALIASLIST>();
                al->address = aa;
                tempInfo[head->ans->offset->sp->i]->pointsto = nullptr;
                AliasUnion(&tempInfo[head->ans->offset->sp->i]->pointsto, al);
                changed = xchanged;
            }
            else if (head->dc.left->retval)
            {
                AliasUnion(&tempInfo[head->ans->offset->sp->i]->pointsto, parmList);
            }
            else if (!(head->temps & TEMP_LEFT) && head->dc.left->mode == i_direct)
            {
                // temp, mem
                ALIASLIST* result = nullptr;
                ALIASNAME* an = LookupMem(head->dc.left);
                ALIASADDRESS* aa;
                ALIASLIST* addr;
                bool xchanged = changed;
                an = LookupAliasName(an, 0);
                aa = LookupAddress(an, 0);
                AliasUnion(&tempInfo[head->ans->offset->sp->i]->pointsto, aa->pointsto);
                changed = xchanged;
            }
            else if (head->temps & TEMP_LEFT)
            {
                // temp, temp
                AliasUnion(&tempInfo[head->ans->offset->sp->i]->pointsto, tempInfo[head->dc.left->offset->sp->i]->pointsto);
            }
        }
    }
}
static int InferOffset(IMODE* im)
{
    QUAD* q = tempInfo[im->offset->sp->i]->instructionDefines;
    if (q)
    {
        if (q->dc.opcode == i_add)
        {
            if ((q->temps & TEMP_LEFT) && q->dc.left->mode == i_direct)
            {
                if (q->dc.right->mode == i_immed && isintconst(q->dc.right->offset))
                    return q->dc.right->offset->i;
            }
            else if ((q->temps & TEMP_RIGHT) && q->dc.right->mode == i_direct)
            {
                if (q->dc.left->mode == i_immed && isintconst(q->dc.left->offset))
                    return q->dc.left->offset->i;
            }
        }
        else if (q->dc.opcode == i_sub)
        {
            if ((q->temps & TEMP_LEFT) && q->dc.left->mode == i_direct)
            {
                if (q->dc.right->mode == i_immed && isintconst(q->dc.right->offset))
                    return -q->dc.right->offset->i;
            }
        }
        else if (q->dc.opcode == i_lsl)
        {
            if (q->dc.right->mode == i_immed && isintconst(q->dc.right->offset))
                if (q->temps & TEMP_LEFT)
                    return InferOffset(q->dc.left) << q->dc.right->offset->i;
        }
        else if (q->dc.opcode == i_mul)
        {
            if (q->dc.left->mode == i_immed && isintconst(q->dc.left->offset))
                if (q->temps & TEMP_RIGHT)
                    return InferOffset(q->dc.right) * q->dc.left->offset->i;
            if (q->dc.right->mode == i_immed && isintconst(q->dc.right->offset))
                if (q->temps & TEMP_LEFT)
                    return InferOffset(q->dc.left) * q->dc.right->offset->i;
        }
    }
    return 0;
}
static int InferStride(IMODE* im)
{
    QUAD* q = tempInfo[im->offset->sp->i]->instructionDefines;
    if (q)
    {
        if (q->dc.opcode == i_lsl)
        {
            if ((q->temps & TEMP_LEFT) && q->dc.left->mode == i_direct)
            {
                if (q->dc.right->mode == i_immed && isintconst(q->dc.right->offset))
                    return 1 << q->dc.right->offset->i;
            }
        }
        else if (q->dc.opcode == i_mul || q->dc.opcode == i_add || q->dc.opcode == i_sub)
        {
            IMODE* one = q->dc.left;
            IMODE* two = q->dc.right;
            if (one->mode == i_immed && isintconst(one->offset))
            {
                IMODE* three = one;
                one = two;
                two = three;
            }
            if (one->mode == i_direct && one->offset->type == se_tempref)
            {
                if (two->mode == i_immed && isintconst(two->offset))
                {
                    if (q->dc.opcode == i_add || q->dc.opcode == i_sub)
                        return InferStride(one);
                    return two->offset->i;
                }
            }
        }
    }
    return 1;
}
static void SetStride(ALIASADDRESS* addr, int stride)
{
    auto it = addrNames.find(&addr->name);
    if (it != addrNames.end())
    {
        ALIASLIST* addresses = it->second->addresses;
        while (addresses)
        {
            ALIASADDRESS* scan = addresses->address;
            while (scan)
            {
                if (addr != scan && addr->name == scan->name)
                {
                    if (addr->offset < scan->offset)
                    {
                        int o2 = addr->offset + (scan->offset - addr->offset) % stride;
                        if (addr->offset == o2)
                        {
                            AliasUnion(&addr->pointsto, scan->pointsto);
                            scan->merge = addr;
                        }
                        else
                        {
                            ALIASADDRESS* sc2 = LookupAddress(addr->name, o2);
                            if (sc2 && sc2 != scan)
                            {
                                AliasUnion(&sc2->pointsto, scan->pointsto);
                                scan->merge = sc2;
                            }
                        }
                    }
                }
                scan = scan->next;
            }
            addresses = addresses->next;
        }
    }
}
static void Infer(IMODE* ans, IMODE* reg, ALIASLIST* pointsto)
{
    if (pointsto)
    {
        ALIASLIST* result = nullptr;
        int c = InferOffset(reg);
        int l = InferStride(reg);
        if (l)
        {
            bool xchanged = changed;
            while (pointsto)
            {
                ALIASADDRESS* addr = LookupAddress(pointsto->address->name, pointsto->address->offset + c);
                ALIASLIST* al = aAllocate<ALIASLIST>();
                al->address = addr;
                AliasUnion(&result, al);
                SetStride(pointsto->address, l);
                pointsto = pointsto->next;
            }
            changed = xchanged;
            AliasUnion(&tempInfo[ans->offset->sp->i]->pointsto, result);
        }
    }
}
static void HandleAdd(QUAD* head)
{
    if ((head->ans->size == ISZ_ADDR) && (head->temps & TEMP_ANS))
    {
        if (head->dc.opcode == i_add && head->dc.left->mode == i_immed)
        {
            if (head->temps & TEMP_RIGHT)
            {
                if (isintconst(head->dc.left->offset))
                {
                    // C + R
                    ALIASLIST* scan = tempInfo[head->dc.right->offset->sp->i]->pointsto;
                    ALIASLIST* result = nullptr;
                    bool xchanged = changed;
                    while (scan)
                    {
                        ALIASADDRESS* addr = LookupAddress(scan->address->name, scan->address->offset + head->dc.left->offset->i);
                        ALIASLIST* al = aAllocate<ALIASLIST>();
                        al->address = addr;
                        AliasUnion(&result, al);
                        scan = scan->next;
                    }
                    changed = xchanged;
                    AliasUnion(&tempInfo[head->ans->offset->sp->i]->pointsto, result);
                }
                else
                {
                    // p + R
                    if (head->dc.left->offset->type != se_labcon)  // needed for exports
                    {
                        ALIASNAME* nm = LookupMem(head->dc.left->offset->sp->imvalue);
                        ALIASADDRESS* aa = LookupAddress(nm, 0);
                        ALIASLIST* al = aAllocate<ALIASLIST>();
                        al->address = aa;
                        Infer(head->ans, head->dc.right, al);
                    }
                }
            }
            else if (head->dc.right->mode == i_immed)
            {
                if (!isintconst(head->dc.left->offset) && head->dc.left->offset->type != se_labcon)
                {
                    // p + C
                    ALIASNAME* nm = LookupMem(head->dc.left->offset->sp->imvalue);
                    ALIASADDRESS* aa = LookupAddress(nm, head->dc.right->offset->i);
                    ALIASLIST* al = aAllocate<ALIASLIST>();
                    al->address = aa;
                    AliasUnion(&tempInfo[head->ans->offset->sp->i]->pointsto, al);
                }
                else if (!isintconst(head->dc.right->offset) && head->dc.right->offset->type != se_labcon)
                {
                    // C + p
                    ALIASNAME* nm = LookupMem(head->dc.right->offset->sp->imvalue);
                    ALIASADDRESS* aa = LookupAddress(nm, head->dc.left->offset->i);
                    ALIASLIST* al = aAllocate<ALIASLIST>();
                    al->address = aa;
                    AliasUnion(&tempInfo[head->ans->offset->sp->i]->pointsto, al);
                }
            }
        }
        else if (head->dc.right->mode == i_immed)
        {

            if (head->temps & TEMP_LEFT)
            {
                if (isintconst(head->dc.right->offset))
                {
                    // R+C
                    int c = head->dc.opcode == i_add ? head->dc.right->offset->i : -head->dc.right->offset->i;
                    ALIASLIST* scan = tempInfo[head->dc.left->offset->sp->i]->pointsto;
                    ALIASLIST* result = nullptr;
                    bool xchanged = changed;
                    while (scan)
                    {
                        ALIASADDRESS* addr = LookupAddress(scan->address->name, scan->address->offset + c);
                        ALIASLIST* al = aAllocate<ALIASLIST>();
                        al->address = addr;
                        AliasUnion(&result, al);
                        scan = scan->next;
                    }
                    changed = xchanged;
                    AliasUnion(&tempInfo[head->ans->offset->sp->i]->pointsto, result);
                }
                else
                {
                    // R + p
                    if (head->dc.right->offset->type != se_labcon)  // needed for exports
                    {
                        ALIASNAME* nm = LookupMem(head->dc.right->offset->sp->imvalue);
                        ALIASADDRESS* aa = LookupAddress(nm, 0);
                        ALIASLIST* al = aAllocate<ALIASLIST>();
                        al->address = aa;
                        Infer(head->ans, head->dc.left, al);
                    }
                }
            }
        }
        else if ((head->temps & (TEMP_LEFT | TEMP_RIGHT)) == (TEMP_LEFT | TEMP_RIGHT))
        {
            // R+R
            ALIASLIST* src;
            IMODE* one = head->dc.left;
            IMODE* two = head->dc.right;
            if (two->size == ISZ_ADDR)
            {
                IMODE* three = one;
                one = two;
                two = three;
            }
            if (one->size == ISZ_ADDR)
            {
                // now one has the pointer, two has something else
                src = tempInfo[one->offset->sp->i]->pointsto;
                Infer(head->ans, two, src);
            }
        }
    }
}
static void HandleAssnBlock(QUAD* head)
{
    ALIASNAME* dest = nullptr;
    if ((head->temps & TEMP_LEFT) && head->dc.left->mode == i_direct)
    {
        // we don't support writing to arbitrary memory, e.g. a pointer returned from a function call
        return;
    }
    else if (head->dc.left->mode == i_immed)
    {
        dest = LookupMem(head->dc.left);
        dest = LookupAliasName(dest, 0);
    }
    else
    {
        diag("HandleAssnBlock: invalid dest type");
    }

    if (head->dc.right->mode == i_direct && ((head->temps & TEMP_RIGHT) || head->dc.right->retval))
    {
        ALIASLIST* src = tempInfo[head->dc.right->offset->sp->i]->pointsto;
        while (src)
        {
            ALIASNAME* srcn = src->address->name;
            LIST* addr = srcn->addresses;
            while (addr)
            {
                ALIASADDRESS* aa = (ALIASADDRESS*)addr->data;
                ALIASADDRESS* aadest = LookupAddress(dest, aa->offset);
                AliasUnion(&aadest->pointsto, aa->pointsto);
                addr = addr->next;
            }
            src = src->next;
        }
    }
    else if (head->dc.right->mode == i_immed)
    {
        ALIASNAME* src = LookupMem(head->dc.right);
        LIST* addr;
        addr = src->addresses;
        while (addr)
        {
            ALIASADDRESS* aa = (ALIASADDRESS*)addr->data;
            ALIASADDRESS* aadest = LookupAddress(dest, aa->offset);
            AliasUnion(&aadest->pointsto, aa->pointsto);
            addr = addr->next;
        }
    }
    else
    {
        diag("HandleAssnBlock: invalid src type");
    }
}
static void HandleParmBlock(QUAD* head) {}
static void HandleParm(QUAD* head)
{
    if (head->dc.left->size == ISZ_ADDR)
    {
        // temp, mem
        ALIASLIST *result = nullptr, **base = nullptr, *addr;
        if (head->temps & TEMP_LEFT)
        {
            base = &tempInfo[head->dc.left->offset->sp->i]->pointsto;
        }
        else if (!isintconst(head->dc.left->offset))
        {
            ALIASNAME* an;
            ALIASADDRESS* aa;
            switch (head->dc.left->offset->type)
            {
                case se_labcon:
                case se_global:
                case se_pc:
                case se_threadlocal:
                    return;
                default:
                    break;
            }
            an = LookupMem(head->dc.left->offset->sp->imvalue);
            if (head->dc.left->mode == i_direct)
                an = LookupAliasName(an, 0);
            aa = LookupAddress(an, 0);
            base = &aa->pointsto;
        }
        if (base)
        {
            addr = *base;
            AliasUnionParm(&parmList, (*base));
            while (addr)
            {
                if (addr->address->name->byUIV)
                {
                    if (!IntersectsUIV(addr->address->pointsto))
                    {
                        ALIASNAME* an = LookupAliasName(addr->address->name, 0);
                        ALIASADDRESS* aa = LookupAddress(an, 0);
                        ALIASLIST* al = aAllocate<ALIASLIST>();
                        al->address = aa;
                        AliasUnion(&addr->address->pointsto, al);
                    }
                }
                addr = addr->next;
            }
        }
    }
}
static void AliasesOneBlock(BLOCK* b)
{
    QUAD* head = b->head;
    while (head != b->tail->fwd)
    {
        switch (head->dc.opcode)
        {
            case i_assnblock:
                HandleAssnBlock(head);
                break;
            case i_parmblock:
                HandleParmBlock(head);
                break;
            case i_parm:
                HandleParm(head);
                break;
            case i_phi:
                HandlePhi(head);
                break;
            case i_assn:
                HandleAssn(head);
                break;
            case i_add:
            case i_sub:
                HandleAdd(head);
                break;
            default:
                break;
        }
        head = head->fwd;
    }
}
static void GatherAliases(BLOCK *b)
{
    AliasesOneBlock(b);
    for (auto d = b->dominates; d; d = d->next)
        AliasesOneBlock(d->block);
}
static void GatherAliases(LOOP* lp)
{
    bool xchanged = changed;
    do
    {
        LIST* lt = lp->contains;
        changed = false;
        while (lt)
        {
            lp = (LOOP*)lt->data;
            if (lp->type == LT_BLOCK)
                AliasesOneBlock(lp->entry);
            else
                GatherAliases(lp);
            lt = lt->next;
        }
        if (changed)
            xchanged = true;
    } while (changed);
    changed = xchanged;
}
static void ormap(BITINT* dest, BITINT* src)
{
    int n = (termCount + BITINTBITS - 1) / BITINTBITS;
    int i;
    for (i = 0; i < n; i++)
    {
        if (~*dest & *src)
        {
            changed = true;
            *dest |= *src;
        }
        dest++, src++;
    }
}
static void andmap(BITINT* dest, BITINT* src)
{
    int n = (termCount + BITINTBITS - 1) / BITINTBITS;
    int i;
    for (i = 0; i < n; i++)
    {
        *dest &= *src;
        dest++, src++;
    }
}
static void complementmap(BITINT* dest)
{
    int n = (termCount + BITINTBITS - 1) / BITINTBITS;
    int i;
    for (i = 0; i < n; i++)
    {
        *dest = ~*dest;
        dest++;
    }
}
static void scanDepends(BITINT* bits, ALIASLIST* alin)
{
    ALIASLIST* al = alin;
    al = alin;
    while (al)
    {
        ALIASADDRESS* aa2 = (ALIASADDRESS*)al->address;
        IMODE* im;
        while (aa2->merge)
            aa2 = aa2->merge;
        if (!isset(processBits, aa2->processIndex))
        {
            setbit(processBits, aa2->processIndex);
            if (aa2->modifiedBy)
                ormap(bits, aa2->modifiedBy);
            scanDepends(bits, aa2->pointsto);
        }
        al = al->next;
    }
}
void AliasStruct(BITINT* bits, IMODE* ans, IMODE* left, IMODE* right)
{
    ALIASLIST* src;
    int i, n = ans->offset->i;
    if (left->offset->type == se_tempref && left->mode == i_direct)
    {
        src = tempInfo[left->offset->sp->i]->pointsto;
        while (src)
        {
            ALIASADDRESS* aa = src->address;
            while (aa->merge)
                aa = aa->merge;
            for (i = 0; i < n; i++)
            {
                ALIASNAME* an = GetAliasName(aa->name, i);
                if (an)
                {
                    ALIASADDRESS* aa2 = LookupAddress(an, 0);
                    while (aa2->merge)
                        aa2 = aa2->merge;
                    if (aa2 && aa2->modifiedBy)
                    {
                        ormap(bits, aa2->modifiedBy);
                    }
                }
            }
            src = src->next;
        }
        setbit(bits, termMap[left->offset->sp->i]);
        return;
    }
    else if (left->mode == i_immed)
    {
        ALIASNAME* an = LookupMem(left);
        ALIASADDRESS* aa;
        for (i = 0; i < n; i++)
        {
            ALIASNAME* an2 = GetAliasName(an, i);
            if (an2)
            {
                aa = LookupAddress(an2, 0);
                while (aa->merge)
                    aa = aa->merge;
                if (aa && aa->modifiedBy)
                {
                    ormap(bits, aa->modifiedBy);
                }
                ResetProcessed();
                scanDepends(bits, aa->pointsto);
            }
        }
        return;
    }
    else
    {
        diag("AliasStruct: invalid src type");
    }
}
void AliasGosub(QUAD* tail, BITINT* parms, BITINT* bits, int n)
{
    int i;
    andmap(bits, uivBytes);
    tail = tail->back;
    while (tail && tail->dc.opcode != i_block && tail->dc.opcode != i_gosub && tail->dc.opcode != i_label)
    {
        if (tail->dc.opcode == i_parm)
        {
            if (tail->temps & TEMP_LEFT)
            {
                int n = tail->dc.left->offset->sp->i;
                ALIASLIST* al = tempInfo[n]->pointsto;
                ResetProcessed();
                scanDepends(parms, al);
                if (tempInfo[n]->indTerms)
                    andmap(parms, tempInfo[n]->indTerms);
            }
            else if (tail->dc.left->mode == i_immed && !isintconst(tail->dc.left->offset) && !isfloatconst(tail->dc.left->offset) &&
                     !iscomplexconst(tail->dc.left->offset) && tail->dc.left->offset->type != se_labcon)
            {
                SimpleType* tp = tail->dc.left->offset->sp->tp;
                while (tp->type == st_pointer)
                    tp = tp->btp;
                if (tail->dc.left->offset->sp->tp->isarray || tp->type == st_struct || tp->type == st_union || tp->type == st_class)
                {
                    ALIASNAME* an = LookupMem(tail->dc.left);
                    int n = tail->dc.left->offset->sp->tp->isarray ? tail->dc.left->offset->sp->tp->size : tp->size;
                    for (i = 0; i < n; i++)
                    {
                        ALIASADDRESS* aa = GetAddress(an, i);
                        if (aa)
                        {
                            while (aa->merge)
                                aa = aa->merge;
                            if (aa && aa->modifiedBy)
                            {
                                ormap(parms, aa->modifiedBy);
                            }
                            ResetProcessed();
                            scanDepends(parms, aa->pointsto);
                        }
                    }
                }
                else
                {
                    AliasUses(parms, tail->dc.left, true);
                }
            }
            else
            {
                AliasUses(parms, tail->dc.left, true);
            }
        }
        tail = tail->back;
    }
    for (i = 0; i < n; i++)
    {
        *bits &= ~*parms;
        bits++, parms++;
    }
}
void AliasUses(BITINT* bits, IMODE* im, bool rhs)
{
    if (im)
    {
        if (rhs)
        {
            if (im->offset->type == se_tempref)
            {
                ormap(bits, tempInfo[im->offset->sp->i]->modifiedBy);
                if (im->mode == i_direct)
                {
                    im = LookupLoadTemp(im, im);
                }
                setbit(bits, termMap[im->offset->sp->i]);
            }
            else if (im->mode == i_direct)
            {
                ALIASNAME* an = LookupMem(im);
                ALIASADDRESS* aa;
                an = LookupAliasName(an, 0);
                aa = LookupAddress(an, 0);
                while (aa->merge)
                    aa = aa->merge;
                if (aa->modifiedBy)
                    ormap(bits, aa->modifiedBy);
                im = GetLoadTemp(im);
                if (im)
                {
                    setbit(bits, termMap[im->offset->sp->i]);
                }
            }
            else if (im->mode == i_immed && !isintconst(im->offset) && !isimaginaryconst(im->offset) &&
                     !iscomplexconst(im->offset) && !isfloatconst(im->offset) && im->offset->type != se_labcon)
            {
                ALIASNAME* an = LookupMem(im);
                ALIASADDRESS* aa;
                aa = LookupAddress(an, 0);
                while (aa->merge)
                    aa = aa->merge;
                if (aa->modifiedBy)
                    ormap(bits, aa->modifiedBy);
                im = im->offset->sp->imvalue;
                if (im)
                {
                    im = GetLoadTemp(im);
                    if (im)
                    {
                        setbit(bits, termMap[im->offset->sp->i]);
                    }
                }
            }
        }
        else
        {
            if (im->offset->type == se_tempref)
            {
                ormap(bits, tempInfo[im->offset->sp->i]->modifiedBy);
                if (im->mode == i_direct)
                {
                    im = LookupLoadTemp(im, im);
                    setbit(bits, termMap[im->offset->sp->i]);
                }
                else
                {
                    auto al = tempInfo[im->offset->sp->i]->pointsto;
                    while (al)
                    {
                        auto addr = al->address;
                        while (addr->merge)
                            addr = addr->merge;
                        if (addr->name->byUIV)
                        {
                            AliasUses(bits, addr->name->v.uiv->im, true);
                        }
                        //                        if (al->address->modifiedBy)
                        //                            ormap(bits, al->address->modifiedBy);
                        al = al->next;
                    }
                    clearbit(bits, termMap[im->offset->sp->i]);
                    im = GetLoadTemp(im);
                    if (im)
                        setbit(bits, termMap[im->offset->sp->i]);
                }
            }
            else if (im->mode == i_direct)
            {
                ALIASNAME* an = LookupMem(im);
                ALIASADDRESS* aa;
                an = LookupAliasName(an, 0);
                aa = LookupAddress(an, 0);
                while (aa->merge)
                    aa = aa->merge;
                if (aa->modifiedBy)
                    ormap(bits, aa->modifiedBy);
                im = GetLoadTemp(im);
                if (im)
                {
                    setbit(bits, termMap[im->offset->sp->i]);
                }
            }
            else if (im->mode == i_immed && !isintconst(im->offset) && !isimaginaryconst(im->offset) &&
                     !iscomplexconst(im->offset) && !isfloatconst(im->offset) && im->offset->type != se_labcon)
            {
                ALIASNAME* an = LookupMem(im);
                ALIASADDRESS* aa;
                aa = LookupAddress(an, 0);
                while (aa->merge)
                    aa = aa->merge;
                if (aa->modifiedBy)
                    ormap(bits, aa->modifiedBy);
                im = im->offset->sp->imvalue;
                if (im)
                {
                    im = GetLoadTemp(im);
                    if (im)
                    {
                        setbit(bits, termMap[im->offset->sp->i]);
                    }
                }
            }
        }
    }
}
static void ScanUIVs(void)
{
    bool done = false;
    ALIASLIST* al = parmList;
    int i;
    ResetProcessed();
    uivBytes = aallocbit(termCount);
    for (auto aab : addresses)
    {
        auto aa = aab.second;
        ALIASADDRESS* aa1 = aa;
        IMODE* im;
        while (aa1->merge)
            aa1 = aa1->merge;
        if (aa1->name->byUIV)
        {
            im = aa1->name->v.uiv->im;
        }
        else
        {
            im = aa1->name->v.name;
        }
        switch (im->offset->type)
        {
            case se_auto:
            case se_global:
            case se_pc:
            case se_threadlocal:
                im = GetLoadTemp(im);
                if (im)
                    setbit(uivBytes, termMap[im->offset->sp->i]);
                if (aa1->modifiedBy)
                    ormap(uivBytes, aa1->modifiedBy);
                break;
            default:
                break;
        }
    }
}
void FinishScanUIVs()
{
    int n = ((termCount) + (BITINTBITS - 1)) / BITINTBITS * sizeof(BITINT);
    ALIASLIST* al = parmList;
    while (al)
    {
        ALIASADDRESS* aa1 = al->address;
        while (aa1->merge)
        {
            aa1 = aa1->merge;
        }
        if (aa1->modifiedBy)
            ormap(uivBytes, aa1->modifiedBy);
        auto lst = aa1->pointsto;
        while (lst)
        {
            ALIASADDRESS* aa2 = lst->address;
            while (aa2->merge)
            {
                aa2 = aa2->merge;
            }
            if (aa2->modifiedBy)
                ormap(uivBytes, aa2->modifiedBy);
            lst = lst->next;
        }
        al = al->next;
    }
}    
static void MakeAliasLists(void)
{
    int i;
    for (i = 0; i < cachedTempCount; i++)
    {
        int n = tempInfo[i]->postSSATemp;
        if (n >= 0 && tempInfo[i]->pointsto)
        {
            tempInfo[n]->pointsto = tempInfo[i]->pointsto;
            tempInfo[i]->pointsto = nullptr;
        }
        tempInfo[i]->modifiedBy = aallocbit(termCount);
    }
    for (i = 0; i < cachedTempCount; i++)
        if (tempInfo[i]->pointsto)
        {
            ALIASLIST* al = tempInfo[i]->pointsto;
            while (al)
            {
                ALIASADDRESS* aa = al->address;
                while (aa->merge)
                    aa = aa->merge;
                if (!aa->modifiedBy)
                    aa->modifiedBy = aallocbit(termCount);
                setbit(aa->modifiedBy, termMap[i]);
                al = al->next;
            }
        }
}
static void ResetProcessed(void) { bitarrayClear(processBits, processCount); }
static void AllocateProcessed(void)
{
    int i;
    processCount = 0;
    for (auto aab : addresses)
    {
        ALIASADDRESS* addr = aab.second;
        ALIASADDRESS* aa = addr;
        while (aa->merge)
            aa = aa->merge;
        aa->processIndex = processCount++;
        addr = addr->next;
    }
    processBits = aallocbit(processCount);
}
static void GatherInds(BITINT* p, int n, ALIASLIST* al)
{
    while (al)
    {
        int k;
        BITINT *r, *s;
        if (!isset(processBits, al->address->processIndex))
        {
            setbit(processBits, al->address->processIndex);
            GatherInds(p, n, al->address->pointsto);
        }
        s = p;
        r = al->address->modifiedBy;
        if (s)
        {
            if (!r)
                r = al->address->modifiedBy = aallocbit(termCount);
            for (k = 0; k < n; k++)
            {
                if (~*r & *s)
                {
                    changed = true;
                    *r |= *s;
                }
                r++, s++;
            }
        }
        al = al->next;
    }
}
static void ScanMem(void)
{
    int i, k;
    int n = (termCount + BITINTBITS - 1) / BITINTBITS;
    do
    {
        changed = false;
        ResetProcessed();
        for (auto aab : addresses)
        {
            ALIASADDRESS* aa = aab.second;
            ALIASADDRESS* aa1 = aa;
            while (aa1->merge)
                aa1 = aa1->merge;
            GatherInds(&aa1->modifiedBy[0], n, aa->pointsto);
        }
    } while (changed);
}
void AliasPass1(void)
{
    AliasInit();
    // when we get here it is expected we are in SSA mode
    Createaddresses();
    GatherAliases(blockArray[0]);
    do
    {
        changed = false;
        GatherAliases(loopArray[loopCount - 1]);
    } while (changed);
}
void AliasPass2(void)
{
    AllocateProcessed();
    MakeAliasLists();
    ScanUIVs();
    ScanMem();
    FinishScanUIVs();
    if (icdFile)
        DumpAliases();
    complementmap(uivBytes);
}
}  // namespace Optimizer