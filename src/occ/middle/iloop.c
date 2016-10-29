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

/* Basic loop identity and data gathering
 * note: some optimizations depend on the fact that the loop numbers
 * are assigned in reverse postorder
 */
extern BLOCK **blockArray;
extern int blockCount;
extern int exitBlock;
extern FILE *icdFile;
extern QUAD *criticalThunks , **criticalThunkPtr;
extern TEMP_INFO **tempInfo;
extern int tempCount;
extern QUAD *intermed_tail;

int loopCount;
LOOP **loopArray;

static BRIGGS_SET *loopItems;

static char * lptype(LOOP *lp)
{
    if (lp->type == LT_ROOT)
        return "root";
    else if (lp->type == LT_SINGLE)
        return "single";
    else if (lp->type == LT_MULTI)
        return "multi";
    else
        return "block";
}
static void dump_loops(void)
{
    int i;
    for (i=0; i < loopCount; i++)
    {
        LOOP *lp = loopArray[i];
        if (lp && lp->type != LT_BLOCK)
        {
            LIST *lt = lp->contains;
            fprintf(icdFile, "; %d/%s/B(%d)/", i+1, lptype(lp), lp->entry->blocknum+1);
            while (lt)
            {
                lp = (LOOP *)lt->data;
                if (lp->type == LT_BLOCK)
                    fprintf(icdFile, "B(%d) ", lp->entry->blocknum+1);
                else
                    fprintf(icdFile, "L(%d) ", lp->loopnum+1);
                lt = lt->next;
            }
            fprintf(icdFile, "\n");
        }
    }
}
static void findInfinite(BLOCK *b)
{
    if (!b->visiteddfst)
    {
        BLOCKLIST *l = b->pred;
        b->visiteddfst = TRUE;
        while (l)
        {
            findInfinite(l->block);
            l = l->next;
        }
    }
}
/* add a thunk block to get us out of an infinite loop */
static void makeNonInfinite(BLOCK *b)
{
    BLOCKLIST *bi = newBlock(), *bi2, **bt;
    QUAD *quad, *quad2;

    blockArray[bi->block->blocknum] = bi->block;
    bi->block->unuseThunk = TRUE;

    bi->block->succ = oAlloc(sizeof(BLOCKLIST));
    bi->block->succ->block = blockArray[exitBlock];
    
    bi->block->pred = oAlloc(sizeof(BLOCKLIST));
    bi->block->pred->block = b;
    
    bt = &b->succ;
    while (*bt)
        bt = &(*bt)->next;
    *bt = bi;

    bi2 = oAlloc(sizeof(BLOCKLIST));
    bi2->block = bi->block;
    
    bt = &blockArray[exitBlock]->pred;
    while (*bt)
        bt = &(*bt)->next;
    *bt = bi2;

    /* note : the following does NOT insert jmps for the
     * newly added block
     */
    *criticalThunkPtr = quad = Alloc(sizeof(QUAD));
    criticalThunkPtr = & quad->fwd;
    quad->dc.opcode = i_block;
    quad->block = bi->block;
    quad->dc.v.label = bi->block->blocknum;

    *criticalThunkPtr = quad2 = Alloc(sizeof(QUAD));
    criticalThunkPtr = & quad2->fwd;
    quad2->back = quad;
    quad2->dc.opcode = i_blockend;
    quad2->block = bi->block;
//	quad2->dc.v.label = bi->block->blocknum;
                        
    bi->block->head = quad;
    bi->block->tail = quad2;
    
}
void CancelInfinite(int orgBlockCount)
{
    int i;
    (void)orgBlockCount;
    while (TRUE)
    {
        for (i=0; i < blockCount; i++)
        {
            if (blockArray[i])
                blockArray[i]->visiteddfst = FALSE;
        }
        findInfinite(blockArray[exitBlock]);
        for (i=exitBlock; i >= 0; i--)
        {
            if (blockArray[i] && !blockArray[i]->visiteddfst)
            {
                makeNonInfinite(blockArray[i]);
                break;
            }
        }
        if (i <0)
            break;
    }
}
void RemoveInfiniteThunks(void)
{
    int i;
    for (i=0; i < blockCount; i++)
    {
        BLOCK *b = blockArray[i];
        if (b && b->unuseThunk)
        {
            BLOCKLIST **l;
            if (b->pred)
            {
                l = &b->pred->block->succ;
                while (*l)
                {
                    if ((*l)->block == b)
                    {
                        (*l) = (*l)->next;
                        break;
                    }
                    l = &(*l)->next;
                }
            }
            if (b->succ)
            {
                l = &b->succ->block->pred;
                while (*l)
                {
                    if ((*l)->block == b)
                    {
                        (*l) = (*l)->next;
                        break;
                    }
                    l = &(*l)->next;
                }
            }
            blockArray[i] = NULL;
        }
    }
}
/* may return either a block or loop */
static LOOP *LoopAncestor(BLOCK *b)
{
    LOOP *head = b->loopParent;
    if (!head)
        return b->loopName;
    while (head->parent != NULL)
        head = head->parent;
    return head;
}
static void FindBody(BLOCKLIST *gen, BLOCK *head, enum e_lptype type)
{
    LIST *queue = NULL, **qx;
    LOOP *lp, **lpp ;
    int i;
    if (!gen)
        return;
    briggsClear(loopItems);
    while (gen)
    {
        LOOP *l = LoopAncestor(gen->block);
        if (l && !briggsTest(loopItems, l->loopnum))
        {
            LIST *bl = oAlloc(sizeof(LIST));
            bl->data = l;
            bl->next = queue;
            queue = bl;
            briggsSet(loopItems, l->loopnum);
        }
        gen = gen->next;
    }
    while (queue)
    {
        if (((LOOP *)queue->data)->entry)
        {
            BLOCKLIST *p = ((LOOP *)queue->data)->entry->pred;
            queue = queue->next;
            while (p)
            {
                if (p->block != head)
                {
                    LOOP *l = LoopAncestor(p->block);
                    if (l && !briggsTest(loopItems, l->loopnum))
                    {
                        LIST *bl = oAlloc(sizeof(LIST));
                        bl->data = l;
                        bl->next = queue;
                        queue = bl;
                        briggsSet(loopItems, l->loopnum);
                    }
                }			
                p = p->next;
            }		
        }
        else
            queue = queue->next;
    }
    lp = oAlloc(sizeof(LOOP));
    loopArray[loopCount] = lp;
    lp->type = type;
    lp->loopnum = loopCount++;
    lp->entry = head;
    lp->parent = NULL;
    head->loopParent = lp;
    qx = &lp->contains;
    *qx = oAlloc(sizeof(LIST));
    (*qx)->data = head->loopName;
    qx = &(*qx)->next;
    for (i=0; i < loopItems->top; i++)
    {
        LIST *l = oAlloc(sizeof(LIST));
        int n = loopItems->data[i];
        l->data = loopArray[n];
        *qx = l;
        qx = &(*qx)->next;
        if (loopArray[n]->type == LT_BLOCK)
            loopArray[n]->entry->loopParent = lp;
        else
            loopArray[n]->parent = lp;
    }	
}
static BLOCK *findCommonDominator(BLOCK *one, BLOCK *two)
{
    int i = one->blocknum;
    while (i)
    {
        int j = two->blocknum;
        while (j)
        {
            if (i == j)
                return blockArray[i];
            j = blockArray[j]->idom;
        }
        i = blockArray[i]->idom;
    }
    return blockArray[0];
}
static void FindLoop(BLOCK *b)
{
    BLOCKLIST *loop = NULL;
    BLOCKLIST *bl;
    BLOCK *Z = b;
    bl = b->pred;
    while (bl)
    {
        if (getEdgeType(bl->block->blocknum, b->blocknum) == F_BACKEDGE)
        {
            Z = findCommonDominator(Z, bl->block);
            if (bl->block != b) // we don't care about duplicates, they will be filtered later
            {
                BLOCKLIST *bm = oAlloc(sizeof(BLOCKLIST));
                bm->block = bl->block;
                bm->next = loop;
                loop = bm;
            }
        }
        bl = bl->next;
    }
    if (Z != b)
    {
        /* again we don't care about duplicates */
        BLOCKLIST **bp = &Z->loopGenerators;
        while (*bp)
            bp = &(*bp)->next;
        (*bp) = loop;
    }
    else
    {
        FindBody(loop, b, LT_SINGLE);
    }
}
static void Loop(BLOCK *b)
{
    BLOCKLIST *bl ;
    if (b->visiteddfst)
        return;
    bl = b->succ;
    b->visiteddfst = TRUE;
    while (bl)
    {
        Loop(bl->block);
        bl = bl->next;
    }
    if (b->loopGenerators)
    {
        FindBody(b->loopGenerators, b, LT_MULTI);
    }
    FindLoop(b);
}
static void CalculateLoopedBlocks(LOOP *l)
{
    if (l->type != LT_BLOCK)
    {
        LIST *l1 = l->contains;
        while (l1)
        {
            CalculateLoopedBlocks((LOOP *)l1->data);
            l1= l1->next;
        }
        l1 = l->contains;
        l->blocks = briggsAlloc(blockCount);
        while (l1)
        {
            LOOP *lm = (LOOP *)l1->data;
            if (lm->type == LT_BLOCK)
                briggsSet(l->blocks, lm->entry->blocknum);
            else
            {
                int i;
                for (i=0; i < lm->blocks->top; i++)
                    briggsSet(l->blocks, lm->blocks->data[i]);
            }
            l1 = l1->next;
        }
    }
}
/* finds all the successors to the loop */
static void CalculateSuccessors(LOOP *lp)
{
    LIST *contains;
    contains = lp->contains;
    while(contains)
    {
        LOOP *current = (LOOP *)contains->data;
        if (lp->type != LT_BLOCK)
            CalculateSuccessors(current);
        contains = contains->next;
    }
    contains = lp->contains;
    while (contains)
    {
        LOOP *inner = (LOOP *)contains->data;
        if (inner->type == LT_BLOCK)
        {
            BLOCKLIST *bl = inner->entry->succ;
            while (bl)
            {
                BLOCKLIST *newExit = oAlloc(sizeof(BLOCKLIST));
                newExit->next = lp->successors;
                lp->successors = newExit;
                newExit->block = bl->block;
                bl = bl->next;
            }
        }
        else
        {
            /* only have to add blocks that exited an inner loop
             * and only if they also exit this loop
             */
            BLOCKLIST *prevSuccessors = inner->successors;
            while (prevSuccessors)
            {
                if (!briggsTest(inner->blocks, prevSuccessors->block->blocknum))
                {
                    BLOCKLIST *newExit = oAlloc(sizeof(BLOCKLIST));
                    newExit->next = lp->successors;
                    lp->successors = newExit;
                    newExit->block = prevSuccessors->block;
                }
                prevSuccessors = prevSuccessors->next;
            }
        }
        contains = contains->next;
    }
}
void BuildLoopTree(void)
{
    BLOCKLIST bl;
    BLOCK *b;
    int i;
    QUAD *tail;
    BOOLEAN skip = FALSE;
    /* this is padded, but, in a really really complex program it could get to be too small
     */
    loopArray = oAlloc(sizeof(LOOP *) * blockCount * 4);
    loopItems = briggsAlloc((blockCount) * 4);
    loopCount = 0;
    for (i=0; i < blockCount; i++)
    {
        if (blockArray[i])
        {
            blockArray[i]->visiteddfst = FALSE;
            blockArray[i]->loopParent = NULL;
            blockArray[i]->loopName = oAlloc(sizeof(LOOP));
            blockArray[i]->loopName->type = LT_BLOCK;
            blockArray[i]->loopName->entry = blockArray[i];
            blockArray[i]->loopName->loopnum = loopCount;
            blockArray[i]->loopGenerators = NULL;
            loopArray[loopCount++] = blockArray[i]->loopName;
        }
//		else
//			loopCount++;
    }
    Loop(blockArray[0]);
//	CalculateSuccessors(loopArray[loopCount-1]);
    
    memset(&bl, 0, sizeof(bl));
    bl.block = blockArray[exitBlock];
    FindBody(&bl, blockArray[0], LT_ROOT);
    CalculateLoopedBlocks(loopArray[loopCount-1]);
 //   if (cparams.prm_icdfile)
 //   {
 //       fprintf(icdFile, "; loop dump\n");
 //       dump_loops();
 //   }
    tail = intermed_tail;
    b = tail->block;
    while (tail)
    {
        switch(tail->dc.opcode)
        {
            case i_skipcompare:
                skip = !skip;
                break;
            case i_block:
                b = tail->block;
                break;
            default:
                break;
        }
        if (tail->block)
        {
            if (skip)
            {
                tail->block->inclusiveLoopParent = b->loopParent;
            }
            else
            {
                tail->block->inclusiveLoopParent = tail->block->loopParent;
            }
        }
        tail = tail->back;
    }
    if (loopCount >= blockCount * 4)
        fatal("internal error");
}
BOOLEAN isAncestor(LOOP *l1, LOOP *l2)
{
    if (l1 == l2)
        return FALSE;
    while (l2)
    {
        if (l1 == l2)
            return TRUE;
        l2 = l2->parent;
    }
    return FALSE;
}
static LOOP *nearestAncestor(LOOP *l1, LOOP *l2)
{
    if (!l1)
        return l2;
    if (!l2)
        return l1;
    if (l1 == l2 || isAncestor(l2, l1))
        return l2;
    while (!isAncestor(l1, l2))
    {
        l1 = l1->parent;
    }
    return l1;
}
/* if loop we are interested in encloses loop variable is modified in */
static BOOLEAN isInvariant(int tnum, LOOP *l)
{
    LOOP *varl = tempInfo[tnum]->variantLoop;
    while (varl)
    {
        if (varl == l)
        {
            return FALSE;
        }
        varl = varl->parent;
    }
    return TRUE;
}
static void CalculateLoopInvariants(BLOCK *b)
{
    BLOCKLIST *children = b->dominates;
    QUAD *head;
    int i;
    if (b == blockArray[0])
        for (i=0; i < tempCount; i++)
            tempInfo[i]->variantLoop = 0;
    /* again the PHI nodes are first, which satisfies the algorithm need
     * to evaluate them first
     */
    head = b->head;
    while (head != b->tail->fwd)
    {
        if (head->dc.opcode == i_phi)
        {
            PHIDATA *pd = head->dc.v.phi;
            tempInfo[pd->T0]->variantLoop = b->loopParent;
        }
        else if ((head->temps & TEMP_ANS) && head->ans->mode == i_direct)
        { 
            LOOP *varying = b->loopParent;
            LOOP *t_varying ;
            int tnum;
            if ((head->temps & TEMP_LEFT) && head->dc.left->mode == i_direct)
            {
                tnum = head->dc.left->offset->v.sp->value.i;
                t_varying = nearestAncestor(tempInfo[tnum]->variantLoop, b->loopParent);
                varying = t_varying;
            }
            if ((head->temps & TEMP_RIGHT) && head->dc.right->mode == i_direct)
            {
                tnum = head->dc.right->offset->v.sp->value.i;
                t_varying = nearestAncestor(tempInfo[tnum]->variantLoop, b->loopParent);
                // if t_varying is a more inner level than varying, we need to choose
                // t_varying because we need the innermost loop
                if (isAncestor(varying, t_varying))
                    varying = t_varying;
            }
            tnum = head->ans->offset->v.sp->value.i;
            tempInfo[tnum]->variantLoop = varying;
        }
        head = head->fwd;
    }
    while (children)
    {
        CalculateLoopInvariants(children->block);
        children = children->next;
    }
}
static BRIGGS_SET *candidates;
static unsigned short *inductionCandidateStack;
static int inductionCandidateStackTop;

