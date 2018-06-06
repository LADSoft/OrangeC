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
 /* we are only doing local opts on temp variables.  At this point,
  * any variable that does not have its address taken is also made a temp
  * variable
  */
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "compiler.h"

extern ARCH_ASM *chosenAssembler;  
extern BITINT bittab[BITINTBITS];
extern int blockCount;
extern BLOCK **blockArray;
extern QUAD *intermed_head;
extern int tempCount;
extern TEMP_INFO **tempInfo;
extern int exitBlock;

int *dfst;
BRIGGS_SET *globalVars;

static BRIGGS_SET *visited ;
static BRIGGS_SET *worklist ;
static BRIGGS_SET *livelist ;
static BOOLEAN hasPhi ;
QUAD *beforeJmp(QUAD *I, BOOLEAN before)
{
    QUAD *start = I;
    while (I->dc.opcode != i_block)
    {
        switch(I->dc.opcode)
        {
            case i_swbranch:
                while (I->dc.opcode == i_swbranch)
                    I = I->back;
                /* I->back is a coswitch, fall through */
            case i_asmcond:
            case i_asmgoto:
            case i_jc:
            case i_ja:
            case i_je:
            case i_jnc:
            case i_jbe:
            case i_jne:
            case i_jl:
            case i_jg:
            case i_jle:
            case i_jge:
            case i_coswitch:
            case i_goto:
                I = I->back;
                if (before)
                    return I;
                return I->fwd;
            default:
                break;
        }
        I = I->back;
    }
//	if (before)
//		return start->back;
    return start;
}
static void liveSetup(void)
{
    BRIGGS_SET *exposed = briggsAllocs(tempCount);
    int i;
    for (i=0; i < blockCount; i++)
    {
        struct _block *blk = blockArray[i];
        if (blk && blk->head)
        {
            QUAD *tail = blk->tail;
            int j;
            briggsClear(exposed);
            do
            {
                if (tail->dc.opcode == i_phi)
                {
                    PHIDATA *pd = tail->dc.v.phi;
                    struct _phiblock *pb = pd->temps;
                    hasPhi = TRUE;
                    briggsReset(exposed, pd->T0);
                    clearbit(blk->liveGen, pd->T0);
                    setbit(blk->liveKills, pd->T0);
                    while (pb)
                    {
                        briggsSet(exposed, pb->Tn);
                        setbit(blk->liveGen, pb->Tn);
                        pb = pb->next;
                    }	
                }
                else
                {
                    if (tail->temps & TEMP_ANS)
                    {
                        if (tail->ans->mode == i_direct)
                        {
                            int tnum = tail->ans->offset->v.sp->value.i;
                            briggsReset(exposed, tnum);
                            clearbit(blk->liveGen, tnum);
                            setbit(blk->liveKills, tnum);
                        }
                        else if (tail->ans->mode == i_ind)
                        {
                            if (tail->ans->offset)
                            {
                                briggsSet(exposed, tail->ans->offset->v.sp->value.i);
                                setbit(blk->liveGen, tail->ans->offset->v.sp->value.i);
                            }
                            if (tail->ans->offset2)
                            {
                                briggsSet(exposed, tail->ans->offset2->v.sp->value.i);
                                setbit(blk->liveGen, tail->ans->offset2->v.sp->value.i);
                            }
                        }
                    }
                    if (tail->temps & TEMP_LEFT)
                        if (tail->dc.left->mode == i_ind || tail->dc.left->mode == i_direct)
                        {
                            if (!tail->dc.left->retval)
                            {
                                if (tail->dc.left->offset)
                                {
                                    briggsSet(exposed, tail->dc.left->offset->v.sp->value.i);
                                    setbit(blk->liveGen, tail->dc.left->offset->v.sp->value.i);
                                }
                                if (tail->dc.left->offset2)
                                {
                                    briggsSet(exposed, tail->dc.left->offset2->v.sp->value.i);
                                    setbit(blk->liveGen, tail->dc.left->offset2->v.sp->value.i);
                                }
                            }
                        }
                    if (tail->temps & TEMP_RIGHT)
                        if (tail->dc.right->mode == i_ind || tail->dc.right->mode == i_direct)
                        {
                            if (tail->dc.right->offset)
                            {
                                briggsSet(exposed, tail->dc.right->offset->v.sp->value.i);
                                setbit(blk->liveGen, tail->dc.right->offset->v.sp->value.i);
                            }
                            if (tail->dc.right->offset2)
                            {
                                briggsSet(exposed, tail->dc.right->offset2->v.sp->value.i);
                                setbit(blk->liveGen, tail->dc.right->offset2->v.sp->value.i);
                            }
                        }
                }
                if (tail != blk->head) /* in case tail == head */
                    tail = tail->back; /* skipping the actual block statement */
            } while (tail != blk->head);
            briggsUnion(globalVars, exposed);
        }
    }
    for (i=0; i < globalVars->top; i++)
    {
        int t = globalVars->data[i], j;
        tempInfo[t]->liveAcrossBlock = TRUE;
    }
}
static void liveOut()
{
    BITINT inWorkList[8192];
    unsigned short *workList = sAlloc((blockCount + 1) * sizeof(unsigned short));
    int i;
    int head = 0, tail = 0;
    int tempDWords = (tempCount + BITINTBITS-1)/BITINTBITS;
    memset(inWorkList, 0, (blockCount + BITINTBITS-1)/BITINTBITS * sizeof(BITINT));
    workList[head++] = exitBlock;
    setbit(inWorkList, exitBlock);
    while (tail != head)
    {
        unsigned n = workList[tail++];
        BLOCKLIST *bl = blockArray[n]->pred;
        while (bl)
        {
            BITINT *b;
            BITINT r;
            n = bl->block->blocknum;
            b = inWorkList + (n/BITINTBITS);
            r = 1 << (n % BITINTBITS);
            if (!(*b & r))
            {
                *b |= r;
                workList[head++] = n;
            }
            bl = bl->next;
        }
    }
    tail = 0;
    while (head != tail)
    {
        BOOLEAN changed = FALSE;
        unsigned n = workList[tail];
        BLOCK *b = blockArray[n];
        BLOCKLIST *bl = b->succ;
        int j;
        BITINT *gen, *kills, *live, *outb;
        if (++tail == blockCount + 1)
            tail = 0;
        clearbit(inWorkList, n);
        memset(b->liveOut, 0 , tempDWords * sizeof(BITINT));
        while (bl)
        {
            live = bl->block->liveIn;
            outb = b->liveOut;
            for (j=0; j < tempDWords; j++)
            {
                if (live[j])
                    outb[j] |= live[j];
            }
            bl = bl->next;
        }
        live = b->liveIn;
        gen = b->liveGen;
        kills = b->liveKills;
        outb = b->liveOut;
        for (j=0; j < tempDWords; j++)
        {
            BITINT c = gen[j] | (outb[j] & ~kills[j]);
            if (changed)
                live[j] = c;
            else if (c != live[j])
            {
                live[j] = c;
                changed = TRUE;
            }
        }
        if (changed)
        {
            bl = b->pred;
            while (bl)
            {
                BITINT *b;
                BITINT r;
                n = bl->block->blocknum;
                b = inWorkList + (n/BITINTBITS);
                r = 1 << (n % BITINTBITS);
                if (!(*b & r))
                {
                    *b |= r;
                    workList[head] = n;
                    if (++head == blockCount + 1)
                        head = 0;
                }
                bl = bl->next;
            }
        }
    }
    /* in the ICD file display live variables at the end of each block */
    if (cparams.prm_icdfile)
    {
        for (i = 0; i < blockCount; i++)
        {
            if (blockArray[i])
            {
                QUAD *t = blockArray[i]->tail->fwd;
                /* this doesn't follow the normal handling rules
                 * so we remove and insert it by hand
                 */
                if (t && t->dc.opcode == i_blockend)
                {
                    t->back->fwd = t->fwd;
                    if (t->fwd)
                        t->fwd->back = t->back;
                }
                    
                if (blockArray[i]->liveOut)
                {
                    QUAD *q = Alloc(sizeof(QUAD));
                    t = blockArray[i]->tail;
                    q->dc.opcode = i_blockend;
                    q->dc.v.data = blockArray[i]->liveOut;
                    q->block = blockArray[i];
                    q->fwd = t->fwd;
                    q->back = t;
                    if (t->fwd)
                        t->fwd->back = q;
                    t->fwd = q;
                }
            }
        }
    }
}
static void killPhiPaths1(void)
{
    int i;
    for (i=0; i < blockCount; i++)
    {
        struct _block *blk = blockArray[i];
        if (blk)
        {
            QUAD *head = blk->head->fwd;
            if (head)
            {
                while (head != blk->tail->fwd && (head->dc.opcode == i_label || 
                       head->ignoreMe))
                {
                    head = head->fwd;
                }
                while (head->dc.opcode == i_phi && head != blk->tail->fwd)
                {
                    PHIDATA *pd = head->dc.v.phi;
                    struct _phiblock *pb = pd->temps;
                    BLOCKLIST *bl = blk->pred;
                    while (pb)
                    {
                        struct _phiblock *pb2 = pd->temps;
                        while (pb2)
                        {
                            if (pb->Tn != pb2->Tn)
                            {
                                setbit(bl->block->liveKills, pb2->Tn);
                            }
                            pb2 = pb2->next;
                        }
                        pb = pb->next;
                        bl = bl->next;
                    }		
                    
                    head = head->fwd;
                }
            }
        }
    }
}
static void killPhiPaths2(void)
{
    int i;
    for (i=0; i < blockCount; i++)
    {
        struct _block *blk = blockArray[i];
        if (blk)
        {
            QUAD *head = blk->head->fwd;
            if (head)
            {
                while (head != blk->tail->fwd && (head->dc.opcode == i_label || 
                       head->ignoreMe))
                {
                    head = head->fwd;
                }
                while (head->dc.opcode == i_phi && head != blk->tail->fwd)
                {
                    PHIDATA *pd = head->dc.v.phi;
                    struct _phiblock *pb = pd->temps;
                    BLOCKLIST *bl = blk->pred;
                    while (pb)
                    {
                        struct _phiblock *pb2 = pd->temps;
                        while (pb2)
                        {
                            clearbit(bl->block->liveOut, pb2->Tn);
                            pb2 = pb2->next;
                        }
                        pb = pb->next;
                        bl = bl->next;
                    }		
                    
                    head = head->fwd;
                }
            }
        }
    }
}
static void markLiveInstruction(BRIGGS_SET *live, QUAD *ins)
{
    switch(ins->dc.opcode)
    {
        case i_parmadj:
        case i_passthrough:
        case i_line:
        case i_varstart:
        case i_dbgblock:
        case i_dbgblockend:
        case i_func:
        case i_trap:
        case i_int:
        case i_ret:
        case i_fret:
        case i_rett:
        case i_cppini:
        case i_block:
        case i_blockend:
        case i_livein:
        case i_prologue:
        case i_epilogue:
        case i_pushcontext:
        case i_popcontext:
        case i_loadcontext:
        case i_unloadcontext:
        case i_tryblock:
        case i_loadstack:
        case i_savestack:
        case i_substack:
        case i_functailstart:
        case i_functailend:
        case i_swbranch:
        case i_skipcompare:
            ins->live = TRUE;
            break;
        case i_parm:
        case i_gosub:
        case i_label:
        case i_goto:
            ins->live = TRUE;
            break;		
        case i_assnblock:
        case i_clrblock:
        case i_parmblock:
            ins->live = TRUE;
            break;
        case i_jne:
        case i_je:
        case i_jc:
        case i_ja:
        case i_jnc:
        case i_jbe:
        case i_jl:
        case i_jg:
        case i_jle:
        case i_jge:
        case i_coswitch:
            ins->live = TRUE;
            break;
        case i_phi:
            if (briggsTest(live, ins->dc.v.phi->T0))
            {
                struct _phiblock *pb = ins->dc.v.phi->temps;
                ins->live = TRUE;
                while (pb)
                {
                    briggsSet(live, pb->Tn);
                    pb = pb->next;
                }
            }
            return;
        default:
            if (!(ins->temps & TEMP_ANS) || ins->ans->mode == i_ind || 
               (chosenAssembler->arch->denyopts & DO_NODEADPUSHTOTEMP) && ins->ans->offset->v.sp->pushedtotemp)
                ins->live = TRUE;
            else if ((ins->temps & TEMP_ANS) && briggsTest(live, ins->ans->offset->v.sp->value.i))
                ins->live = TRUE;

            break;
    }
    if (ins->live)
    {
        if (ins->temps & TEMP_ANS)
        {
            if (ins->ans->mode == i_direct)
            {
                briggsReset(live, ins->ans->offset->v.sp->value.i);
            }
            else if (ins->ans->mode == i_ind)
            {
                if (ins->ans->offset)
                    briggsSet(live, ins->ans->offset->v.sp->value.i);
                if (ins->ans->offset2)
                    briggsSet(live, ins->ans->offset2->v.sp->value.i);
            }
        }
        if (ins->temps & TEMP_LEFT)
        {
            if (ins->dc.left->mode == i_direct || ins->dc.left->mode == i_ind)
            {
                if (ins->dc.left->offset)
                    briggsSet(live, ins->dc.left->offset->v.sp->value.i);
                if (ins->dc.left->offset2)
                    briggsSet(live, ins->dc.left->offset2->v.sp->value.i);
            }
        }
        if (ins->temps & TEMP_RIGHT)
        {
            if (ins->dc.right->mode == i_direct || ins->dc.right->mode == i_ind)
            {
                if (ins->dc.right->offset)
                    briggsSet(live, ins->dc.right->offset->v.sp->value.i);
                if (ins->dc.right->offset2)
                    briggsSet(live, ins->dc.right->offset2->v.sp->value.i);
            }
        }
    }
}
void removeDead(BLOCK *b)
{
    static BRIGGS_SET *live;
    BITINT *p;
    int j,k;
    QUAD *tail;
    BLOCKLIST *bl;
    BOOLEAN done = FALSE;
    if (b == blockArray[0])
    {
        int i;
        liveVariables();
        live = briggsAlloc(tempCount);
        for (i=0; i < blockCount; i++)
            if (blockArray[i])
            {
                QUAD *tail = blockArray[i]->head;
                while (tail != blockArray[i]->tail->fwd)
                {
                    tail->live = tail->alwayslive;
                    tail = tail->fwd;
                }
                blockArray[i]->visiteddfst = FALSE;
            }
    }
    b->visiteddfst = TRUE;
    briggsClear(live);
    p = b->liveOut;
    for (j=0; j < (tempCount + BITINTBITS-1)/BITINTBITS; j++,p++)
        if (*p)
            for (k=0; k < BITINTBITS; k++)
                if (*p & (1 << k))
                {
                    briggsSet(live, j*BITINTBITS + k);
                }
    tail = b->tail;
    while (tail != b->head->back)
    {
        markLiveInstruction(live, tail);
        tail = tail->back;
    }
    bl = b->succ;
    while (bl)
    {
        if (!bl->block->visiteddfst)
            removeDead(bl->block);
        bl = bl->next;
    }
    if (b == blockArray[0])
    {
        QUAD *head = intermed_head;
        BOOLEAN changed = FALSE;
        int i;
        /*
        for (i=0; i < blockCount; i++)
        {
            if (blockArray[i])
                if (blockArray[i]->alwayslive && !blockArray[i]->visiteddfst)
                    removeDead(blockArray[i]);
        }
        */
        for (i=0; i < blockCount; i++)
        {
            BLOCK *b1 = blockArray[i];
            if (b1)
            {
                QUAD *head = b1->head;
                while (head != b1->tail->fwd)
                {
                    if (!head->live)
                    {
                        if (head->dc.opcode != i_block && !head->ignoreMe && head->dc.opcode != i_label)
                        {
                            changed = TRUE;
                            RemoveInstruction(head);
                            if (head->dc.opcode == i_coswitch || (head->dc.opcode >= i_jne && head->dc.opcode <= i_jge))
                            {
                                BLOCKLIST *bl = head->block->succ->next;
                                head->block->succ->next = NULL;
                                while (bl)
                                {
                                    if (bl->block->critical)
                                        UnlinkCritical(bl->block);
                                    bl = bl->next;
                                }
                            }
                        }
                    }
                    head = head->fwd;
                }
            }
        }
        if (changed)
        {
            removeDead(b);
        }
    }
}
void liveVariables(void)
{
    int i;
    sFree();
    hasPhi = FALSE;
    globalVars = briggsAllocs(tempCount);
    worklist = briggsAllocs(blockCount);
    livelist = briggsAllocs(blockCount);
    visited = briggsAllocs(blockCount);
    for (i=0; i < blockCount; i++)
    {
        if (blockArray[i])
        {
            blockArray[i]->liveGen = sallocbit(tempCount);
            blockArray[i]->liveKills = sallocbit(tempCount);
            blockArray[i]-> liveIn = sallocbit(tempCount); 
            blockArray[i]->liveOut = sallocbit(tempCount);
        }
    }
    for (i=0; i <tempCount; i++)
    {
        tempInfo[i]->liveAcrossBlock = FALSE;
    }
    liveSetup();
    if (hasPhi)
        killPhiPaths1();
    liveOut();
    if (hasPhi)
        killPhiPaths2();
}
