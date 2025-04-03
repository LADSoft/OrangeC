/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

#include "ioptimizer.h"
#include "beinterfdefs.h"
#include "config.h"
#include "ildata.h"
#include "rewritex86.h"
#include "iblock.h"
#include "memory.h"
#include "OptUtils.h"
#include "ilocal.h"
#include "irc.h"
#include "optmain.h"

namespace Optimizer
{
static std::unordered_map<std::string, int> nameMap;
static bool overflowInitted;
void localprotect_init() { nameMap.clear(); }
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
static auto InsertName(const char* name)
{
    int label;
    auto it = nameMap.find(name);
    if (it != nameMap.end())
    {
        label = it->second;
    }
    else
    {
        label = nextLabel++;
        put_label(label);
        putstring(name, strlen(name));
        genbyte(0);
        nameMap[name] = label;
    }
    auto result = Allocate<IMODE>();
    result->mode = i_immed;
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

        IMODE* name = InsertName(func->outputName);
        QUAD* ins = Allocate<QUAD>();
        ins->dc.opcode = i_gosub;
        ins->dc.left = init;
        InsertInstruction(it, ins);
        insert_nullparmadj(it->fwd, 0);

        it = tail;
        while (it && it->dc.opcode != i_epilogue)
            it = it->back;
        if (it)
        {
            ins = Allocate<QUAD>();
            ins->dc.opcode = i_parm;
            ins->dc.left = name;
            InsertInstruction(it->back, ins);
            ins = Allocate<QUAD>();
            ins->dc.opcode = i_gosub;
            ins->dc.left = check;
            InsertInstruction(it->back, ins);
            insert_nullparmadj(it->back, 0);
        }
    }
}
static void OverflowInit(QUAD* head)
{
    overflowInitted = true;
    auto it = head;
    while (it && it->dc.opcode != i_prologue)
        it = it->fwd;
    auto init = rwSetSymbol("___buffer_overflow_init", true);

    QUAD* ins = Allocate<QUAD>();
    ins->dc.opcode = i_parm;
    InsertInstruction(it, ins);
    ins->dc.left = make_immed(ISZ_UINT, -lc_maxauto);
    ins = Allocate<QUAD>();
    ins->dc.opcode = i_gosub;
    ins->dc.left = init;
    InsertInstruction(it->fwd, ins);
    insert_nullparmadj(it->fwd->fwd, 0);
}

void CreateUninitializedVariableStubs(QUAD* head, QUAD* tail)
{
    IMODE* check = nullptr;
    std::unordered_map<QUAD*, int> loadHold;
    if (Optimizer::cparams.prm_stackprotect & STACK_UNINIT_VARIABLE)
    {
        for (QUAD* it = head; it; it = it->fwd)
        {
            if (it->runtimeData && !it->runtimeData->asStore)
            {
                InsertName(it->runtimeData->fileName);
                InsertName(it->runtimeData->varName);
                InsertName(reinterpret_cast<SimpleSymbol*>(it->runtimeData->runtimeSym)->outputName);
                loadHold[it] = nextLabel++;
            }
        }
        for (QUAD* it = head; it; it = it->fwd)
        {
            if (it->runtimeData && !it->runtimeData->asStore)
            {
                SimpleSymbol* sym = reinterpret_cast<SimpleSymbol*>(it->runtimeData->runtimeSym);
                int label = loadHold[it];
                put_label(label);
                genint(sym->offset);
                gen_labref(nameMap[it->runtimeData->varName]);
                gen_labref(nameMap[it->runtimeData->fileName]);
                genint(it->runtimeData->lineno);
            }
        }
        for (QUAD* it = head; it; it = it->fwd)
        {
            if (it->runtimeData)
            {
                if (it->runtimeData->asStore)
                {
                    IMODE* ans = Allocate<IMODE>();
                    ans->mode = i_direct;
                    ans->size = ISZ_ADDR;
                    ans->offset = Allocate<SimpleExpression>();
                    ans->offset->type = se_auto;
                    ans->offset->sp = reinterpret_cast<SimpleSymbol*>(it->runtimeData->runtimeSym);
                    QUAD* ins = Allocate<QUAD>();
                    ins->dc.opcode = i_assn;
                    ins->ans = ans;
                    ins->dc.left = make_immed(ISZ_ADDR, 0);
                    InsertInstruction(it, ins);
                    it = it->fwd;
                }
                else
                {
                    if (!check)
                    {
                        if (!overflowInitted)
                            OverflowInit(head);
                        check = rwSetSymbol("___uninit_var_check", true);
                    }
                    IMODE* arg = Allocate<IMODE>();
                    arg->mode = i_immed;
                    arg->offset = Allocate<SimpleExpression>();
                    arg->offset->type = se_labcon;
                    arg->offset->i = loadHold[it];
                    QUAD* ins = Allocate<QUAD>();
                    ins->dc.opcode = i_parm;
                    ins->dc.left = arg;
                    it = it->fwd;
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
}
auto CreateOverflowTable()
{
    InsertName(currentFunction->outputName);
    for (auto sym : functionVariables)
    {
        if (!sym->regmode && sym->storage_class != scc_constant &&
            (sym->storage_class == scc_auto || sym->storage_class == scc_register) && sym->allocate && !sym->anonymous)
        {
            if (sym->tp->isarray)
            {
                InsertName(sym->outputName);
            }
        }
    }
    int label = nextLabel++;
    auto result = Allocate<IMODE>();
    result->mode = i_immed;
    result->offset = Allocate<SimpleExpression>();
    result->offset->type = se_labcon;
    result->offset->i = label;
    put_label(label);
    for (auto sym : functionVariables)
    {
        if (!sym->regmode && sym->storage_class != scc_constant &&
            (sym->storage_class == scc_auto || sym->storage_class == scc_register) && sym->allocate && !sym->anonymous)
        {
            if (sym->tp->isarray || sym->tp->type == st_struct || sym->tp->type == st_union || sym->tp->type == st_class)
            {
                genint(sym->offset + sym->tp->size);
                gen_labref(nameMap[currentFunction->outputName]);
                gen_labref(nameMap[sym->outputName]);
            }
        }
    }
    genint(0);
    return result;
}
void CreateBufferOverflowStubs(QUAD* head, QUAD* tail)
{
    overflowInitted = false;
    if (Optimizer::cparams.prm_stackprotect & STACK_OBJECT_OVERFLOW)
    {
        bool found = false;
        // only considering named variables
        for (auto sym : functionVariables)
        {
            if (!sym->regmode && sym->storage_class != scc_constant &&
                (sym->storage_class == scc_auto || sym->storage_class == scc_register) && sym->allocate && !sym->anonymous)
            {
                if (sym->tp->isarray || sym->tp->type == st_struct || sym->tp->type == st_union || sym->tp->type == st_class)
                {
                    found = true;
                }
            }
        }
        if (found)
        {
            OverflowInit(head);
            auto check = rwSetSymbol("___buffer_overflow_check", true);

            auto it = tail;
            while (it && it->dc.opcode != i_epilogue)
                it = it->back;
            QUAD* ins = Allocate<QUAD>();
            ins->dc.opcode = i_parm;
            ins->dc.left = CreateOverflowTable();
            InsertInstruction(it->back, ins);
            ins = Allocate<QUAD>();
            ins->dc.opcode = i_gosub;
            ins->dc.left = check;
            InsertInstruction(it->back, ins);
            insert_nullparmadj(it->back, 0);
        }
    }
}
}  // namespace Optimizer