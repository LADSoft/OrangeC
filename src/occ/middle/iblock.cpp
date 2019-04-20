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
 * iblock.c
 *
 * find blocks and do LCSE optimization.  Subroutine calls end blocks;
 * this allows the pointer analysis and GCSE routines to find a wider
 * range of CSEs.  LCSE pass creates lots of dead variables so,
 * we need another optimization to get rid of these later...
 *
 * the DAG handling assumes that IMODE's that describe the same temps have
 * equivalent addresses, e.g. they are the same object.
 *
 */
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "compiler.h"

#define DOING_LCSE

extern int codeLabelOffset;
extern COMPILER_PARAMS cparams;
extern TEMP_INFO** tempInfo;
extern ARCH_ASM* chosenAssembler;
extern BLOCK** blockArray;

QUAD *intermed_head, *intermed_tail;
int blockCount;
DAGLIST* ins_hash[DAGSIZE];
DAGLIST* name_hash[DAGSIZE];
short wasgoto = false;

BLOCK* currentBlock;

int blockMax;

static void add_intermed(QUAD* newQuad);
void gen_nodag(enum i_ops op, IMODE* res, IMODE* left, IMODE* right);

int equalnode(EXPRESSION* node1, EXPRESSION* node2)
/*
 *      equalnode will return 1 if the expressions pointed to by
 *      node1 and node2 are equivalent.
 */
{
    if (node1 == 0 || node2 == 0)
        return 0;
    if (node1->type != node2->type)
        return 0;
    if (natural_size(node1) != natural_size(node2))
        return 0;
    switch (node1->type)
    {
        case en_const:
        case en_pc:
        case en_global:
        case en_auto:
        case en_absolute:
        case en_threadlocal:
        case en_structelem:
            return node1->v.sp == node2->v.sp;
        case en_labcon:
            return node1->v.i == node2->v.i;
        default:
            return (!node1->left || equalnode(node1->left, node2->left)) &&
                   (!node1->right || equalnode(node1->right, node2->right));
        case en_c_i:
        case en_c_l:
        case en_c_ul:
        case en_c_ui:
        case en_c_c:
        case en_c_u16:
        case en_c_u32:
        case en_c_bool:
        case en_c_uc:
        case en_c_ll:
        case en_c_ull:
        case en_c_wc:
        case en_nullptr:
            return node1->v.i == node2->v.i;
        case en_c_d:
        case en_c_f:
        case en_c_ld:
        case en_c_di:
        case en_c_fi:
        case en_c_ldi:
            return (node1->v.f == node2->v.f);
        case en_c_dc:
        case en_c_fc:
        case en_c_ldc:
            return (node1->v.c.r == node2->v.c.r) && (node1->v.c.i == node2->v.c.i);
        case en_tempref:
            return node1->v.sp == node2->v.sp;
    }
}

/*-------------------------------------------------------------------------*/

int equalimode(IMODE* ap1, IMODE* ap2)
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

short dhash(UBYTE* str, int len)
/*
 * hashing for dag nodes
 */
{
    int i;
    unsigned short v = 0;
    for (i = 0; i < len; i++)
    {
        v = (v << 3) + (v >> 13) + (v >> 3) + (v << 13);
        v += str[i];
    }
    return v % DAGSIZE;
}

/*-------------------------------------------------------------------------*/

QUAD* LookupNVHash(UBYTE* key, int size, DAGLIST** table)
{
    int hashval = dhash(key, size);
    DAGLIST* list = table[hashval];
    while (list)
    {
        if (list->key && !memcmp(key, list->key, size))
            return (QUAD*)list->rv;
        list = list->next;
    }
    return 0;
}

/*-------------------------------------------------------------------------*/

DAGLIST* ReplaceHash(QUAD* rv, UBYTE* key, int size, DAGLIST** table)
{
    int hashval = dhash(key, size);
    DAGLIST **list = &table[hashval], **flist = list;
    DAGLIST* newDag;
    while (*list)
    {
        if ((*list)->key && !memcmp(key, (*list)->key, size))
        {
            (*list)->rv = (UBYTE*)rv;
            return *list;
        }
        list = *(DAGLIST***)list;
    }
    newDag = (DAGLIST *)oAlloc(sizeof(DAGLIST));
    newDag->rv = (UBYTE*)rv;
    newDag->key = key;
    newDag->next = *flist;
    *flist = newDag;
    return newDag;
}