static void PruneInductionCandidate(int tnum, LOOP *l)
{
    if (tempInfo[tnum]->size >= ISZ_FLOAT)
    {
        briggsReset(candidates, tnum);
    }
    else
    {
        QUAD *I = tempInfo[tnum]->instructionDefines;
        if (I)
        {
            switch(I->dc.opcode)
            {
                case i_phi:
                    {
                        PHIDATA *pd = I->dc.v.phi;
                        struct _phiblock *pb = pd->temps;
                        while (pb)
                        {
                            if (!briggsTest(candidates, pb->Tn) && !isInvariant(pb->Tn, l))
                            {
                                briggsReset(candidates, tnum);
                                break;
                            }
                            pb = pb->next;
                        }
                    }
                    break;
                case i_add:
                case i_sub:
                    if (I->temps & TEMP_RIGHT)
                    {
                        int tn2 = I->dc.right->offset->v.sp->value.i;
                        if (!briggsTest(candidates, tn2))
                            briggsReset(candidates, tnum);
                    }
                    /* FALLTHROUGH */
                case i_neg:
                case i_assn:
                    if (I->temps & TEMP_LEFT)
                    {
                        int tn2 = I->dc.left->offset->v.sp->value.i;
                        if (!briggsTest(candidates, tn2))
                            briggsReset(candidates, tnum);
                    }
                    break;
                default:
                    diag("PruneInductionCandidate: invalid form");
                    break;
            }
        }
        else
        {
            briggsReset(candidates, tnum);
        }
    }
}
/* minor problem: if an induction variable spans multiple loops
 * an induction set will be created for each loop.  These sets will
 * be identical
 */
