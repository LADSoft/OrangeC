/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
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
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <limits.h>
#include "compiler.h"

/* reshaping and loop induction strength reduction */

extern FILE *icdFile;
extern TEMP_INFO **tempInfo;
extern int tempCount;
extern int loopCount;
extern LOOP **loopArray;
extern int blockCount;
extern BLOCK **blockArray;
extern DAGLIST *ins_hash[DAGSIZE];
extern DAGLIST *name_hash[DAGSIZE];

static int cachedTempCount;

#ifdef XXXXX
void DumpInvariants(void)
{
    int i;
    if (icdFile)
    {
        fprintf(icdFile,";************** invariants\n\n");
        for (i=0; i < tempCount; i++)
        {
            if (tempInfo[i]->variantLoop)
            {
                fprintf(icdFile, "T%d = L%d\n", i, tempInfo[i]->variantLoop->loopnum+1);
            }
        }
        fprintf(icdFile,";************** induction sets\n\n");
        for (i=0; i < loopCount; i++)
        {
            if (loopArray[i]->type != LT_BLOCK)
            {
                INDUCTION_LIST *is = loopArray[i]->inductionSets;
                int n = 1;
                if (is)
                    fprintf(icdFile, "Loop %d\n", i + 1);
                while (is)
                {
                    LIST *dt = (LIST *)is->vars;
                    fprintf(icdFile, "%d: ", n++);
                    while (dt)
                    {
                        fprintf(icdFile, "T%d ", (int)dt->data);
                        dt = dt->next;
                    }
                    fprintf(icdFile, "\n");
                    is = is->next;
                }
            }
        }
    }
}
#endif
BOOLEAN variantThisLoop(BLOCK *b, int tnum)
{
    if (tempInfo[tnum]->instructionDefines)
    {
        LOOP *thisLp = b->loopParent;
        LOOP *variant = tempInfo[tnum]->variantLoop;
        return variant == thisLp || isAncestor(thisLp, variant);
    }
    return FALSE;
}
static BOOLEAN usedThisLoop(BLOCK *b, int tnum)
{
    INSTRUCTIONLIST *l = tempInfo[tnum]->instructionUses;
    LOOP *parent = b->loopParent;
    while (l)
    {
        LOOP *thisLp = l->ins->block->loopParent;
        if (parent == thisLp || isAncestor(parent, thisLp))
            return TRUE;
        l = l->next;
    }
    return FALSE;
}
static BOOLEAN inductionThisLoop(BLOCK *b, int tnum)
{
    return tempInfo[tnum]->inductionLoop && variantThisLoop(b, tnum);
}
BOOLEAN matchesop(enum i_ops one, enum i_ops two)
{
    if (one == two)
        return TRUE;
    switch(one)
    {
        case i_neg:
            return two == i_add || two == i_sub || two == i_mul;
        case i_not:
        case i_add:
        case i_sub:
            return two == i_add || two == i_sub;
        case i_mul:
        case i_lsl:
            return two == i_mul || two == i_lsl;
        default:
            return FALSE;
    }
}
static RESHAPE_LIST *InsertExpression(IMODE *im, RESHAPE_EXPRESSION *expr, QUAD *ins, int flags)
{
    RESHAPE_LIST *list = (RESHAPE_LIST *)tAlloc(sizeof(RESHAPE_LIST));
    RESHAPE_LIST **test = &expr->list;
    list->flags = flags;
    list->im = im;
    if (im->mode == i_immed)
        list->rporder = SHRT_MAX; /* evaluate constants as early as possible */
    else
    {
        list->rporder = ins->block->loopParent->loopnum;
    }
    expr->count ++;
    /* sort in descending order */
    while (*test)
    {
        if ((*test)->rporder < list->rporder)
            break;
        test = &(*test)->next;
    }
    list->next = *test;
    *test = list;
    return list;
}
static BOOLEAN GatherExpression(int tx, RESHAPE_EXPRESSION *expr, int flags)
{
    QUAD *ins = tempInfo[tx]->instructionDefines;
    if (ins && matchesop(ins->dc.opcode, expr->op))
    {
        if (ins->dc.opcode == i_neg || ins->dc.opcode == i_not)
        {
            /* the reason for this exemption is I don't want to deal with
             * maintaining the order of successive neg and not instructions
             */
            if (expr->list && expr->list->flags & (RF_NEG | RF_NOT))
                return FALSE;
            if ((ins->temps & TEMP_LEFT) && (ins->dc.left->mode != i_ind))
            {
                if (ins->dc.left->size < ISZ_FLOAT)
                {
                    int tnum = ins->dc.left->offset->v.sp->value.i;
                    if (ins->dc.opcode == i_neg)
                        flags ^= RF_NEG;
                    else
                        flags ^= RF_NOT;
                    if (inductionThisLoop(ins->block, tnum) || !GatherExpression(tnum, expr, flags))
                        InsertExpression(ins->dc.left, expr, ins, flags);
                    return TRUE;
                }
            }
        }
        else if ((ins->temps & (TEMP_LEFT | TEMP_RIGHT)) == (TEMP_LEFT | TEMP_RIGHT))
        {
            if (ins->dc.left->mode != i_ind && ins->dc.right->mode != i_ind)
            {
                if (ins->dc.left->size < ISZ_FLOAT && ins->dc.right->size < ISZ_FLOAT)
                {
                    
                    int tnuml = ins->dc.left->offset->v.sp->value.i;
                    int tnumr = ins->dc.right->offset->v.sp->value.i;
                    int flags1 = flags;
                    if (ins->dc.opcode == i_sub)
                        flags ^= RF_NEG;
                    if (inductionThisLoop(ins->block, tnuml) || !GatherExpression(tnuml, expr, flags1))
                        InsertExpression(ins->dc.left, expr, ins, flags1);
                    if (inductionThisLoop(ins->block, tnumr) || !GatherExpression(tnumr, expr, flags))
                        InsertExpression(ins->dc.right, expr, ins, flags);
                    return TRUE;
                }
            }
        }
        else if ((ins->temps & TEMP_LEFT) && ins->dc.left->mode == i_direct && 
                 ins->dc.right && ins->dc.right->mode == i_immed)
        {
            if (ins->dc.left->size < ISZ_FLOAT && ins->dc.right->size < ISZ_FLOAT)
            {
                int tnum = ins->dc.left->offset->v.sp->value.i;
                int flags1 = flags;
                RESHAPE_LIST *re;
                if (ins->dc.opcode == i_sub)
                    flags ^= RF_NEG;
                
                flags |= (ins->dc.opcode == i_lsl ? RF_SHIFT : 0);
                InsertExpression(ins->dc.right, expr, ins, flags);
                if (inductionThisLoop(ins->block, tnum) || !GatherExpression(tnum, expr, flags1))
                    InsertExpression(ins->dc.left, expr, ins, flags1);
                return TRUE;
            }
        }
        else if ((ins->temps & TEMP_RIGHT) && ins->dc.right->mode == i_direct && ins->dc.left->mode == i_immed)
        {
            if (ins->dc.left->size < ISZ_FLOAT && ins->dc.right->size < ISZ_FLOAT)
            {
                int tnum = ins->dc.right->offset->v.sp->value.i;
                int flags1 = flags;
                RESHAPE_LIST *re;
                if (ins->dc.opcode == i_sub)
                    flags ^= RF_NEG;
                flags |=(ins->dc.opcode == i_lsl ? RF_SHIFT : 0);
                if (inductionThisLoop(ins->block, tnum) || !GatherExpression(tnum, expr, flags))
                    InsertExpression(ins->dc.right, expr, ins, flags);
                InsertExpression(ins->dc.left, expr, ins, flags1);
                return TRUE;
            }
        }
    }
    return FALSE;
}
static void CreateExpressionLists(void)
{
    int i;
    for (i=0; i < cachedTempCount; i++)
    {
        if (tempInfo[i]->expressionRoot)
        {
            if (tempInfo[i]->enode->size < ISZ_FLOAT)
            {
                LOOP *lp = tempInfo[i]->instructionDefines->block->loopParent;
                /* if the prior of the entry is a critical block,
                 * then don't optimize the loop
                 */
                if (lp->entry->pred && !lp->entry->pred->block->critical)
                {
                    enum i_ops op = tempInfo[i]->instructionDefines->dc.opcode;
                    tempInfo[i]->expression.op = op;
                    GatherExpression(i, & tempInfo[i]->expression, 0);
                }
                else
                { 
                    tempInfo[i]->expressionRoot = FALSE;
                }
            }
            else 
            {
                tempInfo[i]->expressionRoot = FALSE;
            }
        }
    }
}
static RESHAPE_LIST *cloneReshape(RESHAPE_LIST *in)
{
    RESHAPE_LIST *rv = NULL, **p = &rv;
    while (in)
    {
        *p = tAlloc(sizeof(RESHAPE_LIST));
        **p = *in;
        (*p)->next = NULL;
        in = in->next;
        p = &(*p)->next;
    }
    return rv;
}
static void DistributeMultiplies(RESHAPE_EXPRESSION *re, RESHAPE_LIST *rl, int tnum)
{
    RESHAPE_LIST*temp = tempInfo[tnum]->expression.list, *replace = NULL;
    int n = INT_MAX;
    while (temp)
    {
        if (!temp->distributed && temp != rl)
        {
            int tnx;
            if (temp->im->offset->type != en_tempref)
            {
                replace = temp;
                break;
            }
            tnx = temp->im->offset->v.sp->value.i;
            if (tempInfo[tnx]->expression.count < n)
            {
                n = tempInfo[tnx]->expression.count;
                replace = temp;
            }
        }
        temp = temp->next;
    }	
    if (replace)
    {
        RESHAPE_LIST **temp1;
        replace->distrib = cloneReshape(re->list);
        replace->distributed = TRUE;
        temp1 = &tempInfo[tnum]->expression.list;
        while (*temp1)
        {
            if (*temp1 == rl)
            {
                *temp1 = (*temp1)->next;
                break;
            }
            temp1 = &(*temp1)->next;
        }
    }
    else
        rl->distributed = TRUE;
}
static void ApplyDistribution(void)
{
    BOOLEAN done = FALSE;
    while (!done)
    {
        int i;
        RESHAPE_EXPRESSION *next = NULL;
        RESHAPE_LIST *rl;
        int n = 0;
        int match = 0;
        for (i=0; i < cachedTempCount; i++)
        {
            if (tempInfo[i]->expressionRoot)
            {
                switch (tempInfo[i]->expression.op)
                {
                    case i_mul:
                    case i_lsl:
                    {
                        RESHAPE_LIST *l = tempInfo[i]->expression.list;
                        while (l)
                        {
                            if (!l->distributed)
                            {
                                if (l->im->offset->type == en_tempref)
                                {
                                    RESHAPE_EXPRESSION *re = &tempInfo[l->im->offset->v.sp->value.i]->expression;
                                    if (re->count > n)
                                    {
                                        n = re->count;
                                        next = re;
                                        match = i;
                                        rl = l;
                                    }
                                }
                            }
                            l = l->next;
                        }
                    }
                        break;
                    default:
                        break;
                }
            }
        }
        if (next)
        {
            done = FALSE;
            DistributeMultiplies(next, rl, match);
        }
        else
            done = TRUE;
    }
}
void ReplaceHashReshape(QUAD *rv, UBYTE *key, int size, DAGLIST **table)
{
    int hashval = dhash(key, size);
    DAGLIST *newDag;
    newDag = tAlloc(sizeof(DAGLIST));
    newDag->rv = (UBYTE *)rv;
    newDag->key = key;
    newDag->next =  table[hashval];
       table[hashval] = newDag;
}
static void replaceIM(IMODE **iml, IMODE *im)
{
    if ((*iml)->mode == i_immed)
        return;
    ReplaceHashReshape((QUAD *)im, (UBYTE *)&(*iml)->offset->v.sp->imvalue, sizeof(IMODE *), name_hash);
    if ((*iml)->mode == i_direct)
        *iml = im;
    else
    {
        IMODELIST *imlx = im->offset->v.sp->imind;
        while (imlx)
        {
            if (im->size == imlx->im->size)
            {
                *iml = imlx->im;
                break;
            }
            imlx = imlx->next;
        }
        if (!imlx)
        {
            SYMBOL *sp = im->offset->v.sp;
            IMODE *imind;
            IMODELIST *imindl;
            if (sp && sp->storage_class != sc_auto && sp->storage_class != sc_register)
            {
                IncGlobalFlag();
            }
            imind = Alloc(sizeof(IMODE));
            imindl = Alloc(sizeof(IMODELIST));
            if (sp && sp->storage_class != sc_auto && sp->storage_class != sc_register)
            {
                DecGlobalFlag();
            }
            *imind = *(im);
            imind->mode = i_ind;
            imind->ptrsize = (*iml)->ptrsize;
            imind->size = (*iml)->size;
            imindl->im = imind;
            imindl->next = im->offset->v.sp->imind;
            im->offset->v.sp->imind = imindl;
            *iml = imind;
        }
    }
}
static void CopyExpressionTree(enum i_ops op, BLOCK *b, QUAD *insertBefore, IMODE **iml, IMODE **imr)
{
    if ((*iml) && (*iml)->offset->type == en_tempref)
    {
        int tnum = (*iml)->offset->v.sp->value.i;
        QUAD *def = tempInfo[tnum]->instructionDefines;
        IMODE *im = (IMODE *)LookupNVHash((UBYTE *)&(*iml)->offset->v.sp->imvalue, sizeof(IMODE *), name_hash);
        if (im)
        {
            replaceIM(iml, im);
        }
        if (def && def->block->loopParent == b->loopParent && def->dc.opcode != i_phi)
        {
            QUAD *newIns ;
            op = i_nop;
            newIns = Alloc(sizeof(QUAD));
            newIns->ans = InitTempOpt(def->ans->offset->v.sp->imvalue->size, def->ans->offset->v.sp->imvalue->size);
            newIns->dc.left = def->dc.left;
            newIns->dc.right = def->dc.right;
            newIns->dc.opcode = def->dc.opcode;
            CopyExpressionTree(op, b, insertBefore, &newIns->dc.left, &newIns->dc.right);
            InsertInstruction(insertBefore->back, newIns);
            replaceIM(iml, newIns->ans);
            ReplaceHashReshape((QUAD *)newIns->ans, (UBYTE *)newIns, DAGCOMPARE, ins_hash);
        }
    }
    if ((*imr) && (*imr)->offset->type == en_tempref)
    {
        int tnum = (*imr)->offset->v.sp->value.i;
        QUAD *def = tempInfo[tnum]->instructionDefines;
        IMODE *im = (IMODE *)LookupNVHash((UBYTE *)&(*imr)->offset->v.sp->imvalue, sizeof(IMODE *), name_hash);
        if (im)
        {
            replaceIM(imr, im);
        }
        if (def && def->block->loopParent == b->loopParent && def->dc.opcode != i_phi)
        {
            QUAD *newIns ;
            op = i_nop;
            newIns = Alloc(sizeof(QUAD));
            newIns->ans = InitTempOpt(def->ans->offset->v.sp->imvalue->size, def->ans->offset->v.sp->imvalue->size);
            newIns->dc.left = def->dc.left;
            newIns->dc.right = def->dc.right;
            newIns->dc.opcode = def->dc.opcode;
            CopyExpressionTree(op, b, insertBefore, &newIns->dc.left, &newIns->dc.right);
            InsertInstruction(insertBefore->back, newIns);
            replaceIM(imr, newIns->ans);
            ReplaceHashReshape((QUAD *)newIns->ans, (UBYTE *)newIns, DAGCOMPARE, ins_hash);
        }
    }
}
static IMODE * InsertAddInstruction(BLOCK *b, int size, QUAD *insertBefore,
                                 int flagsl, IMODE *iml, int flagsr, IMODE *imr)
{
    QUAD *ins, *insn = NULL, *insn2 = NULL;
    IMODE *imrv;
    if (b)
        CopyExpressionTree(i_add, b, insertBefore, &iml, &imr);
    if (flagsl & (RF_NEG | RF_NOT))
    {
        insn = Alloc(sizeof(QUAD));
        insn->ans = InitTempOpt(size,size);
        insn->dc.opcode = flagsl & RF_NEG ? i_neg : i_not;
        insn->dc.left = iml;
        iml = insn->ans;
    }
    /*
    if (flagsr & (RF_NEG | RF_NOT))
    {
        insn2 = Alloc(sizeof(QUAD));
        insn2->ans = InitTempOpt(size,size);
        insn2->dc.opcode = flagsr & RF_NEG ? i_neg : i_not;
        insn2->dc.left = imr;
        imr = insn2->ans;
    }
    */
    ins = Alloc(sizeof(QUAD));
    ins->dc.opcode = flagsr & RF_NEG ? i_sub : i_add;
    ins->dc.left = iml;
    ins->dc.right = imr;
    imrv = (IMODE *)LookupNVHash((UBYTE *)ins, DAGCOMPARE, ins_hash);
    if (imrv)
        return imrv;
    else
    {
        ins->ans = InitTempOpt(size,size);
        if (insn)
        {
            InsertInstruction(insertBefore->back, insn);
        }
        if (insn2)
        {
            InsertInstruction(insertBefore->back, insn2);
        }
        InsertInstruction(insertBefore->back, ins);
        ReplaceHashReshape((QUAD *)ins->ans, (UBYTE *)ins, DAGCOMPARE, ins_hash);
        return ins->ans;
    }
    
}
static IMODE * InsertMulInstruction(BLOCK *b, int size, QUAD *insertBefore,
                                 int flagsl, IMODE *iml, int flagsr, IMODE *imr)
{
    QUAD *ins, *insn = NULL;
    IMODE *imrv;
    if (b)
        CopyExpressionTree(i_mul, b, insertBefore, &iml, &imr);
    if ((flagsl & RF_SHIFT) && !(flagsr & RF_SHIFT))
    {
        int ft = flagsl;
        IMODE *im = iml;
        flagsl = flagsr;
        iml = imr;
        flagsr = ft;
        imr = im;
    }
    if (flagsl & RF_SHIFT)
    {
        if (iml->mode == i_immed && isintconst(iml->offset))
        {
            iml->offset->v.i = (1 << iml->offset->v.i);
        }
        else
        {
            insn = Alloc(sizeof(QUAD));
            insn->ans = InitTempOpt(size,size);
            insn->dc.left = make_immed(size, 1);
            insn->dc.opcode = i_lsl;
            insn->dc.right = iml;
            iml = insn->ans;
        }
    }
    ins = Alloc(sizeof(QUAD));
    ins->dc.opcode = flagsr & RF_SHIFT ? i_lsl : i_mul;
    ins->dc.left = iml;
    ins->dc.right = imr;
    imrv = (IMODE *)LookupNVHash((UBYTE *)ins, DAGCOMPARE, ins_hash);
    if (imrv)
        return imrv;
    else
    {
        ins->ans = InitTempOpt(size,size);
        if (insn)
        {
            InsertInstruction(insertBefore->back, insn);
        }
        InsertInstruction(insertBefore->back, ins);
        ReplaceHashReshape((QUAD *)ins->ans, (UBYTE *)ins, DAGCOMPARE, ins_hash);
        return ins->ans;
    }
    
}
void unmarkPreSSA(QUAD *ins)
{
    if ((ins->temps & TEMP_ANS) && ins->ans->mode == i_direct)
    {
        INSTRUCTIONLIST *il = tempInfo[ins->ans->offset->v.sp->value.i]->instructionUses;
        tempInfo[ins->ans->offset->v.sp->value.i]->preSSATemp = -1;
        while (il)
        {
            unmarkPreSSA(il->ins);
            il = il->next;
        }
    }
}
static void RewriteAdd(BLOCK *b, int tnum)
{
    RESHAPE_LIST *gather= tempInfo[tnum]->expression.list;
    IMODE *left = tempInfo[tnum]->expression.lastName, *right = NULL;
    int size = tempInfo[tnum]->instructionDefines->ans->size;
    QUAD *ia;
    int flagsl = 0, flagsr;
    if (b == NULL)
    {
        ia = tempInfo[tnum]->instructionDefines;
        if (!ia)
            return;
    }
    else
    {
        if (!blockArray[b->loopParent->entry->idom])
            return;
        ia = blockArray[b->loopParent->entry->idom]->tail;
        ia = beforeJmp(ia, FALSE);
    }
    while (gather && (!b || gather->im->mode == i_immed ||
                 (!variantThisLoop(b, gather->im->offset->v.sp->value.i)
                 &&usedThisLoop(b, gather->im->offset->v.sp->value.i))))
    {
        if (!gather->genned)
        {
            gather->genned = TRUE;
            if (!left && (tempInfo[tnum]->expression.op == i_add || tempInfo[tnum]->expression.op == i_sub))
            {
                if (gather->flags & RF_NEG)
                {
                    /* optimization to avoid generating an extraneous i_neg when possible */
#if 0
                    RESHAPE_LIST **l = &gather->next;
                    while (*l)
                    {
                        if (!((*l)->flags & RF_NEG))
                        {
                            RESHAPE_LIST *t = *l;
                            *l = gather;
                            (*l)->next = t->next;
                            t->next = gather;
                            gather = t;
                        }
                        l = &(*l)->next;
                    }
#endif
                }
            }
            if (!left)
            {
                left = gather->im;
                flagsl = gather->flags;
            }
            else
            {
                right = gather->im;
                flagsr = gather->flags;
            }
            if (left && right)
            {
                left = InsertAddInstruction(b, size, ia, 
                                            flagsl, left, flagsr, right);
                flagsl = 0;
            }
        }
        gather = gather->next;
    }
    tempInfo[tnum]->expression.lastName = left;
    if (left)
        ReplaceHashReshape((QUAD *)left, (UBYTE *)&tempInfo[tnum]->enode->v.sp->imvalue, sizeof(IMODE *), name_hash);
    ia = tempInfo[tnum]->instructionDefines;
    if (ia && left) // && !inductionThisLoop(ia->block, tnum))
    {
        unmarkPreSSA(ia);
        if ((ia->temps & TEMP_ANS) && ia->ans->mode == i_direct)
        {
            tempInfo[ia->ans->offset->v.sp->value.i]->preSSATemp = -1;
        }
        if (ia->dc.left && (ia->temps & TEMP_LEFT))
        {
            RemoveFromUses(ia, ia->dc.left->offset->v.sp->value.i);
        }
        ia->dc.left = left;
        if (ia->dc.left && ia->dc.left->offset->type == en_tempref)
        {
            InsertUses(ia, ia->dc.left->offset->v.sp->value.i);
            ia->temps |= TEMP_LEFT;
        }
        else
            ia->temps &= ~TEMP_LEFT;
        ia->dc.opcode = i_assn;
        ia->temps &= ~TEMP_RIGHT;
    }
}
static IMODE *RewriteDistributed(BLOCK *b, int size, IMODE *im, QUAD *ia, 
                                 RESHAPE_LIST *distrib, int flags)
{
    IMODE *total = distrib->lastDistribName;
    RESHAPE_LIST *gather= distrib;
    int flagsr;
    while (gather && (!b || gather->im->mode == i_immed ||
                 (!variantThisLoop(b, gather->im->offset->v.sp->value.i)
                 &&usedThisLoop(b, gather->im->offset->v.sp->value.i))))
    {
        if (!gather->genned)
        {
            IMODE *left = InsertMulInstruction(b, size, ia, 
                                        flags, im, gather->flags, gather->im);
            gather->genned = TRUE;
            if (!total)
            {
                total = left;
            }
            else
            {
                total = InsertAddInstruction(b, size, ia, 0, total, 0, left);
            }
        }
        gather = gather->next;
    }
    if (gather)
    {
        distrib->lastDistribName = total;
        return NULL;
    }
    return total;
}
static void RewriteMul(BLOCK *b, int tnum)
{
    RESHAPE_LIST *gather= tempInfo[tnum]->expression.list;
    if (gather)
    {
        IMODE *left = tempInfo[tnum]->expression.lastName, *right = NULL;
        int flagsl = 0, flagsr;
        int size = tempInfo[tnum]->instructionDefines->ans->size;
        QUAD *ia;
        RESHAPE_LIST *current = gather;
        if (b == NULL)
        {
            ia = tempInfo[tnum]->instructionDefines;
            if (!ia)
                return;
        }
        else
        {
            if (!blockArray[b->loopParent->entry->idom])
                return;
            ia = blockArray[b->loopParent->entry->idom]->tail;
            ia = beforeJmp(ia, FALSE);
        }
        // should be a branch of some sort at ia....
        while (current && (!b || current->im->mode == i_immed ||
                     (!variantThisLoop(b, current->im->offset->v.sp->value.i)
                     &&usedThisLoop(b, current->im->offset->v.sp->value.i))))
        {
            if (!current->genned && !current->distrib)
            {
                current->genned = TRUE;
                if (!left)
                {
                    left = current->im;
                    flagsl = current->flags;
                }
                else
                {
                    right = current->im;
                    flagsr = current->flags;
                }
                if (left && right)
                {
                    left = InsertMulInstruction(b, size, ia, 
                                                flagsl, left, flagsr, right);
                    flagsl = 0;
                    
                }
            }
            current = current->next;
        }
        tempInfo[tnum]->expression.lastName = left;
        while (gather && (!b || gather->im->mode == i_immed ||
                     (!variantThisLoop(b, gather->im->offset->v.sp->value.i)
                     &&usedThisLoop(b, gather->im->offset->v.sp->value.i))))
        {
            if (gather->distrib && !gather->genned)
            {
                IMODE *im = RewriteDistributed(b, size, gather->im, ia,
                                        gather->distrib, gather->flags);
                if (im)
                {
                    if (tempInfo[tnum]->expression.lastName)
                    {
                        tempInfo[tnum]->expression.lastName
                             = InsertMulInstruction(b, size, ia, 
                                                0, im,
                                                gather->flags, tempInfo[tnum]->expression.lastName);
                        gather->genned = TRUE;
                    }
                    else
                        tempInfo[tnum]->expression.lastName = im;
                }
            }
            gather = gather->next;			
        }
        if (left)
            ReplaceHashReshape((QUAD *)left, (UBYTE *)&tempInfo[tnum]->enode->v.sp->imvalue, sizeof(IMODE *), name_hash);
        ia = tempInfo[tnum]->instructionDefines;
        if (ia && left) // && !inductionThisLoop(ia->block, tnum))
        {
            unmarkPreSSA(ia);
            if (ia->dc.left && (ia->temps & TEMP_LEFT))
            {
                RemoveFromUses(ia, ia->dc.left->offset->v.sp->value.i);
            }
            ia->dc.left = tempInfo[tnum]->expression.lastName;
            if (ia->dc.left && ia->dc.left->offset->type == en_tempref)
            {
                InsertUses(ia, ia->dc.left->offset->v.sp->value.i);
                ia->temps |= TEMP_LEFT;
            }
            else
                ia->temps &= ~TEMP_LEFT;
            ia->dc.opcode = i_assn;
            ia->temps &= ~TEMP_RIGHT;
        }	
    }
}
static void RewriteInvariantExpressions(BLOCK *b)
{
    BLOCKLIST *bl = b->dominates;
    int i;
    DAGLIST *old_ins_hash[DAGSIZE];
    DAGLIST *old_name_hash[DAGSIZE];
    memcpy(old_ins_hash, ins_hash, sizeof(DAGLIST *) * DAGSIZE);
    memcpy(old_name_hash, name_hash, sizeof(DAGLIST *) * DAGSIZE);
    for (i = 0; i < cachedTempCount; i++)
    {
        if (tempInfo[i]->expressionRoot && tempInfo[i]->size < ISZ_FLOAT && tempInfo[i]->instructionDefines->block == b)
        {
            switch(tempInfo[i]->expression.op)
            {
                case i_assn:
                case i_phi:
                    break;
                case i_add:
                case i_sub:
                case i_or:
                case i_eor:
                case i_and:
                    RewriteAdd(b, i);
                    break;
                case i_mul:
                case i_lsl:
                    RewriteMul(b, i);
                    break;
                default:
                    diag("RewriteExpressions: invalid expression type");
                    break;
            }
        }
    }
    while (bl)
    {
        RewriteInvariantExpressions(bl->block);
        bl = bl->next;
    }
    memcpy(ins_hash, old_ins_hash, sizeof(DAGLIST *) * DAGSIZE);
    memcpy(name_hash, old_name_hash, sizeof(DAGLIST *) * DAGSIZE);
}
void RewriteInnerExpressions(void)
{
    int i;
    for (i = 0; i < cachedTempCount; i++)
    {
        if (tempInfo[i]->expressionRoot && tempInfo[i]->size < ISZ_FLOAT)
        {
            switch(tempInfo[i]->expression.op)
            {
                case i_assn:
                case i_phi:
                    break;
                case i_add:
                case i_sub:
                case i_or:
                case i_eor:
                case i_and:
                    RewriteAdd(NULL, i);
                    break;
                case i_mul:
                case i_lsl:
                    RewriteMul(NULL, i);
                    break;
                default:
                    diag("RewriteExpressions: invalid expression type");
                    break;
            }
        }
    }
}
void Reshape(void)
{
    cachedTempCount = tempCount;
    CalculateInduction();
    CreateExpressionLists();
    ApplyDistribution(); /* assumes the lists are already sorted */
    memset(name_hash, 0, sizeof(name_hash));
    memset(ins_hash, 0, sizeof(ins_hash));
    RewriteInvariantExpressions(blockArray[0]);
    RewriteInnerExpressions();
#ifdef XXXXX
    DumpInvariants();
#endif
    tFree();
}
