/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
extern ARCH_ASM *chosenAssembler; 
extern int blockCount;
extern BITINT bittab[BITINTBITS];
extern BLOCK **blockArray;
extern QUAD *intermed_head;
extern int tempCount;
extern BOOLEAN setjmp_used;
extern int exitBlock;
extern BOOLEAN functionHasAssembly;
extern LIST *temporarySymbols;
extern QUAD *intermed_tail;

TEMP_INFO **tempInfo;
int tempSize;

BRIGGS_SET *killed;
int tempBottom, nextTemp;

static void renameOneSym(SYMBOL *sp)
{
    TYPE *tp;
    /* needed for pointer aliasing */
    if (!sp->imvalue && basetype(sp->tp)->type != bt_any && basetype(sp->tp)->type != bt_memberptr && !isstructured(sp->tp) && sp->tp->type != bt_ellipse && sp->tp->type != bt_aggregate)
    {
        if (sp->storage_class != sc_auto && sp->storage_class !=
            sc_register)
        {
            IncGlobalFlag();
        }
        if (sp->imaddress)
        {
            IMODE *im = Alloc(sizeof(IMODE));
            *im = *sp->imaddress;
            im->size = sizeFromType(sp->tp);
            im->mode = i_direct;
            sp->imvalue = im;
        }
        else if (sp->imind)
        {
            IMODE *im = Alloc(sizeof(IMODE));
            *im = *sp->imind->im;
            im-> size = ISZ_ADDR;
            im->mode = i_direct;
            sp->imvalue = im;
        }
        else
            sp->imvalue = tempreg(sizeFromType(sp->tp), FALSE);
        
        if (sp->storage_class != sc_auto && sp->storage_class !=
            sc_register)
        {
            DecGlobalFlag();
        }
    }
    tp = sp->tp;
    if (tp->type == bt_typedef)
        tp = tp->btp;
    tp = basetype(tp);
    if (!sp->pushedtotemp && !sp->imaddress && !sp->inasm && !sp->inCatch
        && (((chosenAssembler->arch->hasFloatRegs || tp->type < bt_float) && tp->type < bt_void)  
            || (tp->type == bt_pointer && tp->btp->type != bt_func)
            || isref(tp)) 
        && (sp->storage_class == sc_auto || sp->storage_class == sc_register || sp->storage_class == sc_parameter)
        && !sp->usedasbit)
    {
        /* this works because all IMODES refering to the same
         * variable are the same, at least until this point
         * that will change when we start inserting temps
         */
        IMODE *parmName;
        EXPRESSION *ep = tempenode();
        ep->v.sp->tp = sp->tp;
        ep->right = (EXPRESSION *)sp;
        /* marking both the orignal var and the new temp as pushed to temp*/
        sp->pushedtotemp = TRUE ;
        ep->v.sp->pushedtotemp = TRUE;
        sp->allocate = FALSE;
        if (sp->storage_class == sc_parameter)
        {
            parmName = (IMODE *)Alloc(sizeof(IMODE));
            *parmName = *sp->imvalue;
        }
        if (sp->imvalue)
        {
            ep->isvolatile = sp->imvalue->offset->isvolatile;
            ep->isrestrict = sp->imvalue->offset->isrestrict;
            sp->imvalue->offset = ep ;
        }
        if (sp->imind)
        {
            IMODELIST *iml = sp->imind;
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
            QUAD *q;
            QUAD *bl1 = blockArray[1]->head;
            while (bl1->fwd->dc.opcode == i_line || bl1->fwd->dc.opcode == i_label)
                bl1 = bl1->fwd;
            gen_icode(i_assn, sp->imvalue, parmName, 0);
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
static void renameToTemps(SYMBOL *funcsp)
{
    LIST *lst;
    BOOLEAN doRename = TRUE;
    HASHTABLE *temp = funcsp->inlineFunc.syms;
    doRename &= (cparams.prm_optimize_for_speed || cparams.prm_optimize_for_size) && !functionHasAssembly;
    /* if there is a setjmp in the function, no variable gets moved into a reg */
    doRename &= ! (setjmp_used);
    temp = funcsp->inlineFunc.syms;
    while (temp)
    {
        HASHREC *hr = temp->table[0];
        while (hr)
        {
            SYMBOL *sym = (SYMBOL *)hr->p;
            if (doRename)
                renameOneSym(sym);
            hr = hr->next;
        }
        temp = temp->next;
    }
    lst = temporarySymbols;
    while (lst)
    {
        SYMBOL *sym = (SYMBOL *)lst->data;
        if (!sym->anonymous && doRename)
        {
            renameOneSym(sym);
        }
        lst = lst->next;
    }
}

static int AllocTempOpt(int size1)
{
    int t;
    int i;
    IMODE *rv;
    while (nextTemp < tempCount)
    {
        if (! tempInfo[nextTemp]->inUse)
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
        TEMP_INFO **temp = oAlloc((tempSize + 1000) * sizeof(TEMP_INFO *));
        memcpy(temp, tempInfo, sizeof(TEMP_INFO *) * tempSize);
        tempSize += 1000;
        tempInfo = temp;
    }
    if (!tempInfo[t])
    {
        tempInfo[t] = (TEMP_INFO *)oAlloc(sizeof(TEMP_INFO));
    }
    else
    {
        memset(tempInfo[t], 0, sizeof(TEMP_INFO));
    }
    tempInfo[t]->enode = rv->offset;
    nextTemp = t;
    tempInfo[t]-> partition = t;
    tempInfo[t]->color = -1;
    tempInfo[t]->inUse = TRUE;
    return t;
}
IMODE *InitTempOpt(int size1, int size2)
{
    int t= AllocTempOpt(size1);
    tempInfo[t]->preSSATemp = -1;
    tempInfo[t]->postSSATemp = -1;
    tempInfo[t]->oldInductionVar = -1;
    if (size2 < 0)
        size2 = - size2;
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
    QUAD *head = intermed_head;
    int i;
    nextTemp = tempBottom = tempCount;
    tempSize = tempCount + 1000;
    tempInfo = (TEMP_INFO **)oAlloc(sizeof(TEMP_INFO *) * (tempSize));

    for (i=0; i < tempCount; i++)
    {
        tempInfo[i] = oAlloc(sizeof(TEMP_INFO));
        tempInfo[i]-> partition = i;
        tempInfo[i]->color = -1;
        tempInfo[i]->inUse = TRUE;
    }
    while (head)
    {
        head->temps = 0;
        if  (head->dc.opcode != i_block && !head->ignoreMe && head->dc.opcode != i_passthrough && head->dc.opcode !=
            i_label)
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
                        tempInfo[tnum]->size = - tempInfo[tnum]->size;
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
            if (head->dc.left && (head->dc.left->mode == i_ind 
                                  || head->dc.left->mode == i_direct))
            {
                if (head->dc.left->offset->type == en_tempref)
                {
                    int tnum = head->dc.left->offset->v.sp->value.i;
                    if (!head->dc.left->retval)
                        head->temps |= TEMP_LEFT;
                    tempInfo[tnum]->enode = head->dc.left->offset;
                    tempInfo[tnum]->size = head->dc.left->offset->v.sp->imvalue->size;
                    if (tempInfo[tnum]->size < 0)
                        tempInfo[tnum]->size = - tempInfo[tnum]->size;
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
            if (head->dc.right && (head->dc.right->mode == i_ind 
                                  || head->dc.right->mode == i_direct))
            {
                if (head->dc.right->offset->type == en_tempref)
                {
                    int tnum = head->dc.right->offset->v.sp->value.i;
                    if (!head->dc.right->retval)
                        head->temps |= TEMP_RIGHT;
                    tempInfo[tnum]->enode = head->dc.right->offset;
                    tempInfo[tnum]->size = head->dc.right->offset->v.sp->imvalue->size;
                    if (tempInfo[tnum]->size < 0)
                        tempInfo[tnum]->size = - tempInfo[tnum]->size;
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
        head = head->fwd ;
    }
}
void insertDefines(QUAD *head, BLOCK *b, int tnum)
{
    LIST *l = oAlloc(sizeof(LIST));
    l->data = (void *)head;
    l->next = tempInfo[tnum]->idefines;
    tempInfo[tnum]->idefines = l;

    l = oAlloc(sizeof(LIST));
    l->data = (void *)b;
    l->next = tempInfo[tnum]->bdefines;
    tempInfo[tnum]->bdefines = l;
}
void insertUses(QUAD *head, int dest)
{
    LIST **l = & tempInfo[dest]->iuses;
    while (*l)
    {
        if ((*l)->data == (void *)head)
            return ;
        l = &(*l)->next;
    }
    *l = oAlloc(sizeof(LIST));
    (*l)->data = (void *)head;
}
void definesInfo(void)
{
    QUAD *head = intermed_head;
    BLOCK *block = NULL;
    int i;
    for (i=0; i < tempCount; i++)
    {
        tempInfo[i]->bdefines = NULL;
        tempInfo[i]->idefines = NULL;
    }
    while (head)
    {
        if (head->dc.opcode == i_block)
            block = head->block;
        if ((head->temps & TEMP_ANS) && head->ans->mode == i_direct )
        {
            int tnum = head->ans->offset->v.sp->value.i;
            insertDefines(head, block, tnum);
        }
        head = head->fwd ;
    }
}
void usesInfo(void)
{
    QUAD *head = intermed_head;
    int i;
    for (i=0; i < tempCount; i++)
    {
        tempInfo[i]->iuses = NULL;
    }
    while (head)
    {
        if ((head->temps & TEMP_ANS) && head->ans->mode == i_ind )
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
        head = head->fwd ;
    }
}
void gatherLocalInfo(SYMBOL *funcsp)
{
    renameToTemps(funcsp);
    InitTempInfo();
    definesInfo();
}
