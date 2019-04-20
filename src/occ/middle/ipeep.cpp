/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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

/*
 * icode peep optimizer
 *
 * Does branch optimizationns of various sorts
 */
#include <stdio.h>
#include "compiler.h"

extern int firstLabel;
extern int nextLabel;
extern TEMP_INFO** tempInfo;
extern int tempCount;
extern BLOCK** blockArray;
extern int blockCount;
extern int exitBlock;
extern BITINT bittab[BITINTBITS];
extern QUAD* intermed_head;
extern SYMBOL* theCurrentFunc;
extern bool functionHasAssembly;
extern ARCH_ASM* chosenAssembler;

static BITINT* occursInAbnormal;

static QUAD** golist; /* list of goto statements */

void peepini(void) {}

static void scan_gotos(QUAD* head)
/*
 * make a list of goto statements
 */
{
    while (head)
    {
        switch (head->dc.opcode)
        {
            case i_jc:
            case i_jnc:
            case i_jbe:
            case i_ja:
            case i_je:
            case i_jne:
            case i_jge:
            case i_jg:
            case i_jle:
            case i_jl:
                if (head->dc.left->mode == i_immed && head->dc.right->mode == i_immed && !(chosenAssembler->msil))
                {
                    if (isintconst(head->dc.left->offset) && isintconst(head->dc.right->offset))
                    {
                        LLONG_TYPE l = head->dc.left->offset->v.i;
                        LLONG_TYPE r = head->dc.right->offset->v.i;
                        int ok;
                        switch (head->dc.opcode)
                        {
                            case i_jc:
                                ok = (ULLONG_TYPE)l < (ULLONG_TYPE)r;
                                break;
                            case i_jnc:
                                ok = (ULLONG_TYPE)l >= (ULLONG_TYPE)r;
                                break;
                            case i_jbe:
                                ok = (ULLONG_TYPE)l <= (ULLONG_TYPE)r;
                                break;
                            case i_ja:
                                ok = (ULLONG_TYPE)l > (ULLONG_TYPE)r;
                                break;
                            case i_je:
                                ok = l == r;
                                break;
                            case i_jne:
                                ok = l != r;
                                break;
                            case i_jge:
                                ok = l >= r;
                                break;
                            case i_jg:
                                ok = l > r;
                                break;
                            case i_jle:
                                ok = l <= r;
                                break;
                            case i_jl:
                                ok = l < r;
                                break;
                            default:
                                break;
                        }
                        if (ok)
                        {
                            head->dc.opcode = i_goto;
                            head->dc.left = head->dc.right = NULL;
                            head->temps &= ~(TEMP_LEFT | TEMP_RIGHT);
                        }
                        else
                            RemoveInstruction(head);
                    }
                }
            case i_swbranch:
            case i_coswitch:
            case i_goto:
            case i_cmpblock:
                golist[head->dc.v.label - firstLabel] = head;
                break;
            default:
                break;
        }
        head = head->fwd;
    }
}

/*-------------------------------------------------------------------------*/

static void kill_brtonext(BLOCK* b, QUAD* head)
/*
 * branches to the next statement get wiped
 */
{
    QUAD* temp;
    (void)b;
    while (true)
    {
        switch (head->dc.opcode)
        {
            case i_jc:
            case i_jnc:
            case i_jbe:
            case i_ja:
            case i_je:
            case i_jne:
            case i_jge:
            case i_jg:
            case i_jle:
            case i_jl:
            case i_cmpblock:
                if (chosenAssembler->msil)
                    return;
            case i_goto:
                temp = head->fwd;
                while (temp && (temp->dc.opcode == i_label || temp->ignoreMe || temp->dc.opcode == i_block ||
                                temp->dc.opcode == i_blockend))
                {
                    if (temp->dc.opcode == i_label && temp->dc.v.label == head->dc.v.label)
                    {
                        RemoveInstruction(head);
                        return;
                    }
                    temp = temp->fwd;
                }
            default:
                return;
        }
    }
}

static void kill_dupgoto(BLOCK* b, QUAD* head)
{
    (void)b;
    head = head->fwd;
    while (head && head->dc.opcode != i_label)
    {
        if (head->dc.opcode != i_block && head->dc.opcode != i_blockend && head->dc.opcode != i_dbgblock &&
            head->dc.opcode != i_dbgblockend && head->dc.opcode != i_var && head->dc.opcode != i_label && !head->ignoreMe)
            RemoveInstruction(head);
        head = head->fwd;
    }
}
void weed_goto(void)
{
    bool killing = false;
    QUAD* head = intermed_head;
    BLOCK* b = head->block;
    while (head)
    {
        QUAD* next = head->fwd;
        if (head->block != b)
        {
            killing = false;
            b = head->block;
        }
        if (killing)
        {
            if (head->dc.opcode != i_block && head->dc.opcode != i_blockend && head->dc.opcode != i_dbgblock &&
                head->dc.opcode != i_dbgblockend && head->dc.opcode != i_var && head->dc.opcode != i_label && !head->ignoreMe)
                RemoveInstruction(head);
        }
        else if (head->dc.opcode == i_goto)
            killing = true;
        head = next;
    }
}
/*-------------------------------------------------------------------------*/

