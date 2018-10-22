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

#include "compiler.h"

extern BLOCK** blockArray;
extern int blockCount;
extern TEMP_INFO** tempInfo;
extern int tempCount;
extern SYMBOL* theCurrentFunc;
extern BITINT bittab[BITINTBITS];

static int current;
/* this moving of loop invariant expressions is pretty straightforward.
 * we are in SSA form and we do a depth-first visit of all the blocks
 * as we hit each block we label all the temps (non-phi targets)
 * in the block with their defining block.  As we go through we also look
 * at temporary arguments (direct nodes only) and determine if their definitions
 * are in different blocks.  If so, the use is a valid target for loop invariant
 * code motion, and in this case we determine if the source is at the same or a higher
 * nesting level (both enclosed in the same loop subtree).  If it as the same level
 * we don't *have* to move it but we do anyway.  Then we move the calculation to a point
 * right after the original definition
 *
 * one gotcha is that some nodes, e.g. an add, may have two temps and we have to
 * take both into account
 *
 * also most of the time an expression can't be moved without moving an assignment first
 * but left alone the algorithm will move a lot of assignments without having any
 * related expressions to move, then the target of the assignment has to be spilled.
 * this tentatively moves assignments statements by copying them, then after the fact
 * deletes either the moved assignment statement or the original assignment statement
 * based on need.
 *
 * this algorithm will only move expressions involving temps, there may be other
 * candidates for loop invariant code motion but we take the easy way here.
 *
 * (technique borrowed from Scribble and optimized)
 */
