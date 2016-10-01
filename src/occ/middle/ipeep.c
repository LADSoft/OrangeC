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
/*
 * icode peep optimizer
 *
 * Does branch optimizationns of various sorts
 */
#include <stdio.h>
#include "compiler.h"

extern int firstLabel;
extern int nextLabel;
extern TEMP_INFO **tempInfo;
extern int tempCount;
extern BLOCK **blockArray;
extern int blockCount;
extern int exitBlock;
extern BITINT bittab[BITINTBITS];
extern QUAD *intermed_head;
extern SYMBOL *theCurrentFunc;
extern BOOLEAN functionHasAssembly;
extern ARCH_ASM *chosenAssembler;

static BITINT *occursInAbnormal;

static QUAD **golist; /* list of goto statements */

void peepini(void)
{
}

static void scan_gotos(QUAD *head)
/*
 * make a list of goto statements
 */
{
    while (head)
    {
        switch (head->dc.opcode)
        {
            case i_jc:
            case i_jnc:
            case i_jbe:
            case i_ja:
            case i_je:
            case i_jne:
            case i_jge:
            case i_jg:
            case i_jle:
            case i_jl:
                if (head->dc.left->mode == i_immed && head->dc.right->mode == i_immed && !(chosenAssembler->msil))
                {
                    if (isintconst(head->dc.left->offset) && isintconst(head->dc.right->offset))
                    {
                        LLONG_TYPE l = head->dc.left->offset->v.i;
                        LLONG_TYPE r = head->dc.right->offset->v.i;
                        int ok ;
                        switch (head->dc.opcode)
                        {
                            case i_jc:
                                ok = (ULLONG_TYPE)l < (ULLONG_TYPE)r;
                                break ;
                            case i_jnc:
                                ok = (ULLONG_TYPE)l >= (ULLONG_TYPE)r;
                                break ;
                            case i_jbe:
                                ok = (ULLONG_TYPE)l <= (ULLONG_TYPE)r;
                                break ;
                            case i_ja:
                                ok = (ULLONG_TYPE)l > (ULLONG_TYPE)r;
                                break ;
                            case i_je:
                                ok = l == r;
                                break ;
                            case i_jne:
                                ok = l != r;
                                break ;
                            case i_jge:
                                ok = l >= r;
                                break ;
                            case i_jg:
                                ok = l > r;
                                break ;
                            case i_jle:
                                ok = l <= r;
                                break ;
                            case i_jl:
                                ok = l < r;
                                break ;
                            default:
                                break;
                        }
                        if (ok)
                        {
                            head->dc.opcode = i_goto;
                            head->dc.left = head->dc.right = NULL;
                            head->temps &= ~(TEMP_LEFT | TEMP_RIGHT);
                        }
                        else
                            RemoveInstruction(head);
                    }
                }
            case i_swbranch:
            case i_coswitch:
            case i_goto:
                golist[head->dc.v.label - firstLabel] = head;
                break;
            default:
                break;
        }
        head = head->fwd;
    }
}

/*-------------------------------------------------------------------------*/

static void kill_brtonext(BLOCK *b, QUAD *head)
/*
 * branches to the next statement get wiped
 */
{
    QUAD *temp;
    (void)b;
    while (TRUE)
    {
        switch (head->dc.opcode)
        {
            case i_jc:
            case i_jnc:
            case i_jbe:
            case i_ja:
            case i_je:
            case i_jne:
            case i_jge:
            case i_jg:
            case i_jle:
            case i_jl:
                if (chosenAssembler->msil)
                    return;
            case i_goto:
                temp = head->fwd;
                while (temp && (temp->dc.opcode == i_label || temp->ignoreMe 
                    || temp->dc.opcode == i_block || temp->dc.opcode == i_blockend))
                {
                    if (temp->dc.opcode == i_label && temp->dc.v.label == head
                        ->dc.v.label)
                    {
                        RemoveInstruction(head);
                        return;
                    }
                    temp = temp->fwd;
                }
            default:
                return ;
        }
    }
}

static void kill_dupgoto(BLOCK *b, QUAD *head)
{
    (void)b;
    head = head->fwd;
    while (head && head->dc.opcode != i_label)
    {
        if (head->dc.opcode != i_block && head->dc.opcode != i_blockend 
            && head->dc.opcode != i_dbgblock && head->dc.opcode != i_dbgblockend && head->dc.opcode != i_var
            && head->dc.opcode != i_label && !head->ignoreMe)
            RemoveInstruction(head);
        head = head->fwd;
    }
}
/*-------------------------------------------------------------------------*/

