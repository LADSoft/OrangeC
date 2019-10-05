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

/* we are only doing local opts on temp variables.  At this point,
 * any variable that does not have its address taken is also made a temp
 * variable
 */
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "compiler.h"

extern COMPILER_PARAMS cparams;
extern ARCH_ASM* chosenAssembler;
extern int blockCount;
extern BITINT bittab[BITINTBITS];
extern BLOCK** blockArray;
extern QUAD* intermed_head;
extern int tempCount;
extern bool setjmp_used;
extern int exitBlock;
extern bool functionHasAssembly;
extern LIST* temporarySymbols;
extern QUAD* intermed_tail;

TEMP_INFO** tempInfo;
int tempSize;

BRIGGS_SET* killed;
int tempBottom, nextTemp;

int fastcallAlias;

static void CalculateFastcall(SYMBOL* funcsp)
{
    fastcallAlias = 0;
#ifndef CPPTHISCALL
    if (funcsp->attribs.inheritable.linkage != lk_fastcall)
        return;
#endif

    if (chosenAssembler->arch->fastcallRegCount)
    {
        SYMLIST* hr = basetype(funcsp->tp)->syms->table[0];
        while (hr)
        {
            SYMBOL* sym = hr->p;
            if (sym->thisPtr)
            {
                fastcallAlias++;
            }
            else
            {
                if (funcsp->attribs.inheritable.linkage != lk_fastcall)
                    break;
                if (fastcallAlias >= chosenAssembler->arch->fastcallRegCount)
                    break;
                TYPE* tp = basetype(sym->tp);

                if ((tp->type < bt_float || (tp->type == bt_pointer && basetype(basetype(tp)->btp)->type != bt_func) ||
                     isref(tp)) &&
                    (sym->storage_class == sc_parameter))
                {
                    if (sym->tp->size > chosenAssembler->arch->parmwidth)
                        break;
                }
                else
                {
                    break;
                }
                fastcallAlias++;
            }
            hr = hr->next;
        }
    }
}
static void renameOneSym(SYMBOL* sym, int structret)
{
    TYPE* tp;
    /* needed for pointer aliasing */
    if (!sym->imvalue && basetype(sym->tp)->type != bt_any && basetype(sym->tp)->type != bt_memberptr && !isstructured(sym->tp) &&
        sym->tp->type != bt_ellipse && sym->tp->type != bt_aggregate)
    {
        if (sym->storage_class != sc_auto && sym->storage_class != sc_register)
        {
            IncGlobalFlag();
        }
        if (sym->imaddress)
        {
            IMODE* im = (IMODE*)Alloc(sizeof(IMODE));
            *im = *sym->imaddress;
            im->size = sizeFromType(sym->tp);
            im->mode = i_direct;
            sym->imvalue = im;
        }
        else if (sym->imind)
        {
            IMODE* im = (IMODE*)Alloc(sizeof(IMODE));
            *im = *sym->imind->im;
            im->size = ISZ_ADDR;
            im->mode = i_direct;
            sym->imvalue = im;
        }
        else
            sym->imvalue = tempreg(sizeFromType(sym->tp), false);

        if (sym->storage_class != sc_auto && sym->storage_class != sc_register)
        {
            DecGlobalFlag();
        }
    }
    tp = sym->tp;
    if (tp->type == bt_typedef)
        tp = tp->btp;
    tp = basetype(tp);

    bool fastcallCandidate = sym->storage_class == sc_parameter && fastcallAlias &&
        (sym->offset - fastcallAlias * chosenAssembler->arch->parmwidth < chosenAssembler->arch->retblocksize);

    if (!sym->pushedtotemp && (!sym->imaddress || fastcallCandidate) && !sym->inasm && (!sym->inCatch || fastcallCandidate) &&
        (((chosenAssembler->arch->hasFloatRegs || tp->type < bt_float) && tp->type < bt_void) ||
        (tp->type == bt_pointer && basetype(basetype(tp)->btp)->type != bt_func) || isref(tp)) &&
            (sym->storage_class == sc_auto || sym->storage_class == sc_register || sym->storage_class == sc_parameter) &&
        (!sym->usedasbit || fastcallCandidate))
    {
        /* this works because all IMODES refering to the same
            * variable are the same, at least until this point
            * that will change when we start inserting temps
            */
        IMODE* parmName;
        EXPRESSION* ep;
        if (sym->imaddress || sym->inCatch)
        {
            ep = anonymousVar(sc_auto, sym->tp);  // fastcall which takes an address
        }
        else
        {
            ep = tempenode();
            ep->v.sp->tp = sym->tp;
            ep->right = (EXPRESSION*)sym;
            /* marking both the orignal var and the new temp as pushed to temp*/
            sym->pushedtotemp = true;
            ep->v.sp->pushedtotemp = true;
        }
        sym->allocate = false;

        bool dofastcall = false;

        if (sym->storage_class == sc_parameter)
        {
            if (fastcallAlias)
            {
                // for fastcall, rename the affected parameter nodes with
                // a temp.   It will later be precolored...
                if (sym->offset - (fastcallAlias + structret) * chosenAssembler->arch->parmwidth <
                    chosenAssembler->arch->retblocksize &&
                    (!structret || sym->offset != chosenAssembler->arch->retblocksize))
                {
                    parmName = tempreg(sym->imvalue->size, 0);
                    dofastcall = true;
                }
                else
                {
                    parmName = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
                    *parmName = *sym->imvalue;
                }
            }
            else
            {
                parmName = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
                *parmName = *sym->imvalue;
            }
        }
        if (sym->imvalue)
        {
            ep->isvolatile = sym->imvalue->offset->isvolatile;
            ep->isrestrict = sym->imvalue->offset->isrestrict;
            sym->imvalue->offset = ep;
        }
        if (sym->imaddress)
        {
            ep->isvolatile = sym->imvalue->offset->isvolatile;
            ep->isrestrict = sym->imvalue->offset->isrestrict;
            sym->imaddress->offset = ep;
        }
        if (sym->imind)
        {
            IMODELIST* iml = sym->imind;
            ep->isvolatile = sym->imind->im->offset->isvolatile;
            ep->isrestrict = sym->imind->im->offset->isrestrict;
            while (iml)
            {
                iml->im->offset = ep;
                iml = iml->next;
            }
        }
        ep->v.sp->imvalue = sym->imvalue;
        if (sym->storage_class == sc_parameter && !(chosenAssembler->arch->denyopts & DO_NOLOADSTACK))
        {
            QUAD* q;
            QUAD* bl1 = blockArray[1]->head;
            while (bl1->fwd->dc.opcode == i_line || bl1->fwd->dc.opcode == i_label)
                bl1 = bl1->fwd;
            gen_icode(i_assn, sym->imvalue, parmName, 0);
            if (dofastcall)
            {
                intermed_tail->alwayslive = true;
                intermed_tail->fastcall =
                    -(sym->offset - chosenAssembler->arch->retblocksize) / chosenAssembler->arch->parmwidth - 1 + structret;
            }
            q = intermed_tail;
            q->back->fwd = nullptr;
            intermed_tail = q->back;
            q->block = bl1->block;
            q->fwd = bl1->fwd;
            q->back = bl1;
            q->fwd->back = q;
            q->back->fwd = q;
        }
    }
}
static void renameToTemps(SYMBOL* funcsp)
{
    LIST* lst;
    bool doRename = true;
    HASHTABLE* temp = funcsp->inlineFunc.syms;
    CalculateFastcall(funcsp);
    doRename &= (cparams.prm_optimize_for_speed || cparams.prm_optimize_for_size) && !functionHasAssembly;
    /* if there is a setjmp in the function, no variable gets moved into a reg */
    doRename &= !(setjmp_used);
    doRename &= !(funcsp->anyTry);
    temp = funcsp->inlineFunc.syms;
    bool structret = !!isstructured(basetype(funcsp->tp)->btp);
    while (temp)
    {
        SYMLIST* hr = temp->table[0];
        while (hr)
        {
            SYMBOL* sym = hr->p;
            if (doRename)
                renameOneSym(sym, structret);
            hr = hr->next;
        }
        temp = temp->next;
    }
    lst = temporarySymbols;
    while (lst)
    {
        SYMBOL* sym = (SYMBOL*)lst->data;
        if (!sym->anonymous && doRename)
        {
            renameOneSym(sym, structret);
        }
        lst = lst->next;
    }
}

