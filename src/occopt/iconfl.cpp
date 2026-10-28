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
#include <unordered_map>
#include "ioptimizer.h"
#include "beinterfdefs.h"
#include "config.h"
#include "iblock.h"
#include "OptUtils.h"
#include "irc.h"
#include "optmain.h"
#include "memory.h"
#include "iconfl.h"
#include <functional>
#include <algorithm>
#include <ilive.h>
namespace Optimizer
{
static std::unordered_map<int, std::unordered_set<int>> conflicts;

void conflictini(void) { ResetConflict(); }
void EvaluateConflicts(int n, std::function<void(int)> func) { std::for_each(conflicts[n].begin(), conflicts[n].end(), func); }
void ResetConflict(void) { conflicts.clear(); }
int findPartition(int T0)
{
    while (T0 != tempInfo[T0]->partition)
        T0 = tempInfo[T0]->partition;
    return T0;
}
bool isConflicting(int T0, int T1)
{
    int bucket;
    T0 = findPartition(T0);
    T1 = findPartition(T1);
    if (T0 == T1)
        return false;
    return conflicts[T0].find(T1) != conflicts[T0].end();
}
void insertConflict(int i, int j)
{
    TempInfo *ti, *tj;
    int bucket;
    i = findPartition(i);
    j = findPartition(j);
    if (i == j)
        return;
    if (isConflicting(i, j))
        return;
    ti = tempInfo[i];
    tj = tempInfo[j];
    if (maxAddr && ti->usedAsAddress != tj->usedAsAddress)
        return;
    if (ti->usedAsFloat != tj->usedAsFloat)
        return;
    conflicts[i].insert(j);
    conflicts[j].insert(i);
}
void CalculateConflictGraph(std::unordered_set<int>& nodes, bool optimize)
{
    int i;
    SimpleBitSet live(tempCount);
    SimpleBitSet nodes1(tempCount);
    ResetConflict();
    for (auto j : nodes)
        nodes1.setbit(j);
    for (i = 0; i < blockCount; i++)
    {

        if (blockArray[i])
        {
            QUAD* tail = blockArray[i]->tail;
            QUAD* head = blockArray[i]->head;
            int k;
            live.clear();
            live = liveOutTemps[i];
            if (!nodes.empty())
                live.dointersect(nodes1, live);
            do
            {
                InternalConflict(tail);
                if (tail->dc.opcode == i_phi)
                {
                    PHIDATA* pd = tail->dc.v.phi;
                    struct _phiblock* pb = pd->temps;
                    if (nodes.empty() || nodes1.isset(pd->T0))
                    {
                        for (int i = 0; i < live.size(); i++)
                        {
                            if (live.isset(i))
                                insertConflict(i, pd->T0);
                        }
                    }
                    while (pb)
                    {
                        if (nodes.empty() || nodes1.isset(pb->Tn))
                        {
                            struct _phiblock* pb2 = pd->temps;
                            while (pb2)
                            {
                                if (nodes.empty() || nodes1.isset(pb2->Tn))
                                {
                                    if (live.isset(pb->Tn) || live.isset(pb2->Tn))
                                        insertConflict(pb->Tn, pb2->Tn);
                                }
                                pb2 = pb2->next;
                            }
                        }
                        pb = pb->next;
                    }
                    pb = pd->temps;
                    while (pb)
                    {
                        if (nodes.empty() || nodes1.isset(pb->Tn))
                        {
                            live.setbit(pb->Tn);
                        }
                        pb = pb->next;
                    }
                }
                else if (tail->dc.opcode == i_assnblock)
                {
                    if (tail->dc.right->offset->type == se_tempref && tail->dc.left->offset->type == se_tempref)
                    {
                        insertConflict(tail->dc.right->offset->sp->i, tail->dc.left->offset->sp->i);
                    }
                }
                else if (tail->dc.opcode == i_gosub)
                {
                    if (tail->fastcall)
                    {
                        QUAD* x = tail->back;
                        while (x->dc.opcode != i_gosub && x->dc.opcode != i_block && x->dc.opcode != i_parmstack)
                        {
                            if (x->fastcall > 0)
                            {
                                live.setbit(x->dc.left->offset->sp->i);
                            }
                            x = x->back;
                        }
                    }
                }
                else if (tail->dc.opcode == i_inlineasm)
                {
                    if (optimize)
                    {
                        // dest reg
                        if (tail->assemblyRegs[0] != 255)
                        {
                            live.clrbit(tail->assemblyTempRegStart);
                            for (int i = 0; i < live.size(); i++)
                            {
                                if (live.isset(i))
                                    insertConflict(i, tail->assemblyTempRegStart);
                            }
                        }
                        // src regs
                        for (int i = 1; i < tail->assemblyRegCount; i++)
                            if (tail->assemblyRegs[i] != 255)
                                live.setbit(tail->assemblyTempRegStart + i);
                    }
                }
                else if (tail->temps & TEMP_ANS)
                {
                    if (tail->ans->mode == i_direct)
                    {
                        int tnum = tail->ans->offset->sp->i;
                        int k = -1;
                        if (nodes.empty() || nodes1.isset(tnum))
                        {
                            if (optimize)
                            {
                                if (tail->dc.opcode == i_assn)
                                {
                                    if ((tail->temps & (TEMP_LEFT | TEMP_ANS)) == (TEMP_LEFT | TEMP_ANS))
                                    {
                                        if (tail->dc.left->mode == i_direct && tail->ans->mode == i_direct && !tail->ans->bits &&
                                            !tail->dc.left->bits && !tail->dc.left->retval)
                                        {
                                            if (tail->dc.left->size == tail->ans->size)
                                            {
                                                if (!tail->ans->offset->sp->pushedtotemp &&
                                                    !tail->dc.left->offset->sp->pushedtotemp)
                                                {
                                                    k = tail->dc.left->offset->sp->i;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            live.clrbit(tnum);
                            // processor dependent
                            if (tail->atomic && (tail->temps & TEMP_RIGHT) && tail->ans->size >= ISZ_FLOAT &&
                                tail->ans->size != ISZ_BITINT)
                            {
                                // fetch_modify or modify_fetch may need an extra floating point register allocated...
                                insertConflict(tnum, tail->dc.right->offset->sp->i);
                            }
                            if (tail->ans->size == ISZ_ULONGLONG || tail->ans->size == -ISZ_ULONGLONG)
                            {
                                if (tail->dc.left->mode == i_ind)
                                {
                                    //                                    if (tail->dc.left->offset)
                                    //                                        insertConflict(tnum,
                                    //                                        tail->dc.left->offset->sp->i);
                                    if (tail->dc.left->offset2)
                                        insertConflict(tnum, tail->dc.left->offset2->sp->i);
                                }
                                if (tail->dc.right && tail->dc.right->mode == i_ind)
                                {
                                    if (tail->dc.right->offset)
                                        insertConflict(tnum, tail->dc.right->offset->sp->i);
                                    if (tail->dc.right->offset2)
                                        insertConflict(tnum, tail->dc.right->offset2->sp->i);
                                }
                            }
                            for (int i = 0; i < live.size(); i++)
                            {
                                if (i != k && live.isset(i))
                                    insertConflict(i, tnum);
                            }
                        }
                    }
                    else if (tail->ans->mode == i_ind)
                    {
                        if (tail->ans->offset)
                        {
                            int tnum = tail->ans->offset->sp->i;
                            if (nodes.empty() || nodes1.isset(tnum))
                            {
                                live.setbit(tnum);
                            }
                        }
                        if (tail->ans->offset2)
                        {
                            int tnum = tail->ans->offset2->sp->i;
                            if (nodes.empty() || nodes1.isset(tnum))
                            {
                                live.setbit(tnum);
                            }
                        }
                    }
                }
                if ((tail->temps & TEMP_LEFT) && !tail->dc.left->retval)
                {
                    if (tail->dc.left->offset)
                    {
                        int tnum = tail->dc.left->offset->sp->i;
                        if (nodes.empty() || nodes1.isset(tnum))
                        {
                            live.setbit(tnum);
                        }
                    }
                    if (tail->dc.left->offset2)
                    {
                        int tnum = tail->dc.left->offset2->sp->i;
                        if (nodes.empty() || nodes1.isset(tnum))
                        {
                            live.setbit(tnum);
                        }
                    }
                }
                if (tail->temps & TEMP_RIGHT)
                {
                    if (tail->dc.right->offset)
                    {
                        int tnum = tail->dc.right->offset->sp->i;
                        if (nodes.empty() || nodes1.isset(tnum))
                        {
                            live.setbit(tnum);
                        }
                    }
                    if (tail->dc.right->offset2)
                    {
                        int tnum = tail->dc.right->offset2->sp->i;
                        if (nodes.empty() || nodes1.isset(tnum))
                        {
                            live.setbit(tnum);
                        }
                    }
                }
                if (tail != head)
                    tail = tail->back;
            } while (tail != head);
        }
    }
}
}  // namespace Optimizer
