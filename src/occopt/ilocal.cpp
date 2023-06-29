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

/* we are only doing local opts on temp variables.  At this point,
 * any variable that does not have its address taken is also made a temp
 * variable
 */
#include <cstdio>
#include <malloc.h>
#include <cstring>
#include "ioptimizer.h"
#include "beinterfdefs.h"
#include "config.h"
#include "iblock.h"
#include "OptUtils.h"
#include "ildata.h"
#include "optmain.h"
#include "ioptutil.h"
#include "memory.h"

void diag(const char* fmt, ...) {}

namespace Optimizer
{
int tempSize;

BRIGGS_SET* killed;
int tempBottom, nextTemp;

static void CalculateFastcall(SimpleSymbol* funcsp, std::vector<SimpleSymbol*>& functionVariables)
{
    fastcallAlias = 0;
#ifndef CPPTHISCALL
    if (!funcsp->isfastcall)
        return;
#endif

    if (chosenAssembler->arch->fastcallRegCount)
    {
        for (auto sym : functionVariables)
        {
            if (sym->thisPtr)
            {
                fastcallAlias++;
            }
            else
            {
                if (!funcsp->isfastcall)
                    break;
                if (fastcallAlias >= chosenAssembler->arch->fastcallRegCount)
                    break;
                SimpleType* tp = sym->tp;

                if ((tp->type < st_f || (tp->type == st_pointer && tp->btp->type != st_func) || tp->type == st_lref ||
                     tp->type == st_rref) &&
                    (sym->storage_class == scc_parameter))
                {
                    if (sym->tp->size > chosenAssembler->arch->parmwidth)
                        break;
                }
                else
                {
                    break;
                }
                fastcallAlias++;
            }
        }
    }
}
SimpleExpression* anonymousVar(enum e_scc_type storage_class, SimpleType* tp)
{
    static int anonct = 1;
    char buf[256];
    SimpleSymbol* rv = Allocate<SimpleSymbol>();
    //    rv->key = NextSymbolKey();
    if (tp->size == 0 && (tp->type == st_struct || tp->type == st_union))
        tp = tp->sp->tp;
    rv->storage_class = storage_class;
    rv->tp = tp;
    rv->anonymous = true;
    rv->allocate = !Parser::anonymousNotAlloc;
    if (currentFunction)
        rv->i = currentFunction->i;
    my_sprintf(buf, "$anontempSE%d", anonct++);
    rv->name = rv->outputName = litlate(buf);
    rv->align = tp->sp && tp->sp->align ? tp->sp->align : alignFromISZ(tp->sizeFromType);
    cacheTempSymbol(rv);
    SimpleExpression* rve = Allocate<SimpleExpression>();
    rve->type = storage_class == scc_auto || storage_class == scc_parameter ? se_auto : se_global;
    rve->sp = rv;
    return rve;
}
static void MoveLastToPosition(QUAD* bl1)
{
    QUAD* q = intermed_tail;
    q->back->fwd = nullptr;
    intermed_tail = q->back;
    q->block = bl1->block;
    q->fwd = bl1->fwd;
    q->back = bl1;
    q->fwd->back = q;
    q->back->fwd = q;
}
static IMODE* localVar(SimpleType* tp)
{
    SimpleExpression* exp = anonymousVar(scc_auto, tp);
    SimpleSymbol* sym = exp->sp;
    exp->sizeFromType = tp->sizeFromType;
    sym->sizeFromType = tp->sizeFromType;
    sym->anonymous = false;
    IMODE* ap = Allocate<IMODE>();
    sym->imvalue = ap;
    ap->offset = exp;
    ap->mode = i_direct;
    ap->size = exp->sizeFromType;
    return ap;
}
static void renameOneSym(SimpleSymbol* sym, int structret)
{
    SimpleType* tp;
    /* needed for pointer aliasing */
    if (!sym->imvalue && sym->tp->type != st_any && sym->tp->type != st_memberptr &&
        ((sym->tp->type != st_struct &&
        sym->tp->type != st_union) || sym->tp->structuredAlias) && sym->tp->type != st_ellipse && sym->tp->type != st_aggregate)
    {
        if (sym->imaddress)
        {
            IMODE* im = Allocate<IMODE>();
            *im = *sym->imaddress;
            im->size = sym->tp->sizeFromType;
            im->mode = i_direct;
            sym->imvalue = im;
        }
        else if (sym->imind)
        {
            IMODE* im = Allocate<IMODE>();
            *im = *sym->imind->im;
            im->size = ISZ_ADDR;
            im->mode = i_direct;
            sym->imvalue = im;
        }
        else
            sym->imvalue = tempreg(sym->tp->sizeFromType, false);
    }
    tp = sym->tp;

    bool fastcallCandidate = sym->storage_class == scc_parameter && fastcallAlias &&
                             (sym->offset - fastcallAlias * chosenAssembler->arch->parmwidth < chosenAssembler->arch->retblocksize);

    if (!sym->pushedtotemp &&
        (!sym->addressTaken && (cparams.prm_optimize_for_speed || cparams.prm_optimize_for_size) || fastcallCandidate) &&
        !sym->inasm && (!sym->inCatch || fastcallCandidate) &&
        (((chosenAssembler->arch->hasFloatRegs || tp->type < st_f) && tp->type < st_void) ||
         sym->tp->structuredAlias ||
         (tp->type == st_pointer && tp->btp->type != st_func) || tp->type == st_lref || tp->type == st_rref) &&
        (sym->storage_class == scc_auto || sym->storage_class == scc_register || sym->storage_class == scc_parameter) &&
        (!sym->usedasbit || fastcallCandidate) && !sym->tp->isvolatile)
    {
        /* this works because all IMODES refering to the same
         * variable are the same, at least until this point
         * that will change when we start inserting temps
         */
        IMODE* parmName = nullptr;
        SimpleExpression* ep;
        if (sym->imaddress || sym->inCatch)
        {
            ep = anonymousVar(scc_auto, sym->tp->structuredAlias ? sym->tp->structuredAlias : sym->tp);  // fastcall which takes an address
        }
        else
        {
            ep = tempenode();
            ep->sp->tp = sym->tp->structuredAlias ? sym->tp->structuredAlias : sym->tp;
            ep->right = (SimpleExpression*)sym;
            /* marking both the orignal var and the new temp as pushed to temp*/
            sym->pushedtotemp = true;
            ep->sp->pushedtotemp = true;
        }
        sym->allocate = false;

        bool dofastcall = false;

        if (sym->storage_class == scc_parameter)
        {
            if (fastcallAlias)
            {
                // for fastcall, rename the affected parameter nodes with
                // a temp.   It will later be precolored...
                if (sym->offset - (fastcallAlias + structret) * chosenAssembler->arch->parmwidth <
                        chosenAssembler->arch->retblocksize &&
                    (!structret || sym->offset != chosenAssembler->arch->retblocksize))
                {
                    parmName = tempreg(sym->imvalue->size, 0);
                    dofastcall = true;
                }
                else
                {
                    parmName = Allocate<IMODE>();
                    *parmName = *sym->imvalue;
                }
            }
            else
            {
                parmName = Allocate<IMODE>();
                *parmName = *sym->imvalue;
            }
        }
        if (sym->imvalue)
        {
            ep->isvolatile = sym->imvalue->offset->isvolatile;
            ep->isrestrict = sym->imvalue->offset->isrestrict;
            sym->imvalue->offset = ep;
        }
        if (sym->imaddress)
        {
            ep->isvolatile = sym->imvalue->offset->isvolatile;
            ep->isrestrict = sym->imvalue->offset->isrestrict;
            sym->imaddress->offset = ep;
        }
        if (sym->imind)
        {
            IMODELIST* iml = sym->imind;
            ep->isvolatile = sym->imind->im->offset->isvolatile;
            ep->isrestrict = sym->imind->im->offset->isrestrict;
            while (iml)
            {
                iml->im->offset = ep;
                iml = iml->next;
            }
        }
        ep->sp->imvalue = sym->imvalue;
        if (sym->storage_class == scc_parameter && !(chosenAssembler->arch->denyopts & DO_NOLOADSTACK))
        {
            QUAD* q;
            QUAD* bl1 = blockArray[1]->head;
            while (bl1->fwd->dc.opcode == i_line || bl1->fwd->dc.opcode == i_label)
                bl1 = bl1->fwd;
            gen_icode(i_assn, sym->imvalue, parmName, 0);
            if (dofastcall)
            {
                intermed_tail->alwayslive = true;
                intermed_tail->fastcall =
                    -(sym->offset - chosenAssembler->arch->retblocksize) / chosenAssembler->arch->parmwidth - 1 + structret;
            }
            MoveLastToPosition(bl1);
        }
    }
}
static void renameToTemps(std::vector<SimpleSymbol*>& functionVariables)
{
    LIST* lst;
    bool doRename = true;
    CalculateFastcall(currentFunction, functionVariables);
    doRename &= !functionHasAssembly;
    /* if there is a setjmp in the function, no variable gets moved into a reg */
    doRename &= !(setjmp_used);
    doRename &= !(currentFunction->anyTry);
    bool structret = currentFunction->tp->btp->type == st_struct || currentFunction->tp->btp->type == st_union;
    for (auto sym : functionVariables)
    {
        if (doRename)
            renameOneSym(sym, structret);
    }

    for (auto sym : temporarySymbols)
    {
        if (!sym->anonymous && doRename)
        {
            renameOneSym(sym, structret);
        }
    }
}

static int AllocTempOpt(int size1)
{
    int t;
    int i;
    IMODE* rv;
    while (nextTemp < tempCount)
    {
        if (!tempInfo[nextTemp]->inUse)
            break;
        nextTemp++;
    }
    if (nextTemp < tempCount)
    {
        int n = tempCount;
        tempCount = nextTemp;
        rv = tempreg(size1, false);
        tempCount = n;
    }
    else
    {
        rv = tempreg(size1, false);
    }
    t = rv->offset->sp->i;
    if (t >= tempSize)
    {
        TEMP_INFO** temp = oAllocate<TEMP_INFO*>(tempSize + 1000);
        memcpy(temp, tempInfo, sizeof(TEMP_INFO*) * tempSize);
        tempSize += 1000;
        tempInfo = temp;
    }
    if (!tempInfo[t])
    {
        tempInfo[t] = oAllocate<TEMP_INFO>();
    }
    else
    {
        memset(tempInfo[t], 0, sizeof(TEMP_INFO));
    }
    tempInfo[t]->enode = rv->offset;
    nextTemp = t;
    tempInfo[t]->partition = t;
    tempInfo[t]->color = -1;
    tempInfo[t]->inUse = true;
    return t;
}
IMODE* InitTempOpt(int size1, int size2)
{
    int t = AllocTempOpt(size1);
    tempInfo[t]->preSSATemp = -1;
    tempInfo[t]->postSSATemp = -1;
    tempInfo[t]->oldInductionVar = -1;
    if (size2 < 0)
        size2 = -size2;
    if (size2 == ISZ_UINT)
        size2 = chosenAssembler->arch->compatibleIntSize;
    if (size2 == ISZ_ADDR)
    {
        tempInfo[t]->usedAsAddress = true;
        size2 = chosenAssembler->arch->compatibleAddrSize;
    }
    if (size2 >= ISZ_FLOAT)
        tempInfo[t]->usedAsFloat = true;
    tempInfo[t]->size = size2;
    return tempInfo[t]->enode->sp->imvalue;
}
static void InitTempInfo(void)
{
    QUAD* head = intermed_head;
    int i;
    nextTemp = tempBottom = tempCount;
    tempSize = tempCount + 1000;
    tempInfo = oAllocate<TEMP_INFO*>(tempSize);

    for (i = 0; i < tempCount; i++)
    {
        tempInfo[i] = oAllocate<TEMP_INFO>();
        tempInfo[i]->partition = i;
        tempInfo[i]->color = -1;
        tempInfo[i]->inUse = true;
    }
    while (head)
    {
        head->temps = 0;
        if (head->dc.opcode != i_block && !head->ignoreMe && head->dc.opcode != i_passthrough && head->dc.opcode != i_label)
        {
            if (head->ans && (head->ans->mode == i_ind || head->ans->mode == i_direct))
            {
                if (head->ans->offset->type == se_tempref)
                {
                    int tnum = head->ans->offset->sp->i;
                    if (!head->ans->retval)
                        head->temps |= TEMP_ANS;
                    tempInfo[tnum]->enode = head->ans->offset;
                    tempInfo[tnum]->size = head->ans->offset->sp->imvalue->size;
                    if (tempInfo[tnum]->size < 0)
                        tempInfo[tnum]->size = -tempInfo[tnum]->size;
                    if (tempInfo[tnum]->size == ISZ_UINT)
                        tempInfo[tnum]->size = chosenAssembler->arch->compatibleIntSize;
                    if (tempInfo[tnum]->size == ISZ_ADDR)
                    {
                        tempInfo[tnum]->usedAsAddress = true;
                        tempInfo[tnum]->size = chosenAssembler->arch->compatibleAddrSize;
                    }
                    if (tempInfo[tnum]->size >= ISZ_FLOAT)
                        tempInfo[tnum]->usedAsFloat = true;
                }
            }
            if (head->dc.left && (head->dc.left->mode == i_ind || head->dc.left->mode == i_direct))
            {
                if (head->dc.left->offset->type == se_tempref)
                {
                    int tnum = head->dc.left->offset->sp->i;
                    if (!head->dc.left->retval)
                        head->temps |= TEMP_LEFT;
                    tempInfo[tnum]->enode = head->dc.left->offset;
                    tempInfo[tnum]->size = head->dc.left->offset->sp->imvalue->size;
                    if (tempInfo[tnum]->size < 0)
                        tempInfo[tnum]->size = -tempInfo[tnum]->size;
                    if (tempInfo[tnum]->size == ISZ_UINT)
                        tempInfo[tnum]->size = chosenAssembler->arch->compatibleIntSize;
                    if (tempInfo[tnum]->size == ISZ_ADDR)
                    {
                        tempInfo[tnum]->usedAsAddress = true;
                        tempInfo[tnum]->size = chosenAssembler->arch->compatibleAddrSize;
                    }
                    if (tempInfo[tnum]->size >= ISZ_FLOAT)
                        tempInfo[tnum]->usedAsFloat = true;
                }
            }
            if (head->dc.right && (head->dc.right->mode == i_ind || head->dc.right->mode == i_direct))
            {
                if (head->dc.right->offset->type == se_tempref)
                {
                    int tnum = head->dc.right->offset->sp->i;
                    if (!head->dc.right->retval)
                        head->temps |= TEMP_RIGHT;
                    tempInfo[tnum]->enode = head->dc.right->offset;
                    tempInfo[tnum]->size = head->dc.right->offset->sp->imvalue->size;
                    if (tempInfo[tnum]->size < 0)
                        tempInfo[tnum]->size = -tempInfo[tnum]->size;
                    if (tempInfo[tnum]->size == ISZ_UINT)
                        tempInfo[tnum]->size = chosenAssembler->arch->compatibleIntSize;
                    if (tempInfo[tnum]->size == ISZ_ADDR)
                    {
                        tempInfo[tnum]->usedAsAddress = true;
                        tempInfo[tnum]->size = chosenAssembler->arch->compatibleAddrSize;
                    }
                    if (tempInfo[tnum]->size >= ISZ_FLOAT)
                        tempInfo[tnum]->usedAsFloat = true;
                }
            }
        }
        head = head->fwd;
    }
}
void insertDefines(QUAD* head, BLOCK* b, int tnum)
{
    LIST* l = oAllocate<LIST>();
    l->data = (void*)head;
    l->next = tempInfo[tnum]->idefines;
    tempInfo[tnum]->idefines = l;

    l = oAllocate<LIST>();
    l->data = (void*)b;
    l->next = tempInfo[tnum]->bdefines;
    tempInfo[tnum]->bdefines = l;
}
void insertUses(QUAD* head, int dest)
{
    LIST** l = &tempInfo[dest]->iuses;
    while (*l)
    {
        if ((*l)->data == (void*)head)
            return;
        l = &(*l)->next;
    }
    *l = oAllocate<LIST>();
    (*l)->data = (void*)head;
}
void definesInfo(void)
{
    QUAD* head = intermed_head;
    BLOCK* block = nullptr;
    int i;
    for (i = 0; i < tempCount; i++)
    {
        tempInfo[i]->bdefines = nullptr;
        tempInfo[i]->idefines = nullptr;
    }
    while (head)
    {
        if (head->dc.opcode == i_block)
            block = head->block;
        if ((head->temps & TEMP_ANS) && head->ans->mode == i_direct)
        {
            int tnum = head->ans->offset->sp->i;
            insertDefines(head, block, tnum);
        }
        head = head->fwd;
    }
}
void usesInfo(void)
{
    QUAD* head = intermed_head;
    int i;
    for (i = 0; i < tempCount; i++)
    {
        tempInfo[i]->iuses = nullptr;
    }
    while (head)
    {
        if ((head->temps & TEMP_ANS) && head->ans->mode == i_ind)
        {
            if (head->ans->offset)
                insertUses(head, head->ans->offset->sp->i);
            if (head->ans->offset2)
                insertUses(head, head->ans->offset2->sp->i);
        }
        if (head->temps & TEMP_LEFT)
        {
            if (head->dc.left->offset)
                insertUses(head, head->dc.left->offset->sp->i);
            if (head->dc.left->offset2)
                insertUses(head, head->dc.left->offset2->sp->i);
        }
        if (head->temps & TEMP_RIGHT)
        {
            if (head->dc.right->offset)
                insertUses(head, head->dc.right->offset->sp->i);
            if (head->dc.right->offset2)
                insertUses(head, head->dc.right->offset2->sp->i);
        }
        head = head->fwd;
    }
}
void gatherLocalInfo(std::vector<SimpleSymbol*>& functionVariables)
{
    renameToTemps(functionVariables);
    InitTempInfo();
    definesInfo();
}
}  // namespace Optimizer