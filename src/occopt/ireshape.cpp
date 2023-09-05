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
#include "OptUtils.h"
#include "iblock.h"
#include "iloop.h"
#include "memory.h"
#include "ilocal.h"
#include "ilive.h"
#include "FNV_hash.h"
/* reshaping and loop induction strength reduction */

namespace Optimizer
{
static int cachedTempCount;
static std::unordered_map<QUAD*, IMODE*, OrangeC::Utils::fnv1a32_binary<DAGCOMPARE>, OrangeC::Utils::bin_eql<DAGCOMPARE>> in_hash;
static std::unordered_map<IMODE**, IMODE*, OrangeC::Utils::fnv1a32_binary<sizeof(IMODE*)>, OrangeC::Utils::bin_eql<sizeof(IMODE*)>>
    nm_hash;

#ifdef XXXXX
void DumpInvariants(void)
{
    int i;
    if (icdFile)
    {
        fprintf(icdFile, ";************** invariants\n\n");
        for (i = 0; i < tempCount; i++)
        {
            if (tempInfo[i]->variantLoop)
            {
                fprintf(icdFile, "T%d = L%d\n", i, tempInfo[i]->variantLoop->loopnum + 1);
            }
        }
        fprintf(icdFile, ";************** induction sets\n\n");
        for (i = 0; i < loopCount; i++)
        {
            if (loopArray[i]->type != LT_BLOCK)
            {
                INDUCTION_LIST* is = loopArray[i]->inductionSets;
                int n = 1;
                if (is)
                    fprintf(icdFile, "Loop %d\n", i + 1);
                while (is)
                {
                    LIST* dt = (LIST*)is->vars;
                    fprintf(icdFile, "%d: ", n++);
                    while (dt)
                    {
                        fprintf(icdFile, "T%d ", (int)dt->data);
                        dt = dt->next;
                    }
                    fprintf(icdFile, "\n");
                    is = is->next;
                }
            }
        }
    }
}
#endif
bool variantThisLoop(BLOCK* b, int tnum)
{
    if (tempInfo[tnum]->instructionDefines)
    {
        LOOP* thisLp = b->loopParent;
        LOOP* variant = tempInfo[tnum]->variantLoop;
        return variant == thisLp || isAncestor(thisLp, variant);
    }
    return false;
}
static bool usedThisLoop(BLOCK* b, int tnum)
{
    INSTRUCTIONLIST* l = tempInfo[tnum]->instructionUses;
    LOOP* parent = b->loopParent;
    while (l)
    {
        LOOP* thisLp = l->ins->block->loopParent;
        if (parent == thisLp || isAncestor(parent, thisLp))
            return true;
        l = l->next;
    }
    return false;
}
static bool inductionThisLoop(BLOCK* b, int tnum) { return tempInfo[tnum]->inductionLoop && variantThisLoop(b, tnum); }
bool matchesop(enum i_ops one, enum i_ops two)
{
    if (one == two)
        return true;
    switch (one)
    {
        case i_neg:
            return two == i_add || two == i_sub || two == i_mul;
        case i_not:
        case i_add:
        case i_sub:
            return two == i_add || two == i_sub;
        case i_mul:
        case i_lsl:
            return two == i_mul || two == i_lsl;
        default:
            return false;
    }
}
static RESHAPE_LIST* InsertExpression(IMODE* im, RESHAPE_EXPRESSION* expr, QUAD* ins, int flags)
{
    RESHAPE_LIST* list = tAllocate<RESHAPE_LIST>();
    RESHAPE_LIST** test = &expr->list;
    list->flags = flags;
    list->im = im;
    if (im->mode == i_immed)
        list->rporder = SHRT_MAX; /* evaluate constants as early as possible */
    else
    {
        list->rporder = ins->block->loopParent->loopnum;
    }
    expr->count++;
    /* sort in descending order */
    while (*test)
    {
        if ((*test)->rporder < list->rporder)
            break;
        test = &(*test)->next;
    }
    list->next = *test;
    *test = list;
    return list;
}
static bool GatherExpression(int tx, RESHAPE_EXPRESSION* expression, int flags)
{
    QUAD* ins = tempInfo[tx]->instructionDefines;
    if (ins && matchesop(ins->dc.opcode, expression->op))
    {
        if (ins->dc.opcode == i_neg || ins->dc.opcode == i_not)
        {
            /* the reason for this exemption is I don't want to deal with
             * maintaining the order of successive neg and not instructions
             */
            if (expression->list && expression->list->flags & (RF_NEG | RF_NOT))
                return false;
            if ((ins->temps & TEMP_LEFT) && (ins->dc.left->mode != i_ind))
            {
                if (ins->dc.left->size < ISZ_FLOAT)
                {
                    int tnum = ins->dc.left->offset->sp->i;
                    if (ins->dc.opcode == i_neg)
                        flags ^= RF_NEG;
                    else
                        flags ^= RF_NOT;
                    if (inductionThisLoop(ins->block, tnum) || !GatherExpression(tnum, expression, flags))
                        InsertExpression(ins->dc.left, expression, ins, flags);
                    return true;
                }
            }
        }
        else if ((ins->temps & (TEMP_LEFT | TEMP_RIGHT)) == (TEMP_LEFT | TEMP_RIGHT))
        {
            if (ins->dc.left->mode != i_ind && ins->dc.right->mode != i_ind)
            {
                if (ins->dc.left->size < ISZ_FLOAT && ins->dc.right->size < ISZ_FLOAT)
                {

                    int tnuml = ins->dc.left->offset->sp->i;
                    int tnumr = ins->dc.right->offset->sp->i;
                    int flags1 = flags;
                    if (ins->dc.opcode == i_sub)
                        flags ^= RF_NEG;
                    if (inductionThisLoop(ins->block, tnuml) || !GatherExpression(tnuml, expression, flags1))
                        InsertExpression(ins->dc.left, expression, ins, flags1);
                    if (inductionThisLoop(ins->block, tnumr) || !GatherExpression(tnumr, expression, flags))
                        InsertExpression(ins->dc.right, expression, ins, flags);
                    return true;
                }
            }
        }
        else if ((ins->temps & TEMP_LEFT) && ins->dc.left->mode == i_direct && ins->dc.right && ins->dc.right->mode == i_immed)
        {
            if (ins->dc.left->size < ISZ_FLOAT && ins->dc.right->size < ISZ_FLOAT)
            {
                int tnum = ins->dc.left->offset->sp->i;
                int flags1 = flags;
                RESHAPE_LIST* re;
                if (ins->dc.opcode == i_sub)
                    flags ^= RF_NEG;

                flags |= (ins->dc.opcode == i_lsl ? RF_SHIFT : 0);
                InsertExpression(ins->dc.right, expression, ins, flags);
                if (inductionThisLoop(ins->block, tnum) || !GatherExpression(tnum, expression, flags1))
                    InsertExpression(ins->dc.left, expression, ins, flags1);
                return true;
            }
        }
        else if ((ins->temps & TEMP_RIGHT) && ins->dc.right->mode == i_direct && ins->dc.left->mode == i_immed)
        {
            if (ins->dc.left->size < ISZ_FLOAT && ins->dc.right->size < ISZ_FLOAT)
            {
                int tnum = ins->dc.right->offset->sp->i;
                int flags1 = flags;
                RESHAPE_LIST* re;
                if (ins->dc.opcode == i_sub)
                    flags ^= RF_NEG;
                flags |= (ins->dc.opcode == i_lsl ? RF_SHIFT : 0);
                if (inductionThisLoop(ins->block, tnum) || !GatherExpression(tnum, expression, flags))
                    InsertExpression(ins->dc.right, expression, ins, flags);
                InsertExpression(ins->dc.left, expression, ins, flags1);
                return true;
            }
        }
    }
    return false;
}
static void CreateExpressionLists(void)
{
    int i;
    for (i = 0; i < cachedTempCount; i++)
    {
        if (tempInfo[i]->expressionRoot)
        {
            if (tempInfo[i]->enode->sizeFromType < ISZ_FLOAT)
            {
                LOOP* lp = tempInfo[i]->instructionDefines->block->loopParent;
                /* if the prior of the entry is a critical block,
                 * then don't optimize the loop
                 */
                if (lp->entry->pred && !lp->entry->pred->block->critical)
                {
                    enum i_ops op = tempInfo[i]->instructionDefines->dc.opcode;
                    tempInfo[i]->expression.op = op;
                    GatherExpression(i, &tempInfo[i]->expression, 0);
                }
                else
                {
                    tempInfo[i]->expressionRoot = false;
                }
            }
            else
            {
                tempInfo[i]->expressionRoot = false;
            }
        }
    }
}
static RESHAPE_LIST* cloneReshape(RESHAPE_LIST* in)
{
    RESHAPE_LIST *rv = nullptr, **p = &rv;
    while (in)
    {
        *p = tAllocate<RESHAPE_LIST>();
        **p = *in;
        (*p)->next = nullptr;
        in = in->next;
        p = &(*p)->next;
    }
    return rv;
}
static void DistributeMultiplies(RESHAPE_EXPRESSION* re, RESHAPE_LIST* rl, int tnum)
{
    RESHAPE_LIST *temp = tempInfo[tnum]->expression.list, *replace = nullptr;
    int n = INT_MAX;
    while (temp)
    {
        if (!temp->distributed && temp != rl)
        {
            int tnx;
            if (temp->im->offset->type != se_tempref)
            {
                replace = temp;
                break;
            }
            tnx = temp->im->offset->sp->i;
            if (tempInfo[tnx]->expression.count < n)
            {
                n = tempInfo[tnx]->expression.count;
                replace = temp;
            }
        }
        temp = temp->next;
    }
    if (replace)
    {
        RESHAPE_LIST** temp1;
        replace->distrib = cloneReshape(re->list);
        replace->distributed = true;
        temp1 = &tempInfo[tnum]->expression.list;
        while (*temp1)
        {
            if (*temp1 == rl)
            {
                *temp1 = (*temp1)->next;
                break;
            }
            temp1 = &(*temp1)->next;
        }
    }
    else
        rl->distributed = true;
}
static void ApplyDistribution(void)
{
    bool done = false;
    while (!done)
    {
        int i;
        RESHAPE_EXPRESSION* next = nullptr;
        RESHAPE_LIST* rl = nullptr;
        int n = 0;
        int match = 0;
        for (i = 0; i < cachedTempCount; i++)
        {
            if (tempInfo[i]->expressionRoot)
            {
                switch (tempInfo[i]->expression.op)
                {
                    case i_mul:
                    case i_lsl: {
                        RESHAPE_LIST* l = tempInfo[i]->expression.list;
                        while (l)
                        {
                            if (!l->distributed)
                            {
                                if (l->im->offset->type == se_tempref)
                                {
                                    RESHAPE_EXPRESSION* re = &tempInfo[l->im->offset->sp->i]->expression;
                                    if (re->count > n)
                                    {
                                        n = re->count;
                                        next = re;
                                        match = i;
                                        rl = l;
                                    }
                                }
                            }
                            l = l->next;
                        }
                    }
                    break;
                    default:
                        break;
                }
            }
        }
        if (next)
        {
            done = false;
            DistributeMultiplies(next, rl, match);
        }
        else
            done = true;
    }
}
static void replaceIM(IMODE** iml, IMODE* im)
{
    if ((*iml)->mode == i_immed)
        return;
    nm_hash[&(*iml)->offset->sp->imvalue] = im;
    if ((*iml)->mode == i_direct)
        *iml = im;
    else
    {
        IMODELIST* imlx = im->offset->sp->imind;
        while (imlx)
        {
            if (im->size == imlx->im->size)
            {
                *iml = imlx->im;
                break;
            }
            imlx = imlx->next;
        }
        if (!imlx)
        {
            SimpleSymbol* sym = im->offset->sp;
            IMODE* imind;
            IMODELIST* imindl;
            imind = Allocate<IMODE>();
            imindl = Allocate<IMODELIST>();
            *imind = *(im);
            imind->mode = i_ind;
            imind->ptrsize = (*iml)->ptrsize;
            imind->size = (*iml)->size;
            imindl->im = imind;
            imindl->next = im->offset->sp->imind;
            im->offset->sp->imind = imindl;
            *iml = imind;
        }
    }
}
static void CopyExpressionTree(enum i_ops op, BLOCK* b, QUAD* insertBefore, IMODE** iml, IMODE** imr)
{
    if ((*iml) && (*iml)->offset->type == se_tempref)
    {
        int tnum = (*iml)->offset->sp->i;
        QUAD* def = tempInfo[tnum]->instructionDefines;

        IMODE* im = nullptr;
        auto it = nm_hash.find(&(*iml)->offset->sp->imvalue);
        if (it != nm_hash.end())
            im = it->second;
        if (im)
        {
            replaceIM(iml, im);
        }
        if (def && def->block->loopParent == b->loopParent && def->dc.opcode != i_phi)
        {
            QUAD* newIns;
            op = i_nop;
            newIns = Allocate<QUAD>();
            newIns->ans = InitTempOpt(def->ans->offset->sp->imvalue->size, def->ans->offset->sp->imvalue->size);
            newIns->dc.left = def->dc.left;
            newIns->dc.right = def->dc.right;
            newIns->dc.opcode = def->dc.opcode;
            CopyExpressionTree(op, b, insertBefore, &newIns->dc.left, &newIns->dc.right);
            InsertInstruction(insertBefore->back, newIns);
            replaceIM(iml, newIns->ans);
            in_hash[newIns] = newIns->ans;
        }
    }
    if ((*imr) && (*imr)->offset->type == se_tempref)
    {
        int tnum = (*imr)->offset->sp->i;
        QUAD* def = tempInfo[tnum]->instructionDefines;
        IMODE* im = nullptr;
        auto it = nm_hash.find(&(*imr)->offset->sp->imvalue);
        if (it != nm_hash.end())
            im = it->second;
        if (im)
        {
            replaceIM(imr, im);
        }
        if (def && def->block->loopParent == b->loopParent && def->dc.opcode != i_phi)
        {
            QUAD* newIns;
            op = i_nop;
            newIns = Allocate<QUAD>();
            newIns->ans = InitTempOpt(def->ans->offset->sp->imvalue->size, def->ans->offset->sp->imvalue->size);
            newIns->dc.left = def->dc.left;
            newIns->dc.right = def->dc.right;
            newIns->dc.opcode = def->dc.opcode;
            CopyExpressionTree(op, b, insertBefore, &newIns->dc.left, &newIns->dc.right);
            InsertInstruction(insertBefore->back, newIns);
            replaceIM(imr, newIns->ans);
            in_hash[newIns] = newIns->ans;
        }
    }
}
static IMODE* InsertAddInstruction(BLOCK* b, int size, QUAD* insertBefore, int flagsl, IMODE* iml, int flagsr, IMODE* imr)
{
    QUAD *ins, *insn = nullptr, *insn2 = nullptr;
    IMODE* imrv;
    if (b)
        CopyExpressionTree(i_add, b, insertBefore, &iml, &imr);
    if (flagsl & (RF_NEG | RF_NOT))
    {
        insn = Allocate<QUAD>();
        insn->ans = InitTempOpt(size, size);
        insn->dc.opcode = flagsl & RF_NEG ? i_neg : i_not;
        insn->dc.left = iml;
        iml = insn->ans;
    }
    /*
    if (flagsr & (RF_NEG | RF_NOT))
    {
        insn2 = Allocate<QUAD>();
        insn2->ans = InitTempOpt(size,size);
        insn2->dc.opcode = flagsr & RF_NEG ? i_neg : i_not;
        insn2->dc.left = imr;
        imr = insn2->ans;
    }
    */
    ins = Allocate<QUAD>();
    ins->dc.opcode = flagsr & RF_NEG ? i_sub : i_add;
    ins->dc.left = iml;
    ins->dc.right = imr;
    imrv = nullptr;
    auto it = in_hash.find(ins);
    if (it != in_hash.end())
        imrv = it->second;
    if (imrv)
        return imrv;
    else
    {
        ins->ans = InitTempOpt(size, size);
        if (insn)
        {
            InsertInstruction(insertBefore->back, insn);
        }
        if (insn2)
        {
            InsertInstruction(insertBefore->back, insn2);
        }
        InsertInstruction(insertBefore->back, ins);
        in_hash[ins] = ins->ans;
        return ins->ans;
    }
}
static IMODE* InsertMulInstruction(BLOCK* b, int size, QUAD* insertBefore, int flagsl, IMODE* iml, int flagsr, IMODE* imr)
{
    QUAD *ins, *insn = nullptr;
    IMODE* imrv;
    if (b)
        CopyExpressionTree(i_mul, b, insertBefore, &iml, &imr);
    if ((flagsl & RF_SHIFT) && !(flagsr & RF_SHIFT))
    {
        int ft = flagsl;
        IMODE* im = iml;
        flagsl = flagsr;
        iml = imr;
        flagsr = ft;
        imr = im;
    }
    if (flagsl & RF_SHIFT)
    {
        if (iml->mode == i_immed && isintconst(iml->offset))
        {
            iml->offset->i = (1 << iml->offset->i);
        }
        else
        {
            insn = Allocate<QUAD>();
            insn->ans = InitTempOpt(size, size);
            insn->dc.left = make_immed(size, 1);
            insn->dc.opcode = i_lsl;
            insn->dc.right = iml;
            iml = insn->ans;
        }
    }
    ins = Allocate<QUAD>();
    ins->dc.opcode = flagsr & RF_SHIFT ? i_lsl : i_mul;
    ins->dc.left = iml;
    ins->dc.right = imr;
    imrv = nullptr;
    auto it = in_hash.find(ins);
    if (it != in_hash.end())
        imrv = it->second;
    if (imrv)
        return imrv;
    else
    {
        ins->ans = InitTempOpt(size, size);
        if (insn)
        {
            InsertInstruction(insertBefore->back, insn);
        }
        InsertInstruction(insertBefore->back, ins);
        in_hash[ins] = ins->ans;
        return ins->ans;
    }
}
void unmarkPreSSA(QUAD* ins)
{
    if ((ins->temps & TEMP_ANS) && ins->ans->mode == i_direct)
    {
        INSTRUCTIONLIST* il = tempInfo[ins->ans->offset->sp->i]->instructionUses;
        tempInfo[ins->ans->offset->sp->i]->preSSATemp = -1;
        while (il)
        {
            unmarkPreSSA(il->ins);
            il = il->next;
        }
    }
}
static void RewriteAdd(BLOCK* b, int tnum)
{
    RESHAPE_LIST* gather = tempInfo[tnum]->expression.list;
    IMODE *left = tempInfo[tnum]->expression.lastName, *right = nullptr;
    int size = tempInfo[tnum]->instructionDefines->ans->size;
    QUAD* ia;
    int flagsl = 0, flagsr;
    if (b == nullptr)
    {
        ia = tempInfo[tnum]->instructionDefines;
        if (!ia)
            return;
    }
    else
    {
        if (!blockArray[b->loopParent->entry->idom])
            return;
        ia = blockArray[b->loopParent->entry->idom]->tail;
        ia = beforeJmp(ia, false);
    }
    while (gather && (!b || gather->im->mode == i_immed ||
                      (!variantThisLoop(b, gather->im->offset->sp->i) && usedThisLoop(b, gather->im->offset->sp->i))))
    {
        if (!gather->genned)
        {
            gather->genned = true;
            if (!left && (tempInfo[tnum]->expression.op == i_add || tempInfo[tnum]->expression.op == i_sub))
            {
                if (gather->flags & RF_NEG)
                {
                    /* optimization to avoid generating an extraneous i_neg when possible */
#if 0
                        RESHAPE_LIST **l = &gather->next;
                        while (*l)
                        {
                            if (!((*l)->flags & RF_NEG))
                            {
                                RESHAPE_LIST *t = *l;
                                *l = gather;
                                (*l)->next = t->next;
                                t->next = gather;
                                gather = t;
                            }
                            l = &(*l)->next;
                        }
#endif
                }
            }
            if (!left)
            {
                left = gather->im;
                flagsl = gather->flags;
            }
            else
            {
                right = gather->im;
                flagsr = gather->flags;
            }
            if (left && right)
            {
                left = InsertAddInstruction(b, size, ia, flagsl, left, flagsr, right);
                flagsl = 0;
            }
        }
        gather = gather->next;
    }
    tempInfo[tnum]->expression.lastName = left;
    if (left)
        nm_hash[&tempInfo[tnum]->enode->sp->imvalue] = left;
    ia = tempInfo[tnum]->instructionDefines;
    if (ia && left)  // && !inductionThisLoop(ia->block, tnum))
    {
        unmarkPreSSA(ia);
        if ((ia->temps & TEMP_ANS) && ia->ans->mode == i_direct)
        {
            tempInfo[ia->ans->offset->sp->i]->preSSATemp = -1;
        }
        if (ia->dc.left && (ia->temps & TEMP_LEFT))
        {
            RemoveFromUses(ia, ia->dc.left->offset->sp->i);
        }
        ia->dc.left = left;
        if (ia->dc.left && ia->dc.left->offset->type == se_tempref)
        {
            InsertUses(ia, ia->dc.left->offset->sp->i);
            ia->temps |= TEMP_LEFT;
        }
        else
            ia->temps &= ~TEMP_LEFT;
        ia->dc.opcode = i_assn;
        ia->temps &= ~TEMP_RIGHT;
    }
}
static IMODE* RewriteDistributed(BLOCK* b, int size, IMODE* im, QUAD* ia, RESHAPE_LIST* distrib, int flags)
{
    IMODE* total = distrib->lastDistribName;
    RESHAPE_LIST* gather = distrib;
    int flagsr;
    while (gather && (!b || gather->im->mode == i_immed ||
                      (!variantThisLoop(b, gather->im->offset->sp->i) && usedThisLoop(b, gather->im->offset->sp->i))))
    {
        if (!gather->genned)
        {
            IMODE* left = InsertMulInstruction(b, size, ia, flags, im, gather->flags, gather->im);
            gather->genned = true;
            if (!total)
            {
                total = left;
            }
            else
            {
                total = InsertAddInstruction(b, size, ia, 0, total, 0, left);
            }
        }
        gather = gather->next;
    }
    if (gather)
    {
        distrib->lastDistribName = total;
        return nullptr;
    }
    return total;
}
static void RewriteMul(BLOCK* b, int tnum)
{
    RESHAPE_LIST* gather = tempInfo[tnum]->expression.list;
    if (gather)
    {
        IMODE *left = tempInfo[tnum]->expression.lastName, *right = nullptr;
        int flagsl = 0, flagsr;
        int size = tempInfo[tnum]->instructionDefines->ans->size;
        QUAD* ia;
        RESHAPE_LIST* current = gather;
        if (b == nullptr)
        {
            ia = tempInfo[tnum]->instructionDefines;
            if (!ia)
                return;
        }
        else
        {
            if (!blockArray[b->loopParent->entry->idom])
                return;
            ia = blockArray[b->loopParent->entry->idom]->tail;
            ia = beforeJmp(ia, false);
        }
        // should be a branch of some sort at ia....
        while (current && (!b || current->im->mode == i_immed ||
                           (!variantThisLoop(b, current->im->offset->sp->i) && usedThisLoop(b, current->im->offset->sp->i))))
        {
            if (!current->genned && !current->distrib)
            {
                current->genned = true;
                if (!left)
                {
                    left = current->im;
                    flagsl = current->flags;
                }
                else
                {
                    right = current->im;
                    flagsr = current->flags;
                }
                if (left && right)
                {
                    left = InsertMulInstruction(b, size, ia, flagsl, left, flagsr, right);
                    flagsl = 0;
                }
            }
            current = current->next;
        }
        tempInfo[tnum]->expression.lastName = left;
        while (gather && (!b || gather->im->mode == i_immed ||
                          (!variantThisLoop(b, gather->im->offset->sp->i) && usedThisLoop(b, gather->im->offset->sp->i))))
        {
            if (gather->distrib && !gather->genned)
            {
                IMODE* im = RewriteDistributed(b, size, gather->im, ia, gather->distrib, gather->flags);
                if (im)
                {
                    if (tempInfo[tnum]->expression.lastName)
                    {
                        tempInfo[tnum]->expression.lastName =
                            InsertMulInstruction(b, size, ia, 0, im, gather->flags, tempInfo[tnum]->expression.lastName);
                        gather->genned = true;
                    }
                    else
                        tempInfo[tnum]->expression.lastName = im;
                }
            }
            gather = gather->next;
        }
        if (left)
            nm_hash[&tempInfo[tnum]->enode->sp->imvalue] = left;
        ia = tempInfo[tnum]->instructionDefines;
        if (ia && left)  // && !inductionThisLoop(ia->block, tnum))
        {
            unmarkPreSSA(ia);
            if (ia->dc.left && (ia->temps & TEMP_LEFT))
            {
                RemoveFromUses(ia, ia->dc.left->offset->sp->i);
            }
            ia->dc.left = tempInfo[tnum]->expression.lastName;
            if (ia->dc.left && ia->dc.left->offset->type == se_tempref)
            {
                InsertUses(ia, ia->dc.left->offset->sp->i);
                ia->temps |= TEMP_LEFT;
            }
            else
                ia->temps &= ~TEMP_LEFT;
            ia->dc.opcode = i_assn;
            ia->temps &= ~TEMP_RIGHT;
        }
    }
}
static void RewriteInvariantExpressions(BLOCK* b)
{
    BLOCKLIST* bl = b->dominates;
    int i;
    decltype(in_hash) old_in_hash(in_hash);
    decltype(nm_hash) old_nm_hash(nm_hash);
    in_hash.clear();
    nm_hash.clear();
    for (i = 0; i < cachedTempCount; i++)
    {
        if (tempInfo[i]->expressionRoot && tempInfo[i]->size < ISZ_FLOAT && tempInfo[i]->instructionDefines->block == b)
        {
            switch (tempInfo[i]->expression.op)
            {
                case i_assn:
                case i_phi:
                    break;
                case i_add:
                case i_sub:
                case i_or:
                case i_eor:
                case i_and:
                    RewriteAdd(b, i);
                    break;
                case i_mul:
                case i_lsl:
                    RewriteMul(b, i);
                    break;
                default:
                    diag("RewriteExpressions: invalid expression type");
                    break;
            }
        }
    }
    while (bl)
    {
        RewriteInvariantExpressions(bl->block);
        bl = bl->next;
    }
    in_hash = old_in_hash;
    nm_hash = old_nm_hash;
}
void RewriteInnerExpressions(void)
{
    int i;
    for (i = 0; i < cachedTempCount; i++)
    {
        if (tempInfo[i]->expressionRoot && tempInfo[i]->size < ISZ_FLOAT)
        {
            switch (tempInfo[i]->expression.op)
            {
                case i_assn:
                case i_phi:
                    break;
                case i_add:
                case i_sub:
                case i_or:
                case i_eor:
                case i_and:
                    RewriteAdd(nullptr, i);
                    break;
                case i_mul:
                case i_lsl:
                    RewriteMul(nullptr, i);
                    break;
                default:
                    diag("RewriteExpressions: invalid expression type");
                    break;
            }
        }
    }
}
void Reshape(void)
{
    cachedTempCount = tempCount;
    CalculateInduction();
    CreateExpressionLists();
    ApplyDistribution(); /* assumes the lists are already sorted */
    nm_hash.clear();
    in_hash.clear();
    RewriteInvariantExpressions(blockArray[0]);
    RewriteInnerExpressions();
#ifdef XXXXX
    DumpInvariants();
#endif
    tFree();
}
}  // namespace Optimizer