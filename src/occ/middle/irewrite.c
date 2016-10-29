/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include "compiler.h"
extern ARCH_ASM *chosenAssembler; 
extern int exitBlock, blockCount;
extern BLOCK **blockArray;
extern TEMP_INFO **tempInfo;
extern int tempCount;
extern BRIGGS_SET *globalVars;
extern QUAD *intermed_head;
void Prealloc(int pass)
{
    int i;
    BOOLEAN done = FALSE;
    BRIGGS_SET *eobGlobals;
    liveVariables();
    globalVars = briggsAlloc(tempCount * 2 < 100 ? 100 : tempCount * 2);
    eobGlobals = briggsAlloc(tempCount * 2 < 100 ? 100 : tempCount * 2);
    if (!chosenAssembler->gen->preRegAlloc)
        return;
    for (; pass < 11 && !done; pass++)
    {
        for (i=0; i < blockCount; i++)
        {
            if (blockArray[i])
            {
                QUAD *tail = blockArray[i]->tail;
                BITINT *p = blockArray[i]->liveOut;
                int j,k;
                briggsClear(globalVars);
                briggsClear(eobGlobals);
                for (j=0; j < (tempCount + BITINTBITS-1)/BITINTBITS; j++,p++)
                    if (*p)
                        for (k=0; k < BITINTBITS; k++)
                            if (*p & (1 << k))
                            {
                                int n = j*BITINTBITS + k;
                                briggsSet(globalVars, n);
                                briggsSet(eobGlobals, n);
                            }
                while (tail != blockArray[i]->head)
                {
                    if ((tail->temps & TEMP_ANS) && (tail->ans->mode == i_direct))
                    {
                        briggsReset(globalVars, tail->ans->offset->v.sp->value.i);
                    }
                    done |= chosenAssembler->gen->preRegAlloc(tail, globalVars, eobGlobals, pass);
                    if ((tail->temps & TEMP_ANS) && (tail->ans->mode == i_ind))
                    {
                        if (tail->ans->offset)
                            briggsSet(globalVars, tail->ans->offset->v.sp->value.i);
                        if (tail->ans->offset2)
                            briggsSet(globalVars, tail->ans->offset2->v.sp->value.i);
                    }
                    if (tail->temps & TEMP_LEFT)
                    {
                        if (tail->dc.left->offset)
                            briggsSet(globalVars, tail->dc.left->offset->v.sp->value.i);
                        if (tail->dc.left->offset2)
                            briggsSet(globalVars, tail->dc.left->offset2->v.sp->value.i);
                    }
                    if (tail->temps & TEMP_RIGHT)
                    {
                        if (tail->dc.right->offset)
                            briggsSet(globalVars, tail->dc.right->offset->v.sp->value.i);
                        if (tail->dc.right->offset2)
                            briggsSet(globalVars, tail->dc.right->offset2->v.sp->value.i);
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
    for (i=0; i < blockCount; i++)
    {
        if (blockArray[i])
        {
            QUAD *tail = blockArray[i]->tail;
            QUAD *head = blockArray[i]->head;
            ULLONG_TYPE liveRegs = 0;
            BITINT *p = blockArray[i]->liveOut;
            int j,k;
            int gosubcolor = -1;
            for (j=0; j < (tempCount + BITINTBITS-1)/BITINTBITS; j++,p++)
                if (*p)
                    for (k=0; k < BITINTBITS; k++)
                        if (*p & (1 << k))
                        {
                            int n = j*BITINTBITS+k;
                            liveRegs |= ((ULLONG_TYPE)1) << chosenAssembler->arch->regNames[tempInfo[n]->color].reg1live;
                            if (chosenAssembler->arch->regNames[tempInfo[n]->color].reg2live >= 0)
                                liveRegs |= ((ULLONG_TYPE)1) << chosenAssembler->arch->regNames[tempInfo[n]->color].reg2live;
                        }
            while (tail != head)
            {
                if (tail->dc.opcode == i_gosub)
                {
                    if (gosubcolor != -1)
                    {
                        liveRegs &= ~(((ULLONG_TYPE)1) << chosenAssembler->arch->regNames[tempInfo[gosubcolor]->color].reg1live);
                        if (chosenAssembler->arch->regNames[tempInfo[gosubcolor]->color].reg2live >= 0)
                            liveRegs &= ~(((ULLONG_TYPE)1) << chosenAssembler->arch->regNames[tempInfo[gosubcolor]->color].reg2live);
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
                            int tnum = tail->ans->offset->v.sp->value.i;
                            liveRegs &= ~(((ULLONG_TYPE)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg1live);
                            if (chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live >= 0)
                                liveRegs &= ~(((ULLONG_TYPE)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live);
                        }
                    }
                    else
                    {
                        int tnum;
                        if (tail->ans->offset)
                        {
                            tnum = tail->ans->offset->v.sp->value.i;
                            liveRegs |= ((ULLONG_TYPE)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg1live;
                            if (chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live >= 0)
                                liveRegs |= ((ULLONG_TYPE)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live;
                        }
                        if (tail->ans->offset2)
                        {
                            tnum = tail->ans->offset2->v.sp->value.i;
                            liveRegs |= ((ULLONG_TYPE)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg1live;
                            if (chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live >= 0)
                                liveRegs |= ((ULLONG_TYPE)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live;
                        }
                    }
                }
                if (tail->temps & TEMP_LEFT)
                {
                    int tnum;
                    if ((tail->dc.left->offset) && (tail->dc.left->size < ISZ_FLOAT || tail->dc.left->mode == i_ind))
                    {
                        tnum = tail->dc.left->offset->v.sp->value.i;
                        if (tail->dc.left->retval)
                            gosubcolor = tnum;
                        liveRegs |= ((ULLONG_TYPE)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg1live;
                        if (chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live >= 0)
                            liveRegs |= ((ULLONG_TYPE)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live;
                    }
                    if (tail->dc.left->offset2)
                    {
                        tnum = tail->dc.left->offset2->v.sp->value.i;
                        liveRegs |= ((ULLONG_TYPE)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg1live;
                        if (chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live >= 0)
                            liveRegs |= ((ULLONG_TYPE)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live;
                    }
                }
                if (tail->temps & TEMP_RIGHT)
                {
                    int tnum;
                    if ((tail->dc.right->offset) && (tail->dc.right->size < ISZ_FLOAT || tail->dc.right->mode == i_ind))
                    {
                        tnum = tail->dc.right->offset->v.sp->value.i;
                        liveRegs |= ((ULLONG_TYPE)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg1live;
                        if (chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live >= 0)
                            liveRegs |= ((ULLONG_TYPE)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live;
                    }
                    if (tail->dc.right->offset2)
                    {
                        tnum = tail->dc.right->offset2->v.sp->value.i;
                        liveRegs |= ((ULLONG_TYPE)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg1live;
                        if (chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live >= 0)
                            liveRegs |= ((ULLONG_TYPE)1) << chosenAssembler->arch->regNames[tempInfo[tnum]->color].reg2live;
                    }
                }
                if (tail->dc.opcode == i_coswitch)
                    tail->liveRegs = liveRegs;
                tail = tail->back;
            }
        }
    }
}