void kill_labeledgoto(BLOCK* b, QUAD* head)
/*
 * if any goto goes to a label which is immediately followed by a goto,
 * replaces the original goto label with the new label annd possibly
 * get rid of the labeled goto if it is preceded by another goto
 */
{
    QUAD* newhead;
    BLOCKLIST** bt;
    int oldlabel = head->dc.v.label;
    (void)b;
    newhead = head->fwd;
    /* look for following goto */
    while (newhead && (newhead->ignoreMe || newhead->dc.opcode == i_block))
        newhead = newhead->fwd;
    if (!newhead)
        return;
    if (newhead->dc.opcode != i_goto)
        return;
    bt = &newhead->block->pred;
    while (*bt)
    {
        QUAD* tail = (*bt)->block->tail;
        tail = beforeJmp(tail, false);
        switch (tail->dc.opcode)
        {
            case i_goto:
                if (tail->dc.v.label == oldlabel)
                {
                    tail->dc.v.label = newhead->dc.v.label;
                    kill_brtonext(tail->block, tail);
                }
                break;
            case i_jc:
            case i_jnc:
            case i_jbe:
            case i_ja:
            case i_je:
            case i_jne:
            case i_jge:
            case i_jg:
            case i_jle:
            case i_jl:
            case i_cmpblock:
                if (tail->dc.v.label == oldlabel)
                {
                    tail->dc.v.label = newhead->dc.v.label;
                    kill_brtonext(tail->block, tail);
                }
                break;
            case i_coswitch:
                do
                {
                    if (tail->dc.v.label == oldlabel)
                    {
                        tail->dc.v.label = newhead->dc.v.label;
                        kill_brtonext(tail->block, tail);
                    }
                    tail = tail->fwd;
                } while (tail->dc.opcode == i_swbranch);

                break;
            default:
                break;
        }
        bt = &(*bt)->next;
    }
}

/*-------------------------------------------------------------------------*/

void kill_jumpover(BLOCK* b, QUAD* head)
/*
 * Conditionnal jumps over gotos get squashed here
 */
{
    i_ops newtype;
    QUAD* newhead = head->fwd;
    (void)b;
    while (newhead->dc.opcode == i_block || newhead->dc.opcode == i_blockend || newhead->dc.opcode == i_dbgblock ||
           newhead->dc.opcode == i_dbgblockend || newhead->ignoreMe)
    {
        newhead = newhead->fwd;
    }
    /* if followed by while will be voided by intervening labels */
    if (newhead->dc.opcode == i_goto)
    {
        QUAD* fwd = newhead->fwd;
        while (fwd->dc.opcode == i_block || fwd->dc.opcode == i_blockend || newhead->dc.opcode == i_dbgblock ||
               newhead->dc.opcode == i_dbgblockend || fwd->dc.opcode == i_label || fwd->ignoreMe)
        {
            if (fwd->dc.opcode == i_label && head->dc.v.label == fwd->dc.v.label)
                break;
            fwd = fwd->fwd;
        }

        if (fwd->dc.opcode != i_label)
            return;
        /* if it was a goto and we are branching around it,
         * swap the conditional type and
         * put the new label in the goto statement
         */
        head->dc.v.label = newhead->dc.v.label;
        RemoveInstruction(newhead);
        switch (head->dc.opcode)
        {
            case i_jc:
                newtype = i_jnc;
                break;
            case i_jnc:
                newtype = i_jc;
                break;
            case i_jbe:
                newtype = i_ja;
                break;
            case i_ja:
                newtype = i_jbe;
                break;
            case i_je:
                newtype = i_jne;
                break;
            case i_jne:
                newtype = i_je;
                break;
            case i_jge:
                newtype = i_jl;
                break;
            case i_jg:
                newtype = i_jle;
                break;
            case i_jle:
                newtype = i_jg;
                break;
            case i_jl:
                newtype = i_jge;
                break;
            default:
                break;
        }
        /* remove the goto */
        head->dc.opcode = newtype;
        while (1)
        {
            while (newhead && (newhead->dc.opcode == i_block || newhead->ignoreMe || newhead->dc.opcode == i_dbgblock ||
                               newhead->dc.opcode == i_dbgblockend || newhead->dc.opcode == i_blockend))
            {
                newhead = newhead->fwd;
            }
            if (!newhead || newhead->dc.opcode != i_goto)
                break;
            RemoveInstruction(newhead);
            newhead = newhead->fwd;
        }
    }
}
static int peep_assn(BLOCK* b, QUAD* head)
{
    (void)b;
    if (head->temps == (TEMP_LEFT | TEMP_ANS) && !(head->dc.right) && head->dc.left->size == head->ans->size &&
        head->dc.left->mode == i_direct && head->ans->mode == i_direct && !head->dc.left->bits)
    {
        int t0 = head->ans->offset->v.sp->value.i;
        int t1 = head->dc.left->offset->v.sp->value.i;
        if (!isset(occursInAbnormal, t0) && !isset(occursInAbnormal, t1))
        {
            if (t0 == t1)
            {
                RemoveInstruction(head);
                return -1;
            }
        }
    }
    if (!(chosenAssembler->arch->denyopts & DO_NOOPTCONVERSION))
    {
        if (head->temps == (TEMP_LEFT | TEMP_ANS) && head->dc.left->size != head->ans->size)
        {
            if (head->back->dc.opcode == i_assn)
            {
                if (head->back->ans->size == head->back->dc.left->size && head->back->ans == head->dc.left)
                    if (head->back->dc.left->mode != i_immed && !head->back->dc.left->bits)
                    {
                        if (!head->back->dc.left->retval)
                        {
                            head->dc.left = head->back->dc.left;
                            if ((!head->dc.left->offset || head->dc.left->offset->type != en_tempref) &&
                                (!head->dc.left->offset2 || head->dc.left->offset2->type != en_tempref))
                                head->temps &= ~TEMP_LEFT;
                            return -1;
                        }
                    }
            }
        }
    }
    return 0;
}
/*-------------------------------------------------------------------------*/

