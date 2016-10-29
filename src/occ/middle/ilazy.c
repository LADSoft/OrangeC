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
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "compiler.h"

/* global data flow analysis is an optimal placement of all expressions
 * it is culled from:
 *
 * Lazy Code Motion
 * Jens Knoop, Oliver Ruthing, Bernhard Steffen
 *
 * the optimal method of using the reverse DFST ordering
 * for iterating bit algorithms was culled from:
 *
 * ANALYSIS OF A SIMPLE ALGORITHM FOR GLOBAL DATA FLOW PROBLEMS
 * Matthew S. Hecht, Jeffrey D. Uhlman
 *
 * the algorithm in this module optimizes for both expression calculations 
 * and register pressure
 *
 * note that this code inserts copies and changes operands, 
 * and there is a reliance on live variable analysis and copy coalescing
 * for post-cleanup of the code.
 *
 * note: this module relies implicitly on the assumption that if the same
 * expression is evaluated two or more times, the same temp variable names
 * are used in each evaluation sequence as in all the others.
 */
extern FILE *icdFile;
extern COMPILER_PARAMS cparams;
extern ARCH_ASM *chosenAssembler; 
extern BOOLEAN setjmp_used;

extern int walkPostorder, walkPreorder;

extern int exitBlock;
extern int cachedTempCount;
extern BITINT bittab[BITINTBITS];
extern int blockCount;
extern BLOCK **blockArray;
extern TEMP_INFO **tempInfo;
extern int tempCount;
extern QUAD *intermed_head, *intermed_tail;
extern int walkPostorder;
extern BITINT *uivBytes;

unsigned short *termMap;
unsigned short *termMapUp;
unsigned termCount;

static BLOCK **reverseOrder, **forwardOrder;
static BITINT *tempBytes, *tempBytes2, *tempBytes3;
static int blocks;
static BITINT *unMoveableTerms;
static BITINT *ocpTerms;


