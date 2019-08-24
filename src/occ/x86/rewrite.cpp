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

#include "be.h"
extern ARCH_ASM* chosenAssembler;
extern int tempCount;
extern TEMP_INFO** tempInfo;
extern BLOCKLIST** blockArray;
extern int prm_useesp;

extern int prm_lscrtdll;

EXPRESSION* fltexp;

int uses_substack;

static IMODE* AllocateTemp(int size) { return InitTempOpt(size, size); }
void insert_parm(QUAD* head, QUAD* q)
{
    if (q->dc.left->mode == i_ind)
    {
        IMODE* temp;
        QUAD* move;
        temp = AllocateTemp(q->dc.left->size);
        //					tempInfo[tempCount-1] = temp->offset;
        move = (QUAD*)beLocalAlloc(sizeof(QUAD));
        move->dc.opcode = i_assn;
        move->ans = temp;
        move->dc.left = q->dc.left;
        q->dc.left = temp;
        InsertInstruction(head, move);
        InsertInstruction(move, q);
    }
    else
    {
        InsertInstruction(head, q);
    }
}
void insert_nullparmadj(QUAD* head, int v)
{
    QUAD* q = (QUAD*)beLocalAlloc(sizeof(QUAD));
    q->dc.opcode = i_parmadj;
    q->dc.left = make_parmadj(0);
    q->dc.right = make_parmadj(v);
    InsertInstruction(head, q);
}
static void rvColor(IMODE* ip)
{
    int n = ip->offset->v.sp->value.i;
    tempInfo[n]->precolored = true;
    if (ip->size >= ISZ_CFLOAT)
    {
        tempInfo[n]->color = ip->size == ISZ_CFLOAT ? 40 : 56;
    }
    else if (ip->size >= ISZ_FLOAT)
    {
        tempInfo[n]->color = ip->size == ISZ_FLOAT || ip->size == ISZ_IFLOAT ? 32 : 48;
    }
    else if (ip->size == ISZ_ULONGLONG || ip->size == -ISZ_ULONGLONG)
    {
        tempInfo[n]->color = R_EAXEDX;
    }
    else if (ip->size == ISZ_USHORT || ip->size == -ISZ_USHORT || ip->size == ISZ_U16)
        tempInfo[n]->color = R_AX;
    else if (ip->size == ISZ_UCHAR || ip->size == -ISZ_UCHAR || ip->size == ISZ_BOOLEAN)
        tempInfo[n]->color = R_AL;
    else
        tempInfo[n]->color = R_EAX;
    tempInfo[n]->enode->v.sp->regmode = 2;
    tempInfo[n]->precolored = true;
}
void precolor(QUAD* head) /* precolor an instruction */
{
    if (head->fastcall && head->dc.opcode != i_gosub)
    {
        int ta;
        if (head->fastcall < 0)
        {
            // callee
            ta = head->dc.left->offset->v.sp->value.i;
            head->precolored |= TEMP_LEFT;
            tempInfo[ta]->precolored = true;
            tempInfo[ta]->enode->v.sp->regmode = 2;
            if (tempInfo[ta]->size == ISZ_ULONGLONG || tempInfo[ta]->size == -ISZ_ULONGLONG)
                tempInfo[ta]->color = 10;  // ASSUME ECX::EDX
            else
                tempInfo[ta]->color =
                    chosenAssembler->arch->fastcallRegs[(head->fastcall > 0 ? head->fastcall : -head->fastcall) - 1];
        }
        else
        {
            // call site
            if (head->dc.left->mode != i_direct || head->dc.left->offset->type != en_tempref)
            {
                IMODE* temp = InitTempOpt(head->dc.left->size, head->dc.left->size);
                QUAD* q = (QUAD*)(QUAD*)Alloc(sizeof(QUAD));
                *q = *head;
                q->dc.opcode = i_assn;
                q->ans = temp;
                head->dc.left = temp;
                InsertInstruction(head->back, q);
            }
            if (head->dc.left->offset && head->dc.left->offset->type == en_tempref)
            {
                ta = head->dc.left->offset->v.sp->value.i;
                head->precolored |= TEMP_LEFT;

                //            ta = head->ans->offset->v.sp->value.i;
                //            head->precolored |= TEMP_ANS;
                tempInfo[ta]->precolored = true;
                tempInfo[ta]->enode->v.sp->regmode = 2;
                if (tempInfo[ta]->size == ISZ_ULONGLONG || tempInfo[ta]->size == -ISZ_ULONGLONG)
                    tempInfo[ta]->color = 10;  // ASSUME ECX::EDX
                else
                    tempInfo[ta]->color =
                        chosenAssembler->arch->fastcallRegs[(head->fastcall > 0 ? head->fastcall : -head->fastcall) - 1];
            }
        }
    }
    else if (head->dc.opcode == i_sdiv || head->dc.opcode == i_udiv)
    {
        if (head->temps & TEMP_ANS)
        {
            int ta = head->ans->offset->v.sp->value.i;
            if (tempInfo[ta]->size < ISZ_FLOAT)
            {
                tempInfo[ta]->precolored = true;
                tempInfo[ta]->enode->v.sp->regmode = 2;
                tempInfo[ta]->color = R_EAX;
                head->precolored |= TEMP_ANS;
            }
        }
        if ((head->temps & TEMP_LEFT) && head->dc.left->mode == i_direct)
        {
            int tl = head->dc.left->offset->v.sp->value.i;
            if (tempInfo[tl]->size < ISZ_FLOAT)
            {
                tempInfo[tl]->precolored = true;
                tempInfo[tl]->enode->v.sp->regmode = 2;
                tempInfo[tl]->color = R_EAXEDX;
                head->precolored |= TEMP_LEFT;
            }
        }
    }
    else if (head->dc.opcode == i_smod || head->dc.opcode == i_umod)
    {
        if (head->temps & TEMP_ANS)
        {
            int ta = head->ans->offset->v.sp->value.i;
            if (tempInfo[ta]->size < ISZ_FLOAT)
            {
                tempInfo[ta]->precolored = true;
                tempInfo[ta]->enode->v.sp->regmode = 2;
                tempInfo[ta]->color = R_EDX;
                head->precolored |= TEMP_ANS;
            }
        }
        if ((head->temps & TEMP_LEFT) && head->dc.left->mode == i_direct)
        {
            int tl = head->dc.left->offset->v.sp->value.i;
            if (tempInfo[tl]->size < ISZ_FLOAT)
            {
                tempInfo[tl]->precolored = true;
                tempInfo[tl]->enode->v.sp->regmode = 2;
                tempInfo[tl]->color = R_EAXEDX;
                head->precolored |= TEMP_LEFT;
            }
        }
    }
    else if (head->dc.opcode == i_muluh || head->dc.opcode == i_mulsh)
    {
        if (head->temps & TEMP_ANS)
        {
            int ta = head->ans->offset->v.sp->value.i;
            if (tempInfo[ta]->size < ISZ_FLOAT)
            {
                tempInfo[ta]->precolored = true;
                tempInfo[ta]->enode->v.sp->regmode = 2;
                tempInfo[ta]->color = R_EDX;
                head->precolored |= TEMP_ANS;
            }
        }
        if ((head->temps & TEMP_LEFT) && head->dc.left->mode == i_direct)
        {
            int tl = head->dc.left->offset->v.sp->value.i;
            if (tempInfo[tl]->size < ISZ_FLOAT)
            {
                tempInfo[tl]->precolored = true;
                tempInfo[tl]->enode->v.sp->regmode = 2;
                tempInfo[tl]->color = R_EAX;
                head->precolored |= TEMP_LEFT;
            }
        }
    }
    else if (head->dc.opcode == i_lsr || head->dc.opcode == i_asr || head->dc.opcode == i_lsl)
    {
        if (head->temps & TEMP_RIGHT)
        {
            int tr = head->dc.right->offset->v.sp->value.i;
            tempInfo[tr]->precolored = true;
            head->precolored |= TEMP_RIGHT;
            switch (tempInfo[tr]->size)
            {
                case ISZ_BOOLEAN:
                case ISZ_UCHAR:
                case -ISZ_UCHAR:
                    tempInfo[tr]->color = R_CL;
                    break;
                case ISZ_U16:
                case ISZ_USHORT:
                case -ISZ_USHORT:
                    tempInfo[tr]->color = R_CX;
                    break;
                case ISZ_ULONGLONG:
                case -ISZ_ULONGLONG:
                    tempInfo[tr]->color = R_EDXECX;
                    break;
                default:
                    tempInfo[tr]->color = R_ECX;
                    break;
            }
            tempInfo[tr]->enode->v.sp->regmode = 2;
        }
    }
    if (head->ans && head->ans->retval)
        rvColor(head->ans);
    if (head->dc.left && head->dc.left->retval)
        rvColor(head->dc.left);
    if (head->dc.right && head->dc.right->retval)
        rvColor(head->dc.right);
}
static bool hasbp(EXPRESSION* expr)
{
    if (!expr)
        return false;
    if (expr->type == en_add || expr->type == en_structadd)
        return (hasbp(expr->left) || hasbp(expr->right));
    return expr->type == en_auto;
}
static int mult(EXPRESSION* match, int mpy, int total)
{
    if (isintconst(match))
    {
        total = total + mpy * match->v.i;
    }
    else if (match->type == en_add)
    {
        total = mult(match->left, mpy, total);
        total = mult(match->right, mpy, total);
    }
    else
        diag("mult: invalid node type");
    return total;
}
void ProcessOneInd(EXPRESSION* match, EXPRESSION** ofs1, EXPRESSION** ofs2, EXPRESSION** ofs3, int* scale)
{
    if (match && match->type == en_tempref)
    {
        int n = match->v.sp->value.i;
        QUAD* ins = tempInfo[n]->instructionDefines;
        if (ins)
        {
            switch (ins->dc.opcode)
            {
                case i_add:
                    if (!tempInfo[ins->ans->offset->v.sp->value.i]->inductionLoop)
                    {
                        if (*ofs1 && ins->ans->offset->v.sp->value.i == (*ofs1)->v.sp->value.i)
                        {
                            EXPRESSION *offset1, *offset2 = *ofs2, *offset3 = nullptr;
                            if (ins->temps & TEMP_LEFT)
                            {
                                offset1 = ins->dc.left->offset;
                                if (ins->temps & TEMP_RIGHT)
                                {
                                    if (offset2)
                                        break;
                                    offset2 = ins->dc.right->offset;
                                }
                                else if (ins->dc.right->mode == i_immed)
                                {
                                    offset3 = ins->dc.right->offset;
                                }
                                else
                                    break;
                            }
                            else if (ins->temps & TEMP_RIGHT)
                            {
                                offset1 = ins->dc.right->offset;
                                if (ins->dc.left->mode == i_immed)
                                {
                                    offset3 = ins->dc.left->offset;
                                }
                                else
                                    break;
                            }
                            else if (ins->dc.left->mode == i_immed && ins->dc.right->mode == i_immed)
                            {
                                offset1 = nullptr;
                                offset3 = exprNode(en_add, ins->dc.left->offset, ins->dc.right->offset);
                            }
                            else
                                break;
                            if ((hasbp(offset3) || hasbp(*ofs3)) && offset1 && offset2)
                                break;
                            *ofs1 = offset1;
                            *ofs2 = offset2;
                            if (offset3)
                            {
                                if (*ofs3)
                                    *ofs3 = exprNode(en_add, *ofs3, offset3);
                                else
                                    *ofs3 = offset3;
                            }
                        }
                        else if (*ofs2 && ins->ans->offset->v.sp->value.i == (*ofs2)->v.sp->value.i)
                        {
                            EXPRESSION *offset1 = *ofs1, *offset2, *offset3 = nullptr;
                            if (ins->temps & TEMP_LEFT)
                            {
                                offset2 = ins->dc.left->offset;
                                if (ins->temps & TEMP_RIGHT)
                                {
                                    if (offset1 || *scale)
                                        break;
                                    offset1 = ins->dc.right->offset;
                                }
                                else if (ins->dc.right->mode == i_immed)
                                {
                                    offset3 = ins->dc.right->offset;
                                }
                                else
                                    break;
                            }
                            else if (ins->temps & TEMP_RIGHT)
                            {
                                offset2 = ins->dc.right->offset;
                                if (ins->dc.left->mode == i_immed)
                                {
                                    offset3 = ins->dc.left->offset;
                                }
                                else
                                    break;
                            }
                            else if (ins->dc.left->mode == i_immed && ins->dc.right->mode == i_immed)
                            {
                                if (*scale && (ins->dc.left->size == ISZ_ADDR || ins->dc.right->size == ISZ_ADDR))
                                    break;
                                offset2 = nullptr;
                                offset3 = exprNode(en_add, ins->dc.left->offset, ins->dc.right->offset);
                            }
                            else
                                break;
                            if ((hasbp(offset3) || hasbp(*ofs3)) && offset1 && offset2)
                                break;
                            *ofs1 = offset1;
                            *ofs2 = offset2;
                            if (offset3 && *scale)
                                offset3 = intNode(offset3->type, offset3->v.i << *scale);
                            if (offset3)
                            {
                                if (*ofs3)
                                    *ofs3 = exprNode(en_add, *ofs3, offset3);
                                else
                                    *ofs3 = offset3;
                            }
                        }
                    }
                    break;
                case i_sub:
                    if (!tempInfo[ins->ans->offset->v.sp->value.i]->inductionLoop)
                    {
                        if (*ofs1 && ins->ans->offset->v.sp->value.i == (*ofs1)->v.sp->value.i)
                        {
                            if (ins->temps & TEMP_LEFT)
                            {
                                EXPRESSION *offset1, *offset3;
                                offset1 = ins->dc.left->offset;
                                if (ins->dc.right->mode != i_immed || !isintconst(ins->dc.right->offset))
                                    break;
                                offset3 = ins->dc.right->offset;
                                *ofs1 = offset1;
                                if (*ofs3)
                                    *ofs3 = exprNode(en_sub, *ofs3, offset3);
                                else
                                    *ofs3 = intNode(offset3->type, -offset3->v.i);
                            }
                        }
                        else if (*ofs2 && ins->ans->offset->v.sp->value.i == (*ofs2)->v.sp->value.i)
                        {
                            if (ins->temps & TEMP_LEFT)
                            {
                                EXPRESSION *offset2, *offset3;
                                offset2 = ins->dc.left->offset;
                                if (ins->dc.right->mode != i_immed || !isintconst(ins->dc.right->offset))
                                    break;
                                offset3 = ins->dc.right->offset;
                                *ofs2 = offset2;
                                if (offset3 && *scale)
                                    offset3 = intNode(offset3->type, offset3->v.i << *scale);
                                if (*ofs3)
                                    *ofs3 = exprNode(en_sub, *ofs3, offset3);
                                else
                                    *ofs3 = intNode(offset3->type, -offset3->v.i);
                            }
                        }
                    }
                    break;
                case i_lsl:
                    if (!*scale && (ins->temps & TEMP_LEFT))
                    {
                        if (ins->dc.right->mode == i_immed && isintconst(ins->dc.right->offset))
                        {
                            EXPRESSION *offset1 = *ofs1, *offset2 = *ofs2;
                            int scl = ins->dc.right->offset->v.i;
                            if (scl >= 4)
                                break;
                            if (*ofs1 && ins->ans->offset->v.sp->value.i == (*ofs1)->v.sp->value.i)
                            {
                                offset1 = offset2;
                                offset2 = ins->dc.left->offset;
                            }
                            else if (*ofs2 && ins->ans->offset->v.sp->value.i == (*ofs2)->v.sp->value.i)
                            {
                                offset2 = ins->dc.left->offset;
                            }
                            else
                                break;
                            *ofs1 = offset1;
                            *ofs2 = offset2;
                            *scale = scl;
                        }
                    }
                    break;
                case i_assn:
                    if ((ins->temps & (TEMP_LEFT | TEMP_ANS)) == (TEMP_LEFT | TEMP_ANS))
                    {
                        if (ins->ans->mode == i_direct && ins->dc.left->mode == i_direct)
                            if (ins->ans->size == ins->dc.left->size || ins->ans->size == -ins->dc.left->size)
                                if (ins->ans->size >= ISZ_UINT || ins->ans->size <= -ISZ_UINT)
                                    if (!ins->dc.left->retval && !ins->ans->offset->v.sp->pushedtotemp)
                                    {
                                        QUAD* insz = tempInfo[ins->dc.left->offset->v.sp->value.i]->instructionDefines;
                                        if ((insz && insz->dc.opcode != i_assn) ||
                                            ins->block == tempInfo[n]->instructionUses->ins->block)
                                        {
                                            if (*ofs1 && (*ofs1)->v.sp->value.i == ins->ans->offset->v.sp->value.i)
                                                *ofs1 = ins->dc.left->offset;
                                            else if (*ofs2 && (*ofs2)->v.sp->value.i == ins->ans->offset->v.sp->value.i)
                                                *ofs2 = ins->dc.left->offset;
                                        }
                                    }
                    }
                    else if ((ins->temps & (TEMP_LEFT | TEMP_ANS)) == TEMP_ANS && ins->ans->mode == i_direct &&
                             ins->dc.left->mode == i_immed)
                    {
                        EXPRESSION* xofs = ins->dc.left->offset;
                        if (*ofs1 && (*ofs1)->v.sp->value.i == ins->ans->offset->v.sp->value.i)
                            *ofs1 = nullptr;
                        else if (*ofs2 && (*ofs2)->v.sp->value.i == ins->ans->offset->v.sp->value.i)
                        {
                            if (*scale)
                                xofs = intNode(xofs->type, mult(xofs, 1 << *scale, 0));
                            *ofs2 = nullptr;
                            scale = 0;
                        }
                        if (*ofs3)
                        {
                            *ofs3 = exprNode(en_add, *ofs3, xofs);
                        }
                        else
                        {
                            *ofs3 = xofs;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
}
void ProcessInd(EXPRESSION** ofs1, EXPRESSION** ofs2, EXPRESSION** ofs3, int* scale)
{
    EXPRESSION *ofs4, *ofs5;
    do
    {
        ofs4 = *ofs1, ofs5 = *ofs2;
        ProcessOneInd(*ofs1, ofs1, ofs2, ofs3, scale);
        ProcessOneInd(*ofs2, ofs1, ofs2, ofs3, scale);
    } while ((ofs4 != *ofs1 || ofs5 != *ofs2));
}
// relies on constant offsets being calculated the same way every time
static bool MatchesConst(EXPRESSION* one, EXPRESSION* two)
{
    if (one == two)
        return true;
    if (!one || !two)
        return false;
    if (one->type != two->type)
        return false;
    if (!MatchesConst(one->left, two->left) || !MatchesConst(one->right, two->right))
        return false;
    if (isintconst(one))
        return one->v.i == two->v.i;
    switch (one->type)
    {
        case en_global:
        case en_auto:
        case en_pc:
        case en_labcon:
        case en_threadlocal:
            return one->v.sp == two->v.sp;
        default:
            return false;
    }
}
static bool MatchesMem(IMODE* one, IMODE* two)
{
    if (one == two)
        return true;
    if (one->mode == i_ind && two->mode == i_ind)
    {
        if ((one->offset && !two->offset) || (one->offset2 && !two->offset2) || (one->offset3 && !two->offset3))
            return false;
        if ((!one->offset && two->offset) || (!one->offset2 && two->offset2) || (!one->offset3 && two->offset3))
            return false;
        if (one->offset)
            if (one->offset->v.sp->value.i != two->offset->v.sp->value.i)
                return false;
        if (one->offset2)
        {
            if (one->offset2->v.sp->value.i != two->offset2->v.sp->value.i)
                return false;
            if (one->scale != two->scale)
                return false;
        }
        if (one->offset3)
            if (!MatchesConst(one->offset3, two->offset3))
                return false;
        return true;
    }
    return false;
}
static bool twomem(IMODE* left, IMODE* right)
{
    if (left->mode == i_ind || (left->mode == i_direct && left->offset->type != en_tempref))
        if (right->mode == i_ind || (right->mode == i_direct && right->offset->type != en_tempref))
            return true;
    return false;
}
static void HandleAssn(QUAD* ins, BRIGGS_SET* globalVars)
{
    if ((ins->temps & (TEMP_LEFT | TEMP_RIGHT)) && ins->ans->size < ISZ_ULONGLONG && ins->ans->size > -ISZ_ULONGLONG)
    {
        if (ins->ans->mode == i_ind || !(ins->temps & TEMP_ANS))
        {
            if (!briggsTest(globalVars, ins->dc.left->offset->v.sp->value.i))
            {
                if (ins->back->ans == ins->dc.left)
                {
                    switch (ins->back->dc.opcode)
                    {
                        case i_neg:
                        case i_not:
                        case i_sub:
                        case i_lsl:
                        case i_lsr:
                        case i_asr:
                            if ((ins->back->dc.right && ins->back->dc.right->mode == i_immed) ||
                                ((ins->back->temps & TEMP_RIGHT) && ins->back->dc.right->mode == i_direct))
                                if ((ins->back->temps & TEMP_LEFT) && ins->back->dc.left->mode == i_direct)
                                    if (ins->back->back->dc.opcode == i_assn)
                                        if (ins->back->dc.left == ins->back->back->ans)
                                            if (MatchesMem(ins->ans, ins->back->back->dc.left))
                                            {
                                                if (!briggsTest(globalVars, ins->back->ans->offset->v.sp->value.i) &&
                                                    !briggsTest(globalVars, ins->back->back->ans->offset->v.sp->value.i))
                                                {
                                                    if (!twomem(ins->ans, ins->back->dc.right))
                                                    {
                                                        ins->dc.left = ins->ans;
                                                        ins->dc.right = ins->back->dc.right;
                                                        if (ins->dc.left->offset)
                                                            if (ins->dc.left->offset->type == en_tempref)
                                                                ins->temps = TEMP_LEFT | TEMP_ANS;
                                                            else
                                                                ins->temps = 0;
                                                        else
                                                            ins->temps = TEMP_LEFT | TEMP_ANS;
                                                        if (ins->dc.right && ins->dc.right->offset &&
                                                            ins->dc.right->offset->type == en_tempref)
                                                            ins->temps |= TEMP_RIGHT;
                                                        ins->dc.opcode = ins->back->dc.opcode;
                                                    }
                                                }
                                            }
                            break;
                        case i_add:
                        case i_or:
                        case i_eor:
                        case i_and:
                            if ((ins->back->dc.right && ins->back->dc.right->mode == i_immed) ||
                                ((ins->back->temps & TEMP_RIGHT) && ins->back->dc.right->mode == i_direct))
                                if ((ins->back->temps & TEMP_LEFT) && ins->back->dc.left->mode == i_direct)
                                    if (ins->back->back->dc.opcode == i_assn)
                                        if (ins->back->dc.left == ins->back->back->ans)
                                            if (MatchesMem(ins->ans, ins->back->back->dc.left))
                                            {
                                                if (!briggsTest(globalVars, ins->back->ans->offset->v.sp->value.i) &&
                                                    !briggsTest(globalVars, ins->back->back->ans->offset->v.sp->value.i))
                                                {
                                                    if (!twomem(ins->ans, ins->back->dc.right))
                                                    {
                                                        ins->dc.left = ins->ans;
                                                        ins->dc.right = ins->back->dc.right;
                                                        if (ins->dc.left->offset)
                                                            if (ins->dc.left->offset->type == en_tempref)
                                                                ins->temps = TEMP_LEFT | TEMP_ANS;
                                                            else
                                                                ins->temps = 0;
                                                        else
                                                            ins->temps = TEMP_LEFT | TEMP_ANS;
                                                        if (ins->dc.right && ins->dc.right->offset &&
                                                            ins->dc.right->offset->type == en_tempref)
                                                            ins->temps |= TEMP_RIGHT;
                                                        ins->dc.opcode = ins->back->dc.opcode;
                                                        break;
                                                    }
                                                }
                                            }
                            if ((ins->back->dc.left && ins->back->dc.left->mode == i_immed) ||
                                ((ins->temps & TEMP_LEFT) && ins->back->dc.left->mode == i_direct))
                                if ((ins->back->temps & TEMP_RIGHT) && ins->back->dc.right->mode == i_direct)
                                    if (ins->back->back->dc.opcode == i_assn)
                                        if (ins->back->dc.right == ins->back->back->ans)
                                            if (MatchesMem(ins->ans, ins->back->back->dc.left))
                                            {
                                                if (!briggsTest(globalVars, ins->back->ans->offset->v.sp->value.i) &&
                                                    !briggsTest(globalVars, ins->back->back->ans->offset->v.sp->value.i))
                                                {
                                                    if (!twomem(ins->ans, ins->back->dc.left))
                                                    {
                                                        ins->dc.right = ins->back->dc.left;
                                                        ins->dc.left = ins->ans;
                                                        if (ins->dc.left->offset)
                                                            if (ins->dc.left->offset->type == en_tempref)
                                                                ins->temps = TEMP_LEFT | TEMP_ANS;
                                                            else
                                                                ins->temps = 0;
                                                        else
                                                            ins->temps = TEMP_LEFT | TEMP_ANS;
                                                        if (ins->dc.right && ins->dc.right->offset &&
                                                            ins->dc.right->offset->type == en_tempref)
                                                            ins->temps |= TEMP_RIGHT;
                                                        ins->dc.opcode = ins->back->dc.opcode;
                                                        break;
                                                    }
                                                }
                                            }

                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }
    /*
    // this to use xors instead of loading zeros...
    else if (ins->dc.left->mode == i_immed && ((!ins->temps & TEMP_ANS) || (ins->ans->mode == i_ind)))
    {
        if (isintconst(ins->dc.left->offset) && ins->dc.left->offset->v.i== 0)
        {
            if (ins->ans->size >= ISZ_UINT || ins->ans->size <= -ISZ_UINT)
            {
                IMODE *imn = InitTempOpt(ins->ans->size, ins->ans->size);
                QUAD *newIns = (QUAD *)Alloc(sizeof(QUAD));
                newIns->ans = imn;
                newIns->dc.left = ins->dc.left;
                newIns->dc.opcode = i_assn;
                newIns->temps = TEMP_ANS;
                ins->dc.left = imn;
                ins->temps |= TEMP_LEFT;
                InsertInstruction(ins->back, newIns);
            }
        }
    }
    */
}
int preRegAlloc(QUAD* ins, BRIGGS_SET* globalVars, BRIGGS_SET* eobGlobals, int pass)
{
    IMODE* ind = nullptr;
    if (pass == 1)
    {
        switch (ins->dc.opcode)
        {
            case i_je:
            case i_jne:
            case i_jl:
            case i_jc:
            case i_jg:
            case i_ja:
            case i_jle:
            case i_jbe:
            case i_jge:
            case i_jnc:
            case i_sete:
            case i_setne:
            case i_setl:
            case i_setc:
            case i_setg:
            case i_seta:
            case i_setle:
            case i_setbe:
            case i_setge:
            case i_setnc:
                // if right is an immed, check for a one-bit bitfield in followback
                // of moves of left... if one is found propagate it forward...
                if (ins->dc.right->mode == i_immed && ins->dc.left->mode == i_direct && (ins->temps & TEMP_LEFT))
                {
                    if (isintconst(ins->dc.right->offset))
                    {
                        IMODE* im = ins->dc.left;
                        QUAD* t = ins->back;
                        while (t->dc.opcode == i_assn && t->ans == im)
                        {
                            if (t->dc.left->bits)
                            {
                                if (t->dc.left->bits == 1 && (ins->dc.opcode == i_sete || ins->dc.opcode == i_setne ||
                                                              ins->dc.opcode == i_je || ins->dc.opcode == i_jne))
                                {
                                    ins->dc.left = t->dc.left;
                                    ins->temps &= ~TEMP_LEFT;
                                    ins->temps |= t->temps & TEMP_LEFT;
                                }
                                break;
                            }
                            else if (t->dc.left->mode != i_direct || !(t->temps & TEMP_LEFT))
                                break;
                            im = t->dc.left;
                            t = t->back;
                        }
                    }
                }
            default:
                break;
        }
        if (ins->ans && ins->ans->mode == i_ind)
        {
            EXPRESSION *offset1 = ins->ans->offset, *offset2 = nullptr, *offset3 = nullptr;
            int scale = 0;
            ProcessInd(&offset1, &offset2, &offset3, &scale);
            if (offset2 || offset3)
            {
                IMODE* ind1 = (IMODE*)Alloc(sizeof(IMODE));
                *ind1 = *ins->ans;
                if ((offset1 && offset1->type == en_tempref) || (offset2 && offset2->type == en_tempref))
                {
                    ins->temps |= TEMP_ANS;
                }
                else
                {
                    ins->temps &= ~TEMP_ANS;
                }
                ins->ans = ind1;
                if (offset3 && !offset2 && !offset1)
                {
                    ind1->offset = offset3;
                    ind1->mode = i_direct;
                }
                else
                {
                    ind1->offset = offset1;
                    ind1->offset2 = offset2;
                    ind1->offset3 = offset3;
                    ind1->scale = scale;
                }
            }
        }
        if (ins->dc.left && ins->dc.left->mode == i_ind)
        {
            EXPRESSION *offset1 = ins->dc.left->offset, *offset2 = nullptr, *offset3 = nullptr;
            int scale = 0;
            ProcessInd(&offset1, &offset2, &offset3, &scale);
            if (offset2 || offset3)
            {
                IMODE* ind1 = (IMODE*)Alloc(sizeof(IMODE));
                *ind1 = *ins->dc.left;
                if ((offset1 && offset1->type == en_tempref) || (offset2 && offset2->type == en_tempref))
                {
                    ins->temps |= TEMP_LEFT;
                }
                else
                {
                    ins->temps &= ~TEMP_LEFT;
                }
                ins->dc.left = ind1;
                if (offset3 && !offset2 && !offset1)
                {
                    ind1->offset = offset3;
                    ind1->mode = i_direct;
                }
                else
                {
                    ind1->offset = offset1;
                    ind1->offset2 = offset2;
                    ind1->offset3 = offset3;
                    ind1->scale = scale;
                }
            }
        }
        if (ins->dc.right && ins->dc.right->mode == i_ind)
        {
            EXPRESSION *offset1 = ins->dc.right->offset, *offset2 = nullptr, *offset3 = nullptr;
            int scale = 0;
            ProcessInd(&offset1, &offset2, &offset3, &scale);
            if (offset2 || offset3)
            {
                IMODE* ind1 = (IMODE*)Alloc(sizeof(IMODE));
                *ind1 = *ins->dc.right;
                if ((offset1 && offset1->type == en_tempref) || (offset2 && offset2->type == en_tempref))
                {
                    ins->temps |= TEMP_RIGHT;
                }
                else
                {
                    ins->temps &= ~TEMP_RIGHT;
                }
                ins->dc.right = ind1;
                if (offset3 && !offset2 && !offset1)
                {
                    ind1->offset = offset3;
                    ind1->mode = i_direct;
                }
                else
                {
                    ind1->offset = offset1;
                    ind1->offset2 = offset2;
                    ind1->offset3 = offset3;
                    ind1->scale = scale;
                }
            }
        }
        return 0;
    }
    else if (pass == 2)
    {
        switch (ins->dc.opcode)
        {
            case i_assn:
                HandleAssn(ins, globalVars);
                break;
            default:
                break;
        }
        return 1;
    }
    else if (pass == 3)
    {
        switch (ins->dc.opcode)
        {
            IMODE* t;
            QUAD* newIns;
            case i_lsl:
            case i_lsr:
            case i_asr:
                if (ins->temps & TEMP_RIGHT)
                {
                    t = InitTempOpt(ins->dc.right->size, ins->dc.right->size);
                    newIns = (QUAD*)Alloc(sizeof(QUAD));
                    newIns->ans = t;
                    newIns->dc.left = ins->dc.right;
                    newIns->dc.opcode = i_assn;
                    ins->dc.right = t;
                    InsertInstruction(ins->back, newIns);
                }
                break;
            case i_sdiv:
            case i_udiv:
            case i_smod:
            case i_umod:
                /*
                if (ins->ans->size <= ISZ_ULONG && ins->dc.right->mode == i_immed && isintconst(ins->dc.right->offset))
                {
                    t = InitTempOpt(ins->dc.right->size, ins->dc.right->size);
                    newIns = (QUAD *)Alloc(sizeof(QUAD));
                    newIns->ans = t;
                    newIns->dc.left = ins->dc.right;
                    newIns->dc.opcode = i_assn;
                    ins->dc.right = t;
                    ins->temps |= TEMP_RIGHT;
                    InsertInstruction(ins->back, newIns);
                }
                */
            case i_muluh:
            case i_mulsh:
                t = InitTempOpt(ins->ans->size, ins->ans->size);
                newIns = (QUAD*)Alloc(sizeof(QUAD));
                newIns->dc.left = t;
                newIns->ans = ins->ans;
                newIns->dc.opcode = i_assn;
                ins->ans = t;
                InsertInstruction(ins, newIns);
                break;
            default:
                break;
        }
        return 1;
    }
    return 1;
}

static int floatsize(IMODE* im)
{
    if (im && im->mode == i_direct && im->retval)
        switch (im->size)
        {
            case ISZ_FLOAT:
            case ISZ_IFLOAT:
                return ISZ_FLOAT;
            case ISZ_DOUBLE:
            case ISZ_IDOUBLE:
            case ISZ_LDOUBLE:
            case ISZ_ILDOUBLE:
                return ISZ_DOUBLE;
            case ISZ_CFLOAT:
                return ISZ_CFLOAT;
            case ISZ_CDOUBLE:
            case ISZ_CLDOUBLE:
                return ISZ_CDOUBLE;
            default:
                break;
        }
    return 0;
}
#define imax(x, y) ((x) > (y) ? (x) : (y))

int examine_icode(QUAD* head)
{
    BLOCK* b = nullptr;
    bool changed = false;
    QUAD* hold = head;
    uses_substack = false;
    while (head)
    {
        if (head->dc.opcode == i_sdiv || head->dc.opcode == i_udiv || head->dc.opcode == i_smod || head->dc.opcode == i_umod)
        {
            if (head->dc.left->size < ISZ_ULONGLONG && head->dc.left->size > -ISZ_ULONGLONG)
            {
                int size;
                IMODE* im;
                QUAD* q;
                if (head->dc.opcode == i_sdiv || head->dc.opcode == i_smod)
                    size = -ISZ_ULONGLONG;
                else
                    size = ISZ_ULONGLONG;
                im = InitTempOpt(size, size);
                q = (QUAD*)beLocalAlloc(sizeof(QUAD));
                q->dc.opcode = i_assn;
                q->ans = im;
                q->dc.left = head->dc.left;
                head->dc.left = im;
                if (head->temps & TEMP_LEFT)
                    q->temps = TEMP_LEFT;
                head->temps |= TEMP_LEFT;
                q->temps |= TEMP_ANS;
                InsertInstruction(head->back, q);
            }
        }
        head = head->fwd;
    }
    head = hold;
    while (head)
    {
        int sza, szl;

        if (head->dc.opcode == i_block)
        {
            b = head->block;
        }
        // replace indirections through a non-register with a register friendly version
        else if (head->dc.opcode != i_line && head->dc.opcode != i_passthrough && head->dc.opcode != i_label &&
                 head->dc.opcode != i_expressiontag)
        {
            IMODE *ians = 0, *tans;
            if (head->ans && head->ans->mode == i_ind && head->ans->offset->type != en_tempref)
            {
                IMODE* temp;
                QUAD* q;
                IMODE* tl;
                IMODE* hl;
                ians = head->ans;
                temp = AllocateTemp(ISZ_ADDR);
                //				tempInfo[tempCount-1] = temp->offset;
                q = (QUAD*)(QUAD*)beLocalAlloc(sizeof(QUAD));
                tl = (IMODE*)(IMODE*)beLocalAlloc(sizeof(IMODE));
                tans = hl = indnode(temp, head->ans->size);
                *tl = *head->ans;
                tl->size = ISZ_ADDR;
                tl->mode = i_direct;
                q->dc.opcode = i_assn;
                q->ans = temp;
                q->dc.left = tl;
                head->ans = hl;
                head->temps |= TEMP_ANS;
                hl->bits = tl->bits;
                hl->startbit = tl->startbit;
                tl->bits = tl->startbit = 0;
                InsertInstruction(head->back, q);
                changed = true;
            }
            if (head->dc.left && head->dc.left->mode == i_ind && head->dc.left->offset->type != en_tempref)
            {
                IMODE* temp;
                QUAD* q;
                IMODE* tl;
                IMODE* hl;
                if (ians && ians == head->dc.left)
                {
                    head->dc.left = tans;
                    head->temps |= TEMP_LEFT;
                }
                else
                {
                    temp = AllocateTemp(ISZ_ADDR);
                    //				tempInfo[tempCount-1] = temp->offset;
                    q = (QUAD*)(QUAD*)beLocalAlloc(sizeof(QUAD));
                    tl = (IMODE*)(IMODE*)beLocalAlloc(sizeof(IMODE));
                    hl = indnode(temp, head->dc.left->size);
                    *tl = *head->dc.left;
                    tl->size = ISZ_ADDR;
                    tl->mode = i_direct;
                    q->dc.opcode = i_assn;
                    q->ans = temp;
                    q->dc.left = tl;
                    head->dc.left = hl;
                    head->temps |= TEMP_LEFT;
                    hl->bits = tl->bits;
                    hl->startbit = tl->startbit;
                    tl->bits = tl->startbit = 0;
                    InsertInstruction(head->back, q);
                    changed = true;
                }
            }
            if (head->dc.right && head->dc.right->mode == i_ind && head->dc.right->offset->type != en_tempref)
            {
                IMODE* temp;
                QUAD* q;
                IMODE* tl;
                IMODE* hl;
                {
                    temp = AllocateTemp(ISZ_ADDR);
                    //				tempInfo[tempCount-1] = temp->offset;
                    q = (QUAD*)(QUAD*)beLocalAlloc(sizeof(QUAD));
                    tl = (IMODE*)(IMODE*)beLocalAlloc(sizeof(IMODE));
                    hl = indnode(temp, head->dc.right->size);
                    *tl = *head->dc.right;
                    tl->size = ISZ_ADDR;
                    tl->mode = i_direct;
                    q->dc.opcode = i_assn;
                    q->ans = temp;
                    q->dc.left = tl;
                    head->dc.right = hl;
                    head->temps |= TEMP_RIGHT;
                    hl->bits = tl->bits;
                    hl->startbit = tl->startbit;
                    tl->bits = tl->startbit = 0;
                    InsertInstruction(head->back, q);
                    changed = true;
                }
            }
            if (head->dc.left && head->dc.left->bits && head->dc.right)
            {
                if ((head->dc.opcode != i_setne && head->dc.opcode != i_sete && head->dc.opcode != i_jne &&
                     head->dc.opcode != i_je) ||
                    (head->dc.left->bits != 1 ||
                     (head->dc.right->mode != i_immed || !isintconst(head->dc.right->offset) || head->dc.right->offset->v.i != 0)))
                {
                    IMODE* temp;
                    QUAD* q;
                    q = (QUAD*)(QUAD*)beLocalAlloc(sizeof(QUAD));
                    temp = AllocateTemp(head->dc.left->size);
                    q->dc.opcode = i_assn;
                    q->ans = temp;
                    q->dc.left = head->dc.left;
                    head->dc.left = temp;
                    head->temps |= TEMP_LEFT;
                    InsertInstruction(head->back, q);
                    changed = true;
                }
            }
            if (head->dc.right && head->dc.right->bits && head->dc.left)
            {
                if ((head->dc.opcode != i_setne && head->dc.opcode != i_sete && head->dc.opcode != i_jne &&
                     head->dc.opcode != i_je) ||
                    (head->dc.right->bits != 1 ||
                     (head->dc.left->mode != i_immed || !isintconst(head->dc.left->offset) || head->dc.left->offset->v.i != 0)))
                {
                    IMODE* temp;
                    QUAD* q;
                    q = (QUAD*)(QUAD*)beLocalAlloc(sizeof(QUAD));
                    temp = AllocateTemp(head->dc.right->size);
                    q->dc.opcode = i_assn;
                    q->ans = temp;
                    q->dc.left = head->dc.right;
                    head->dc.right = temp;
                    head->temps |= TEMP_RIGHT;
                    InsertInstruction(head->back, q);
                    changed = true;
                }
            }
            /* must be last because it changes head */
            if (head->ans && head->ans->bits)
            {
                if (head->dc.opcode != i_assn || head->dc.left->mode != i_immed || !isintconst(head->dc.left->offset))
                {
                    IMODE* temp;
                    QUAD* q;
                    q = (QUAD*)(QUAD*)beLocalAlloc(sizeof(QUAD));
                    *q = *head;
                    temp = AllocateTemp(head->ans->size);
                    q->ans = temp;
                    head->dc.opcode = i_assn;
                    head->dc.left = temp;
                    head->dc.right = nullptr;
                    head->temps = (head->temps & TEMP_ANS) | TEMP_LEFT;
                    InsertInstruction(head->back, q);
                    changed = true;
                }
            }
        }
        switch (head->dc.opcode)
        {
            case i_parm:
                // if pushing < int, make a temp for it so a reg will get allocated */
                szl = head->dc.left->size;
                if (szl < 0)
                    szl = -szl;
                if (szl < ISZ_UINT && (head->dc.left->offset->type != en_tempref || head->dc.left->mode == i_ind))
                {
                    IMODE* temp;
                    QUAD* q;
                    temp = AllocateTemp(ISZ_UINT);
                    //					tempInfo[tempCount-1] = temp->offset;
                    q = (QUAD*)beLocalAlloc(sizeof(QUAD));
                    q->dc.opcode = i_assn;
                    q->ans = temp;
                    q->dc.left = head->dc.left;
                    head->dc.left = temp;
                    head->temps |= TEMP_LEFT;
                    InsertInstruction(head->back, q);
                    changed = true;
                }
                else if (head->dc.left->mode == i_immed && head->dc.left->offset->type == en_auto)
                {
                    IMODE* temp;
                    QUAD* q;
                    temp = AllocateTemp(ISZ_ADDR);
                    //					tempInfo[tempCount-1] = temp->offset;
                    q = (QUAD*)beLocalAlloc(sizeof(QUAD));
                    q->dc.opcode = i_assn;
                    q->ans = temp;
                    q->dc.left = head->dc.left;
                    head->dc.left = temp;
                    head->temps |= TEMP_LEFT;
                    InsertInstruction(head->back, q);
                    changed = true;
                }
                /*
                else if (head->dc.left->mode == i_ind)
                {
                    IMODE *temp;
                    QUAD *q;
                    temp = AllocateTemp(head->dc.left->size);
//					tempInfo[tempCount-1] = temp->offset;
                    q = (QUAD *)beLocalAlloc(sizeof(QUAD));
                    q->dc.opcode = i_assn;
                    q->ans = temp;
                    q->dc.left = head->dc.left;
                    head->dc.left = temp;
                    head->temps |= TEMP_LEFT;
                    InsertInstruction(head->back, q);
                    changed = true;
                }
                */
                break;
            case i_coswitch:
                /* we want the switch selector to be in a reg.  This is especially important for
                 * the COMPACT switch type, since it needs to reuse the reg in the table processing
                 */
                if (head->dc.left->mode != i_direct || head->dc.left->offset->type != en_tempref)
                {
                    IMODE* temp;
                    QUAD* q;
                    temp = AllocateTemp(head->dc.left->size);
                    //						tempInfo[tempCount-1] = temp->offset;
                    q = (QUAD*)beLocalAlloc(sizeof(QUAD));
                    q->dc.opcode = i_assn;
                    q->ans = temp;
                    q->dc.left = head->dc.left;
                    head->dc.left = temp;
                    head->temps |= TEMP_LEFT;
                    InsertInstruction(head->back, q);
                    changed = true;
                }
                break;
            case i_assn:
                /* for conversions from floating to int, replace the assignment with a call to
                 * ftol or ftoll as necessary
                 */
                {
                    bool i1 = head->dc.left->size >= ISZ_IFLOAT;
                    bool i2 = head->ans->size >= ISZ_IFLOAT;
                    bool i3 = head->dc.left->size < ISZ_CFLOAT;
                    bool i4 = head->ans->size < ISZ_CFLOAT;
                    if ((i1 && i3 && !i2) || (i2 && i4 && !i1))
                    {

                        if (prm_lscrtdll)
                        {
                            QUAD* q = (QUAD*)Alloc(sizeof(QUAD));
                            q->dc.opcode = i_assn;
                            q->dc.left = set_symbol("__fzero", false);
                            q->dc.left->mode = i_direct;
                            q->dc.left->size = ISZ_ADDR;
                            q->ans = InitTempOpt(ISZ_ADDR, ISZ_ADDR);
                            q->temps = TEMP_ANS;
                            IMODE* im = (IMODE*)Alloc(sizeof(IMODE));
                            *im = *q->ans;
                            im->mode = i_ind;
                            im->size = head->ans->size;

                            InsertInstruction(head->back, q);
                            head->dc.left = im;
                            head->temps |= TEMP_LEFT;
                        }
                        else
                        {
                            head->dc.left = set_symbol("__fzero", false);
                            head->temps &= ~TEMP_LEFT;
                        }
                    }
                    else if (head->dc.left->size >= ISZ_FLOAT &&
                             (head->ans->size == ISZ_ULONGLONG || head->ans->size == -ISZ_ULONGLONG))
                    {
                        QUAD* q = (QUAD*)(QUAD*)beLocalAlloc(sizeof(QUAD));
                        IMODE* ret;
                        ret = AllocateTemp(head->ans->size);
                        ret->retval = true;
                        q->dc.opcode = i_parm;
                        q->dc.left = head->dc.left;
                        if (head->dc.left->size >= ISZ_CFLOAT)
                        {
                            QUAD* q1 = (QUAD*)Alloc(sizeof(QUAD));
                            q1->dc.opcode = i_assn;
                            q1->dc.left = head->dc.left;
                            q->dc.left = q1->ans = InitTempOpt(head->dc.left->size + ISZ_FLOAT - ISZ_CFLOAT,
                                                               head->dc.left->size + ISZ_FLOAT - ISZ_CFLOAT);
                            InsertInstruction(head->back, q1);
                        }
                        if (q->dc.left->size == ISZ_FLOAT || q->dc.left->size == ISZ_IFLOAT)
                        {
                            QUAD* q1 = (QUAD*)Alloc(sizeof(QUAD));
                            q1->dc.opcode = i_assn;
                            q1->dc.left = q->dc.left;
                            q->dc.left = q1->ans = InitTempOpt(q->dc.left->size + 1, q->dc.left->size + 1);
                            InsertInstruction(head->back, q1);
                        }
                        insert_parm(head->back, q);

                        q = (QUAD*)(QUAD*)beLocalAlloc(sizeof(QUAD));
                        q->dc.opcode = i_gosub;
                        if (head->ans->size == ISZ_ULONGLONG)
                            q->dc.left = set_symbol("__ftoull", true);
                        else
                            q->dc.left = set_symbol("__ftoll", true);
                        InsertInstruction(head->back, q);
                        insert_nullparmadj(head->back, sizeFromISZ(ISZ_DOUBLE));
                        if (head->ans->mode == i_ind && (head->temps & TEMP_LEFT))
                        {
                            q = (QUAD*)Alloc(sizeof(QUAD));
                            q->ans = head->ans;
                            head->ans = AllocateTemp(head->ans->size);
                            q->dc.left = head->ans;
                            q->temps = TEMP_LEFT | TEMP_ANS;
                            InsertInstruction(head, q);
                        }
                        head->dc.left = ret;
                        head->temps &= ~(TEMP_LEFT | TEMP_RIGHT);
                        if (head == b->tail)
                            b->tail = head->fwd;
                        changed = true;
                    }
                    else if (head->dc.left->size >= ISZ_FLOAT && (head->ans->size <= ISZ_ULONG && head->ans->size != ISZ_BOOLEAN))
                    {
                        QUAD* q = (QUAD*)(QUAD*)beLocalAlloc(sizeof(QUAD));
                        IMODE* ret;
                        int n = 0;
                        switch (head->ans->size)
                        {
                            case ISZ_UCHAR:
                                n = 0;
                                break;

                            case -ISZ_UCHAR:
                                n = 1;
                                break;
                            case ISZ_USHORT:
                                n = 2;
                                break;
                            case -ISZ_USHORT:
                                n = 3;
                                break;
                            case ISZ_UINT:
                                n = 4;
                                break;
                            case -ISZ_UINT:
                                n = 5;
                                break;
                            case ISZ_ULONG:
                                n = 4;
                                break;
                            case -ISZ_ULONG:
                                n = 5;
                                break;
                            default:
                                n = 1;
                                break;
                        }
                        ret = AllocateTemp(head->ans->size);
                        ret->retval = true;
                        q->dc.opcode = i_parm;
                        q->dc.left = make_immed(ISZ_UINT, n);
                        insert_parm(head->back, q);
                        q = (QUAD*)Alloc(sizeof(QUAD));
                        q->dc.opcode = i_parm;
                        q->dc.left = head->dc.left;
                        if (head->dc.left->size >= ISZ_CFLOAT)
                        {
                            QUAD* q1 = (QUAD*)Alloc(sizeof(QUAD));
                            q1->dc.opcode = i_assn;
                            q1->dc.left = head->dc.left;
                            q->dc.left = q1->ans = InitTempOpt(head->dc.left->size + ISZ_FLOAT - ISZ_CFLOAT,
                                                               head->dc.left->size + ISZ_FLOAT - ISZ_CFLOAT);
                            InsertInstruction(head->back, q1);
                        }
                        if (q->dc.left->size == ISZ_FLOAT || q->dc.left->size == ISZ_IFLOAT)
                        {
                            QUAD* q1 = (QUAD*)Alloc(sizeof(QUAD));
                            q1->dc.opcode = i_assn;
                            q1->dc.left = q->dc.left;
                            q->dc.left = q1->ans = InitTempOpt(q->dc.left->size + 1, q->dc.left->size + 1);
                            InsertInstruction(head->back, q1);
                        }
                        insert_parm(head->back, q);

                        q = (QUAD*)(QUAD*)beLocalAlloc(sizeof(QUAD));
                        q->dc.opcode = i_gosub;
                        q->dc.left = set_symbol("__ftoi", true);
                        InsertInstruction(head->back, q);
                        insert_nullparmadj(head->back, sizeFromISZ(ISZ_DOUBLE) + sizeFromISZ(ISZ_UINT));
                        if (head->ans->mode == i_ind && (head->temps & TEMP_LEFT))
                        {
                            q = (QUAD*)Alloc(sizeof(QUAD));
                            q->ans = head->ans;
                            head->ans = AllocateTemp(head->ans->size);
                            q->dc.left = head->ans;
                            q->temps = TEMP_LEFT | TEMP_ANS;
                            InsertInstruction(head, q);
                        }
                        head->dc.left = ret;
                        head->temps &= ~(TEMP_LEFT | TEMP_RIGHT);
                        if (head == b->tail)
                            b->tail = head->fwd;
                        changed = true;
                    }
                    else if (head->ans->size >= ISZ_FLOAT &&
                             (head->dc.left->size == ISZ_UINT || head->dc.left->size == ISZ_ULONG ||
                              head->dc.left->size == ISZ_ULONGLONG || head->dc.left->size == -ISZ_ULONGLONG))
                    {
                        QUAD* q = (QUAD*)(QUAD*)beLocalAlloc(sizeof(QUAD));
                        IMODE* ret;
                        ret = AllocateTemp(ISZ_DOUBLE);
                        ret->retval = true;
                        ret->altretval = true;
                        q->dc.opcode = i_parm;
                        q->dc.left = head->dc.left;
                        insert_parm(head->back, q);
                        q = (QUAD*)(QUAD*)beLocalAlloc(sizeof(QUAD));
                        q->dc.opcode = i_gosub;
                        if (head->dc.left->size == ISZ_ULONGLONG)
                            q->dc.left = set_symbol("__ulltof", true);
                        else if (head->dc.left->size == -ISZ_ULONGLONG)
                            q->dc.left = set_symbol("__lltof", true);
                        else
                            q->dc.left = set_symbol("__ultof", true);
                        InsertInstruction(head->back, q);
                        insert_nullparmadj(head->back, sizeFromISZ(head->dc.left->size));
                        if (head->ans->mode == i_ind && (head->temps & TEMP_LEFT))
                        {
                            q = (QUAD*)Alloc(sizeof(QUAD));
                            q->ans = head->ans;
                            head->ans = AllocateTemp(head->ans->size);
                            q->dc.left = head->ans;
                            q->temps = TEMP_LEFT | TEMP_ANS;
                            InsertInstruction(head, q);
                        }
                        head->dc.left = ret;
                        head->temps &= ~(TEMP_LEFT | TEMP_RIGHT);
                        if (head == b->tail)
                            b->tail = head->fwd;
                        changed = true;
                    }
                    else
                    {
                        szl = head->dc.left->size;
                        sza = head->ans->size;
                        if (szl != sza)
                        {
                            if (head->dc.left->mode != i_immed &&
                                (head->dc.left->offset->type != en_tempref || head->dc.left->mode == i_ind ||
                                 (szl >= ISZ_FLOAT && sza < ISZ_FLOAT)))
                            {
                                IMODE* temp;
                                QUAD* q;
                                temp = AllocateTemp(head->dc.left->size);
                                //						tempInfo[tempCount-1] = temp->offset;
                                q = (QUAD*)beLocalAlloc(sizeof(QUAD));
                                q->dc.opcode = i_assn;
                                q->ans = temp;
                                q->dc.left = head->dc.left;
                                q->temps = head->temps | TEMP_ANS;
                                head->dc.left = temp;
                                head->temps |= TEMP_LEFT;
                                InsertInstruction(head->back, q);
                                changed = true;
                            }
                            if (head->ans->offset->type != en_tempref || head->ans->mode == i_ind)
                            {
                                IMODE* temp;
                                QUAD* q;
                                temp = AllocateTemp(head->ans->size);
                                //						tempInfo[tempCount-1] = temp->offset;
                                q = (QUAD*)beLocalAlloc(sizeof(QUAD));
                                q->dc.opcode = i_assn;
                                q->dc.left = temp;
                                q->ans = head->ans;
                                q->temps = head->temps | TEMP_LEFT;
                                head->ans = temp;
                                head->temps |= TEMP_ANS;
                                InsertInstruction(head, q);
                                changed = true;
                            }
                        }
                        else if (!head->dc.left->retval &&
                                 (head->dc.left->offset->type != en_tempref || head->dc.left->mode == i_ind) &&
                                 (head->dc.left->mode != i_immed || !isintconst(head->dc.left->offset)) && !head->ans->retval &&
                                 (head->ans->offset->type != en_tempref || head->ans->mode == i_ind))
                        { /* make sure at least one side of the assignment is in a reg */
                            if (szl < ISZ_FLOAT || chosenAssembler->arch->hasFloatRegs)
                            {
                                IMODE* temp;
                                QUAD* q;
                                temp = AllocateTemp(head->ans->size);
                                //						tempInfo[tempCount-1] = temp->offset;
                                q = (QUAD*)beLocalAlloc(sizeof(QUAD));
                                q->dc.opcode = i_assn;
                                q->dc.left = temp;
                                q->ans = head->ans;
                                head->ans = temp;
                                head->temps |= TEMP_ANS;
                                InsertInstruction(head, q);
                                changed = true;
                            }
                        }
                    }
                }
                break;
            case i_substack:
                uses_substack = true;
                /* replace a substack node with a call to the function which performs the
                 * function.  This is a windows thing specifically, because if the stack
                 * based allocation is > 4096 bytes we will cause a GPF.  The function basically
                 * allocates 4K blocks of data in such a way that the stack can be expanded without
                 * GPFs
                 */
                /*
               if (head->dc.left->mode != i_immed || head->dc.left->offset->v.i > 4080)
               {
                   QUAD *q = (QUAD *)beLocalAlloc(sizeof(QUAD));
                    IMODE *ret ;
                   ret = AllocateTemp(head->ans->size);
                   ret->retval = true;
                   q->dc.opcode = i_parm;
                   q->dc.left = head->dc.left;
                   insert_parm(head->back, q);
                   q = (QUAD *)beLocalAlloc(sizeof(QUAD));
                   q->dc.opcode = i_gosub;
                   q->dc.left = set_symbol( "___substackp", true);
                   InsertInstruction(head->back, q);
                   insert_nullparmadj(head->back, 4);
                   if (head->ans->mode == i_ind && (head->temps & TEMP_LEFT))
                   {
                       q = (QUAD *)Alloc(sizeof(QUAD));
                       q->ans = head->ans;
                       head->ans = AllocateTemp(head->ans->size);
                       q->dc.left = head->ans;
                       q->temps = TEMP_LEFT | TEMP_ANS;
                       InsertInstruction(head, q);
                   }
                   head->dc.left = ret ;
                   head->dc.opcode = i_assn;
                   head->temps &= ~(TEMP_LEFT | TEMP_RIGHT);
                   if (head == b->tail)
                       b->tail = head->fwd;
                   changed = true;
               }
               */
                break;
            case i_not:
            case i_neg:
                if (head->ans->mode != i_direct || head->ans->offset->type != en_tempref)
                {
                    if (!equalimode(head->dc.left, head->ans))
                    {
                        IMODE* temp;
                        QUAD* q;
                        temp = AllocateTemp(head->dc.left->size);
                        //						tempInfo[tempCount-1] = temp->offset;
                        q = (QUAD*)beLocalAlloc(sizeof(QUAD));
                        q->dc.opcode = i_assn;
                        q->ans = head->ans;
                        q->dc.left = temp;
                        head->ans = temp;
                        head->temps |= TEMP_ANS;
                        InsertInstruction(head, q);
                        if (head == b->tail)
                            b->tail = head->fwd;
                        changed = true;
                    }
                }
                break;
            case i_setne:
            case i_sete:
            case i_setc:
            case i_seta:
            case i_setnc:
            case i_setbe:
            case i_setl:
            case i_setg:
            case i_setle:
            case i_setge:
                /* for the set functions, the 386 instruction returns a byte value
                 * make sure a conversion is done if necessary
                 */
                /*
               if (head->ans->size < -ISZ_UCHAR || head->ans->size > ISZ_UCHAR)
               {
                       IMODE *temp;
                       QUAD *q;
                       temp = AllocateTemp(ISZ_UCHAR);
//						tempInfo[tempCount-1] = temp->offset;
                       q = (QUAD *)beLocalAlloc(sizeof(QUAD));
                       q->dc.opcode = i_assn;
                       q->ans = temp;
                       q->dc.left = head->dc.left;
                       head->dc.left = temp;
                       head->temps |= TEMP_LEFT;
                       InsertInstruction(head->back, q);
                       changed = true;
               }
               */
                /* fall through */
            case i_jc:
            case i_ja:
            case i_je:
            case i_jnc:
            case i_jbe:
            case i_jne:
            case i_jl:
            case i_jg:
            case i_jle:
            case i_jge:
                /* make sure one of the ops is in a register */
                if (head->dc.left->size < ISZ_FLOAT)
                {
                    if (head->dc.left->mode == i_immed && head->dc.left->offset->type == en_auto)
                    {
                        IMODE* temp;
                        QUAD* q;
                        temp = AllocateTemp(head->dc.left->size);
                        //						tempInfo[tempCount-1] = temp->offset;
                        q = (QUAD*)beLocalAlloc(sizeof(QUAD));
                        q->dc.opcode = i_assn;
                        q->ans = temp;
                        q->dc.left = head->dc.left;
                        head->dc.left = temp;
                        head->temps |= TEMP_LEFT;
                        InsertInstruction(head->back, q);
                        changed = true;
                    }
                    if (head->dc.right->mode == i_immed &&
                        (head->dc.left->mode == i_immed || head->dc.right->offset->type == en_auto))
                    {
                        IMODE* temp;
                        QUAD* q;
                        temp = AllocateTemp(head->dc.right->size);
                        //						tempInfo[tempCount-1] = temp->offset;
                        q = (QUAD*)beLocalAlloc(sizeof(QUAD));
                        q->dc.opcode = i_assn;
                        q->ans = temp;
                        q->dc.left = head->dc.right;
                        head->dc.right = temp;
                        head->temps |= TEMP_RIGHT;
                        InsertInstruction(head->back, q);
                        changed = true;
                    }
                    if (head->dc.left->mode != i_immed && head->dc.right->mode != i_immed &&
                        (head->dc.left->mode != i_direct || head->dc.left->offset->type != en_tempref) &&
                        (head->dc.right->mode != i_direct || head->dc.right->offset->type != en_tempref))
                    {
                        IMODE* temp;
                        QUAD* q;
                        temp = AllocateTemp(head->dc.right->size);
                        //						tempInfo[tempCount-1] = temp->offset;
                        q = (QUAD*)beLocalAlloc(sizeof(QUAD));
                        q->dc.opcode = i_assn;
                        q->ans = temp;
                        q->dc.left = head->dc.right;
                        head->dc.right = temp;
                        head->temps |= TEMP_RIGHT;
                        InsertInstruction(head->back, q);
                        changed = true;
                    }
                }
                break;
            case i_sub:
                if (head->dc.left->mode == i_immed && (head->dc.left->offset->type == en_auto))
                {
                    IMODE* temp;
                    QUAD* q;
                    temp = AllocateTemp(ISZ_ADDR);
                    //						tempInfo[tempCount-1] = temp->offset;
                    q = (QUAD*)beLocalAlloc(sizeof(QUAD));
                    q->dc.opcode = i_assn;
                    q->ans = temp;
                    q->dc.left = head->dc.left;
                    head->dc.left = temp;
                    head->temps |= TEMP_LEFT;
                    InsertInstruction(head->back, q);
                    changed = true;
                }
                if (head->dc.right->mode == i_immed && (head->dc.right->offset->type == en_auto))
                {
                    IMODE* temp;
                    QUAD* q;
                    temp = AllocateTemp(ISZ_ADDR);
                    //						tempInfo[tempCount-1] = temp->offset;
                    q = (QUAD*)beLocalAlloc(sizeof(QUAD));
                    q->dc.opcode = i_assn;
                    q->ans = temp;
                    q->dc.left = head->dc.right;
                    head->dc.right = temp;
                    head->temps |= TEMP_RIGHT;
                    InsertInstruction(head->back, q);
                    changed = true;
                }
                if (head->dc.right->mode == i_ind)
                {
                    if (head->ans->size == ISZ_ULONGLONG || head->ans->size == -ISZ_ULONGLONG)
                    {
                        IMODE* temp;
                        QUAD* q;
                        temp = AllocateTemp(head->dc.right->size);
                        //						tempInfo[tempCount-1] = temp->offset;
                        q = (QUAD*)beLocalAlloc(sizeof(QUAD));
                        q->dc.opcode = i_assn;
                        q->ans = temp;
                        q->dc.left = head->dc.right;
                        head->dc.right = temp;
                        head->temps |= TEMP_RIGHT;
                        InsertInstruction(head->back, q);
                        if (head == b->tail)
                            b->tail = head->fwd;
                        changed = true;
                    }
                }
            case i_and:
            case i_or:
            case i_eor:
            case i_add:
            case i_array:
            case i_struct:
                /* make sure one of the ops is in a register  if necessary*/
            binary_join:
                if (head->ans->mode != i_direct || head->ans->offset->type != en_tempref)
                {
                    if ((!equalimode(head->dc.left, head->ans) || head->dc.right->mode != i_immed) &&
                        (!equalimode(head->dc.right, head->ans) || head->dc.left->mode != i_immed))
                    {
                        IMODE* temp;
                        QUAD* q;
                        temp = AllocateTemp(head->dc.left->size);
                        //						tempInfo[tempCount-1] = temp->offset;
                        q = (QUAD*)beLocalAlloc(sizeof(QUAD));
                        q->dc.opcode = i_assn;
                        q->ans = head->ans;
                        q->dc.left = temp;
                        head->ans = temp;
                        head->temps |= TEMP_ANS;
                        InsertInstruction(head, q);
                        if (head == b->tail)
                            b->tail = head->fwd;
                        changed = true;
                    }
                }
                if (head->ans->size == ISZ_ULONGLONG || head->ans->size == -ISZ_ULONGLONG)
                {
                    if (head->dc.left->mode == i_ind && head->dc.right->mode == i_ind)
                    {
                        IMODE* temp;
                        QUAD* q;
                        temp = AllocateTemp(head->dc.right->size);
                        //						tempInfo[tempCount-1] = temp->offset;
                        q = (QUAD*)beLocalAlloc(sizeof(QUAD));
                        q->dc.opcode = i_assn;
                        q->ans = temp;
                        q->dc.left = head->dc.right;
                        head->dc.right = temp;
                        head->temps |= TEMP_RIGHT;
                        InsertInstruction(head->back, q);
                        if (head == b->tail)
                            b->tail = head->fwd;
                        changed = true;
                    }
                    else if (head->dc.right->mode == i_ind)
                    {
                        /* sub will have already been taken care of */
                        int flags = head->temps;
                        IMODE* temp = head->dc.left;
                        head->dc.left = head->dc.right;
                        head->dc.right = temp;
                        head->temps = (head->temps & TEMP_ANS) | ((flags & TEMP_RIGHT) ? TEMP_LEFT : 0) |
                                      ((flags & TEMP_LEFT) ? TEMP_RIGHT : 0);
                        changed = true;
                    }
                }
                else if ((head->temps & (TEMP_ANS | TEMP_LEFT | TEMP_RIGHT)) == (TEMP_ANS | TEMP_LEFT | TEMP_RIGHT))
                {
                    if (head->ans->size != ISZ_ADDR && head->ans->mode == i_direct && head->dc.left->mode == i_direct &&
                        head->dc.right->mode == i_direct)
                    {
                        // this selects the least recently assigned IND node and
                        // replaces one of the temps with it...
                        if (head->dc.left != head->dc.right)
                        {
                            QUAD* q = head->back;
                            QUAD *first = nullptr, *second = nullptr;
                            int flags = 0;
                            while (q && q->dc.opcode != i_block)
                            {
                                if (q->ans == head->dc.right && !(flags & 2))
                                {
                                    flags |= 2;
                                    if (q->dc.opcode == i_assn && q->ans->size == q->dc.left->size)
                                    {
                                        if (q->dc.left->mode == i_ind)
                                        {
                                            if (first)
                                            {
                                                second = q;
                                                break;
                                            }
                                            else
                                            {
                                                first = q;
                                            }
                                        }
                                    }
                                }
                                else if (q->dc.opcode != i_sub && q->dc.opcode != i_sdiv && q->dc.opcode != i_udiv &&
                                         q->dc.opcode != i_smod && q->dc.opcode != i_umod && q->ans == head->dc.left &&
                                         !(flags & 1))
                                {
                                    flags |= 1;
                                    if (q->dc.opcode == i_assn && q->ans->size == q->dc.left->size)
                                    {
                                        if (q->dc.left->mode == i_ind)
                                        {
                                            if (first)
                                            {
                                                second = q;
                                                break;
                                            }
                                            else
                                            {
                                                first = q;
                                            }
                                        }
                                    }
                                }
                                q = q->back;
                            }
                            if (second)
                                first = second;
                            if (first && !first->dc.left->bits)
                            {
                                if (head->dc.left == first->ans)
                                {
                                    head->dc.left = first->dc.left;
                                }
                                else
                                {
                                    head->dc.right = first->dc.left;
                                }
                            }
                        }
                    }
                }

                break;
            case i_lsl:
            case i_lsr:
            case i_asr:
                /* for long long argument, we need to call a helper function */
                if (head->ans->size == ISZ_ULONGLONG || head->ans->size == -ISZ_ULONGLONG)
                {
                    if (head->dc.right->mode != i_immed)
                    {
                        QUAD* q;
                        IMODE* temp;
                        IMODE* ret;
                        ret = AllocateTemp(ISZ_ULONGLONG);
                        ret->retval = true;
                        if (head->dc.right->size >= ISZ_ULONGLONG || head->dc.right->size == -ISZ_ULONGLONG)
                        {
                            temp = AllocateTemp(ISZ_ULONG);
                            q = (QUAD*)(QUAD*)beLocalAlloc(sizeof(QUAD));
                            q->dc.opcode = i_assn;
                            q->ans = temp;
                            q->dc.left = head->dc.right;
                            head->dc.right = temp;
                            head->temps |= TEMP_RIGHT;
                            InsertInstruction(head->back, q);
                        }
                        q = (QUAD*)(QUAD*)beLocalAlloc(sizeof(QUAD));
                        q->dc.opcode = i_parm;
                        q->dc.left = head->dc.right;
                        insert_parm(head->back, q);
                        q = (QUAD*)(QUAD*)beLocalAlloc(sizeof(QUAD));
                        q->dc.opcode = i_parm;
                        q->dc.left = head->dc.left;
                        insert_parm(head->back, q);
                        q = (QUAD*)(QUAD*)beLocalAlloc(sizeof(QUAD));
                        q->dc.opcode = i_gosub;
                        switch (head->dc.opcode)
                        {
                            case i_lsl:
                                q->dc.left = set_symbol("__LXSHL", true);
                                break;
                            case i_lsr:
                                q->dc.left = set_symbol("__LXSHR", true);
                                break;
                            case i_asr:
                                q->dc.left = set_symbol("__LXSAR", true);
                                break;
                            default:
                                break;
                        }
                        InsertInstruction(head->back, q);
                        insert_nullparmadj(head->back, 12);
                        head->dc.left = ret;
                        head->dc.right = nullptr;
                        head->dc.opcode = i_assn;
                        head->temps &= ~(TEMP_LEFT | TEMP_RIGHT);
                        if (head->ans->mode == i_ind && (head->temps & TEMP_LEFT))
                        {
                            q = (QUAD*)Alloc(sizeof(QUAD));
                            q->ans = head->ans;
                            head->ans = AllocateTemp(head->ans->size);
                            q->dc.left = head->ans;
                            q->temps = TEMP_LEFT | TEMP_ANS;
                            InsertInstruction(head, q);
                        }
                        if (head == b->tail)
                            b->tail = head->fwd;
                        changed = true;
                    }
                    else if (head->ans->offset->type != en_tempref || head->ans->mode == i_ind)
                    {
                        IMODE* temp;
                        QUAD* q;
                        temp = AllocateTemp(head->dc.left->size);
                        q = (QUAD*)beLocalAlloc(sizeof(QUAD));
                        q->dc.opcode = i_assn;
                        q->dc.left = temp;
                        q->ans = head->ans;
                        head->ans = temp;
                        head->temps |= TEMP_ANS;
                        InsertInstruction(head, q);
                        changed = true;
                    }
                }
                else
                {
                    if (head->dc.left->mode != i_immed && (head->dc.left->mode != i_direct || !(head->temps & TEMP_LEFT)))
                    {
                        IMODE* temp;
                        QUAD* q;
                        temp = AllocateTemp(head->dc.left->size);
                        q = (QUAD*)beLocalAlloc(sizeof(QUAD));
                        q->dc.opcode = i_assn;
                        q->dc.left = head->dc.left;
                        q->ans = temp;
                        ;
                        head->dc.left = temp;
                        head->temps |= TEMP_LEFT;
                        InsertInstruction(head->back, q);
                        changed = true;
                    }
                    if (head->dc.right->mode != i_immed && (head->dc.right->mode != i_direct || !(head->temps & TEMP_RIGHT)))
                    {
                        IMODE* temp;
                        QUAD* q;
                        temp = AllocateTemp(head->dc.left->size);
                        q = (QUAD*)beLocalAlloc(sizeof(QUAD));
                        q->dc.opcode = i_assn;
                        q->dc.left = head->dc.right;
                        q->ans = temp;
                        ;
                        head->dc.right = temp;
                        head->temps |= TEMP_RIGHT;
                        InsertInstruction(head->back, q);
                        changed = true;
                    }
                    goto binary_join;
                }
                break;
            case i_mul:
                if (head->ans->size == ISZ_UCHAR || head->ans->size == -ISZ_UCHAR)
                {
                    IMODE* t = InitTempOpt(ISZ_UINT, ISZ_UINT);
                    QUAD* newIns = (QUAD*)Alloc(sizeof(QUAD));
                    newIns->ans = t;
                    newIns->dc.left = head->dc.left;
                    newIns->dc.opcode = i_assn;
                    newIns->temps = (head->temps & TEMP_LEFT) | TEMP_ANS;
                    head->dc.left = t;
                    InsertInstruction(head->back, newIns);
                    t = InitTempOpt(ISZ_UINT, ISZ_UINT);
                    newIns = (QUAD*)Alloc(sizeof(QUAD));
                    newIns->ans = t;
                    newIns->dc.left = head->dc.right;
                    newIns->dc.opcode = i_assn;
                    newIns->temps = ((head->temps & TEMP_RIGHT) ? TEMP_LEFT : 0) | TEMP_ANS;
                    head->dc.right = t;
                    InsertInstruction(head->back, newIns);
                    t = InitTempOpt(ISZ_UINT, ISZ_UINT);
                    newIns = (QUAD*)Alloc(sizeof(QUAD));
                    newIns->ans = head->ans;
                    newIns->dc.left = t;
                    newIns->dc.opcode = i_assn;
                    newIns->temps = TEMP_LEFT | (head->temps & TEMP_ANS);
                    head->ans = t;
                    InsertInstruction(head, newIns);
                    head->temps = TEMP_LEFT | TEMP_RIGHT | TEMP_ANS;
                    break;
                }
            case i_sdiv:
            case i_udiv:
            case i_smod:
            case i_umod:
                /* for complex argument, we need to call a helper function */
                if (head->ans->size >= ISZ_CFLOAT)
                {
                    QUAD* q;
                    int limited = head->cxlimited;
                    IMODE* ret;
                    ret = AllocateTemp(ISZ_CDOUBLE);
                    ret->retval = true;
                    if (head->ans->size == ISZ_CFLOAT)
                    {
                        q = (QUAD*)beLocalAlloc(sizeof(QUAD));
                        q->dc.opcode = i_assn;
                        q->alwayslive = true;
                        q->ans = InitTempOpt(ISZ_CLDOUBLE, ISZ_CLDOUBLE);
                        q->dc.left = head->dc.right;
                        head->dc.right = q->ans;
                        q->temps = TEMP_ANS | (head->temps & TEMP_RIGHT ? TEMP_LEFT : 0);
                        InsertInstruction(head->back, q);
                    }
                    q = (QUAD*)(QUAD*)beLocalAlloc(sizeof(QUAD));
                    q->dc.opcode = i_parm;
                    q->dc.left = head->dc.right;
                    insert_parm(head->back, q);
                    if (head->ans->size == ISZ_CFLOAT)
                    {
                        q = (QUAD*)beLocalAlloc(sizeof(QUAD));
                        q->dc.opcode = i_assn;
                        q->alwayslive = true;
                        q->ans = InitTempOpt(ISZ_CLDOUBLE, ISZ_CLDOUBLE);
                        q->dc.left = head->dc.left;
                        head->dc.left = q->ans;
                        q->temps = TEMP_ANS | (head->temps & TEMP_LEFT ? TEMP_LEFT : 0);
                        InsertInstruction(head->back, q);
                    }
                    q = (QUAD*)(QUAD*)beLocalAlloc(sizeof(QUAD));
                    q->dc.opcode = i_parm;
                    q->dc.left = head->dc.left;
                    insert_parm(head->back, q);
                    q = (QUAD*)(QUAD*)beLocalAlloc(sizeof(QUAD));
                    q->dc.opcode = i_gosub;
                    switch (head->dc.opcode)
                    {
                        case i_mul:
                            if (limited)
                                q->dc.left = set_symbol("__CPLXMULLIM", true);
                            else
                                q->dc.left = set_symbol("__CPLXMUL", true);
                            break;
                        case i_sdiv:
                        case i_udiv:
                            if (limited)
                                q->dc.left = set_symbol("__CPLXDIVLIM", true);
                            else
                                q->dc.left = set_symbol("__CPLXDIV", true);
                            break;
                        default:
                            break;
                    }
                    InsertInstruction(head->back, q);
                    insert_nullparmadj(head->back, 8 * 4);
                    if (head->ans->size != ISZ_CDOUBLE)
                    {
                        QUAD* q1 = (QUAD*)Alloc(sizeof(QUAD));
                        q1->dc.opcode = i_assn;
                        q1->dc.left = ret;
                        q1->alwayslive = true;
                        ret = q1->ans = InitTempOpt(ret->size, ret->size);
                        InsertInstruction(head->back, q1);
                    }

                    head->dc.left = ret;
                    head->dc.right = nullptr;
                    head->dc.opcode = i_assn;
                    head->temps &= ~TEMP_RIGHT;
                    if (head->ans->mode == i_ind && (head->temps & TEMP_LEFT))
                    {
                        q = (QUAD*)Alloc(sizeof(QUAD));
                        q->ans = head->ans;
                        head->ans = AllocateTemp(head->ans->size);
                        q->dc.left = head->ans;
                        q->temps = TEMP_LEFT | TEMP_ANS;
                        InsertInstruction(head, q);
                    }
                    if (head == b->tail)
                        b->tail = head->fwd;
                    changed = true;
                }
                /* for long long argument, we need to call a helper function */
                else if (head->ans->size == ISZ_ULONGLONG || head->ans->size == -ISZ_ULONGLONG)
                {
                    QUAD* q;
                    IMODE* ret;
                    ret = AllocateTemp(head->ans->size);
                    ret->retval = true;
                    q = (QUAD*)(QUAD*)beLocalAlloc(sizeof(QUAD));
                    q->dc.opcode = i_parm;
                    q->dc.left = head->dc.right;
                    insert_parm(head->back, q);
                    q = (QUAD*)(QUAD*)beLocalAlloc(sizeof(QUAD));
                    q->dc.opcode = i_parm;
                    q->dc.left = head->dc.left;
                    insert_parm(head->back, q);
                    q = (QUAD*)(QUAD*)beLocalAlloc(sizeof(QUAD));
                    q->dc.opcode = i_gosub;
                    switch (head->dc.opcode)
                    {
                        case i_mul:
                            q->dc.left = set_symbol("__LXMUL", true);
                            break;
                        case i_sdiv:
                            q->dc.left = set_symbol("__LXDIVS", true);
                            break;
                        case i_udiv:
                            q->dc.left = set_symbol("__LXDIVU", true);
                            break;
                        case i_smod:
                            q->dc.left = set_symbol("__LXMODS", true);
                            break;
                        case i_umod:
                            q->dc.left = set_symbol("__LXMODU", true);
                            break;
                        default:
                            break;
                    }
                    InsertInstruction(head->back, q);
                    insert_nullparmadj(head->back, 16);
                    head->dc.left = ret;
                    head->dc.right = nullptr;
                    head->dc.opcode = i_assn;
                    head->temps &= ~(TEMP_LEFT | TEMP_RIGHT);
                    if (head->ans->mode == i_ind && (head->temps & TEMP_LEFT))
                    {
                        q = (QUAD*)Alloc(sizeof(QUAD));
                        q->ans = head->ans;
                        head->ans = AllocateTemp(head->ans->size);
                        q->dc.left = head->ans;
                        q->temps = TEMP_LEFT | TEMP_ANS;
                        InsertInstruction(head, q);
                    }
                    if (head == b->tail)
                        b->tail = head->fwd;
                    changed = true;
                }
                else if (!(head->temps & TEMP_ANS) && (head->dc.opcode != i_mul || !(head->temps & TEMP_LEFT)))
                {
                    QUAD* q;
                    IMODE* temp;
                    temp = AllocateTemp(head->dc.left->size);
                    q = (QUAD*)(QUAD*)beLocalAlloc(sizeof(QUAD));
                    q->dc.opcode = i_assn;
                    q->dc.left = temp;
                    q->ans = head->ans;
                    head->ans = temp;
                    head->temps |= TEMP_ANS;
                    InsertInstruction(head, q);
                    if (head == b->tail)
                        b->tail = head->fwd;
                    changed = true;
                }
                // this is primarily to optimize away the multiply by I needed to get an imaginary number
                // the multiplication isn't fixed in the front end because the resulting conversion would be caught in this function
                // and replaced with zero...
                else if (head->dc.right->mode == i_immed &&
                         (isfloatconst(head->dc.right->offset) || isimaginaryconst(head->dc.right->offset)) &&
                         head->dc.right->offset->v.f.ValueIsOne())
                {
                    head->temps &= ~TEMP_RIGHT;
                    head->dc.right = nullptr;
                    head->dc.opcode = i_assn;
                    break;
                }
                else
                    goto binary_join;
                break;
            default:
                break;
        }
        head = head->fwd;
    }
    if (prm_useesp)
    {
        extern void SetUsesESP(bool yes);
        SetUsesESP(!uses_substack && !theCurrentFunc->xc && !theCurrentFunc->canThrow);
    }
    int floatretsize = 0;
    head = hold;
    while (head)
    {
        if (head->dc.opcode == i_assn)
        {
            floatretsize = imax(floatretsize, floatsize(head->ans));
            floatretsize = imax(floatretsize, floatsize(head->dc.left));
        }
        head = head->fwd;
    }
    TYPE* fltret = nullptr;
    switch (floatretsize)
    {
        case ISZ_FLOAT:
        case ISZ_DOUBLE:
            fltret = (TYPE*)Alloc(sizeof(TYPE));
            fltret->type = bt_double;
            fltret->size = 8;
            fltret->rootType = fltret;
            break;
        case ISZ_CFLOAT:
        case ISZ_CDOUBLE:
            fltret = (TYPE*)Alloc(sizeof(TYPE));
            fltret->type = bt_double_complex;
            fltret->size = 16;
            fltret->rootType = fltret;
            break;
    }
    if (fltret)
    {
        fltexp = anonymousVar(sc_auto, fltret);
        cacheTempSymbol(fltexp->v.sp);
    }
    else
    {
        fltexp = nullptr;
    }
    return changed;
}
static void IterateConflict(int ans, int t)
{
    QUAD* head = tempInfo[t]->instructionDefines;
    if (head)
    {
        if ((head->temps & TEMP_LEFT) && head->dc.left->mode == i_direct)
        {
            if (head->dc.left->offset->v.sp->pushedtotemp)
                insertConflict(ans, head->dc.left->offset->v.sp->value.i);
            else
                IterateConflict(ans, head->dc.left->offset->v.sp->value.i);
        }
        if ((head->temps & TEMP_RIGHT) && head->dc.right->mode == i_direct)
        {
            if (head->dc.right->offset->v.sp->pushedtotemp)
                insertConflict(ans, head->dc.right->offset->v.sp->value.i);
            else
                IterateConflict(ans, head->dc.right->offset->v.sp->value.i);
        }
    }
}
void cg_internal_conflict(QUAD* head)
{
    switch (head->dc.opcode)
    {
        case i_udiv:
        case i_sdiv:
        case i_umod:
        case i_smod:
        case i_muluh:
        case i_mulsh:
            /* for divs we have to make sure the answer node conflicts with anything
             * that was used to load the numerator...
             */
            if (head->ans->offset && head->ans->offset->type == en_tempref && head->dc.left->offset &&
                head->dc.left->offset->type == en_tempref)
            {
                int t1 = head->ans->offset->v.sp->value.i;
                IterateConflict(t1, head->dc.left->offset->v.sp->value.i);
            }
        case i_lsl:
        case i_lsr:
        case i_asr:
        case i_sub:
            /* make sure that when regs are allocated, the right- hand argument is in a different
             * reg than the result.  For shifts this is the count value, for divs this is the denominator
             * for subs we do it to avoid generating neg instructions later on...
             */
            if (head->ans->offset && head->ans->offset->type == en_tempref && head->dc.right->offset &&
                head->dc.right->offset->type == en_tempref)
            {
                //can't do it for long longs in registers on this architecture, due to a dearth of registers...
                if (head->dc.opcode != i_sub || head->ans->size != ISZ_ULONGLONG && head->ans->size != -ISZ_ULONGLONG)
                {
                    int t1 = head->ans->offset->v.sp->value.i;
                    int t2 = head->dc.right->offset->v.sp->value.i;
                    insertConflict(t1, t2);
                }
            }
            break;
        case i_assn:
            if (head->genConflict)
            {
                if (head->temps == (TEMP_ANS | TEMP_LEFT))
                {
                    int t1 = head->ans->offset->v.sp->value.i;
                    int t2 = head->dc.left->offset->v.sp->value.i;
                    insertConflict(t1, t2);
                }
            }
            break;
        default:
            break;
    }
}
