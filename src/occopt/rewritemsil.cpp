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

#include "ioptimizer.h"
#include "beinterfdefs.h"
#include "ildata.h"
#include "OptUtils.h"
#include "iblock.h"
#include "ilocal.h"
#include "memory.h"

// weed out structures with nested structures or bit fields
bool qualifiedStruct(SimpleSymbol* sp)
{
    //    SYMLIST *hr;
    if (!sp->tp->size)
        return false;
#if 0
    if (sp->tp->type == st_union)
        return false;
    hr = sp->tp->syms->table[0];
    while (hr)
    {
        SYMBOL *check = (SYMBOL *)hr->p;
        if (basetype(check->tp)->bits)
            return false;
        //        if (isstructured(check->tp))
        //            return false;
//        if (basetype(check->tp)->array)
//            return false;
        hr = hr->next;
    }
#endif
    return true;
}

QUAD* leftInsertionPos(QUAD* head, IMODE* im)
{
    QUAD* rv = head;
    head = head->back;
    while (head && head->dc.opcode != i_block)
    {
        if (head->temps & TEMP_ANS)
        {
            if (im->offset->sp->i == head->ans->offset->sp->i)
            {
                rv = head;
                if (!(head->temps & TEMP_LEFT))
                    break;
                im = head->dc.left;
                if (im->offset->sp->pushedtotemp)
                    break;
            }
        }
        head = head->back;
    }
    return rv;
}

