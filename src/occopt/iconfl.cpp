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
#include "config.h"
#include "iblock.h"
#include "OptUtils.h"
#include "irc.h"
#include "optmain.h"
#include "memory.h"
#include "ioptutil.h"

namespace Optimizer
{
void conflictini(void) {}
void resetConflict(void)
{
    int i;
    for (i = 0; i < tempCount; i++)
    {
        tempInfo[i]->conflicts = callocbit(tempCount);
        tempInfo[i]->neighbors = 0;
    }
}
int findPartition(int T0)
{
    while (T0 != tempInfo[T0]->partition)
        T0 = tempInfo[T0]->partition;
    return T0;
}
void insertConflict(int i, int j)
{
    TEMP_INFO *ti, *tj;
    int bucket;
    i = findPartition(i);
    j = findPartition(j);
    if (i == j)
        return;
    if (isset(tempInfo[i]->conflicts, j))
        return;
    ti = tempInfo[i];
    tj = tempInfo[j];
    if (maxAddr && ti->usedAsAddress != tj->usedAsAddress)
        return;
    if (ti->usedAsFloat != tj->usedAsFloat)
        return;
    setbit(ti->conflicts, j);
    setbit(tj->conflicts, i);
}
static void JoinOneList(int T0, int T1)
{
    int i;
    BITINT* t0 = tempInfo[T0]->conflicts;
    BITINT* t1 = tempInfo[T1]->conflicts;
    return;
    for (i = 0; i < (tempCount + BITINTBITS - 1) / BITINTBITS; i++)
        *t1 = *t0 |= *t1;
}
void JoinConflictLists(int T0, int T1)
{
    JoinOneList(T0, T1);
    JoinOneList(T1, T0);
}
bool isConflicting(int T0, int T1)
{
    int bucket;
    T0 = findPartition(T0);
    T1 = findPartition(T1);
    if (T0 == T1)
        return false;
    return !!isset(tempInfo[T0]->conflicts, T1);
}
void CalculateConflictGraph(BRIGGS_SET* nodes, bool optimize)
{
    int i, j;
    BRIGGS_SET* live = briggsAllocc(tempCount);
    resetConflict();
    for (i = 0; i < blockCount; i++)
    {

        if (blockArray[i])
        {
            BITINT* p = blockArray[i]->liveOut;
            QUAD* tail = blockArray[i]->tail;
            QUAD* head = blockArray[i]->head;
            int j, k;
            briggsClear(live);
            for (j = 0; j < (tempCount + BITINTBITS - 1) / BITINTBITS; j++, p++)
                if (*p)
                    for (k = 0; k < BITINTBITS; k++)
                        if (*p & (1 << k))
                        {
                            if (!nodes || briggsTest(nodes, j * BITINTBITS + k))
                                briggsSet(live, j * BITINTBITS + k);
                        }
            do
            {
                InternalConflict(tail);
                if (tail->dc.opcode == i_phi)
                {
                    PHIDATA* pd = tail->dc.v.phi;
                    struct _phiblock* pb = pd->temps;
                    if (!nodes || briggsTest(nodes, pd->T0))
                    {
                        for (j = 0; j < live->top; j++)
                        {
                            insertConflict(live->data[j], pd->T0);
                        }
                    }
                    while (pb)
                    {
                        if (!nodes || briggsTest(nodes, pb->Tn))
                        {
                            struct _phiblock* pb2 = pd->temps;
                            while (pb2)
                            {
                                if (!nodes || briggsTest(nodes, pb2->Tn))
                                {
                                    if (briggsTest(live, pb->Tn) || briggsTest(live, pb2->Tn))
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
                        if (!nodes || briggsTest(nodes, pb->Tn))
                        {
                            briggsSet(live, pb->Tn);
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
                                briggsSet(live, x->dc.left->offset->sp->i);
                            }
                            x = x->back;
                        }
                    }
                }
                else if (tail->temps & TEMP_ANS)
                {
                    if (tail->ans->mode == i_direct)
                    {
                        int tnum = tail->ans->offset->sp->i;
                        int k = -1;
                        if (!nodes || briggsTest(nodes, tnum))
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
                            briggsReset(live, tnum);
                            // processor dependent
                            if (tail->atomic && (tail->temps & TEMP_RIGHT) && tail->ans->size >= ISZ_FLOAT)
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
                            for (j = 0; j < live->top; j++)
                            {
                                if (live->data[j] != k)
                                    insertConflict(live->data[j], tnum);
                            }
                        }
                    }
                    else if (tail->ans->mode == i_ind)
                    {
                        if (tail->ans->offset)
                        {
                            int tnum = tail->ans->offset->sp->i;
                            if (!nodes || briggsTest(nodes, tnum))
                            {
                                briggsSet(live, tnum);
                            }
                        }
                        if (tail->ans->offset2)
                        {
                            int tnum = tail->ans->offset2->sp->i;
                            if (!nodes || briggsTest(nodes, tnum))
                            {
                                briggsSet(live, tnum);
                            }
                        }
                    }
                }
                if ((tail->temps & TEMP_LEFT) && !tail->dc.left->retval)
                {
                    if (tail->dc.left->offset)
                    {
                        int tnum = tail->dc.left->offset->sp->i;
                        if (!nodes || briggsTest(nodes, tnum))
                        {
                            briggsSet(live, tnum);
                        }
                    }
                    if (tail->dc.left->offset2)
                    {
                        int tnum = tail->dc.left->offset2->sp->i;
                        if (!nodes || briggsTest(nodes, tnum))
                        {
                            briggsSet(live, tnum);
                        }
                    }
                }
                if (tail->temps & TEMP_RIGHT)
                {
                    if (tail->dc.right->offset)
                    {
                        int tnum = tail->dc.right->offset->sp->i;
                        if (!nodes || briggsTest(nodes, tnum))
                        {
                            briggsSet(live, tnum);
                        }
                    }
                    if (tail->dc.right->offset2)
                    {
                        int tnum = tail->dc.right->offset2->sp->i;
                        if (!nodes || briggsTest(nodes, tnum))
                        {
                            briggsSet(live, tnum);
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