static void CalculateInductionCandidates(LOOP *l)
{
    LIST *blocks , *p;
    int i;
    briggsClear(candidates);
    inductionCandidateStackTop = 0;

    /* done here just in case for some reason an induction set drills down 
     * into a lower loop 
     */
    for (i=0; i < tempCount; i++)
        tempInfo[i]->onstack = FALSE;
    
    for (i=0; i < l->blocks->top; i++)
    {
        BLOCK *b = blockArray[l->blocks->data[i]];
        if (b)
        {
            QUAD *head = b->head;
            while (head != b->tail->fwd)
            {
                switch(head->dc.opcode)
                {
                    case i_add:
                    case i_sub:
                        if ((head->temps & TEMP_ANS) && head->ans->mode == i_direct)
                        {
                            if (head->dc.left->mode == i_immed || ((head->temps & TEMP_LEFT) && head->dc.left->mode == i_direct))
                                if (head->dc.right->mode == i_immed || ((head->temps & TEMP_RIGHT) && head->dc.right->mode == i_direct))
                                {
                                    if (head->ans->size < ISZ_FLOAT && head->dc.left->size < ISZ_FLOAT && head->dc.right->size < ISZ_FLOAT)
                                    {
                                        int tnum = head->ans->offset->v.sp->value.i;
                                        briggsSet(candidates, tnum);
                                        inductionCandidateStack[inductionCandidateStackTop++] = tnum;
                                        tempInfo[tnum]->onstack = TRUE;
                                    }
                                }
                        }
                        break;
                    case i_neg:
                    case i_assn:
                        if ((head->temps & TEMP_ANS) && head->ans->mode == i_direct)
                        {
                            if (head->dc.left->mode == i_immed || ((head->temps & TEMP_LEFT) && head->dc.left->mode == i_direct))
                            {
                                if (head->ans->size < ISZ_FLOAT && head->dc.left->size < ISZ_FLOAT)
                                {
                                    int tnum = head->ans->offset->v.sp->value.i;
                                    briggsSet(candidates, tnum);
                                    inductionCandidateStack[inductionCandidateStackTop++] = tnum;
                                    tempInfo[tnum]->onstack = TRUE;
                                }
                            }
                        }
                        break;
                    case i_phi:
                        {
                            PHIDATA *pd = head->dc.v.phi;
                            int tnum = pd->T0;
                            if (tempInfo[tnum]->size < ISZ_FLOAT)
                            {
                                briggsSet(candidates, tnum);
                                inductionCandidateStack[inductionCandidateStackTop++] = tnum;
                                tempInfo[tnum]->onstack = TRUE;
                            }
                        }
                        break;
                    default:
                        break;
                }
                /* prep for reshaping */
                switch (head->dc.opcode)
                {
                    case i_add:
                    case i_sub:
                    case i_mul:
                    case i_lsl:
                        if (head->ans->size < ISZ_FLOAT && head->dc.left->size < ISZ_FLOAT)
                        {
                            if (head->temps & TEMP_ANS)
                            {
                                int tnum = head->ans->offset->v.sp->value.i;
                                INSTRUCTIONLIST *l = tempInfo[tnum]->instructionUses;
                                while (l)
                                {
                                    if ((l->ins->temps & TEMP_ANS) && l->ins->ans->mode == i_ind)
                                    {
                                        if (tnum == l->ins->ans->offset->v.sp->value.i)
                                        {
                                            tempInfo[tnum]->expressionRoot = TRUE;
                                        }
                                    }
                                    if (l->ins->temps & TEMP_LEFT)
                                    {
                                        if (tnum == l->ins->dc.left->offset->v.sp->value.i)
                                        {
                                            if (l->ins->dc.left->mode == i_ind)
                                                tempInfo[tnum]->expressionRoot = TRUE;
                                            else if (head->dc.opcode != i_not && head->dc.opcode != i_neg)
                                                if (!matchesop(head->dc.opcode, l->ins->dc.opcode))
                                                    tempInfo[tnum]->expressionRoot = TRUE;
                                        }
                                    }
                                    if (l->ins->temps & TEMP_RIGHT)
                                    {
                                        if (tnum == l->ins->dc.right->offset->v.sp->value.i)
                                        {
                                            if (l->ins->dc.right->mode == i_ind)
                                                tempInfo[tnum]->expressionRoot = TRUE;
                                            else if (head->dc.opcode != i_not && head->dc.opcode != i_neg)
                                                if (!matchesop(head->dc.opcode, l->ins->dc.opcode))
                                                    tempInfo[tnum]->expressionRoot = TRUE;
                                        }
                                    }
                                    l = l->next;
                                }
                            }
                        }
                        break;				
                    default:
                        break;
                }
                head = head->fwd;
            }
        }
    }
    while (inductionCandidateStackTop)
    {
        int tnum = inductionCandidateStack[--inductionCandidateStackTop];
        tempInfo[tnum]->onstack = FALSE;
        PruneInductionCandidate(tnum, l);
        if (!briggsTest(candidates, tnum))
        {
            INSTRUCTIONLIST *li = tempInfo[tnum]->instructionUses;
            while (li)
            {
                if (briggsTest(l->blocks, li->ins->block->blocknum))
                {
                    if (li->ins->temps & TEMP_ANS)
                    {
                        int tnum = li->ins->ans->offset->v.sp->value.i;
                        if (briggsTest(candidates, tnum))
                        {
                            if (!tempInfo[tnum]->onstack)
                                inductionCandidateStack[inductionCandidateStackTop++] = tnum;
                        }
                    }
                }
                li = li->next;
            }
        }
    }
}
static int max_dfs = 0;
static unsigned short *strongStack;
static int strongStackTop;
LIST *strongRegiondfs(int tnum)
{
    TEMP_INFO *t = tempInfo[tnum];
    INSTRUCTIONLIST *u = t->instructionUses;
    LIST *rv = NULL;
    t->temp = t->dfstOrder = max_dfs++;
    strongStack[strongStackTop++] = tnum;
    t->visiteddfst = TRUE;
    t->onstack = TRUE;
    while (u)
    {
        int ux = -1;
        switch (u->ins->dc.opcode)
        {
            case i_phi:
                {
                    PHIDATA *pd = u->ins->dc.v.phi;
                    if (tempInfo[pd->T0]->size < ISZ_FLOAT)
                        ux = pd->T0;
                }
                break;
            case i_add:
            case i_sub:
            case i_assn:
            case i_neg:
                if (u->ins->temps & TEMP_ANS)
                {
                    ux = u->ins->ans->offset->v.sp->value.i;
                    if (tempInfo[ux]->size >= ISZ_FLOAT)
                        ux = -1;
                }
                break;
            default:
                break;
        }
        if (ux >= 0)
        {
            TEMP_INFO *up = tempInfo[ux];
            if (!up->visiteddfst)
            {
                LIST *l3 = strongRegiondfs(ux);
                if (up->temp < t->temp)
                    t->temp = up->temp;
                if (l3)
                {
                    l3->next = rv;
                    rv = l3;
                }
            }
            else if (up->onstack)
            {
                if (up->dfstOrder < t->temp)
                    t->temp = up->dfstOrder;
            }
        }
        u = u->next;
    }
    if (t->temp == t->dfstOrder)
    {
        int vp;
        LIST *region = NULL, *temp;
        do {
            vp = strongStack[--strongStackTop];
            tempInfo[vp]->onstack = FALSE;
            temp = oAlloc(sizeof(LIST));
            temp->data = (void *)vp;
            temp->next = region;
            region = temp;
        } while (vp != tnum);
        temp = oAlloc(sizeof(LIST));
        temp->data = region;
        temp->next = rv;
        rv = temp;
    }
    return rv;
}
/* this returns a two-layer list
 * the top layer is a linked list of strong regions
 * each secondary layer is the induction set for the region
 * first element of list is the anchors
 */