/*-------------------------------------------------------------------------*/

static void add_intermed(QUAD* newQuad)
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
            currentBlock = NULL;
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
IMODE* liveout2(QUAD* q)
{
    if (!q)
        return NULL;
    return q->ans;
#ifdef XXXXX
    if (q->dc.opcode == i_assn)
        if (q->livein & IM_LIVELEFT)
            if (q->dc.left->retval)
                rv = q->ans;
            else
                rv = q->dc.left;
        else
            rv = liveout2((QUAD*)q->dc.left);
    else
        rv = q->ans;
    return rv;
#endif
}
/*-------------------------------------------------------------------------*/

QUAD* liveout(QUAD* node)
{
    QUAD* outnode;
    outnode = (QUAD*)(QUAD *)Alloc(sizeof(QUAD));
    outnode->dc.opcode = node->dc.opcode;
    outnode->ans = node->ans;
    outnode->dc.v = node->dc.v;
    if (node->livein & IM_LIVELEFT)
        outnode->dc.left = node->dc.left;
    else
        outnode->dc.left = liveout2((QUAD*)node->dc.left);

    if (node->livein & IM_LIVERIGHT)
        outnode->dc.right = node->dc.right;
    else
        outnode->dc.right = liveout2((QUAD*)node->dc.right);
    return outnode;
}

#ifdef DOING_LCSE
int ToQuadConst(IMODE** im)
{
    if (*im && (*im)->mode == i_immed)
    {
        QUAD *rv, temp;
        memset(&temp, 0, sizeof(temp));
        if (isintconst((*im)->offset))
        {
            temp.dc.opcode = i_icon;
            temp.dc.v.i = (*im)->offset->v.i;
        }
        else if (isfloatconst((*im)->offset))
        {
            temp.dc.opcode = i_fcon;
            temp.dc.v.f = (*im)->offset->v.f;
        }
        else if (isimaginaryconst((*im)->offset))
        {
            temp.dc.opcode = i_imcon;
            temp.dc.v.f = (*im)->offset->v.f;
        }
        else if (iscomplexconst((*im)->offset))
        {
            temp.dc.opcode = i_cxcon;
            temp.dc.v.c.r = (*im)->offset->v.c.r;
            temp.dc.v.c.i = (*im)->offset->v.c.i;
        }
        else
        {
            /*			DIAG("ToQuadConst:  unknown constant type");*/
            /* might get address constants here*/
            return 0;
        }
        rv = LookupNVHash((UBYTE*)&temp, DAGCOMPARE, ins_hash);
        if (!rv)
        {
            rv = (QUAD *)Alloc(sizeof(QUAD));
            *rv = temp;
            rv->ans = tempreg(ISZ_UINT, 0);
            add_intermed(rv);
            ReplaceHash(rv, (UBYTE*)rv, DAGCOMPARE, ins_hash);
            wasgoto = false;
        }
        *im = (IMODE*)rv;
        return 1; /* it is now not a livein node any more*/
    }
    return 0;
}
#endif
bool usesAddress(IMODE* im)
{
    if (im->offset)
    {
        switch (im->offset->type)
        {
            case en_auto:
            case en_pc:
            case en_absolute:
            case en_global:
            case en_threadlocal:
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
 *  1) replace any named operand IMODE with the QUAD it refers to.  Otherwise
 *     if there is none set the livein flag for the IMODE so it
 *     can be distinguished from quads later
 *  2) constant fold and algebra
 *  3) look up the resulting quad in the CSE table and replace the
 *     instruction with an assigment if it is there, otherwise
 *     enter the quad in the CSE table
 *  4) convert the resulting quad QUAD members back to IMODE members
 *  5) save the final QUAD back to the names table for use in step 1 of
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
 * PTR indirections won't be CSE'd because the IMODE structs will be
 * regenerated each time a pointer is indirected.  This is what we want
 * since we can't be sure two pointers don't point to the same thing.
 * if we implement the RESTRICT keyword this will have to change.
 *
 * Yes this does rely on the IMODE addresses being the same each
 * time a variable is used.  IEXPR makes sure the IMODE addresses are
 * constant for each global and local variable, both when used as an
 * address and when used as data.
 */
