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
#include "compiler.h"

/* Single static assignment algorithms, see robert morgan, building an optimizing compiler 
 * This isn't too intelligent as it makes a new name for each instance of an assignment,
 * remembering the old name so it can convert back for GCSE.  I gather one would
 * normally make a lookaside table with maybe usedef chains or something instead of this
 * kind of wholesale rewrite of the intermediate code...  but this works ok for me...
 *
 * note that this does some coalescing of temps which isn't strictly necessary
 * because of the coalescing of temps in the iterated register coalescing algorithm.
 * 
 */

extern ARCH_ASM *chosenAssembler; 
extern QUAD *intermed_head;
extern int exitBlock;
extern int tempCount;
extern BLOCK **blockArray;
extern int blockCount;
extern TEMP_INFO **tempInfo;
extern LOOP **loopArray;
extern int loopCount;
extern BRIGGS_SET *globalVars;
extern DAGLIST *ins_hash[DAGSIZE];
extern DAGLIST *name_hash[DAGSIZE];
extern int cachedTempCount;
extern BITINT bittab[BITINTBITS];
extern BOOLEAN functionHasAssembly;
/* don't make this too large, we are stacking a copy... */
static BOOLEAN changed;
static LIST *savedDag;

IMODE *trueName, *falseName;

void SSAInit(void)
{
	savedDag = NULL;
}
/*
 * this is an internal calculation needed to locate where to put PHI nodes
 */
static void DominancePrime(BLOCK *workList[], BRIGGS_SET *dest, BLOCKLIST *src)
{
    int top = 0;
    while (src)
    {
        if (!briggsTest(dest, src->block->blocknum))
        {
            briggsSet(dest, src->block->blocknum);
            workList[top++] = src->block;
        }
        src = src->next;
    }
    while (top)
    {
        BLOCK *b = workList[--top];
        BLOCKLIST *d = b->dominanceFrontier;
        while (d)
        {
            if (!briggsTest(dest, d->block->blocknum))
            {
                briggsSet(dest, d->block->blocknum);
                workList[top++] = d->block;
            }
            d = d->next;
        }
    }
}
/*
 * insert a phi node
 */
static void insertPhiOp(BLOCK *b, int tnum)
{
    QUAD *q = Alloc(sizeof(QUAD)), *I;
    PHIDATA *phi = oAlloc(sizeof(PHIDATA));
    BLOCKLIST *pred = b->pred;
    struct _phiblock **pbhold;
    LIST *list;
    phi->T0 = tnum;
    q->dc.opcode = i_phi;
    q->dc.v.phi = phi;
    
    I = b->head->fwd;
    while (I->dc.opcode == i_label || I->ignoreMe)
    {
        if (I == b->tail)
        {
            I = I->fwd;
            break;
        }
        I = I->fwd;
    }
    
    pbhold = &phi->temps;
    while(pred)
    {
        struct _phiblock *pb = oAlloc(sizeof(struct _phiblock));
        LIST *list;
        phi->nblocks++;
        *pbhold = pb;
        pbhold = &(*pbhold)->next;
        pb->block = pred->block;
        pb->Tn = tnum;
        pred = pred->next;
        tempInfo[tnum]->blockDefines = b;
    }
    InsertInstruction(I->back, q);
    
}
/*
 * go through all the blocks and find out where to put the phi nodes
 */
static void insertPhiNodes(void)
{
    BLOCK **workList = oAlloc(blockCount * sizeof(BLOCK *));
    BRIGGS_SET *bset = briggsAlloc(blockCount);
    BLOCKLIST *entry = oAlloc(sizeof(BLOCKLIST));
    int i, max = tempCount;
    
    entry->block = blockArray[0];
    for (i=0; i < max; i++)
    {
        if (briggsTest(globalVars, i))
        {
            int j;
            entry->next = (BLOCKLIST *)tempInfo[i]->bdefines;
            briggsClear(bset);
            DominancePrime(workList, bset, entry);
            for (j=0; j < bset->top; j++)
            {
                BLOCK *b = blockArray[bset->data[j]];
                if (b)
                {
                    if (isset(b->liveIn, i))
                    {
                        /* avoid phi ops which are essentially assigns */
                        if (b->pred && b->pred->next)
                            insertPhiOp(b, i);
                    }
                }
            }		
        }
    }		
}
/* now if there is a gosub, we have to invalidate global variables that
 * are being cached for CSE.  Because of the way this is done, the
 * variables in question may become cached again later, but, they might
 * not - it depends on the relationship of the links in the rename_hash
 * table to the push of the top level links at the beginning of each block
 */
