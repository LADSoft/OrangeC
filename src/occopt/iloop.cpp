/* Software License Agreement
 *
 *     Copyright(C) 1994-2026 David Lindauer, (LADSoft)
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

#include <cstdio>
#include <malloc.h>
#include <cstring>
#include <climits>
#include <stack>
#include "ioptimizer.h"
#include "beinterfdefs.h"
#include "Utils.h"
#include "iblock.h"
#include "OptUtils.h"
#include "ildata.h"
#include "iflow.h"
#include "memory.h"
#include "ilocal.h"
#include "ireshape.h"
#include "optmain.h"
#include "config.h"
#include <algorithm>
#include <unordered_set>
/* Basic loop identity and loop gathering
 * note: some optimizations depend on the fact that the loop numbers
 * are assigned in reverse postorder
 */
namespace Optimizer
{
SimpleVector<FastList<Loop*>> loopContains;
std::vector<TestableList<int>> blocksInLoop;
std::vector<std::list<std::list<int>>> loopInductionSets;
std::vector<Loop*> loopArray;

static std::vector<std::unique_ptr<Loop>> loopNameInBlock;
int loopCount;
static const char* lptype(Loop* lp)
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
    for (i = 0; i < loopCount; i++)
    {
        Loop* lp = loopArray[i];
        if (lp && lp->type != LT_BLOCK)
        {
            fprintf(icdFile, "; %d/%s/B(%d)/", i + 1, lptype(lp), lp->entry->blocknum + 1);
            for (auto lp1 : loopContains[lp->loopnum])
            {
                if (lp1->type == LT_BLOCK)
                    fprintf(icdFile, "B(%d) ", lp1->entry->blocknum + 1);
                else
                    fprintf(icdFile, "L(%d) ", lp1->loopnum + 1);
            }
            fprintf(icdFile, "\n");
        }
    }
}
static void findInfinite(Block* b)
{
    if (!b->visiteddfst)
    {
        BLOCKLIST* l = b->pred;
        b->visiteddfst = true;
        while (l)
        {
            findInfinite(l->block);
            l = l->next;
        }
    }
}
/* add a thunk block to get us out of an infinite loop */
static void makeNonInfinite(Block* b)
{
    Block* nb = NewBlock();
    BLOCKLIST** bt;
    QUAD *quad, *quad2;

    blockArray[nb->blocknum] = nb;
    nb->unuseThunk = true;

    nb->succ = oAllocate<BLOCKLIST>();
    nb->succ->block = blockArray[exitBlock];

    nb->pred = oAllocate<BLOCKLIST>();
    nb->pred->block = b;

    bt = &b->succ;
    while (*bt)
        bt = &(*bt)->next;
    *bt = oAllocate<BLOCKLIST>();
    (*bt)->block = nb;

    bt = &blockArray[exitBlock]->pred;
    while (*bt)
        bt = &(*bt)->next;
    *bt = oAllocate<BLOCKLIST>();
    (*bt)->block = nb;

    /* note : the following does NOT insert jmps for the
     * newly added block
     */
    *criticalThunkPtr = quad = Allocate<QUAD>();
    criticalThunkPtr = &quad->fwd;
    quad->dc.opcode = i_block;
    quad->block = nb;
    quad->dc.v.label = nb->blocknum;

    *criticalThunkPtr = quad2 = Allocate<QUAD>();
    criticalThunkPtr = &quad2->fwd;
    quad2->back = quad;
    quad2->dc.opcode = i_blockend;
    quad2->block = nb;
    //	quad2->dc.v.label = bi->block->blocknum;

    nb->head = quad;
    nb->tail = quad2;
}
void CancelInfinite(int orgBlockCount)
{
    int i;
    (void)orgBlockCount;
    while (true)
    {
        for (i = 0; i < blockCount; i++)
        {
            if (blockArray[i])
                blockArray[i]->visiteddfst = false;
        }
        findInfinite(blockArray[exitBlock]);
        for (i = exitBlock; i >= 0; i--)
        {
            if (blockArray[i] && !blockArray[i]->visiteddfst)
            {
                makeNonInfinite(blockArray[i]);
                break;
            }
        }
        if (i < 0)
            break;
    }
}
void RemoveInfiniteThunks(void)
{
    int i;
    for (i = 0; i < blockCount; i++)
    {
        Block* b = blockArray[i];
        if (b && b->unuseThunk)
        {
            BLOCKLIST** l;
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
            blockArray[i] = nullptr;
        }
    }
}
/* may return either a block or loop */
static Loop* LoopAncestor(Block* b)
{
    Loop* head = b->loopParent;
    if (!head)
        return loopNameInBlock[b->blocknum].get();
    while (head->parent != nullptr)
        head = head->parent;
    return head;
}
static void FindBody(BLOCKLIST* gen, Block* head, enum e_lptype type)
{
    std::stack<Loop*> queue;
    Loop** lpp;
    int i;
    if (!gen)
        return;

    TestableList<int> loopItems;

    while (gen)
    {
        Loop* l = LoopAncestor(gen->block);
        if (l && !loopItems.test(l->loopnum))
        {
            queue.push(l);
            loopItems.push_back(l->loopnum);
        }
        gen = gen->next;
    }
    while (!queue.empty())
    {
        auto item = queue.top();
        if (item->entry)
        {
            BLOCKLIST* p = item->entry->pred;
            queue.pop();
            while (p)
            {
                if (p->block != head)
                {
                    Loop* l = LoopAncestor(p->block);
                    if (l && !loopItems.test(l->loopnum))
                    {
                        queue.push(l);
                        loopItems.push_back(l->loopnum);
                    }
                }
                p = p->next;
            }
        }
        else
        {
            queue.pop();
        }
    }
    Loop* lp = new Loop{e_lptype::LT_BLOCK};
    lp->loopnum = loopCount;
    lp->type = type;
    lp->entry = head;
    lp->parent = nullptr;
    if (loopCount >= loopArray.size())
    {
        loopArray.resize(loopCount + 40);
        loopContains.resize(loopCount + 40);
        blocksInLoop.resize(loopCount + 40);
        loopInductionSets.resize(loopCount + 40);
    }
    head->loopParent = lp;
    loopArray[loopCount] = lp;
    loopContains[loopCount].push_back(loopNameInBlock[head->blocknum].get());
    for (auto n : loopItems)
    {
        loopContains[loopCount].push_back(loopArray[n]);
        if (loopArray[n]->type == LT_BLOCK)
            loopArray[n]->entry->loopParent = lp;
        else
            loopArray[n]->parent = lp;
    };
    loopCount++;
}
static Block* findCommonDominator(Block* one, Block* two)
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
static void FindLoop(Block* b)
{
    BLOCKLIST* loop = nullptr;
    BLOCKLIST* bl;
    Block* Z = b;
    bl = b->pred;
    while (bl)
    {
        if (getEdgeType(bl->block->blocknum, b->blocknum) == F_BACKEDGE)
        {
            Z = findCommonDominator(Z, bl->block);
            if (bl->block != b)  // we don't care about duplicates, they will be filtered later
            {
                BLOCKLIST* bm = oAllocate<BLOCKLIST>();
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
        BLOCKLIST** bp = &Z->loopGenerators;
        while (*bp)
            bp = &(*bp)->next;
        (*bp) = loop;
    }
    else
    {
        FindBody(loop, b, LT_SINGLE);
    }
}
static void FindLoopOuter(Block* b)
{
    BLOCKLIST* bl;
    if (b->visiteddfst)
        return;
    bl = b->succ;
    b->visiteddfst = true;
    while (bl)
    {
        FindLoopOuter(bl->block);
        bl = bl->next;
    }
    if (b->loopGenerators)
    {
        FindBody(b->loopGenerators, b, LT_MULTI);
    }
    FindLoop(b);
}
static void CalculateLoopedBlocks(Loop* l)
{
    if (l->type != LT_BLOCK)
    {
        for (auto l1 : loopContains[l->loopnum])
        {
            CalculateLoopedBlocks(l1);
        }
        for (auto lm : loopContains[l->loopnum])
        {
            if (lm->type == LT_BLOCK)
            {
                blocksInLoop[l->loopnum].push_back(lm->entry->blocknum);
            }
            else
            {
                for (auto b : blocksInLoop[lm->loopnum])
                    blocksInLoop[l->loopnum].push_back(b);
            }
        }
    }
}
/* finds all the successors to the loop */
static void CalculateSuccessors(Loop* lp)
{
    for (auto current : loopContains[lp->loopnum])
    {
        if (lp->type != LT_BLOCK || current->loopnum == lp->loopnum)
            CalculateSuccessors(current);
    }

    if (lp->type == LT_BLOCK)
    {
        auto succ = lp->entry->succ;
        for (; succ; succ = succ->next)
        {
            if (loopNameInBlock[succ->block->blocknum]->loopnum != lp->loopnum)
            {
                BLOCKLIST* newExit = oAllocate<BLOCKLIST>();
                newExit->next = lp->successors;
                lp->successors = newExit;
                newExit->block = succ->block;
            }
        }
    }
    else
    {
        for (auto inner : loopContains[lp->loopnum])
        {
            /* only have to add blocks that could have exited an inner loop
             * and only if they also exit this loop
             */
            for (BLOCKLIST* prevSuccessors = inner->successors; prevSuccessors; prevSuccessors = prevSuccessors->next)
            {
                if (!blocksInLoop[lp->loopnum].test(prevSuccessors->block->blocknum))
                {
                    BLOCKLIST* newExit = oAllocate<BLOCKLIST>();
                    newExit->next = lp->successors;
                    lp->successors = newExit;
                    newExit->block = prevSuccessors->block;
                }
            }
        }
    }
}
void BuildLoopTree(void)
{
    BLOCKLIST bl;
    Block* b;
    int i;
    QUAD* tail;
    bool skip = false;
    loopArray.clear();
    loopContains.clear();
    blocksInLoop.clear();
    loopInductionSets.clear();
    loopNameInBlock.clear();
    loopCount = 0;

    loopNameInBlock.resize(blockCount);
    loopArray.resize(blockCount);
    loopContains.resize(blockCount);
    blocksInLoop.resize(blockCount);
    loopInductionSets.resize(blockCount);
    for (i = 0; i < blockCount; i++)
    {
        if (blockArray[i])
        {
            blockArray[i]->visiteddfst = false;
            blockArray[i]->loopParent = nullptr;
            Loop* lp = new Loop{e_lptype::LT_BLOCK};
            lp->entry = blockArray[i];
            lp->loopnum = loopCount;
            blockArray[i]->loopGenerators = nullptr;
            loopNameInBlock[i] = std::move(std::unique_ptr<Loop>(lp));
            loopArray[loopCount] = lp;
            loopCount++;
        }
    }
    FindLoopOuter(blockArray[0]);

    memset(&bl, 0, sizeof(bl));
    bl.block = blockArray[exitBlock];
    FindBody(&bl, blockArray[0], LT_ROOT);
    CalculateLoopedBlocks(loopArray[loopCount - 1]);

    CalculateSuccessors(loopArray[loopCount - 1]);
    //   if (cparams.prm_icdfile)
    //   {
    //       fprintf(icdFile, "; loop dump\n");
    //       dump_loops();
    //   }
    tail = intermed_tail;
    b = tail->block;
    while (tail)
    {
        switch (tail->dc.opcode)
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
        Utils::Fatal("Internal error - Loop Count");
}
bool isAncestor(Loop* l1, Loop* l2)
{
    if (l1 == l2)
        return false;
    while (l2)
    {
        if (l1 == l2)
            return true;
        l2 = l2->parent;
    }
    return false;
}
static Loop* nearestAncestor(Loop* l1, Loop* l2)
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
static bool isInvariant(int tnum, Loop* l)
{
    Loop* varl = tempInfo[tnum]->variantLoop;
    while (varl)
    {
        if (varl == l)
        {
            return false;
        }
        varl = varl->parent;
    }
    return true;
}
static void CalculateLoopInvariants(Block* b)
{
    BLOCKLIST* children = b->dominates;
    QUAD* head;
    int i;
    if (b == blockArray[0])
        for (i = 0; i < tempCount; i++)
            tempInfo[i]->variantLoop = nullptr;
    /* again the PHI nodes are first, which satisfies the algorithm need
     * to evaluate them first
     */
    head = b->head;
    while (head != b->tail->fwd)
    {
        if (head->dc.opcode == i_phi)
        {
            PHIDATA* pd = head->dc.v.phi;
            tempInfo[pd->T0]->variantLoop = b->loopParent;
        }
        else if ((head->temps & TEMP_ANS) && head->ans->mode == i_direct)
        {
            Loop* varying = b->loopParent;
            Loop* t_varying;
            int tnum;
            if ((head->temps & TEMP_LEFT) && head->dc.left->mode == i_direct)
            {
                tnum = head->dc.left->offset->sp->i;
                t_varying = nearestAncestor(tempInfo[tnum]->variantLoop, b->loopParent);
                varying = t_varying;
            }
            if ((head->temps & TEMP_RIGHT) && head->dc.right->mode == i_direct)
            {
                tnum = head->dc.right->offset->sp->i;
                t_varying = nearestAncestor(tempInfo[tnum]->variantLoop, b->loopParent);
                // if t_varying is a more inner level than varying, we need to choose
                // t_varying because we need the innermost loop
                if (isAncestor(varying, t_varying))
                    varying = t_varying;
            }
            tnum = head->ans->offset->sp->i;
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

static void PruneInductionCandidate(TestableList<int>& inductionCandidateStack, std::unordered_set<int>& candidates, int tnum,
                                    Loop* l)
{
    if (tempInfo[tnum]->size >= ISZ_FLOAT)
    {
        candidates.erase(tnum);
    }
    else
    {
        QUAD* I = InstructionDefinesSsa[tnum];
        if (I)
        {
            switch (I->dc.opcode)
            {
                case i_phi: {
                    PHIDATA* pd = I->dc.v.phi;
                    struct _phiblock* pb = pd->temps;
                    while (pb)
                    {
                        if (candidates.find(pb->Tn) == candidates.end() && !isInvariant(pb->Tn, l))
                        {
                            candidates.erase(tnum);
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
                        int tn2 = I->dc.right->offset->sp->i;
                        if (candidates.find(tn2) == candidates.end())
                            candidates.erase(tnum);
                    }
                    /* FALLTHROUGH */
                case i_neg:
                case i_assn:
                    if (I->temps & TEMP_LEFT)
                    {
                        int tn2 = I->dc.left->offset->sp->i;
                        if (candidates.find(tn2) == candidates.end())
                            candidates.erase(tnum);
                    }
                    break;
                default:
                    diag("PruneInductionCandidate: invalid form");
                    break;
            }
        }
        else
        {
            candidates.erase(tnum);
        }
    }
}
/* minor problem: if an induction variable spans multiple loops
 * an induction set will be created for each loop.  These sets will
 * be identical
 */
static void CalculateInductionCandidates(std::unordered_set<int>& candidates, Loop* l)
{
    int i;
    TestableList<int> inductionCandidateStack;
    for (auto i : blocksInLoop[l->loopnum])
    {
        Block* b = blockArray[i];
        if (b)
        {
            QUAD* head = b->head;
            while (head != b->tail->fwd)
            {
                switch (head->dc.opcode)
                {
                    case i_add:
                    case i_sub:
                        if ((head->temps & TEMP_ANS) && head->ans->mode == i_direct)
                        {
                            if (head->dc.left->mode == i_immed || ((head->temps & TEMP_LEFT) && head->dc.left->mode == i_direct))
                                if (head->dc.right->mode == i_immed ||
                                    ((head->temps & TEMP_RIGHT) && head->dc.right->mode == i_direct))
                                {
                                    if (head->ans->size < ISZ_FLOAT && head->dc.left->size < ISZ_FLOAT &&
                                        head->dc.right->size < ISZ_FLOAT)
                                    {
                                        int tnum = head->ans->offset->sp->i;
                                        candidates.insert(tnum);
                                        inductionCandidateStack.push(tnum);
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
                                    int tnum = head->ans->offset->sp->i;
                                    candidates.insert(tnum);
                                    inductionCandidateStack.push(tnum);
                                }
                            }
                        }
                        break;
                    case i_phi: {
                        PHIDATA* pd = head->dc.v.phi;
                        int tnum = pd->T0;
                        if (tempInfo[tnum]->size < ISZ_FLOAT)
                        {
                            candidates.insert(tnum);
                            inductionCandidateStack.push(tnum);
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
                                int tnum = head->ans->offset->sp->i;
                                for (auto uses : InstructionUsesSsa[tnum])
                                {
                                    if ((uses->temps & TEMP_ANS) && uses->ans->mode == i_ind)
                                    {
                                        if (tnum == uses->ans->offset->sp->i)
                                        {
                                            tempInfo[tnum]->expressionRoot = true;
                                        }
                                    }
                                    if (uses->temps & TEMP_LEFT)
                                    {
                                        if (tnum == uses->dc.left->offset->sp->i)
                                        {
                                            if (uses->dc.left->mode == i_ind)
                                                tempInfo[tnum]->expressionRoot = true;
                                            else if (head->dc.opcode != i_not && head->dc.opcode != i_neg)
                                                if (!matchesop(head->dc.opcode, uses->dc.opcode))
                                                    tempInfo[tnum]->expressionRoot = true;
                                        }
                                    }
                                    if (uses->temps & TEMP_RIGHT)
                                    {
                                        if (tnum == uses->dc.right->offset->sp->i)
                                        {
                                            if (uses->dc.right->mode == i_ind)
                                                tempInfo[tnum]->expressionRoot = true;
                                            else if (head->dc.opcode != i_not && head->dc.opcode != i_neg)
                                                if (!matchesop(head->dc.opcode, uses->dc.opcode))
                                                    tempInfo[tnum]->expressionRoot = true;
                                        }
                                    }
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
    while (!inductionCandidateStack.empty())
    {
        int tnum = inductionCandidateStack.top();
        inductionCandidateStack.pop();
        PruneInductionCandidate(inductionCandidateStack, candidates, tnum, l);
        if (candidates.find(tnum) == candidates.end())
        {
            for (auto uses : InstructionUsesSsa[tnum])
            {
                if (blocksInLoop[l->loopnum].test(uses->block->blocknum))
                {
                    if (uses->temps & TEMP_ANS)
                    {
                        int tnum = uses->ans->offset->sp->i;
                        if (candidates.find(tnum) != candidates.end())
                        {
                            if (!inductionCandidateStack.test(tnum))
                            {
                                inductionCandidateStack.push(tnum);
                            }
                        }
                    }
                }
            }
        }
    }
};
static void StrongRegiondfs(TestableList<int>& strongStack, std::list<std::list<int>>& strongTemps, int tnum, int& max_dfs)
{
    TempInfo* t = tempInfo[tnum];
    t->temp = t->dfstOrder = max_dfs++;
    strongStack.push(tnum);
    t->visiteddfst = true;
    for (auto uses : InstructionUsesSsa[tnum])
    {
        int ux = -1;
        switch (uses->dc.opcode)
        {
            case i_phi: {
                PHIDATA* pd = uses->dc.v.phi;
                if (tempInfo[pd->T0]->size < ISZ_FLOAT)
                    ux = pd->T0;
            }
            break;
            case i_add:
            case i_sub:
            case i_assn:
            case i_neg:
                if (uses->temps & TEMP_ANS)
                {
                    ux = uses->ans->offset->sp->i;
                    if (abs(tempInfo[ux]->size) > chosenAssembler->arch->compatibleIntSize)
                        ux = -1;
                }
                break;
            default:
                break;
        }
        if (ux >= 0)
        {
            TempInfo* up = tempInfo[ux];
            if (!up->visiteddfst)
            {
                StrongRegiondfs(strongStack, strongTemps, ux, max_dfs);
                if (up->temp < t->temp)
                    t->temp = up->temp;
            }
            else if (strongStack.test(ux))
            {
                if (up->dfstOrder < t->temp)
                    t->temp = up->dfstOrder;
            }
        }
    }

    if (t->temp == t->dfstOrder)
    {
        std::list<int> region;
        int vp;
        do
        {
            vp = strongStack.top();
            strongStack.pop();
            region.push_front(vp);
        } while (vp != tnum);
        if (region.size() > 1)
            strongTemps.push_front(std::move(region));
    }
}
/* this returns a two-layer list
 * the top layer is a linked list of strong regions
 * each secondary layer is the induction set for the region
 * first element of list is the anchor
 *
 * I modified the algorithm to quit generating 1 - element lists...
 */
static void StrongRegions(TestableList<int>& strongStack, std::unordered_set<int>& candidates,
                          std::list<std::list<int>>& strongTemps, Loop* lp)
{
    int i;
    QUAD* head;
    int max_dfs = 0;
    for (i = 0; i < tempCount; i++)
    {
        tempInfo[i]->dfstOrder = 0;
        tempInfo[i]->visiteddfst = false;
        tempInfo[i]->temp = INT_MAX;
    }
    head = lp->entry->head->fwd;
    while (head != lp->entry->tail->fwd && (head->ignoreMe || head->dc.opcode == i_label))
        head = head->fwd;
    while (head != lp->entry->tail->fwd && head->dc.opcode == i_phi)
    {
        PHIDATA* pd = head->dc.v.phi;
        if (candidates.find(pd->T0) != candidates.end() && abs(tempInfo[pd->T0]->size) <= chosenAssembler->arch->compatibleIntSize)
        {
            StrongRegiondfs(strongStack, strongTemps, pd->T0, max_dfs);
        }
        head = head->fwd;
    }
}
void CalculateInduction(void)
{
    int i;
    std::unordered_set<int> candidates;
    CalculateLoopInvariants(blockArray[0]);
    TestableList<int> strongStack;
    for (i = 0; i < loopArray.size(); i++)
    {
        Loop* lp = loopArray[i];
        if (lp && lp->type == LT_SINGLE)
        {
            CalculateInductionCandidates(candidates, lp);
            std::list<std::list<int>> strongTemps;
            StrongRegions(strongStack, candidates, strongTemps, lp);
            if (!strongTemps.empty())
            {
                for (auto&& q : strongTemps)
                {
                    // we optmzed earlier and didn't add one temp items to this list
                    loopInductionSets[lp->loopnum].push_front(q);
                    for (auto j : q)
                    {
                        /* the dominator walk will visit inner loops first,
                         * the outer loops have higher indexes
                         * we want the outermost loop
                         */
                        tempInfo[j]->inductionLoop = imax(i, tempInfo[j]->inductionLoop);
                    }
                }
            }
        }
    }
}
}  // namespace Optimizer