static QUAD* add_dag(QUAD* newQuad)
{
    QUAD* outnode;
#ifdef DOING_LCSE
    QUAD* node;
    /* if the left-hand side is volatile, insert a temp so we can keep
     * going with CSEs
     */
    /*
    if (newQuad->ans && newQuad->ans->vol)
    {
        QUAD *tquad;
        IMODE *treg;
        treg = tempreg(newQuad->ans->size, 0);
        tquad = (QUAD *)Alloc(sizeof(QUAD));
        tquad->ans = newQuad->ans;
        tquad->dc.left = treg;
        tquad->dc.opcode = i_assn;
        newQuad->ans = treg;
        add_dag(newQuad);
        newQuad = tquad;
    }
    */

    /* Transform the quad structure members from imodes to quads */
    node = LookupNVHash((UBYTE*)&newQuad->dc.left, sizeof(void*), name_hash);
    if (node)
    {
        if (node->dc.opcode == i_assn && node->dc.left->mode == i_immed && !node->ans->offset->v.sp->storeTemp)
        {
            newQuad->dc.left = node->dc.left;
            newQuad->livein |= IM_LIVELEFT;
        }
        else if (node->ans->size == newQuad->dc.left->size && node->ans->bits == newQuad->dc.left->bits)
            newQuad->dc.left = (IMODE*)node;
        else
            newQuad->livein |= IM_LIVELEFT;
    }
    else
    {
        /*		if (!ToQuadConst(&newQuad->dc.left))*/
        newQuad->livein |= IM_LIVELEFT;
    }
    node = LookupNVHash((UBYTE*)&newQuad->dc.right, sizeof(void*), name_hash);
    if (node)
    {
        if (node->dc.opcode == i_assn && node->dc.left->mode == i_immed)
        {
            newQuad->dc.right = node->dc.left;
            newQuad->livein |= IM_LIVERIGHT;
        }
        else if (node->ans->size == newQuad->dc.right->size && node->ans->bits == newQuad->dc.right->bits)
            newQuad->dc.right = (IMODE*)node;
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
    node = LookupNVHash((UBYTE*)newQuad, DAGCOMPARE, ins_hash);
    if (!node || (newQuad->dc.opcode == i_assn && node->ans->size != newQuad->ans->size) || node->ans->bits != newQuad->ans->bits)
    {
        if ((cparams.prm_optimize_for_speed || cparams.prm_optimize_for_size) && !(chosenAssembler->arch->denyopts & DO_NOLOCAL))
        {
            /* take care of volatiles by not registering volatile expressions
             * in the CSE table.  At this point a temp var will already exist
             * in the case that a volatile exists as the answer.
             */
            if (newQuad->ans && (newQuad->dc.opcode != i_assn || newQuad->dc.left->mode != i_immed) &&
                (!newQuad->ans->vol && !newQuad->ans->retval &&
                 (newQuad->ans->size < ISZ_FLOAT || chosenAssembler->arch->hasFloatRegs) &&
                 (!newQuad->dc.left || !(newQuad->livein & IM_LIVELEFT) ||
                  (!newQuad->dc.left->vol && (newQuad->dc.left->size < ISZ_FLOAT || chosenAssembler->arch->hasFloatRegs))) &&
                 (!newQuad->dc.right || !(newQuad->livein & IM_LIVERIGHT) ||
                  (!newQuad->dc.right->vol && (newQuad->dc.right->size < ISZ_FLOAT || chosenAssembler->arch->hasFloatRegs)))))
                if (newQuad->dc.opcode != i_add || (!(newQuad->livein & IM_LIVELEFT) || newQuad->dc.left->mode != i_immed) ||
                    (!(newQuad->livein & IM_LIVERIGHT) || newQuad->dc.right->mode != i_immed))
                {
                    if (newQuad->dc.opcode != i_parmstack)
                        if (newQuad->dc.opcode != i_assn || (!newQuad->genConflict && (!(newQuad->livein & IM_LIVELEFT) ||
                                                                                      newQuad->ans->size == newQuad->dc.left->size)))
                            ReplaceHash(newQuad, (UBYTE*)newQuad, DAGCOMPARE, ins_hash);
                }
        }
        /* convert back to a quad structure and generate code */
        node = newQuad;
        outnode = liveout(node);
        outnode->genConflict = newQuad->genConflict;
        add_intermed(outnode);
    }
    else
    {
        outnode = (QUAD*)(QUAD *)Alloc(sizeof(QUAD));
        outnode->dc.opcode = i_assn;
        outnode->ans = newQuad->ans;
        outnode->dc.left = node->ans;
        outnode->genConflict = newQuad->genConflict;
        if (outnode->ans != outnode->dc.left)
            add_intermed(outnode);
    }
    /* Save the newQuad node structure for later lookups
     * always save constants even when no LCSE is to be done because it
     * is needed for constant-folding in subsequent instructions
     */
    if ((cparams.prm_optimize_for_speed || cparams.prm_optimize_for_size) && !(chosenAssembler->arch->denyopts & DO_NOLOCAL))

    {
        if (newQuad->ans && (newQuad->ans->mode == i_ind || newQuad->ans->offset->type != en_tempref))
            flush_dag();
        else if (newQuad->ans /* &&  (newQuad->dc.opcode != i_assn || (newQuad->livein & IM_LIVELEFT)) */
                 && ((cparams.prm_optimize_for_speed || cparams.prm_optimize_for_size) &&
                     ((newQuad->ans->offset->type == en_tempref && newQuad->ans->mode == i_direct) || node->dc.opcode == i_icon ||
                      node->dc.opcode == i_fcon || node->dc.opcode == i_imcon || node->dc.opcode == i_cxcon)))
        {
            ReplaceHash(node, (UBYTE*)&newQuad->ans, sizeof(IMODE*), name_hash);
        }
    }
#else
    add_intermed(newQuad);
    outnode = newQuad;
#endif
    return outnode;
}

/*-------------------------------------------------------------------------*/
void flush_dag(void)
{
#ifdef DOING_LCSE
    memset(name_hash, 0, sizeof(void*) * DAGSIZE);
    memset(ins_hash, 0, sizeof(void*) * DAGSIZE);
#endif
}

/*-------------------------------------------------------------------------*/

void dag_rundown(void)
{
#ifdef DOING_LCSE
    memset(name_hash, 0, sizeof(void*) * DAGSIZE);
    memset(ins_hash, 0, sizeof(void*) * DAGSIZE);
#endif
}

/*-------------------------------------------------------------------------*/
BLOCKLIST* newBlock(void)
{
    BLOCK* block = (BLOCK *)Alloc(sizeof(BLOCK));
    BLOCKLIST* list = (BLOCKLIST*)Alloc(sizeof(BLOCKLIST));
    list->next = 0;
    list->block = block;
    block->blocknum = blockCount++;
    if (blockCount >= blockMax)
    {
        BLOCK** newBlocks = (BLOCK **) Alloc(sizeof(BLOCK*) * (blockMax + 1000));
        memcpy(newBlocks, blockArray, sizeof(BLOCK*) * blockMax);
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

    QUAD* q;
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
    q = (QUAD*)(QUAD *)Alloc(sizeof(QUAD));
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
    QUAD* newQuad;
    if (labno < 0)
        diag("gen_label: uncompensatedlabel");
    flush_dag();
    if (!wasgoto)
        addblock(i_label);
    wasgoto = false;
    newQuad = (QUAD*)(QUAD *)Alloc(sizeof(QUAD));
    newQuad->dc.opcode = i_label;
    newQuad->dc.v.label = labno;
    add_intermed(newQuad);
}
/*-------------------------------------------------------------------------*/

QUAD* gen_icode_with_conflict(enum i_ops op, IMODE* res, IMODE* left, IMODE* right, bool conflicting)
/*
 *      generate a code sequence into the peep list.
 */
{
    QUAD* newQuad;
    if (right && right->mode == i_immed /*&& right->size == ISZ_NONE*/)
    {
        IMODE* newRight = (IMODE *)Alloc(sizeof(IMODE));
        *newRight = *right;
        right = newRight;
        right->size = left->size;
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
    newQuad = (QUAD*)(QUAD *)Alloc(sizeof(QUAD));
    newQuad->genConflict = conflicting;
    newQuad->dc.opcode = op;
    newQuad->dc.left = left;
    newQuad->dc.right = right;
    newQuad->ans = res;
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
QUAD* gen_icode(enum i_ops op, IMODE* res, IMODE* left, IMODE* right)
{
    return gen_icode_with_conflict(op, res, left, right, false);
}

/*-------------------------------------------------------------------------*/

void gen_iiconst(IMODE* res, LLONG_TYPE val)
/*
 *      generate an integer constant sequence into the peep list.
 */
{
    QUAD* newQuad;
    IMODE* left = make_immed(ISZ_UINT, val);
    newQuad = (QUAD*)(QUAD *)Alloc(sizeof(QUAD));
    newQuad->dc.opcode = i_assn;
    newQuad->ans = res;
    newQuad->dc.left = left;
    add_dag(newQuad);
    wasgoto = false;
}

/*-------------------------------------------------------------------------*/

void gen_ifconst(IMODE* res, FPF val)
/*
 *      generate an integer constant sequence into the peep list.
 */
{
    QUAD* newQuad;
    newQuad = (QUAD*)(QUAD *)Alloc(sizeof(QUAD));
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
    QUAD* newQuad;
    flush_dag();
    newQuad = (QUAD*)(QUAD *)Alloc(sizeof(QUAD));
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
    QUAD* newQuad;
    flush_dag();
    newQuad = (QUAD*)(QUAD *)Alloc(sizeof(QUAD));
    newQuad->dc.opcode = i_genword;
    newQuad->dc.left = newQuad->dc.right = newQuad->ans = 0;
    newQuad->dc.v.label = val;
    add_intermed(newQuad);
    wasgoto = false;
}

/*-------------------------------------------------------------------------*/

void gen_icgoto(enum i_ops op, long label, IMODE* left, IMODE* right)
/*
 *      generate a code sequence into the peep list.
 */
{
    QUAD* newQuad;
    if (right && right->mode == i_immed /*&& right->size == ISZ_NONE*/)
        right->size = left->size;

    newQuad = (QUAD*)(QUAD *)Alloc(sizeof(QUAD));
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

QUAD* gen_igosub(enum i_ops op, IMODE* left)
/*
 *      generate a code sequence into the peep list.
 */
{
    QUAD* newQuad;

    newQuad = (QUAD*)(QUAD *)Alloc(sizeof(QUAD));
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

void gen_icode2(enum i_ops op, IMODE* res, IMODE* left, IMODE* right, int label)
/*
 *      generate a code sequence into the peep list.
 *		only being used for switches
 */
{
    QUAD* newQuad;
    newQuad = (QUAD*)(QUAD *)Alloc(sizeof(QUAD));
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
    QUAD* newQuad;
    if (data == 0)
        return;
    newQuad = (QUAD*)(QUAD *)Alloc(sizeof(QUAD));
    newQuad->dc.opcode = i_line;
    newQuad->dc.left = (IMODE*)data; /* text */
    add_intermed(newQuad);
}

/*-------------------------------------------------------------------------*/

void gen_asm(STATEMENT* stmt)
/*
 * generate an ASM statement
 */
{
    QUAD* newQuad;
    newQuad = (QUAD*)(QUAD *)Alloc(sizeof(QUAD));
    newQuad->dc.opcode = i_passthrough;
    newQuad->dc.left = (IMODE*)stmt->select; /* actually is defined by the INASM module*/
    if (chosenAssembler->gen->adjust_codelab)
        chosenAssembler->gen->adjust_codelab(newQuad->dc.left, codeLabelOffset);
    flush_dag();
    add_intermed(newQuad);
}
void gen_asmdata(STATEMENT* stmt)
{
    QUAD* newQuad;
    newQuad = (QUAD*)(QUAD *)Alloc(sizeof(QUAD));
    newQuad->dc.opcode = i_datapassthrough;
    newQuad->dc.left = (IMODE*)stmt->select; /* actually is defined by the INASM module*/
    flush_dag();
    add_intermed(newQuad);
}
/*-------------------------------------------------------------------------*/

void gen_nodag(enum i_ops op, IMODE* res, IMODE* left, IMODE* right)
/*
 *      generate a code sequence into the peep list.
 */
{
    QUAD* newQuad;
    newQuad = (QUAD*)(QUAD *)Alloc(sizeof(QUAD));
    newQuad->dc.opcode = op;
    newQuad->dc.left = left;
    newQuad->dc.right = right;
    newQuad->ans = res;
    add_intermed(newQuad);
    wasgoto = false;
}
void RemoveFromUses(QUAD* ins, int tnum)
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
void InsertUses(QUAD* ins, int tnum)
{
    INSTRUCTIONLIST* l = (INSTRUCTIONLIST *)oAlloc(sizeof(INSTRUCTIONLIST));
    l->next = tempInfo[tnum]->instructionUses;
    l->ins = ins;
    tempInfo[tnum]->instructionUses = l;
}
void RemoveInstruction(QUAD* ins)
{
    if (ins->dc.opcode == i_assn && ins->dc.left->retval)
    {
        // this is so we can inform the backend that the store is gone, e.g. so any fstp will not disappear entirely
        QUAD *find = ins->back;
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
        tempInfo[pd->T0]->instructionDefines = NULL;
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
            int tnum = ins->ans->offset->v.sp->value.i;
            if (ins->ans->mode == i_direct)
                tempInfo[tnum]->instructionDefines = NULL;
            else
            {
                if (ins->ans->offset)
                    RemoveFromUses(ins, tnum);
                if (ins->ans->offset2)
                    RemoveFromUses(ins, ins->ans->offset2->v.sp->value.i);
            }
        }
        if (ins->temps & TEMP_LEFT)
        {
            if (ins->dc.left->offset)
                RemoveFromUses(ins, ins->dc.left->offset->v.sp->value.i);
            if (ins->dc.left->offset2)
                RemoveFromUses(ins, ins->dc.left->offset2->v.sp->value.i);
        }
        if (ins->temps & TEMP_RIGHT)
        {
            if (ins->dc.right->offset)
                RemoveFromUses(ins, ins->dc.right->offset->v.sp->value.i);
            if (ins->dc.right->offset2)
                RemoveFromUses(ins, ins->dc.right->offset2->v.sp->value.i);
        }
    }
}
void InsertInstruction(QUAD* before, QUAD* ins)
{
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
        if (ins->ans && ((ins->ans->offset && ins->ans->offset->type == en_tempref) || ins->ans->offset2))
        {
            ins->temps |= TEMP_ANS;
            if (ins->ans->mode == i_direct)
            {
                tempInfo[ins->ans->offset->v.sp->value.i]->instructionDefines = ins;
            }
            else
            {
                if (ins->ans->offset)
                    InsertUses(ins, ins->ans->offset->v.sp->value.i);
                if (ins->ans->offset2)
                    InsertUses(ins, ins->ans->offset2->v.sp->value.i);
            }
        }
        if (ins->dc.left && ((ins->dc.left->offset && ins->dc.left->offset->type == en_tempref) || ins->dc.left->offset2))
        {
            if (!ins->dc.left->retval)
                ins->temps |= TEMP_LEFT;
            if (ins->dc.left->offset)
                InsertUses(ins, ins->dc.left->offset->v.sp->value.i);
            if (ins->dc.left->offset2)
                InsertUses(ins, ins->dc.left->offset2->v.sp->value.i);
        }
        if (ins->dc.right && ins->dc.right->offset && ins->dc.right->offset->type == en_tempref)
        {
            if (!ins->dc.right->retval)
                ins->temps |= TEMP_RIGHT;
            if (ins->dc.right->offset)
                InsertUses(ins, ins->dc.right->offset->v.sp->value.i);
            if (ins->dc.right->offset2)
                InsertUses(ins, ins->dc.right->offset2->v.sp->value.i);
        }
    }
}