void kill_labeledgoto(BLOCK *b, QUAD *head)
/*
 * if any goto goes to a label which is immediately followed by a goto,
 * replaces the original goto label with the new label annd possibly
 * get rid of the labeled goto if it is preceded by another goto
 */
{
    QUAD *newhead;
    BLOCKLIST *bl, **bt;
    int oldlabel = head->dc.v.label;
    (void)b;
    newhead = head->fwd;
    /* look for following goto */
    while (newhead && (newhead->ignoreMe || newhead->dc.opcode ==
        i_block))
        newhead = newhead->fwd;
    if (!newhead)
        return;
    if (newhead->dc.opcode != i_goto)
        return ;
    bt = &newhead->block->pred;
    while (*bt)
    {
        QUAD *tail = (*bt)->block->tail;
        tail = beforeJmp(tail, FALSE);
        switch (tail->dc.opcode)
        {
            case i_goto:
                if (tail->dc.v.label == oldlabel)
                {
                    tail->dc.v.label = newhead->dc.v.label;
                    kill_brtonext(tail->block, tail);
                }
                break;
            case i_jc:
            case i_jnc:
            case i_jbe:
            case i_ja:
            case i_je:
            case i_jne:
            case i_jge:
            case i_jg:
            case i_jle:
            case i_jl:
                 if (tail->dc.v.label == oldlabel)
                {
                    tail->dc.v.label = newhead->dc.v.label;
                    kill_brtonext(tail->block, tail);
                }
                break;
            case i_coswitch:
                do
                {
                    if (tail->dc.v.label == oldlabel)
                    {
                        tail->dc.v.label = newhead->dc.v.label;
                        kill_brtonext(tail->block, tail);
                    }
                    tail = tail->fwd;
                }
                while (tail->dc.opcode == i_swbranch);
                    
                break;
            default:
                break;
        }
        bt = &(*bt)->next;
    }
}

/*-------------------------------------------------------------------------*/