static int AllocTempOpt(int size1)
{
    int t;
    int i;
    IMODE* rv;
    while (nextTemp < tempCount)
    {
        if (!tempInfo[nextTemp]->inUse)
            break;
        nextTemp++;
    }
    if (nextTemp < tempCount)
    {
        int n = tempCount;
        tempCount = nextTemp;
        rv = tempreg(size1, false);
        tempCount = n;
    }
    else
    {
        rv = tempreg(size1, false);
    }
    t = rv->offset->v.sp->value.i;
    if (t >= tempSize)
    {
        TEMP_INFO** temp = (TEMP_INFO**)oAlloc((tempSize + 1000) * sizeof(TEMP_INFO*));
        memcpy(temp, tempInfo, sizeof(TEMP_INFO*) * tempSize);
        tempSize += 1000;
        tempInfo = temp;
    }
    if (!tempInfo[t])
    {
        tempInfo[t] = (TEMP_INFO*)oAlloc(sizeof(TEMP_INFO));
    }
    else
    {
        memset(tempInfo[t], 0, sizeof(TEMP_INFO));
    }
    tempInfo[t]->enode = rv->offset;
    nextTemp = t;
    tempInfo[t]->partition = t;
    tempInfo[t]->color = -1;
    tempInfo[t]->inUse = true;
    return t;
}
IMODE* InitTempOpt(int size1, int size2)
{
    int t = AllocTempOpt(size1);
    tempInfo[t]->preSSATemp = -1;
    tempInfo[t]->postSSATemp = -1;
    tempInfo[t]->oldInductionVar = -1;
    if (size2 < 0)
        size2 = -size2;
    if (size2 == ISZ_UINT)
        size2 = chosenAssembler->arch->compatibleIntSize;
    if (size2 == ISZ_ADDR)
    {
        tempInfo[t]->usedAsAddress = true;
        size2 = chosenAssembler->arch->compatibleAddrSize;
    }
    if (size2 >= ISZ_FLOAT)
        tempInfo[t]->usedAsFloat = true;
    tempInfo[t]->size = size2;
    return tempInfo[t]->enode->v.sp->imvalue;
}
static void InitTempInfo(void)
{
    QUAD* head = intermed_head;
    int i;
    nextTemp = tempBottom = tempCount;
    tempSize = tempCount + 1000;
    tempInfo = (TEMP_INFO**)oAlloc(sizeof(TEMP_INFO*) * (tempSize));

    for (i = 0; i < tempCount; i++)
    {
        tempInfo[i] = (TEMP_INFO*)oAlloc(sizeof(TEMP_INFO));
        tempInfo[i]->partition = i;
        tempInfo[i]->color = -1;
        tempInfo[i]->inUse = true;
    }
    while (head)
    {
        head->temps = 0;
        if (head->dc.opcode != i_block && !head->ignoreMe && head->dc.opcode != i_passthrough && head->dc.opcode != i_label)
        {
            if (head->ans && (head->ans->mode == i_ind || head->ans->mode == i_direct))
            {
                if (head->ans->offset->type == en_tempref)
                {
                    int tnum = head->ans->offset->v.sp->value.i;
                    if (!head->ans->retval)
                        head->temps |= TEMP_ANS;
                    tempInfo[tnum]->enode = head->ans->offset;
                    tempInfo[tnum]->size = head->ans->offset->v.sp->imvalue->size;
                    if (tempInfo[tnum]->size < 0)
                        tempInfo[tnum]->size = -tempInfo[tnum]->size;
                    if (tempInfo[tnum]->size == ISZ_UINT)
                        tempInfo[tnum]->size = chosenAssembler->arch->compatibleIntSize;
                    if (tempInfo[tnum]->size == ISZ_ADDR)
                    {
                        tempInfo[tnum]->usedAsAddress = true;
                        tempInfo[tnum]->size = chosenAssembler->arch->compatibleAddrSize;
                    }
                    if (tempInfo[tnum]->size >= ISZ_FLOAT)
                        tempInfo[tnum]->usedAsFloat = true;
                }
            }
            if (head->dc.left && (head->dc.left->mode == i_ind || head->dc.left->mode == i_direct))
            {
                if (head->dc.left->offset->type == en_tempref)
                {
                    int tnum = head->dc.left->offset->v.sp->value.i;
                    if (!head->dc.left->retval)
                        head->temps |= TEMP_LEFT;
                    tempInfo[tnum]->enode = head->dc.left->offset;
                    tempInfo[tnum]->size = head->dc.left->offset->v.sp->imvalue->size;
                    if (tempInfo[tnum]->size < 0)
                        tempInfo[tnum]->size = -tempInfo[tnum]->size;
                    if (tempInfo[tnum]->size == ISZ_UINT)
                        tempInfo[tnum]->size = chosenAssembler->arch->compatibleIntSize;
                    if (tempInfo[tnum]->size == ISZ_ADDR)
                    {
                        tempInfo[tnum]->usedAsAddress = true;
                        tempInfo[tnum]->size = chosenAssembler->arch->compatibleAddrSize;
                    }
                    if (tempInfo[tnum]->size >= ISZ_FLOAT)
                        tempInfo[tnum]->usedAsFloat = true;
                }
            }
            if (head->dc.right && (head->dc.right->mode == i_ind || head->dc.right->mode == i_direct))
            {
                if (head->dc.right->offset->type == en_tempref)
                {
                    int tnum = head->dc.right->offset->v.sp->value.i;
                    if (!head->dc.right->retval)
                        head->temps |= TEMP_RIGHT;
                    tempInfo[tnum]->enode = head->dc.right->offset;
                    tempInfo[tnum]->size = head->dc.right->offset->v.sp->imvalue->size;
                    if (tempInfo[tnum]->size < 0)
                        tempInfo[tnum]->size = -tempInfo[tnum]->size;
                    if (tempInfo[tnum]->size == ISZ_UINT)
                        tempInfo[tnum]->size = chosenAssembler->arch->compatibleIntSize;
                    if (tempInfo[tnum]->size == ISZ_ADDR)
                    {
                        tempInfo[tnum]->usedAsAddress = true;
                        tempInfo[tnum]->size = chosenAssembler->arch->compatibleAddrSize;
                    }
                    if (tempInfo[tnum]->size >= ISZ_FLOAT)
                        tempInfo[tnum]->usedAsFloat = true;
                }
            }
        }
        head = head->fwd;
    }
}
void insertDefines(QUAD* head, BLOCK* b, int tnum)
{
    LIST* l = (LIST*)oAlloc(sizeof(LIST));
    l->data = (void*)head;
    l->next = tempInfo[tnum]->idefines;
    tempInfo[tnum]->idefines = l;

    l = (LIST*)oAlloc(sizeof(LIST));
    l->data = (void*)b;
    l->next = tempInfo[tnum]->bdefines;
    tempInfo[tnum]->bdefines = l;
}
void insertUses(QUAD* head, int dest)
{
    LIST** l = &tempInfo[dest]->iuses;
    while (*l)
    {
        if ((*l)->data == (void*)head)
            return;
        l = &(*l)->next;
    }
    *l = (LIST*)oAlloc(sizeof(LIST));
    (*l)->data = (void*)head;
}
void definesInfo(void)
{
    QUAD* head = intermed_head;
    BLOCK* block = nullptr;
    int i;
    for (i = 0; i < tempCount; i++)
    {
        tempInfo[i]->bdefines = nullptr;
        tempInfo[i]->idefines = nullptr;
    }
    while (head)
    {
        if (head->dc.opcode == i_block)
            block = head->block;
        if ((head->temps & TEMP_ANS) && head->ans->mode == i_direct)
        {
            int tnum = head->ans->offset->v.sp->value.i;
            insertDefines(head, block, tnum);
        }
        head = head->fwd;
    }
}
void usesInfo(void)
{
    QUAD* head = intermed_head;
    int i;
    for (i = 0; i < tempCount; i++)
    {
        tempInfo[i]->iuses = nullptr;
    }
    while (head)
    {
        if ((head->temps & TEMP_ANS) && head->ans->mode == i_ind)
        {
            if (head->ans->offset)
                insertUses(head, head->ans->offset->v.sp->value.i);
            if (head->ans->offset2)
                insertUses(head, head->ans->offset2->v.sp->value.i);
        }
        if (head->temps & TEMP_LEFT)
        {
            if (head->dc.left->offset)
                insertUses(head, head->dc.left->offset->v.sp->value.i);
            if (head->dc.left->offset2)
                insertUses(head, head->dc.left->offset2->v.sp->value.i);
        }
        if (head->temps & TEMP_RIGHT)
        {
            if (head->dc.right->offset)
                insertUses(head, head->dc.right->offset->v.sp->value.i);
            if (head->dc.right->offset2)
                insertUses(head, head->dc.right->offset2->v.sp->value.i);
        }
        head = head->fwd;
    }
}
void gatherLocalInfo(SYMBOL* funcsp)
{
    renameToTemps(funcsp);
    InitTempInfo();
    definesInfo();
}
