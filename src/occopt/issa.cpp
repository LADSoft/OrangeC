/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#include <cstdio>
#include <malloc.h>
#include <cstring>
#include "ioptimizer.h"
#include "beinterfdefs.h"
#include "config.h"
#include "iblock.h"
#include "OptUtils.h"
#include "iloop.h"
#include "ilive.h"
#include "ildata.h"
#include "ioptutil.h"
#include "memory.h"
#include "iconfl.h"
#include "ilocal.h"
#include "ialias.h"
#include "optmodules.h"

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
namespace Optimizer
{

/* don't make this too large, we are stacking a copy... */
static bool changed;
static LIST* savedDag;

IMODE *trueName, *falseName;

void SSAInit(void) { savedDag = nullptr; }
/*
 * this is an internal calculation needed to locate where to put PHI nodes
 */
static void DominancePrime(BLOCK* workList[], BRIGGS_SET* dest, BLOCKLIST* src)
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
        BLOCK* b = workList[--top];
        BLOCKLIST* d = b->dominanceFrontier;
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
static void insertPhiOp(BLOCK* b, int tnum)
{
    QUAD *q = Allocate<QUAD>(), *I;
    PHIDATA* phi = Allocate<PHIDATA>();
    BLOCKLIST* pred = b->pred;
    struct _phiblock** pbhold;
    LIST* list;
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
    while (pred)
    {
        struct _phiblock* pb = Allocate<_phiblock>();
        LIST* list;
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
    BLOCK** workList = oAllocate<BLOCK*>(blockCount);
    BRIGGS_SET* bset = briggsAlloc(blockCount);
    BLOCKLIST* entry = oAllocate<BLOCKLIST>();
    int i, max = tempCount;

    entry->block = blockArray[0];
    for (i = 0; i < max; i++)
    {
        if (briggsTest(globalVars, i))
        {
            int j;
            entry->next = (BLOCKLIST*)tempInfo[i]->bdefines;
            briggsClear(bset);
            DominancePrime(workList, bset, entry);
            for (j = 0; j < bset->top; j++)
            {
                BLOCK* b = blockArray[bset->data[j]];
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
static IMODE* renameTemp(BLOCK* b, QUAD* head, IMODE* adr)
{
    IMODE* im = nullptr;
    int tnum = adr->offset->sp->i;
    (void)b;
    (void)head;
    if (tempInfo[tnum]->renameStack)
    {
        int n = (int)tempInfo[tnum]->renameStack->data;
        TEMP_INFO* t = tempInfo[n];
        if (adr->mode == i_direct)
        {
            im = t->enode->sp->imvalue;
        }
        else if (adr->mode == i_ind)
        {
            if (adr->bits)
            {
                im = Allocate<IMODE>();
                *im = *adr;
                im->offset = t->enode;
            }
            else
            {
                IMODELIST* iml = t->enode->sp->imind;
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
                    IMODELIST* iml2 = Allocate<IMODELIST>();
                    im = Allocate<IMODE>();
                    *im = *adr;
                    im->offset = t->enode;
                    iml2->im = im;
                    iml2->next = t->enode->sp->imind;
                    t->enode->sp->imind = iml2;
                }
            }
        }
    }
    return im;
}
/*
 * after the phi nodes are in place, we start renaming all related temps
 * accordingly
 */
static void renameToPhi(BLOCK* b)
{
    QUAD *head = b->head, *tail;
    BLOCKLIST* bl;
    bool done = false;
    if (b->visiteddfst)
        return;
    b->visiteddfst = true;
    decltype(ins_hash) save_ins(ins_hash);
    decltype(name_hash) save_name(name_hash);

    ins_hash.clear();
    name_hash.clear();

    /* go through the code in the block in forward order */
    while (!done)
    {
        IMODE *im, *found;
        QUAD* q;
        /* these always come first, which satisfies renaming the T0s
         * before doing further processing of the block
         */
        if (head->dc.opcode == i_phi)
        {
            PHIDATA* pd = head->dc.v.phi;
            ILIST* list;
            IMODE* rv = InitTempOpt(tempInfo[pd->T0]->enode->sp->imvalue->size, tempInfo[pd->T0]->size);
            int n = rv->offset->sp->i;
            list = oAllocate<ILIST>();
            list->next = tempInfo[pd->T0]->renameStack;
            list->data = n;
            tempInfo[pd->T0]->renameStack = list;
            tempInfo[n]->instructionDefines = head;
            tempInfo[n]->blockDefines = b;
            tempInfo[n]->enode->sp->pushedtotemp = tempInfo[pd->T0]->enode->sp->pushedtotemp;
            tempInfo[n]->enode->sp->storeTemp = tempInfo[pd->T0]->enode->sp->storeTemp;
            tempInfo[n]->enode->sp->loadTemp = tempInfo[pd->T0]->enode->sp->loadTemp;
            tempInfo[n]->preSSATemp = pd->T0;
        }
        /* replace Left if necessary */
        if (head->temps & TEMP_LEFT)
        {
            im = renameTemp(b, head, head->dc.left);
            if (im)
            {
                head->dc.left = im;
                if (!im->offset || im->offset->type != se_tempref)
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
                if (!im->offset || im->offset->type != se_tempref)
                    head->temps &= ~TEMP_RIGHT;
                else
                    head->temps |= TEMP_RIGHT;
            }
        }
        /* evaluate the node if both args are constants */
        //		if (!(head->temps & (TEMP_LEFT | TEMP_RIGHT)))
        //			ConstantFold(head, true);
        if (head->temps & TEMP_ANS)
        {
            int tnum = head->ans->offset->sp->i;
            if (head->ans->mode == i_ind)
            {
                im = renameTemp(b, head, head->ans);
                if (im)
                {
                    InsertUses(head, im->offset->sp->i);
                    if (!im->offset || im->offset->type != se_tempref)
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
                ILIST* list;
                IMODE* rv = InitTempOpt(tempInfo[tnum]->enode->sp->imvalue->size, tempInfo[tnum]->size);
                //						tempInfo[n]->enode->right = tempInfo[tnum]->enode->right;
                int n = rv->offset->sp->i;
                rv->vol = head->ans->vol;
                rv->restricted = head->ans->restricted;
                list = oAllocate<ILIST>();
                list->next = tempInfo[tnum]->renameStack;
                list->data = n;
                tempInfo[tnum]->renameStack = list;
                tempInfo[n]->instructionDefines = head;
                tempInfo[n]->blockDefines = b;
                tempInfo[n]->preSSATemp = tnum;
                tempInfo[n]->enode->sp->pushedtotemp = tempInfo[tnum]->enode->sp->pushedtotemp;
                tempInfo[n]->enode->sp->loadTemp = tempInfo[tnum]->enode->sp->loadTemp;
                tempInfo[n]->enode->sp->storeTemp = tempInfo[tnum]->enode->sp->storeTemp;
                if (tempInfo[tnum]->enode->sp->tp)
                    tempInfo[n]->enode->sp->tp = tempInfo[tnum]->enode->sp->tp;
            }
        }
        if (head->temps & TEMP_LEFT)
        {
            int tnum = head->dc.left->offset->sp->i;
            InsertUses(head, tnum);
        }
        if (head->temps & TEMP_RIGHT)
        {
            int tnum = head->dc.right->offset->sp->i;
            InsertUses(head, tnum);
        }
        if (head == b->tail)
            done = true;
        else
            head = head->fwd;
    }

    /* went through the block the initial time, now rename all the operands
     * to phi nodes in the successor blocks
     */
    bl = b->succ;
    while (bl)
    {
        QUAD* q = bl->block->head;
        while ((q->dc.opcode == i_block || q->ignoreMe || q->dc.opcode == i_label) && q->back != bl->block->tail)
            q = q->fwd;
        while (q->dc.opcode == i_phi && q->back != bl->block->tail)
        {
            struct _phiblock* pb = q->dc.v.phi->temps;
            while (pb)
            {
                if (b == pb->block)
                {
                    ILIST* t = tempInfo[pb->Tn]->renameStack;
                    if (t)
                    {
                        RemoveFromUses(q, pb->Tn);
                        pb->Tn = t->data;
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
    done = false;
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
                    int tnum = tail->ans->offset->sp->i;
                    if (tempInfo[tnum]->renameStack)
                    {
                        TEMP_INFO* t = tempInfo[(int)tempInfo[tnum]->renameStack->data];
                        IMODE* im;
                        LIST* l;
                        tempInfo[tnum]->renameStack = tempInfo[tnum]->renameStack->next;
                        if (tail->dead)
                        {
                            RemoveInstruction(tail);
                        }
                        else
                        {
                            im = t->enode->sp->imvalue;
                            tail->ans = im;
                        }
                    }
                    else
                        diag("rename stack empty");
                }
            }
        }
        if (tail == b->head)
            done = true;
        else
            tail = tail->back;
    }

    {
        /* release the names for the PHI T0 nodes */
        QUAD* q = b->head;
        while (q->back != b->tail && (q->dc.opcode == i_label || q->ignoreMe || q->dc.opcode == i_block))
            q = q->fwd;
        while (q->dc.opcode == i_phi && q->back != b->tail)
        {
            PHIDATA* pd = q->dc.v.phi;
            TEMP_INFO* t = tempInfo[pd->T0];
            if (t->renameStack)
            {
                pd->T0 = t->renameStack->data;
                t->renameStack = t->renameStack->next;
            }
            else
                diag("rename stack for phi node empty");
            q = q->fwd;
        }
    }
    ins_hash = save_ins;
    name_hash = save_name;
}
void TranslateToSSA(void)
{
    int i;
    ins_hash.clear();
    name_hash.clear();
    for (i = 0; i < tempCount; i++)
    {
        tempInfo[i]->preSSATemp = -1;
        tempInfo[i]->postSSATemp = -1;
        tempInfo[i]->partition = i;
        tempInfo[i]->instructionDefines = nullptr;
        tempInfo[i]->instructionUses = nullptr;
        tempInfo[i]->renameStack = nullptr;
        tempInfo[i]->elimPredecessors = nullptr;
        tempInfo[i]->elimSuccessors = nullptr;
    }
    trueName = Allocate<IMODE>();
    trueName->mode = i_immed;
    trueName->size = -ISZ_UINT;
    trueName->offset = Allocate<SimpleExpression>();
    trueName->offset->type = se_i;
    trueName->offset->i = 1;
    falseName = Allocate<IMODE>();
    falseName->mode = i_immed;
    falseName->size = -ISZ_UINT;
    falseName->offset = Allocate<SimpleExpression>();
    falseName->offset->type = se_i;
    falseName->offset->i = 0;
    liveVariables();
    insertPhiNodes();
    for (i = 0; i < blockCount; i++)
        if (blockArray[i])
            blockArray[i]->visiteddfst = false;
    renameToPhi(blockArray[0]);
    AliasRundown();
    tFree();
}

static void findCopies(BRIGGS_SET* copies, bool all)
{
    QUAD* head = intermed_head;
    while (head)
    {
        if (head->dc.opcode == i_phi)
        {
            PHIDATA* pd = head->dc.v.phi;
            struct _phiblock* pb = pd->temps;
            briggsSet(copies, pd->T0);
            while (pb)
            {
                briggsSet(copies, pb->Tn);
                pb = pb->next;
            }
        }
        else if (head->dc.opcode == i_assn)
        {
            if ((head->temps == (TEMP_LEFT | TEMP_ANS)) && head->dc.left->mode == i_direct && head->ans->mode == i_direct &&
                head->ans->size == head->dc.left->size && !head->dc.left->retval &&
                (all || (!head->ans->offset->sp->pushedtotemp && !head->dc.left->offset->sp->pushedtotemp &&
                         !head->ans->offset->sp->storeTemp && !head->dc.left->offset->sp->storeTemp &&
                         !head->ans->offset->sp->loadTemp && !head->dc.left->offset->sp->loadTemp)))
            {
                briggsSet(copies, head->ans->offset->sp->i);
                briggsSet(copies, head->dc.left->offset->sp->i);
            }
        }
        head = head->fwd;
    }
}
static void mergePartition(int tnew, int told)
{
    if (tnew == told)
        return;
    if (tempInfo[tnew]->enode->sp->imvalue && tempInfo[tnew]->enode->sp->imvalue->retval)
    {
        int temp = told;
        told = tnew;
        tnew = temp;
    }
    else if (tempInfo[told]->preSSATemp >= 0)
    {
        if (tempInfo[tempInfo[told]->preSSATemp]->enode->sp->pushedtotemp)
        {
            int temp = told;
            told = tnew;
            tnew = temp;
        }
    }
    JoinConflictLists(told, tnew);
    tempInfo[told]->partition = tnew;
    changed = true;
}
static void convergeCriticals(void)
{
    int i;
    for (i = exitBlock; i < blockCount; i++)
        if (blockArray[i] && blockArray[i]->critical && blockArray[i]->succ)
        {
            BLOCK* b = blockArray[i]->succ->block;
            BLOCKLIST* bl = b->pred;
            int c = 0;
            QUAD* head = b->head->fwd;
            while (bl->block != blockArray[i])
                bl = bl->next, c++;
            while (head != b->tail->fwd && (head->ignoreMe || head->dc.opcode == i_label))
                head = head->fwd;
            while (head != b->tail->fwd && head->dc.opcode == i_phi)
            {
                int i = 0;
                PHIDATA* pd = head->dc.v.phi;
                struct _phiblock* pb = pd->temps;
                int TP = findPartition(pd->T0);
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
static void CoalesceTemps(LOOP* l, bool all)
{
    LIST* p = l->contains;
    while (p)
    {
        LOOP* t = (LOOP*)p->data;
        if (t->type != LT_BLOCK)
            CoalesceTemps(t, all);
        p = p->next;
    }
    p = l->contains;
    while (p)
    {
        LOOP* t = (LOOP*)p->data;
        if (t->type == LT_BLOCK)
        {
            if (!all || (t->entry->blocknum && !t->entry->critical))
            {
                bool done = all;
                BLOCKLIST* bl;
                QUAD* head = t->entry->head;
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
                                        CheckCoalesce(head->ans->offset->sp->i, head->dc.left->offset->sp->i);
                                    }
                                    else if (!head->ans->offset->sp->pushedtotemp)
                                    {
                                        if (!head->dc.left->offset->sp->pushedtotemp)
                                            if (!head->dc.left->offset->sp->loadTemp)
                                                if (!head->ans->offset->sp->storeTemp)
                                                {
                                                    CheckCoalesce(head->ans->offset->sp->i, head->dc.left->offset->sp->i);
                                                }
                                    }
                                }
                            }
                        }
                    }
                    if (head == t->entry->tail)
                        done = true;
                    else
                        head = head->fwd;
                }
                bl = t->entry->succ;
                while (bl)
                {
                    if (!bl->block->critical && !bl->block->unuseThunk && !bl->block->dead)
                    {
                        BLOCKLIST* bb = bl->block->pred;
                        int i = 0;
                        while (bb->block != t->entry)
                        {
                            i++;
                            bb = bb->next;
                        }
                        head = bl->block->head;
                        done = false;
                        while (!done)
                        {
                            if (head->dc.opcode == i_phi)
                            {
                                PHIDATA* pd = head->dc.v.phi;
                                struct _phiblock* pb = pd->temps;
                                int j = i;
                                while (j--)
                                {
                                    pb = pb->next;
                                }
                                CheckCoalesce(pd->T0, pb->Tn);
                            }
                            if (head == bl->block->tail)
                                done = true;
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
static void partition(bool all)
{
    int i;
    BRIGGS_SET* copied = briggsAlloc(tempCount * 2);
    findCopies(copied, all);
    // each temp was partitioned to itself when we allocated the temp
    // now handle critical edges
    convergeCriticals();
    changed = true;
    while (changed)
    {
        changed = false;
        CalculateConflictGraph(copied, false);
        CoalesceTemps(loopArray[loopCount - 1], all);
    }
}
static void returnToNormal(IMODE** adr, bool all)
{
    IMODE* im = nullptr;
    if ((*adr)->offset)
    {
        int tnum = (*adr)->offset->sp->i, T0p;
        TEMP_INFO* t;
        T0p = findPartition(tnum);
        t = tempInfo[T0p];
        if (t->preSSATemp >= 0 && (!all || tempInfo[t->preSSATemp]->enode->sp->pushedtotemp))
        {
            tempInfo[tnum]->postSSATemp = t->postSSATemp = t->preSSATemp;
            tempInfo[t->preSSATemp]->variantLoop = t->variantLoop;
            t = tempInfo[t->preSSATemp];
        }
        else
        {
            tempInfo[tnum]->postSSATemp = t->postSSATemp = T0p;
            t->enode->sp->pushedtotemp |= tempInfo[tnum]->enode->sp->pushedtotemp;
            t->enode->sp->storeTemp |= tempInfo[tnum]->enode->sp->storeTemp;
            t->enode->sp->loadTemp |= tempInfo[tnum]->enode->sp->loadTemp;
            t->enode->sp->imvalue->vol |= tempInfo[tnum]->enode->sp->imvalue->vol;
            t->enode->sp->imvalue->restricted |= tempInfo[tnum]->enode->sp->imvalue->restricted;
        }
        t->temp = true;
        if (!(*adr)->offset2 && !(*adr)->offset3)
        {
            if ((*adr)->mode == i_ind)
            {
                if ((*adr)->bits)
                {
                    im = Allocate<IMODE>();
                    *im = **adr;
                    im->offset = t->enode;
                }
                else
                {
                    IMODELIST* iml = t->enode->sp->imind;
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
                        IMODELIST* iml2 = Allocate<IMODELIST>();
                        im = Allocate<IMODE>();
                        *im = **adr;
                        im->offset = t->enode;
                        iml2->im = im;
                        iml2->next = t->enode->sp->imind;
                        t->enode->sp->imind = iml2;
                    }
                }
            }
            else
            {
                im = t->enode->sp->imvalue;
            }
        }
        else
        {
            im = Allocate<IMODE>();
            *im = **adr;
            im->offset = t->enode;
        }
    }
    if ((*adr)->offset2 || (*adr)->offset3)
    {
        if (!im)
        {
            im = Allocate<IMODE>();
            *im = **adr;
        }
        else
            im->offset3 = (*adr)->offset3;
        if ((*adr)->offset2)
        {
            int tnum = (*adr)->offset2->sp->i, T0p;
            TEMP_INFO* t;
            T0p = findPartition(tnum);
            t = tempInfo[T0p];
            if (t->preSSATemp >= 0 && (!all || tempInfo[t->preSSATemp]->enode->sp->pushedtotemp))
            {
                tempInfo[tnum]->postSSATemp = t->postSSATemp = t->preSSATemp;
                t = tempInfo[t->preSSATemp];
            }
            else
            {
                tempInfo[tnum]->postSSATemp = t->postSSATemp = T0p;
                t->enode->sp->pushedtotemp |= tempInfo[tnum]->enode->sp->pushedtotemp;
                t->enode->sp->storeTemp |= tempInfo[tnum]->enode->sp->storeTemp;
                t->enode->sp->loadTemp |= tempInfo[tnum]->enode->sp->loadTemp;
                t->enode->sp->imvalue->vol |= tempInfo[tnum]->enode->sp->imvalue->vol;
                t->enode->sp->imvalue->restricted |= tempInfo[tnum]->enode->sp->imvalue->restricted;
            }
            t->temp = true;
            im->offset2 = t->enode;
        }
    }
    (*adr) = im;
}
static void copyInstruction(BLOCK* blk, int dest, int src, bool all)
{
    QUAD *q = Allocate<QUAD>(), *tail = blk->tail;
    IMODE *destim, *srcim;
    destim = tempInfo[dest]->enode->sp->imvalue;
    srcim = tempInfo[src]->enode->sp->imvalue;
    tail = beforeJmp(tail, true);
    q->dc.opcode = i_assn;
    q->dc.left = srcim;
    q->ans = destim;
    returnToNormal(&q->ans, all);
    returnToNormal(&q->dc.left, all);
    if (q->ans != q->dc.left)
        InsertInstruction(tail, q);
}
static void BuildAuxGraph(BLOCK* b, int which, BRIGGS_SET* nodes)
{
    QUAD* head = b->head;
    bool done = false;
    briggsClear(nodes);
    while (!done)
    {
        if (head->dc.opcode == i_phi)
        {
            PHIDATA* pd = head->dc.v.phi;
            struct _phiblock* pb = pd->temps;
            int T0p = findPartition(pd->T0), Tip;
            int j = which;
            while (j--)
            {
                pb = pb->next;
            }

            Tip = findPartition(pb->Tn);
            if (T0p != Tip)
            {
                ILIST* l;
                if (!briggsTest(nodes, T0p))
                {
                    briggsSet(nodes, T0p);
                    tempInfo[T0p]->elimPredecessors = nullptr;
                    tempInfo[T0p]->elimSuccessors = nullptr;
                }
                if (!briggsTest(nodes, Tip))
                {
                    briggsSet(nodes, Tip);
                    tempInfo[Tip]->elimPredecessors = nullptr;
                    tempInfo[Tip]->elimSuccessors = nullptr;
                }
                l = oAllocate<ILIST>();
                l->data = T0p;
                l->next = tempInfo[Tip]->elimPredecessors;
                tempInfo[Tip]->elimPredecessors = l;

                l = oAllocate<ILIST>();
                l->data = Tip;
                l->next = tempInfo[T0p]->elimSuccessors;
                tempInfo[T0p]->elimSuccessors = l;
            }
        }
        if (head == b->tail)
            done = true;
        else
            head = head->fwd;
    }
}
static void ElimForward(BRIGGS_SET* visited, ILIST** stack, int T)
{
    ILIST* l = tempInfo[T]->elimSuccessors;
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
    l = oAllocate<ILIST>();
    l->data = T;
    l->next = *stack;
    *stack = l;
}
static bool unvisitedPredecessor(BRIGGS_SET* visited, int T)
{
    ILIST* l = tempInfo[T]->elimPredecessors;
    while (l)
    {
        if (!briggsTest(visited, l->data))
            return true;
        l = l->next;
    }
    return false;
}
static void ElimBackward(BRIGGS_SET* visited, BLOCK* pred, int T, bool all)
{
    ILIST* l;
    briggsSet(visited, T);
    l = tempInfo[T]->elimPredecessors;
    while (l)
    {
        int p = l->data;
        if (!briggsTest(visited, p))
        {
            ElimBackward(visited, pred, p, all);
            copyInstruction(pred, p, T, all);
        }
        l = l->next;
    }
}
static void CreateCopy(BRIGGS_SET* visited, BLOCK* pred, int T, bool all)
{
    /* we are going to ignore copy instructions involving T
     * if T is not live at the end of the block
     */
    //	live = (void *)1;
    if (unvisitedPredecessor(visited, T))
    {
        ILIST* l;
        IMODE* rv = InitTempOpt(tempInfo[T]->enode->sp->imvalue->size, tempInfo[T]->size);
        int u = rv->offset->sp->i;
        tempInfo[u]->enode->sp->pushedtotemp = tempInfo[T]->enode->sp->pushedtotemp;
        tempInfo[u]->enode->sp->storeTemp = tempInfo[T]->enode->sp->storeTemp;
        tempInfo[u]->enode->sp->loadTemp = tempInfo[T]->enode->sp->loadTemp;
        copyInstruction(pred, u, T, all);
        l = tempInfo[T]->elimPredecessors;
        while (l)
        {
            int p = l->data;
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
            int u = tempInfo[T]->elimSuccessors->data;
            briggsSet(visited, T);
            tempInfo[T]->elimSuccessors = tempInfo[T]->elimSuccessors->next;
            copyInstruction(pred, T, u, all);
        }
    }
}
static void EliminatePredecessors(BRIGGS_SET* nodes, BLOCK* pred, BLOCK* b, int which, bool all)
{
    BuildAuxGraph(b, which, nodes);
    if (nodes->top)
    {
        BRIGGS_SET* visited = briggsAlloc(tempCount * 2);
        ILIST* stack = nullptr;
        int i;
        for (i = 0; i < nodes->top; i++)
        {
            if (!briggsTest(visited, nodes->data[i]))
            {
                ElimForward(visited, &stack, nodes->data[i]);
            }
        }
        briggsClear(visited);
        while (stack)
        {
            int n = stack->data;
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
    for (i = 0; i < tempCount; i++)
    {
        if (tempInfo[i]->preSSATemp < 0)
            tempInfo[tempInfo[i]->partition]->preSSATemp = -1;
    }
}
void TranslateFromSSA(bool all)
{
    if (cparams.icd_flags & (ICD_STAYSSA & ~ICD_QUITEARLY))
        return;
    int i;
    QUAD* head;
    BRIGGS_SET* nodes;
    for (i = 0; i < tempCount; i++)
    {
        tempInfo[i]->temp = false;
        if (all && tempInfo[i]->enode)  // && !tempInfo[i]->enode->sp->imaddress)
            tempInfo[i]->preSSATemp = i;
        //        tempInfo[i]->conflicts = allocbit(tempCount*2);
    }
    liveVariables();
    partition(all);
    // cancelPartition();
    nodes = briggsAlloc(tempCount * 2);
    /* go through the code in the block in forward order */
    for (i = 0; i < blockCount; i++)
    {
        bool done = false;
        BLOCKLIST* bl;
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
                    if (head->temps == (TEMP_LEFT | TEMP_ANS) && head->dc.left->mode == i_direct && head->ans->mode == i_direct &&
                        head->dc.left->size == head->ans->size)
                    {
                        if (head->dc.left->offset->sp->i == head->ans->offset->sp->i)
                        {
                            if (head == blockArray[i]->head)
                            {
                                blockArray[i]->head = head->fwd;
                            }
                            if (head == blockArray[i]->tail)
                            {
                                blockArray[i]->tail = head->back;
                                done = true;
                            }
                            RemoveInstruction(head);
                        }
                    }
                }
                if (head == blockArray[i]->tail)
                    done = true;
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
    for (i = 0; i < blockCount; i++)
    {
        bool done = false;
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
                        done = true;
                    }
                    RemoveInstruction(head);
                }
                if (head == blockArray[i]->tail)
                    done = true;
                else
                    head = head->fwd;
            }
        }
    }
    for (i = 0; i < tempCount; i++)
    {
        tempInfo[i]->inUse = tempInfo[i]->temp;
        if (tempInfo[i]->enode && tempInfo[i]->enode->sp->imvalue->retval)
            tempInfo[i]->postSSATemp = i;
        else
            tempInfo[i]->postSSATemp = tempInfo[findPartition(i)]->postSSATemp;
    }
    cFree();
}
}  // namespace Optimizer