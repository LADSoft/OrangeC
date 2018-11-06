/* Software License Agreement
 *
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the
 *     Orange C "Target Code" exception.
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
extern BOOLEAN setjmp_used;
extern int exitBlock;
extern BOOLEAN functionHasAssembly;
extern LIST* temporarySymbols;
extern QUAD* intermed_tail;

TEMP_INFO** tempInfo;
int tempSize;

BRIGGS_SET* killed;
int tempBottom, nextTemp;

int fastcallAlias;

static void CalculateFastcall(SYMBOL *funcsp)
{
#ifndef CPPTHISCALL
    if (funcsp->linkage != lk_fastcall)
        return;
#endif

    fastcallAlias = 0;
    if (chosenAssembler->arch->fastcallRegCount)
    {
        HASHREC *hr = basetype(funcsp->tp)->syms->table[0];
        while (hr)
        {
            SYMBOL *sp = (SYMBOL *)hr->p;
            if (sp->thisPtr)
            {
                fastcallAlias++;
            }
            else
            {
                if (funcsp->linkage != lk_fastcall)
                    break;
                if (fastcallAlias >= chosenAssembler->arch->fastcallRegCount)
                    break;
                TYPE *tp = basetype(sp->tp);
                
                if  ((tp->type < bt_float ||
                    (tp->type == bt_pointer && basetype(basetype(tp)->btp)->type != bt_func) || isref(tp)) &&
                        (sp->storage_class == sc_parameter))
                {
                    if (sp->tp->size > chosenAssembler->arch->parmwidth)
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
static void renameOneSym(SYMBOL* sp, BOOLEAN structret)
{
    TYPE* tp;
    /* needed for pointer aliasing */
    if (!sp->imvalue && basetype(sp->tp)->type != bt_any && basetype(sp->tp)->type != bt_memberptr && !isstructured(sp->tp) &&
        sp->tp->type != bt_ellipse && sp->tp->type != bt_aggregate)
    {
        if (sp->storage_class != sc_auto && sp->storage_class != sc_register)
        {
            IncGlobalFlag();
        }
        if (sp->imaddress)
        {
            IMODE* im = Alloc(sizeof(IMODE));
            *im = *sp->imaddress;
            im->size = sizeFromType(sp->tp);
            im->mode = i_direct;
            sp->imvalue = im;
        }
        else if (sp->imind)
        {
            IMODE* im = Alloc(sizeof(IMODE));
            *im = *sp->imind->im;
            im->size = ISZ_ADDR;
            im->mode = i_direct;
            sp->imvalue = im;
        }
        else
            sp->imvalue = tempreg(sizeFromType(sp->tp), FALSE);

        if (sp->storage_class != sc_auto && sp->storage_class != sc_register)
        {
            DecGlobalFlag();
        }
    }
    tp = sp->tp;
    if (tp->type == bt_typedef)
        tp = tp->btp;
    tp = basetype(tp);

    BOOLEAN fastcallCandidate = sp->storage_class == sc_parameter && fastcallAlias &&
        (sp->offset - fastcallAlias * chosenAssembler->arch->parmwidth < chosenAssembler->arch->retblocksize);

    if (!sp->pushedtotemp && (!sp->imaddress || fastcallCandidate) && !sp->inasm && (!sp->inCatch || fastcallCandidate) &&
        (((chosenAssembler->arch->hasFloatRegs || tp->type < bt_float) && tp->type < bt_void) ||
        (tp->type == bt_pointer && basetype(basetype(tp)->btp)->type != bt_func) || isref(tp)) &&
            (sp->storage_class == sc_auto || sp->storage_class == sc_register || sp->storage_class == sc_parameter) && (!sp->usedasbit || fastcallCandidate))
    {
        /* this works because all IMODES refering to the same
         * variable are the same, at least until this point
         * that will change when we start inserting temps
         */
        IMODE* parmName;
        EXPRESSION* ep;
        if (sp->imaddress || sp->inCatch)
        {
            ep = anonymousVar(sc_auto, sp->tp); // fastcall which takes an address
        }
        else
        {
            ep = tempenode();
            ep->v.sp->tp = sp->tp;
            ep->right = (EXPRESSION*)sp;
            /* marking both the orignal var and the new temp as pushed to temp*/
            sp->pushedtotemp = TRUE;
            ep->v.sp->pushedtotemp = TRUE;
        }
        sp->allocate = FALSE;

        BOOLEAN dofastcall = FALSE;

        if (sp->storage_class == sc_parameter)
        {
            if (fastcallAlias)
            {
                // for fastcall, rename the affected parameter nodes with
                // a temp.   It will later be precolored...
                if (sp->offset - (fastcallAlias + structret) * chosenAssembler->arch->parmwidth < chosenAssembler->arch->retblocksize
                    && (!structret || sp->offset != chosenAssembler->arch->retblocksize))
                {
                    parmName = tempreg(sp->imvalue->size, 0);
                    dofastcall = TRUE;
                }
                else
                {
                    parmName = (IMODE*)Alloc(sizeof(IMODE));
                    *parmName = *sp->imvalue;
                }

            }
            else
            {
                parmName = (IMODE*)Alloc(sizeof(IMODE));
                *parmName = *sp->imvalue;
            }
        }
        if (sp->imvalue)
        {
            ep->isvolatile = sp->imvalue->offset->isvolatile;
            ep->isrestrict = sp->imvalue->offset->isrestrict;
            sp->imvalue->offset = ep;
        }
        if (sp->imaddress)
        {
            ep->isvolatile = sp->imvalue->offset->isvolatile;
            ep->isrestrict = sp->imvalue->offset->isrestrict;
            sp->imaddress->offset = ep;
        }
        if (sp->imind)
        {
            IMODELIST* iml = sp->imind;
            ep->isvolatile = sp->imind->im->offset->isvolatile;
            ep->isrestrict = sp->imind->im->offset->isrestrict;
            while (iml)
            {
                iml->im->offset = ep;
                iml = iml->next;
            }
        }
        ep->v.sp->imvalue = sp->imvalue;
        if (sp->storage_class == sc_parameter && !(chosenAssembler->arch->denyopts & DO_NOLOADSTACK))
        {
            QUAD* q;
            QUAD* bl1 = blockArray[1]->head;
            while (bl1->fwd->dc.opcode == i_line || bl1->fwd->dc.opcode == i_label)
                bl1 = bl1->fwd;
            gen_icode(i_assn, sp->imvalue, parmName, 0);
            if (dofastcall)
            {
                intermed_tail->alwayslive = TRUE;
                intermed_tail->fastcall = -(sp->offset - chosenAssembler->arch->retblocksize) / chosenAssembler->arch->parmwidth - 1 + structret;
            }
            q = intermed_tail;
            q->back->fwd = NULL;
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
    BOOLEAN doRename = TRUE;
    HASHTABLE* temp = funcsp->inlineFunc.syms;
    CalculateFastcall(funcsp);
    doRename &= (cparams.prm_optimize_for_speed || cparams.prm_optimize_for_size) && !functionHasAssembly;
    /* if there is a setjmp in the function, no variable gets moved into a reg */
    doRename &= !(setjmp_used);
    temp = funcsp->inlineFunc.syms;
    BOOLEAN structret = !!isstructured(basetype(funcsp->tp)->btp);
    while (temp)
    {
        HASHREC* hr = temp->table[0];
        while (hr)
        {
            SYMBOL* sym = (SYMBOL*)hr->p;
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
        rv = tempreg(size1, FALSE);
        tempCount = n;
    }
    else
    {
        rv = tempreg(size1, FALSE);
    }
    t = rv->offset->v.sp->value.i;
    if (t >= tempSize)
    {
        TEMP_INFO** temp = oAlloc((tempSize + 1000) * sizeof(TEMP_INFO*));
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
    tempInfo[t]->inUse = TRUE;
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
        tempInfo[t]->usedAsAddress = TRUE;
        size2 = chosenAssembler->arch->compatibleAddrSize;
    }
    if (size2 >= ISZ_FLOAT)
        tempInfo[t]->usedAsFloat = TRUE;
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
        tempInfo[i] = oAlloc(sizeof(TEMP_INFO));
        tempInfo[i]->partition = i;
        tempInfo[i]->color = -1;
        tempInfo[i]->inUse = TRUE;
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
                        tempInfo[tnum]->usedAsAddress = TRUE;
                        tempInfo[tnum]->size = chosenAssembler->arch->compatibleAddrSize;
                    }
                    if (tempInfo[tnum]->size >= ISZ_FLOAT)
                        tempInfo[tnum]->usedAsFloat = TRUE;
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
                        tempInfo[tnum]->usedAsAddress = TRUE;
                        tempInfo[tnum]->size = chosenAssembler->arch->compatibleAddrSize;
                    }
                    if (tempInfo[tnum]->size >= ISZ_FLOAT)
                        tempInfo[tnum]->usedAsFloat = TRUE;
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
                        tempInfo[tnum]->usedAsAddress = TRUE;
                        tempInfo[tnum]->size = chosenAssembler->arch->compatibleAddrSize;
                    }
                    if (tempInfo[tnum]->size >= ISZ_FLOAT)
                        tempInfo[tnum]->usedAsFloat = TRUE;
                }
            }
        }
        head = head->fwd;
    }
}
void insertDefines(QUAD* head, BLOCK* b, int tnum)
{
    LIST* l = oAlloc(sizeof(LIST));
    l->data = (void*)head;
    l->next = tempInfo[tnum]->idefines;
    tempInfo[tnum]->idefines = l;

    l = oAlloc(sizeof(LIST));
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
    *l = oAlloc(sizeof(LIST));
    (*l)->data = (void*)head;
}
void definesInfo(void)
{
    QUAD* head = intermed_head;
    BLOCK* block = NULL;
    int i;
    for (i = 0; i < tempCount; i++)
    {
        tempInfo[i]->bdefines = NULL;
        tempInfo[i]->idefines = NULL;
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
        tempInfo[i]->iuses = NULL;
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