static IMODE* renameTemp(BLOCK *b, QUAD *head, IMODE *adr)
{
    IMODE *im = NULL;
    int tnum = adr->offset->v.sp->value.i;
    (void)b;
    (void)head;
    if (tempInfo[tnum]->renameStack)
    {
        int n = (int)tempInfo[tnum]->renameStack->data;
        TEMP_INFO *t = tempInfo[n];
        if (adr->mode == i_direct)
        {
            im = t->enode->v.sp->imvalue;
        }
        else if (adr->mode == i_ind)
        {
            if (adr->bits)
            {
                im = Alloc(sizeof(IMODE));
                *im = *adr;
                im->offset = t->enode;
            }
            else
            {
                IMODELIST *iml = t->enode->v.sp->imind;
                while (iml)
                {
                    if (iml->im->size == adr->size)
                    {
                        im = iml->im;
                        break;
                    }
                    iml = iml->next;
                }
                if (!iml)
                {
                    IMODELIST *iml2 = Alloc(sizeof(IMODELIST));
                    im = Alloc(sizeof(IMODE));
                    *im = *adr;
                    im->offset = t->enode;
                    iml2->im = im;
                    iml2->next = t->enode->v.sp->imind;
                    t->enode->v.sp->imind = iml2;
                }
            }
        }
    }	
    return im;
}
static void RemoveName(int i)
{
    IMODE *key = tempInfo[i]->enode->v.sp->imvalue;
    int hashval = dhash((UBYTE *)&key, sizeof(void *));
    DAGLIST *list = name_hash[hashval];
    while (list)
    {
        if (list->key && !memcmp(key, list->key, sizeof(void *)))
        {
            memset(list->key, 0, sizeof(void *));
            list->rv = NULL;
        }
        list = list->next;
    }
}
static DAGLIST *InsertHash(QUAD *rv, UBYTE *key, int size, DAGLIST **table)
{
    int hashval = dhash(key, size);
    DAGLIST *newDag;
    newDag = oAlloc(sizeof(DAGLIST));
    newDag->rv = (UBYTE *)rv;
    newDag->key = key;
    newDag->next =  *table;
    *table = newDag;
    return newDag;
}
DAGLIST *getSavedDAG(void)
{
	if (savedDag)
	{
		DAGLIST **rv = (DAGLIST *)savedDag;
		savedDag = (LIST *)rv[0];
		return rv;
	}
	return (DAGLIST **)Alloc(DAGSIZE * sizeof(DAGLIST *));
}
void releaseSavedDAG(DAGLIST **dag)
{
	dag[0] = (DAGLIST *)savedDag;
	savedDag = (LIST *)dag;
}
/*
 * after the phi nodes are in place, we start renaming all related temps
 * accordingly
 */
