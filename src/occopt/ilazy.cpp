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
#include "iexpr.h"
#include "beinterf.h"

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
extern FILE* icdFile;
extern COMPILER_PARAMS cparams;
extern ARCH_ASM* chosenAssembler;
extern bool setjmp_used;

extern int walkPostorder, walkPreorder;

extern int exitBlock;
extern int cachedTempCount;
extern BITINT bittab[BITINTBITS];
extern int blockCount;
extern BLOCK** blockArray;
extern TEMP_INFO** tempInfo;
extern int tempCount;
extern QUAD *intermed_head, *intermed_tail;
extern int walkPostorder;
extern BITINT* uivBytes;
extern SimpleSymbol* currentFunction;
unsigned short* termMap;
unsigned short* termMapUp;
unsigned termCount;

static BLOCK **reverseOrder, **forwardOrder;
static BITINT *tempBytes, *tempBytes2, *tempBytes3;
static int blocks;
static BITINT* unMoveableTerms;
static BITINT* ocpTerms;

static void ormap(BITINT* dest, BITINT* src)
{
    int n = (termCount + BITINTBITS - 1) / BITINTBITS;
    int i;
    for (i = 0; i < n; i++)
    {
        *dest++ |= *src++;
    }
}
static void andmap(BITINT* dest, BITINT* source)
{
    int i;
    int n = (termCount + BITINTBITS - 1) / BITINTBITS;
    for (i = 0; i < n; i++)
        *dest++ &= *source++;
}
static void copymap(BITINT* dest, BITINT* source)
{
    memcpy(dest, source, ((termCount + BITINTBITS - 1) / BITINTBITS) * sizeof(BITINT));
}
static bool samemap(BITINT* dest, BITINT* source)
{
    BITINT* olddest = dest;
    BITINT* oldsrc = source;
    int i;
    int n = (termCount + BITINTBITS - 1) / BITINTBITS;
    for (i = 0; i < n; i++)
        if (*dest++ != *source++)
        {
            copymap(olddest, oldsrc);
            return false;
        }
    return true;
}
static void complementmap(BITINT* dest)
{
    int i;
    int n = (termCount + BITINTBITS - 1) / BITINTBITS;
    for (i = 0; i < n; i++)
        *dest++ ^= (BITINT)-1;
}
static void setmap(BITINT* dest, bool val)
{
    int i;
    int v = val ? 0xff : 0;
    int n = (termCount + BITINTBITS - 1) / BITINTBITS * sizeof(BITINT);
    memset(dest, v, n);
}
static bool AnySet(BITINT* map)
{
    int n = termCount / BITINTBITS;
    int i;
    for (i = 0; i < n; i++)
        if (map[i])
        {
            return true;
        }
    if (i >= n)
        for (i = termCount & -(int)BITINTBITS; i < termCount; i++)
        {
            if (isset(tempBytes, i))
            {
                return true;
            }
        }
    return false;
}
static void EnterGlobal(QUAD* head)
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
    setmap(head->transparent, true);
}
static void GatherGlobals(void)
{
    int i;
    EnterGlobal(blockArray[0]->head);
    setmap(blockArray[0]->head->transparent, false);
    EnterGlobal(blockArray[exitBlock]->tail);
    for (i = 0; i < blockCount; i++)
        if (blockArray[i])
        {
            QUAD* head = blockArray[i]->head;
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
                            ((head->temps & TEMP_ANS) && (head->ans->mode == i_ind || head->ans->offset->sp->pushedtotemp)))
                        {
                            EnterGlobal(head);
                        }
                    }
                    // the gosub is for UIVs, the goto is to allow a convenient point to gather loop invariants
                    // because sometimes this algorithm will move them.  The parms are to optimize
                    // statements associated with parameter pushes...  and the jxx are like variable assignments that affect
                    // the uses set but are transparent to everything...
                    else if (head->dc.opcode == i_gosub || head->dc.opcode == i_parm || head->dc.opcode == i_goto ||
                             ((head->dc.opcode >= i_jne && head->dc.opcode <= i_jge) || head->dc.opcode == i_cmpblock))
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
    setmap(unMoveableTerms, true);
    for (i = 0; i < blockCount; i++)
    {
        BLOCK* b = blockArray[i];
        if (b)
        {
            QUAD* head;
            head = b->head;
            while (head != b->tail->fwd)
            {
                if (!head->ignoreMe && head->dc.opcode != i_label)
                {
                    if (head->temps & TEMP_ANS)
                    {
                        int n = termMap[head->ans->offset->sp->i];
                        if ((!chosenAssembler->arch->hasFloatRegs && head->ans->size >= ISZ_FLOAT) || head->ans->bits ||
                            head->ans->vol || head->ans->offset->sp->loadTemp)
                            clearbit(unMoveableTerms, n);
                    }
                }
                head = head->fwd;
            }
        }
    }
}
static void CalculateUses(void)
{
    int i, j;
    BLOCK* b;
    QUAD *head, *tail;
    setmap(tempBytes, true);
    tail = intermed_tail;
    while (!tail->OCP)
        tail = tail->back;
    while (tail)
    {
        bool first = true;
        head = tail;
        setmap(head->uses, false);
        while (head && (first || !head->OCP))
        {
            first = false;
            if (!head->ignoreMe)
            {
                if (head->temps & TEMP_ANS)
                {
                    if (head->ans->mode == i_ind)
                    {
                        setbit(tail->uses, termMap[head->ans->offset->sp->i]);
                    }
                }
                if (head->temps & TEMP_LEFT)
                {
                    if (head->dc.left->mode == i_ind)
                    {
                        setbit(tail->uses, termMap[head->dc.left->offset->sp->i]);
                    }
                    else if (!head->dc.left->offset->sp->pushedtotemp)
                    {
                        setbit(tail->uses, termMap[head->dc.left->offset->sp->i]);
                    }
                }
                if (head->temps & TEMP_RIGHT)
                {
                    if (head->dc.right->mode == i_ind)
                    {
                        setbit(tail->uses, termMap[head->dc.right->offset->sp->i]);
                    }
                    else if (!head->dc.right->offset->sp->pushedtotemp)
                    {
                        setbit(tail->uses, termMap[head->dc.right->offset->sp->i]);
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
    int n = (termCount + BITINTBITS - 1) / BITINTBITS;
    int i, j, k;
    QUAD *head = nullptr, *tail;
    tail = intermed_tail;
    while (!tail->OCP)
        tail = tail->back;
    while (tail)
    {
        setmap(tail->transparent, true);
        if (tail->dc.opcode == i_gosub)
        {
            QUAD* next = tail->fwd;
            setmap(tempBytes3, false);
            AliasGosub(tail, tempBytes3, tail->transparent, n /** sizeof(BITINT)*/);
            while (next && !next->OCP)
            {
                if (next->dc.left && next->dc.left->retval && (next->temps & TEMP_ANS) && next->ans->mode == i_direct)
                {
                    int n = next->ans->offset->sp->i;
                    clearbit(head->transparent, termMap[n]);
                    if (tempInfo[n]->terms)
                        andmap(tail->transparent, tempInfo[n]->terms);
                    break;
                }
                next = next->fwd;
            }
            complementmap(tempBytes3);
            for (i = 0; i < termCount; i++)
                if (!isset(tempBytes3, i) && tempInfo[termMapUp[i]]->terms)
                {
                    andmap(tail->transparent, tempInfo[termMapUp[i]]->terms);
                }
        }
        else if (tail->dc.opcode == i_assnblock)
        {
            setmap(tempBytes, false);
            AliasStruct(tempBytes, tail->ans, tail->dc.left, tail->dc.right);
            complementmap(tempBytes);
            for (i = 0; i < termCount; i++)
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
                        int n = head->ans->offset->sp->i;
                        if (!tempInfo[n]->uses)
                        {
                            tempInfo[n]->uses = aallocbit(termCount);
                            AliasUses(tempInfo[n]->uses, head->dc.left, true);
                            AliasUses(tempInfo[n]->uses, head->dc.right, true);
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
        QUAD* last = tail;
        do
        {
            tail = tail->fwd;
        } while (tail && !tail->OCP);
        if (tail)
        {
            if (tail->ans && tail->dc.opcode != i_label)
            {
                setmap(tempBytes, false);
                AliasUses(tempBytes, tail->ans, true);
                for (i = 0; i < termCount; i++)
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
static QUAD* previous(QUAD* tail)
{
    BLOCK* b = tail->block;
    do
    {
        tail = tail->back;
    } while (tail && !tail->OCP);
    if (!tail || tail->block != b)
        return nullptr;
    return tail;
}
static QUAD* successor(QUAD* head)
{
    BLOCK* b = head->block;
    do
    {
        head = head->fwd;
    } while (head && !head->OCP);
    if (!head || head->block != b)
        return nullptr;
    return head;
}
static QUAD* First(QUAD* head)
{
    if (head->OCP)
        return head;
    return successor(head);
}
static QUAD* Last(QUAD* tail)
{
    if (tail->OCP)
        return tail;
    return previous(tail);
}
static void CalculateDSafe(void)
{
    int i;
    bool changed;
    do
    {
        changed = false;
        for (i = 0; i < blocks; i++)
        {
            BLOCK* b = reverseOrder[i];
            if (b)
            {
                QUAD* tail = Last(b->tail);
                if (tail)
                {
                    BLOCKLIST* pbl = tail->block->succ;
                    if (pbl)
                    {
                        copymap(tempBytes, tail->transparent);
                        while (pbl)
                        {
                            QUAD* first = First(pbl->block->head);
                            if (first)
                            {
                                andmap(tempBytes, first->dsafe);
                            }
                            else if (!pbl->block->dead)
                                diag("Empty block in dsafe");
                            pbl = pbl->next;
                        }
                        ormap(tempBytes, tail->uses);
                        changed |= !samemap(tail->dsafe, tempBytes);
                    }
                    while (true)
                    {
                        QUAD* prev = previous(tail);
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
    bool changed;
    setmap(blockArray[0]->head->earliest, true);
    do
    {
        changed = false;
        for (i = 0; i < blocks; i++)
        {
            BLOCK* b = forwardOrder[i];
            if (b)
            {
                QUAD* head = First(b->head);
                if (head)
                {
                    BLOCKLIST* pbl = head->block->pred;
                    if (pbl)
                    {
                        setmap(tempBytes3, true);
                        while (pbl)
                        {
                            QUAD* tail = Last(pbl->block->tail);
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
                            else if (!pbl->block->dead)
                                diag("Empty block in earliest");
                            pbl = pbl->next;
                        }
                        changed |= !samemap(head->earliest, tempBytes3);
                    }
                    while (true)
                    {
                        QUAD* next = successor(head);
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
    bool changed;
    do
    {
        changed = false;
        for (i = 0; i < blocks; i++)
        {
            BLOCK* b = forwardOrder[i];
            if (b)
            {
                QUAD* head = First(b->head);
                if (head)
                {
                    BLOCKLIST* pbl = head->block->pred;
                    copymap(tempBytes3, head->dsafe);
                    andmap(tempBytes3, head->earliest);
                    if (pbl)
                    {
                        setmap(tempBytes, false);
                        while (pbl)
                        {
                            QUAD* tail = Last(pbl->block->tail);
                            if (tail)
                            {
                                copymap(tempBytes2, tail->uses);
                                complementmap(tempBytes2);
                                andmap(tempBytes2, tail->delay);
                                andmap(tempBytes, tempBytes2);
                            }
                            else if (!pbl->block->dead)
                                diag("Empty block in delay");
                            pbl = pbl->next;
                        }
                        ormap(tempBytes3, tempBytes);
                    }
                    changed |= !samemap(head->delay, tempBytes3);
                    while (true)
                    {
                        QUAD* next = successor(head);
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
    for (i = 0; i < blocks; i++)
    {
        BLOCK* b = reverseOrder[i];
        if (b)
        {
            QUAD* tail = Last(b->tail);
            if (tail)
            {
                BLOCKLIST* pbl = tail->block->succ;
                if (pbl)
                {
                    setmap(tail->latest, true);
                    while (pbl)
                    {
                        QUAD* first = First(pbl->block->head);
                        if (first)
                            andmap(tail->latest, first->delay);
                        else if (!pbl->block->dead)
                            diag("Empty block in latest");
                        pbl = pbl->next;
                    }
                    complementmap(tail->latest);
                }
                else
                {
                    setmap(tail->latest, false);
                }
                ormap(tail->latest, tail->uses);
                andmap(tail->latest, tail->delay);
                while (true)
                {
                    QUAD* prev = previous(tail);
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
    bool changed;
    for (i = 0; i < blocks; i++)
        setmap(reverseOrder[i]->head->isolated, true);
    do
    {
        changed = false;
        for (i = 0; i < blocks; i++)
        {
            BLOCK* b = reverseOrder[i];
            if (b && b->blocknum != exitBlock)
            {
                QUAD* tail = Last(b->tail);
                if (tail)
                {
                    BLOCKLIST* pbl = tail->block->succ;
                    if (pbl)
                    {
                        setmap(tempBytes, true);
                        while (pbl)
                        {
                            QUAD* first = First(pbl->block->head);
                            if (first)
                            {
                                copymap(tempBytes2, first->uses);
                                complementmap(tempBytes2);
                                andmap(tempBytes2, first->isolated);
                                ormap(tempBytes2, first->latest);
                                andmap(tempBytes, tempBytes2);
                            }
                            else if (!pbl->block->dead)
                                diag("Empty block in isolated");
                            pbl = pbl->next;
                        }
                        changed |= !samemap(tail->isolated, tempBytes);
                    }
                    while (true)
                    {
                        QUAD* prev = previous(tail);
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
    QUAD* tail = intermed_tail;
    while (tail && !tail->OCP)
        tail = tail->back;
    for (i = 0; i < tempCount; i++)
    {
        tempInfo[i]->termClear = true;
    }
    while (tail)
    {
        tail = tail->back;
        while (tail && !tail->OCP)
        {
            if ((tail->temps & TEMP_ANS) && tail->ans->mode == i_direct)
            {
                int n = tail->ans->offset->sp->i;
                if ((tail->temps & (TEMP_LEFT | TEMP_RIGHT)) || tail->dc.left->retval)
                {
                    if (((tail->temps & TEMP_LEFT) || tail->dc.left->retval) /*&& 
                        (!tail->dc.left->offset->sp->pushedtotemp)*/)
                    {
                        int l = tail->dc.left->offset->sp->i;
                        if (!tempInfo[l]->terms)
                        {
                            tempInfo[l]->terms = aallocbit(termCount);
                        }
                        if (tempInfo[l]->termClear)
                        {
                            setmap(tempInfo[l]->terms, true);
                            tempInfo[l]->termClear = false;
                        }
                        if (tempInfo[n]->terms)
                        {
                            andmap(tempInfo[l]->terms, tempInfo[n]->terms);
                        }
                        clearbit(tempInfo[l]->terms, termMap[n]);
                    }
                    if ((tail->temps & TEMP_RIGHT) && !tail->dc.right->offset->sp->pushedtotemp)
                    {
                        int l = tail->dc.right->offset->sp->i;
                        if (!tempInfo[l]->terms)
                        {
                            tempInfo[l]->terms = aallocbit(termCount);
                        }
                        if (tempInfo[l]->termClear)
                        {
                            setmap(tempInfo[l]->terms, true);
                            tempInfo[l]->termClear = false;
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
    for (i = 0; i < termCount; i++)
        if (!isset(uivBytes, i) && tempInfo[termMapUp[i]]->terms)
        {
            andmap(uivBytes, tempInfo[termMapUp[i]]->terms);
        }
}
static void CalculateOCPAndRO(void)
{
    int i;
    ocpTerms = aallocbit(termCount);
    for (i = 0; i < blocks; i++)
    {
        if (forwardOrder[i])
        {
            QUAD* head = forwardOrder[i]->head;
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
static void HandleOCP(QUAD* after, int tn)
{
    if (tempInfo[tn]->idefines)
    {
        QUAD* p = (QUAD*)(tempInfo[tn]->idefines->data);
        QUAD* tail = after;
        QUAD* ins = (QUAD*)Alloc(sizeof(QUAD));
        QUAD* bans;
        bool a = false, l = false;
        if (after->dc.opcode != i_block && after->dc.opcode != i_label)
        {
            QUAD *beforea, *beforel = nullptr;
            after = after->back;
            beforea = nullptr;
            while (!after->OCP && !beforel && !after->ignoreMe && after->dc.opcode != i_label &&
                   (!after->dc.left || !after->dc.left->retval) && after->dc.opcode != i_block)
            {
                if (after->temps & TEMP_ANS)
                {
                    if (tn == after->ans->offset->sp->i)
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
        ins->uses = nullptr;
        ins->transparent = nullptr;
        ins->dsafe = nullptr;
        ins->earliest = nullptr;
        ins->delay = nullptr;
        ins->latest = nullptr;
        ins->isolated = nullptr;
        ins->OCP = nullptr;
        ins->RO = nullptr;

        ins->OCPInserted = true;
        ins->fwd = after->fwd;
        ins->back = after;
        ins->back->fwd = ins;
        ins->fwd->back = ins;
        ins->block = after->block;
        if (after == after->block->tail)
            after->block->tail = ins;
        bans = (QUAD*)Alloc(sizeof(QUAD));
        bans->ans = tempInfo[tn]->copy;
        bans->dc.left = ins->ans;
        bans->dc.opcode = i_assn;
        bans->OCPInserted = true;
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
static IMODE* GetROVar(IMODE* oldvar, IMODE* newvar, bool mov)
{
    if (oldvar->mode == i_ind)
    {
        IMODELIST* iml = newvar->offset->sp->imind;
        IMODE* im = nullptr;
        while (iml)
        {
            if (iml->im->size == oldvar->size && iml->im->bits == oldvar->bits && iml->im->startbit == oldvar->startbit)
            {
                im = iml->im;
                break;
            }
            iml = iml->next;
        }
        if (!iml)
        {
            IMODELIST* iml2 = (IMODELIST*)Alloc(sizeof(IMODELIST));
            im = (IMODE*)Alloc(sizeof(IMODE));
            *im = *newvar;
            im->mode = i_ind;
            im->size = oldvar->size;
            im->ptrsize = oldvar->ptrsize;
            im->startbit = oldvar->startbit;
            im->bits = oldvar->bits;
            iml2->im = im;
            iml2->next = newvar->offset->sp->imind;
            newvar->offset->sp->imind = iml2;
        }
        newvar = im;
    }
    else
    {
        if (oldvar->offset->sp->pushedtotemp)
            return oldvar;
        if (mov)
        {
            int n = oldvar->offset->sp->i;
            QUAD* q = (QUAD*)tempInfo[n]->idefines->data;
            if (q->dc.opcode == i_assn && q->dc.left->mode == i_immed)
                return q->dc.left;
        }
    }
    return newvar;
}
static void HandleRO(QUAD* after, int tn)
{
    BITINT* OCP = after->OCP;

    if (after->dc.opcode != i_block)
        do
        {
            if (!after->OCPInserted)
            {
                if ((after->temps & TEMP_ANS) && after->ans->mode == i_direct && after->ans->offset->sp->i == tn &&
                    (after->dc.opcode != i_assn || !(after->temps & TEMP_LEFT) || !after->dc.left->offset->sp->pushedtotemp))
                {
                    after->dc.left =
                        GetROVar(after->ans, tempInfo[after->ans->offset->sp->i]->copy, after->dc.opcode == i_assn);
                    after->dc.opcode = i_assn;
                    after->temps &= ~TEMP_RIGHT;
                    after->temps |= TEMP_LEFT;
                    after->dc.right = nullptr;
                    if (after->dc.left->offset->type != se_tempref)
                        after->temps &= ~TEMP_LEFT;
                }
                else
                {

                    if ((after->temps & TEMP_LEFT) && after->dc.left->offset->sp->i == tn)
                    {
                        after->dc.left = GetROVar(after->dc.left, tempInfo[after->dc.left->offset->sp->i]->copy,
                                                  after->dc.opcode == i_assn);
                        if (after->dc.left->offset->type != se_tempref)
                            after->temps &= ~TEMP_LEFT;
                    }
                    if ((after->temps & TEMP_RIGHT) && after->dc.right->offset->sp->i == tn)
                    {
                        after->dc.right = GetROVar(after->dc.right, tempInfo[after->dc.right->offset->sp->i]->copy, false);
                    }
                }
            }
            after = after->back;

        } while (!after->OCP && after->dc.opcode != i_label);
}
static void MoveExpressions(void)
{
    int i, j;
    for (i = 0; i < termCount; i++)
    {
        if (isset(ocpTerms, i))
        {
            int j;
            int size = tempInfo[termMapUp[i]]->enode->sp->imvalue->size;
            tempInfo[termMapUp[i]]->copy = InitTempOpt(size, size);
            for (j = 0; j < blocks; j++)
            {

                QUAD* head = forwardOrder[j]->head;
                while (head != forwardOrder[j]->tail->fwd)
                {
                    if (head->OCP)
                    {
                        if (isset(head->OCP, i))
                        {
                            HandleOCP(head, termMapUp[i]);
                        }
                    }
                    head = head->fwd;
                }
            }
        }
    }
    for (i = 0; i < termCount; i++)
    {
        if (isset(ocpTerms, i))
        {
            int j;
            for (j = 0; j < blocks; j++)
            {

                QUAD* head = forwardOrder[j]->head;
                while (head != forwardOrder[j]->tail->fwd)
                {
                    if (head->OCP)
                    {
                        if (isset(head->RO, i))
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
    QUAD* head = intermed_head;
    int i;
    for (i = 0; i < tempCount; i++)
        tempInfo[i]->temp = -1;
    while (head)
    {
        if ((head->precolored & TEMP_ANS) && !head->ans->retval)
        {
            QUAD* newIns = (QUAD*)Alloc(sizeof(QUAD));
            i = head->ans->offset->sp->i;
            if (tempInfo[i]->temp < 0)
            {
                IMODE* newImode = InitTempOpt(head->ans->size, head->ans->size);
                tempInfo[i]->temp = newImode->offset->sp->i;
                newIns->dc.left = newImode;
            }
            else
            {
                newIns->dc.left = tempInfo[tempInfo[i]->temp]->enode->sp->imvalue;
            }
            newIns->dc.opcode = i_assn;
            newIns->ans = head->ans;
            head->ans = newIns->dc.left;
            InsertInstruction(head, newIns);
        }
        if ((head->precolored & TEMP_LEFT) && !head->dc.left->retval)
        {
            QUAD* newIns = (QUAD*)Alloc(sizeof(QUAD));
            i = head->dc.left->offset->sp->i;
            if (tempInfo[i]->temp < 0)
            {
                IMODE* newImode = InitTempOpt(head->dc.left->size, head->dc.left->size);
                tempInfo[i]->temp = newImode->offset->sp->i;
                newIns->ans = newImode;
            }
            else
            {
                newIns->ans = tempInfo[tempInfo[i]->temp]->enode->sp->imvalue;
            }
            newIns->dc.opcode = i_assn;
            newIns->dc.left = head->dc.left;
            head->dc.left = newIns->ans;
            InsertInstruction(head->back, newIns);
        }
        if ((head->precolored & TEMP_RIGHT) && !head->dc.right->retval)
        {
            QUAD* newIns = (QUAD*)Alloc(sizeof(QUAD));
            i = head->dc.right->offset->sp->i;
            if (tempInfo[i]->temp < 0)
            {
                IMODE* newImode = InitTempOpt(head->dc.right->size, head->dc.right->size);
                tempInfo[i]->temp = newImode->offset->sp->i;
                newIns->ans = newImode;
            }
            else
            {
                newIns->ans = tempInfo[tempInfo[i]->temp]->enode->sp->imvalue;
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
    for (i = 0; i < blockCount; i++)
    {
        BLOCK* b = blockArray[i];
        if (b && b->head == b->tail)
        {
            QUAD* ins = (QUAD*)Alloc(sizeof(QUAD));
            ins->dc.opcode = i_blockend;
            InsertInstruction(b->head, ins);
        }
    }
}
static int fgc(enum e_fgtype type, BLOCK* parent, BLOCK* b) { return true; }
void SetGlobalTerms(void)
{
    int i, j;
    termCount = 0;
    for (i = 0; i < tempCount; i++)
        if (tempInfo[i]->inUse)
            termCount++;
    termMap = (unsigned short*)Alloc(sizeof(unsigned short) * tempCount);
    termMapUp = (unsigned short*)Alloc(sizeof(unsigned short) * termCount);
    for (i = 0, j = 0; i < tempCount; i++)
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
    forwardOrder = (BLOCK**)oAlloc(sizeof(BLOCK*) * blockCount);
    blocks = 0;
    for (i = 0; i < blockCount; i++)
        if (blockArray[i])
            blockArray[i]->visiteddfst = false;
    forwardOrder[blocks++] = blockArray[0];
    for (i = 0; i < blocks; i++)
    {
        BLOCK* b = forwardOrder[i];
        BLOCKLIST* bl = b->succ;
        while (bl)
        {
            if (!bl->block->visiteddfst)
            {
                bl->block->visiteddfst = true;
                forwardOrder[blocks++] = bl->block;
            }
            bl = bl->next;
        }
    }
    reverseOrder = (BLOCK**)oAlloc(sizeof(BLOCK*) * blockCount);
    blocks = 0;
    for (i = 0; i < blockCount; i++)
        if (blockArray[i])
            blockArray[i]->visiteddfst = false;
    reverseOrder[blocks++] = blockArray[exitBlock];
    for (i = 0; i < blocks; i++)
    {
        BLOCK* b = reverseOrder[i];
        BLOCKLIST* bl = b->pred;
        while (bl)
        {
            if (!bl->block->visiteddfst)
            {
                bl->block->visiteddfst = true;
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
