/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
 *
 */

/* we are only doing local opts on temp variables.  At this point,
 * any variable that does not have its address taken is also made a temp
 * variable
 */
#include <algorithm>
#include <iterator>
#include <cstdio>
#include <malloc.h>
#include <cstring>
#include "ioptimizer.h"
#include "beinterfdefs.h"
#include "config.h"
#include "iblock.h"
#include "OptUtils.h"
#include "ildata.h"
#include "ioptutil.h"
#include "iflow.h"
#include "memory.h"
#include "ilive.h"

namespace Optimizer
{
int* dfst;
BriggsSet* globalVars;

static BriggsSet* visited;
static BriggsSet* worklist;
static BriggsSet* livelist;
static bool hasPhi;
QUAD* beforeJmp(QUAD* I, bool before)
{
    QUAD* start = I;
    while (I->dc.opcode != i_block)
    {
        switch (I->dc.opcode)
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
            case i_cmpblock:
            case i_computedgoto:
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
    BriggsSet* exposed = briggsAllocs(tempCount);
    int i;
    for (i = 0; i < blockCount; i++)
    {
        struct Block* blk = blockArray[i];
        if (blk && blk->head)
        {
            QUAD* tail = blk->tail;
            int j;
            briggsClear(exposed);
            do
            {
                if (tail->dc.opcode == i_phi)
                {
                    PHIDATA* pd = tail->dc.v.phi;
                    struct _phiblock* pb = pd->temps;
                    hasPhi = true;
                    briggsReset(exposed, pd->T0);
                    blk->liveGen->erase(pd->T0);
                    blk->liveKills->insert(pd->T0);
                    while (pb)
                    {
                        briggsSet(exposed, pb->Tn);
                        blk->liveGen->insert(pb->Tn);
                        pb = pb->next;
                    }
                }
                else if (tail->dc.opcode == i_passthrough)
                {
                    int tnum = tail->assemblyTempRegStart;
                    briggsReset(exposed, tnum);
                    blk->liveGen->erase(tnum);
                    blk->liveKills->insert(tnum);
                    for (int i = 1; i < tail->assemblyRegCount; i++)
                    {
                        briggsSet(exposed, tnum + i);
                        blk->liveGen->insert(tnum + i);
                    }
                }
                else
                {
                    if (tail->temps & TEMP_ANS)
                    {
                        if (tail->ans->mode == i_direct)
                        {
                            int tnum = tail->ans->offset->sp->i;
                            briggsReset(exposed, tnum);
                            blk->liveGen->erase(tnum);
                            blk->liveKills->insert(tnum);
                        }
                        else if (tail->ans->mode == i_ind)
                        {
                            if (tail->ans->offset)
                            {
                                briggsSet(exposed, tail->ans->offset->sp->i);
                                blk->liveGen->insert(tail->ans->offset->sp->i);
                            }
                            if (tail->ans->offset2)
                            {
                                briggsSet(exposed, tail->ans->offset2->sp->i);
                                blk->liveGen->insert(tail->ans->offset2->sp->i);
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
                                    briggsSet(exposed, tail->dc.left->offset->sp->i);
                                    blk->liveGen->insert(tail->dc.left->offset->sp->i);
                                }
                                if (tail->dc.left->offset2)
                                {
                                    briggsSet(exposed, tail->dc.left->offset2->sp->i);
                                    blk->liveGen->insert(tail->dc.left->offset2->sp->i);
                                }
                            }
                        }
                    if (tail->temps & TEMP_RIGHT)
                        if (tail->dc.right->mode == i_ind || tail->dc.right->mode == i_direct)
                        {
                            if (tail->dc.right->offset)
                            {
                                briggsSet(exposed, tail->dc.right->offset->sp->i);
                                blk->liveGen->insert(tail->dc.right->offset->sp->i);
                            }
                            if (tail->dc.right->offset2)
                            {
                                briggsSet(exposed, tail->dc.right->offset2->sp->i);
                                blk->liveGen->insert(tail->dc.right->offset2->sp->i);
                            }
                        }
                }
                if (tail != blk->head) /* in case tail == head */
                    tail = tail->back; /* skipping the actual block statement */
            } while (tail != blk->head);
            briggsUnion(globalVars, exposed);
        }
    }
    for (i = 0; i < globalVars->top; i++)
    {
        int t = globalVars->data[i], j;
        tempInfo[t]->liveAcrossBlock = true;
    }
}
static void liveOut()
{
    std::list<unsigned> workList;
    std::list<unsigned> reverseOrder;
    int i;
    // reset the visited flags for every block
    for (i = 0; i < blockCount; i++)
        if (blockArray[i])
            blockArray[i]->visiteddfst = false;

    // start at the end, with the exit block

    // this one is completely consumed while making the reverse order list
    workList.push_back(exitBlock);
    // this one is consumed later, when we are evaluating the livouts
    reverseOrder.push_back(exitBlock);
    // last block has been visited
    blockArray[exitBlock]->visiteddfst = true;
    // calculate a reverse order to traverse the blocks, where each block is evaluated sometime after all its successors are
    // evaluated.
    while (!workList.empty())
    {
        // get a block off the worklist
        unsigned n = workList.front();
        workList.pop_front();

        // process all predecessors
        BLOCKLIST* bl = blockArray[n]->pred;
        while (bl)
        {
            // if a a predecessor has not been visited
            if (!bl->block->visiteddfst)
            {
                // mark it as visited
                bl->block->visiteddfst = true;
                // the the block to the work list to visit it
                workList.push_back(bl->block->blocknum);
                // we add it to the reverse order list here
                reverseOrder.push_back(bl->block->blocknum);
            }
            bl = bl->next;
        }
    }
    // go thrugh the reverse order list finding the livout for each block
    while (!reverseOrder.empty())
    {
        // get a block off the traversal list
        unsigned n = reverseOrder.front();
        reverseOrder.pop_front();
        Block* b = blockArray[n];
        b->visiteddfst = false;  // visited this pass (flag has taken on negative logic
        b->liveOut->clear();
        std::set<unsigned> temp, temp1;
        // for each successor
        BLOCKLIST* bl = b->succ;
        while (bl)
        {
            // add the successor's livein nodes to our liveout nodes
            std::set_union(b->liveOut->begin(), b->liveOut->end(), bl->block->liveIn->begin(), bl->block->liveIn->end(),
                           std::inserter(temp, temp.begin()));
            *b->liveOut = std::move(temp);
            bl = bl->next;
        }

        // this next bit recalculates the liveIn for this block
        // the next bit calculates gen | (liveout & ~kills)
        for (auto i : *b->liveOut)
        {
            // livout & ~kills
            if (b->liveKills->find(i) == b->liveKills->end())
                temp1.insert(i);
        }
        // add in gen
        std::set_union(temp1.begin(), temp1.end(), b->liveGen->begin(), b->liveGen->end(), std::inserter(temp, temp.begin()));

        // if liveIn has changed
        if (*b->liveIn != temp)
        {
            // assign the new liveIn
            *b->liveIn = std::move(temp);

            // for each predecessor
            bl = b->pred;
            while (bl)
            {
                // any predecessor block that was already visited this pass has to be visited again;
                // we've changed liveIn so that means we have to recalcuate the liveouts for predecessors
                // unless they are already queued for a future evaluation....
                if (!bl->block->visiteddfst)
                {
                    bl->block->visiteddfst = true;
                    reverseOrder.push_back(bl->block->blocknum);
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
                QUAD* t = blockArray[i]->tail->fwd;
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
                    QUAD* q = Allocate<QUAD>();
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
    for (i = 0; i < blockCount; i++)
    {
        struct Block* blk = blockArray[i];
        if (blk)
        {
            QUAD* head = blk->head->fwd;
            if (head)
            {
                while (head != blk->tail->fwd && (head->dc.opcode == i_label || head->ignoreMe))
                {
                    head = head->fwd;
                }
                while (head->dc.opcode == i_phi && head != blk->tail->fwd)
                {
                    PHIDATA* pd = head->dc.v.phi;
                    struct _phiblock* pb = pd->temps;
                    BLOCKLIST* bl = blk->pred;
                    while (pb)
                    {
                        struct _phiblock* pb2 = pd->temps;
                        while (pb2)
                        {
                            if (pb->Tn != pb2->Tn)
                            {
                                bl->block->liveKills->insert(pb2->Tn);
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
    for (i = 0; i < blockCount; i++)
    {
        struct Block* blk = blockArray[i];
        if (blk)
        {
            QUAD* head = blk->head->fwd;
            if (head)
            {
                while (head != blk->tail->fwd && (head->dc.opcode == i_label || head->ignoreMe))
                {
                    head = head->fwd;
                }
                while (head->dc.opcode == i_phi && head != blk->tail->fwd)
                {
                    PHIDATA* pd = head->dc.v.phi;
                    struct _phiblock* pb = pd->temps;
                    BLOCKLIST* bl = blk->pred;
                    while (pb)
                    {
                        struct _phiblock* pb2 = pd->temps;
                        while (pb2)
                        {
                            bl->block->liveOut->erase(pb2->Tn);
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
static void markLiveInstruction(BriggsSet* live, QUAD* ins)
{
    switch (ins->dc.opcode)
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
            ins->live = true;
            break;
        case i_parm:
        case i_gosub:
        case i_label:
        case i_goto:
        case i_computedgoto:
            ins->live = true;
            break;
        case i_assnblock:
        case i_clrblock:
        case i_parmblock:
        case i_cmpblock:
            ins->live = true;
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
            ins->live = true;
            break;
        case i_phi:
            if (briggsTest(live, ins->dc.v.phi->T0))
            {
                struct _phiblock* pb = ins->dc.v.phi->temps;
                ins->live = true;
                while (pb)
                {
                    briggsSet(live, pb->Tn);
                    pb = pb->next;
                }
            }
            return;
        default:
            if (!(ins->temps & TEMP_ANS) || ins->ans->mode == i_ind ||
                ((chosenAssembler->arch->denyopts & DO_NODEADPUSHTOTEMP) && ins->ans->offset->sp->pushedtotemp))
                ins->live = true;
            else if ((ins->temps & TEMP_ANS) && briggsTest(live, ins->ans->offset->sp->i))
                ins->live = true;

            break;
    }
    if (ins->live)
    {
        if (ins->temps & TEMP_ANS)
        {
            if (ins->ans->mode == i_direct)
            {
                briggsReset(live, ins->ans->offset->sp->i);
            }
            else if (ins->ans->mode == i_ind)
            {
                if (ins->ans->offset)
                    briggsSet(live, ins->ans->offset->sp->i);
                if (ins->ans->offset2)
                    briggsSet(live, ins->ans->offset2->sp->i);
            }
        }
        if (ins->temps & TEMP_LEFT)
        {
            if (ins->dc.left->mode == i_direct || ins->dc.left->mode == i_ind)
            {
                if (ins->dc.left->offset)
                    briggsSet(live, ins->dc.left->offset->sp->i);
                if (ins->dc.left->offset2)
                    briggsSet(live, ins->dc.left->offset2->sp->i);
            }
        }
        if (ins->temps & TEMP_RIGHT)
        {
            if (ins->dc.right->mode == i_direct || ins->dc.right->mode == i_ind)
            {
                if (ins->dc.right->offset)
                    briggsSet(live, ins->dc.right->offset->sp->i);
                if (ins->dc.right->offset2)
                    briggsSet(live, ins->dc.right->offset2->sp->i);
            }
        }
    }
}
void removeDead(Block* b)
{
    static BriggsSet* live;
    BITINT* p;
    int j, k;
    QUAD* tail;
    BLOCKLIST* bl;
    bool done = false;
    if (b == blockArray[0])
    {
        int i;
        liveVariables();
        live = briggsAlloc(tempCount);
        for (i = 0; i < blockCount; i++)
            if (blockArray[i])
            {
                QUAD* tail = blockArray[i]->head;
                while (tail != blockArray[i]->tail->fwd)
                {
                    tail->live = tail->alwayslive;
                    tail = tail->fwd;
                }
                blockArray[i]->visiteddfst = false;
            }
    }
    b->visiteddfst = true;
    briggsClear(live);
    for (auto l : *b->liveOut)
    {
        briggsSet(live, l);
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
        QUAD* head = intermed_head;
        bool changed = false;
        int i;
        for (i = 0; i < blockCount; i++)
        {
            Block* b1 = blockArray[i];
            if (b1)
            {
                QUAD* head = b1->head;
                while (head != b1->tail->fwd)
                {
                    if (!head->live)
                    {
                        if (head->dc.opcode != i_block && !head->ignoreMe && head->dc.opcode != i_label)
                        {
                            changed = true;
                            RemoveInstruction(head);
                            if (head->dc.opcode == i_coswitch || (head->dc.opcode >= i_jne && head->dc.opcode <= i_jge) ||
                                head->dc.opcode == i_cmpblock)
                            {
                                BLOCKLIST* bl = head->block->succ->next;
                                head->block->succ->next = nullptr;
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
    briggsFrees();
    hasPhi = false;
    globalVars = briggsAllocs(tempCount);
    worklist = briggsAllocs(blockCount);
    livelist = briggsAllocs(blockCount);
    visited = briggsAllocs(blockCount);
    int ccount = 0;
    for (i = 0; i < tempCount; i++)
        if (tempInfo[i]->inUse)
            ccount++;
    for (i = 0; i < blockCount; i++)
    {
        if (blockArray[i])
        {
            if (!blockArray[i]->liveGen)
            {
                blockArray[i]->liveGen = new std::set<unsigned>();
                blockArray[i]->liveKills = new std::set<unsigned>();
                blockArray[i]->liveIn = new std::set<unsigned>();
                blockArray[i]->liveOut = new std::set<unsigned>();
            }
            else
            {
                // at the end of the function processing these will be dangling...
                blockArray[i]->liveGen->clear();
                blockArray[i]->liveKills->clear();
                blockArray[i]->liveIn->clear();
                blockArray[i]->liveOut->clear();
            }
        }
    }
    for (i = 0; i < tempCount; i++)
    {
        tempInfo[i]->liveAcrossBlock = false;
    }
    liveSetup();
    if (hasPhi)
        killPhiPaths1();
    liveOut();
    if (hasPhi)
        killPhiPaths2();
}
}  // namespace Optimizer