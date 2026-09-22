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
#include <iterator>
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

typedef struct _aliasName
{
    bool byUIV;
    IMODE* im;
    std::list<int> offset;
} ALIASNAME;

typedef struct _aliasAddress
{
    struct _aliasAddress* merge;
    ALIASNAME* name;
    int offset;
} ALIASADDRESS;

typedef std::set<ALIASADDRESS*> ALIASLIST;

#pragma pack(1)
struct ptrint
{
    void* ptr;
    int intval;
};
#pragma pack()

static int cachedTempCount;

static bool changed;
static ALIASLIST parmList;

static std::unordered_map<ptrint, ALIASADDRESS*, OrangeC::Utils::fnv1a32_type<ptrint>, OrangeC::Utils::type_eql<ptrint>> addresses;
static std::unordered_map<IMODE**, std::list<ALIASNAME*>, OrangeC::Utils::fnv1a32_binary<sizeof(IMODE*)>,
                          OrangeC::Utils::bin_eql<sizeof(IMODE*)>>
    mem;
static std::unordered_map<ptrint, ALIASNAME, OrangeC::Utils::fnv1a32_type<ptrint>, OrangeC::Utils::type_eql<ptrint>> names;

static std::multimap<ALIASNAME*, ALIASNAME*> nameToChildren;