static void renameToPhi(BLOCK *b)
{
    QUAD *head = b->head, *tail;
    BLOCKLIST *bl;
    BOOLEAN done = FALSE;
    DAGLIST **saved_ins = getSavedDAG();
    DAGLIST **saved_name = getSavedDAG();
    if (b->visiteddfst)
        return;
    b->visiteddfst = TRUE;
    
    memcpy(saved_ins, ins_hash, sizeof(ins_hash));
    memcpy(saved_name, name_hash, sizeof(name_hash));
    /* go through the code in the block in forward order */
    while (!done)
    {	
        IMODE *im, *found;
        QUAD *q;
        /* these always come first, which satisfies renaming the T0s
         * before doing further processing of the block
         */
        if (head->dc.opcode == i_phi)
        {
            PHIDATA *pd = head->dc.v.phi;
            LIST *list;
            IMODE *rv = InitTempOpt(tempInfo[pd->T0]->enode->v.sp->imvalue->size, 
                        tempInfo[pd->T0]->size);
            int n = rv->offset->v.sp->value.i;
            list = oAlloc(sizeof(LIST));
            list->next = tempInfo[pd->T0]->renameStack;
            list->data = (void *)n;
            tempInfo[pd->T0]->renameStack = list;
            tempInfo[n]->instructionDefines = head;
            tempInfo[n]->blockDefines = b;
            tempInfo[n]->enode->v.sp->pushedtotemp = tempInfo[pd->T0]->enode->v.sp->pushedtotemp;
            tempInfo[n]->enode->v.sp->storeTemp = tempInfo[pd->T0]->enode->v.sp->storeTemp;
            tempInfo[n]->enode->v.sp->loadTemp = tempInfo[pd->T0]->enode->v.sp->loadTemp;
            tempInfo[n]->preSSATemp = pd->T0;
        }
        /* replace Left if necessary */
        if (head->temps & TEMP_LEFT)
        {
            im = renameTemp(b, head, head->dc.left);
            if (im)
            {
                head->dc.left = im;
                if (!im->offset || im->offset->type != en_tempref)
                    head->temps &= ~TEMP_LEFT;
                else
                    head->temps |= TEMP_LEFT;
            }
        }
        /* replace right if necessary */
        if (head->temps & TEMP_RIGHT)
        {
            im = renameTemp(b, head, head->dc.right);
            if (im)
            {
                head->dc.right = im;
                if (!im->offset || im->offset->type != en_tempref)
                    head->temps &= ~TEMP_RIGHT;
                else
                    head->temps |= TEMP_RIGHT;
            }
        }
        /* evaluate the node if both args are constants */
//		if (!(head->temps & (TEMP_LEFT | TEMP_RIGHT)))
//			ConstantFold(head, TRUE);
        if (head->temps & TEMP_ANS)
        {
            int tnum = head->ans->offset->v.sp->value.i;
            if (head->ans->mode == i_ind)
            {
                im = renameTemp(b, head, head->ans);
                if (im)
                {
                    InsertUses(head, im->offset->v.sp->value.i);
                    if (!im->offset || im->offset->type != en_tempref)
                        head->temps &= ~TEMP_ANS;
                    else
                        head->temps |= TEMP_ANS;
                    head->ans = im;
                }
            }
            else if (head->ans->mode == i_direct)
            {
                    /* now we have a tempreg for the answer, if necessary
                     * rename it
                     */
                LIST *list;
                IMODE *rv = InitTempOpt(tempInfo[tnum]->enode->v.sp->imvalue->size, tempInfo[tnum]->size);
//						tempInfo[n]->enode->right = tempInfo[tnum]->enode->right;
                int n = rv->offset->v.sp->value.i;
                rv->vol = head->ans->vol;
                rv->restricted = head->ans->restricted;
                list = oAlloc(sizeof(LIST));
                list->next = tempInfo[tnum]->renameStack;
                list->data = (void *)n;
                tempInfo[tnum]->renameStack = list;
                tempInfo[n]->instructionDefines = head;
                tempInfo[n]->blockDefines = b;
                tempInfo[n]->preSSATemp = tnum;
                tempInfo[n]->enode->v.sp->pushedtotemp = tempInfo[tnum]->enode->v.sp->pushedtotemp;
                tempInfo[n]->enode->v.sp->loadTemp = tempInfo[tnum]->enode->v.sp->loadTemp;
                tempInfo[n]->enode->v.sp->storeTemp = tempInfo[tnum]->enode->v.sp->storeTemp;
            }
        }
        if (head->temps & TEMP_LEFT)
        {
            int tnum = head->dc.left->offset->v.sp->value.i;
            InsertUses(head, tnum);
        }
        if (head->temps & TEMP_RIGHT)
        {
            int tnum = head->dc.right->offset->v.sp->value.i;
            InsertUses(head, tnum);
        }
        if (head == b->tail)
            done = TRUE;
        else
            head = head->fwd;
    }

    /* went through the block the initial time, now rename all the operands
     * to phi nodes in the successor blocks
     */	
    bl = b->succ;
    while (bl) 
    {
        QUAD *q = bl->block->head;
        while ((q->dc.opcode == i_block || q->ignoreMe || q->dc.opcode == i_label) && q->back != bl->block->tail)
            q = q->fwd;
        while (q->dc.opcode == i_phi && q->back != bl->block->tail)
        {
            struct _phiblock *pb = q->dc.v.phi->temps;
            while (pb)
            {
                if (b == pb->block)
                {
                    LIST *t = tempInfo[pb->Tn]->renameStack;
                    if (t)
                    {
                        RemoveFromUses(q, pb->Tn);
                        pb->Tn = (unsigned)t->data;
                        InsertUses(q, pb->Tn);
//						tempInfo[pb->Tn]->preSSATemp = -1;
                    }
                    break;
                }
                pb = pb->next;
            }
            q = q->fwd;
        }
        bl = bl->next;
    }
    
    /* now go through all the blocks we immediately dominate and process them
     */
    bl = b->dominates;
    /* when we get here, we simply process any remaining blocks */
    while (bl)
    {
        renameToPhi(bl->block);
        bl = bl->next;
    }
    /* ok now we've evaluated all the dependencies, go through and finish
     * renaming the answers, and also release anything we have put on the 
     * temporary stacks
     */
    tail = b->tail;
    done = FALSE;
    while (!done)
    {
        /* if it is an i_nop get rid of it
         * these can get generated when branches are optimized away
         */
        if (tail->dc.opcode == i_nop)
        {
            if (tail == b->tail)
                b->tail = tail->back;
            RemoveInstruction(tail);
        }
        else
        {
            /* otherwise process the instruction */
            /* rename answer nodes */
            if (tail->temps & TEMP_ANS)
            {
                if (tail->ans->mode == i_direct)
                {
                    int tnum = tail->ans->offset->v.sp->value.i;
                    if (tempInfo[tnum]->renameStack)
                    {
                        TEMP_INFO *t = tempInfo[(int)tempInfo[tnum]->renameStack->data];
                        IMODE *im;
                        LIST *l;
                        tempInfo[tnum]->renameStack = tempInfo[tnum]->renameStack->next;
                        if (tail->dead)
                        {
                            RemoveInstruction(tail);
                        }
                        else
                        {
                            im = t->enode->v.sp->imvalue;
                            tail->ans = im;
                        }
                    }
                    else
                        diag("rename stack empty");
                }
            }
        }
        if (tail == b->head)
            done = TRUE;
        else
            tail = tail->back;			
    }
    
    {
    /* release the names for the PHI T0 nodes */
        QUAD *q = b->head;
        while (q->back != b->tail &&
               (q->dc.opcode == i_label || q->ignoreMe || q->dc.opcode == i_block))
            q = q->fwd;
        while (q->dc.opcode == i_phi && q->back != b->tail)
        {
            PHIDATA *pd = q->dc.v.phi;
            TEMP_INFO *t = tempInfo[pd->T0];
            if (t->renameStack)
            {
                pd->T0 = (unsigned)t->renameStack->data;
                t->renameStack = t->renameStack->next;
            }
            else
                diag("rename stack for phi node empty");
            q = q->fwd;
        }
    }
    memcpy(ins_hash, saved_ins, sizeof(ins_hash));
    memcpy(name_hash, saved_name, sizeof(name_hash));
	releaseSavedDAG(saved_ins);
	releaseSavedDAG(saved_name);
}
void TranslateToSSA(void)
{
    int i;
    memset(name_hash, 0, sizeof(void*) * DAGSIZE);
    memset(ins_hash, 0, sizeof(void*) * DAGSIZE);
    for (i=0; i < tempCount; i++)
    {
        tempInfo[i]->preSSATemp = -1;
        tempInfo[i]->postSSATemp = -1;
        tempInfo[i]->partition = i;
        tempInfo[i]->instructionDefines = NULL;
        tempInfo[i]->instructionUses = NULL;
        tempInfo[i]->renameStack = NULL;
        tempInfo[i]->elimPredecessors = NULL;
        tempInfo[i]->elimSuccessors = NULL;
    }
    trueName = Alloc(sizeof(QUAD));
    trueName->mode = i_immed;
    trueName->size = -ISZ_UINT;
    trueName->offset = intNode(en_c_i, 1);
    falseName = Alloc(sizeof(QUAD));
    falseName->mode = i_immed;
    falseName->size = -ISZ_UINT;
    falseName->offset = intNode(en_c_i, 0);
    liveVariables();
    insertPhiNodes();
    for (i=0; i < blockCount; i++)
        if (blockArray[i])
            blockArray[i]->visiteddfst = FALSE;
    renameToPhi(blockArray[0]);
    AliasRundown();
    tFree();
}

