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
static std::unordered_map<IMODE*, IMODE*> tempTranslation;
static std::unordered_map<IMODE*, IMODE*> memTranslation;
static std::unordered_map<QUAD*, IMODE*, OrangeC::Utils::fnv1a32_binary<sizeof(_basic_dag)>,
                          OrangeC::Utils::bin_eql<sizeof(_basic_dag)>>
    expressionTranslation;

static void InsertTempEquivalence(IMODE* src, IMODE* dest) { tempTranslation[src] = dest; }
static void InsertMemEquivalence(IMODE* src, IMODE* dest)
{
    auto it = memTranslation.find(src);
    if (it == memTranslation.end())
        memTranslation[src] = dest;
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
                        if (lst->im->ptrsize == im->ptrsize)
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
static void LookupMemOrTempEquivalence(IMODE*& im)
{
    if (im)
    {
        LookupInd(im);
        if (im->offset->type == se_tempref && im->mode == i_direct)
        {
            auto it = tempTranslation.find(im);
            if (it != tempTranslation.end())
                im = it->second;
        }
        else if (im->mode != i_immed)
        {
            auto it = memTranslation.find(im);
            if (it != memTranslation.end())
                im = it->second;
        }
    }
}
static void LookupEquivalence(QUAD* temp, QUAD* head)
{
    if (head->dc.right)
        printf("hi");
    LookupMemOrTempEquivalence(head->dc.left);
    if (head->dc.left && head->dc.left->offset->type == se_tempref)
        head->temps |= TEMP_LEFT;
    LookupMemOrTempEquivalence(head->dc.right);
    if (head->dc.right && head->dc.right->offset->type == se_tempref)
        head->temps |= TEMP_RIGHT;
    auto it = expressionTranslation.find(head);
    if (it != expressionTranslation.end())
    {
        head->dc.opcode = i_assn;
        head->dc.left = it->second;
        head->dc.right = nullptr;
        head->temps = TEMP_ANS | TEMP_LEFT;
    }
}
static void InsertExpressionEquivalence(QUAD* src, IMODE* dest)
{
    auto it = expressionTranslation.find(src);
    if (it == expressionTranslation.end())
        expressionTranslation[src] = dest;
}
static void ModifyOne(IMODE* im) { memTranslation.erase(im); }
static void Modifies(IMODE* mem) { ProcessIMModifies(mem, ModifyOne); }
static void ModifiesGosub() { ProcessUIVAddresses(ModifyOne); }

static void GCSEProcessBlock(Block* b)
{
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
                }
                else
                {
                    LookupEquivalence(&temp, head);
                    int n = temp.ans->offset->sp->i;
                    if (temp.dc.opcode == i_assn)
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
                                InsertTempEquivalence(head->ans, head->dc.left);
                            }
                        }
                        else
                        {
                            // mem or const
                            InsertMemEquivalence(head->dc.left, head->ans);
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
    forwardOrder.push_back(0);
    // last block has been visited
    blockArray[0]->visiteddfst = true;
    // calculate a foorward order to traverse the blocks, where each block is evaluated sometime after all its successors are
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
            // if a a succecessor has not been visited
            if (!bl->block->visiteddfst)
            {
                // mark it as visited
                bl->block->visiteddfst = true;
                // the the block to the work list to visit it
                workList.push_back(bl->block->blocknum);
                // we add it to the reverse order list here
                forwardOrder.push_back(bl->block->blocknum);
            }
            bl = bl->next;
        }
    }
    for (auto f : forwardOrder)
    {
        GCSEProcessBlock(blockArray[f]);
    }
    tempTranslation.clear();
    memTranslation.clear();
    expressionTranslation.clear();
}

}  // namespace Optimizer