static bool peep(BLOCK* b, bool branches)
/*
 * ICODE peep main routine
 */
{
    QUAD* head = b->head;
    bool changed = false;
    if (b->visiteddfst)
        return changed;
    b->visiteddfst = true;
    while (head != b->tail->fwd)
    {
        int rv = false;
        switch (head->dc.opcode)
        {
            case i_goto:
                if (branches && !functionHasAssembly)
                {
                    kill_dupgoto(b, head);
                    kill_brtonext(b, head);
                }
                break;
            case i_jc:
            case i_jnc:
            case i_jbe:
            case i_ja:
            case i_je:
            case i_jne:
            case i_jge:
            case i_jg:
            case i_jle:
            case i_jl:
                if (branches && !functionHasAssembly)
                {
                    kill_jumpover(b, head);
                    kill_brtonext(b, head);
                }
                break;
            case i_label:
                if (branches)
                {
                    kill_labeledgoto(b, head);
                }
                break;
            case i_nop: /* just kill it */
                RemoveInstruction(head);
                break;
            case i_assn:
                rv = peep_assn(b, head);
                break;
            default:
                break;
        }
        changed |= !!rv;
        if (rv <= 0)
            head = head->fwd;
    }
    return changed;
}
static void scan_abnormal(void)
{
    int i;
    occursInAbnormal = allocbit(tempCount);
    for (i = exitBlock; i < blockCount; i++)
    {
        if (blockArray[i] && blockArray[i]->critical && blockArray[i]->succ)
        {
            BLOCK* b = blockArray[i]->succ->block;
            QUAD* head = b->head->fwd;
            while (head != b->tail->fwd && (head->ignoreMe || head->dc.opcode == i_label))
                head = head->fwd;
            while (head != b->tail->fwd && head->dc.opcode == i_phi)
            {
                PHIDATA* pd = head->dc.v.phi;
                struct _phiblock* pb = pd->temps;
                while (pb && pb->block != blockArray[i])
                    pb = pb->next;
                setbit(occursInAbnormal, pd->T0);
                setbit(occursInAbnormal, pb->Tn);
                head = head->fwd;
            }
        }
    }
}
void peep_icode(bool branches)
{
    int i;
    bool changed;
    golist = (QUAD**)oAlloc(sizeof(QUAD*) * (nextLabel - firstLabel));
    scan_gotos(intermed_head);
    scan_abnormal();
    for (i = 0; i < blockCount; i++)
        if (blockArray[i])
            blockArray[i]->visiteddfst = false;

    do
    {
        changed = false;
        for (i = 0; i < blockCount; i++)
            if (blockArray[i])
                changed |= peep(blockArray[i], branches);
    } while (changed);
}
