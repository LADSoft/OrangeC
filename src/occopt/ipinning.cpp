/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <limits.h>
#include "ioptimizer.h"
#include "beinterfdefs.h"
#include "ildata.h"
#include "../occ/winmode.h"
#include "../occ/be.h"
#include "SharedMemory.h"
#include <deque>
#include <functional>
#include <map>
#include <set>
#include <stack>
#include "ildata.h"
#include "iblock.h"
#include "memory.h"
#include "ilocal.h"
#include "OptUtils.h"
#include "ilive.h"

extern Optimizer::SimpleSymbol* currentFunction;


namespace Optimizer
{
static void LoadAddresses(std::map<std::string, std::deque<QUAD*>>& map, std::map<std::string, std::deque<QUAD*>>& automap)
{
    QUAD *head = intermed_head;
    while (head)
    {
        if (head->dc.opcode == i_assn && !(head->temps & TEMP_LEFT) && head->dc.left->mode == i_immed)
        {
            if (head->fwd->dc.opcode != i_add || head->fwd->dc.right->mode != i_direct || head->fwd->dc.right->offset->type != se_structelem)
            {
                switch (head->dc.left->offset->type)
                {
                    case se_auto:
                        automap[head->dc.left->offset->sp->outputName].push_back(head);
                        break;
                    case se_global:
                    case se_pc:
                    case se_threadlocal:
                        map[head->dc.left->offset->sp->outputName].push_back(head);
                        break;
                    case se_labcon:
                    {
                        char buf[256];
                        sprintf(buf, "%d", head->dc.left->offset->i);
                        map[buf].push_back(head);
                        break;
                    }
                }
            }
        }
        head = head->fwd;
    }
}
static void LoadDebugBlocks(std::deque<std::pair<QUAD*, QUAD*>>& blocks)
{
    QUAD *head = intermed_head;
    int count = 0;
    while (head)
    {
        head->index = count++;
        switch (head->dc.opcode)
        {
        case i_dbgblock:
            blocks.push_back(std::pair<QUAD*, QUAD*>(head, nullptr));
            break;
        case i_dbgblockend:
            for (auto it = blocks.rbegin(); it != blocks.rend(); ++it)
            {
                if (it->second == nullptr)
                {
                    it->second = head;
                    break;
                }
            }
            break;
        }
        head = head->fwd;
    }
}
static int top(int b1, int b2)
{
    if (b1 == b2)
        return b1;
    while (b1)
    {
        int test = b2;
        while (test)
        {
            if (b1 == test)
                break;
            test = blockArray[test]->idom;
        }
        if (b1 == test)
            break;
        int b3 = blockArray[b1]->idom;
        while (b3 > b1)
            b3 = blockArray[b3]->idom;
        b1 = b3;
    }
    return b1;
}
static int bottom(int b1, int b2)
{
    if (b1 == b2)
        return b1;
    while (b1)
    {
        int test = b2;
        while (test != exitBlock)
        {
            if (b1 == test)
                break;
            test = blockArray[test]->pdom;
            if (test == 0)
                test = exitBlock;
        }
        if (b1 == test)
            break;
        int b3 = blockArray[b1]->pdom;
        if (b3 == 0)
            b3 = exitBlock;
        while (b3 < b1)
        {
            b3 = blockArray[b3]->pdom;
            if (b3 == 0)
                b3 = exitBlock;
        }
        b1 = b3;
    }
    return b1;
}
static int FindDominatingInstruction(std::deque<QUAD*>& instructions)
{
    bool first = true;
    int current = 0;
    for (auto i : instructions)
    {
        if (first)
        {
            current = i->block->blocknum;
            first = false;
        }
        else
        {
            current = top(current, i->block->blocknum);
        }
    }
    for (auto i : instructions)
    {
        if (i->block->blocknum == current)
            return i->index;
    }
    return blockArray[current]->head->index;
}
static int FindPostDominatingInstruction(std::deque<QUAD*>& instructions)
{
    bool first = true;
    int current = 0;
    for (auto i : instructions)
    {
        if (first)
        {
            current = i->block->blocknum;
            first = false;
        }
        else
        {
            current = bottom(current, i->block->blocknum);
        }
    }
    int ins = 0;
    for (auto i : instructions)
    {
        if (i->block->blocknum == current)
            ins = i->index;
    }
    if (ins != 0)
        return ins;
    return blockArray[current]->tail->index;
}
static std::pair<QUAD*, QUAD*> FindDebugBlock(int begin, int end, std::deque<std::pair<QUAD*, QUAD*>>& blocks)
{
    std::pair<QUAD*, QUAD*> rv;
    int spread = INT_MAX;
    for (auto&& block : blocks)
    { 
        if (block.first->index <= begin && block.second->index >= end)
        {
            int spread1 = block.second->index - block.first->index;
            if (spread1 < spread)
            {
                rv = block;
                spread = spread1;
            }
        }
    }
    if (spread == INT_MAX)
        rv = blocks.front();
    return rv;
}

static IMODE* pinnedVar(SimpleType* tp)
{
    SimpleExpression* exp = anonymousVar(scc_auto, tp);
    SimpleSymbol* sym = exp->sp;
    exp->sizeFromType = tp->sizeFromType;
    sym->sizeFromType = tp->sizeFromType;
    sym->anonymous = false;
    tp->pinned = true;
    IMODE* ap = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
    sym->imvalue = ap;
    ap->offset = exp;
    ap->mode = i_direct;
    ap->size = exp->sizeFromType;
    return ap;
}
static void InsertInitialLoad(QUAD* begin, std::deque<QUAD*>& addresses, IMODE*&managed, IMODE*&unmanaged)
{
    IMODE *left = addresses.front()->dc.left;
    SimpleType *tp;
    tp = (SimpleType*)Alloc(sizeof(SimpleType));
    tp->btp = (SimpleType*)Alloc(sizeof(SimpleType));
    tp = (SimpleType*)Alloc(sizeof(SimpleType));
    tp->type = st_lref;
    tp->size = sizeFromISZ(ISZ_ADDR);
    tp->sizeFromType = ISZ_ADDR;
    if (left->offset->type == se_labcon)
    {
        tp->btp = (SimpleType*)Alloc(sizeof(SimpleType));
        tp->btp->type = st_i;
        tp->btp->size = 1;
        tp->btp->sizeFromType = -ISZ_UCHAR;
    }
    else
    {
        tp->btp = left->offset->sp->tp;
        if (tp->btp->isarray)
            tp->btp = tp->btp->btp;
    }
    IMODE *ans = pinnedVar(tp);
    ans->size = left->size;
    QUAD *move = (QUAD*)Alloc(sizeof(QUAD));
    move->ans = ans;
    move->dc.left = left;
    move->dc.opcode = i_assn;
    InsertInstruction(begin, move);
    IMODE* ans2 = (IMODE*) Alloc(sizeof(IMODE));
    *ans2 = *ans;
    ans2->size = ISZ_UINT;
    managed = ans;
    unmanaged = ans2;
}
static void ReplaceLoads(IMODE* managed, IMODE* unmanaged, std::deque<QUAD*>& addresses)
{
    for (auto a : addresses)
    {
        a->dc.left = unmanaged;
    }
}
static QUAD* WadeToEndOfDbgBlock(QUAD *head)
{
    QUAD* orig = head;
    int count = 1;
    while (head)
    {
        if (head->dc.opcode == i_dbgblock)
            count++;
        else if (head->dc.opcode == i_dbgblockend)
            if (!--count)
               return head;
        head = head->fwd;
    }
    return orig;
}
static void InsertFinalThunk(IMODE* managed, QUAD* end)
{
    QUAD* load = (QUAD*)Alloc(sizeof(QUAD));
    load->ans = InitTempOpt(ISZ_UINT, ISZ_UINT);
    load->dc.left = make_immed(ISZ_UINT, 0);
    load->dc.opcode = i_assn;
    QUAD* store = (QUAD*)Alloc(sizeof(QUAD));
    store->ans = managed;
    store->dc.left = load->ans;
    store->dc.opcode = i_assn;
    InsertInstruction(end->back, load);
    InsertInstruction(load, store);
}


void RewriteForPinning()
{
    std::map<std::string, std::deque<QUAD*>> addresses, autos;
    LoadAddresses(addresses, autos);
    if (addresses.size())
    {
        std::deque<std::pair<QUAD*, QUAD*>> blocks;
        LoadDebugBlocks(blocks);
        for (auto a : addresses)
        {
            int begin = FindDominatingInstruction(a.second);
            int end = FindPostDominatingInstruction(a.second);
            std::pair<QUAD*, QUAD*> pair = FindDebugBlock(begin, end, blocks);
            IMODE* managed, *unmanaged;
            InsertInitialLoad(pair.first, a.second, managed, unmanaged);
            ReplaceLoads(managed, unmanaged, a.second);
            InsertFinalThunk(managed, pair.second);
        }
    }
    if (autos.size())
    {
        for (auto a : autos)
        {
            for (auto s : a.second)
            {
                s->dc.left->size = ISZ_UINT;
            }
        }
    }
}
}