void kill_jumpover(BLOCK *b, QUAD *head)
/*
 * Conditionnal jumps over gotos get squashed here
 */
{
    int newtype;
    QUAD *newhead = head->fwd;
    (void)b;
    while (newhead->dc.opcode == i_block || newhead->dc.opcode == i_blockend || 
           newhead->dc.opcode == i_dbgblock || newhead->dc.opcode == i_dbgblockend ||
           newhead->ignoreMe)
    {
        newhead = newhead->fwd;
    }
    /* if followed by while will be voided by intervening labels */
    if (newhead->dc.opcode == i_goto)
    {
        QUAD *fwd = newhead->fwd;
        while (fwd->dc.opcode == i_block || fwd->dc.opcode == i_blockend ||
               newhead->dc.opcode == i_dbgblock || newhead->dc.opcode == i_dbgblockend ||
               fwd->dc.opcode == i_label || fwd->ignoreMe)
        {
            if (fwd->dc.opcode == i_label && head->dc.v.label == fwd->dc.v.label)
                break;
            fwd = fwd->fwd;
        }
                
        if (fwd->dc.opcode != i_label)
            return;
        /* if it was a goto and we are branching around it, 
         * swap the conditional type and 
         * put the new label in the goto statement 
         */
        head->dc.v.label = newhead->dc.v.label;
        RemoveInstruction(newhead);
        switch (head->dc.opcode)
        {
            case i_jc:
                newtype = i_jnc;
                break;
            case i_jnc:
                newtype = i_jc;
                break;
            case i_jbe:
                newtype = i_ja;
                break;
            case i_ja:
                newtype = i_jbe;
                break;
            case i_je:
                newtype = i_jne;
                break;
            case i_jne:
                newtype = i_je;
                break;
            case i_jge:
                newtype = i_jl;
                break;
            case i_jg:
                newtype = i_jle;
                break;
            case i_jle:
                newtype = i_jg;
                break;
            case i_jl:
                newtype = i_jge;
                break;
            default:
                break;
        }
        /* remove the goto */
        head->dc.opcode = newtype;
        while (1) {
            while (newhead && (newhead->dc.opcode == i_block || newhead->ignoreMe 
                   || newhead->dc.opcode == i_dbgblock || newhead->dc.opcode == i_dbgblockend
                   || newhead->dc.opcode == i_blockend)) {
                newhead = newhead->fwd;
            }
            if (!newhead || newhead->dc.opcode != i_goto)
                break;
            RemoveInstruction(newhead);
            newhead = newhead->fwd;
        }
    }
}
static BOOLEAN kill_noprev(BLOCK *b, QUAD *head)
{
    if (b->pred && !b->pred->next)
    {
        QUAD *tail = b->pred->block->tail;
        tail = beforeJmp(tail, FALSE);
        if (tail->dc.opcode == i_goto)
        {
            if (tail->dc.v.label != head->dc.v.label)
            {
                QUAD *tail2 = b->tail;
                tail2 = beforeJmp(tail2, FALSE);
                if (tail2->dc.opcode == i_goto)
                {
                    kill_dupgoto(b, head);
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}
static int peep_assn(BLOCK *b, QUAD *head)
{
    (void)b;
    if (head->temps == (TEMP_LEFT | TEMP_ANS) && !(head->dc.right) && 
        head->dc.left->size == head->ans->size &&
        head->dc.left->mode == i_direct && head->ans->mode == i_direct && !head->dc.left->bits)
    {
        int t0 = head->ans->offset->v.sp->value.i;
        int t1 = head->dc.left->offset->v.sp->value.i;
        if (!isset(occursInAbnormal, t0) && !isset(occursInAbnormal,t1))
        {
            if (t0 == t1)
            {
                RemoveInstruction(head);			
                return -1;
            }
        }
    }
    if (!(chosenAssembler->arch->denyopts & DO_NOOPTCONVERSION))
    {
        if (head->temps == (TEMP_LEFT | TEMP_ANS) && 
            head->dc.left->size != head->ans->size)
        {
            if (head->back->dc.opcode == i_assn)
            {
                if (head->back->ans->size == head->back->dc.left->size && head->back->ans == head->dc.left)
                    if (head->back->dc.left->mode != i_immed && !head->back->dc.left->bits)
                    {
                        head->dc.left = head->back->dc.left;
                        if ((!head->dc.left->offset || head->dc.left->offset->type != en_tempref)
                            && (!head->dc.left->offset2 || head->dc.left->offset2->type != en_tempref))						
                            head->temps &= ~TEMP_LEFT;
                        return -1;
                    }
            }
        }
    }
    return 0;
}
/*-------------------------------------------------------------------------*/

static BOOLEAN peep(BLOCK *b, BOOLEAN branches)
/*
 * ICODE peep main routine
 */
{
    QUAD *head = b->head;
    BLOCKLIST *bl;
    BOOLEAN changed = FALSE;
    if (b->visiteddfst)
        return changed;
    b->visiteddfst = TRUE;
    while (head != b->tail->fwd)
    {
        int rv = FALSE;
        switch (head->dc.opcode)
        {
            case i_goto:
                if (branches && !functionHasAssembly)
                {
                    kill_dupgoto(b, head);
                    kill_brtonext(b, head);
                }
                break;
            case i_jc:
            case i_jnc:
            case i_jbe:
            case i_ja:
            case i_je:
            case i_jne:
            case i_jge:
            case i_jg:
            case i_jle:
            case i_jl:
                if (branches && !functionHasAssembly)
                {
                       kill_jumpover(b, head);
                    kill_brtonext(b, head);
                }
                break;
            case i_label:
                if (branches)
                {
                    kill_labeledgoto(b, head);
                    if (kill_noprev(b, head))
                        return TRUE;
                }
                break;
            case i_nop: /* just kill it */
                RemoveInstruction(head);
                break;
            case i_assn:
                rv = peep_assn(b, head);
                break;
            default:
                break;
        }
        changed |= !!rv;
        if (rv <=0)
            head = head->fwd;
    }
    return changed;
}
static void scan_abnormal(void)
{
    int i;
    occursInAbnormal = allocbit(tempCount);
    for (i=exitBlock; i < blockCount; i++)
    {
        if (blockArray[i] && blockArray[i]->critical && blockArray[i]->succ)
        {
            BLOCK * b = blockArray[i]->succ->block;
            QUAD *head = b->head->fwd;
            while (head != b->tail->fwd &&
                   (head->ignoreMe || head->dc.opcode == i_label))
                head = head->fwd;
            while (head != b->tail->fwd && head->dc.opcode == i_phi)
            {
                PHIDATA *pd = head->dc.v.phi;
                struct _phiblock *pb = pd->temps;
                while (pb && pb->block != blockArray[i])
                    pb = pb->next;
                setbit(occursInAbnormal,pd->T0);
                setbit(occursInAbnormal,pb->Tn);
                head = head->fwd;
            }
        }
    }
}
void peep_icode(BOOLEAN branches)
{
    int i;
    BOOLEAN changed;
    golist = oAlloc(sizeof(QUAD*)*(nextLabel - firstLabel));
    scan_gotos(intermed_head);
    scan_abnormal();
    for (i=0; i < blockCount; i++)
        if (blockArray[i])
            blockArray[i]->visiteddfst = FALSE;


    do 
    {
        changed = FALSE;
        for (i=0; i < blockCount; i++)
            if (blockArray[i])
                changed |= peep(blockArray[i], branches);
    } while (changed);
}