static struct reflist
{
    struct reflist* next;
    QUAD* old;
    QUAD* newVal;
} * refs;
static void EnterRef(QUAD* old, QUAD* newVal)
{
    struct reflist* newRef = oAlloc(sizeof(struct reflist));
    newRef->old = old;
    newRef->newVal = newVal;
    newRef->next = refs;
    refs = newRef;
}
static void WeedRefs(void)
{
    while (refs)
    {
        if (refs->newVal->invarKeep)
        {
            refs->old->back->fwd = refs->old->fwd;
            refs->old->fwd->back = refs->old->back;
            if (refs->old == refs->old->block->tail)
                refs->old->block->tail = refs->old->back;
        }
        else
        {
            refs->newVal->back->fwd = refs->newVal->fwd;
            refs->newVal->fwd->back = refs->newVal->back;
            if (refs->newVal == refs->newVal->block->tail)
                refs->newVal->block->tail = refs->newVal->back;
            tempInfo[refs->old->ans->offset->v.sp->value.i]->blockDefines = refs->old->block;
            tempInfo[refs->old->ans->offset->v.sp->value.i]->instructionDefines = refs->old;
        }
        refs = refs->next;
    }
}
static void keep(IMODE* l)
{
    if (l && l->mode == i_direct && l->offset->type == en_tempref)
    {
        QUAD* i = tempInfo[l->offset->v.sp->value.i]->instructionDefines;
        if (i->invarInserted)
        {
            i->invarKeep = TRUE;
            keep(i->dc.left);
            keep(i->dc.right);
        }
    }
}
static BOOLEAN IsAncestor(BLOCK* b1, BLOCK* b2)
{
    BOOLEAN rv = FALSE;
    if (b1)
    {
        LOOP* lb1 = b1->loopParent;
        LOOP* temp = b2->loopParent->parent;
        while (temp && !rv)
        {
            if (temp == lb1)
                rv = TRUE;
            temp = temp->parent;
        }
    }
    return rv;
}
static void MoveTo(BLOCK* dest, BLOCK* src, QUAD* head)
{
    QUAD* insert = beforeJmp(dest->tail, TRUE);
    QUAD* head2 = Alloc(sizeof(QUAD));
    *head2 = *head;
    EnterRef(head, head2);
    head = head2;
    if (insert == dest->tail)
        dest->tail = head;
    head->back = insert;
    head->fwd = insert->fwd;
    insert->fwd = insert->fwd->back = head;
    tempInfo[head->ans->offset->v.sp->value.i]->blockDefines = dest;
    tempInfo[head->ans->offset->v.sp->value.i]->instructionDefines = head;
    head->block = dest;
    head->invarInserted = TRUE;
    if (head->dc.opcode != i_assn || head->dc.left->mode == i_immed)
    {
        keep(head->dc.left);
        keep(head->dc.right);
        head->invarKeep = TRUE;
    }
}
static void MoveExpression(BLOCK* b, QUAD* head, BLOCK* pbl, BLOCK* pbr)
{
    if (IsAncestor(pbl, b))
    {
        if (pbr)
        {
            if (IsAncestor(pbr, b))
            {
                if (pbr->preWalk > pbl->preWalk)
                {
                    MoveTo(pbr, b, head);
                }
                else
                {
                    MoveTo(pbl, b, head);
                }
            }
        }
        else if (pbl->preWalk < b->preWalk)
        {
            MoveTo(pbl, b, head);
        }
    }
}
static BOOLEAN isPhiUsing(LOOP* considering, int temp)
{
    BOOLEAN rv = FALSE;
    if (temp != -1 && considering)
    {
        rv = isset(considering->invariantPhiList, temp);
    }
    return rv;
}
static BOOLEAN InvariantPhiUsing(QUAD* head)
{
    int ans = head->ans->offset->v.sp->value.i;
    BOOLEAN rv = FALSE;
    int left = -1, right = -1;
    LOOP* considering = head->block->loopParent;
    if (tempInfo[ans]->preSSATemp >= 0)
        rv = !tempInfo[tempInfo[ans]->preSSATemp]->enode->v.sp->pushedtotemp;
    if (head->temps & TEMP_LEFT)
        left = head->dc.left->offset->v.sp->value.i;
    if (head->temps & TEMP_RIGHT)
        right = head->dc.right->offset->v.sp->value.i;
    while (considering && !considering->invariantPhiList)
    {
        considering = considering->parent;
    }
    if (considering)
    {
        rv &= !isPhiUsing(considering, ans) && !isPhiUsing(considering, left) && !isPhiUsing(considering, right);
    }
    return rv;
}
void ScanForInvariants(BLOCK* b)
{
    BLOCKLIST* bl = b->succ;
    QUAD* head = b->head;
    b->preWalk = current++;

    while (head != b->tail->fwd)
    {
        QUAD* next = head->fwd;
        if (!head->ignoreMe && head->dc.opcode != i_label && head->dc.opcode != i_assnblock)
        {
            if (head->dc.opcode == i_phi)
            {

                LOOP* parent = head->block->inclusiveLoopParent;
                struct _phiblock* pb;
                if (!parent->invariantPhiList)
                {
                    LOOP* last = parent->parent;
                    while (last && !last->invariantPhiList)
                        last = last->parent;

                    parent->invariantPhiList = allocbit(tempCount);
                    if (last)
                        memcpy(bits(parent->invariantPhiList), bits(last->invariantPhiList),
                               ((tempCount) + (BITINTBITS - 1)) / BITINTBITS * sizeof(BITINT));
                }
                pb = head->dc.v.phi->temps;
                while (pb)
                {
                    setbit(parent->invariantPhiList, pb->Tn);
                    pb = pb->next;
                }
                setbit(parent->invariantPhiList, head->dc.v.phi->T0);
            }
            else if ((head->temps & TEMP_ANS) && head->ans->mode == i_direct && head->ans->size < ISZ_FLOAT)
            {
                tempInfo[head->ans->offset->v.sp->value.i]->blockDefines = b;
                if (!tempInfo[head->ans->offset->v.sp->value.i]->inductionLoop && (head->temps & (TEMP_LEFT | TEMP_RIGHT)))
                {
                    BOOLEAN canMove = TRUE;
                    BLOCK *pbl = NULL, *pbr = NULL;
                    if ((head->temps & TEMP_LEFT) && head->dc.left->mode == i_direct)
                    {
                        pbl = tempInfo[head->dc.left->offset->v.sp->value.i]->blockDefines;
                    }
                    else if (head->dc.left->mode != i_immed)
                        canMove = FALSE;
                    if ((head->temps & TEMP_RIGHT) && head->dc.right->mode == i_direct)
                    {
                        pbr = tempInfo[head->dc.right->offset->v.sp->value.i]->blockDefines;
                    }
                    else if (head->dc.right && head->dc.right->mode != i_immed)
                        canMove = FALSE;
                    if (canMove)
                        canMove = InvariantPhiUsing(head);
                    if (canMove)
                        if (pbl && pbl->preWalk != b->preWalk && pbl->nesting == b->nesting &&
                            (!pbr || (pbr->preWalk != b->preWalk && pbr->nesting == b->nesting)))
                            MoveExpression(b, head, pbl, pbr);
                }
                /*
                else if (head->dc.opcode == i_assn && head->dc.left->mode == i_immed)
                {
                    if (!isarithmeticconst(head->dc.left->offset))
                    {
                        LOOP *lp = b->loopParent;
                        if (lp)
                        {
                            BLOCK *b1 = lp->entry;
                            if (b1)
                            {
                                b1 = blockArray[b1->idom];
                                if (b1 && b1 != b)
                                {
//									MoveTo(b1, b, head);
                                }
                            }
                        }
                        
                    }
                }
                */
            }
        }
        head = next;
    }
    while (bl)
    {
        if (!bl->block->preWalk)
        {
            ScanForInvariants(bl->block);
        }
        bl = bl->next;
    }
}
void MoveLoopInvariants(void)
{
    int i;
    refs = NULL;
    for (i = 0; i < blockCount; i++)
        if (blockArray[i])
            blockArray[i]->preWalk = 0;
    for (i = 0; i < tempCount; i++)
        tempInfo[i]->blockDefines = NULL;
    current = 1;
    ScanForInvariants(blockArray[0]);
    WeedRefs();
}