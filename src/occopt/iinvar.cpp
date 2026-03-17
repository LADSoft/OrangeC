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

#include "ioptimizer.h"
#include "beinterfdefs.h"
#include "iinvar.h"
#include "iblock.h"
#include "OptUtils.h"
#include "ioptutil.h"
#include "memory.h"
#include "ilive.h"
#include "ilocal.h"
#include "iloop.h"
#include "optmain.h"

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
namespace Optimizer
{
static void EnterRef(QUAD* old, QUAD* newVal)
{
    struct reflist* newRef = oAllocate<reflist>();
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
            tempInfo[refs->old->ans->offset->sp->i]->blockDefines = refs->old->block;
            tempInfo[refs->old->ans->offset->sp->i]->instructionDefines = refs->old;
        }
        refs = refs->next;
    }
}
static void keep(IMODE* l)
{
    if (l && l->mode == i_direct && l->offset->type == se_tempref)
    {
        QUAD* i = tempInfo[l->offset->sp->i]->instructionDefines;
        if (i->invarInserted)
        {
            i->invarKeep = true;
            keep(i->dc.left);
            keep(i->dc.right);
        }
    }
}
static bool IsAncestor(Block* b1, Block* b2)
{
    bool rv = false;
    if (b1)
    {
        Loop* lb1 = b1->loopParent;
        Loop* temp = b2->loopParent->parent;
        while (temp && !rv)
        {
            if (temp == lb1)
                rv = true;
            temp = temp->parent;
        }
    }
    return rv;
}
static bool Blocked(Block* one, Block* two)
{
    int i = one->blocknum;
    while (i)
    {
        if (blockArray[i]->head->moveBarrier)
            return true;
        int j = two->blocknum;
        while (j)
        {
            if (blockArray[j]->head->moveBarrier)
                return true;
            j = blockArray[j]->idom;
        }
        i = blockArray[i]->idom;
    }
    return false;
}
static void InsertAssign(Block* dest, QUAD* head, IMODE* ans, IMODE* left)
{
    QUAD* insert = beforeJmp(dest->tail, true);
    while (left != insert->ans && insert->dc.opcode != i_block && insert->dc.opcode != i_label)
        insert = insert->back;
    do
    {
        QUAD* assn = Allocate<QUAD>();
        *assn = *head;
        assn->temps = TEMP_LEFT | TEMP_ANS;
        assn->dc.left = left;
        assn->ans = ans;
        assn->fwd = insert->fwd;
        assn->back = insert;
        assn->block = dest;
        insert->fwd->back = assn;
        insert->fwd = assn;
        if (insert == dest->tail)
            dest->tail = assn;
        if (left == insert->ans)
            insert = insert->back;
        while (left != insert->ans && insert->dc.opcode != i_block && insert->dc.opcode != i_label)
            insert = insert->back;
    } while (insert->dc.opcode != i_block && insert->dc.opcode != i_label);
}
static int MoveTo(Block* dest, Block* src, QUAD* head, IMODE* first = nullptr, IMODE* last = nullptr)
{
    int rv = 0;
    QUAD* insert;
    if (last)
    {
        insert = dest->head;
        while (insert->ignoreMe || insert->dc.opcode == i_block || insert->dc.opcode == i_label || insert->dc.opcode == i_phi)
            insert = insert->fwd;
        insert = insert->back;
    }
    else
    {
        insert = beforeJmp(dest->tail, true);
    }
    rv = insert->index;
    QUAD* head2 = Allocate<QUAD>();
    *head2 = *head;
    if (!first && !last)
        EnterRef(head, head2);
    if (last)
        head2->dc.left = last;
    head = head2;
    if (insert == dest->tail)
        dest->tail = head;
    head->back = insert;
    head->fwd = insert->fwd;
    insert->fwd = insert->fwd->back = head;
    tempInfo[head->ans->offset->sp->i]->blockDefines = dest;
    tempInfo[head->ans->offset->sp->i]->instructionDefines = head;
    head->block = dest;
    head->invarInserted = true;
    if (!first && !last && (head->dc.opcode != i_assn || head->dc.left->mode == i_immed || (!(head->temps & TEMP_LEFT) && head->ans->offset->sp->loadTemp)))
    {
        keep(head->dc.left);
        keep(head->dc.right);
        head->invarKeep = true;
    }
    return rv;
}
static void MoveExpression(Block* b, QUAD* head, Block* pbl, Block* pbr)
{
    if (IsAncestor(pbl, b) && !Blocked(pbl, b))
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
static bool isPhiUsing(Loop* considering, int temp)
{
    bool rv = false;
    if (temp != -1 && considering)
    {
        rv = isset(considering->invariantPhiList, temp);
    }
    return rv;
}
static bool InvariantPhiUsing(QUAD* head)
{
    int ans = head->ans->offset->sp->i;
    bool rv = false;
    int left = -1, right = -1;
    Loop* considering = head->block->loopParent;
    if (tempInfo[ans]->preSSATemp >= 0)
        rv = !tempInfo[tempInfo[ans]->preSSATemp]->enode->sp->pushedtotemp;
    if (head->temps & TEMP_LEFT)
        left = head->dc.left->offset->sp->i;
    if (head->temps & TEMP_RIGHT)
        right = head->dc.right->offset->sp->i;
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
void ScanForInvariants(Block* b)
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
                Loop* parent = head->block->inclusiveLoopParent;
                struct _phiblock* pb;
                if (!parent->invariantPhiList)
                {
                    Loop* last = parent->parent;
                    while (last && !last->invariantPhiList)
                        last = last->parent;

                    parent->invariantPhiList = allocbit(tempCount);
                    if (last)
                        memcpy(parent->invariantPhiList, last->invariantPhiList,
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
                tempInfo[head->ans->offset->sp->i]->blockDefines = b;
                if (!tempInfo[head->ans->offset->sp->i]->inductionLoop && (head->temps & (TEMP_LEFT | TEMP_RIGHT)))
                {
                    bool canMove = true;
                    Block *pbl = nullptr, *pbr = nullptr;
                    if ((head->temps & TEMP_LEFT) && head->dc.left->mode == i_direct)
                    {
                        pbl = tempInfo[head->dc.left->offset->sp->i]->blockDefines;
                    }
                    else if (head->dc.left->mode != i_immed)
                        canMove = false;
                    if ((head->temps & TEMP_RIGHT) && head->dc.right->mode == i_direct)
                    {
                        pbr = tempInfo[head->dc.right->offset->sp->i]->blockDefines;
                    }
                    else if (head->dc.right && head->dc.right->mode != i_immed)
                        canMove = false;
                    if (head->atomic)
                        canMove = false;
                    if (canMove)
                        canMove = InvariantPhiUsing(head);
                    if (canMove)
                        if (pbl && pbl->preWalk != b->preWalk && pbl->nesting == b->nesting &&
                            (!pbr || (pbr->preWalk != b->preWalk && pbr->nesting == b->nesting)))
                            MoveExpression(b, head, pbl, pbr);
                }
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
static int CommonLoop(int n1, int n2)
{
    if (n1 < n2)
    {
        auto parent = loopArray[n1];
        while (parent && parent->loopnum != n2)
            parent = parent->parent;
        if (parent)
            return n2;
    }
    else
    {
        auto parent = loopArray[n2];
        while (parent && parent->loopnum != n1)
            parent = parent->parent;
        if (parent)
            return n1;

    }
    return -1;
}
static int CalculateLoop(Block*  block,  std::unordered_map<int, int>& blockToLoop)
{
    int ln;
    auto it = blockToLoop.find(block->blocknum);
    if (it != blockToLoop.end())
    {
        ln = it->second;
    }
    else
    {
        ln = block->loopParent->loopnum;
    }
    return ln;
}
IMODE* loadVarInd(IMODE* loadVar, int size)
{
    IMODELIST* iml = loadVar->offset->sp->imind;
    while (iml)
    {
        if (iml->im->size == size)
            return iml->im;
        iml = iml->next;
    }
    auto rv = Allocate<IMODE>();
    *rv = *loadVar;
    rv->mode = i_ind;
    rv->ptrsize = rv->size;
    rv->size = size;
    iml = Allocate<IMODELIST>();
    iml->im = rv;
    iml->next = loadVar->offset->sp->imind;
    loadVar->offset->sp->imind = iml;
    return rv;
}
// this part is done outside the SSA process
void ScanForVariableMotion(void)
{
    if (loopCount < 2 || loopArray[loopCount - 2]->type == LT_BLOCK)
        return;
    // this first one is a map because we iterated over it, and this resulted in variances in code generation
    // when compiled with one compiler verses another
    // which we can't have for purposes of the testing
    std::map<IMODE*, std::list<QUAD*>> uses;
    std::unordered_map<IMODE*, std::list<QUAD*>> defines;
    std::unordered_map<int, std::list<QUAD*>> loads;
    std::unordered_map<int, int> loopDoms;
    std::unordered_map<int, int> blockToLoop;
    std::set<int> gosubBlocks;
    QUAD* head = intermed_head;
    bool changed = true;
    // find all the uses for in-memory variables
    int index = 0;
    while (head)
    {
        head->index = ++index;

        if (head->back && head->back->block != head->block)
            changed = true;
        if (!head->ignoreMe && head->dc.opcode != i_assnblock)
        {
            if ((head->temps & TEMP_ANS) && !(head->temps & TEMP_LEFT) && head->ans->offset->sp->loadTemp &&
                head->dc.left->offset->type >= se_absolute && head->dc.left->offset->type <= se_global &&
                head->dc.left->size < ISZ_FLOAT && 
                !head->dc.left->offset->sp->addressTaken && !head->dc.left->offset->sp->tp->isvolatile)
            {
                uses[head->dc.left].push_back(head);
                loads[head->ans->offset->sp->i] = {};
                defines[head->dc.left] = {};
            }
        }
        if (head->dc.opcode == i_gosub || head->dc.opcode == i_cmpxchgstrong || head->dc.opcode == i_cmpxchgweak)
        {
            if (changed)
            {
                changed = false;
                gosubBlocks.insert(head->block->blocknum);
            }
        }
        head = head->fwd;
    }
    head = intermed_head;
    // find all the definitions for in-memory variables
    while (head)
    {
        if (!head->ignoreMe && head->dc.opcode != i_assnblock)
        {
            if (head->dc.opcode == i_assn && head->ans->offset->type != se_tempref && head->ans->size < ISZ_FLOAT)
            {
                auto it = defines.find(head->ans);
                if (it != defines.end())
                {
                    defines[head->ans].push_back(head);
                }
            }
            if ((head->temps & TEMP_ANS) && (head->ans->size < ISZ_FLOAT || head->ans->mode == i_ind))
            {
                auto&& load = loads.find(head->ans->offset->sp->i);
                if (load != loads.end())
                    load->second.push_back(head);
            }
            if ((head->temps & TEMP_LEFT) && (head->dc.left->size < ISZ_FLOAT || head->dc.left->mode == i_ind))
            {
                auto&& load = loads.find(head->dc.left->offset->sp->i);
                if (load != loads.end())
                    load->second.push_back(head);
            }
            if ((head->temps & TEMP_RIGHT) && (head->dc.right->size < ISZ_FLOAT || head->dc.right->mode == i_ind))
            {
                auto&& load = loads.find(head->dc.right->offset->sp->i);
                if (load != loads.end())
                    load->second.push_back(head);
            }
        }
        head = head->fwd;
    }
    // calculate the loop number for each definition and use
    for (auto loop = 0; loop < loopCount; loop++)
    {
        auto lp = loopArray[loop];
        if (lp->type != LT_BLOCK && lp->type != LT_ROOT)
        {
            loopDoms[blockArray[lp->entry->idom]->idom] = lp->loopnum;
        }
    }
    for (auto&& u : uses)
    {
        for (auto&& u1 : u.second)
        {
            int b = u1->block->blocknum;
            auto it = loopDoms.find(b);
            if (it  != loopDoms.end())
            {
                int n = blockArray[b]->loopParent->loopnum;
                for (; b < blockCount && blockArray[b]->loopParent->loopnum == n; b++)
                {
                    blockToLoop[b] = it->second;
                }
            }
        }
    }
    for (auto&& d : defines)
    {
        for (auto&& d1 : d.second)
        {
            int b = d1->block->blocknum;
            auto it = loopDoms.find(b);
            if (it != loopDoms.end())
            {
                int n = blockArray[b]->loopParent->loopnum;
                for (; b < blockCount && blockArray[b]->loopParent->loopnum == n; b++)
                {
                    blockToLoop[b] = it->second;
                }
            }
        }
    }

    for (auto&& u : uses)
    {
        if (u.second.size())
        {
            // calculate list of loops for this variable
            // uses in the entryway are translated
            // to the next lower loop, through use of the Dom propery
            std::list<int> loopList;
            for (auto t : u.second)
            {
                loopList.push_back(CalculateLoop(t->block, blockToLoop));
            }
            // now get rid of duplicates and find the outermost loops
            bool changed = true;
            while (changed)
            {
                changed = false;
                loopList.sort();
                // get rid of duplicates
                for (auto it = loopList.begin(); it != loopList.end(); )
                {
                    int n = *it;
                    ++it;
                    while (it != loopList.end() && *it == n)
                    {
                        it = loopList.erase(it);
                    }
                }
                // find outermost loops
                for (auto it = loopList.begin(); it != loopList.end(); ++it)
                {
                    auto it1 = it;
                    ++it1;
                    for (; it1 != loopList.end(); ++it1)
                    {
                        if (*it != *it1)
                        {
                            int n = CommonLoop(*it, *it1);
                            if (n >= 0)
                            {
                                *it = *it1 = n;
                                changed = true;
                            }
                        }
                    }
                }
            }
            // add definitions to each successor
            // rename the memory location and make it a pushedtotemp...
            auto sz = u.second.front()->ans->size;
            IMODE* tempVar = nullptr;
            IMODE* loadVar = nullptr;

            // now we have a list of outermost loops...
            for (auto loop : loopList)
            {
                Loop* parent = loopArray[loop];
                Block* dom = blockArray[blockArray[parent->entry->idom]->idom];
                int firstUseInDominator = 0;
                std::list<QUAD*> loopUses, loopDefines, loopLoads;
                // figure out if there are any uses this loop
                for (auto t : u.second)
                {
                    auto ln = CalculateLoop(t->block, blockToLoop);
                    int n = CommonLoop(loop, ln);
                    if (n == loop)
                    {
                        if (!firstUseInDominator && t->block == dom)
                            firstUseInDominator = t->index;
                        loopUses.push_back(t);
                    }
                }
                if ((firstUseInDominator && loopUses.size() > 1) || (!firstUseInDominator && loopUses.size()))
                {
                    head = loopUses.front();
                    QUAD* definedInDominator = nullptr;
                    // yes get a list of definitions this loop
                    for (auto t : defines[head->dc.left])
                    {
                        auto ln = CalculateLoop(t->block, blockToLoop);
                        int n = CommonLoop(loop, ln);
                        if (n == loop)
                        {
                            if (t->block == dom)
                                definedInDominator = t;
                            loopDefines.push_back(t);
                        }
                    }
                    for (auto t : loads[head->ans->offset->sp->i])
                    {
                        auto ln = CalculateLoop(t->block, blockToLoop);
                        int n = CommonLoop(loop, ln);
                        if (n == loop)
                            loopLoads.push_back(t);
                    }
                    if (parent->type != LT_ROOT || !loopDefines.size())
                    {
                        bool defineAtEnd = (definedInDominator && loopDefines.size() > 1) || (!definedInDominator && loopDefines.size());
                        bool doit = false;
                        if (head->dc.left->offset->sp->storage_class == scc_parameter)
                        {
                            // if it is a parameter we can do this optimization
                            doit = true;
                        }
                        else
                        {
                            // else we can only do this optimization if nothing external would modify the variable
                            for (int i = 0; i < parent->blocks->top; i++)
                            {
                                doit = true;
                                if (gosubBlocks.find(parent->blocks->data[i]) != gosubBlocks.end())
                                {
                                    doit = false;
                                    break;
                                }
                            }
                        }
                        if (doit)
                        {
                            if (!tempVar)   
                            {
                                tempVar = InitTempOpt(sz, sz);
                                tempVar->offset->sp->pushedtotemp = true;
                                tempVar->offset->sp->invartemp = true;
                                tempVar->offset->sp->imvalue = tempVar;
                                loadVar = InitTempOpt(sz, sz);
                                loadVar->offset->sp->imvalue = loadVar;
                                loadVar->offset->sp->loadTemp = true;
                            }
                            // add a definition to the dominator
                            // this will be a duplicate of anything previously there as we need to rename the answer temp....
                            if (!firstUseInDominator)
                            {
                                firstUseInDominator = MoveTo(dom, head->block, head, tempVar);
                            }
                            InsertAssign(dom, head, tempVar, head->ans);
                            InsertAssign(dom, head, loadVar, tempVar);
                            if (defineAtEnd)
                            {
                                for (auto succ = parent->successors; succ; succ = succ->next)
                                {
                                    MoveTo(succ->block, loopDefines.front()->block, loopDefines.front(), nullptr, tempVar);
                                }
                            }
                            // now rename all uses within the loop
                            for (auto u : loopUses)
                            {
                                if (dom != u->block || (!definedInDominator && firstUseInDominator && u->index > firstUseInDominator))
                                {
                                    u->dc.left = tempVar;
                                    u->temps |= TEMP_LEFT;
                                }
                            }
                            if (defineAtEnd)
                            {
                                for (auto d : loopDefines)
                                {
                                    if (dom != d->block || (firstUseInDominator && d->index > firstUseInDominator))
                                    {
                                        d->ans = tempVar;
                                        d->temps |= TEMP_ANS;
                                    }
                                }
                            }

                            int loadVarNum = head->ans->offset->sp->i;
                            for (auto load : loopLoads)
                            {
                                if ((load->temps & TEMP_ANS) && load->ans->offset->sp->i == loadVarNum)
                                {
                                    if (load->ans->mode == i_ind)
                                    {
                                        load->ans = loadVarInd(loadVar, load->ans->size);
                                    }
                                    else if (load->temps & TEMP_LEFT)
                                    {
                                        load->ans = loadVar;
                                    }
                                }
                                if ((load->temps & TEMP_LEFT) && load->dc.left->offset->sp->i == loadVarNum)
                                {
                                    if (load->dc.left->mode == i_ind)
                                    {
                                        load->dc.left = loadVarInd(loadVar, load->dc.left->size);
                                    }
                                    else
                                    {
                                        load->dc.left = loadVar;
                                    }
                                }
                                if ((load->temps & TEMP_RIGHT) && load->dc.right->offset->sp->i == loadVarNum)
                                {
                                    if (load->dc.right->mode == i_ind)
                                    {
                                        load->dc.right = loadVarInd(loadVar, load->dc.right->size);
                                    }
                                    else
                                    {
                                        load->dc.right = loadVar;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
// Before going into SSA
void MoveLoopVariables(void)
{
    if (Optimizer::architecture != ARCHITECTURE_MSIL)
    {
        refs = nullptr;
        ScanForVariableMotion();
        // not weeding because we want to keep everything...
    }
}
// while in SSA
void MoveLoopInvariants(void)
{
    int i;
    refs = nullptr;
    return;
    for (i = 0; i < blockCount; i++)
        if (blockArray[i])
            blockArray[i]->preWalk = 0;
    for (i = 0; i < tempCount; i++)
        tempInfo[i]->blockDefines = nullptr;
    current = 1;
    ScanForInvariants(blockArray[0]);
    WeedRefs();
}
}  // namespace Optimizer