static void ormap(BITINT *dest, BITINT *src)
{
    int n = (termCount  + BITINTBITS-1)/BITINTBITS;
    int i;
    for (i=0; i < n; i++)
    {
        *dest++ |= *src++;
    }
}
static void andmap(BITINT *dest, BITINT *source)
{
    int i;
    int n = (termCount  + BITINTBITS-1)/BITINTBITS;
    for (i=0; i < n; i++)
        *dest++ &= *source++;
}
static void copymap(BITINT *dest, BITINT *source)
{
    memcpy(dest, source, ((termCount + BITINTBITS-1) / BITINTBITS) * sizeof(BITINT) );
}
static BOOLEAN samemap(BITINT *dest, BITINT *source)
{
    BITINT *olddest = dest;
    BITINT *oldsrc = source;
    int i;
    int n = (termCount  + BITINTBITS-1)/BITINTBITS;
    for (i=0; i < n; i++)
        if (*dest ++ != *source ++)
        {
            copymap(olddest, oldsrc);
            return FALSE;
        }
    return TRUE;
}
static void complementmap(BITINT *dest)
{
    int i;
    int n = (termCount  + BITINTBITS-1)/BITINTBITS;
    for (i=0; i < n; i++)
        *dest++ ^= (BITINT)-1;
}
static void setmap(BITINT *dest , BOOLEAN val)
{
    int i;
    int v = val ? 0xff : 0;
    int n = (termCount  + BITINTBITS-1)/BITINTBITS * sizeof(BITINT);
    memset(dest, v, n);
}
static BOOLEAN AnySet(BITINT *map)
{
    int n = termCount/BITINTBITS;
    int i;
    for (i=0; i < n; i++)
        if (map[i])
        {
            return TRUE;
        }
    if (i >= n)
        for (i= termCount & -BITINTBITS; i < termCount; i++)
        {
            if (isset(tempBytes, i))
            {
                return TRUE;
            }
        }
    return FALSE;
}
static void EnterGlobal(QUAD *head)
{
    head->dsafe = aallocbit(termCount);
    head->earliest = aallocbit(termCount);
    head->delay = aallocbit(termCount);
    head->latest = aallocbit(termCount);
    head->isolated = aallocbit(termCount);
    head->OCP = aallocbit(termCount);
    head->RO = aallocbit(termCount);
    head->uses = aallocbit(termCount);
    head->transparent = aallocbit(termCount);
    setmap(head->transparent, TRUE);
}
static void GatherGlobals(void)
{
    int i;
    EnterGlobal(blockArray[0]->head);
    setmap(blockArray[0]->head->transparent, FALSE);
    EnterGlobal(blockArray[exitBlock]->tail);
    for (i=0; i < blockCount; i++)
        if (blockArray[i])
        {
            QUAD *head = blockArray[i]->head;
            while (head != blockArray[i]->tail->fwd)
            {
                if (head->dc.opcode != i_label && !head->ignoreMe)
                {
                    if (head->needsOCP || head->dc.opcode == i_block)
                    {
                        EnterGlobal(head);
                    }
                    else if (head->ans)
                    {
                        if (!(head->temps & TEMP_ANS) || head->dc.opcode == i_clrblock || head->dc.opcode == i_assnblock ||
                            ((head->temps & TEMP_ANS) && (head->ans->mode == i_ind
                                                         || head->ans->offset->v.sp->pushedtotemp)))
                        {
                            EnterGlobal(head);
                        }
                    }
                    // the gosub is for UIVs, the goto is to allow a convenient point to gather loop invariants
                    // because sometimes this algorithm will move them.  The parms are to optimize
                    // statements associated with parameter pushes...  and the jxx are like variable assignments that affect
                    // the uses set but are transparent to everything...
                    else if (head->dc.opcode == i_gosub || head->dc.opcode == i_parm || head->dc.opcode == i_goto || (head->dc.opcode >= i_jne && head->dc.opcode <= i_jge))
                    {
                        EnterGlobal(head);
                    }
                }   
                head = head->fwd;
            }
        }
}
void SetunMoveableTerms(void)
{
        int i;
    // if they don't have floating point regs then don't move expressions involving
    // floating point
    unMoveableTerms = aallocbit(termCount);
    setmap(unMoveableTerms, TRUE);
    for (i=0; i < blockCount; i++)
    {
        BLOCK *b = blockArray[i];
        if (b)
        {
            QUAD *head;
            head = b->head;
            while (head != b->tail->fwd)
            {
                if (!head->ignoreMe && head->dc.opcode != i_label)
                {
                    if (head->temps & TEMP_ANS)
                    {
                        int n = termMap[head->ans->offset->v.sp->value.i];
                        if ((!chosenAssembler->arch->hasFloatRegs
                                &&head->ans->size >= ISZ_FLOAT) || head->ans->bits || head->ans->vol || head->ans->offset->v.sp->loadTemp)
                            clearbit(unMoveableTerms,n );
                    }
                }
                head = head->fwd;
            }
        }
    }
}
static void CalculateUses(void)
{
    int i,j;
    BLOCK *b;
    QUAD *head, *tail;
    setmap(tempBytes, TRUE);
    tail = intermed_tail;
    while (!tail->OCP)
        tail = tail->back;
    while (tail)
    {
        BOOLEAN first = TRUE;
        head = tail;
        setmap(head->uses, FALSE);
        while (head && (first || !head->OCP))
        {
            first = FALSE;
            if (!head->ignoreMe)
            {
                if (head->temps & TEMP_ANS)
                {
                    if (head->ans->mode == i_ind)
                    {
                        setbit(tail->uses, termMap[head->ans->offset->v.sp->value.i]);
                    }
                }
                if (head->temps & TEMP_LEFT)
                {
                    if (head->dc.left->mode == i_ind)
                    {
                        setbit(tail->uses, termMap[head->dc.left->offset->v.sp->value.i]);
                    }
                    else if (!head->dc.left->offset->v.sp->pushedtotemp)
                    {
                        setbit(tail->uses, termMap[head->dc.left->offset->v.sp->value.i]);
                    }
                }
                if (head->temps & TEMP_RIGHT)
                {
                    if (head->dc.right->mode == i_ind)
                    {
                        setbit(tail->uses, termMap[head->dc.right->offset->v.sp->value.i]);
                    }
                    else if (!head->dc.right->offset->v.sp->pushedtotemp)
                    {
                        setbit(tail->uses, termMap[head->dc.right->offset->v.sp->value.i]);
                    }
                }
            }
            head = head->back;
        }
        tail = head;
    }
}
static void CalculateTransparent(void)
{
    int n = (termCount + BITINTBITS - 1)/BITINTBITS;
    int i,j,k;
    QUAD *head, *tail;
    tail = intermed_tail;
    while (!tail->OCP)
        tail = tail->back;
    while (tail)
    {
        setmap(tail->transparent, TRUE);
        if (tail->dc.opcode == i_gosub)
        {
            QUAD *next = tail->fwd;
            setmap(tempBytes3, FALSE);
            AliasGosub(tail, tempBytes3, tail->transparent, n /** sizeof(BITINT)*/);
            while (next && !next->OCP)
            {
                if (next->dc.left && next->dc.left->retval && (next->temps & TEMP_ANS) && next->ans->mode == i_direct)
                {
                    int n = next->ans->offset->v.sp->value.i;
                    clearbit(head->transparent, termMap[n]);
                    if (tempInfo[n]->terms)
                        andmap(tail->transparent, tempInfo[n]->terms);
                    break;
                }
                next = next->fwd;
            }
            complementmap(tempBytes3);
            for (i=0; i < termCount; i++)
                if (!isset(tempBytes3, i) && tempInfo[termMapUp[i]]->terms)
                {
                    andmap(tail->transparent, tempInfo[termMapUp[i]]->terms);
                }
        }
        else if (tail->dc.opcode == i_assnblock)
        {
            setmap(tempBytes, FALSE);
            AliasStruct(tempBytes, tail->ans, tail->dc.left, tail->dc.right);
            complementmap(tempBytes);
            for (i=0; i < termCount; i++)
            {
                if (!isset(tempBytes, i))
                {
                    if (tempInfo[termMapUp[i]]->terms)
                        andmap(tempBytes, tempInfo[termMapUp[i]]->terms);
                }
            }
            andmap(tail->transparent, tempBytes);
        }
        head = tail->back;
        while (head && !head->OCP)
        {
            if (!head->ignoreMe)
            {
                if (head->ans && head->dc.opcode != i_label)
                {
                    if (head->temps & TEMP_ANS)
                    {
                        int n = head->ans->offset->v.sp->value.i;
                        if (!tempInfo[n]->uses)
                        {
                            tempInfo[n]->uses = aallocbit(termCount);
                            AliasUses(tempInfo[n]->uses, head->dc.left, TRUE);
                            AliasUses(tempInfo[n]->uses, head->dc.right,TRUE);
                        }                    
                    }
                }
            }
            head = head->back;
        }
        tail = head;
    }
    head = intermed_head;
    tail = head;
    
    while (head)
    {
        QUAD *last = tail;
        do
        {
            tail = tail->fwd;
        } while (tail && !tail->OCP) ;
        if (tail)
        {
            if (tail->ans && tail->dc.opcode != i_label)
            {
                setmap(tempBytes, FALSE);
                AliasUses(tempBytes, tail->ans, TRUE);
                for (i=0; i < termCount; i++)
                {
                    if (isset(tempBytes, i))
                    {
                        clearbit(tail->transparent, i);
                        if (tempInfo[termMapUp[i]]->terms)
                            andmap(tail->transparent, tempInfo[termMapUp[i]]->terms);
                    }
                }
            }
        }
        head = tail;
    }
}
static QUAD *previous(QUAD *tail)
{
    BLOCK *b = tail->block;
    do
    {
        tail = tail->back;
    } while (tail && !tail->OCP);
    if (!tail || tail->block != b)
        return NULL;
    return tail;
}
static QUAD *successor(QUAD *head)
{
    BLOCK *b = head->block;
    do
    {
        head = head->fwd;
    } while (head && !head->OCP);
    if (!head || head->block != b)
        return NULL;
    return head;
}
static QUAD *First(QUAD *head)
{
    if (head->OCP)
        return head;
    return successor(head);
}
static QUAD *Last(QUAD *tail)
{
    if (tail->OCP)
        return tail;
    return previous(tail);
}
static void CalculateDSafe(void)
{
    int i;
    BOOLEAN changed;
    do
    {
        changed = FALSE;
        for (i=0; i < blocks; i++)
        {
            BLOCK *b = reverseOrder[i];
            if (b)
            {
                QUAD *tail = Last(b->tail);
                if (tail)
                {
                    BLOCKLIST *pbl = tail->block->succ;
                    if (pbl)
                    {
                        copymap(tempBytes, tail->transparent);
                        while (pbl)
                        {
                            QUAD *first = First(pbl->block->head);
                            if (first)
                            {
                                andmap(tempBytes, first->dsafe);
                            }
                            else
                                diag("Empty block in dsafe");
                            pbl = pbl->next;
                        }
                        ormap(tempBytes, tail->uses);
                        changed |= !samemap(tail->dsafe, tempBytes);
                    }
                    while (TRUE)
                    {
                        QUAD *prev = previous(tail);
                        if (prev)
                        {
                            copymap(tempBytes, prev->transparent);
                            andmap(tempBytes, tail->dsafe);
                            ormap(tempBytes, prev->uses);
                            changed |= !samemap(prev->dsafe, tempBytes);
                        }
                        else
                        {
                            break;
                        }
                        tail = prev;
                    }
                }
                else
                    diag("Empty block in dsafe");
            }
        }
    } while (changed);
}
static void CalculateEarliest(void)
{
    int i;
    BOOLEAN changed;
    setmap(blockArray[0]->head->earliest, TRUE);
    do
    {
        changed = FALSE;
        for (i=0; i < blocks; i++)
        {
            BLOCK *b = forwardOrder[i];
            if (b)
            {
                QUAD *head = First(b->head);
                if (head)
                {
                    BLOCKLIST *pbl = head->block->pred;
                    if (pbl)
                    {
                        setmap(tempBytes3, TRUE);
                        while (pbl)
                        {
                            QUAD *tail = Last(pbl->block->tail);
                            if (tail)
                            {
                                copymap(tempBytes, tail->dsafe);
                                complementmap(tempBytes);
                                andmap(tempBytes, tail->earliest);
                                copymap(tempBytes2, tail->transparent);
                                complementmap(tempBytes2);
                                ormap(tempBytes, tempBytes2);
                                ormap(tempBytes3, tempBytes);
                            }
                            else
                                diag("Empty block in earliest");
                            pbl = pbl->next;
                        }
                        changed |= !samemap(head->earliest, tempBytes3);
                    }
                    while (TRUE)
                    {
                        QUAD *next = successor(head);
                        if (next)
                        {
                            copymap(tempBytes, head->dsafe);
                            complementmap(tempBytes);
                            andmap(tempBytes, head->earliest);
                            copymap(tempBytes2, head->transparent);
                            complementmap(tempBytes2);
                            ormap(tempBytes, tempBytes2);
                            changed |= !samemap(next->earliest, tempBytes);
                        }
                        else
                        {
                            break;
                        }
                        head = next;
                    }
                }
                else
                    diag("Empty block in earliest");
            }
        }
    } while (changed);
    
}
static void CalculateDelay(void)
{
    int i;
    BOOLEAN changed;
    do
    {
        changed = FALSE;
        for (i=0; i < blocks; i++)
        {
            BLOCK *b = forwardOrder[i];
            if (b)
            {
                QUAD *head = First(b->head);
                if (head)
                {
                    BLOCKLIST *pbl = head->block->pred;
                    copymap(tempBytes3, head->dsafe);
                    andmap(tempBytes3, head->earliest);
                    if (pbl)
                    {
                        setmap(tempBytes, FALSE);
                        while (pbl)
                        {	
                            QUAD *tail = Last(pbl->block->tail);
                            if (tail)
                            {
                                copymap(tempBytes2, tail->uses);
                                complementmap(tempBytes2);
                                andmap(tempBytes2, tail->delay);
                                andmap(tempBytes, tempBytes2);
                            }
                            else
                                diag("Empty block in delay");
                            pbl = pbl->next;
                        }
                        ormap(tempBytes3, tempBytes);
                    }
                    changed |= !samemap(head->delay, tempBytes3);
                    while (TRUE)
                    {
                        QUAD *next = successor(head);
                        if (next)
                        {
                            copymap(tempBytes, head->uses);
                            complementmap(tempBytes);
                            andmap(tempBytes, head->delay);
                            copymap(tempBytes2, next->dsafe);
                            andmap(tempBytes2, next->earliest);
                            ormap(tempBytes, tempBytes2);
                            changed |= !samemap(next->delay, tempBytes);
                        }
                        else
                        {
                            break;
                        }
                        head = next;
                    }
                }
                else
                    diag("Empty block in delay");
            }
        }
    } while (changed);
    
}
static void CalculateLatest(void)
{
    int i;
    for (i=0; i < blocks; i++)
    {
        BLOCK *b = reverseOrder[i];
        if (b)
        {
            QUAD *tail = Last(b->tail);
            if (tail)
            {
                BLOCKLIST *pbl = tail->block->succ;
                if (pbl)
                {
                    setmap(tail->latest, TRUE);
                    while (pbl)
                    {
                        QUAD *first = First(pbl->block->head);
                        if (first)
                            andmap(tail->latest, first->delay);
                        else
                            diag("Empty block in latest");
                        pbl = pbl->next;
                    }
                    complementmap(tail->latest);
                }
                else
                {
                    setmap(tail->latest, FALSE);
                }
                ormap(tail->latest, tail->uses);
                andmap(tail->latest, tail->delay);
                while (TRUE)
                {
                    QUAD *prev = previous(tail);
                    if (prev)
                    {
                        copymap(prev->latest, tail->delay);
                        complementmap(prev->latest);
                        ormap(prev->latest, prev->uses);
                        andmap(prev->latest, prev->delay);
                    }
                    else
                    {
                        break;
                    }
                    tail = prev;
                }
            }
            else
                diag("Empty block in latest");
        }
    }
}
static void CalculateIsolated(void)
{
    int i;
    BOOLEAN changed;
    for (i=0; i < blocks; i++)
        setmap(reverseOrder[i]->head->isolated, TRUE);
    do
    {
        changed = FALSE;
        for (i=0; i < blocks; i++)
        {
            BLOCK *b = reverseOrder[i];
            if (b && b->blocknum != exitBlock)
            {
                QUAD *tail = Last(b->tail);
                if (tail)
                {
                    BLOCKLIST *pbl = tail->block->succ;
                    if (pbl)
                    {
                        setmap(tempBytes, TRUE);
                        while (pbl)
                        {
                            QUAD *first = First(pbl->block->head);
                            if (first)
                            {
                                copymap(tempBytes2, first->uses);
                                complementmap(tempBytes2);
                                andmap(tempBytes2, first->isolated);
                                ormap(tempBytes2, first->latest);
                                andmap(tempBytes, tempBytes2);
                            }
                            else
                                diag("Empty block in isolated");
                            pbl = pbl->next;
                        }
                        changed |= !samemap(tail->isolated, tempBytes);
                    }
                    while (TRUE)
                    {
                        QUAD *prev = previous(tail);
                        if (prev)
                        {
                            copymap(tempBytes2, tail->uses);
                            complementmap(tempBytes2);
                            andmap(tempBytes2, tail->isolated);
                            ormap(tempBytes2, tail->latest);
                            changed |= !samemap(prev->isolated, tempBytes2);
                        }
                        else
                        {
                            break;
                        }
                        tail = prev;
                    }
                }
                else
                    diag("Empty block in isolated");
            }
        }
    } while (changed);
}
static void GatherTerms(void)
{
        int i;
    QUAD *tail = intermed_tail;
    while (tail && !tail->OCP)
        tail = tail->back;
        for (i = 0; i < tempCount; i++)
        {
            tempInfo[i]->termClear = TRUE;
        }
    while (tail)
    {
        tail = tail->back;
        while (tail && !tail->OCP)
        {
            if ((tail->temps & TEMP_ANS) && tail->ans->mode == i_direct)
            {
                int n = tail->ans->offset->v.sp->value.i;
                if ((tail->temps & (TEMP_LEFT | TEMP_RIGHT)) || tail->dc.left->retval)
                {
                    if (((tail->temps & TEMP_LEFT) || tail->dc.left->retval) /*&& 
                        (!tail->dc.left->offset->v.sp->pushedtotemp)*/)
                    {
                        int l = tail->dc.left->offset->v.sp->value.i;
                        if (!tempInfo[l]->terms)
                        {
                            tempInfo[l]->terms = aallocbit(termCount);
                        }
                        if (tempInfo[l]->termClear)
                        {
                            setmap(tempInfo[l]->terms, TRUE);
                            tempInfo[l]->termClear = FALSE;
                        }
                        if (tempInfo[n]->terms)
                        {
                            andmap(tempInfo[l]->terms, tempInfo[n]->terms);
                        }
                        clearbit(tempInfo[l]->terms, termMap[n]);
                    }
                    if ((tail->temps & TEMP_RIGHT) && !tail->dc.right->offset->v.sp->pushedtotemp)
                    {
                        int l = tail->dc.right->offset->v.sp->value.i;
                        if (!tempInfo[l]->terms)
                        {
                            tempInfo[l]->terms = aallocbit(termCount);
                        }
                        if (tempInfo[l]->termClear)
                        {
                            setmap(tempInfo[l]->terms, TRUE);
                            tempInfo[l]->termClear = FALSE;
                        }
                        if (tempInfo[n]->terms)
                        {
                            andmap(tempInfo[l]->terms, tempInfo[n]->terms);
                        }
                        clearbit(tempInfo[l]->terms, termMap[n]);
                    }
                }
            }
            tail = tail->back;
        }
    }
    for (i=0; i < termCount; i++)
        if (!isset(uivBytes, i) && tempInfo[termMapUp[i]]->terms)
        {
            andmap(uivBytes, tempInfo[termMapUp[i]]->terms);
        }
}
static void CalculateOCPAndRO(void)
{
    int i;
    ocpTerms = aallocbit(termCount);
    for (i=0; i < blocks; i++)
    {		
        if (forwardOrder[i])
        {
            QUAD *head = forwardOrder[i]->head;
            while (head != forwardOrder[i]->tail->fwd)
            {
                if (head->OCP)
                {
                
                    copymap(head->OCP, head->isolated);
                    complementmap(head->OCP);
                    andmap(head->OCP, head->latest);
                    andmap(head->OCP, unMoveableTerms);
                    ormap(ocpTerms, head->OCP);
                    copymap(head->RO, head->latest);
                    andmap(head->RO, head->isolated);
                    complementmap(head->RO);
                    andmap(head->RO, head->uses);
                    andmap(head->RO, unMoveableTerms);
                    ormap(ocpTerms, head->RO);
                    
                }
                head = head->fwd;
            }
        }
    }
}
static void HandleOCP(QUAD *after, int tn)
{
    if (tempInfo[tn]->idefines)
    {
        QUAD *p  = (QUAD *)(tempInfo[tn]->idefines->data);
        QUAD *tail = after;
        QUAD *ins = Alloc(sizeof(QUAD));
        QUAD *bans;
        BOOLEAN a = FALSE, l = FALSE;
        if (after->dc.opcode != i_block && after->dc.opcode != i_label)
        {
            QUAD *beforea, *beforel = NULL;
            after = after->back;
            beforea = NULL;
            while (!after->OCP && !beforel &&
                   !after->ignoreMe && after->dc.opcode != i_label &&
                   (!after->dc.left || !after->dc.left->retval) &&
                   after->dc.opcode != i_block)
            {
                if (after->temps & TEMP_ANS) 
                {
                    if (tn == after->ans->offset->v.sp->value.i)
                        if (after->ans->mode == i_direct)
                            beforea = after;
                }
                after = after->back;
            }
            if (beforel)
                after = beforel;
            else if (beforea)
                after = beforea;
        }
        else
        {
            while (after->fwd->ignoreMe || after->fwd->dc.opcode == i_label || after->fwd->OCPInserted)
                after = after->fwd;
        }
        *ins = *p;
        ins->uses = NULL;
        ins->transparent = NULL;
        ins->dsafe = NULL;
        ins->earliest = NULL;
        ins->delay = NULL;
        ins->latest = NULL;
        ins->isolated = NULL;
        ins->OCP = NULL;
        ins->RO = NULL;
        
        ins->OCPInserted = TRUE;
        ins->fwd = after->fwd;
        ins->back = after;
        ins->back->fwd = ins;
        ins->fwd->back = ins;
        ins->block = after->block;
        if (after == after->block->tail)
            after->block->tail = ins;
        bans = Alloc(sizeof(QUAD));
        bans->ans = tempInfo[tn]->copy;
        bans->dc.left = ins->ans;
        bans->dc.opcode = i_assn;
        bans->OCPInserted = TRUE;
        bans->back = ins;
        bans->fwd = ins->fwd;
        bans->back->fwd = bans;
        bans->fwd->back = bans;
        bans->block = ins->block;
        bans->temps = TEMP_LEFT | TEMP_ANS;
        if (ins == ins->block->tail)
            ins->block->tail = bans;
    }
}
static IMODE *GetROVar(IMODE *oldvar, IMODE *newvar, BOOLEAN mov)
{
    if (oldvar->mode == i_ind)
    {
        IMODELIST *iml = newvar->offset->v.sp->imind;
        IMODE *im = NULL;
        while (iml)
        {
            if (iml->im->size == oldvar->size && 
                iml->im->bits == oldvar->bits && iml->im->startbit == oldvar->startbit)
            {
                im = iml->im;
                break;
            }
            iml = iml->next;
        }
        if (!iml)
        {
            IMODELIST *iml2 = Alloc(sizeof(IMODELIST));
            im = Alloc(sizeof(IMODE));
            *im = *newvar;
            im->mode = i_ind;
            im->size = oldvar->size;
            im->ptrsize = oldvar->ptrsize;
            im->startbit = oldvar->startbit;
            im->bits = oldvar->bits;
            iml2->im = im;
            iml2->next = newvar->offset->v.sp->imind;
            newvar->offset->v.sp->imind = iml2;
        }
        newvar = im;
    }
    else
    {
        if (oldvar->offset->v.sp->pushedtotemp)
            return oldvar;
        if (mov)
        {
            int n = oldvar->offset->v.sp->value.i;
            QUAD *q = (QUAD *)tempInfo[n]->idefines->data;
            if (q->dc.opcode == i_assn && q->dc.left->mode == i_immed)
                return q->dc.left;
        }
    }
    return newvar;
}
static void HandleRO(QUAD *after, int tn)
{
    BITINT *OCP = after->OCP;

    if (after->dc.opcode != i_block)
    do 
    {
        if (!after->OCPInserted)
        {
            if ((after->temps & TEMP_ANS)
                && after->ans->mode == i_direct && after->ans->offset->v.sp->value.i == tn 
                && (after->dc.opcode != i_assn || !(after->temps & TEMP_LEFT) || !after->dc.left->offset->v.sp->pushedtotemp))
            {
                after->dc.left = GetROVar(after->ans, tempInfo[after->ans->offset->v.sp->value.i]->copy, after->dc.opcode == i_assn);
                after->dc.opcode = i_assn;
                after->temps &= ~TEMP_RIGHT;
                after->temps |= TEMP_LEFT;
                after->dc.right = NULL;
                if (after->dc.left->offset->type != en_tempref)
                    after->temps &= ~TEMP_LEFT;
            }
            else
            {
            
                if ((after->temps & TEMP_LEFT) && after->dc.left->offset->v.sp->value.i == tn)
                {
                    after->dc.left = GetROVar(after->dc.left, tempInfo[after->dc.left->offset->v.sp->value.i]->copy, after->dc.opcode == i_assn);
                    if (after->dc.left->offset->type != en_tempref)
                        after->temps &= ~TEMP_LEFT;
                }
                if ((after->temps & TEMP_RIGHT) && after->dc.right->offset->v.sp->value.i == tn)
                {
                    after->dc.right = GetROVar(after->dc.right, tempInfo[after->dc.right->offset->v.sp->value.i]->copy, FALSE);
                }
            }
        }
        after = after->back;
        
    } while (!after->OCP && after->dc.opcode != i_label);
}
static void MoveExpressions(void)
{
    int i,j;
    for (i=0; i < termCount; i++)
    {
        if (isset(ocpTerms,i))
        {
            int j;
            int size = tempInfo[termMapUp[i]]->enode->v.sp->imvalue->size;
            tempInfo[termMapUp[i]]->copy = InitTempOpt(size, size);
            for (j=0; j < blocks; j++)
            {
    
                QUAD *head = forwardOrder[j]->head;
                while (head != forwardOrder[j]->tail->fwd)
                {
                    if (head->OCP)
                    {
                        if (isset(head->OCP,i))
                        {
                            HandleOCP(head, termMapUp[i]);
                       }
                    }
                    head = head->fwd;
                }
            }
        }
    }
    for (i=0; i < termCount; i++)
    {
        if (isset(ocpTerms,i))
        {
            int j;
            for (j=0; j < blocks; j++)
            {
            
                QUAD *head = forwardOrder[j]->head;
                while (head != forwardOrder[j]->tail->fwd)
                {
                    if (head->OCP)
                    {
                        if (isset(head->RO,i))
                            HandleRO(head, termMapUp[i]);
                    }
                    head = head->fwd;
                }
            }
        }
    }
}
void RearrangePrecolors(void)
{
    QUAD *head = intermed_head;
    int i;
    for (i=0; i < tempCount; i++)
        tempInfo[i]->temp = -1;
    while (head)
    {
        if ((head->precolored & TEMP_ANS) && !head->ans->retval)
        {
            QUAD *newIns = Alloc(sizeof(QUAD));
            i = head->ans->offset->v.sp->value.i;
            if (tempInfo[i]->temp < 0)
            {
                IMODE *newImode = InitTempOpt(head->ans->size, head->ans->size);
                tempInfo[i]->temp = newImode->offset->v.sp->value.i;
                newIns->dc.left = newImode;
            }
            else
            {
                newIns->dc.left = tempInfo[tempInfo[i]->temp]->enode->v.sp->imvalue;
            }
            newIns->dc.opcode = i_assn;
            newIns->ans = head->ans;
            head->ans = newIns->dc.left;
            InsertInstruction(head, newIns);
        }
        if ((head->precolored & TEMP_LEFT) && !head->dc.left->retval)
        {
            QUAD *newIns = Alloc(sizeof(QUAD));
            i = head->dc.left->offset->v.sp->value.i;
            if (tempInfo[i]->temp < 0)
            {
                IMODE *newImode = InitTempOpt(head->dc.left->size, head->dc.left->size);
                tempInfo[i]->temp = newImode->offset->v.sp->value.i;
                newIns->ans = newImode;
            }
            else
            {
                newIns->ans = tempInfo[tempInfo[i]->temp]->enode->v.sp->imvalue;
            }
            newIns->dc.opcode = i_assn;
            newIns->dc.left = head->dc.left;
            head->dc.left = newIns->ans;
            InsertInstruction(head->back, newIns);
        }
        if ((head->precolored & TEMP_RIGHT) && !head->dc.right->retval)
        {
            QUAD *newIns = Alloc(sizeof(QUAD));
            i = head->dc.right->offset->v.sp->value.i;
            if (tempInfo[i]->temp < 0)
            {
                IMODE *newImode = InitTempOpt(head->dc.right->size, head->dc.right->size);
                tempInfo[i]->temp = newImode->offset->v.sp->value.i;
                newIns->ans = newImode;
            }
            else
            {
                newIns->ans = tempInfo[tempInfo[i]->temp]->enode->v.sp->imvalue;
            }
            newIns->dc.opcode = i_assn;
            newIns->dc.left = head->dc.right;
            head->dc.right = newIns->ans;
            InsertInstruction(head->back, newIns);
        }
        head->precolored = 0;
        head = head->fwd;
    }
}
static void PadBlocks(void)
{
    int i;
    for (i=0; i < blockCount; i++)
    {
        BLOCK *b = blockArray[i];
        if (b && b->head == b->tail)
        {
            QUAD *ins = Alloc(sizeof(QUAD));
            ins->dc.opcode = i_blockend;
            InsertInstruction(b->head, ins);
        }
    }
}
static int fgc(enum e_fgtype type, BLOCK *parent, BLOCK *b)
{
    return TRUE;
}
void SetGlobalTerms(void)
{
    int i, j;
    termCount = 0;
    for (i=0; i < tempCount; i++)
        if (tempInfo[i]->inUse)
            termCount++;
    termMap = Alloc(sizeof(unsigned short) * tempCount);
    termMapUp = Alloc(sizeof(unsigned short) * termCount);
    for (i=0, j=0; i < tempCount; i++)
        if (tempInfo[i]->inUse)
        {
            termMap[i] = j;
            termMapUp[j] = i;
            j++;
        }
}
void GlobalOptimization(void)
{
    int i;
    PadBlocks();
    forwardOrder = oAlloc(sizeof(BLOCK *) * blockCount);
    blocks = 0;
    for (i=0; i < blockCount; i++)
        if (blockArray[i])
            blockArray[i]->visiteddfst = FALSE;
    forwardOrder[blocks++] = blockArray[0];
    for (i=0; i < blocks; i++)
    {
        BLOCK *b = forwardOrder[i];
        BLOCKLIST *bl = b->succ;
        while (bl)
        {
            if (!bl->block->visiteddfst)
            {
                bl->block->visiteddfst = TRUE;
                forwardOrder[blocks++] = bl->block;
            }
            bl = bl->next;
        }		
    }
    reverseOrder = oAlloc(sizeof(BLOCK *) * blockCount);
    blocks = 0;
    for (i=0; i < blockCount; i++)
        if (blockArray[i])
            blockArray[i]->visiteddfst = FALSE;
    reverseOrder[blocks++] = blockArray[exitBlock];
    for (i=0; i < blocks; i++)
    {
        BLOCK *b = reverseOrder[i];
        BLOCKLIST *bl = b->pred;
        while (bl)
        {
            if (!bl->block->visiteddfst)
            {
                bl->block->visiteddfst = TRUE;
                reverseOrder[blocks++] = bl->block;
            }
            bl = bl->next;
        }		
    }
                    
    definesInfo();
    tempBytes = aallocbit(termCount);
    tempBytes2 = aallocbit(termCount);
    tempBytes3 = aallocbit(termCount);
    GatherGlobals();
    GatherTerms();
    CalculateUses();
    CalculateTransparent();
    SetunMoveableTerms();
    CalculateDSafe();
    CalculateEarliest();
    CalculateDelay();
    CalculateLatest();
    CalculateIsolated();
    CalculateOCPAndRO();
    MoveExpressions();
}
