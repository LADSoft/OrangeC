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

/*
 * iblock.c
 *
 * find blocks and do LCSE optimization.  Subroutine calls end blocks;
 * this allows the pointer analysis and GCSE routines to find a wider
 * range of CSEs.  LCSE pass creates lots of dead variables so,
 * we need another optimization to get rid of these later...
 *
 * the DAG handling assumes that Optimizer::IMODE's that describe the same temps have
 * equivalent addresses, e.g. they are the same object.
 *
 */
#include <cstdio>
#include <malloc.h>
#include <cstring>
#include "ioptimizer.h"
#include "beinterfdefs.h"
#include "config.h"
#include "iblock.h"
#include "OptUtils.h"
#include "memory.h"
#include "ilocal.h"

#define DOING_LCSE

namespace Optimizer
{
TEMP_INFO** tempInfo;
BLOCK** blockArray;

Optimizer::QUAD *intermed_head, *intermed_tail;
int blockCount;
std::unordered_map<QUAD*, QUAD*, OrangeC::Utils::fnv1a32_binary<DAGCOMPARE>, OrangeC::Utils::bin_eql<DAGCOMPARE>> ins_hash;
std::unordered_map<IMODE**, QUAD*, OrangeC::Utils::fnv1a32_binary<sizeof(IMODE*)>, OrangeC::Utils::bin_eql<sizeof(IMODE*)>>
    name_hash;
short wasgoto = false;

BLOCK* currentBlock;

int blockMax;

void gen_nodag(enum i_ops op, Optimizer::IMODE* res, Optimizer::IMODE* left, Optimizer::IMODE* right);

/*-------------------------------------------------------------------------*/

int equalimode(Optimizer::IMODE* ap1, Optimizer::IMODE* ap2)
/*
 * return true if the imodes match
 */
{
    if (!ap1 || !ap2)
        return false;
    if (ap1->mode && ap1->mode != ap2->mode)
        return false;
    if (ap1->retval || ap2->retval)
        return false;
    /*   if (ap1->size && ap1->size != ap2->size)*/
    /*      return false;*/
    switch (ap1->mode)
    {

        case i_none:
            return false;
        default:
            if (ap1->offset->isvolatile || ap2->offset->isvolatile)
                return false;
            return equalnode(ap1->offset, ap2->offset);
    }
}

/*-------------------------------------------------------------------------*/

void add_intermed(Optimizer::QUAD* newQuad)
/*
 *      add the icode quad to the icode list
 */
{
    switch (newQuad->dc.opcode)
    {
        case i_line:
        case i_dbgblock:
        case i_dbgblockend:
        case i_varstart:
        case i_func:
            newQuad->ignoreMe = true;
            break;
        default:
            break;
    };
    if (intermed_head == 0)
    {
        intermed_head = intermed_tail = newQuad;
        newQuad->fwd = 0;
        newQuad->back = 0;
        if (newQuad->dc.opcode != i_block)
            currentBlock = nullptr;
    }
    else
    {
        newQuad->fwd = 0;
        newQuad->back = intermed_tail;
        intermed_tail->fwd = newQuad;
        intermed_tail = newQuad;
    }
    newQuad->block = currentBlock;
}
Optimizer::IMODE* liveout2(Optimizer::QUAD* q)
{
    Optimizer::IMODE* rv;
    if (!q)
        return nullptr;
    return q->ans;
#ifdef XXXXX
    if (q->dc.opcode == i_assn)
        if (q->livein & IM_LIVELEFT)
            if (q->dc.left->retval)
                rv = q->ans;
            else
                rv = q->dc.left;
        else
            rv = liveout2((Optimizer::QUAD*)q->dc.left);
    else
        rv = q->ans;
    return rv;
#endif
}
/*-------------------------------------------------------------------------*/

Optimizer::QUAD* liveout(Optimizer::QUAD* node)
{
    Optimizer::QUAD* outnode;
    outnode = Allocate<Optimizer::QUAD>();
    outnode->dc.opcode = node->dc.opcode;
    outnode->ans = node->ans;
    outnode->dc.v = node->dc.v;
    if (node->livein & IM_LIVELEFT)
        outnode->dc.left = node->dc.left;
    else
        outnode->dc.left = liveout2((Optimizer::QUAD*)node->dc.left);

    if (node->livein & IM_LIVERIGHT)
        outnode->dc.right = node->dc.right;
    else
        outnode->dc.right = liveout2((Optimizer::QUAD*)node->dc.right);
    return outnode;
}

#ifdef DOING_LCSE
int ToQuadConst(Optimizer::IMODE** im)
{
    if (*im && (*im)->mode == i_immed)
    {
        Optimizer::QUAD *rv, temp;
        memset(&temp, 0, sizeof(temp));
        if (isintconst((*im)->offset))
        {
            temp.dc.opcode = i_icon;
            temp.dc.v.i = (*im)->offset->i;
        }
        else if (isfloatconst((*im)->offset))
        {
            temp.dc.opcode = i_fcon;
            temp.dc.v.f = (*im)->offset->f;
        }
        else if (isimaginaryconst((*im)->offset))
        {
            temp.dc.opcode = i_imcon;
            temp.dc.v.f = (*im)->offset->f;
        }
        else if (iscomplexconst((*im)->offset))
        {
            temp.dc.opcode = i_cxcon;
            temp.dc.v.c.r = (*im)->offset->c.r;
            temp.dc.v.c.i = (*im)->offset->c.i;
        }
        else
        {
            /*			DIAG("ToQuadConst:  unknown constant type");*/
            /* might get address constants here*/
            return 0;
        }
        rv = nullptr;
        auto it = ins_hash.find(&temp);
        if (it != ins_hash.end())
            rv = it->second;

        if (!rv)
        {
            rv = Allocate<Optimizer::QUAD>();
            *rv = temp;
            rv->ans = tempreg(ISZ_UINT, 0);
            add_intermed(rv);
            ins_hash[rv] = rv;
            wasgoto = false;
        }
        *im = (Optimizer::IMODE*)rv;
        return 1; /* it is now not a livein node any more*/
    }
    return 0;
}
#endif
bool usesAddress(Optimizer::IMODE* im)
{
    if (im->offset)
    {
        switch (im->offset->type)
        {
            case Optimizer::se_auto:
            case Optimizer::se_pc:
            case Optimizer::se_absolute:
            case Optimizer::se_global:
            case Optimizer::se_threadlocal:
                return true;
            default:
                return false;
        }
    }
    return false;
}
/*
 * this is the primary local CSE subroutine
 *
 * The steps are:
 *  1) replace any named operand Optimizer::IMODE with the Optimizer::QUAD it refers to.  Otherwise
 *     if there is none set the livein flag for the Optimizer::IMODE so it
 *     can be distinguished from quads later
 *  2) constant fold and algebra
 *  3) look up the resulting quad in the CSE table and replace the
 *     instruction with an assigment if it is there, otherwise
 *     enter the quad in the CSE table
 *  4) convert the resulting quad Optimizer::QUAD members back to Optimizer::IMODE members
 *  5) save the final Optimizer::QUAD back to the names table for use in step 1 of
 *     subsequent instructions
 *
 * if local CSEs are disabled we simply don't put anything in the lookup
 * tables.
 *
 * if a volatile var is on the right-hand side we don't put the
 * CSE in the CSE table.
 *
 * if a variable on the left is volatile we create a temp variable
 * for the result and then assign the volatile to the temp.  This
 * prevents us from using a volatile as an intermediate.
 *
 * by the time we get here, casts will have been assigned unique
 * operand values so they won't be CSE'd.  I may fix it some time in
 * the future.
 *
 * PTR indirections won't be CSE'd because the Optimizer::IMODE structs will be
 * regenerated each time a pointer is indirected.  This is what we want
 * since we can't be sure two pointers don't point to the same thing.
 * if we implement the RESTRICT keyword this will have to change.
 *
 * Yes this does rely on the Optimizer::IMODE addresses being the same each
 * time a variable is used.  IEXPR makes sure the Optimizer::IMODE addresses are
 * constant for each global and local variable, both when used as an
 * address and when used as data.
 */
static Optimizer::QUAD* add_dag(Optimizer::QUAD* newQuad)
{
    Optimizer::QUAD* outnode;
#ifdef DOING_LCSE
    Optimizer::QUAD* node;
    /* if the left-hand side is volatile, insert a temp so we can keep
     * going with CSEs
     */
    /*
    if (newQuad->ans && newQuad->ans->vol)
    {
        Optimizer::QUAD *tquad;
        Optimizer::IMODE *treg;
        treg = tempreg(newQuad->ans->size, 0);
        tquad = Allocate<Optimizer::QUAD>();
        tquad->ans = newQuad->ans;
        tquad->dc.left = treg;
        tquad->dc.opcode = i_assn;
        newQuad->ans = treg;
        add_dag(newQuad);
        newQuad = tquad;
    }
    */

    /* Transform the quad structure members from imodes to quads */
    node = nullptr;
    auto it = name_hash.find(&newQuad->dc.left);
    if (it != name_hash.end())
        node = it->second;
    if (node)
    {
        if (node->dc.opcode == i_assn && node->dc.left->mode == i_immed && !node->ans->offset->sp->storeTemp && node->dc.left->size == newQuad->dc.left->size)
        {
            newQuad->dc.left = node->dc.left;
            newQuad->livein |= IM_LIVELEFT;
        }
        else if (node->ans->size == newQuad->dc.left->size && node->ans->bits == newQuad->dc.left->bits)
            newQuad->dc.left = (Optimizer::IMODE*)node;
        else
            newQuad->livein |= IM_LIVELEFT;
    }
    else
    {
        /*		if (!ToQuadConst(&newQuad->dc.left))*/
        newQuad->livein |= IM_LIVELEFT;
    }
    node = nullptr;
    it = name_hash.find(&newQuad->dc.right);
    if (it != name_hash.end())
        node = it->second;
    if (node)
    {
        if (node->dc.opcode == i_assn && node->dc.left->mode == i_immed && node->dc.left->size == newQuad->dc.right->size)
        {
            newQuad->dc.right = node->dc.left;
            newQuad->livein |= IM_LIVERIGHT;
        }
        else if (node->ans->size == newQuad->dc.right->size && node->ans->bits == newQuad->dc.right->bits)
            newQuad->dc.right = (Optimizer::IMODE*)node;
        else
            newQuad->livein |= IM_LIVERIGHT;
    }
    else
    {
        /*		if (!ToQuadConst(&newQuad->dc.right))*/
        newQuad->livein |= IM_LIVERIGHT;
    }

    /* constant folding, now! */
    /*    ConstantFold(newQuad); */

    /* Now replace the CSE or enter it into the table */
    node = nullptr;
    auto it1 = ins_hash.find(newQuad);
    if (it1 != ins_hash.end())
        node = it1->second;

    if (!node || (newQuad->dc.opcode == i_assn && node->ans->size != newQuad->ans->size) || node->ans->bits != newQuad->ans->bits)
    {
        if ((cparams.prm_optimize_for_speed || cparams.prm_optimize_for_size) &&
            !(Optimizer::chosenAssembler->arch->denyopts & DO_NOLCSE))
        {
            /* take care of volatiles by not registering volatile expressions
             * in the CSE table.  At this point a temp var will already exist
             * in the case that a volatile exists as the answer.
             */
            if (newQuad->ans && !newQuad->atomic && (newQuad->dc.opcode != i_assn || newQuad->dc.left->mode != i_immed) &&
                (!newQuad->ans->vol && !newQuad->ans->retval &&
                 (newQuad->ans->size < ISZ_FLOAT || Optimizer::chosenAssembler->arch->hasFloatRegs) &&
                 (!newQuad->dc.left || !(newQuad->livein & IM_LIVELEFT) ||
                  (!newQuad->dc.left->vol &&
                   (newQuad->dc.left->size < ISZ_FLOAT || Optimizer::chosenAssembler->arch->hasFloatRegs))) &&
                 (!newQuad->dc.right || !(newQuad->livein & IM_LIVERIGHT) ||
                  (!newQuad->dc.right->vol &&
                   (newQuad->dc.right->size < ISZ_FLOAT || Optimizer::chosenAssembler->arch->hasFloatRegs)))))
                if (newQuad->dc.opcode != i_add || (!(newQuad->livein & IM_LIVELEFT) || newQuad->dc.left->mode != i_immed) ||
                    (!(newQuad->livein & IM_LIVERIGHT) || newQuad->dc.right->mode != i_immed))
                {
                    if (newQuad->dc.opcode != i_parmstack)
                        if (newQuad->dc.opcode != i_assn ||
                            (!newQuad->genConflict &&
                             (!(newQuad->livein & IM_LIVELEFT) || newQuad->ans->size == newQuad->dc.left->size)))
                             ins_hash[newQuad] = newQuad;
                }
        }
        /* convert back to a quad structure and generate code */
        node = newQuad;
        outnode = liveout(node);
        outnode->genConflict = newQuad->genConflict;
        outnode->atomic = newQuad->atomic;
        add_intermed(outnode);
    }
    else
    {
        outnode = Allocate<Optimizer::QUAD>();
        outnode->dc.opcode = i_assn;
        outnode->ans = newQuad->ans;
        outnode->dc.left = node->ans;
        outnode->genConflict = newQuad->genConflict;
        outnode->atomic = newQuad->atomic;
        if (outnode->ans != outnode->dc.left)
            add_intermed(outnode);
    }
    /* Save the newQuad node structure for later lookups
     * always save constants even when no LCSE is to be done because it
     * is needed for constant-folding in subsequent instructions
     */
    if ((cparams.prm_optimize_for_speed || cparams.prm_optimize_for_size) &&
        !(Optimizer::chosenAssembler->arch->denyopts & DO_NOLCSE))

    {
        if (newQuad->ans && (newQuad->ans->mode == i_ind || newQuad->ans->offset->type != Optimizer::se_tempref))
            flush_dag(true);
        else if (newQuad->ans /* &&  (newQuad->dc.opcode != i_assn || (newQuad->livein & IM_LIVELEFT)) */
                 && ((cparams.prm_optimize_for_speed || cparams.prm_optimize_for_size) &&
                     ((newQuad->ans->offset->type == Optimizer::se_tempref && newQuad->ans->mode == i_direct) ||
                      node->dc.opcode == i_icon || node->dc.opcode == i_fcon || node->dc.opcode == i_imcon ||
                      node->dc.opcode == i_cxcon)))
        {
            name_hash[&newQuad->ans] = node;
        }
    }
#else
    add_intermed(newQuad);
    outnode = newQuad;
#endif
    return outnode;
}

/*-------------------------------------------------------------------------*/
void flush_dag(bool leaveAddresses)
{
#ifdef DOING_LCSE
    ins_hash.clear();
    if (leaveAddresses)
    {
        for (auto it = name_hash.begin(); it != name_hash.end();)
        {
            auto n = it->second;
            if (n->dc.opcode == i_assn)
            {
                if (n->dc.left->mode == i_immed && usesAddress(n->dc.left))
                    ++it;
                else
                    it = name_hash.erase(it);
            }
            else
            {
               it = name_hash.erase(it);
            }
        }
    }
    else
    {
        name_hash.clear();
    }
#endif
}

/*-------------------------------------------------------------------------*/

void dag_rundown(void)
{
#ifdef DOING_LCSE
    ins_hash.clear();
    name_hash.clear();
#endif
}

/*-------------------------------------------------------------------------*/
BLOCKLIST* newBlock(void)
{
    if (blockCount == 0)
    {
        memset(blockArray, 0, sizeof(BLOCK*) * blockMax);
    }
    BLOCK* block = Allocate<BLOCK>();
    BLOCKLIST* list = Allocate<BLOCKLIST>();
    list->next = 0;
    list->block = block;
    block->blocknum = blockCount++;
    if (blockCount >= blockMax)
    {
        BLOCK** newBlocks = (BLOCK**)calloc(sizeof(BLOCK*), blockMax + 1000);
        memcpy(newBlocks, blockArray, sizeof(BLOCK*) * blockMax);
        free(blockArray);
        blockMax += 1000;
        blockArray = newBlocks;
    }
    blockArray[block->blocknum] = block;
    currentBlock = block;
    return list;
}
void addblock(int val)
/*
 * create a block
 */
{
    BLOCKLIST* list;

    Optimizer::QUAD* q;
    if (blockCount)
    {
        currentBlock->tail = intermed_tail;
    }
    switch (val)
    {
        case i_ret:
        case i_rett:
            return;
    }

    /* block statement gets included */
    q = Allocate<Optimizer::QUAD>();
    q->dc.opcode = i_block;
    q->ans = q->dc.right = 0;
    q->dc.v.label = blockCount;
    add_intermed(q);
    /* now make a basic block and add to the blocklist */
    list = newBlock();
    list->block->head = intermed_tail;
    list->block->tail = intermed_tail;
    q->block = list->block;
}

/*
 * intermed code poitners
 */
void gen_label(int labno)
/*
 *      add a compiler generated label to the intermediate list.
 */
{
    Optimizer::QUAD* newQuad;
    if (labno < 0)
        diag("gen_label: uncompensatedlabel");
    flush_dag();
    if (!wasgoto)
        addblock(i_label);
    wasgoto = false;
    newQuad = Allocate<Optimizer::QUAD>();
    newQuad->dc.opcode = i_label;
    newQuad->dc.v.label = labno;
    add_intermed(newQuad);
}
/*-------------------------------------------------------------------------*/

Optimizer::QUAD* gen_icode_with_conflict(enum i_ops op, Optimizer::IMODE* res, Optimizer::IMODE* left, Optimizer::IMODE* right,
                                         bool conflicting, bool atomic)
/*
 *      generate a code sequence into the peep list.
 */
{
    Optimizer::QUAD* newQuad;
    if (right && right->mode == i_immed && right->size != left->size)
    {
        if (op != i_lsl && op != i_asr && op != i_lsr)
        {
            Optimizer::IMODE* newRight = Allocate<Optimizer::IMODE>();
            *newRight = *right;
            right = newRight;
            right->size = left->size;
        }
    }
    switch (op)
    {
        case i_ret:
        case i_rett:
        case i_directbranch:
        case i_gosub:
            flush_dag();
            break;
        default:
            break;
    }
    newQuad = Allocate<Optimizer::QUAD>();
    newQuad->genConflict = conflicting;
    newQuad->dc.opcode = op;
    newQuad->dc.left = left;
    newQuad->dc.right = right;
    newQuad->ans = res;
    if (atomic)
    {
        newQuad->atomic = true;
    }
    newQuad = add_dag(newQuad);
    switch (op)
    {
        case i_ret:
        case i_rett:
            flush_dag();
            break;
        default:
            break;
    }
    wasgoto = false;
    return newQuad;
}
Optimizer::QUAD* gen_icode(enum i_ops op, Optimizer::IMODE* res, Optimizer::IMODE* left, Optimizer::IMODE* right)
{
    return gen_icode_with_conflict(op, res, left, right, false);
}
QUAD* gen_icode_with_atomic(enum i_ops op, IMODE* res, IMODE* left, IMODE* right)
{
    return gen_icode_with_conflict(op, res, left, right, false, true);
}

/*-------------------------------------------------------------------------*/

void gen_iiconst(Optimizer::IMODE* res, long long val)
/*
 *      generate an integer constant sequence into the peep list.
 */
{
    Optimizer::QUAD* newQuad;
    Optimizer::IMODE* left = make_immed(ISZ_UINT, val);
    newQuad = Allocate<Optimizer::QUAD>();
    newQuad->dc.opcode = i_assn;
    newQuad->ans = res;
    newQuad->dc.left = left;
    add_dag(newQuad);
    wasgoto = false;
}

/*-------------------------------------------------------------------------*/

void gen_ifconst(Optimizer::IMODE* res, FPF val)
/*
 *      generate an integer constant sequence into the peep list.
 */
{
    Optimizer::QUAD* newQuad;
    newQuad = Allocate<Optimizer::QUAD>();
    newQuad->dc.opcode = i_fcon;
    newQuad->dc.v.f = val;
    newQuad->ans = res;
    add_dag(newQuad);
    wasgoto = false;
}

/*-------------------------------------------------------------------------*/

void gen_igoto(enum i_ops op, long label)
/*
 *      generate a code sequence into the peep list.
 */
{
    Optimizer::QUAD* newQuad;
    flush_dag();
    newQuad = Allocate<Optimizer::QUAD>();
    newQuad->dc.opcode = op;
    newQuad->dc.left = newQuad->dc.right = newQuad->ans = 0;
    newQuad->dc.v.label = label;
    add_intermed(newQuad);
    addblock(i_goto);
    wasgoto = true;
}

/*-------------------------------------------------------------------------*/

void gen_data(int val)
{
    Optimizer::QUAD* newQuad;
    flush_dag();
    newQuad = Allocate<Optimizer::QUAD>();
    newQuad->dc.opcode = i_genword;
    newQuad->dc.left = newQuad->dc.right = newQuad->ans = 0;
    newQuad->dc.v.label = val;
    add_intermed(newQuad);
    wasgoto = false;
}

/*-------------------------------------------------------------------------*/

void gen_icgoto(enum i_ops op, long label, Optimizer::IMODE* left, Optimizer::IMODE* right)
/*
 *      generate a code sequence into the peep list.
 */
{
    Optimizer::QUAD* newQuad;
    if (right && right->mode == i_immed /*&& right->size == ISZ_NONE*/)
        right->size = left->size;

    newQuad = Allocate<Optimizer::QUAD>();
    newQuad->dc.opcode = op;
    newQuad->dc.left = left;
    newQuad->dc.right = right;
    newQuad->ans = 0;
    newQuad->dc.v.label = label;
    add_dag(newQuad);
    flush_dag();
    addblock(op);
    wasgoto = true;
}

/*-------------------------------------------------------------------------*/

Optimizer::QUAD* gen_igosub(enum i_ops op, Optimizer::IMODE* left)
/*
 *      generate a code sequence into the peep list.
 */
{
    Optimizer::QUAD* newQuad;

    newQuad = Allocate<Optimizer::QUAD>();
    newQuad->dc.opcode = op;
    newQuad->dc.left = left;
    newQuad->dc.right = 0;
    newQuad->ans = 0;
    newQuad->dc.v.label = 0;
    add_dag(newQuad);
    flush_dag();
    /*     addblock(op); */
    wasgoto = true;
    return intermed_tail;
}

/*-------------------------------------------------------------------------*/

void gen_icode2(enum i_ops op, Optimizer::IMODE* res, Optimizer::IMODE* left, Optimizer::IMODE* right, int label)
/*
 *      generate a code sequence into the peep list.
 *		only being used for switches
 */
{
    Optimizer::QUAD* newQuad;
    newQuad = Allocate<Optimizer::QUAD>();
    newQuad->dc.opcode = op;
    newQuad->dc.left = left;
    newQuad->dc.right = right;
    newQuad->ans = res;
    newQuad->dc.v.label = label;
    add_intermed(newQuad);
    wasgoto = false;
}

/*-------------------------------------------------------------------------*/

void gen_line(LINEDATA* data)
/*
 * generate a line number statement
 */
{
    Optimizer::QUAD* newQuad;
    if (data == 0)
        return;
    newQuad = Allocate<Optimizer::QUAD>();
    newQuad->dc.opcode = i_line;
    newQuad->dc.left = (Optimizer::IMODE*)data; /* text */
    add_intermed(newQuad);
}

/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/

void gen_nodag(enum i_ops op, Optimizer::IMODE* res, Optimizer::IMODE* left, Optimizer::IMODE* right)
/*
 *      generate a code sequence into the peep list.
 */
{
    Optimizer::QUAD* newQuad;
    newQuad = Allocate<Optimizer::QUAD>();
    newQuad->dc.opcode = op;
    newQuad->dc.left = left;
    newQuad->dc.right = right;
    newQuad->ans = res;
    add_intermed(newQuad);
    wasgoto = false;
}
void RemoveFromUses(Optimizer::QUAD* ins, int tnum)
{
    INSTRUCTIONLIST** l = &tempInfo[tnum]->instructionUses;
    while (*l)
    {
        if ((*l)->ins == ins)
        {
            (*l) = (*l)->next;
            break;
        }
        l = &(*l)->next;
    }
}
void InsertUses(Optimizer::QUAD* ins, int tnum)
{
    INSTRUCTIONLIST* l = oAllocate<INSTRUCTIONLIST>();
    l->next = tempInfo[tnum]->instructionUses;
    l->ins = ins;
    tempInfo[tnum]->instructionUses = l;
}
void RemoveInstruction(Optimizer::QUAD* ins)
{
    if (ins->dc.opcode == i_assn && ins->dc.left->retval)
    {
        // this is so we can inform the backend that the store is gone, e.g. so any fstp will not disappear entirely
        Optimizer::QUAD* find = ins->back;
        while (find->dc.opcode != i_block && find->dc.opcode != i_gosub)
            find = find->back;
        if (find->dc.opcode == i_gosub)
            find->novalue = ins->dc.left->size;
    }
    switch (ins->dc.opcode)
    {
        case i_dbgblock:
        case i_dbgblockend:
        case i_varstart:
        case i_func:
        case i_label:
        case i_expressiontag:
            return;
        default:
            break;
    }
    if (ins->block->head == ins)
        return;
    if (ins->block->tail == ins)
        ins->block->tail = ins->back;
    ins->back->fwd = ins->fwd;
    if (ins->fwd)
        ins->fwd->back = ins->back;
    if (ins->dc.opcode == i_phi)
    {
        PHIDATA* pd = ins->dc.v.phi;
        struct _phiblock* pb = pd->temps;
        tempInfo[pd->T0]->instructionDefines = nullptr;
        while (pb)
        {
            RemoveFromUses(ins, pb->Tn);
            pb = pb->next;
        }
    }
    else
    {
        if (ins->temps & TEMP_ANS)
        {
            int tnum = ins->ans->offset->sp->i;
            if (ins->ans->mode == i_direct)
                tempInfo[tnum]->instructionDefines = nullptr;
            else
            {
                if (ins->ans->offset)
                    RemoveFromUses(ins, tnum);
                if (ins->ans->offset2)
                    RemoveFromUses(ins, ins->ans->offset2->sp->i);
            }
        }
        if (ins->temps & TEMP_LEFT)
        {
            if (ins->dc.left->offset)
                RemoveFromUses(ins, ins->dc.left->offset->sp->i);
            if (ins->dc.left->offset2)
                RemoveFromUses(ins, ins->dc.left->offset2->sp->i);
        }
        if (ins->temps & TEMP_RIGHT)
        {
            if (ins->dc.right->offset)
                RemoveFromUses(ins, ins->dc.right->offset->sp->i);
            if (ins->dc.right->offset2)
                RemoveFromUses(ins, ins->dc.right->offset2->sp->i);
        }
    }
}
void InsertInstruction(Optimizer::QUAD* before, Optimizer::QUAD* ins)
{
    INSTRUCTIONLIST* l;
    ins->block = before->block;
    if (before->fwd && before->fwd->dc.opcode == i_skipcompare)
        if (before->fwd->dc.v.label)
            before = before->fwd;
    ins->fwd = before->fwd;
    ins->back = before;
    if (before->fwd)
        before->fwd->back = ins;
    before->fwd = ins;
    if (ins->back == ins->block->tail)
        ins->block->tail = ins;
    if (ins->dc.opcode == i_phi)
    {
        PHIDATA* pd = ins->dc.v.phi;
        struct _phiblock* pb = pd->temps;
        tempInfo[pd->T0]->instructionDefines = ins;
        while (pb)
        {
            InsertUses(ins, pb->Tn);
            pb = pb->next;
        }
    }
    else
    {
        ins->temps = 0;
        if (ins->ans && ((ins->ans->offset && ins->ans->offset->type == Optimizer::se_tempref) || ins->ans->offset2))
        {
            ins->temps |= TEMP_ANS;
            if (ins->ans->mode == i_direct)
            {
                tempInfo[ins->ans->offset->sp->i]->instructionDefines = ins;
            }
            else
            {
                if (ins->ans->offset)
                    InsertUses(ins, ins->ans->offset->sp->i);
                if (ins->ans->offset2)
                    InsertUses(ins, ins->ans->offset2->sp->i);
            }
        }
        if (ins->dc.left &&
            ((ins->dc.left->offset && ins->dc.left->offset->type == Optimizer::se_tempref) || ins->dc.left->offset2))
        {
            if (!ins->dc.left->retval)
                ins->temps |= TEMP_LEFT;
            if (ins->dc.left->offset)
                InsertUses(ins, ins->dc.left->offset->sp->i);
            if (ins->dc.left->offset2)
                InsertUses(ins, ins->dc.left->offset2->sp->i);
        }
        if (ins->dc.right && ins->dc.right->offset && ins->dc.right->offset->type == Optimizer::se_tempref)
        {
            if (!ins->dc.right->retval)
                ins->temps |= TEMP_RIGHT;
            if (ins->dc.right->offset)
                InsertUses(ins, ins->dc.right->offset->sp->i);
            if (ins->dc.right->offset2)
                InsertUses(ins, ins->dc.right->offset2->sp->i);
        }
    }
}
}  // namespace Optimizer