static LIST *strongRegions(LOOP *lp, LIST **anchors)
{
    int i;
    QUAD *head;
    LIST *rv = NULL;
    *anchors = NULL;
    strongStack = oAlloc(sizeof(unsigned short) *tempCount);
    max_dfs = 0;
    strongStackTop = 0;
    for (i=0; i < tempCount; i++)
    {
        tempInfo[i]->dfstOrder = 0;
        tempInfo[i]->visiteddfst = FALSE;
        tempInfo[i]->temp = INT_MAX;
    }
    head = lp->entry->head->fwd;
    while (head != lp->entry->tail->fwd && 
           (head->ignoreMe || head->dc.opcode == i_label))
        head = head->fwd;
    while (head != lp->entry->tail->fwd && head->dc.opcode == i_phi)
    {
        PHIDATA *pd = head->dc.v.phi;
        if (briggsTest(candidates, pd->T0) && tempInfo[pd->T0]->size < ISZ_FLOAT)
        {
            LIST *l1 = strongRegiondfs(pd->T0);
            if (l1)
            {
                l1->next = rv;
                rv = l1;
            }
            l1 = oAlloc(sizeof(LIST));
            l1->data = (void *)pd->T0;
            l1->next = *anchors;
            *anchors = l1;
        }
        head = head->fwd;
    }
    return rv;
}
void CalculateInduction(void)
{
    int i;
    inductionCandidateStack = oAlloc(sizeof(unsigned short) * tempCount);
    
    candidates = briggsAlloc(tempCount);
    CalculateLoopInvariants(blockArray[0]);

    for (i=0; i < loopCount; i++)
    {
        LOOP *lp = loopArray[i];
        if (lp && lp->type == LT_SINGLE)
        {
            LIST *strongTemps;
            LIST *anchors = NULL;
            CalculateInductionCandidates(lp);
            strongTemps = strongRegions(lp, &anchors);
            if (strongTemps && anchors)
            {
                LIST *regions = strongTemps;
                while (regions)
                {
                    LIST *r = (LIST *)regions->data;
                    if (r->next)
                    {
                        LIST *t = anchors;
                        while (t)
                        {
                            while (r)
                            {
                                if (t->data == r->data)
                                    break;
                                r = r->next;
                            }
                            if (!r || t->data == r->data)
                                break;
                            t = t->next;
                        }
                        if (t || r)
                        {
                            INDUCTION_LIST *temp = oAlloc(sizeof(INDUCTION_LIST));
                            LIST *q;
                            temp->vars = regions->data;
                            temp->next = lp->inductionSets;
                            lp->inductionSets = temp;
                            q = regions->data;
                            while (q)
                            {
                                /* the dominator walk will visit inner loops first,
                                 * the outer loops have higher indexes
                                 * we want the outermost loop
                                 */
                                tempInfo[(int)q->data]->inductionLoop = imax(i, tempInfo[(int)q->data]->inductionLoop);
                                q = q->next;
                            }
                        }
                    }
                    regions = regions->next;
                }
            }
        }
    }
}
