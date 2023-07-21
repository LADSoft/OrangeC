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

#include "ioptimizer.h"
#include "beinterfdefs.h"
#include "config.h"
#include "ildata.h"
#include "rewritex86.h"
#include "iblock.h"
#include "memory.h"
#include "OptUtils.h"
#include "ilocal.h"

namespace Optimizer
{

static bool CanaryCheckVars(FunctionData* fd, bool strong)
{
    for (auto sym : functionVariables)
        if (!sym->regmode && sym->storage_class != scc_constant &&
                       (sym->storage_class == scc_auto || sym->storage_class == scc_register) && sym->allocate && !sym->anonymous)
        {
            if (strong)
            {
                if (sym->stackProtectStrong || sym->addressTaken)
                    return true;
            }
            else if (sym->stackProtectBasic)
            {
                return true;
            }
        }
    for (auto sym : temporarySymbols)
                // compiler-created variable
        if (sym->storage_class != scc_static && sym->storage_class != scc_constant && !sym->stackblock)
        {
            if (strong)
            {
                if (sym->stackProtectStrong || sym->addressTaken)
                    return true;
            }
            else if (sym->stackProtectBasic)
            {
                return true;
            }
       }
    return false;
}
bool HasCanary(FunctionData* fd)
{
    bool hasCanary = false;
    if (Optimizer::cparams.prm_stackprotect & STACK_PROTECT_WITH_CANARY)
    {
        if (Optimizer::cparams.prm_stackprotect & STACK_PROTECT_ALL)
            hasCanary = true;
        else 
        {
            if (Optimizer::cparams.prm_stackprotect & STACK_PROTECT_EXPLICIT)
                hasCanary = fd->name->stackProtectExplicit;
            if (Optimizer::cparams.prm_stackprotect & STACK_PROTECT_STRONG)
                hasCanary |= fd->name->allocaUsed | CanaryCheckVars(fd, true);
            if (Optimizer::cparams.prm_stackprotect & STACK_PROTECT_BASIC)
                hasCanary |= fd->name->allocaUsed | CanaryCheckVars(fd, false);
        }
    }
    return hasCanary;
}
static auto insert_funcname(SimpleSymbol *func)
{
    int label = nextLabel++;
    put_label(label);
    putstring(func->outputName, strlen(func->outputName));
    genbyte(0);
    auto result = Allocate<IMODE>();
    result->mode =  i_immed;
    result->offset = Allocate<SimpleExpression>();
    result->offset->type = se_labcon;
    result->offset->i = label;
    return result;
}
void CreateCanaryStubs(QUAD* head, QUAD* tail, SimpleSymbol* func)
{
    auto it = head;
    while (it && it->dc.opcode != i_prologue)
        it = it->fwd;
    if (it)
    {        
        auto init = rwSetSymbol("___canary_init", true);
        auto check = rwSetSymbol("___canary_check", true);

        QUAD* ins = Allocate<QUAD>();
        ins->dc.opcode = i_gosub;
        ins->dc.left = init;
        InsertInstruction(it->fwd->fwd, ins);
        insert_nullparmadj(it->fwd->fwd->fwd, 0);

        it = tail;
        while (it && it->dc.opcode != i_epilogue)
           it = it->back;
        if (it)
        {
            ins = Allocate<QUAD>();
            ins->dc.opcode = i_parm;
            ins->dc.left = insert_funcname(func);
            InsertInstruction(it->back, ins);
            ins = Allocate<QUAD>();
            ins->dc.opcode = i_gosub;
            ins->dc.left = check;
            InsertInstruction(it->back, ins);
            insert_nullparmadj(it->back, 0);
        }
    }
}
}