int msil_examine_icode(QUAD* head)
{
    int parmIndex = 0;
    IMODE* fillinvararg = NULL;
    while (head)
    {
        if (head->dc.opcode == i_gosub)
        {
            if (head->altvararg)
            {
                if (fillinvararg)
                    fillinvararg->offset->i = parmIndex;
                if (!parmIndex)
                    head->nullvararg = true;
                fillinvararg = NULL;
                parmIndex = 0;
            }
        }
        if (head->dc.opcode != i_block && head->dc.opcode != i_blockend && head->dc.opcode != i_dbgblock &&
            head->dc.opcode != i_dbgblockend && head->dc.opcode != i_var && head->dc.opcode != i_label &&
            head->dc.opcode != i_line && head->dc.opcode != i_passthrough && head->dc.opcode != i_func &&
            head->dc.opcode != i_gosub && head->dc.opcode != i_parmadj && head->dc.opcode != i_ret &&
            head->dc.opcode != i_varstart && head->dc.opcode != i_parmblock && head->dc.opcode != i_coswitch &&
            head->dc.opcode != i_swbranch && head->dc.opcode != i_expressiontag)
        {
            if (head->dc.opcode == i_muluh || head->dc.opcode == i_mulsh)
            {
                int sz = head->dc.opcode == i_muluh ? ISZ_ULONGLONG : -ISZ_ULONGLONG;
                IMODE* ap = InitTempOpt(sz, sz);
                QUAD* q = (QUAD*)Alloc(sizeof(QUAD));
                q->dc.opcode = i_assn;
                q->ans = ap;
                q->temps = TEMP_ANS;
                q->dc.left = head->dc.left;
                head->dc.left = ap;
                head->temps |= TEMP_LEFT;
                InsertInstruction(head->back, q);
                head->dc.right->size = sz;
            }
            if (head->dc.left && head->dc.left->mode == i_immed && head->dc.opcode != i_assn)
            {
                IMODE* ap = InitTempOpt(head->dc.left->size, head->dc.left->size);
                QUAD *q = (QUAD*)Alloc(sizeof(QUAD)), *t;
                q->dc.opcode = i_assn;
                q->ans = ap;
                q->temps = TEMP_ANS;
                q->dc.left = head->dc.left;
                head->dc.left = ap;
                head->temps |= TEMP_LEFT;
                t = head;
                if (head->temps & TEMP_RIGHT)
                    t = leftInsertionPos(head, head->dc.right);
                InsertInstruction(t->back, q);
            }
            /*
            if (head->dc.opcode == i_clrblock)
            {
                // insert the value to clear it to, e.g. zero
                IMODE *ap = InitTempOpt(head->dc.right->size, head->dc.right->size);
                QUAD *q = (QUAD *)Alloc(sizeof(QUAD));
                q->alwayslive = true;
                q->dc.opcode = i_assn;
                q->ans = ap;
                q->temps = TEMP_ANS;
                q->dc.left = (IMODE *)Alloc(sizeof(IMODE));
                q->dc.left->mode = i_immed;
                q->dc.left->offset = simpleIntNode(se_i, 0);
                InsertInstruction(head->back, q);
            }
            */
            if (head->dc.right && head->dc.right->offset->type == se_structelem)
            {
                // by definition this is an add node...
                if (!qualifiedStruct(head->dc.right->offset->sp->parentClass))
                {
                    head->dc.right->mode = i_immed;
                    head->dc.right->offset = simpleIntNode(se_i, head->dc.right->offset->sp->offset);
                }
            }
            if (head->dc.right && head->dc.right->mode == i_immed)
            {
                if ((head->dc.opcode != i_je && head->dc.opcode != i_jne) || !((head->dc.right->offset->type == se_i || head->dc.right->offset->type == se_ui) && head->dc.right->offset->i == 0))
                {
                    if (head->dc.right->offset->type != se_structelem)
                    {
                        IMODE* ap = InitTempOpt(head->dc.right->size, head->dc.right->size);
                        QUAD* q = (QUAD*)Alloc(sizeof(QUAD));
                        q->dc.opcode = i_assn;
                        q->ans = ap;
                        q->temps = TEMP_ANS;
                        q->dc.left = head->dc.right;
                        head->dc.right = ap;
                        head->temps |= TEMP_RIGHT;
                        InsertInstruction(head->back, q);
                    }
                }
            }
            if (head->vararg)
            {
                // handle varargs... this won't work in the case of nested vararg funcs
                QUAD* q = (QUAD*)Alloc(sizeof(QUAD));
                QUAD* q1 = (QUAD*)Alloc(sizeof(QUAD));
                IMODE* ap = InitTempOpt(ISZ_ADDR, ISZ_ADDR);
                IMODE* ap1 = (IMODE*)Alloc(sizeof(IMODE));
                IMODE* ap2 = InitTempOpt(-ISZ_UINT, -ISZ_UINT);
                IMODE* ap3 = make_immed(-ISZ_UINT, parmIndex++);
                QUAD* prev = head;
                ap1->offset = objectArray_exp;
                ap1->mode = i_direct;
                ap1->size = ISZ_ADDR;
                while (prev->back && !prev->back->varargPrev)
                    prev = prev->back;
                if (parmIndex - 1 == 0)
                {
                    // this is for the initialization of the object array
                    QUAD* q2 = (QUAD*)Alloc(sizeof(QUAD));
                    QUAD* q3 = (QUAD*)Alloc(sizeof(QUAD));
                    IMODE* ap4 = InitTempOpt(ISZ_ADDR, ISZ_ADDR);
                    IMODE* ap5 = (IMODE*)Alloc(sizeof(IMODE));
                    IMODE* ap6 = (IMODE*)Alloc(sizeof(IMODE));
                    ap6->offset = objectArray_exp;
                    ap6->mode = i_direct;
                    ap6->size = ISZ_ADDR;
                    ap5->mode = i_immed;
                    ap5->size = ISZ_UINT;
                    ap5->offset = simpleIntNode(se_i, 0);
                    fillinvararg = ap5;
                    q2->dc.opcode = i_assn;
                    q2->ans = ap4;
                    q2->dc.left = ap5;
                    q2->temps = TEMP_ANS;
                    q3->dc.opcode = i_assn;
                    q3->ans = ap6;
                    q3->dc.left = ap4;
                    q3->temps = TEMP_LEFT;
                    InsertInstruction(prev->back, q2);
                    InsertInstruction(prev->back, q3);
                }
                // this is to load this param into the object array
                // it inserts the params need by the stelem.ref
                // the stelem.ref and any boxing are done later...
                q->dc.opcode = i_assn;
                q->ans = ap;
                q->dc.left = ap1;
                q->temps = TEMP_ANS;
                q->alwayslive = true;
                q1->dc.opcode = i_assn;
                q1->ans = ap2;
                q1->dc.left = ap3;
                q1->temps = TEMP_ANS;
                q1->alwayslive = true;
                InsertInstruction(prev->back, q);
                InsertInstruction(prev->back, q1);
            }
        }
        head = head->fwd;
    }
    return 0;
}