static std::multimap<ALIASNAME*, ALIASADDRESS*> nameToAddress;
static std::multimap<IMODE*, IMODE*> pointsFrom;
static std::vector<ALIASLIST> tempPointsTo;
static std::unordered_map<ALIASADDRESS*, ALIASLIST> addressToAlias;
static std::multimap<ALIASADDRESS*, IMODE*> addressToInd;
static std::unordered_map<ALIASNAME*, ALIASNAME*> nameToParent;
void AliasInit(void)
{
    tempPointsTo.clear();
    tempPointsTo.resize(tempCount);
    addresses.clear();
    names.clear();
    mem.clear();
    nameToChildren.clear();
    nameToParent.clear();
    nameToAddress.clear();
    addressToAlias.clear();
    addressToInd.clear();
    pointsFrom.clear();
    parmList.clear();
    cachedTempCount = tempCount;
    changed = false;
}
void AliasRundown(void)
{
    aFree();
    tempPointsTo.clear();
    addresses.clear();
    names.clear();
    mem.clear();
    nameToChildren.clear();
    nameToParent.clear();
    nameToAddress.clear();
    addressToAlias.clear();
    addressToInd.clear();
    pointsFrom.clear();
    parmList.clear();
}
static void PrintOffs(std::list<int>& data)
{
    for (auto offset : data)
    {
        oprintf(icdFile, "@%d", offset);
    }
}
static void PrintAddress(ALIASNAME* name, int offs)
{
    oprintf(icdFile, "(");
    if (!name)
    {
        oprintf(icdFile, "stub");
    }
    else
    {
        putamode(nullptr, name->im);
        PrintOffs(name->offset);
    }
    oprintf(icdFile, ",%d)", offs);
}
static void DumpAliases(void)
{
    oprintf(icdFile, "\nfunction: %s\n", currentFunction->outputName);
    int i;
    oprintf(icdFile, "Alias Dump:\n");
    for (auto aab : addresses)
    {
        ALIASADDRESS* aa = aab.second;
        ALIASADDRESS* aa1 = aa;
        while (aa1->merge)
            aa1 = aa1->merge;
        PrintAddress(aa->name, aa->offset);
        oprintf(icdFile, ": ");
        for (auto address : addressToAlias[aa1])
        {
            PrintAddress(address->name, address->offset);
            oprintf(icdFile, " ");
        }
        oprintf(icdFile, "\n");
    }
    for (i = 0; i < cachedTempCount; i++)
    {
        if (tempPointsTo[i].size())
        {
            oprintf(icdFile, "T%d:", i);
            for (auto address : tempPointsTo[i])
            {
                PrintAddress(address->name, address->offset);
                oprintf(icdFile, " ");
            }
            oprintf(icdFile, "\n");
        }
    }
    ALIASNAME* current = nullptr;
    for (auto&& pair : nameToChildren)
    {
        if (current != pair.first)
        {
            oprintf(icdFile, "\n");
            putamode(nullptr, pair.first->im);
            PrintOffs(pair.first->offset);
            oprintf(icdFile, ": ");
            current = pair.first;
        }
        putamode(nullptr, pair.second->im);
        PrintOffs(pair.second->offset);
    }
    current = nullptr;
    for (auto&& pair : nameToAddress)
    {
        if (current != pair.first)
        {
            oprintf(icdFile, "\n");
            putamode(nullptr, pair.first->im);
            PrintOffs(pair.first->offset);
            oprintf(icdFile, ": ");
            current = pair.first;
        }
        PrintAddress(pair.second->name, pair.second->offset);
    }

    ALIASADDRESS* current1 = nullptr;
    for (auto&& pair : addressToInd)
    {
        if (current1 != pair.first)
        {
            oprintf(icdFile, "\n");
            PrintAddress(pair.first->name, pair.first->offset);
            oprintf(icdFile, ": ");
            current1 = pair.first;
        }
        putamode(nullptr, pair.second);
    }
    {
        oprintf(icdFile, "\nUIV: ");
        for (auto address : parmList)
        {
            while (address->merge)
                address = address->merge;
            PrintAddress(address->name, address->offset);
            oprintf(icdFile, " ");
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
            if ((p->byUIV == false && p->im == im) || (p->byUIV == true && p->im == im && p->offset.size() == 0))
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
    p->im = im;
    switch (im->offset->type)
    {
        case se_auto:
        case se_global:
            p->im = im;
            p->byUIV = true;
            break;
        default:
            break;
    }
    it->second.push_back(p);
    return p;
}
static void AliasUnion(ALIASLIST& dest, ALIASLIST& src)
{
    std::unordered_set<IMODE*> matches;
    for (auto&& d : dest)
        matches.insert(d->name->im);
    for (auto s : src)
    {
        if (matches.find(s->name->im) == matches.end())
        {
            matches.insert(s->name->im);
            dest.insert(s);
            changed = true;
        }
    }
}
static void AliasUnionParm(ALIASLIST& dest, ALIASLIST& src)
{
    std::unordered_set<ALIASNAME*> matches;
    for (auto&& d : dest)
        matches.insert(d->name);
    for (auto s : src)
    {
        if (matches.find(s->name) == matches.end())
        {
            matches.insert(s->name);
            dest.insert(s);
            changed = true;
        }
    }
}
static ALIASNAME* LookupAliasName(ALIASNAME* name, int offset)
{
    ptrint str;
    str.ptr = name;
    str.intval = offset;
    auto it = names.find(str);
    if (it != names.end())
        return &it->second;

    ALIASNAME result;

    result.byUIV = true;
    result.im = name->im;
    if (name->byUIV)
    {
        result.offset = name->offset;
    }
    result.offset.push_back(offset);
    names[str] = std::move(result);
    auto rv = &names[str];
    nameToChildren.insert(std::pair(name, rv));
    nameToParent[rv] = name;
    return rv;
}
static ALIASNAME* GetAliasName(ALIASNAME* name, int offset)
{
    ptrint str;
    str.ptr = name;
    str.intval = offset;
    auto it = names.find(str);
    if (it != names.end())
        return &it->second;
    return nullptr;
}
static ALIASADDRESS* LookupAddress(ALIASNAME* name, int offset)
{
    ptrint str;
    str.ptr = name;
    str.intval = offset;
    IMODE* im;
    LIST* li;
    auto it = addresses.find(str);
    if (it != addresses.end())
        return it->second;
    ALIASADDRESS* addr;
    addr = aAllocate<ALIASADDRESS>();
    addr->name = name;
    addr->offset = offset;
    addresses[str] = addr;
    im = addr->name->im;
    switch (im->offset->type)
    {
        case se_auto:
            //			if (im->offset->sp->storage_class != scc_parameter)
            break;
        case se_global: {
            ALIASLIST l = {addr};
            AliasUnion(parmList, l);
        }
        break;
        default:
            break;
    }

    nameToAddress.insert(std::pair(name, addr));
    return addr;
}
static ALIASADDRESS* GetAddress(ALIASNAME* name, int offset)
{
    ptrint str;
    str.ptr = name;
    str.intval = offset;
    auto it = addresses.find(str);
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
            if (addressToAlias[aa].size() == 0)
            {
                ALIASNAME* an = LookupAliasName(p, 0);
                addressToAlias[aa].clear();
                addressToAlias[aa].insert(LookupAddress(an, 0));
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
static bool IntersectsUIV(ALIASLIST& al)
{
    for (auto address : al)
    {
        if (address->name->byUIV)
            return true;
    }
    return false;
}
static void HandlePhi(QUAD* head)
{
    if (tempInfo[head->dc.v.phi->T0]->enode->sp->imvalue->size == ISZ_ADDR)
    {
        struct _phiblock* pb = head->dc.v.phi->temps;
        ALIASLIST l;
        bool xchanged = changed;
        while (pb)
        {
            AliasUnion(l, tempPointsTo[pb->Tn]);
            pb = pb->next;
        }
        changed = xchanged;
        tempPointsTo[head->dc.v.phi->T0] = std::move(l);
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
            for (auto addr : tempPointsTo[head->ans->offset->sp->i])
            {
                AliasUnion(addressToAlias[addr], tempPointsTo[head->dc.left->offset->sp->i]);
            }
        }
        else if (head->dc.left->mode == i_immed && head->dc.left->size == ISZ_ADDR && head->dc.left->offset->type != se_labcon)
        {
            // ind, immed
            ALIASNAME* an = LookupMem(head->ans->offset->sp->imvalue);
            ALIASADDRESS* aa;
            if (head->ans->mode == i_direct)
                an = LookupAliasName(an, 0);
            aa = LookupAddress(an, 0);
            for (auto addr : tempPointsTo[head->ans->offset->sp->i])
            {
                AliasUnion(addressToAlias[addr], addressToAlias[aa]);
            }
        }
    }
    else if (head->dc.left->mode == i_ind && (head->temps & TEMP_ANS))
    {
        // temp, ind
        ALIASLIST result;
        bool xchanged = changed;
        for (auto addr : tempPointsTo[head->dc.left->offset->sp->i])
        {
            if (addr->name->byUIV)
            {
                if (!IntersectsUIV(addressToAlias[addr]))
                {
                    ALIASNAME* an = LookupAliasName(addr->name, addr->offset);
                    ALIASADDRESS* aa = LookupAddress(an, 0);
                    ALIASLIST al1 = {aa};
                    AliasUnion(addressToAlias[addr], al1);
                }
            }
            AliasUnion(result, addressToAlias[addr]);
        }
        AliasUnion(tempPointsTo[head->ans->offset->sp->i], result);
        changed = xchanged;
    }
    else if (head->ans->size == ISZ_ADDR)
    {
        if (!(head->temps & TEMP_ANS) && !head->ans->retval)
        {
            if (head->temps & TEMP_LEFT)
            {
                // mem, temp
                ALIASNAME* an = LookupMem(head->ans);
                ALIASADDRESS* aa;
                an = LookupAliasName(an, 0);
                aa = LookupAddress(an, 0);
                AliasUnion(addressToAlias[aa], tempPointsTo[head->dc.left->offset->sp->i]);
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
                    ALIASLIST al = {aa2};
                    if (head->ans->mode == i_direct)
                        an = LookupAliasName(an, 0);
                    aa = LookupAddress(an, 0);
                    AliasUnion(addressToAlias[aa], al);
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
                ALIASLIST al = {aa};
                tempPointsTo[head->ans->offset->sp->i].clear();
                AliasUnion(tempPointsTo[head->ans->offset->sp->i], al);
                changed = xchanged;
            }
            else if (head->dc.left->retval)
            {
                AliasUnion(tempPointsTo[head->ans->offset->sp->i], parmList);
            }
            else if (!(head->temps & TEMP_LEFT) && head->dc.left->mode == i_direct)
            {
                // temp, mem
                ALIASNAME* an = LookupMem(head->dc.left);
                ALIASADDRESS* aa;
                bool xchanged = changed;
                an = LookupAliasName(an, 0);
                aa = LookupAddress(an, 0);
                AliasUnion(tempPointsTo[head->ans->offset->sp->i], addressToAlias[aa]);
                changed = xchanged;
            }
            else if (head->temps & TEMP_LEFT)
            {
                // temp, temp
                AliasUnion(tempPointsTo[head->ans->offset->sp->i], tempPointsTo[head->dc.left->offset->sp->i]);
            }
        }
    }
    else if ((head->temps & TEMP_ANS) && head->ans->mode == i_direct && head->dc.left->mode == i_direct &&
             head->dc.left->offset->type == se_global)
    {
        // mem, temp
        ALIASNAME* an = LookupMem(head->dc.left);
        ALIASADDRESS* aa;
        an = LookupAliasName(an, 0);
        aa = LookupAddress(an, 0);
        ALIASLIST al = {aa};
        AliasUnion(tempPointsTo[head->ans->offset->sp->i], al);
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
    auto range = nameToAddress.equal_range(addr->name);
    for (auto it = range.first; it != range.second; ++it)
    {
        ALIASADDRESS* scan = it->second;
        if (addr != scan && addr->name == scan->name)
        {
            if (addr->offset < scan->offset)
            {
                int o2 = addr->offset + (scan->offset - addr->offset) % stride;
                if (addr->offset == o2)
                {
                    AliasUnion(addressToAlias[addr], addressToAlias[scan]);
                    scan->merge = addr;
                }
                else
                {
                    ALIASADDRESS* sc2 = LookupAddress(addr->name, o2);
                    if (sc2 && sc2 != scan)
                    {
                        AliasUnion(addressToAlias[sc2], addressToAlias[scan]);
                        scan->merge = sc2;
                    }
                }
            }
        }
    }
}
static void Infer(IMODE* ans, IMODE* reg, ALIASLIST& pointsto)
{
    if (pointsto.size())
    {
        ALIASLIST result;
        int c = InferOffset(reg);
        int l = InferStride(reg);
        if (l)
        {
            bool xchanged = changed;
            for (auto address : pointsto)
            {
                ALIASADDRESS* addr = LookupAddress(address->name, address->offset + c);
                ALIASLIST al = {addr};
                AliasUnion(result, al);
                SetStride(address, l);
            }
            changed = xchanged;
            AliasUnion(tempPointsTo[ans->offset->sp->i], result);
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
                    ALIASLIST result;
                    bool xchanged = changed;
                    for (auto scan : tempPointsTo[head->dc.right->offset->sp->i])
                    {
                        ALIASADDRESS* addr = LookupAddress(scan->name, scan->offset + head->dc.left->offset->i);
                        ALIASLIST al = {addr};
                        AliasUnion(result, al);
                    }
                    changed = xchanged;
                    AliasUnion(tempPointsTo[head->ans->offset->sp->i], result);
                }
                else
                {
                    // p + R
                    if (head->dc.left->offset->type != se_labcon && head->dc.left->offset->type != se_pc)  // needed for exports
                    {
                        ALIASNAME* nm = LookupMem(head->dc.left->offset->sp->imvalue);
                        ALIASADDRESS* aa = LookupAddress(nm, 0);
                        ALIASLIST al = {aa};
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
                    ALIASLIST al = {aa};
                    AliasUnion(tempPointsTo[head->ans->offset->sp->i], al);
                }
                else if (!isintconst(head->dc.right->offset) && head->dc.right->offset->type != se_labcon &&
                         head->dc.right->offset->type != se_pc)
                {
                    // C + p
                    ALIASNAME* nm = LookupMem(head->dc.right->offset->sp->imvalue);
                    ALIASADDRESS* aa = LookupAddress(nm, head->dc.left->offset->i);
                    ALIASLIST al = {aa};
                    AliasUnion(tempPointsTo[head->ans->offset->sp->i], al);
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
                    ALIASLIST result;
                    bool xchanged = changed;
                    for (auto scan : tempPointsTo[head->dc.left->offset->sp->i])
                    {
                        ALIASADDRESS* addr = LookupAddress(scan->name, scan->offset + c);
                        ALIASLIST al = {addr};
                        AliasUnion(result, al);
                    }
                    changed = xchanged;
                    AliasUnion(tempPointsTo[head->ans->offset->sp->i], result);
                }
                else
                {
                    // R + p
                    if (head->dc.right->offset->type != se_labcon && head->dc.right->offset->type != se_pc)  // needed for exports
                    {
                        ALIASNAME* nm = LookupMem(head->dc.right->offset->sp->imvalue);
                        ALIASADDRESS* aa = LookupAddress(nm, 0);
                        ALIASLIST al = {aa};
                        Infer(head->ans, head->dc.left, al);
                    }
                }
            }
        }
        else if ((head->temps & (TEMP_LEFT | TEMP_RIGHT)) == (TEMP_LEFT | TEMP_RIGHT))
        {
            // R+R
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
                Infer(head->ans, two, tempPointsTo[one->offset->sp->i]);
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
        for (auto src : tempPointsTo[head->dc.right->offset->sp->i])
        {
            auto range = nameToAddress.equal_range(src->name);
            for (auto it = range.first; it != range.second; ++it)
            {
                ALIASADDRESS* aa = it->second;
                ALIASADDRESS* aadest = LookupAddress(dest, aa->offset);
                AliasUnion(addressToAlias[aadest], addressToAlias[aa]);
            }
        }
    }
    else if (head->dc.right->mode == i_immed)
    {
        ALIASNAME* src = LookupMem(head->dc.right);
        auto range = nameToAddress.equal_range(src);
        for (auto it = range.first; it != range.second; ++it)
        {
            ALIASADDRESS* aa = it->second;
            ALIASADDRESS* aadest = LookupAddress(dest, aa->offset);
            AliasUnion(addressToAlias[aadest], addressToAlias[aa]);
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
        ALIASLIST result, *base = nullptr, templist;
        if (head->temps & TEMP_LEFT)
        {
            for (auto p : tempPointsTo[head->dc.left->offset->sp->i])
            {
                int offset = 0;
                ALIASNAME* tempname;
                ALIASADDRESS* tempaddr;
                if (auto parent = nameToParent[p->name]; parent != nullptr)
                {
                    tempname = parent;
                    offset = p->name->offset.back();
                }
                else
                {
                    tempname = p->name;
                }
                tempaddr = LookupAddress(tempname, offset);
                templist.insert(tempaddr);
            }
            base = &templist;
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
            base = &addressToAlias[aa];
        }
        if (base)
        {
            AliasUnionParm(parmList, *base);
            for (auto addr : *base)
            {
                if (addr->name->byUIV)
                {
                    if (!IntersectsUIV(addressToAlias[addr]))
                    {
                        ALIASNAME* an = LookupAliasName(addr->name, 0);
                        ALIASADDRESS* aa = LookupAddress(an, 0);
                        ALIASLIST al = {aa};
                        AliasUnion(addressToAlias[addr], al);
                    }
                }
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
    for (auto aab : addresses)
    {
        auto aa = aab.second;
        ALIASADDRESS* aa1 = aa;
        IMODE* im;
        while (aa1->merge)
            aa1 = aa1->merge;
        im = aa1->name->im;
        for (auto addr : addressToAlias[aa1])
        {
            IMODE* imp;
            aa1 = addr;
            while (aa1->merge)
                aa1 = aa1->merge;
            imp = aa1->name->im;
            pointsFrom.insert(std::pair(imp, im));
        }
    }
    for (int i = 0; i < cachedTempCount; i++)
    {
        if (tempPointsTo[i].size())
        {
            auto iml = tempInfo[i]->enode->sp->imind;
            while (iml)
            {
                IMODE* im = iml->im;
                if (im)
                {
                    for (auto addr : tempPointsTo[i])
                    {
                        IMODE* imp;
                        auto aa1 = addr;
                        while (aa1->merge)
                            aa1 = aa1->merge;
                        imp = aa1->name->im;
                        pointsFrom.insert(std::pair(im, imp));
                        addressToInd.insert(std::pair(addr, im));
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
void UIVAddressesInternal(std::function<void(IMODE*)> processor, std::unordered_set<ALIASNAME*>& visitedNames, ALIASNAME* name,
                          int offset)
{
    visitedNames.insert(name);
    if (name->im->size != ISZ_ADDR && name->im->mode == i_direct)
    {
        processor(name->im);
    }
    auto addresses = nameToAddress.equal_range(name);
    for (auto currentAddress = addresses.first; currentAddress != addresses.second; ++currentAddress)
    {
        if (currentAddress->second->offset >= offset)
        {
            auto range = addressToInd.equal_range(currentAddress->second);
            for (auto imp = range.first; imp != range.second; ++imp)
                processor(imp->second);
        }
    }
    auto children = nameToChildren.equal_range(name);
    for (auto child = children.first; child != children.second; ++child)
    {
        if (visitedNames.find(child->second) == visitedNames.end())
        {
            UIVAddressesInternal(processor, visitedNames, child->second, 0);
        }
    }
}
void ProcessUIVAddresses(std::function<void(IMODE*)> processor)
{
    std::unordered_set<ALIASNAME*> visitedNames;
    for (auto address : parmList)
    {
        UIVAddressesInternal(processor, visitedNames, address->name, address->offset);
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
}
}  // namespace Optimizer
