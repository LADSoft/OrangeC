/* Software License Agreement
 *
 *     Copyright(C) 1994-2026 David Lindauer, (LADSoft)
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

#include <cstdio>
#include <malloc.h>
#include <cstring>
#include <climits>
#include <list>
#include <memory>
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
#define RF_NOT 1
#define RF_NEG 2
#define RF_SHIFT 4

struct ReshapeList
{
    ReshapeList() : im(nullptr), lastDistribName(nullptr), rpOrder(0), rfFlags(0), distributed(0), genned(0) {}
    std::map<int, std::shared_ptr<ReshapeList>> distrib; /* distributive lists for multiplication */
    IMODE* im;
    IMODE* lastDistribName;
    unsigned rpOrder;
    unsigned char rfFlags : 4; /* holds the RF values */
    unsigned char distributed : 1;
    unsigned char genned : 1;
};
struct ReshapeExpression
{
    enum i_ops op;
    std::map<int, std::shared_ptr<ReshapeList>> reshapeList;
    short count;
    IMODE* lastName;
};

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
                auto set = loopInductionSets[i];
                int n = 1;
                if (!set.empty())
                    fprintf(icdFile, "Loop %d\n", i + 1);
                for (auto vars : set)
                {
                    fprintf(icdFile, "%d: ", n++);
                    for (auto var : vars)
                    {
                        fprintf(icdFile, "T%d ", var);
                    }
                    fprintf(icdFile, "\n");
                }
            }
        }
    }
}
#endif
bool variantThisLoop(Block* b, int tnum)
{
    if (InstructionDefinesSsa[tnum])
    {
        Loop* thisLp = b->loopParent;
        Loop* variant = tempInfo[tnum]->variantLoop;
        return variant == thisLp || isAncestor(thisLp, variant);
    }
    return false;
}
static bool usedThisLoop(Block* b, int tnum)
{
    Loop* parent = b->loopParent;
    for (auto uses : InstructionUsesSsa[tnum])
    {
        Loop* thisLp = uses->block->loopParent;
        if (parent == thisLp || isAncestor(parent, thisLp))
            return true;
    }
    return false;
}
static bool inductionThisLoop(Block* b, int tnum) { return tempInfo[tnum]->inductionLoop && variantThisLoop(b, tnum); }
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
static ReshapeList* InsertExpression(std::unordered_map<int, std::shared_ptr<ReshapeList>>& reshapes, IMODE* im,
                                     ReshapeExpression& expr, QUAD* ins, int rfFlags)
{
    ReshapeList* reshapeList = new ReshapeList;
    reshapeList->rfFlags = rfFlags;
    reshapeList->im = im;
    if (im->mode == i_immed)
        reshapeList->rpOrder = SHRT_MAX; /* evaluate constants as early as possible */
    else
    {
        reshapeList->rpOrder = ins->block->loopParent->loopnum;
    }
    expr.count++;
    reshapes[reshapeList->rpOrder] = std::move(std::shared_ptr<ReshapeList>(reshapeList));
    return reshapeList;
}
static bool GatherExpression(std::unordered_map<int, std::shared_ptr<ReshapeList>>& reshapes, int tx, ReshapeExpression& expression,
                             int rfFlags)
{
    QUAD* ins = InstructionDefinesSsa[tx];
    if (ins && matchesop(ins->dc.opcode, expression.op))
    {
        if (ins->dc.opcode == i_neg || ins->dc.opcode == i_not)
        {
            /* the reason for this exemption is I don't want to deal with
             * maintaining the order of successive neg and not instructions
             */
            if (!expression.reshapeList.empty() && (expression.reshapeList.begin()->second.get()->rfFlags & (RF_NEG | RF_NOT)))
                return false;
            if ((ins->temps & TEMP_LEFT) && (ins->dc.left->mode != i_ind))
            {
                if (ins->dc.left->size < ISZ_FLOAT)
                {
                    int tnum = ins->dc.left->offset->sp->i;
                    if (ins->dc.opcode == i_neg)
                        rfFlags ^= RF_NEG;
                    else
                        rfFlags ^= RF_NOT;
                    if (inductionThisLoop(ins->block, tnum) || !GatherExpression(reshapes, tnum, expression, rfFlags))
                        InsertExpression(reshapes, ins->dc.left, expression, ins, rfFlags);
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
                    int flags1 = rfFlags;
                    if (ins->dc.opcode == i_sub)
                        rfFlags ^= RF_NEG;
                    if (inductionThisLoop(ins->block, tnuml) || !GatherExpression(reshapes, tnuml, expression, flags1))
                        InsertExpression(reshapes, ins->dc.left, expression, ins, flags1);
                    if (inductionThisLoop(ins->block, tnumr) || !GatherExpression(reshapes, tnumr, expression, rfFlags))
                        InsertExpression(reshapes, ins->dc.right, expression, ins, rfFlags);
                    return true;
                }
            }
        }
        else if ((ins->temps & TEMP_LEFT) && ins->dc.left->mode == i_direct && ins->dc.right && ins->dc.right->mode == i_immed)
        {
            if (ins->dc.left->size < ISZ_FLOAT && ins->dc.right->size < ISZ_FLOAT)
            {
                int tnum = ins->dc.left->offset->sp->i;
                int flags1 = rfFlags;
                ReshapeList* re;
                if (ins->dc.opcode == i_sub)
                    rfFlags ^= RF_NEG;

                rfFlags |= (ins->dc.opcode == i_lsl ? RF_SHIFT : 0);
                InsertExpression(reshapes, ins->dc.right, expression, ins, rfFlags);
                if (inductionThisLoop(ins->block, tnum) || !GatherExpression(reshapes, tnum, expression, flags1))
                    InsertExpression(reshapes, ins->dc.left, expression, ins, flags1);
                return true;
            }
        }
        else if ((ins->temps & TEMP_RIGHT) && ins->dc.right->mode == i_direct && ins->dc.left->mode == i_immed)
        {
            if (ins->dc.left->size < ISZ_FLOAT && ins->dc.right->size < ISZ_FLOAT)
            {
                int tnum = ins->dc.right->offset->sp->i;
                int flags1 = rfFlags;
                ReshapeList* re;
                if (ins->dc.opcode == i_sub)
                    rfFlags ^= RF_NEG;
                rfFlags |= (ins->dc.opcode == i_lsl ? RF_SHIFT : 0);
                if (inductionThisLoop(ins->block, tnum) || !GatherExpression(reshapes, tnum, expression, rfFlags))
                    InsertExpression(reshapes, ins->dc.right, expression, ins, rfFlags);
                InsertExpression(reshapes, ins->dc.left, expression, ins, flags1);
                return true;
            }
        }
    }
    return false;
}
static void CreateExpressionLists(std::vector<ReshapeExpression*>& expressions,
                                  std::unordered_map<int, std::shared_ptr<ReshapeList>>& reshapes, int firstTempCount)
{
    int i;
    for (i = 0; i < firstTempCount; i++)
    {
        if (tempInfo[i]->expressionRoot)
        {
            if (tempInfo[i]->enode->sizeFromType < ISZ_FLOAT)
            {
                Loop* lp = InstructionDefinesSsa[i]->block->loopParent;
                /* if the prior of the entry is a critical block,
                 * then don't optimize the loop
                 */
                if (lp->entry->pred && !lp->entry->pred->block->critical)
                {
                    enum i_ops op = InstructionDefinesSsa[i]->dc.opcode;
                    expressions[i]->op = op;
                    (void)GatherExpression(reshapes, i, *expressions[i], 0);
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
static void DistributeMultiplies(std::vector<ReshapeExpression*>& expressions, ReshapeExpression& re, ReshapeList& rl, int tnum)
{
    ReshapeList* replace = nullptr;
    int n = INT_MAX;
    for (auto&& temp : expressions[tnum]->reshapeList)
    {
        if (!temp.second->distributed && temp.second->rpOrder != rl.rpOrder)
        {
            int tnx;
            if (temp.second->im->offset->type != se_tempref)
            {
                replace = temp.second.get();
                break;
            }
            tnx = temp.second->im->offset->sp->i;
            if (expressions[tnx]->count < n)
            {
                n = expressions[tnum]->count;
                replace = temp.second.get();
                // should there be  abreak?
            }
        }
    }
    if (replace)
    {
        replace->distrib = re.reshapeList;
        replace->distributed = true;

        expressions[tnum]->reshapeList.erase(rl.rpOrder);
    }
    else
    {
        rl.distributed = true;
    }
}
static void ApplyDistribution(std::vector<ReshapeExpression*>& expressions, int firstTempCount)
{
    bool done = false;
    while (!done)
    {
        int i;
        ReshapeExpression* next = nullptr;
        ReshapeList* rl = nullptr;
        int n = 0;
        int match = 0;
        for (i = 0; i < firstTempCount; i++)
        {
            if (tempInfo[i]->expressionRoot)
            {
                switch (expressions[i]->op)
                {
                    case i_mul:
                    case i_lsl: {
                        for (auto&& l : expressions[i]->reshapeList)
                        {
                            if (!l.second->distributed)
                            {
                                if (l.second->im->offset->type == se_tempref)
                                {
                                    ReshapeExpression* re = expressions[l.second->im->offset->sp->i];
                                    if (re->count > n)
                                    {
                                        n = re->count;
                                        next = re;
                                        match = i;
                                        rl = l.second.get();
                                    }
                                }
                            }
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
            DistributeMultiplies(expressions, *next, *rl, match);
        }
        else
        {
            done = true;
        }
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
static void CopyExpressionTree(enum i_ops op, Block* b, QUAD* insertBefore, IMODE** iml, IMODE** imr)
{
    if ((*iml) && (*iml)->offset->type == se_tempref)
    {
        int tnum = (*iml)->offset->sp->i;
        QUAD* def = InstructionDefinesSsa[tnum];

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
        QUAD* def = InstructionDefinesSsa[tnum];
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
static IMODE* InsertAddInstruction(Block* b, int size, QUAD* insertBefore, int flagsl, IMODE* iml, int flagsr, IMODE* imr)
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
        InsertInstruction(insertBefore->back, ins);
        in_hash[ins] = ins->ans;
        return ins->ans;
    }
}
static IMODE* InsertMulInstruction(Block* b, int size, QUAD* insertBefore, int flagsl, IMODE* iml, int flagsr, IMODE* imr)
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
        InstructionUsesSsa[ins->ans->offset->sp->i];
        tempInfo[ins->ans->offset->sp->i]->preSSATemp = -1;
        for (auto uses : InstructionUsesSsa[ins->ans->offset->sp->i])
        {
            unmarkPreSSA(uses);
        }
    }
}
static void RewriteAdd(std::vector<ReshapeExpression*>& expressions, Block* b, int tnum)
{
    IMODE *left = expressions[tnum]->lastName, *right = nullptr;
    int size = InstructionDefinesSsa[tnum]->ans->size;
    QUAD* ia;
    int flagsl = 0, flagsr;
    if (b == nullptr)
    {
        ia = InstructionDefinesSsa[tnum];
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
    for (auto& gather : expressions[tnum]->reshapeList)
    {
        if (!(!b || gather.second->im->mode == i_immed ||
              (!variantThisLoop(b, gather.second->im->offset->sp->i) && usedThisLoop(b, gather.second->im->offset->sp->i))))
        {
            if (!gather.second->genned)
            {
                gather.second->genned = true;
                if (!left && (expressions[tnum]->op == i_add || expressions[tnum]->op == i_sub))
                {
                    if (gather.second->rfFlags & RF_NEG)
                    {
                        /* optimization to avoid generating an extraneous i_neg when possible */
#if 0
                            ReshapeList **l = &gather.second.next;
                            while (*l)
                            {
                                if (!((*l)->rfFlags & RF_NEG))
                                {
                                    ReshapeList *t = *l;
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
                    left = gather.second->im;
                    flagsl = gather.second->rfFlags;
                }
                else
                {
                    right = gather.second->im;
                    flagsr = gather.second->rfFlags;
                }
                if (left && right)
                {
                    left = InsertAddInstruction(b, size, ia, flagsl, left, flagsr, right);
                    flagsl = 0;
                }
            }
        }
    }
    expressions[tnum]->lastName = left;
    if (left)
        nm_hash[&tempInfo[tnum]->enode->sp->imvalue] = left;
    ia = InstructionDefinesSsa[tnum];
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
static IMODE* RewriteDistributed(Block* b, int size, IMODE* im, QUAD* ia, std::map<int, std::shared_ptr<ReshapeList>>& distrib,
                                 int rfFlags)
{
    IMODE* total = distrib.begin()->second->lastDistribName;
    std::map<int, std::shared_ptr<ReshapeList>>::iterator it;
    for (it = distrib.begin(); it != distrib.end(); ++it)
    {
        auto gather = (*it).second.get();
        if (!(!b || gather->im->mode == i_immed ||
              (!variantThisLoop(b, gather->im->offset->sp->i) && usedThisLoop(b, gather->im->offset->sp->i))))
        {
            if (!gather->genned)
            {
                IMODE* left = InsertMulInstruction(b, size, ia, rfFlags, im, gather->rfFlags, gather->im);
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
        }
    }
    if (it != distrib.end())
    {
        (*it).second->lastDistribName = total;
        return nullptr;
    }
    return total;
}
static void RewriteMul(std::vector<ReshapeExpression*>& expressions, Block* b, int tnum)
{
    if (expressions[tnum]->reshapeList.size())
    {
        IMODE *left = expressions[tnum]->lastName, *right = nullptr;
        int flagsl = 0, flagsr;
        int size = InstructionDefinesSsa[tnum]->ans->size;
        QUAD* ia;
        if (b == nullptr)
        {
            ia = InstructionDefinesSsa[tnum];
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
        for (auto&& current : expressions[tnum]->reshapeList)
        {
            if (!(!b || current.second->im->mode == i_immed ||
                  (!variantThisLoop(b, current.second->im->offset->sp->i) && usedThisLoop(b, current.second->im->offset->sp->i))))
            {
                if (!current.second->genned && current.second->distrib.empty())
                {
                    current.second->genned = true;
                    if (!left)
                    {
                        left = current.second->im;
                        flagsl = current.second->rfFlags;
                    }
                    else
                    {
                        right = current.second->im;
                        flagsr = current.second->rfFlags;
                    }
                    if (left && right)
                    {
                        left = InsertMulInstruction(b, size, ia, flagsl, left, flagsr, right);
                        flagsl = 0;
                    }
                }
            }
        }
        expressions[tnum]->lastName = left;
        for (auto&& gather : expressions[tnum]->reshapeList)
        {
            if (!(!b || gather.second->im->mode == i_immed ||
                  (!variantThisLoop(b, gather.second->im->offset->sp->i) && usedThisLoop(b, gather.second->im->offset->sp->i))))
            {
                if (!gather.second->distrib.empty() && !gather.second->genned)
                {
                    IMODE* im = RewriteDistributed(b, size, gather.second->im, ia, gather.second->distrib, gather.second->rfFlags);
                    if (im)
                    {
                        if (expressions[tnum]->lastName)
                        {
                            expressions[tnum]->lastName =
                                InsertMulInstruction(b, size, ia, 0, im, gather.second->rfFlags, expressions[tnum]->lastName);
                            gather.second->genned = true;
                        }
                        else
                            expressions[tnum]->lastName = im;
                    }
                }
            }
        }
        if (left)
            nm_hash[&tempInfo[tnum]->enode->sp->imvalue] = left;
        ia = InstructionDefinesSsa[tnum];
        if (ia && left)  // && !inductionThisLoop(ia->block, tnum))
        {
            unmarkPreSSA(ia);
            if (ia->dc.left && (ia->temps & TEMP_LEFT))
            {
                RemoveFromUses(ia, ia->dc.left->offset->sp->i);
            }
            ia->dc.left = expressions[tnum]->lastName;
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
static void RewriteInvariantExpressions(std::vector<ReshapeExpression*>& expressions, Block* b, int firstTempCount)
{
    BLOCKLIST* bl = b->dominates;
    int i;
    decltype(in_hash) old_in_hash(in_hash);
    decltype(nm_hash) old_nm_hash(nm_hash);
    in_hash.clear();
    nm_hash.clear();
    for (i = 0; i < firstTempCount; i++)
    {
        if (tempInfo[i]->expressionRoot && tempInfo[i]->size < ISZ_FLOAT && InstructionDefinesSsa[i]->block == b)
        {
            switch (expressions[i]->op)
            {
                case i_assn:
                case i_phi:
                    break;
                case i_add:
                case i_sub:
                case i_or:
                case i_eor:
                case i_and:
                    RewriteAdd(expressions, b, i);
                    break;
                case i_mul:
                case i_lsl:
                    RewriteMul(expressions, b, i);
                    break;
                default:
                    diag("RewriteExpressions: invalid expression type");
                    break;
            }
        }
    }
    while (bl)
    {
        RewriteInvariantExpressions(expressions, bl->block, firstTempCount);
        bl = bl->next;
    }
    in_hash = std::move(old_in_hash);
    nm_hash = std::move(old_nm_hash);
}
void RewriteInnerExpressions(std::vector<ReshapeExpression*>& expressions, int firstTempCount)
{
    int i;
    for (i = 0; i < firstTempCount; i++)
    {
        if (tempInfo[i]->expressionRoot && tempInfo[i]->size < ISZ_FLOAT)
        {
            switch (expressions[i]->op)
            {
                case i_assn:
                case i_phi:
                    break;
                case i_add:
                case i_sub:
                case i_or:
                case i_eor:
                case i_and:
                    RewriteAdd(expressions, nullptr, i);
                    break;
                case i_mul:
                case i_lsl:
                    RewriteMul(expressions, nullptr, i);
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
    std::vector<ReshapeExpression*> expressions;
    std::unordered_map<int, std::shared_ptr<ReshapeList>> reshapes;

    int cachedTempCount = tempCount;
    CalculateInduction();
    CreateExpressionLists(expressions, reshapes, cachedTempCount);
    ApplyDistribution(expressions, cachedTempCount); /* assumes the lists are already sorted */
    nm_hash.clear();
    in_hash.clear();
    RewriteInvariantExpressions(expressions, blockArray[0], cachedTempCount);
    RewriteInnerExpressions(expressions, cachedTempCount);
#ifdef XXXXX
    DumpInvariants();
#endif
}
}  // namespace Optimizer