static void findCopies(BRIGGS_SET *copies, BOOLEAN all)
{
    QUAD *head = intermed_head;
    while (head)
    {	
        if (head->dc.opcode == i_phi)
        {
            PHIDATA *pd = head->dc.v.phi;
            struct _phiblock *pb = pd->temps;
            briggsSet(copies, pd->T0);
            while (pb)
            {
                briggsSet(copies, pb->Tn);
                pb = pb->next;
            }
            
        }
        else if (head->dc.opcode == i_assn)	
        {
            if ((head->temps == (TEMP_LEFT | TEMP_ANS)) 
                && head->dc.left->mode == i_direct && head->ans->mode == i_direct
                && head->ans->size == head->dc.left->size
                && !head->dc.left->retval && (all ||
                    (!head->ans->offset->v.sp->pushedtotemp
                && !head->dc.left->offset->v.sp->pushedtotemp
                && !head->ans->offset->v.sp->storeTemp
                && !head->dc.left->offset->v.sp->storeTemp
                && !head->ans->offset->v.sp->loadTemp
                && !head->dc.left->offset->v.sp->loadTemp)))
            {
                briggsSet(copies, head->ans->offset->v.sp->value.i);
                briggsSet(copies, head->dc.left->offset->v.sp->value.i);
            }
        }
        head = head->fwd;
    }
}
static void mergePartition(int tnew, int told)
{
    if (tnew == told)
        return;
    if (tempInfo[tnew]->enode->v.sp->imvalue && tempInfo[tnew]->enode->v.sp->imvalue->retval)
    {
        int temp = told;
        told = tnew;
        tnew = temp;
    }
    else if (tempInfo[told]->preSSATemp >= 0)
    {
        if (tempInfo[tempInfo[told]->preSSATemp]->enode->v.sp->pushedtotemp)
        {
            int temp = told;
            told = tnew;
            tnew = temp;
        }
    }
    JoinConflictLists(told, tnew);
    tempInfo[told]->partition = tnew;
    changed = TRUE;
}
static void convergeCriticals(void)
{
    int i;
    for (i=exitBlock; i < blockCount; i++)
        if (blockArray[i] && blockArray[i]->critical && blockArray[i]->succ)
        {
            BLOCK *b = blockArray[i]->succ->block;
            BLOCKLIST *bl = b->pred;
            int c = 0;
            QUAD *head = b->head->fwd;
            while (bl->block != blockArray[i])
                bl = bl->next, c++;
            while (head != b->tail->fwd && (head->ignoreMe
                                            || head->dc.opcode == i_label))
                head = head->fwd;
            while (head != b->tail->fwd && head->dc.opcode == i_phi)
            {
                int i = 0;
                PHIDATA *pd = head->dc.v.phi;
                struct _phiblock *pb = pd->temps;
                int TP= findPartition(pd->T0);
                while (i < c)
                    pb = pb->next, i++;
                mergePartition(TP, findPartition(pb->Tn));
                head = head->fwd;
            }
        }
}
static void CheckCoalesce(int T0, int Ti)
{
    int T0p = findPartition(T0), Tip = findPartition(Ti);
    if (T0p != Tip && !isConflicting(T0p, Tip))
    {
        int TP = T0p;
        mergePartition(TP, Tip);
    }
}
/* walks the loop tree to hit inner loops first */
static void CoalesceTemps(LOOP *l, BOOLEAN all)
{
    LIST *p = l->contains;
    while (p)
    {
        LOOP *t = (LOOP *)p->data;
        if (t->type != LT_BLOCK)
            CoalesceTemps(t, all);
        p = p->next;
    }
    p = l->contains;
    while (p)
    {
        LOOP *t = (LOOP *)p->data;
        if (t->type == LT_BLOCK)
        {
            if (!all || (t->entry->blocknum && !t->entry->critical))
            {
                BOOLEAN done = all;
                BLOCKLIST *bl;
                QUAD *head = t->entry->head;
                while (!done)
                {
                    if (head->dc.opcode == i_assn)
                    {
                        if (head->temps == (TEMP_LEFT | TEMP_ANS))
                        {
                            if (head->ans->mode == i_direct && head->dc.left->mode == i_direct)
                            {
                                if (head->ans->size == head->dc.left->size && !head->dc.left->bits)
                                {
                                    if (all)
                                    {
                                        CheckCoalesce(head->ans->offset->v.sp->value.i, head->dc.left->offset->v.sp->value.i);
                                    }
                                    else if (!head->ans->offset->v.sp->pushedtotemp)
                                    {
                                        if (!head->dc.left->offset->v.sp->pushedtotemp)
                                            if (!head->dc.left->offset->v.sp->loadTemp)
                                                if (!head->ans->offset->v.sp->storeTemp)
                                                {
                                                    CheckCoalesce(head->ans->offset->v.sp->value.i, head->dc.left->offset->v.sp->value.i);
                                                }
                                    }
                                }
                            }
                        }
                    }
                    if (head == t->entry->tail)
                        done = TRUE;
                    else
                        head = head->fwd;
                }
                bl = t->entry->succ;
                while (bl)
                {
                    if (!bl->block->critical && !bl->block->unuseThunk)
                    {
                        BLOCKLIST *bb = bl->block->pred;
                        int i = 0;
                        while (bb->block != t->entry)
                        {
                            i++;
                            bb = bb->next;
                        }
                        head = bl->block->head;
                        done = FALSE;
                        while (!done)
                        {
                            if (head->dc.opcode == i_phi)
                            {
                                PHIDATA *pd = head->dc.v.phi;
                                struct _phiblock *pb = pd->temps;
                                int j = i;
                                while (j--)
                                {
                                    pb = pb->next;
                                }
                                CheckCoalesce(pd->T0, pb->Tn);
                            }
                            if (head == bl->block->tail)
                                done = TRUE;
                            else
                                head = head->fwd;
                        }
                    }
                    bl = bl->next;
                }
            }
        }
        p = p->next;
    }
        
}
static void partition(BOOLEAN all)
{
    int i;
    BRIGGS_SET *copied = briggsAlloc(tempCount * 2);
    findCopies(copied, all);
    // each temp was partitioned to itself when we allocated the temp
    // now handle critical edges 
    convergeCriticals();
    changed = TRUE;
    while (changed)
    {
        changed = FALSE;
        CalculateConflictGraph(copied, FALSE);
        CoalesceTemps(loopArray[loopCount-1], all);	
    }
}
static void returnToNormal(IMODE **adr, BOOLEAN all)
{
    IMODE *im = NULL;
    if ((*adr)->offset)
    {
        int tnum = (*adr)->offset->v.sp->value.i, T0p;
        TEMP_INFO *t;
        T0p = findPartition(tnum);
        t = tempInfo[T0p];
        if (t->preSSATemp >= 0 && (!all || tempInfo[t->preSSATemp]->enode->v.sp->pushedtotemp))
        {
            tempInfo[tnum]->postSSATemp = t->postSSATemp = t->preSSATemp;
            tempInfo[t->preSSATemp]->variantLoop = t->variantLoop;
            t = tempInfo[t->preSSATemp];
        }
        else
        {
            tempInfo[tnum]->postSSATemp = t->postSSATemp = T0p;
            t->enode->v.sp->pushedtotemp |= 
                    tempInfo[tnum]->enode->v.sp->pushedtotemp;
            t->enode->v.sp->storeTemp |= 
                    tempInfo[tnum]->enode->v.sp->storeTemp;
            t->enode->v.sp->loadTemp |= 
                    tempInfo[tnum]->enode->v.sp->loadTemp;
            t->enode->v.sp->imvalue-> vol |= 
                    tempInfo[tnum]->enode->v.sp->imvalue->vol;
            t->enode->v.sp->imvalue->restricted |= 
                    tempInfo[tnum]->enode->v.sp->imvalue->restricted;
        }
        t->temp = TRUE;
        if (!(*adr)->offset2 && !(*adr)->offset3)
        {
            if ((*adr)->mode == i_ind)
            {
                if ((*adr)->bits)
                {
                    im = Alloc(sizeof(IMODE));
                    *im = **adr;
                    im->offset = t->enode;
                }
                else
                {
                    IMODELIST *iml = t->enode->v.sp->imind;
                    while (iml)
                    {
                        if (iml->im->size == (*adr)->size)
                        {
                            im = iml->im;
                            break;
                        }
                        iml = iml->next;
                    }
                    if (!iml)
                    {
                        IMODELIST *iml2 = Alloc(sizeof(IMODELIST));
                        im = Alloc(sizeof(IMODE));
                        *im = **adr;
                        im->offset = t->enode;
                        iml2->im = im;
                        iml2->next = t->enode->v.sp->imind;
                        t->enode->v.sp->imind = iml2;
                    }
                }
            }
            else
            {
                im= t->enode->v.sp->imvalue;
            }
        }
        else
        {
            im = Alloc(sizeof(IMODE));
            *im = **adr;
            im->offset = t->enode;
        }
    }
    if ((*adr)->offset2 || (*adr)->offset3)
    {
        if (!im)
        {
            im = Alloc(sizeof(IMODE));
            *im = **adr;
        }
        else
            im->offset3 = (*adr)->offset3;
        if ((*adr)->offset2)
        {
            int tnum = (*adr)->offset2->v.sp->value.i, T0p;
            TEMP_INFO *t;
            T0p = findPartition(tnum);
            t = tempInfo[T0p];
            if (t->preSSATemp >= 0 && (!all || tempInfo[t->preSSATemp]->enode->v.sp->pushedtotemp))
            {
                tempInfo[tnum]->postSSATemp = t->postSSATemp = t->preSSATemp;
                t = tempInfo[t->preSSATemp];
            }
            else
            {
                tempInfo[tnum]->postSSATemp = t->postSSATemp = T0p;
                t->enode->v.sp->pushedtotemp |= 
                        tempInfo[tnum]->enode->v.sp->pushedtotemp;
                t->enode->v.sp->storeTemp |= 
                        tempInfo[tnum]->enode->v.sp->storeTemp;
                t->enode->v.sp->loadTemp |= 
                        tempInfo[tnum]->enode->v.sp->loadTemp;
                t->enode->v.sp->imvalue-> vol |= 
                        tempInfo[tnum]->enode->v.sp->imvalue->vol;
                t->enode->v.sp->imvalue->restricted |= 
                        tempInfo[tnum]->enode->v.sp->imvalue->restricted;
            }
            t->temp = TRUE;
            im->offset2 = t->enode;
        }
    }
    (*adr) = im;
}
static void copyInstruction(BLOCK *blk, int dest, int src, BOOLEAN all)
{
    QUAD *q = Alloc(sizeof(QUAD)), *tail = blk->tail;
    IMODE *destim, *srcim;
    destim = tempInfo[dest]->enode->v.sp->imvalue;
    srcim = tempInfo[src]->enode->v.sp->imvalue;
    tail = beforeJmp(tail, TRUE);
    q->dc.opcode = i_assn;
    q->dc.left = srcim;
    q->ans = destim;
    returnToNormal(&q->ans, all);
    returnToNormal(&q->dc.left, all);
    if (q->ans != q->dc.left)
        InsertInstruction(tail, q);
}
static void BuildAuxGraph(BLOCK *b, int which, BRIGGS_SET *nodes)
{
    QUAD *head = b->head;
    BOOLEAN done = FALSE;
    briggsClear(nodes);
    while (!done)
    {
        if (head->dc.opcode == i_phi)
        {
            PHIDATA *pd = head->dc.v.phi;
            struct _phiblock *pb = pd->temps;
            int T0p = findPartition(pd->T0), Tip;
            int j = which;
            while (j-- )
            {
                pb = pb->next;
            }
            
            Tip = findPartition(pb->Tn);
            if (T0p != Tip)
            {
                LIST *l;
                if (!briggsTest(nodes, T0p))
                {
                    briggsSet(nodes, T0p);
                    tempInfo[T0p]->elimPredecessors = NULL;
                    tempInfo[T0p]->elimSuccessors = NULL;
                }
                if (!briggsTest(nodes, Tip))
                {
                    briggsSet(nodes, Tip);
                    tempInfo[Tip]->elimPredecessors = NULL;
                    tempInfo[Tip]->elimSuccessors = NULL;
                }
                l = oAlloc(sizeof(LIST));
                l->data = (void *)T0p;
                l->next = tempInfo[Tip]->elimPredecessors;
                tempInfo[Tip]->elimPredecessors = l;
                
                l = oAlloc(sizeof(LIST));
                l->data = (void *)Tip;
                l->next =tempInfo[T0p]->elimSuccessors;
                tempInfo[T0p]->elimSuccessors = l;
            }			
        }
        if (head == b->tail)
            done = TRUE;
        else
            head = head->fwd;
    }
    
}
static void ElimForward(BRIGGS_SET *visited, LIST **stack, int T)
{
    LIST *l = tempInfo[T]->elimSuccessors;
    briggsSet(visited, T);
    while (l)
    {
        int n = (int)l->data;
        if (!briggsTest(visited, n))
        {
            ElimForward(visited, stack, n);
        }
        l = l->next;
    }
    l = oAlloc(sizeof(LIST));
    l->data = (void *)T;
    l->next = *stack;
    *stack = l;
}
static BOOLEAN unvisitedPredecessor(BRIGGS_SET *visited, int T)
{
    LIST *l = tempInfo[T]->elimPredecessors;
    while (l)
    {
        if (!briggsTest(visited, (int)l->data))
            return TRUE;
        l = l->next;
    }
    return FALSE;
}
static void ElimBackward(BRIGGS_SET *visited, BLOCK *pred, int T, BOOLEAN all)
{
    LIST *l;
    briggsSet(visited, T);
    l = tempInfo[T]->elimPredecessors;
    while (l)
    {
        int p = (int)l->data;
        if (!briggsTest(visited, p))
        {
            ElimBackward(visited, pred, p, all);
            copyInstruction(pred, p, T, all);
        }
        l = l->next;
    }
}
static void CreateCopy(BRIGGS_SET *visited, BLOCK *pred, int T, BOOLEAN all)
{
    /* we are going to ignore copy instructions involving T
     * if T is not live at the end of the block
     */
//	live = (void *)1;
    if (unvisitedPredecessor(visited, T))
    {
        LIST *l;
        IMODE *rv = InitTempOpt(tempInfo[T]->enode->v.sp->imvalue->size, tempInfo[T]->size);
        int u = rv->offset->v.sp->value.i;
        tempInfo[u]->enode->v.sp->pushedtotemp = tempInfo[T]->enode->v.sp->pushedtotemp;
        tempInfo[u]->enode->v.sp->storeTemp = tempInfo[T]->enode->v.sp->storeTemp;
        tempInfo[u]->enode->v.sp->loadTemp = tempInfo[T]->enode->v.sp->loadTemp;
        copyInstruction(pred, u, T, all);
        l = tempInfo[T]->elimPredecessors;
        while (l)
        {
            int p = (int)l->data;
            if (!briggsTest(visited, p))
            {
                ElimBackward(visited, pred, p, all);
                copyInstruction(pred, p, u, all);
            }
            l = l->next;
        }
    }
    else if (tempInfo[T]->elimSuccessors)
    {
        while (tempInfo[T]->elimSuccessors)
        {
            int u = (int)tempInfo[T]->elimSuccessors->data;
            briggsSet(visited, T);
            tempInfo[T]->elimSuccessors = tempInfo[T]->elimSuccessors->next;
            copyInstruction(pred, T, u, all);
        }
    }
}
static void EliminatePredecessors(BRIGGS_SET *nodes, BLOCK *pred, BLOCK *b, 
                                  int which, BOOLEAN all)
{
    BuildAuxGraph(b, which,  nodes);
    if (nodes->top)
    {
        BRIGGS_SET *visited = briggsAlloc(tempCount * 2);
        LIST *stack = NULL;
        int i;
        for (i=0; i < nodes->top; i++)
        {
            if (!briggsTest(visited, nodes->data[i]))
            {
                ElimForward(visited, &stack, nodes->data[i]);
            }
        }
        briggsClear(visited);
        while (stack)
        {
            int n = (int)stack->data;
            if (!briggsTest(visited, n))
            {
                CreateCopy(visited, pred, n, all);
            }
            stack = stack->next;
        }
    }
}
static void cancelPartition(void)
{
    int i;
    for (i=0; i < tempCount; i++)
    {
        if (tempInfo[i]->preSSATemp < 0)
            tempInfo[tempInfo[i]->partition]->preSSATemp = -1;
    }
}
void TranslateFromSSA(BOOLEAN all)
{
    int i;
    QUAD *head;
    BRIGGS_SET *nodes ;
    for (i=0; i < tempCount; i++)
    {
        tempInfo[i]->temp = FALSE;
        if (all && tempInfo[i]->enode) // && !tempInfo[i]->enode->v.sp->imaddress)
            tempInfo[i]->preSSATemp = i;
//        tempInfo[i]->conflicts = allocbit(tempCount*2);
    }
    liveVariables();
    partition(all);
    //cancelPartition();
    nodes = briggsAlloc(tempCount * 2);
    /* go through the code in the block in forward order */
    for (i=0; i < blockCount; i++)
    {
        BOOLEAN done = FALSE;
        BLOCKLIST *bl;
        int j;
        if (blockArray[i])
        {
            head = blockArray[i]->head;
            /* rename to partition head */
            while (!done)
            {	
                if (head->temps & TEMP_LEFT)
                    returnToNormal(&head->dc.left, all);
                if (head->temps & TEMP_RIGHT)
                    returnToNormal(&head->dc.right, all);
                if (head->temps & TEMP_ANS)
                    returnToNormal(&head->ans, all);
                if (head->dc.opcode == i_assn)
                {
                    /* remove an assignment to self */
                    if (head->temps == (TEMP_LEFT | TEMP_ANS) && head->dc.left->mode == i_direct &&
                        head->ans->mode == i_direct && head->dc.left->size == head->ans->size)
                    {
                        if (head->dc.left->offset->v.sp->value.i == head->ans->offset->v.sp->value.i)
                        {
                            if (head == blockArray[i]->head)
                            {
                                blockArray[i]->head = head->fwd;
                            }
                            if (head == blockArray[i]->tail)
                            {
                                blockArray[i]->tail = head->back;
                                done = TRUE;
                            }
                            RemoveInstruction(head);
                        }
                    }
                }
                if (head == blockArray[i]->tail)
                    done = TRUE;
                else
                    head = head->fwd;
            }
            /* insert copy operations for phi nodes */
            bl = blockArray[i]->pred;
            j = 0;
            while (bl)
            {
                EliminatePredecessors(nodes, bl->block, blockArray[i], j, all);
                j++;
                bl = bl->next;
            }
        }
    }
    // get rid of phi nodes
    for (i=0; i < blockCount; i++)
    {
        BOOLEAN done = FALSE;
        if (blockArray[i])
        {
            head = blockArray[i]->head;
            while (!done)
            {
                if (head->dc.opcode == i_phi)
                {
                    if (head == blockArray[i]->head)
                    {
                        blockArray[i]->head = head->fwd;
                    }
                    if (head == blockArray[i]->tail)
                    {
                        blockArray[i]->tail = head->back;
                        done = TRUE;
                    }
                    RemoveInstruction(head);
                }
                if (head == blockArray[i]->tail)
                    done = TRUE;
                else
                    head = head->fwd;
            }
        }
    }
    for (i=0; i < tempCount; i++)
    {
        tempInfo[i]->inUse = tempInfo[i]->temp;
        if (tempInfo[i]->enode && tempInfo[i]->enode->v.sp->imvalue->retval)
            tempInfo[i]->postSSATemp = i;
        else
            tempInfo[i]->postSSATemp = tempInfo[findPartition(i)]->postSSATemp;
    }
    cFree();
}
