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
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <limits.h>
#include "compiler.h"
/* reshaping and loop induction strength reduction */

/* there is some minor problems - if an induction set spans multiple loops
 * then temps will be allocated for each loop but the inner loop temps will
 * be unused
 *
 * note this may sometimes degrade code quality, for example if there is an
 * initializition of an induction variable before a loop and a multiply involving
 * it after the loop, adds will be thrown into the loop but there will additionally
 * be a multiply before the loop to get the initial value.
 */
extern QUAD *intermed_head;
extern int loopCount;
extern LOOP **loopArray;
extern BLOCK **blockArray;
extern int blockCount;
extern DAGLIST *ins_hash[DAGSIZE];
extern TEMP_INFO **tempInfo;
extern int tempCount;

static void ScanVarStrength(INSTRUCTIONLIST *l, IMODE *multiplier, 
                            int tnum, int match, LIST *vars)
{
    while (l)
    {
        IMODE *oldMult = multiplier;
        IMODE *ans = NULL;
        QUAD *head = l->ins;
        switch(head->dc.opcode)
        {
            case i_mul:
                if (head->temps & TEMP_RIGHT)
                {
                    int tr = head->dc.right->offset->v.sp->value.i;
                    if (tr == match)
                    {
                        if (head->dc.left->mode == i_immed)
                        {
                            if (!multiplier)
                            {
                                multiplier = head->dc.left;
                                tr = head->ans->offset->v.sp->value.i;
                                ans = head->ans;
                                ScanVarStrength(tempInfo[tr]->instructionUses,
                                                multiplier, tnum, tr, vars);
                            }
                            else if (multiplier->mode == i_immed)
                            {
                                multiplier = make_immed(multiplier->size, head->dc.left->offset->v.i * multiplier->offset->v.i);
                                tr = head->ans->offset->v.sp->value.i;
                                ans = head->ans;
                                ScanVarStrength(tempInfo[tr]->instructionUses,
                                                multiplier, tnum, tr, vars);
                            }
                        }
                    }
                }
                if (head->temps & TEMP_LEFT)
                {
                    int tr = head->dc.left->offset->v.sp->value.i;
                    if (tr == match)
                    {
                        if (head->dc.right->mode == i_immed)
                        {
                            if (!multiplier)
                            {
                                multiplier = head->dc.right;
                                tr = head->ans->offset->v.sp->value.i;
                                ans = head->ans;
                                ScanVarStrength(tempInfo[tr]->instructionUses,
                                                multiplier, tnum, tr, vars);
                            }
                            else if (multiplier->mode == i_immed)
                            {
                                multiplier = make_immed(multiplier->size, head->dc.right->offset->v.i * multiplier->offset->v.i);
                                tr = head->ans->offset->v.sp->value.i;
                                ans = head->ans;
                                ScanVarStrength(tempInfo[tr]->instructionUses,
                                                multiplier, tnum, tr, vars);
                            }
                        }
                    }
                }
                break;
            case i_lsl:
                if (head->temps & TEMP_LEFT)
                {
                    int tr = head->dc.left->offset->v.sp->value.i;
                    if (tr == match)
                    {
                        if (head->dc.right->mode == i_immed)
                        {
                            // this first if is architecture specific
                            // forces use of shifts in addressing modes
                            if (head->dc.right->offset->v.i > 3)
                            {
                                if (!multiplier)
                                {
                                    multiplier = make_immed(head->dc.right->size, 1 << head->dc.right->offset->v.i);
                                    tr = head->ans->offset->v.sp->value.i;
                                    ans = head->ans;
                                    ScanVarStrength(tempInfo[tr]->instructionUses,
                                                    multiplier, tnum, tr, vars);
                                    break;
                                }
                                else if (multiplier->mode == i_immed)
                                {
                                    multiplier = make_immed(multiplier->size, (1 << head->dc.right->offset->v.i) * multiplier->offset->v.i);
                                    tr = head->ans->offset->v.sp->value.i;
                                    ans = head->ans;
                                    ScanVarStrength(tempInfo[tr]->instructionUses,
                                                    multiplier, tnum, tr, vars);
                                    break;
                                }
                            }
                        }
                    }
                }
                break;
            case i_assn:
                if (head->dc.left->mode == i_direct && head->ans->mode== i_direct)
                {
                    int tr = head->ans->offset->v.sp->value.i;
                    ScanVarStrength(tempInfo[tr]->instructionUses,
                                    multiplier, tnum, tr, vars);
                }
            default:
                break;
        }
        if (multiplier && (ans) && multiplier->size < ISZ_FLOAT && ans->size < ISZ_FLOAT &&
            (multiplier->mode == i_immed || multiplier->offset->type == en_tempref)) // && !tempInfo[multiplier->offset->v.sp->value.i]->markStrength))
        {
            USES_STRENGTH *s = tempInfo[tnum]->sl;
            while (s)
            {
                if (s->multiplier == multiplier)
                    break;
                s = s->next;
            }
            if (!s)
            {
                LIST *v = vars;
                while (v)
                {
                    USES_STRENGTH *s1 = tAlloc(sizeof(USES_STRENGTH));
                    IMODE *rv = InitTempOpt((ans)->size, (ans)->size);
                    int n = rv->offset->v.sp->value.i;
                    s1->next = tempInfo[(int)v->data]->sl;
                    tempInfo[(int)v->data]->sl = s1;
                    s1->multiplier = multiplier;
                    s1->strengthName = n;
                    tempInfo[n]->enode->v.sp->pushedtotemp = TRUE;
                    tempInfo[n]->inductionLoop = tempInfo[(int)v->data]->inductionLoop;
                    tempInfo[n]->oldInductionVar = (int)v->data;
                    v = v->next;
                }
                s = tempInfo[tnum]->sl;
            }
            tempInfo[ans->offset->v.sp->value.i]->strengthRename = s->strengthName;
        }
        multiplier = oldMult;
        l = l->next;
    }
}
static void ScanStrength(void)
{
    int i;
    for (i=0; i < loopCount; i++)
    {
        LOOP *lt = loopArray[i];
        if (lt->type != LT_BLOCK)
        {
            INDUCTION_LIST *sets = lt->inductionSets;
            while (sets)
            {
                LIST *vars = sets->vars;
                int n;
                for (n=0; vars; vars = vars->next, n++);
                vars = sets->vars;
                while (vars)
                {
                    ScanVarStrength(tempInfo[(int)vars->data]->instructionUses, NULL,
                                    (int)vars->data, (int)vars->data, sets->vars);
                    vars = vars->next;
                }
                sets = sets->next;
            }
        }
    }
}
void ReplaceOneUses(QUAD *head, IMODE **im)
{
    int n = tempInfo[(*im)->offset->v.sp->value.i]->strengthRename;
    
    if (n > 0)
    {
        // cancel the SSA backward translation to make the global opts happy
        if ((head->temps & TEMP_ANS) && head->ans->mode == i_direct)
            unmarkPreSSA(head);
        if ((*im)->mode == i_direct)
            *im = tempInfo[n]->enode->v.sp->imvalue;
        else
        {
            IMODELIST *iml = tempInfo[n]->enode->v.sp->imind;
            while (iml)
            {
                if (iml->im->size == (*im)->size)
                {
                    *im = iml->im;
                    break;
                }
                iml = iml->next;
            }
        }
    }
}
void ReplaceStrengthUses(QUAD *head)
{
    if ((head->temps & TEMP_ANS) && head->ans->mode == i_ind)
    {
        ReplaceOneUses(head, &head->ans);
    }
    if (head->temps & TEMP_LEFT)
    {
        ReplaceOneUses(head, &head->dc.left);
    }
    if (head->temps & TEMP_RIGHT)
    {
        ReplaceOneUses(head, &head->dc.right);
    }
}
void ReduceStrengthAssign(QUAD *head)
{
    if ((head->temps & (TEMP_LEFT | TEMP_ANS)) == (TEMP_LEFT | TEMP_ANS))
    {
        if (head->dc.left->mode == i_direct && head->ans->mode == i_direct)
        {
            int ta = head->ans->offset->v.sp->value.i;
            int tl = head->dc.left->offset->v.sp->value.i;
            USES_STRENGTH *sla = tempInfo[ta]->sl;
            USES_STRENGTH *sll = tempInfo[tl]->sl;
            if (sla && sll)
            {
                while (sla && sll)
                {
                    QUAD *ins = Alloc(sizeof(QUAD));
                    ins->dc.opcode = head->dc.opcode;
                    ins->ans = tempInfo[sla->strengthName]->enode->v.sp->imvalue;
                    ins->dc.left = tempInfo[sll->strengthName]->enode->v.sp->imvalue;
                    InsertInstruction(head->back, ins);
                    sla = sla->next;
                    sll = sll->next;
                    
                }
                return;
            }
        }
    }
    ReplaceStrengthUses(head);
}
static IMODE *StrengthConstant(QUAD *head, IMODE * im1, IMODE *im2, int size)
{
    QUAD *ins, q1;
    IMODE *rv;
    if (im1->mode ==  i_immed && isintconst(im1->offset))
        if (im2->mode == i_immed && isintconst(im2->offset))
        {
            return make_immed(ISZ_UINT, im1->offset->v.i * im2->offset->v.i);
        }
    if (im2->offset && im2->offset->type == en_tempref && im2->mode == i_direct)
        while (TRUE)
        {
            QUAD *q = tempInfo[im2->offset->v.sp->value.i]->instructionDefines;
            if (q->dc.opcode != i_assn || q->dc.left->size != q->ans->size || q->dc.left->offset->type != en_tempref)
                break;
            im2 = q->dc.left;
        }
        
    ins = Alloc(sizeof(QUAD));
    if (im2->mode == i_immed)
    {
        IMODE *im = im1;
        im1 = im2 ;
        im2 = im;
    }
    ins->dc.opcode = i_mul;
    if (im1->mode == i_immed)
    {
        if (im1->offset->v.i == 0)
        {
            im2 = im1;
            im1 = NULL;
            ins->dc.opcode = i_assn;
        }
        else
        {
            int n = pwrof2(im1->offset->v.i);
            if (n >= 0)
            {
                ins->dc.opcode = i_lsl;
                im1 = make_immed(im1->size, n);
            }
        }			
    }
    ins->dc.left = im2;
    ins->dc.right = im1;
    ins->ans = InitTempOpt(size, size);
    memset(&q1, 0, sizeof(q1));
    q1.dc.opcode = ins->dc.opcode;
    q1.dc.left = im2;
    q1.dc.right = im1;
    if (q1.dc.left->offset->type == en_tempref && q1.dc.left->mode == i_direct)
    {
        int n = q1.dc.left->offset->v.sp->value.i;
        if (tempInfo[n]->preSSATemp >= 0)
            q1.dc.left = tempInfo[tempInfo[n]->preSSATemp]->enode->v.sp->imvalue;
            
    }
    if (q1.dc.right && q1.dc.right->offset->type == en_tempref && q1.dc.right->mode == i_direct)
    {
        int n = q1.dc.right->offset->v.sp->value.i;
        if (tempInfo[n]->preSSATemp >= 0)
            q1.dc.right = tempInfo[tempInfo[n]->preSSATemp]->enode->v.sp->imvalue;
            
    }
    if (ins->dc.opcode != i_assn)
    {
        rv = (IMODE *)LookupNVHash((UBYTE *)&q1, DAGCOMPARE, ins_hash);
        if (rv)
        {
            int n = rv->offset->v.sp->value.i;
            tempInfo[ins->ans->offset->v.sp->value.i]->preSSATemp = n;
        }
        else
        {
            QUAD *q2 = Alloc(sizeof(QUAD));
            *q2 = q1;
            ReplaceHashReshape((QUAD *)ins->ans, (UBYTE *)q2, DAGCOMPARE, ins_hash);
            tempInfo[ins->ans->offset->v.sp->value.i]->preSSATemp = ins->ans->offset->v.sp->value.i;
        }
    }	
    InsertInstruction(head->back, ins);
    return ins->ans;	
}
static void DoCompare(QUAD *head, IMODE **temp, IMODE **cnst)
{
    IMODE *working = *temp;
    while(!tempInfo[working->offset->v.sp->value.i]->inductionLoop)
    {
        QUAD *l = tempInfo[working->offset->v.sp->value.i]->instructionDefines;
        if (l && l->dc.opcode == i_assn && (l->temps & TEMP_LEFT) && l->dc.left->mode == i_direct)
        {
            working = l->dc.left;
        }
        else
            return;
    }
    if (tempInfo[working->offset->v.sp->value.i]->inductionLoop)
    {
        USES_STRENGTH *sl;
        int n = working->offset->v.sp->value.i;
        
        int il;
//		if (tempInfo[n]->oldInductionVar >= 0)
//			n = tempInfo[n]->oldInductionVar;
        sl = tempInfo[n]->sl;
        // just use first one
        if (sl)
        {
            if (!tempInfo[n]->inductionInitVar)
                *temp = tempInfo[sl->strengthName]->enode->v.sp->imvalue;
            *cnst = StrengthConstant(head, *cnst, sl->multiplier, (*cnst)->size);
        }
    }
}
static QUAD *ReduceStrengthCompare(QUAD *head)
{
    if (head->temps & TEMP_LEFT)
    {
        DoCompare(head, &head->dc.left, &head->dc.right);
        return head;
    }
    if (head->temps & TEMP_RIGHT)
    {
        DoCompare(head, &head->dc.right, &head->dc.left);
        return head;
    }
    ReplaceStrengthUses(head);
    return head;
}
static void ReduceStrengthAdd(QUAD *head)
{
    if ((head->temps & (TEMP_LEFT | TEMP_ANS)) == (TEMP_LEFT | TEMP_ANS))
    {
        if (head->dc.left->mode == i_direct && head->ans->mode == i_direct)
        {
            int ta = head->ans->offset->v.sp->value.i;
            int tl = head->dc.left->offset->v.sp->value.i;
            USES_STRENGTH *sla = tempInfo[ta]->sl;
            USES_STRENGTH *sll = tempInfo[tl]->sl;
            if (sla && sll)
            {
                while (sla && sll)
                {
                    QUAD *ins = Alloc(sizeof(QUAD));
                    ins->dc.opcode = head->dc.opcode;
                    ins->ans = tempInfo[sla->strengthName]->enode->v.sp->imvalue;
                    ins->dc.left = tempInfo[sll->strengthName]->enode->v.sp->imvalue;
                    ins->dc.right = StrengthConstant(head, sla->multiplier, head->dc.right, head->dc.right->size);
                    InsertInstruction(head->back, ins); // /// pulled out// the extra back is an attempt to go past the load
                    sla = sla->next;
                    sll = sll->next;
                    
                }
                return;
            }
        }
    }
    if ((head->temps & (TEMP_RIGHT | TEMP_ANS)) == (TEMP_RIGHT | TEMP_ANS))
    {
        if (head->dc.right->mode == i_direct && head->ans->mode == i_direct)
        {
            int ta = head->ans->offset->v.sp->value.i;
            int tr = head->dc.right->offset->v.sp->value.i;
            USES_STRENGTH *sla = tempInfo[ta]->sl;
            USES_STRENGTH *slr = tempInfo[tr]->sl;
            if (sla && slr)
            {
                while (sla && slr)
                {
                    QUAD *ins = Alloc(sizeof(QUAD));
                    ins->dc.opcode = head->dc.opcode;
                    ins->ans = tempInfo[sla->strengthName]->enode->v.sp->imvalue;
                    ins->dc.right = tempInfo[slr->strengthName]->enode->v.sp->imvalue;
                    ins->dc.left = StrengthConstant(head, sla->multiplier, head->dc.left, head->dc.left->size);
                    InsertInstruction(head->back, ins);
                    sla = sla->next;
                    slr = slr->next;
                    
                }
                return;
            }
        }
    }
    ReplaceStrengthUses(head);
}
static int HandlePhiInitVar(QUAD *insin, USES_STRENGTH *sl, int tnum)
{
    IMODE *rv;
    QUAD *head = tempInfo[tnum]->instructionDefines;
    if (head->dc.opcode == i_assn && (head->dc.left->mode == i_immed || head->dc.left->size == head->ans->size))
        rv = head->dc.left;
    else if (head->dc.opcode == i_phi)
        rv = tempInfo[head->dc.v.phi->T0]->enode->v.sp->imvalue;
    else
        rv = head->ans;
    rv = StrengthConstant(head, rv, sl->multiplier, tempInfo[tnum]->size);
    if (rv->mode == i_immed)
    {
        QUAD *ins = Alloc(sizeof(QUAD));
        int n;
        ins->dc.left = rv;
        ins->ans = InitTempOpt(tempInfo[tnum]->size, tempInfo[tnum]->size);
        ins->dc.opcode = i_assn;
        InsertInstruction(head->back, ins);
        n = ins->ans->offset->v.sp->value.i;
        tempInfo[n]->sl = sl;
        tempInfo[n]->inductionLoop = insin->block->loopParent->loopnum;
        tempInfo[n]->inductionInitVar = 1;
        tempInfo[tnum]->strengthRename = n;
        return ins->ans->offset->v.sp->value.i;
    }
    return rv->offset->v.sp->value.i;
}
static void ReduceStrengthPhi(QUAD *head)
{
    PHIDATA *pd = head->dc.v.phi;
    if (tempInfo[pd->T0]->inductionLoop)
    {
        USES_STRENGTH *sl = tempInfo[pd->T0]->sl;
        int c = 0;
        while (sl)
        {
            struct _phiblock *pb = pd->temps, **newpb;
            QUAD *ins = Alloc(sizeof(QUAD));
            PHIDATA *newpd = Alloc(sizeof(PHIDATA));
            ins->dc.opcode = i_phi;
            ins->dc.v.phi = newpd;
            newpd->T0 = sl->strengthName;
            newpd->nblocks = pd->nblocks;
            newpb = &newpd->temps;
            while (pb)
            {
                USES_STRENGTH *sl1 = tempInfo[pb->Tn]->sl;
                int i;
                *newpb = Alloc(sizeof(struct _phiblock));
                (*newpb)->block = pb->block;
                for (i=0; sl1 && i < c; i++)
                        sl1 = sl1->next;
                if (sl1)
                {
                    (*newpb)->Tn = sl1->strengthName;
                }
                else
                {
                    (*newpb)->Tn = HandlePhiInitVar(head, sl, pb->Tn);
                    tempInfo[(*newpb)->Tn]->inductionLoop = -1; // no invariant code motion
                }
                newpb = &(*newpb)->next;
                pb = pb->next;
            }
            InsertInstruction(head->back, ins);
            c++;
            sl = sl->next;
        }
    }
}
static void ReduceStrength(BLOCK *b)
{
    BLOCKLIST *bl = b->dominates;
    QUAD *head;
//	DAGLIST *temp_hash[DAGSIZE];
//	memcpy(temp_hash, ins_hash, sizeof(ins_hash));	
    head = b->head;
    while (head != b->tail->fwd)
    {
        switch(head->dc.opcode)
        {
            case i_assn:
            case i_neg:
                ReduceStrengthAssign(head);
                break;
            case i_add:
                if (head->dc.right->offset->type == en_structelem)
                    break;
            case i_sub:
            case i_or:
            case i_eor:
            case i_and:
                ReduceStrengthAdd(head);
                break;
            case i_je:
            case i_jne:
            case i_jl:
            case i_jc:
            case i_jg:
            case i_ja:
            case i_jle:
            case i_jbe:
            case i_jge:
            case i_jnc:
                ReduceStrengthCompare(head);
                break;			
            default:
                ReplaceStrengthUses(head);
                break;
        }
        head = head->fwd;
    }
    while (bl)
    {
        ReduceStrength(bl->block);
        bl = bl->next;
    }
//	memcpy(ins_hash, temp_hash, sizeof(ins_hash));	
}
void ReduceLoopStrength(void)
{
    int i;
    memset(ins_hash, 0, sizeof(ins_hash));
    CalculateInduction();
    ScanStrength();
    for (i=0; i < blockCount; i++)
    {
        BLOCK *b = blockArray[i];
        if (b)
        {
            QUAD *head = b->head;
            while ((head->dc.opcode == i_block || head->ignoreMe || head->dc.opcode == i_label) && head->back != b->tail)
                head = head->fwd;
            while (head->dc.opcode == i_phi && head->back != b->tail)
            {
                ReduceStrengthPhi(head);
                head = head->fwd;
            }
        }
    }
    ReduceStrength(blockArray[0]);
    tFree();
}
