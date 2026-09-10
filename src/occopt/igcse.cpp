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
#include "ioptimizer.h"
#include "beinterfdefs.h"
#include "optmodules.h"
#include "config.h"
#include "ildata.h"
#include "iflow.h"
#include "iblock.h"
#include "OptUtils.h"
#include "ialias.h"
#include "optmain.h"
#include "ioptutil.h"
#include "memory.h"
#include "ilocal.h"

namespace Optimizer
{
namespace
{
static int gcseBlockNumber;
static std::unordered_map<IMODE*, IMODE*> tempTranslation;
static std::vector<std::unordered_map<IMODE*, IMODE*>> memTranslation;
static std::vector<std::unordered_map<QUAD*, IMODE*, OrangeC::Utils::fnv1a32_binary<sizeof(_basic_dag)>,
                                      OrangeC::Utils::bin_eql<sizeof(_basic_dag)>>>
    expressionTranslation;
static std::vector<std::unordered_set<IMODE*>> memKilled;

static bool IsReplaceable(IMODE* im)
{
    return (!chosenAssembler->arch->hasFloatRegs && im->size >= ISZ_FLOAT && im->size != ISZ_BITINT) || im->bits || im->vol;
}
static void InsertTempEquivalence(IMODE* src, IMODE* dest)
{
    if (!IsReplaceable(src))
    {
        tempTranslation[src] = dest;
    }
}
static void InsertMemEquivalence(IMODE* src, IMODE* dest)
{
    if (!IsReplaceable(src))
    {
        auto&& mt = memTranslation[gcseBlockNumber];
        auto it = mt.find(src);
        if (it == mt.end())
            mt[src] = dest;
    }
}
static void LookupInd(IMODE*& im)
{
    if (im->offset->type == se_tempref && im->mode == i_ind)
    {
        auto im2 = im->offset->sp->imvalue;
        if (im2)
        {
            auto it = tempTranslation.find(im2);
            if (it != tempTranslation.end())
            {
                im2 = it->second;
                if (im2->offset->sp->imind)
                {
                    for (auto lst = im2->offset->sp->imind; lst; lst = lst->next)
                    {
                        if (lst->im->size == im->size && lst->im->bits == im->bits && lst->im->startbit == im->startbit)
                        {
                            im = lst->im;
                            break;
                        }
                    }
                }
            }
        }
    }
}
static void LookupMemOrTempEquivalence(IMODE*& im, bool skipMem)
{
    if (im)
    {
        LookupInd(im);
        if (im->offset->type == se_tempref && im->mode == i_direct)
        {
            auto it = tempTranslation.find(im);
            if (it != tempTranslation.end() && !it->second->retval)
                if (!skipMem || (it->second->mode == i_direct && it->second->offset->type == se_tempref))
                    im = it->second;
        }
        else if (im->mode != i_immed)
        {
            auto&& mt = memTranslation[gcseBlockNumber];
            auto it = mt.find(im);
            if (it != mt.end())
                im = it->second;
        }
    }
}
static void LookupEquivalence(QUAD* temp, QUAD* head)
{
    LookupMemOrTempEquivalence(head->dc.left, !!head->dc.right);
    if (head->dc.left && !head->dc.left->retval && head->dc.left->offset->type == se_tempref)
        head->temps |= TEMP_LEFT;
    else
        head->temps &= ~TEMP_LEFT;
    LookupMemOrTempEquivalence(head->dc.right, !!head->dc.left);
    if (head->dc.right && head->dc.right->offset->type == se_tempref)
        head->temps |= TEMP_RIGHT;
    else
        head->temps &= ~TEMP_RIGHT;
    auto&& et = expressionTranslation[gcseBlockNumber];
    auto it = et.find(head);
    if (it != et.end())
    {
        head->dc.opcode = i_assn;
        head->dc.left = it->second;
        head->dc.right = nullptr;
        head->temps = TEMP_ANS | TEMP_LEFT;
    }
}
static void InsertExpressionEquivalence(QUAD* src, IMODE* dest)
{
    if (!IsReplaceable(dest))
    {
        auto&& et = expressionTranslation[gcseBlockNumber];
        auto it = et.find(src);
        if (it == et.end())
            et[src] = dest;
    }
}
static void ModifyOne(IMODE* im)
{
    memTranslation[gcseBlockNumber].erase(im);
    memKilled[gcseBlockNumber].insert(im);
}
static void Modifies(IMODE* mem)
{
    ProcessIMModifies(mem, ModifyOne);
    memTranslation[gcseBlockNumber].erase(mem);
    memKilled[gcseBlockNumber].insert(mem);
}
static void ModifiesGosub() { ProcessUIVAddresses(ModifyOne); }

static void LoadBlockMem(Block* b)
{
    gcseBlockNumber = b->blocknum;
    if (b->pred)
    {
        if (b->pred->next)
        {
            struct myless
            {
                bool operator()(const QUAD* a, const QUAD* b) const { return memcmp(a, b, sizeof(_basic_dag)) < 0; }
            };
            int count = 0;
            std::multimap<IMODE*, IMODE*> mmmemTranslation;
            std::multimap<QUAD*, IMODE*, myless> mmexpressionTranslation;
            std::unordered_set<IMODE*> mmKilled;
            // multiple predecessors
            for (auto bl = b->pred; bl; bl = bl->next, count++)
            {
                for (auto&& mt : memTranslation[bl->block->blocknum])
                {
                    mmmemTranslation.emplace(mt.first, mt.second);
                }
                for (auto&& et : expressionTranslation[bl->block->blocknum])
                {
                    mmexpressionTranslation.emplace(et.first, et.second);
                }
                for (auto k : memKilled[bl->block->blocknum])
                {
                    mmKilled.insert(k);
                }
            }
            for (auto key = mmmemTranslation.begin(); key != mmmemTranslation.end(); ++key)
            {
                int count1 = 0;
                auto range = mmmemTranslation.equal_range(key->first);
                IMODE* cmp = nullptr;
                for (auto d = range.first; d != range.second; ++d)
                {
                    if (cmp && cmp != d->second)
                        break;
                    ++count1;
                    cmp = d->second;
                }
                if (count1 == count && mmKilled.find(key->first) != mmKilled.end())
                    memTranslation[gcseBlockNumber][range.first->first] = range.first->second;
            }
            for (auto key = mmexpressionTranslation.begin(); key != mmexpressionTranslation.end(); ++key)
            {
                int count1 = 0;
                auto range = mmexpressionTranslation.equal_range(key->first);
                IMODE* cmp = nullptr;
                for (auto d = range.first; d != range.second; ++d)
                {
                    if (cmp && cmp != d->second)
                        break;
                    ++count1;
                    cmp = d->second;
                }
                if (count1 == count)
                    expressionTranslation[gcseBlockNumber][range.first->first] = range.first->second;
            }
        }
        else
        {
            // single predecessor
            memTranslation[gcseBlockNumber] = memTranslation[b->pred->block->blocknum];
            expressionTranslation[gcseBlockNumber] = expressionTranslation[b->pred->block->blocknum];
        }
    }
}

static void GCSEProcessBlock(Block* b)
{
    LoadBlockMem(b);
    auto head = b->head;
    while (head != b->tail->fwd)
    {
        if (!head->ignoreMe && head->dc.opcode != i_label && !head->atomic)
        {
            QUAD temp = *head;
            if (temp.temps & TEMP_ANS)
            {
                if (temp.ans->mode == i_ind)
                {
                    Modifies(temp.ans);
                    LookupEquivalence(&temp, head);
                    LookupInd(head->ans);
                    Modifies(head->ans);
                }
                else
                {
                    LookupEquivalence(&temp, head);
                    if (temp.dc.opcode == i_assn)
                    {
                        if (!head->ans->retval)
                        {
                            if (head->temps & TEMP_LEFT)
                            {
                                // assign of temp to temp
                                if (head->dc.left->mode == i_ind)
                                {
                                    // load from mem
                                    InsertMemEquivalence(head->dc.left, head->ans);
                                }
                                else
                                {
                                    // load from temp
                                    if (!head->dc.left->retval && head->ans->size == head->dc.left->size &&
                                        !head->ans->offset->sp->pushedtotemp && !head->genConflict)
                                        InsertTempEquivalence(head->ans, head->dc.left);
                                }
                            }
                            else
                            {
                                // mem or const
                                InsertMemEquivalence(head->dc.left, head->ans);
                            }
                        }
                    }
                    else if (temp.dc.opcode != i_assnblock && temp.dc.right)
                    {
                        // math of some sort
                        if (head->dc.opcode == i_assn)
                        {
                            // already existed
                            InsertTempEquivalence(head->ans, head->dc.left);
                        }
                        else
                        {
                            InsertExpressionEquivalence(head, head->ans);
                        }
                    }
                }
            }
            else if (temp.dc.opcode == i_assn)
            {
                // store to memory
                Modifies(temp.ans);
                LookupEquivalence(&temp, head);
            }
            else if (temp.dc.opcode == i_parm)
            {
                LookupEquivalence(&temp, head);
            }
            else if (temp.dc.opcode == i_gosub)
            {
                // gosub may modify memory
                ModifiesGosub();
            }
            else if (temp.dc.opcode == i_passthrough)
            {
                // passthrough
                ModifiesGosub();
            }
        }
        head = head->fwd;
    }
}

static void GCSEProcessPhi(Block* b)
{
    auto head = b->head;
    while (head != b->tail->fwd)
    {
        if (!head->ignoreMe && head->dc.opcode != i_label && !head->atomic)
        {
            if (head->dc.opcode == i_phi)
            {
                struct _phiblock* pb = head->dc.v.phi->temps;
                while (pb)
                {
                    auto im = tempInfo[pb->Tn]->enode->sp->imvalue;
                    if (im)
                    {
                        LookupMemOrTempEquivalence(im, false);
                        pb->Tn = im->offset->sp->i;
                    }
                    pb = pb->next;
                }
            }
            else
            {
                break;
            }
        }
        head = head->fwd;
    }
}

}  // namespace

void GlobalOptimization(void)
{
    tempTranslation.clear();
    memTranslation.clear();
    expressionTranslation.clear();

    std::list<unsigned> workList;
    std::list<unsigned> forwardOrder;
    int i;
    // reset the visited flags for every block
    for (i = 0; i < blockCount; i++)
        if (blockArray[i])
            blockArray[i]->visiteddfst = false;

    // start at the end, with the exit block

    // this one is completely consumed while making the forward order list
    workList.push_back(0);
    // this one is consumed later, when we are evaluating the livouts
    forwardOrder.push_front(0);
    // last block has been visited
    blockArray[0]->visiteddfst = true;
    // calculate a foorward order to traverse the blocks, where each block is evaluated sometime after all its predecessors are

    // evaluated.
    while (!workList.empty())
    {
        // get a block off the worklist
        unsigned n = workList.front();
        workList.pop_front();

        // process all succecessors
        BLOCKLIST* bl = blockArray[n]->succ;
        while (bl)
        {
            // if a successor has not been visited
            if (!bl->block->visiteddfst)
            {
                bool doVisit = true;
                for (auto pred = bl->block->pred; pred && doVisit; pred = pred->next)
                    doVisit &= pred->block->visiteddfst;

                if (doVisit)
                {
                    // mark it as visited
                    bl->block->visiteddfst = true;
                    // the the block to the work list to visit it
                    workList.push_back(bl->block->blocknum);
                    // we add it to the forward order list here
                    forwardOrder.push_back(bl->block->blocknum);
                }
            }
            bl = bl->next;
        }
    }
    memTranslation.resize(blockCount);
    expressionTranslation.resize(blockCount);
    memKilled.resize(blockCount);
    for (auto f : forwardOrder)
    {
        GCSEProcessBlock(blockArray[f]);
    }
    for (auto f : forwardOrder)
    {
        GCSEProcessPhi(blockArray[f]);
    }
    tempTranslation.clear();
    memTranslation.clear();
    expressionTranslation.clear();
    memKilled.clear();
}

}  // namespace Optimizer
