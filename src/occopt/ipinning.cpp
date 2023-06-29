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
#include <climits>
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

// this implementation is lacking in that if you have a structure which has a structure which has an array field
// it won't be taken care of...
//
namespace Optimizer
{
void Register(QUAD* head, IMODE* im, std::string name, std::map<std::string, std::deque<QUAD*>>& map,
              std::map<std::string, std::deque<QUAD*>>& automap)
{
    switch (im->offset->type)
    {
        case se_auto:
            automap[name].push_back(head);
            break;
        case se_global:
        case se_pc:
        case se_threadlocal:
        case se_labcon:
        case se_structelem:
            map[name].push_back(head);
            break;
    }
}
static void LoadAddresses(std::map<std::string, std::deque<QUAD*>>& map, std::map<std::string, std::deque<QUAD*>>& automap)
{
    QUAD* head = intermed_head;
    while (head)
    {
        if (head->dc.opcode == i_assn && !(head->temps & TEMP_LEFT) && head->dc.left->mode == i_immed)
        {
            if (head->fwd->dc.opcode != i_add || head->fwd->dc.right->mode != i_direct ||
                head->fwd->dc.right->offset->type != se_structelem)
            {
                switch (head->dc.left->offset->type)
                {
                    case se_auto:
                    case se_global:
                    case se_threadlocal:
                    case se_labcon: {
                        std::string name;
                        if (head->dc.left->offset->type == se_labcon)
                        {
                            char buf[256];
                            my_sprintf(buf, "%d", (int)head->dc.left->offset->i);
                            name = buf;
                        }
                        else
                        {
                            name = head->dc.left->offset->sp->outputName;
                        }
                        if (head->fwd->dc.opcode == i_add && head->fwd->dc.right->offset->type == se_structelem)
                        {
                            name = name + head->fwd->dc.right->offset->sp->outputName;
                            Register(head->fwd, head->fwd->dc.right, name, map, automap);
                        }
                        else
                        {
                            Register(head, head->dc.left, name, map, automap);
                        }
                    }
                }
            }
        }
        head = head->fwd;
    }
}
static void LoadDebugBlocks(std::deque<std::pair<QUAD*, QUAD*>>& blocks)
{
    QUAD* head = intermed_head;
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
        if (block.first->index <= begin && block.second->index >= end - 1)
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
    IMODE* ap = Allocate<IMODE>();
    sym->imvalue = ap;
    ap->offset = exp;
    ap->mode = i_direct;
    ap->size = exp->sizeFromType;
    return ap;
}
static void InsertInitialLoad(QUAD* begin, std::deque<QUAD*>& addresses, IMODE*& managed, IMODE*& unmanaged)
{
    while (begin->fwd->dc.opcode == i_label || begin->fwd->dc.opcode == i_line || begin->fwd->dc.opcode == i_blockend ||
           begin->fwd->dc.opcode == i_block)
        begin = begin->fwd;
    IMODE* exp;
    if (addresses.front()->dc.opcode == i_add)
    {
        exp = addresses.front()->dc.right;
    }
    else
    {
        exp = addresses.front()->dc.left;
    }
    SimpleType* tp;
    tp = Allocate<SimpleType>();
    tp->type = st_lref;
    tp->size = sizeFromISZ(ISZ_ADDR);
    tp->sizeFromType = ISZ_ADDR;
    if (exp->offset->type == se_labcon)
    {
        tp->btp = Allocate<SimpleType>();
        tp->btp->type = st_i;
        tp->btp->size = 1;
        tp->btp->sizeFromType = -ISZ_UCHAR;
    }
    else
    {
        tp->btp = exp->offset->sp->tp;
        if (tp->btp->isarray)
            tp->btp = tp->btp->btp;
        /*
        while (tp->btp->type == st_pointer)
        {
            SimpleType* tp1 = Allocate<SimpleType>();
            *tp1 = *tp->btp;
            tp1->type = st_lref;
            tp->btp = tp1;
            tp = tp->btp;
        }
        */
    }
    IMODE* ans = pinnedVar(tp);
    ans->size = exp->size;
    managed = ans;
    unmanaged = ans;

    if (addresses.front()->dc.opcode == i_add)
    {
        // address of something global points to
        QUAD* one = Allocate<QUAD>();
        *one = *addresses.front()->back;
        InsertInstruction(begin, one);
        begin = begin->fwd;
        QUAD* two = Allocate<QUAD>();
        *two = *addresses.front();
        InsertInstruction(begin, two);
        begin = begin->fwd;
        exp = two->ans;
    }
    QUAD* move = Allocate<QUAD>();
    move->ans = ans;
    move->dc.left = exp;
    move->dc.opcode = i_assn;
    InsertInstruction(begin, move);
    begin = begin->fwd;
}
static void ReplaceLoads(IMODE* managed, IMODE* unmanaged, std::deque<QUAD*>& addresses)
{
    for (auto a : addresses)
    {
        if (a->dc.opcode == i_add)
        {
            a->back->dc.left = unmanaged;
            a->back->ans = a->ans;
            RemoveInstruction(a);
        }
        else
        {
            a->dc.left = unmanaged;
        }
    }
}
static void InsertFinalThunk(IMODE* managed, QUAD* end)
{
    // in case of a return statement...
    while (end->back->dc.opcode == i_dbgblock || end->back->dc.opcode == i_line)
        end = end->back;
    if (end->back->dc.opcode == i_goto)
        end = end->back;
    QUAD* load = Allocate<QUAD>();
    load->ans = InitTempOpt(ISZ_ADDR, ISZ_ADDR);
    load->dc.left = make_immed(ISZ_ADDR, 0);
    load->dc.opcode = i_assn;
    QUAD* store = Allocate<QUAD>();
    store->ans = managed;
    store->dc.left = load->ans;
    store->dc.opcode = i_assn;
    InsertInstruction(end->back, load);
    InsertInstruction(load, store);
}

static bool Matches(std::deque<QUAD*>& group, QUAD* current)
{
    for (auto g : group)
    {
        for (int i = current->block->blocknum; i; i = blockArray[i]->idom)
        {
            if (i == g->block->blocknum)
                return true;
        }
    }
    return false;
}
static std::deque<std::deque<QUAD*>> Sort(std::deque<QUAD*>& addresses)
{
    std::deque<std::deque<QUAD*>> aa;
    for (auto a : addresses)
    {
        bool found = false;
        for (auto&& b : aa)
        {
            found = Matches(b, a);
            if (found)
            {
                b.push_back(a);
                break;
            }
        }
        if (!found)
        {
            std::deque<QUAD*> hold;
            hold.push_back(a);
            aa.push_back(hold);
        }
    }
    return aa;
}
void RewriteForPinning()
{
    std::map<std::string, std::deque<QUAD*>> addresses, autos;
    LoadAddresses(addresses, autos);
    if (addresses.size())
    {
        std::deque<std::pair<QUAD*, QUAD*>> blocks;
        LoadDebugBlocks(blocks);
        for (auto b : addresses)
        {
            // address of a global variable
            QUAD* ins;
            std::deque<std::deque<QUAD*>> aa = Sort(b.second);
            for (auto&& a : aa)
            {
                int begin = FindDominatingInstruction(a);
                int end = FindPostDominatingInstruction(a);
                std::pair<QUAD*, QUAD*> pair = FindDebugBlock(begin, end, blocks);
                IMODE *managed, *unmanaged;
                InsertInitialLoad(pair.first, a, managed, unmanaged);
                ReplaceLoads(managed, unmanaged, a);
                InsertFinalThunk(managed, pair.second);
            }
        }
    }
}
}  // namespace Optimizer