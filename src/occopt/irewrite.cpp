/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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
#include "config.h"
#include "iblock.h"
#include "OptUtils.h"
#include "ilive.h"
#include "ioptutil.h"
#include "optmain.h"

namespace Optimizer
{

void Prealloc(int pass)
{
    int tempCountIn = tempCount;
    int i;
    bool done = false;
    BriggsSet* eobGlobals;
    liveVariables();
    globalVars = briggsAlloc(tempCount * 2 < 100 ? 100 : tempCount * 2);
    eobGlobals = briggsAlloc(tempCount * 2 < 100 ? 100 : tempCount * 2);
    for (; pass < 11 && !done; pass++)
    {
        for (i = 0; i < blockCount; i++)
        {
            if (blockArray[i])
            {
                QUAD* tail = blockArray[i]->tail;
                BITINT* p = blockArray[i]->liveOut;
                int j, k;
                briggsClear(globalVars);
                briggsClear(eobGlobals);
                for (j = 0; j < (tempCountIn + BITINTBITS - 1) / BITINTBITS; j++, p++)
                    if (*p)
                        for (k = 0; k < BITINTBITS; k++)
                            if (*p & (1 << k))
                            {
                                int n = j * BITINTBITS + k;
                                briggsSet(globalVars, n);
                                briggsSet(eobGlobals, n);
                            }
                while (tail != blockArray[i]->head)
                {
                    if ((tail->temps & TEMP_ANS) && (tail->ans->mode == i_direct))
                    {
                        briggsReset(globalVars, tail->ans->offset->sp->i);
                    }
                    PreRegAlloc(tail, globalVars, eobGlobals, pass);
                    if ((tail->temps & TEMP_ANS) && (tail->ans->mode == i_ind))
                    {
                        if (tail->ans->offset)
                            briggsSet(globalVars, tail->ans->offset->sp->i);
                        if (tail->ans->offset2)
                            briggsSet(globalVars, tail->ans->offset2->sp->i);
                    }
                    if (tail->temps & TEMP_LEFT)
                    {
                        if (tail->dc.left->offset)
                            briggsSet(globalVars, tail->dc.left->offset->sp->i);
                        if (tail->dc.left->offset2)
                            briggsSet(globalVars, tail->dc.left->offset2->sp->i);
                    }
                    if (tail->temps & TEMP_RIGHT)
                    {
                        if (tail->dc.right->offset)
                            briggsSet(globalVars, tail->dc.right->offset->sp->i);
                        if (tail->dc.right->offset2)
                            briggsSet(globalVars, tail->dc.right->offset2->sp->i);
                    }
                    tail = tail->back;
                }
            }
        }
    }
}
void CalculateBackendLives(void)
{
    int i;
    liveVariables();
    for (i = 0; i < blockCount; i++)
    {
        if (blockArray[i])
        {
            QUAD* tail = blockArray[i]->tail;
            QUAD* head = blockArray[i]->head;
            unsigned long long liveRegs = 0;
            BITINT* p = blockArray[i]->liveOut;
            int j, k;
            int gosubcolor = -1;
            for (j = 0; j < (tempCount + BITINTBITS - 1) / BITINTBITS; j++, p++)
                if (*p)
                    for (k = 0; k < BITINTBITS; k++)
                        if (*p & (1 << k))
                        {
                            int n = j * BITINTBITS + k;
                            liveRegs |= ((unsigned long long)1) << chosenAssembler->arch->regNames[tempInfo[n]->color].reg1live;
                            if (chosenAssembler->arch->regNames[tempInfo[n]->color].reg2live >= 0)
                                liveRegs |= ((unsigned long long)1) << chosenAssembler->arch->regNames[tempInfo[n]->color].reg2live;
                        }
            while (tail != head)
            {
                if (tail->dc.opcode == i_gosub)
                {
                    if (gosubcolor != -1)
                    {
                        liveRegs &=
                            ~(((unsigned long long)1) << chosenAssembler->arch->regNames[tempInfo[gosubcolor]->color].reg1live);
                        if (chosenAssembler->arch->regNames[tempInfo[gosubcolor]->color].reg2live >= 0)
                            liveRegs &=
                                ~(((unsigned long long)1) << chosenAssembler->arch->regNames[tempInfo[gosubcolor]->color].reg2live);
                        gosubcolor = -1;
                    }
                }
                tail->liveRegs = liveRegs;
                if (tail->temps & TEMP_ANS)
                {
                    if (tail->ans->mode == i_direct)
                    {
                        if (tail->ans->size < ISZ_FLOAT)
                        {
                            int tnum = tail->ans->offset->sp->i;
                            liveRegs &=
                                ~(((unsigned long long)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg1live);
                            if (chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live >= 0)
                                liveRegs &=
                                    ~(((unsigned long long)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live);
                        }
                    }
                    else
                    {
                        int tnum;
                        if (tail->ans->offset)
                        {
                            tnum = tail->ans->offset->sp->i;
                            liveRegs |= ((unsigned long long)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg1live;
                            if (chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live >= 0)
                                liveRegs |= ((unsigned long long)1)
                                            << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live;
                        }
                        if (tail->ans->offset2)
                        {
                            tnum = tail->ans->offset2->sp->i;
                            liveRegs |= ((unsigned long long)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg1live;
                            if (chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live >= 0)
                                liveRegs |= ((unsigned long long)1)
                                            << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live;
                        }
                    }
                }
                if (tail->temps & TEMP_LEFT)
                {
                    int tnum;
                    if ((tail->dc.left->offset) && (tail->dc.left->size < ISZ_FLOAT || tail->dc.left->mode == i_ind))
                    {
                        tnum = tail->dc.left->offset->sp->i;
                        if (tail->dc.left->retval)
                            gosubcolor = tnum;
                        liveRegs |= ((unsigned long long)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg1live;
                        if (chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live >= 0)
                            liveRegs |= ((unsigned long long)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live;
                    }
                    if (tail->dc.left->offset2)
                    {
                        tnum = tail->dc.left->offset2->sp->i;
                        liveRegs |= ((unsigned long long)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg1live;
                        if (chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live >= 0)
                            liveRegs |= ((unsigned long long)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live;
                    }
                }
                if (tail->temps & TEMP_RIGHT)
                {
                    int tnum;
                    if ((tail->dc.right->offset) && (tail->dc.right->size < ISZ_FLOAT || tail->dc.right->mode == i_ind))
                    {
                        tnum = tail->dc.right->offset->sp->i;
                        liveRegs |= ((unsigned long long)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg1live;
                        if (chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live >= 0)
                            liveRegs |= ((unsigned long long)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live;
                    }
                    if (tail->dc.right->offset2)
                    {
                        tnum = tail->dc.right->offset2->sp->i;
                        liveRegs |= ((unsigned long long)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg1live;
                        if (chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live >= 0)
                            liveRegs |= ((unsigned long long)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live;
                    }
                }
                if (tail->dc.opcode == i_coswitch)
                    tail->liveRegs = liveRegs;
                tail = tail->back;
            }
        }
    }
}
}  // namespace Optimizer