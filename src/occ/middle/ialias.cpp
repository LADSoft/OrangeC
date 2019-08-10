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
#include <malloc.h>
#include <string.h>
#include <limits.h>
#include "compiler.h"
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
extern BLOCK** blockArray;
extern int blockCount;
extern int exitBlock;
extern BITINT bittab[BITINTBITS];
extern TEMP_INFO** tempInfo;
extern int tempCount;
extern QUAD *intermed_head, *intermed_tail;
extern FILE* icdFile;
extern int walkPostorder;
extern SYMBOL* theCurrentFunc;
extern unsigned termCount;
extern int loopCount;
extern LOOP** loopArray;
extern unsigned short *termMap, *termMapUp;

int cachedTempCount;
BITINT* uivBytes;
static bool changed;
static ALIASLIST* parmList;
static int processCount;
BITINT* processBits;
struct UIVHash
{
    struct UIVHash* next;
    ALIASNAME* name;
    int offset;
    ALIASNAME* result;
};

static ALIASADDRESS* addresses[DAGSIZE];
static ALIASNAME* mem[DAGSIZE];
static struct UIVHash* names[DAGSIZE];
static ADDRBYNAME* addrNames[DAGSIZE];
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
    memset(addresses, 0, sizeof(addresses));
    memset(names, 0, sizeof(names));
    memset(mem, 0, sizeof(mem));
    memset(addrNames, 0, sizeof(addrNames));
    parmList = nullptr;
    uivBytes = nullptr;
    cachedTempCount = tempCount;
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
    int i;
    oprintf(icdFile, "Alias Dump:\n");
    for (i = 0; i < DAGSIZE; i++)
    {
        ALIASADDRESS* aa = addresses[i];
        while (aa)
        {
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
            aa = aa->next;
        }
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
    ALIASNAME** p;
    int hash;
    switch (im->offset->type)
    {
        case en_global:
        case en_pc:
        case en_auto:
        case en_threadlocal:
            if (im->offset->v.sp->imvalue)
                im = im->offset->v.sp->imvalue;
            break;
        default:
            break;
    }
    hash = dhash((UBYTE*)&im, sizeof(im));
    p = &mem[hash];
    while (*p)
    {
        if (((*p)->byUIV == false && (*p)->v.name == im) ||
            ((*p)->byUIV == true && (*p)->v.uiv->im == im && (*p)->v.uiv->offset == nullptr))
        {
            return *p;
        }
        p = &(*p)->next;
    }
    *p = (ALIASNAME *) aAlloc(sizeof(ALIASNAME));
    (*p)->v.name = im;
    switch (im->offset->type)
    {
        case en_auto:
        case en_global:
            (*p)->v.uiv = (UIV *)aAlloc(sizeof(UIV));
            (*p)->v.uiv->im = im;
            (*p)->byUIV = true;
            break;
        default:
            break;
    }
    return *p;
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
            ALIASLIST* al = (ALIASLIST *)aAlloc(sizeof(ALIASLIST));
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
            ALIASLIST* al = (ALIASLIST *)aAlloc(sizeof(ALIASLIST));
            al->address = src->address;
            *q = al;
            changed = true;
        }
        src = src->next;
    }
}
static ALIASNAME* LookupAliasName(ALIASNAME* name, int offset)
{
    int str[(sizeof(ALIASNAME*) + sizeof(int)) / sizeof(int)];
    int hash;
    ALIASNAME* result;
    struct UIVHash** uivs;
    str[0] = offset;
    *((ALIASNAME**)(str + 1)) = name;
    hash = dhash((UBYTE*)str, sizeof(str));
    uivs = &names[hash];
    while (*uivs)
    {
        if ((*uivs)->name == name && (*uivs)->offset == offset)
            return (*uivs)->result;
        uivs = &(*uivs)->next;
    }
    *uivs = (UIVHash *)aAlloc(sizeof(struct UIVHash));
    (*uivs)->name = name;
    (*uivs)->offset = offset;
    result = (ALIASNAME *)aAlloc(sizeof(ALIASNAME));
    result->byUIV = true;
    result->v.uiv = (UIV *)aAlloc(sizeof(UIV));
    if (name->byUIV)
    {
        *result->v.uiv = *name->v.uiv;
        result->v.uiv->alias = nullptr;
    }
    else
    {
        result->v.uiv->im = name->v.name;
    }
    result->v.uiv->offset = (UIVOffset *)aAlloc(sizeof(struct UIVOffset));
    result->v.uiv->offset->offset = offset;
    if (name->byUIV)
        result->v.uiv->offset->next = name->v.uiv->offset;
    (*uivs)->result = result;
    return result;
}
static ALIASNAME* GetAliasName(ALIASNAME* name, int offset)
{
    int str[(sizeof(ALIASNAME*) + sizeof(int)) / sizeof(int)];
    int hash;
    ALIASNAME* result;
    struct UIVHash** uivs;
    str[0] = offset;
    *((ALIASNAME**)(str + 1)) = name;
    hash = dhash((UBYTE*)str, sizeof(str));
    uivs = &names[hash];
    while (*uivs)
    {
        if ((*uivs)->name == name && (*uivs)->offset == offset)
            return (*uivs)->result;
        uivs = &(*uivs)->next;
    }
    return nullptr;
}
static ALIASADDRESS* LookupAddress(ALIASNAME* name, int offset)
{
    int str[(sizeof(ALIASNAME*) + sizeof(int)) / sizeof(int)];
    int hash;
    ALIASADDRESS *addr, **search;
    IMODE* im;
    LIST* li;
    str[0] = offset;
    *((ALIASNAME**)(str + 1)) = name;
    hash = dhash((UBYTE*)str, sizeof(str));
    search = &addresses[hash];
    while (*search)
    {
        if ((*search)->name == name && (*search)->offset == offset)
            return (*search);
        search = &(*search)->next;
    }
    addr = (*search) = (ALIASADDRESS *)aAlloc(sizeof(ALIASADDRESS));
    addr->name = name;
    addr->offset = offset;
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
        case en_auto:
            //			if (im->offset->v.sp->storage_class != sc_parameter)
            break;
        case en_global:
        {
            ALIASLIST* l = (ALIASLIST *)aAlloc(sizeof(ALIASLIST));
            l->address = addr;
            AliasUnion(&parmList, l);
        }
        break;
        default:
            break;
    }
    li = (LIST *)(LIST *)aAlloc(sizeof(LIST));
    li->data = addr;
    li->next = name->addresses;
    name->addresses = li;
    hash = dhash((UBYTE *)&name, sizeof(name));
    ADDRBYNAME* q = addrNames[hash];
    while (q)
    {
        if (q->name == name)
            break;
        q = q->next;
    }
    if (!q)
    {
        q = (ADDRBYNAME*)aAlloc(sizeof(ADDRBYNAME));
        q->next = addrNames[hash];
        addrNames[hash] = q;
        q->name = name;
    }
    ALIASLIST* ali = (ALIASLIST *)aAlloc(sizeof(ALIASLIST));
    ali->address = addr;
    ali->next = q->addresses;
    q->addresses = ali;
    return addr;
}
static ALIASADDRESS* GetAddress(ALIASNAME* name, int offset)
{
    int str[(sizeof(ALIASNAME*) + sizeof(int)) / sizeof(int)];
    int hash;
    ALIASADDRESS *addr, **search;
    IMODE* im;
    LIST* li;
    str[0] = offset;
    *((ALIASNAME**)(str + 1)) = name;
    hash = dhash((UBYTE*)str, sizeof(str));
    search = &addresses[hash];
    while (*search)
    {
        if ((*search)->name == name && (*search)->offset == offset)
            return (*search);
        search = &(*search)->next;
    }
    return nullptr;
}
static void CreateMem(IMODE* im)
{
    ALIASNAME* p;
    if (im->offset->type != en_pc && im->offset->type != en_sub)
    {
        if (im->mode == i_immed)
        {
            if (!im->offset->v.sp->imvalue)
            {
                // make one in the case of global addresses that aren't used
                // directly
                IMODE* ap2 = (IMODE*)(IMODE *)Alloc(sizeof(IMODE));
                ap2->offset = im->offset;
                ap2->mode = i_direct;
                ap2->size = ISZ_ADDR;
                im->offset->v.sp->imvalue = ap2;
            }
            p = LookupMem(im->offset->v.sp->imvalue);
        }
        else
        {
            ALIASADDRESS* aa;
            p = LookupMem(im);
            p = LookupAliasName(p, 0);
        }
        if (im->size == ISZ_ADDR || im->offset->type == en_global)
        {
            ALIASADDRESS* aa;
            aa = LookupAddress(p, 0);
            if (!aa->pointsto)
            {
                ALIASNAME* an = LookupAliasName(p, 0);
                aa->pointsto = (_aliaslist *)(ALIASLIST *)aAlloc(sizeof(ALIASLIST));
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
                        (!isarithmeticconst(head->dc.left->offset) && head->dc.left->offset->type != en_labcon &&
                         head->dc.left->offset->type != en_add))
                        CreateMem(head->dc.left);
                }
                if (head->dc.right && !(head->temps & TEMP_RIGHT))
                {
                    // fixme...
                    if (head->dc.right->mode == i_direct ||
                        (!isarithmeticconst(head->dc.right->offset) && head->dc.right->offset->type != en_labcon &&
                         head->dc.right->offset->type != en_add))
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
    if (tempInfo[head->dc.v.phi->T0]->enode->v.sp->imvalue->size == ISZ_ADDR)
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
            ALIASLIST* src = tempInfo[head->dc.left->offset->v.sp->value.i]->pointsto;
            addr = tempInfo[head->ans->offset->v.sp->value.i]->pointsto;
            while (addr)
            {
                AliasUnion(&addr->address->pointsto, src);
                addr = addr->next;
            }
        }
        else if (head->dc.left->mode == i_immed && head->dc.left->size == ISZ_ADDR && head->dc.left->offset->type != en_labcon)
        {
            // ind, immed
            ALIASLIST* addr;
            ALIASNAME* an = LookupMem(head->ans->offset->v.sp->imvalue);
            ALIASADDRESS* aa;
            if (head->ans->mode == i_direct)
                an = LookupAliasName(an, 0);
            aa = LookupAddress(an, 0);
            addr = tempInfo[head->ans->offset->v.sp->value.i]->pointsto;
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
        ALIASLIST* addr = tempInfo[head->dc.left->offset->v.sp->value.i]->pointsto;
        bool xchanged = changed;
        while (addr)
        {
            if (addr->address->name->byUIV)
            {
                if (!IntersectsUIV(addr->address->pointsto))
                {
                    ALIASNAME* an = LookupAliasName(addr->address->name, addr->address->offset);
                    ALIASADDRESS* aa = LookupAddress(an, 0);
                    ALIASLIST* al = (ALIASLIST *)aAlloc(sizeof(ALIASLIST));
                    al->address = aa;
                    AliasUnion(&addr->address->pointsto, al);
                }
            }
            AliasUnion(&result, addr->address->pointsto);
            addr = addr->next;
        }
        changed = xchanged;
        tempInfo[head->ans->offset->v.sp->value.i]->pointsto = result;
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
                AliasUnion(&aa->pointsto, tempInfo[head->dc.left->offset->v.sp->value.i]->pointsto);
            }
            else if (head->dc.left->mode == i_immed && head->dc.left->size == ISZ_ADDR && head->dc.left->offset->type != en_labcon)
            {
                // mem, immed
                ALIASNAME* an2 = LookupMem(head->dc.left);
                ALIASADDRESS* aa2 = LookupAddress(an2, 0);
                if (head->ans->offset->v.sp->imvalue)
                {
                    ALIASNAME* an = LookupMem(head->ans->offset->v.sp->imvalue);
                    ALIASADDRESS* aa;
                    ALIASLIST* al = (ALIASLIST *)aAlloc(sizeof(ALIASLIST));
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
            if (head->dc.left->mode == i_immed && head->dc.left->size == ISZ_ADDR && head->dc.left->offset->type != en_labcon &&
                !isintconst(head->dc.left->offset))
            {
                // temp, immed
                bool xchanged = changed;
                ALIASNAME* an = LookupMem(head->dc.left);
                ALIASADDRESS* aa = LookupAddress(an, 0);
                ALIASLIST* al = (ALIASLIST*)(ALIASLIST *)aAlloc(sizeof(ALIASLIST));
                al->address = aa;
                tempInfo[head->ans->offset->v.sp->value.i]->pointsto = nullptr;
                AliasUnion(&tempInfo[head->ans->offset->v.sp->value.i]->pointsto, al);
                changed = xchanged;
            }
            else if (head->dc.left->retval)
            {
                AliasUnion(&tempInfo[head->ans->offset->v.sp->value.i]->pointsto, parmList);
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
                AliasUnion(&tempInfo[head->ans->offset->v.sp->value.i]->pointsto, aa->pointsto);
                changed = xchanged;
            }
            else if (head->temps & TEMP_LEFT)
            {
                // temp, temp
                AliasUnion(&tempInfo[head->ans->offset->v.sp->value.i]->pointsto,
                           tempInfo[head->dc.left->offset->v.sp->value.i]->pointsto);
            }
        }
    }
}
static int InferOffset(IMODE* im)
{
    QUAD* q = tempInfo[im->offset->v.sp->value.i]->instructionDefines;
    if (q)
    {
        if (q->dc.opcode == i_add)
        {
            if ((q->temps & TEMP_LEFT) && q->dc.left->mode == i_direct)
            {
                if (q->dc.right->mode == i_immed && isintconst(q->dc.right->offset))
                    return q->dc.right->offset->v.i;
            }
            else if ((q->temps & TEMP_RIGHT) && q->dc.right->mode == i_direct)
            {
                if (q->dc.left->mode == i_immed && isintconst(q->dc.left->offset))
                    return q->dc.left->offset->v.i;
            }
        }
        else if (q->dc.opcode == i_sub)
        {
            if ((q->temps & TEMP_LEFT) && q->dc.left->mode == i_direct)
            {
                if (q->dc.right->mode == i_immed && isintconst(q->dc.right->offset))
                    return -q->dc.right->offset->v.i;
            }
        }
        else if (q->dc.opcode == i_lsl)
        {
            if (q->dc.right->mode == i_immed && isintconst(q->dc.right->offset))
                if (q->temps & TEMP_LEFT)
                    return InferOffset(q->dc.left) << q->dc.right->offset->v.i;
        }
        else if (q->dc.opcode == i_mul)
        {
            if (q->dc.left->mode == i_immed && isintconst(q->dc.left->offset))
                if (q->temps & TEMP_RIGHT)
                    return InferOffset(q->dc.right) * q->dc.left->offset->v.i;
            if (q->dc.right->mode == i_immed && isintconst(q->dc.right->offset))
                if (q->temps & TEMP_LEFT)
                    return InferOffset(q->dc.left) * q->dc.right->offset->v.i;
        }
    }
    return 0;
}
static int InferStride(IMODE* im)
{
    QUAD* q = tempInfo[im->offset->v.sp->value.i]->instructionDefines;
    if (q)
    {
        if (q->dc.opcode == i_lsl)
        {
            if ((q->temps & TEMP_LEFT) && q->dc.left->mode == i_direct)
            {
                if (q->dc.right->mode == i_immed && isintconst(q->dc.right->offset))
                    return 1 << q->dc.right->offset->v.i;
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
            if (one->mode == i_direct && one->offset->type == en_tempref)
            {
                if (two->mode == i_immed && isintconst(two->offset))
                {
                    if (q->dc.opcode == i_add || q->dc.opcode == i_sub)
                        return InferStride(one);
                    return two->offset->v.i;
                }
            }
        }
    }
    return 1;
}
static void SetStride(ALIASADDRESS* addr, int stride)
{
    int hash = dhash((UBYTE *)&addr->name, sizeof(addr->name));
    ADDRBYNAME* q = addrNames[hash];
    while (q)
    {
        if (q->name == addr->name)
            break;
        q = q->next;
    }
    if (q)
    {
        ALIASLIST* addresses = q->addresses;
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
                ALIASLIST* al = (ALIASLIST *)aAlloc(sizeof(ALIASLIST));
                al->address = addr;
                AliasUnion(&result, al);
                SetStride(pointsto->address, l);
                pointsto = pointsto->next;
            }
            changed = xchanged;
            AliasUnion(&tempInfo[ans->offset->v.sp->value.i]->pointsto, result);
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
                    ALIASLIST* scan = tempInfo[head->dc.right->offset->v.sp->value.i]->pointsto;
                    ALIASLIST* result = nullptr;
                    bool xchanged = changed;
                    while (scan)
                    {
                        ALIASADDRESS* addr = LookupAddress(scan->address->name, scan->address->offset + head->dc.left->offset->v.i);
                        ALIASLIST* al = (ALIASLIST *)aAlloc(sizeof(ALIASLIST));
                        al->address = addr;
                        AliasUnion(&result, al);
                        scan = scan->next;
                    }
                    changed = xchanged;
                    AliasUnion(&tempInfo[head->ans->offset->v.sp->value.i]->pointsto, result);
                }
                else
                {
                    // p + R
                    if (head->dc.left->offset->type != en_labcon)  // needed for exports
                    {
                        ALIASNAME* nm = LookupMem(head->dc.left->offset->v.sp->imvalue);
                        ALIASADDRESS* aa = LookupAddress(nm, 0);
                        ALIASLIST* al = (ALIASLIST *)aAlloc(sizeof(ALIASLIST));
                        al->address = aa;
                        Infer(head->ans, head->dc.right, al);
                    }
                }
            }
            else if (head->dc.right->mode == i_immed)
            {
                if (!isintconst(head->dc.left->offset) && head->dc.left->offset->type != en_labcon)
                {
                    // p + C
                    ALIASNAME* nm = LookupMem(head->dc.left->offset->v.sp->imvalue);
                    ALIASADDRESS* aa = LookupAddress(nm, head->dc.right->offset->v.i);
                    ALIASLIST* al = (ALIASLIST *)aAlloc(sizeof(ALIASLIST));
                    al->address = aa;
                    AliasUnion(&tempInfo[head->ans->offset->v.sp->value.i]->pointsto, al);
                }
                else if (!isintconst(head->dc.right->offset) && head->dc.right->offset->type != en_labcon)
                {
                    // C + p
                    ALIASNAME* nm = LookupMem(head->dc.right->offset->v.sp->imvalue);
                    ALIASADDRESS* aa = LookupAddress(nm, head->dc.left->offset->v.i);
                    ALIASLIST* al = (ALIASLIST *)aAlloc(sizeof(ALIASLIST));
                    al->address = aa;
                    AliasUnion(&tempInfo[head->ans->offset->v.sp->value.i]->pointsto, al);
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
                    int c = head->dc.opcode == i_add ? head->dc.right->offset->v.i : -head->dc.right->offset->v.i;
                    ALIASLIST* scan = tempInfo[head->dc.left->offset->v.sp->value.i]->pointsto;
                    ALIASLIST* result = nullptr;
                    bool xchanged = changed;
                    while (scan)
                    {
                        ALIASADDRESS* addr = LookupAddress(scan->address->name, scan->address->offset + c);
                        ALIASLIST* al = (ALIASLIST *)aAlloc(sizeof(ALIASLIST));
                        al->address = addr;
                        AliasUnion(&result, al);
                        scan = scan->next;
                    }
                    changed = xchanged;
                    AliasUnion(&tempInfo[head->ans->offset->v.sp->value.i]->pointsto, result);
                }
                else
                {
                    // R + p
                    if (head->dc.right->offset->type != en_labcon)  // needed for exports
                    {
                        ALIASNAME* nm = LookupMem(head->dc.right->offset->v.sp->imvalue);
                        ALIASADDRESS* aa = LookupAddress(nm, 0);
                        ALIASLIST* al = (ALIASLIST *)aAlloc(sizeof(ALIASLIST));
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
                src = tempInfo[one->offset->v.sp->value.i]->pointsto;
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
        ALIASLIST* src = tempInfo[head->dc.right->offset->v.sp->value.i]->pointsto;
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
            base = &tempInfo[head->dc.left->offset->v.sp->value.i]->pointsto;
        }
        else if (!isintconst(head->dc.left->offset))
        {
            ALIASNAME* an;
            ALIASADDRESS* aa;
            switch (head->dc.left->offset->type)
            {
                case en_labcon:
                case en_global:
                case en_pc:
                case en_threadlocal:
                    return;
                default:
                    break;
            }
            an = LookupMem(head->dc.left->offset->v.sp->imvalue);
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
                        ALIASLIST* al = (ALIASLIST *)aAlloc(sizeof(ALIASLIST));
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
    int i, n = ans->offset->v.i;
    if (left->offset->type == en_tempref && left->mode == i_direct)
    {
        src = tempInfo[left->offset->v.sp->value.i]->pointsto;
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
        setbit(bits, termMap[left->offset->v.sp->value.i]);
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
                ALIASLIST* al = tempInfo[tail->dc.left->offset->v.sp->value.i]->pointsto;
                ResetProcessed();
                scanDepends(parms, al);
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
            if (im->offset->type == en_tempref)
            {
                ormap(bits, tempInfo[im->offset->v.sp->value.i]->modifiedBy);
                if (im->mode == i_direct)
                {
                    im = LookupLoadTemp(im, im);
                }
                setbit(bits, termMap[im->offset->v.sp->value.i]);
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
                    setbit(bits, termMap[im->offset->v.sp->value.i]);
                }
            }
            else if (im->mode == i_immed && !isintconst(im->offset) && !isimaginaryconst(im->offset) && !iscomplexconst(im->offset) && !isfloatconst(im->offset) &&
                     im->offset->type != en_labcon)
            {
                ALIASNAME* an = LookupMem(im);
                ALIASADDRESS* aa;
                aa = LookupAddress(an, 0);
                while (aa->merge)
                    aa = aa->merge;
                if (aa->modifiedBy)
                    ormap(bits, aa->modifiedBy);
                im = im->offset->v.sp->imvalue;
                if (im)
                {
                    im = GetLoadTemp(im);
                    if (im)
                    {
                        setbit(bits, termMap[im->offset->v.sp->value.i]);
                    }
                }
            }
        }
        else
        {
            setbit(bits, termMap[im->offset->v.sp->value.i]);
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
    for (i = 0; i < DAGSIZE; i++)
    {
        ALIASADDRESS* aa = addresses[i];
        while (aa)
        {
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
                case en_auto:
                case en_global:
                case en_pc:
                case en_threadlocal:
                    im = GetLoadTemp(im);
                    if (im)
                        setbit(uivBytes, termMap[im->offset->v.sp->value.i]);
                    break;
                default:
                    break;
            }
            aa = aa->next;
        }
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
    for (i = 0; i < DAGSIZE; i++)
    {
        ALIASADDRESS* addr = addresses[i];
        while (addr)
        {
            ALIASADDRESS* aa = addr;
            while (aa->merge)
                aa = aa->merge;
            aa->processIndex = processCount++;
            addr = addr->next;
        }
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
        for (i = 0; i < DAGSIZE; i++)
        {
            ALIASADDRESS* aa = addresses[i];
            while (aa)
            {
                ALIASADDRESS* aa1 = aa;
                while (aa1->merge)
                    aa1 = aa1->merge;
                GatherInds(&aa1->modifiedBy[0], n, aa->pointsto);
                aa = aa->next;
            }
        }
    } while (changed);
}
void AliasPass1(void)
{
    AliasInit();
    // when we get here it is expected we are in SSA mode
    Createaddresses();
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
    //    if (icdFile)
    //        DumpAliases();
    complementmap(uivBytes);
}
