/* Software License Agreement
 *
 *     Copyright(C) 1994-2026 David Lindauer, (LADSoft)
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
#include "igcse.h"
#include "ildata.h"
#include "OptUtils.h"
#include "output.h"
#include "iout.h"
#include "ilocal.h"
#include "memory.h"
#include "ioptutil.h"
#include "optmain.h"
#include "FNV_hash.h"
#include <functional>
#include <algorithm>
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

static std::unordered_map<ptrint*, ALIASADDRESS*, OrangeC::Utils::fnv1a32_binary<sizeof(ptrint)>,
                          OrangeC::Utils::bin_eql<sizeof(ptrint)>>
    addresses;
static std::unordered_map<IMODE**, std::list<ALIASNAME*>, OrangeC::Utils::fnv1a32_binary<sizeof(IMODE*)>,
                          OrangeC::Utils::bin_eql<sizeof(IMODE*)>>
    mem;
static std::unordered_map<ptrint*, UIVHash*, OrangeC::Utils::fnv1a32_binary<sizeof(ptrint)>,
                          OrangeC::Utils::bin_eql<sizeof(ptrint)>>
    names;
static std::unordered_map<ALIASNAME**, ADDRBYNAME*, OrangeC::Utils::fnv1a32_binary<sizeof(ALIASNAME*)>,
                          OrangeC::Utils::bin_eql<sizeof(ALIASNAME*)>>
    addrNames;
static std::multimap<IMODE*, IMODE*> pointsFrom;
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
    pointsFrom.clear();
    parmList = nullptr;
    uivBytes = nullptr;
    cachedTempCount = tempCount;
    processBits = nullptr;
    processCount = 0;
    changed = false;
}
void AliasRundown(void)
{
    addresses.clear();
    names.clear();
    mem.clear();
    addrNames.clear();
    pointsFrom.clear();
}
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
            if ((p->byUIV == false && p->v.name == im) || (p->byUIV == true && p->v.uiv->im == im && p->v.uiv->offset == nullptr))
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
    else if ((head->temps & TEMP_ANS) && head->ans->mode == i_direct && head->dc.left->mode == i_direct &&
             head->dc.left->offset->type == se_global)
    {
        // mem, temp
        ALIASLIST* al;
        ALIASNAME* an = LookupMem(head->dc.left);
        ALIASADDRESS* aa;
        an = LookupAliasName(an, 0);
        aa = LookupAddress(an, 0);
        al = Allocate<ALIASLIST>();
        al->address = aa;
        AliasUnion(&tempInfo[head->ans->offset->sp->i]->pointsto, al);
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
                    if (head->dc.left->offset->type != se_labcon && head->dc.left->offset->type != se_pc)  // needed for exports
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
                if (!isintconst(head->dc.left->offset) && head->dc.left->offset->type != se_labcon &&
                    head->dc.left->offset->type != se_pc)
                {
                    // p + C
                    ALIASNAME* nm = LookupMem(head->dc.left->offset->sp->imvalue);
                    ALIASADDRESS* aa = LookupAddress(nm, head->dc.right->offset->i);
                    ALIASLIST* al = aAllocate<ALIASLIST>();
                    al->address = aa;
                    AliasUnion(&tempInfo[head->ans->offset->sp->i]->pointsto, al);
                }
                else if (!isintconst(head->dc.right->offset) && head->dc.right->offset->type != se_labcon &&
                         head->dc.right->offset->type != se_pc)
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
                    if (head->dc.right->offset->type != se_labcon && head->dc.right->offset->type != se_pc)  // needed for exports
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
        return;
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
static void AliasesOneBlock(Block* b)
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
static void GatherAliases(Block* b)
{
    AliasesOneBlock(b);
    for (auto d = b->dominates; d; d = d->next)
        AliasesOneBlock(d->block);
}
static void GatherAliases(Loop* lp)
{
    bool xchanged = changed;
    do
    {
        LOOPLIST* lt = lp->contains;
        changed = false;
        while (lt)
        {
            lp = lt->loop;
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
static void InitIMModifies()
{
    ALIASLIST* al = parmList;
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
        auto addr = aa1->pointsto;
        while (addr)
        {
            IMODE* imp;
            aa1 = addr->address;
            while (aa1->merge)
                aa1 = aa1->merge;
            if (aa1->name->byUIV)
            {
                imp = aa1->name->v.uiv->im;
            }
            else
            {
                imp = aa1->name->v.name;
            }
            pointsFrom.insert(std::pair(imp, im));
            addr = addr->next;
        }
    }
    for (int i = 0; i < cachedTempCount; i++)
    {
        if (tempInfo[i]->pointsto)
        {
            auto iml = tempInfo[i]->enode->sp->imind;
            while (iml)
            {
                IMODE* im = iml->im;
                if (im)
                {
                    auto addr = tempInfo[i]->pointsto;
                    while (addr)
                    {
                        IMODE* imp;
                        auto aa1 = addr->address;
                        while (aa1->merge)
                            aa1 = aa1->merge;
                        if (aa1->name->byUIV)
                        {
                            imp = aa1->name->v.uiv->im;
                        }
                        else
                        {
                            imp = aa1->name->v.name;
                        }
                        pointsFrom.insert(std::pair(im, imp));
                        addr = addr->next;
                    }
                }
                iml = iml->next;
            }
        }
    }
}
void ProcessIMModifies(IMODE* mem, std::function<void(IMODE*)> processor)
{
    auto bounds = pointsFrom.equal_range(mem);
    for (auto it = bounds.first; it != bounds.second; ++it)
    {
        processor(it->second);
    }
}
void ProcessUIVAddresses(std::function<void(IMODE*)> processor)
{
    ALIASLIST* al = parmList;
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
        processor(im);
    }
}
void AliasPass1(void)
{
    sFree();
    briggsFrees();
    AliasInit();
    // when we get here it is expected we are in SSA mode
    Createaddresses();
    GatherAliases(blockArray[0]);
    do
    {
        changed = false;
        GatherAliases(loopArray[loopCount - 1]);
    } while (changed);
    InitIMModifies();
    icdFile = fopen("hi.txt", "w");
    DumpAliases();
    fclose(icdFile);
    icdFile = nullptr;
}
}  // namespace Optimizer
