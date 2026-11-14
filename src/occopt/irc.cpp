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
#include <cstdlib>
#include <climits>
#include <stack>
#include <algorithm>
#include "ioptimizer.h"
#include "beinterfdefs.h"
#include <vector>
#include <deque>
#include <map>
#include "irc.h"
#include "config.h"
#include "iblock.h"
#include "igcse.h"
#include "OptUtils.h"
#include "ildata.h"
#include "memory.h"
#include "ilive.h"
#include "optmain.h"
#include "irewrite.h"
#include "iconfl.h"
#include "ilocal.h"
#include <array>
#include <mutex>
#include <algorithm>
#include "beinterfdefs.h"
#include "configx86.h"

#define MAX_INTERNAL_REGS 256

#define REG_MAX (chosenAssembler->arch->registerCount)

/* implements 'iterated register coalescing', George & Appel
 * and includes the recommended change to commit the first set of coalescings
 * with the pathological case, priority, and spill modifications given in
 * 'A new MLRISC Register Alloctor', Leung & George
 * this is an adaptation of the psuedocode they give.  The main departure is
 * to use Morgan's representation of the conflict(interference) graph
 * instead of using adjacency lists directly as in the psuedocode.
 *
 * We also add in Smith, et.al 'A Generalized Algorithm for Graph-Coloring
 *    Register Allocation', which handles allocating multiple register classes
 * simultaneously.  This allows us to easily handle various architecture
 * irregularities by defining an architecture via structures
 *
 * note: previously to the time this module runs, we universally need IMODE values
 * that are equivalent to be the same object.  Once we hit this module
 * we are done with the optimizations that require that assumption to hold so we
 * can start replacing imodes wholesale without regard for it.
 */

namespace Optimizer
{
int maxAddr;
int lc_maxauto;
std::vector<char> colors;
std::vector<bool> precolored;

static std::deque<int> tempStack;
static unsigned long long regmask;
static int localspill, spillcount;
static std::deque<int> simplifyWorkList;
static SimpleBitSet coalescedNodes;
static SimpleBitSet stackedTemps;
static SimpleBitSet adjacent, adjacent1;
static std::set<int> workingMoves;
static std::unordered_set<int> activeMoves;
static std::unordered_set<int> coalescedMoves;
static std::set<int> tempMoves[2];
static std::vector<QUAD*> instructionList;
static std::list<int> freezeWorkList;
static std::list<int> spillWorkList;
static std::set<int> spilledNodes;
static std::unordered_map<int, int> hiMoves;
std::map<int, std::set<int>> tempWorkingMoves;

static std::vector<std::vector<unsigned>> worstCase[2];
static std::vector<ARCH_REGCLASS*> regClasses;
static std::vector<ARCH_REGVERTEX*> vertexes;

static std::vector<ARCH_REGCLASS*> tempRegClasses;
static std::vector<int> tempRegCounts;
static std::vector<int> squeezes;
static std::vector<std::vector<int>> rawSqueezes;
static std::vector<int> degrees;
static std::map<int, int> spillCosts;
static std::vector<bool> doGlobals;
static std::map<int, IMODE*> spilledVars;
static std::map<int, bool> spilling;
static std::map<int, bool> spilled;
static std::map<int, bool> triedSpill;
static std::map<int, bool> spillTemp;

static int instructionCount;

static int classCount, vertexCount;

static int accesses;
inline void PUSH(int t)
{
    tempStack.push_front(t);
    stackedTemps.setbit(t);
}
inline int POP()
{
    if (tempStack.empty())
    {
        return -1;
    }
    else
    {
        auto rv = tempStack.front();
        tempStack.pop_front();
        return rv;
    }
}

static void InitRegAliases(ARCH_REGDESC* desc)
{
    for (int i = 0; i < REG_MAX; i++)
    {
        desc[i].aliasRegs.push_back(0);
        std::copy_backward(desc[i].aliasRegs.begin(), desc[i].aliasRegs.end() - 1, desc[i].aliasRegs.end());
        desc[i].aliasRegs[0] = i;
    }
}
static void InitTree(ARCH_REGVERTEX* parent, ARCH_REGVERTEX* child)
{
    child->parent = parent;
    child->index = vertexCount++;
    for (auto c = child->cls; c; c = c->next)
    {
        c->vertex = child->index;
        c->index = classCount++;
    }
    if (child->left)
        InitTree(child, child->left);
    if (child->right)
        InitTree(child, child->right);
}
static void LoadVertexes(ARCH_REGVERTEX* child)
{
    for (auto c = child->cls; c; c = c->next)
    {
        regClasses[c->index] = c;
    }
    vertexes[child->index] = child;
    if (child->left)
        LoadVertexes(child->left);
    if (child->right)
        LoadVertexes(child->right);
}
static void LoadWorstCase(int useNoStackFrame)
{
    worstCase[useNoStackFrame].resize(classCount);
    for (auto&& t : worstCase[useNoStackFrame])
        t.resize(classCount);

    for (int i = 0; i < classCount; i++)
    {
        for (int j = 0; j < classCount; j++)
        {
            int max = 0;
            for (int k = 0; k < regClasses[j]->regCount[useNoStackFrame]; k++)
            {
                int count = 0;
                std::vector<int> temp;
                std::vector<int> reg1(regClasses[i]->regCount[useNoStackFrame]);
                std::copy(regClasses[i]->regs.begin(), regClasses[i]->regs.begin() + regClasses[i]->regCount[useNoStackFrame],
                          reg1.begin());
                std::sort(reg1.begin(), reg1.end());
                std::vector<int> reg2(chosenAssembler->arch->regNames[regClasses[j]->regs[k]].aliasRegs);
                std::sort(reg2.begin(), reg2.end());
                std::set_intersection(reg1.begin(), reg1.end(), reg2.begin(), reg2.end(), std::inserter(temp, temp.begin()));
                count = temp.size();
                if (count > max)
                    max = count;
            }
            worstCase[useNoStackFrame][i][j] = max;
        }
    }
}
static void LoadAliases(int useNoStackFrame, ARCH_REGVERTEX* v)
{
    if (v->left)
        LoadAliases(useNoStackFrame, v->left);
    if (v->right)
        LoadAliases(useNoStackFrame, v->right);
    std::set<int> result;

    if (v->left)
    {
        std::set_union(v->aliasRegs[useNoStackFrame].begin(), v->aliasRegs[useNoStackFrame].end(),
                       v->left->aliasRegs[useNoStackFrame].begin(), v->left->aliasRegs[useNoStackFrame].end(),
                       std::inserter(result, result.begin()));
        v->aliasRegs[useNoStackFrame] = std::move(result);
    }
    if (v->right)
    {
        std::set_union(v->aliasRegs[useNoStackFrame].begin(), v->aliasRegs[useNoStackFrame].end(),
                       v->right->aliasRegs[useNoStackFrame].begin(), v->right->aliasRegs[useNoStackFrame].end(),
                       std::inserter(result, result.begin()));
        v->aliasRegs[useNoStackFrame] = std::move(result);
    }
    for (auto c = v->cls; c; c = c->next)
    {
        for (int i = 0; i < c->regCount[useNoStackFrame]; ++i)
        {
            std::vector<int> incomingRegs(chosenAssembler->arch->regNames[c->regs[i]].aliasRegs);
            std::sort(incomingRegs.begin(), incomingRegs.end());
            std::set_union(c->aliasRegs[useNoStackFrame].begin(), c->aliasRegs[useNoStackFrame].end(), incomingRegs.begin(),
                           incomingRegs.end(), std::inserter(result, result.begin()));
            c->aliasRegs[useNoStackFrame] = std::move(result);
        }
        std::set_union(v->aliasRegs[useNoStackFrame].begin(), v->aliasRegs[useNoStackFrame].end(),
                       c->aliasRegs[useNoStackFrame].begin(), c->aliasRegs[useNoStackFrame].end(),
                       std::inserter(result, result.begin()));
        v->aliasRegs[useNoStackFrame] = std::move(result);
    }
}
void LoadSaturationBounds(int useNoStackFrame)
{
    for (int i = 0; i < classCount; i++)
    {
        regClasses[i]->saturationBound[useNoStackFrame].resize(vertexCount);
        for (int j = 0; j < vertexCount; j++)
        {
            int rv = 0;
            std::vector<int> temp;
            std::vector<int> reg1(regClasses[i]->regs);
            reg1.resize(regClasses[i]->regCount[useNoStackFrame]);
            std::sort(reg1.begin(), reg1.end());
            std::set_intersection(reg1.begin(), reg1.end(), vertexes[j]->aliasRegs[useNoStackFrame].begin(),
                                  vertexes[j]->aliasRegs[useNoStackFrame].end(), std::inserter(temp, temp.begin()));
            regClasses[i]->saturationBound[useNoStackFrame][j] = temp.size();
        }
    }
}
}  // namespace Optimizer
void regInit(void)
{
    using namespace Optimizer;
    classCount = vertexCount = 0;
    InitRegAliases(chosenAssembler->arch->regNames);

    InitTree(nullptr, chosenAssembler->arch->regRoot);

    vertexes.resize(vertexCount);
    regClasses.resize(classCount);

    LoadVertexes(chosenAssembler->arch->regRoot);

    LoadWorstCase(0);
    LoadWorstCase(1);
    LoadAliases(0, chosenAssembler->arch->regRoot);
    LoadAliases(1, chosenAssembler->arch->regRoot);
    LoadSaturationBounds(0);
    LoadSaturationBounds(1);
}
namespace Optimizer
{
void alloc_init(void) { spillcount = 0; }
static void ScanForVar(SimpleExpression* exp)
{
    if (exp)
    {
        if (exp->type == se_auto)
        {
            cacheTempSymbol(exp->sp);
        }
        else if (exp->type != se_tempref)
        {
            ScanForVar(exp->left);
            ScanForVar(exp->right);
        }
    }
}
static void ScanForAnonymousVars(void)
{
    static QUAD* head;
    head = intermed_head;
    while (head)
    {
        if (head->dc.opcode != i_block && head->dc.opcode != i_blockend && head->dc.opcode != i_dbgblock &&
            head->dc.opcode != i_dbgblockend && head->dc.opcode != i_var && head->dc.opcode != i_label &&
            head->dc.opcode != i_line && head->dc.opcode != i_inlineasm)
        {
            if (head->ans)
            {
                ScanForVar(head->ans->offset);
                ScanForVar(head->ans->offset2);
                ScanForVar(head->ans->offset3);
            }
            if (head->dc.left)
            {
                ScanForVar(head->dc.left->offset);
                ScanForVar(head->dc.left->offset2);
                ScanForVar(head->dc.left->offset3);
            }
            if (head->dc.right)
            {
                ScanForVar(head->dc.right->offset);
                ScanForVar(head->dc.right->offset2);
                ScanForVar(head->dc.right->offset3);
            }
        }
        head = head->fwd;
    }
}
static void ScanAutos(SimpleExpression* e)
{
    if (e)
    {
        if (e->type == se_auto)
            e->sp->allocate = true;
        else if (e->type == se_add || e->type == se_sub)
        {
            ScanAutos(e->left);
            ScanAutos(e->right);
        }
    }
}
void AllocateStackSpace(int begin)
/*
 * This allocates space for local variables
 * we do this AFTER the register optimization so that we can
 * avoid allocating space on the stack twice
 */
{
    int maxlvl = -1;
    /* base assignments are at level 0, function body starts at level 1*/
    int max;
    ScanForAnonymousVars();

    for (auto sym : functionVariables)
        if (sym->i >= maxlvl && sym->storage_class != scc_constant)
            maxlvl = sym->i + 1;
    for (auto sym : temporarySymbols)
        if (sym->i >= maxlvl && sym->storage_class != scc_constant)
            maxlvl = sym->i + 1;
    if (maxlvl == -1)
        return;
    std::vector<std::deque<SimpleSymbol*>> queue;
    queue.resize(maxlvl);
    int oldlvl = -1;
    std::map<SimpleSymbol*, int> modes;
    for (auto sym : functionVariables)
    {
        sym->allocate = sym->forceAllocate | sym->inasm;
        if (sym->storage_class != scc_constant)
        {
            int lvl = sym->i;
            queue[lvl].push_back(sym);
            modes[sym] = sym->anonymous ? 2 : (lvl > oldlvl);
        }
    }
    for (auto sym : temporarySymbols)
    {
        sym->allocate = sym->forceAllocate | sym->inasm;
        if (sym->storage_class != scc_constant)
        {
            int lvl = sym->i;
            queue[lvl].push_back(sym);
            modes[sym] = 2;
        }
    }
    // get rid of any variables we don't care about in the output file...
    for (auto head = intermed_head; head; head = head->fwd)
    {
        if (head->dc.opcode != i_block && !head->ignoreMe && head->dc.opcode != i_inlineasm && head->dc.opcode != i_label)
        {
            if (head->ans)
            {
                ScanAutos(head->ans->offset);
                ScanAutos(head->ans->offset2);
                ScanAutos(head->ans->offset3);
            }
            if (head->dc.left)
            {
                ScanAutos(head->dc.left->offset);
                ScanAutos(head->dc.left->offset2);
                ScanAutos(head->dc.left->offset3);
            }
            if (head->dc.right)
            {
                ScanAutos(head->dc.right->offset);
                ScanAutos(head->dc.right->offset2);
                ScanAutos(head->dc.right->offset3);
            }
        }
    }
    bool show = false;
    lc_maxauto = max = begin;

    for (auto&& dq : queue)
    {
        int oldauto = lc_maxauto;
        // we dont do stack compression if we are doing runtime checks
        // so the stack compression got completely broken at some point in the past, the if statement was never true
        // im commenting it out for now since so much testing was done but leaving it in causes static analysis flags.
        //        if (!Optimizer::cparams.prm_stackprotect & (STACK_OBJECT_OVERFLOW | STACK_UNINIT_VARIABLE))
        //            lc_maxauto = max;
        for (auto&& sym : dq)
        {
            if (modes[sym] & 1)  // overlay?
                lc_maxauto = oldauto;
            bool doit;
            if (modes[sym] & 2)
            {
                // compiler-created variable
                doit = sym->storage_class != scc_static && sym->storage_class != scc_constant && !sym->stackblock;
            }
            else
            {
                // declared variable
                doit = !sym->regmode && sym->storage_class != scc_constant &&
                       (sym->storage_class == scc_auto || sym->storage_class == scc_register) && sym->allocate && !sym->anonymous;
            }
            if (sym->allocate && sym->offset == 0 && sym->tp->size)
            {
                int val;
                lc_maxauto += sym->tp->size;
                if ((sym->tp->isarray || sym->tp->type == st_struct || sym->tp->type == st_union || sym->tp->type == st_class) &&
                    (Optimizer::cparams.prm_stackprotect & STACK_OBJECT_OVERFLOW))
                {
                    // make a canary on each such variables, that will be checked for buffer overflow...
                    lc_maxauto += chosenAssembler->arch->type_sizes->a_addr;
                }
                if (sym->tp->isatomic && needsAtomicLockFromISZ(sym->tp->sizeFromType))
                {
                    lc_maxauto += ATOMIC_FLAG_SPACE;
                }
                val = lc_maxauto % sym->align;
                if (val != 0)
                    lc_maxauto += sym->align - val;
                if (sym->tp->type == st_struct || sym->tp->type == st_union || sym->tp->type == st_class)
                    if (lc_maxauto % chosenAssembler->arch->stackalign)
                        lc_maxauto += chosenAssembler->arch->stackalign - lc_maxauto % chosenAssembler->arch->stackalign;
                sym->offset = -lc_maxauto;
                if (lc_maxauto > max)
                    max = lc_maxauto;
            }
            oldauto = max;
        }
    }
    for (auto sym : temporarySymbols)
    {
        if (sym->stackblock)
        {
            sym->offset -= max;
        }
    }

    lc_maxauto = max;
    if (cparams.prm_stackalign)
    {
        int n = lc_maxauto + chosenAssembler->arch->retblocksize;
        if (n % cparams.prm_stackalign)
            lc_maxauto += cparams.prm_stackalign - n % cparams.prm_stackalign;
    }
    else
    {
        if (lc_maxauto % chosenAssembler->arch->stackalign)
            lc_maxauto += chosenAssembler->arch->stackalign - lc_maxauto % chosenAssembler->arch->stackalign;
    }
}
void FillInPrologue(QUAD* head, SimpleSymbol* funcsp)
{
    IMODE *ip, *ip1;
    if ((cparams.prm_cplusplus && cparams.prm_xcept) || lc_maxauto || funcsp->hasParams)
        regmask |= FRAME_FLAG_NEEDS_FRAME;
    if (regmask || lc_maxauto || (funcsp->syms && ((SimpleSymbol*)funcsp->syms->data)->tp->type != st_void))
        regmask |= FRAME_FLAG_NEEDS_FRAME;
    while (head->dc.opcode != i_prologue)
    {
        head = head->fwd;
    }
    head->dc.left = ip = make_immed(ISZ_UINT, regmask);
    head->dc.right = ip1 = make_immed(ISZ_UINT, lc_maxauto);
    while (head && head->dc.opcode != i_epilogue)
    {
        head = head->fwd;
    }
    if (head)
    {
        head->dc.left = ip;
        head->dc.right = ip1;
    }
}
static SimpleExpression* spillVar(enum e_scc_type storage_class, SimpleType* tp)
{
    SimpleExpression* rv = anonymousVar(storage_class, tp);
    SimpleSymbol* sym = rv->sp;
    rv->sizeFromType = tp->sizeFromType;
    sym->sizeFromType = tp->sizeFromType;
    sym->spillVar = true;
    sym->anonymous = false;
    return rv;
}
static IMODE* make_ioffset(SimpleExpression* exp)
{
    IMODE* ap;
    SimpleSymbol* sym = varsp(exp);
    if (sym && sym->imvalue && sym->imvalue->size == exp->sizeFromType)
        return sym->imvalue;
    ap = Allocate<IMODE>();
    ap->offset = exp;
    ap->mode = i_direct;
    ap->size = exp->sizeFromType;
    if (sym && !sym->imvalue)
        sym->imvalue = ap;
    return ap;
}
static void GetSpillVar(int i)
{
    SimpleExpression* exp = nullptr;
    SimpleType* tp = tempInfo[i]->enode->sp->tp;
    if (tempInfo[i]->enode->sp->invartemp)
    {
        auto i1 = tempInfo[i]->enode->sp->i;
        if (InstructionDefinesSsa[i1])
        {
            auto im0 = InstructionDefinesSsa[i1]->dc.left;
            if (im0->mode == i_direct && im0->offset->type == se_tempref && im0->offset->sp->loadTemp)
            {
                auto sp1 = im0->offset->sp;
                if (InstructionDefinesSsa[sp1->i])
                {
                    auto im1 = InstructionDefinesSsa[sp1->i]->dc.left;
                    if (im1->mode == i_direct &&
                        (im1->offset->type == se_global || im1->offset->type == se_auto || im1->offset->type == se_absolute))
                    {
                        exp = im1->offset;
                    }
                }
            }
        }
    }
    if (tp->type == st_pointer && tp->isarray)
    {
        // if we get here with an array type, it is a pointer to an array which was in an argument
        tp = Allocate<SimpleType>();
        tp->type = st_pointer;
        tp->sizeFromType = ISZ_ADDR;
        tp->size = sizeFromISZ(ISZ_ADDR);
    }
    if (!exp)
        exp = spillVar(scc_auto, tp);
    spilledVars[i] = make_ioffset(exp);
    spilledVars[i]->offset->sp->spillVar = true;
    spilledVars[i]->size = tempInfo[i]->enode->sp->imvalue->size;
}
static void CopyLocalColors(void)
{
    QUAD* head = intermed_head;
    regmask = 0;
    while (head)
    {
        if (head->dc.opcode == i_inlineasm)
        {
            for (int i = 0; i < head->assemblyRegCount; i++)
                if (head->assemblyRegs[i] != 255)
                    regmask |= chosenAssembler->arch->regNames[head->assemblyRegs[i] - 1].pushMask;
        }
        else if (head->dc.opcode != i_block && !head->ignoreMe && head->dc.opcode != i_inlineasm && head->dc.opcode != i_label)
        {
            if ((head->temps & TEMP_ANS) || (head->ans && head->ans->retval))
            {
                if (head->ans->offset)
                {
                    head->ansColor = colors[head->ans->offset->sp->i];
                    if (head->ansColor >= 0)
                        regmask |= chosenAssembler->arch->regNames[head->ansColor].pushMask;
                }
                if (head->ans->offset2)
                {
                    head->scaleColor = colors[head->ans->offset2->sp->i];
                    if (head->scaleColor >= 0)
                        regmask |= chosenAssembler->arch->regNames[head->scaleColor].pushMask;
                }
            }
            if ((head->temps & TEMP_LEFT) || (head->dc.left && head->dc.left->retval))
            {
                if (head->dc.left->offset)
                {
                    head->leftColor = colors[head->dc.left->offset->sp->i];
                    if (head->leftColor >= 0)
                        regmask |= chosenAssembler->arch->regNames[head->leftColor].pushMask;
                }
                if (head->dc.left->offset2)
                {
                    head->scaleColor = colors[head->dc.left->offset2->sp->i];
                    if (head->scaleColor >= 0)
                        regmask |= chosenAssembler->arch->regNames[head->scaleColor].pushMask;
                }
            }
            if ((head->temps & TEMP_RIGHT) || (head->dc.right && head->dc.right->retval))
            {
                if (head->dc.right->offset)
                {
                    head->rightColor = colors[head->dc.right->offset->sp->i];
                    if (head->rightColor >= 0)
                        regmask |= chosenAssembler->arch->regNames[head->rightColor].pushMask;
                }
                if (head->dc.right->offset2)
                {
                    head->scaleColor = colors[head->dc.right->offset2->sp->i];
                    if (head->scaleColor >= 0)
                        regmask |= chosenAssembler->arch->regNames[head->scaleColor].pushMask;
                }
            }
        }
        head = head->fwd;
    }
}
int SqueezeChange(int temp, int t, int delta)
{
    ARCH_REGVERTEX* v = vertexes[t];
    do
    {
        int alpha = rawSqueezes[temp][v->index];

        rawSqueezes[temp][v->index] += delta;

        if (delta >= 0)
            delta = imax(0, imin(delta, tempRegClasses[temp]->saturationBound[noStackFrame][v->index] - alpha));
        else
            delta = imin(0, imax(delta, delta - (tempRegClasses[temp]->saturationBound[noStackFrame][v->index] - alpha)));

        v = v->parent;
    } while (delta && v);
    return delta;
}
void SqueezeInit(void)
{

    int i;
    for (i = 0; i < tempCount; i++)
        if (doGlobals[i] && !precolored[i])
        {
            squeezes[i] = 0;
            if (rawSqueezes[i].size())
                std::fill(rawSqueezes[i].begin(), rawSqueezes[i].end(), 0);
            else
                rawSqueezes[i].resize(vertexCount);
            degrees[i] = 0;
        }
    for (i = 0; i < tempCount; i++)
    {
        if (doGlobals[i] && !precolored[i])
        {
            std::set<int> regs;
            int n;
            EvaluateConflicts(i, [i, &regs](int n) {
                if (!precolored[n])
                {
                    squeezes[i] += SqueezeChange(i, tempRegClasses[n]->vertex,
                                                 +worstCase[noStackFrame][tempRegClasses[i]->index][tempRegClasses[n]->index]);
                    degrees[i]++;
                }
                else
                {
                    regs.insert(colors[n]);
                }
            });
            tempRegCounts[i] = tempRegClasses[i]->regCount[noStackFrame];
            for (auto j : regs)
            {
                if (tempRegClasses[i]->aliasRegs[noStackFrame].find(j) != tempRegClasses[i]->aliasRegs[noStackFrame].end())
                {
                    tempRegCounts[i]--;
                }
            }
        }
    }
}
static void CalculateFunctionFlags(void)
{
    SimpleBitSet exposed(tempCount);
    int i, j;
    for (j = 0; j < tempCount; j++)
        tempInfo[j]->liveAcrossFunctionCall = false;
    for (i = 0; i < blockCount; i++)
    {
        struct Block* blk = blockArray[i];
        if (blk && blk->head)
        {
            QUAD* tail = blk->tail;
            int j, k;
            exposed = liveOutTemps[blk->blocknum];
            while (tail != blk->head)
            {
                if (tail->dc.opcode == i_gosub || tail->dc.opcode == i_cmpxchgweak || tail->dc.opcode == i_cmpxchgstrong)
                {
                    for (j = 0; j < exposed.size(); j++)
                        if (exposed.isset(j))
                            tempInfo[j]->liveAcrossFunctionCall = true;
                }
                if (tail->temps & TEMP_ANS)
                {
                    if (tail->ans->mode == i_direct)
                    {
                        int tnum = tail->ans->offset->sp->i;
                        exposed.clrbit(tnum);
                    }
                    else if (tail->ans->mode == i_ind)
                    {
                        if (tail->ans->offset)
                            exposed.setbit(tail->ans->offset->sp->i);
                        if (tail->ans->offset2)
                            exposed.setbit(tail->ans->offset2->sp->i);
                    }
                }
                if (tail->temps & TEMP_LEFT)
                    if (tail->dc.left->mode == i_ind || tail->dc.left->mode == i_direct)
                    {
                        if (!tail->dc.left->retval)
                        {
                            if (tail->dc.left->offset)
                                exposed.setbit(tail->dc.left->offset->sp->i);
                            if (tail->dc.left->offset2)
                                exposed.setbit(tail->dc.left->offset2->sp->i);
                        }
                    }
                if (tail->temps & TEMP_RIGHT)
                    if (tail->dc.right->mode == i_ind || tail->dc.right->mode == i_direct)
                    {
                        if (tail->dc.right->offset)
                            exposed.setbit(tail->dc.right->offset->sp->i);
                        if (tail->dc.right->offset2)
                            exposed.setbit(tail->dc.right->offset2->sp->i);
                    }
                tail = tail->back;
            }
        }
    }
}
static void InitClasses(void)
{
    int i;
    for (i = 0; i < tempCount; i++)
    {
        if (tempInfo[i]->inUse && abs(tempInfo[i]->size) != ISZ_BITINT)
        {
            if (!tempInfo[i]->liveAcrossFunctionCall)
            {
                tempRegClasses[i] = chosenAssembler->arch->regClasses[abs(tempInfo[i]->size) * 2];
                tempRegCounts[i] = tempRegClasses[i]->regCount[noStackFrame];
            }
            else
            {
                tempRegClasses[i] = chosenAssembler->arch->regClasses[abs(tempInfo[i]->size) * 2 + 1];
                tempRegCounts[i] = tempRegClasses[i]->regCount[noStackFrame];
            }
        }
        if (!tempRegClasses[i])
            doGlobals[i] = false;
    }
}
static void CountInstructions(bool first)
{
    QUAD* head = intermed_head;
    int i;
    doGlobals.clear();
    doGlobals.resize(tempCount + 200);
    instructionCount = 0;
    while (head)
    {
        head->index = 0;
        switch (head->dc.opcode)
        {
            case i_block:
            case i_blockend:
            case i_line:
            case i_label:
            case i_expressiontag:
            case i_dbgblock:
            case i_dbgblockend:
            case i_varstart:
                break;
            case i_inlineasm:
                for (int i = 0; i < head->assemblyRegCount; i++)
                    doGlobals[head->assemblyTempRegStart + i] = true;
                break;
            default:
                head->index = instructionCount++;
                if (head->temps & TEMP_ANS)
                {
                    if (head->ans->offset)
                    {
                        doGlobals[head->ans->offset->sp->i] = true;
                    }
                    if (head->ans->offset2)
                    {
                        doGlobals[head->ans->offset2->sp->i] = true;
                    }
                }
                if (head->temps & TEMP_LEFT)
                {
                    if (head->dc.left->offset)
                    {
                        doGlobals[head->dc.left->offset->sp->i] = true;
                    }
                    if (head->dc.left->offset2)
                    {
                        doGlobals[head->dc.left->offset2->sp->i] = true;
                    }
                }
                if (head->temps & TEMP_RIGHT)
                {
                    if (head->dc.right->offset)
                    {
                        doGlobals[head->dc.right->offset->sp->i] = true;
                    }
                    if (head->dc.right->offset2)
                    {
                        doGlobals[head->dc.right->offset2->sp->i] = true;
                    }
                }
                break;
        }
        head = head->fwd;
    }
    for (i = 0; i < tempCount; i++)
        tempInfo[i]->inUse = doGlobals[i];

    workingMoves.clear();
    spilledVars.clear();
    spilling.clear();
    spillTemp.clear();
    activeMoves.clear();
    coalescedMoves.clear();
    tempMoves[0].clear();
    tempMoves[1].clear();
    hiMoves.clear();
    tempWorkingMoves.clear();

    instructionList.clear();
    squeezes.clear();
    degrees.clear();
    spillCosts.clear();

    instructionList.resize(instructionCount);
    tempRegClasses.resize(tempCount + 200);
    tempRegCounts.resize(tempCount + 200);
    squeezes.resize(tempCount + 200);
    rawSqueezes.resize(tempCount + 200);
    degrees.resize(tempCount + 200);
    head = intermed_head;
    while (head)
    {
        switch (head->dc.opcode)
        {
            case i_block:
            case i_blockend:
            case i_line:
            case i_label:
            case i_expressiontag:
            case i_dbgblock:
            case i_dbgblockend:
            case i_varstart:
                break;
            default:
                instructionList[head->index] = head;
                break;
        }
        head = head->fwd;
    }
}
static void Adjacent(int n);
static void Adjacent1(int n);
static bool BriggsCoalesceInit(int u, int v, int n)
{
    int k = 0;
    Adjacent1(u);
    Adjacent(v);
    adjacent.union_from(adjacent1);
    for (auto&& t : adjacent)
    {
        if (squeezes[t] >= tempRegCounts[t])
            k++;
    }
    auto K = imin(tempRegClasses[u]->regCount[noStackFrame], tempRegClasses[v]->regCount[noStackFrame]);
    if (k >= K)
    {
        hiMoves[n] = k - (K - 1);
        return false;
    }
    else
    {
        return true;
    }
}
static bool GeorgeCoalesceInit(int u, int v, int n)
{
    /*u is precolored, v is not */
    Adjacent(v);
    int k = 0;
    for (auto t : adjacent)
    {
        if (squeezes[t] >= tempRegCounts[t])
        {
            if (!isConflicting(u, t))
            {
                k++;
            }
        }
    }
    if (k > 0)
    {
        hiMoves[n] = k;
        return false;
    }
    return true;
}
static int tiny(QUAD* head, IMODE* compare)
{
    QUAD* tail = head->back;
    while (tail->spill)
    {
        tail = tail->back;
    }
    if ((tail->temps & TEMP_ANS) && tail->ans->offset)
        return tail->ans->offset->sp->i == compare->offset->sp->i;
    else
        return false;
}
/* we are doing kthe conflict graph (interference graph)
 * according to Morgan instead of the way it is done in George & Appel
 * accordingly, this function just gets a list of moves
 */
static int fsizeFromISZ(int sz)
{
    int n = sizeFromISZ(sz);
    if (sz >= ISZ_FLOAT)
        n += 100;
    return n;
}
static void Build(Block* b)
{
    QUAD* head;
    BLOCKLIST* bl;
    int i;
    if (b == nullptr)
    {
        for (i = 0; i < tempCount; i++)
        {
            TempInfo* t = tempInfo[i];
            t->temp = 0;
        }
        b = blockArray[0];
    }
    head = b->head;
    while (head != b->tail)
    {
        if (head->dc.opcode == i_assn)
        {
            if (head->temps == (TEMP_ANS | TEMP_LEFT))
            {
                if (head->ans->mode == i_direct && head->dc.left->mode == i_direct && !head->ans->bits && !head->dc.left->bits &&
                    !head->dc.left->retval)
                {
                    int u = head->ans->offset->sp->i;
                    int v = head->dc.left->offset->sp->i;
                    if (tempRegClasses[u] && (tempRegClasses[u] == tempRegClasses[v] ||
                                              fsizeFromISZ(tempInfo[u]->size) == fsizeFromISZ(tempInfo[v]->size)))
                    {
                        TempInfo* t;
                        workingMoves.insert(head->index);
                        tempWorkingMoves[u].insert(head->index);
                        tempWorkingMoves[v].insert(head->index);

                        if (precolored[u])
                        {
                            if (!precolored[v])
                                GeorgeCoalesceInit(u, v, head->index);
                        }
                        else if (precolored[v])
                        {
                            GeorgeCoalesceInit(v, u, head->index);
                        }
                        else
                        {
                            BriggsCoalesceInit(u, v, head->index);
                        }
                    }
                }
            }
        }
        if (head->temps & TEMP_ANS)
        {
            if (head->ans->offset)
            {
                spillCosts[head->ans->offset->sp->i] += 10 * head->block->spillCost;
            }
            if (head->ans->offset2)
            {
                spillCosts[head->ans->offset2->sp->i] += 10 * head->block->spillCost;
            }
        }
        if (head->temps & TEMP_LEFT)
        {
            if (head->dc.left->offset)
            {
                spillCosts[head->dc.left->offset->sp->i] += 10 * head->block->spillCost;
                if (!tiny(head, head->dc.left))
                    tempInfo[head->dc.left->offset->sp->i]->temp = 1;
            }
            if (head->dc.left->offset2)
            {
                spillCosts[head->dc.left->offset2->sp->i] += head->block->spillCost;
                tempInfo[head->dc.left->offset2->sp->i]->temp = 1;
            }
        }
        if (head->temps & TEMP_RIGHT)
        {
            if (head->dc.right->offset)
            {
                spillCosts[head->dc.right->offset->sp->i] += 10 * head->block->spillCost;
                if (!tiny(head, head->dc.right))
                    tempInfo[head->dc.right->offset->sp->i]->temp = 1;
            }
            if (head->dc.right->offset2)
            {
                spillCosts[head->dc.right->offset2->sp->i] += 10 * head->block->spillCost;
                tempInfo[head->dc.right->offset2->sp->i]->temp = 1;
            }
        }
        head = head->fwd;
    }
    bl = b->dominates;
    while (bl)
    {
        Build(bl->block);
        bl = bl->next;
    }
}
static void Adjacent(int n)
{
    adjacent.clear();
    adjacent.resize(tempCount);
    EvaluateConflicts(n, [](int i) {
        if (!stackedTemps.isset(i) && !coalescedNodes.isset(i))
        {
            adjacent.setbit(i);
        }
    });
}
static void Adjacent1(int n)
{
    adjacent1.clear();
    adjacent1.resize(tempCount);
    EvaluateConflicts(n, [](int i) {
        if (!stackedTemps.isset(i) && !coalescedNodes.isset(i))
        {
            adjacent1.setbit(i);
        }
    });
}
static std::set<int>& NodeMoves(int n, int index)
{
    if (!tempWorkingMoves[n].empty())
    {
        int i;
        std::set<int>& rv = tempMoves[index];
        std::set<int>& p = rv;
        std::set<int>& w = tempWorkingMoves[n];
        p.clear();
        for (auto i : w)
        {
            if (activeMoves.find(i) != activeMoves.end() || workingMoves.find(i) != workingMoves.end())
                p.insert(i);
        }
        return rv;
    }
    static std::set<int> emptyrv;
    return emptyrv;
}
static bool MoveRelated(int n, int index)
{
    std::set<int>& data = NodeMoves(n, index);
    return !data.empty();
}
int print;
static void MkWorkList(void)
{
    int i;
    /* going backwatds to get spill temps first */
    for (i = 0; i < tempCount; i++)
    {
        TempInfo* t = tempInfo[i];
        if (doGlobals[i] && !precolored[i] && t->ircinitial)
        {
            doGlobals[i] = false;
            t->ircinitial = false;
            if (squeezes[i] >= tempRegCounts[i])
            {
                if (print)
                    printf(":%d %d %d", i, squeezes[i], tempRegCounts[i]);
                spillWorkList.push_back(i);
            }
            else if (MoveRelated(i, 0))
            {
                freezeWorkList.push_back(i);
            }
            else
            {
                if (print)
                    printf("$%d", i);
                simplifyWorkList.push_back(i);
            }
        }
    }
}
static void EnableMoves(SimpleBitSet& nodes, int index)
{
    for (auto t : nodes)
    {
        std::set<int>& nm = NodeMoves(t, index);
        for (auto k : nm)
        {
            if (activeMoves.find(k) != activeMoves.end())
            {
                --hiMoves[k];
                if (hiMoves[k] <= 0)
                {
                    activeMoves.erase(k);
                    workingMoves.insert(k);
                }
            }
        }
    }
}
static void DecrementDegree(int m, int n)
{
    if (!precolored[m])
    {
        int cur = squeezes[m];
        squeezes[m] += SqueezeChange(m, tempRegClasses[n]->vertex,
                                     -(int)worstCase[noStackFrame][tempRegClasses[m]->index][tempRegClasses[n]->index]);
        degrees[m]--;
        if (cur == tempRegCounts[m] && squeezes[m] < tempRegCounts[m])
        {
            Adjacent(m);
            EnableMoves(adjacent, 1);
            spillWorkList.remove(m);
            if (MoveRelated(m, 1))
            {
                freezeWorkList.push_back(m);
            }
            else
            {
                simplifyWorkList.push_back(m);
            }
        }
    }
}
static void Simplify(void)
{
    int n = simplifyWorkList.front();
    simplifyWorkList.pop_front();
    int i, t;
    if (tempRegClasses[n])
    {
        PUSH(n);
        Adjacent1(n);
        freezeWorkList.remove(n);  // DAL I added this because it seemed needed
        if (squeezes[n] >= tempRegCounts[n])
            EnableMoves(adjacent1, 0);
        for (auto t : adjacent1)
            DecrementDegree(t, n);
    }
}
static void AddWorkList(int u)
{
    if (!precolored[u] && !MoveRelated(u, 1) && squeezes[u] < tempRegCounts[u])
    {
        freezeWorkList.remove(u);
        simplifyWorkList.push_back(u);
    }
}
static int Combine(int u, int v)
{
    bool found = false;
    for (auto n : freezeWorkList)
    {
        if (n == v)
        {
            found = true;
            break;
        }
    }
    if (found)
    {
        freezeWorkList.remove(v);
    }
    else
    {
        spillWorkList.remove(v);
    }
    coalescedNodes.setbit(v);
    tempInfo[v]->partition = u;
    // assumes that the more stringent set has less elements
    if (tempInfo[v]->liveAcrossFunctionCall)
    {
        tempRegClasses[u] = tempRegClasses[v];
        tempRegCounts[u] = tempRegCounts[v];
        tempInfo[u]->liveAcrossFunctionCall = true;
    }
    else if (tempInfo[u]->liveAcrossFunctionCall)
    {
        tempRegClasses[v] = tempRegClasses[u];
        // this next line seems a little wrong but it works...
        tempRegCounts[u] = tempRegCounts[v];
        tempInfo[v]->liveAcrossFunctionCall = true;
    }
    if (precolored[u])
    {
        std::set<int>& nm = tempWorkingMoves[v];
        for (auto k : nm)
        {
            if (instructionList[k])
            {
                int x = instructionList[k]->ans->offset->sp->i;
                int y = instructionList[k]->dc.left->offset->sp->i;
                if (!precolored[x] && !precolored[y])
                {
                    hiMoves[k] = 0;
                    tempWorkingMoves[u].insert(k);
                }
            }
        }
    }
    else
    {
        auto&& tu = tempWorkingMoves[u];
        auto&& tv = tempWorkingMoves[v];
        if (!tv.empty())
        {
            std::set<int> temp;
            std::set_union(tu.begin(), tu.end(), tv.begin(), tv.end(), std::inserter(temp, temp.begin()));
            tu = std::move(temp);
        }
    }
    bool losingHiDegreeNode = squeezes[u] >= tempRegCounts[u] && squeezes[v] >= tempRegCounts[v];

    EvaluateConflicts(v, [u, v](int t) {
        if (!coalescedNodes.isset(t))
        {
            if (t != u && !isConflicting(t, u))
            {
                insertConflict(t, u);
                if (!precolored[u])
                {
                    squeezes[u] += SqueezeChange(u, tempRegClasses[t]->vertex,
                                                 +worstCase[noStackFrame][tempRegClasses[u]->index][tempRegClasses[t]->index]);
                    degrees[u]++;
                }
                if (!precolored[t])
                {
                    squeezes[t] += SqueezeChange(t, tempRegClasses[u]->vertex,
                                                 +worstCase[noStackFrame][tempRegClasses[t]->index][tempRegClasses[u]->index]);
                    degrees[t]++;
                }
            }
            DecrementDegree(t, v);
        }
    });

    if (squeezes[u] >= tempRegCounts[u])
    {
        bool found = false;
        for (auto n : freezeWorkList)
        {
            if (n == u)
            {
                found = true;
                break;
            }
        }
        if (found)
        {
            freezeWorkList.remove(u);
            spillWorkList.push_back(u);
        }
    }
    if (losingHiDegreeNode)
    {
        Adjacent1(u);
        EnableMoves(adjacent1, 0);
    }
    return u;
}
static bool AllOK(int u, int v)
{
    int i, t;
    if (tempInfo[u]->enode->sp->imvalue->retval)
        return false;
    Adjacent(v);
    for (auto t : adjacent)
    {
        if (!precolored[t] && squeezes[t] >= tempRegCounts[t] && !isConflicting(t, u))
            return false;
    }
    return true;
}
static bool Conservative(int u, int v)
{
    int i, t, k = 0;
    Adjacent1(u);
    Adjacent(v);
    adjacent.union_from(adjacent1);
    for (auto&& t : adjacent)
    {
        if (squeezes[t] >= tempRegCounts[t])
            ++k;
    }
    return k < tempRegClasses[u]->regCount[noStackFrame] && k < tempRegClasses[v]->regCount[noStackFrame];
}
static bool conflictsWithSameColor(int u, int v)
{
    if (precolored[u])
    {
        bool rv = false;
        EvaluateConflicts(v, [&rv, u](int j) {
            if (!rv)
            {
                int k = findPartition(j);
                if (precolored[k] && colors[k] == colors[u])
                {
                    rv = true;
                }
            }
        });
        if (rv)
            return true;
    }
    return false;
}
static bool inBothRegClasses(int u, int v)
{
    if (precolored[u])
    {
        int n = colors[u];
        for (int i = 0; i < tempRegCounts[v]; i++)
            if (n == tempRegClasses[v]->regs[i])
                return true;
        return false;
    }
    return true;
}
static bool Coalesce(void)
{
    if (!workingMoves.empty())
    {
        auto sel = *workingMoves.begin();
        int u, v;
        u = findPartition(instructionList[sel]->ans->offset->sp->i);
        v = findPartition(instructionList[sel]->dc.left->offset->sp->i);
        workingMoves.erase(sel);
        tempWorkingMoves[u].erase(sel);
        tempWorkingMoves[v].erase(sel);
        if (precolored[v] || (!precolored[u] && spilled[u]))
        {
            std::swap(u, v);
        }
        if (u == v)
        {
            /* same reg, discard it */
            coalescedMoves.insert(sel);
            AddWorkList(u);
        }
        else if (precolored[v] || isConflicting(u, v) || conflictsWithSameColor(u, v) || !inBothRegClasses(u, v))
        {
            /* both precolored or otherwise in conflict, can't do anything with it */
            // constrainedMoves.insert(sel);
            AddWorkList(u);
            AddWorkList(v);
        }
        else if ((precolored[u] && AllOK(u, v)) || (!precolored[u] && Conservative(u, v)))
        {
            /* zero or one precolored reg, constraints ok, coalesce */
            coalescedMoves.insert(sel);
            AddWorkList(Combine(u, v));
        }
        else
        {
            /* zero or one precolored reg, constraints not ok, we can't coalesce at this time */
            activeMoves.insert(sel);
            hiMoves[sel] = 0;
            if (precolored[u])
            {
                if (!precolored[v])
                    GeorgeCoalesceInit(u, v, sel);
            }
            else if (precolored[v])
                GeorgeCoalesceInit(v, u, sel);
            else
                BriggsCoalesceInit(u, v, sel);
        }
        return true;
    }
    /* no moves buffered for attempted coalesce at this time */
    return false;
}
static void FreezeMoves(int u)
{
    std::set<int>& nm = NodeMoves(u, 0);
    int i;
    for (auto j : nm)
    {
        int v;
        int n = instructionList[j]->ans->offset->sp->i;
        if (findPartition(n) == u)
        {
            v = instructionList[j]->dc.left->offset->sp->i;
        }
        else
        {
            v = instructionList[j]->ans->offset->sp->i;
        }
        if (v != -1)
        {
            if (activeMoves.find(j) != activeMoves.end())
            {
                activeMoves.erase(j);
            }
            else
            {
                workingMoves.erase(j);
                tempWorkingMoves[v].erase(j);
                tempWorkingMoves[u].erase(j);
            }
            // frozenMoves.insert(j);
            if (squeezes[v] < tempRegCounts[v] && !MoveRelated(v, 1))
            {
                freezeWorkList.remove(v);
                simplifyWorkList.push_back(v);
            }
        }
    }
}
static void Freeze(void)
{
    int u = *freezeWorkList.begin();
    freezeWorkList.remove(u);
    simplifyWorkList.push_back(u);
    FreezeMoves(u);
}
static unsigned SpillCost(int i)
{
    int cost;
    auto& def = instructionDefines[i];
    auto& use = instructionUses[i];
    if (def.empty() || use.empty())
        return UINT_MAX;
    if (def.size() == 1 && use.size() == 1)
    {
        QUAD* q = use.front();
        ;
        if (def.front()->index == q->index - 1)
            if (!(q->temps & TEMP_ANS) && q->dc.opcode == i_assn && q->ans->offset->type != se_add && q->ans->mode == i_direct &&
                (q->ans->offset->sp && q->ans->offset->sp->spillVar))
                return UINT_MAX;
    }
    cost = spillCosts[i];  // tempInfo[i]->degree;
    if (cost <= UINT_MAX - 1 - REG_MAX)
        return cost + REG_MAX;
    return UINT_MAX - 1;
}
static void SelectSpill(void)
{
    int m = -1;
    unsigned priority = UINT_MAX;
    int i;
    bool printing = false;
    for (auto s : spillWorkList)
    {
        if (!spilled[s])
        {
            unsigned n = SpillCost(s);
            if (n < priority)
            {
                m = s;
                priority = n;
            }
        }
    }
    if (priority == UINT_MAX)
    {
        int sz;
        m = *spillWorkList.begin();
        sz = 0;
        // no good candidates, select the first...
        for (auto s : spillWorkList)
        {
            if (!spilled[s] && sizeFromISZ(tempInfo[s]->size) > sz)
            {
                m = s;
                sz = sizeFromISZ(tempInfo[s]->size);
            }
        }
    }
    spillWorkList.remove(m);
    if (triedSpill[m])
        spilledNodes.insert(m);
    else
        simplifyWorkList.push_back(m);
    triedSpill[m] = true;
    FreezeMoves(m);
}
static void AssignColors(void)
{
    int i, j;
    int m;
    for (auto m : tempStack)  // this traverses the stack backwards since we are pushing at the front
    {
        std::set<int> regs;
        ARCH_REGCLASS* cls = tempRegClasses[m];
        EvaluateConflicts(m, [&regs](int j) {
            int u = findPartition(j);
            if (colors[u] >= 0)
            {
                int x = colors[u];
                int k;
                regs.insert(x);
                for (k = 0; k < chosenAssembler->arch->regNames[x].aliasRegs.size(); k++)
                {
                    regs.insert(chosenAssembler->arch->regNames[x].aliasRegs[k]);
                }
            }
        });
        for (i = 0; i < cls->regCount[noStackFrame]; i++)
            if (regs.find(cls->regs[i]) == regs.end())
            {
                colors[m] = cls->regs[i];
                tempInfo[m]->enode->sp->regmode = 2;
                break;
            }
        if (i == cls->regCount[noStackFrame])
        {
            spilledNodes.insert(m);
        }
    }
    tempStack.clear();
    for (i = 0; i < tempCount; i++)
        colors[i] = colors[findPartition(i)];
}
static IMODE* InsertLoad(QUAD* head, IMODE* mem)
{
    QUAD* insert;
    IMODE* t = InitTempOpt(mem->size, mem->size);
    spilled[t->offset->sp->i] = true;
    tempInfo[t->offset->sp->i]->ircinitial = true;
    head = head->back;
    insert = Allocate<QUAD>();
    insert->dc.opcode = i_assn;
    insert->ans = t;
    insert->dc.left = mem;
    insert->block = head->block;
    insert->index = -1;
    insert->spill = true;
    InsertInstruction(head, insert);
    accesses++;
    return t;
}
static void InsertStore(QUAD* head, IMODE** im, IMODE* mem)
{
    IMODE* t = InitTempOpt(mem->size, mem->size);
    QUAD* insert;
    int tn = t->offset->sp->i;
    int ta = (*im)->offset->sp->i;
    spilled[tn] = true;
    precolored[tn] = precolored[ta];
    tempInfo[tn]->enode->sp->regmode = tempInfo[ta]->enode->sp->regmode;
    colors[tn] = colors[ta];
    tempInfo[tn]->ircinitial = true;
    *im = t;
    insert = Allocate<QUAD>();
    insert->dc.opcode = i_assn;
    insert->ans = mem;
    insert->dc.left = t;
    insert->block = head->block;
    insert->index = -1;
    if (precolored[tn])
        insert->precolored |= TEMP_LEFT;
    InsertInstruction(head, insert);
    accesses++;
}
static void RewriteAllSpillNodes(void)
{
    QUAD* head = intermed_head;
    std::array<int, 6> tempSpillVars;
    std::array<IMODE*, 6> tempSpillNodes;
    //    IMODE *mem = tempInfo[n]->spillVar;
    while (head)
    {
        IMODE* t;
        std::fill(tempSpillVars.begin(), tempSpillVars.end(), -1);
        std::fill(tempSpillNodes.begin(), tempSpillNodes.end(), nullptr);
        if (head->temps & TEMP_ANS)
        {
            if (head->ans->mode == i_direct)
            {
                if (spilling[head->ans->offset->sp->i])
                {
                    InsertStore(head, &head->ans, spilledVars[head->ans->offset->sp->i]);
                }
            }
            else
            {
                if (head->ans->offset && head->ans->offset->type == se_tempref && spilling[head->ans->offset->sp->i])
                    tempSpillVars[0] = head->ans->offset->sp->i;
                if (head->ans->offset2 && head->ans->offset2->type == se_tempref && spilling[head->ans->offset2->sp->i])
                    tempSpillVars[1] = head->ans->offset2->sp->i;
            }
        }
        if (head->temps & TEMP_LEFT)
        {
            if (head->dc.left->offset && head->dc.left->offset->type == se_tempref && spilling[head->dc.left->offset->sp->i])
                tempSpillVars[2] = head->dc.left->offset->sp->i;
            if (head->dc.left->offset2 && head->dc.left->offset2->type == se_tempref && spilling[head->dc.left->offset2->sp->i])
                tempSpillVars[3] = head->dc.left->offset2->sp->i;
        }
        if (head->temps & TEMP_RIGHT)
        {
            if (head->dc.right->offset && head->dc.right->offset->type == se_tempref && spilling[head->dc.right->offset->sp->i])
                tempSpillVars[4] = head->dc.right->offset->sp->i;
            if (head->dc.right->offset2 && head->dc.right->offset2->type == se_tempref && spilling[head->dc.right->offset2->sp->i])
                tempSpillVars[5] = head->dc.right->offset2->sp->i;
        }
        // this is all to guard against loading the same var two or more times for the same instruction
        // which is both optimally bad and can also lead to insidious bugs
        // because of some assumptions made by the rewrite module
        for (int i = 0; i < 6; i++)
            if (tempSpillVars[i] != -1)
            {
                int j;
                for (j = 0; j < i; j++)
                    if (tempSpillVars[j] == tempSpillVars[i])
                    {
                        break;
                    }
                if (j >= i)
                {
                    tempSpillNodes[i] = InsertLoad(head, spilledVars[tempSpillVars[i]]);
                }
                else
                {
                    tempSpillNodes[i] = tempSpillNodes[j];
                }
            }
        if (tempSpillNodes[0] || tempSpillNodes[1])
        {
            IMODE* im = Allocate<IMODE>();
            *im = *head->ans;
            head->ans = im;
        }
        if (tempSpillNodes[0])
            head->ans->offset = tempSpillNodes[0]->offset;
        if (tempSpillNodes[1])
            head->ans->offset2 = tempSpillNodes[1]->offset;
        if (tempSpillNodes[2] || tempSpillNodes[3])
        {
            IMODE* im = Allocate<IMODE>();
            *im = *head->dc.left;
            head->dc.left = im;
        }
        if (tempSpillNodes[2])
            head->dc.left->offset = tempSpillNodes[2]->offset;
        if (tempSpillNodes[3])
            head->dc.left->offset2 = tempSpillNodes[3]->offset;
        if (tempSpillNodes[4] || tempSpillNodes[5])
        {
            IMODE* im = Allocate<IMODE>();
            *im = *head->dc.right;
            head->dc.right = im;
        }
        if (tempSpillNodes[4])
            head->dc.right->offset = tempSpillNodes[4]->offset;
        if (tempSpillNodes[5])
            head->dc.right->offset2 = tempSpillNodes[5]->offset;
        head = head->fwd;
    }
}
/* give any remaining spill vars a color */
static void SpillColor(void)
{
    int i;
    for (auto n : spilledNodes)
    {
        if (!spilledVars[n])
            GetSpillVar(n);
    }
}
/* attempt to use the same spill var for any spilled variables
 * which are connected by an uncoalesced move
 */
static void SpillCoalesce(std::set<int>& C, std::set<int>& S)
{
    typedef struct _move_
    {
        int a;
        int b;
        int cost;
        std::list<QUAD*> uses;
    } MOVE;
    std::list<MOVE> moves;
    for (auto n : S)
    {
        std::set<int>& nm = tempWorkingMoves[n];
        for (auto k : nm)
        {
            if (coalescedMoves.find(k) == coalescedMoves.end())
            {
                QUAD* head = instructionList[k];
                if (head)
                {
                    int a = head->ans->offset->sp->i;
                    int b = head->dc.left->offset->sp->i;
                    if (fsizeFromISZ(tempInfo[a]->size) == fsizeFromISZ(tempInfo[b]->size))
                    {
                        bool test;
                        if (n == a)
                            test = S.find(b) != S.end();
                        else
                            test = S.find(a) != S.end();
                        if (test)
                        {
                            if (C.find(a) != C.end() || C.find(b) != C.end())
                            {
                                if (a > b)
                                {
                                    std::swap(a, b);
                                }
                                auto it = moves.begin();
                                for (; it != moves.end(); ++it)
                                {
                                    auto mt = *it;
                                    if (mt.a == a && mt.b == b)
                                    {
                                        auto it = mt.uses.begin();
                                        for (; it != mt.uses.end(); ++it)
                                        {
                                            if ((*it) == head)
                                                break;
                                        }
                                        if (it == mt.uses.end())
                                        {
                                            mt.uses.push_back(head);
                                            mt.cost += head->block->spillCost;
                                        }
                                        break;
                                    }
                                }
                                if (it == moves.end())
                                {
                                    MOVE mt = {0};
                                    mt.a = a;
                                    mt.b = b;
                                    mt.cost = head->block->spillCost;
                                    mt.uses.push_back(head);
                                    moves.push_back(std::move(mt));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    while (!moves.empty())
    {
        std::list<MOVE>::iterator mv;
        unsigned cost = 0;
        for (auto it = moves.begin(); it != moves.end(); ++it)
        {
            auto&& mt = *it;
            if (cost < mt.cost)
            {
                mv = it;
                cost = mt.cost;
            }
        }
        if (!isConflicting(mv->a, mv->b))
        {
            if (spilledVars[mv->a])
            {
                if (spilledVars[mv->b])  // just in case...
                {
                    // deallocate one of the spills
                    QUAD* head;
                    IMODE* spill = spilledVars[mv->b];
                    IMODE* nw = spilledVars[mv->a];
                    int i;
                    if (nw != spill)
                    {
                        spill->offset->sp->allocate = false;
                        // make sure all uses of the deallocated spill get renamed
                        for (i = 0; i < tempCount; i++)
                            if (spilledVars[i] == spill)
                                spilledVars[i] = nw;
                        head = intermed_head;
                        while (head)
                        {
                            if (head->ans == spill)
                                head->ans = nw;
                            if (head->dc.left == spill)
                                head->dc.left = nw;
                            head = head->fwd;
                        }
                    }
                }
                else
                    spilledVars[mv->b] = spilledVars[mv->a];
            }
            else if (spilledVars[mv->b])
            {
                spilledVars[mv->a] = spilledVars[mv->b];
            }
            else
            {
                GetSpillVar(mv->a);
                spilledVars[mv->b] = spilledVars[mv->a];
            }
        }
        moves.erase(mv);
    }
}
static void InsertCandidates(std::unordered_set<int>& spillProcessed, int W, std::set<int>& L)
{
    std::set<int>& moves = tempWorkingMoves[W];
    int i;
    for (auto&& j : moves)
    {
        if (coalescedMoves.find(j) == coalescedMoves.end())
        {
            QUAD* head = instructionList[j];
            if (head)
            {
                int n = head->ans->offset->sp->i;
                if (n == W)
                    n = head->dc.left->offset->sp->i;
                if (!spilled[n] && spillProcessed.find(n) == spillProcessed.end())
                {
                    L.insert(n);
                    spillProcessed.insert(n);
                }
            }
        }
    }
}
static unsigned lscost(int size)
{
    ARCH_SIZING* p = chosenAssembler->arch->regCosts;
    switch (size)
    {
        case ISZ_BIT:
            return 1;
        case ISZ_UCHAR:
        case -ISZ_UCHAR:
            return p->a_char;
        case ISZ_BOOLEAN:
            return p->a_bool;
        case ISZ_USHORT:
        case -ISZ_USHORT:
        case ISZ_U16:
            return p->a_short;
        case ISZ_WCHAR:
            return p->a_wchar_t;
        case ISZ_ULONG:
        case -ISZ_ULONG:
            return p->a_long;
        case ISZ_ULONGLONG:
        case -ISZ_ULONGLONG:
            return p->a_longlong;
        case ISZ_UINT:
        case -ISZ_UINT:
        case ISZ_U32:
        case ISZ_UNATIVE:
        case -ISZ_UNATIVE:
            return p->a_int;
            /*        case ISZ_ENUM:*/
            /*            return p->a_enum;*/
        case ISZ_ADDR:
            return p->a_addr;
        case ISZ_SEG:
            return p->a_farseg;
        case ISZ_FARPTR:
            return p->a_farptr;
        case ISZ_FLOAT:
        case ISZ_IFLOAT:
            return p->a_float;
        case ISZ_DOUBLE:
        case ISZ_IDOUBLE:
            return p->a_double;
        case ISZ_LDOUBLE:
        case ISZ_ILDOUBLE:
            return p->a_longdouble;
        case ISZ_CFLOAT:
            return (p->a_float + p->a_fcomplexpad) * 2;
        case ISZ_CDOUBLE:
            return (p->a_double + p->a_rcomplexpad) * 2;
        case ISZ_CLDOUBLE:
            return (p->a_longdouble + p->a_lrcomplexpad) * 2;
        default:
            return 1;
    }
}
static unsigned SpillPropagateSavings(int w, std::set<int>& S)
{
    unsigned savings = 0;
    std::set<int>& moves = tempWorkingMoves[w];
    int i;
    for (auto j : moves)
    {
        if (coalescedMoves.find(j) == coalescedMoves.end())
        {
            QUAD* head = instructionList[j];
            if (head)
            {
                int n = head->ans->offset->sp->i;
                if (n == w)
                    n = head->dc.left->offset->sp->i;
                if (S.find(n) != S.end() && !isConflicting(n, w))
                {
                    int sv;
                    int cost = lscost(abs(tempInfo[n]->enode->sp->imvalue->size));
                    if (UINT_MAX / cost < head->block->spillCost)
                        return UINT_MAX;
                    sv = cost * head->block->spillCost;
                    if (UINT_MAX - sv < savings)
                        return UINT_MAX;
                    savings += sv;
                }
            }
        }
    }
    return savings;
}
static void SpillPropagate(std::unordered_set<int>& spillProcessed, std::set<int>& P, std::set<int>& S, std::set<int>& L,
                           std::set<int>& NP)
{
    L.clear();
    NP.clear();
    for (auto i : P)
        InsertCandidates(spillProcessed, i, L);
    while (!L.empty())
    {
        int w = *L.begin();
        L.erase(w);
        if (SpillPropagateSavings(w, S) >= spillCosts[w])
        {
            S.insert(w);
            NP.insert(w);
            InsertCandidates(spillProcessed, w, L);
        }
    }
    P = NP;
}
static void SpillPropagateAndCoalesce(void)
{
    std::set<int> P;
    std::set<int> np;
    std::set<int> L;
    int i;
    std::unordered_set<int> spillProcessed;
    P = spilledNodes;
    do
    {
        SpillCoalesce(P, spilledNodes);
        SpillPropagate(spillProcessed, P, spilledNodes, L, np);
    } while (!P.empty());
}
/* this routine and its callees don't bother to update
 * most internal tables, since what is going to happen after
 * this runs is that everything will be built from scratch anyway
 */
static void RewriteProgram(void)
{
    int i;
    SpillPropagateAndCoalesce();
    SpillColor();
    spilling.clear();
    for (auto n : spilledNodes)
    {
        spilling[n] = true;
        spilled[n] = true;
    }
    RewriteAllSpillNodes();
    for (i = 0; i < tempCount; i++)
    {
        if (colors[i] != -1 || coalescedNodes.isset(i))
            tempInfo[i]->ircinitial = true;
    }
    // letting restart of the algorithm clear necessary variables
}
static IMODE* copyImode(IMODE* in)
{
    SimpleExpression *offset = nullptr, *offset2 = nullptr;
    if (in->offset && in->offset->type == se_tempref)
    {
        int tnum = in->offset->sp->i;
        if (coalescedNodes.isset(tnum))
        {
            offset = tempInfo[findPartition(tnum)]->enode;
        }
    }
    if (in->offset2 && in->offset2->type == se_tempref)
    {
        int tnum = in->offset2->sp->i;
        if (coalescedNodes.isset(tnum))
        {
            offset2 = tempInfo[findPartition(tnum)]->enode;
        }
    }
    if (offset || offset2)
    {
        IMODE* im = Allocate<IMODE>();
        *im = *in;
        if (offset)
            im->offset = offset;
        if (offset2)
            im->offset2 = offset2;
        return im;
    }
    return in;
}
static void KeepCoalescedNodes(void)
{
    QUAD* head = intermed_head;
    int i;
    while (head)
    {
        QUAD* next = head->fwd;
        if (head->index != -1 && coalescedMoves.find(head->index) != coalescedMoves.end())
        {
            instructionList[head->index] = nullptr;
            if (head == head->block->tail)
                head->block->tail = head->back;
            head->fwd->back = head->back;
            head->back->fwd = head->fwd;
        }
        else
        {
            if (head->temps & TEMP_ANS)
            {
                head->ans = copyImode(head->ans);
            }
            if (head->temps & TEMP_LEFT)
            {
                head->dc.left = copyImode(head->dc.left);
            }
            if (head->temps & TEMP_RIGHT)
            {
                head->dc.right = copyImode(head->dc.right);
            }
        }
        head = next;
    }
    coalescedNodes.clear();
    coalescedMoves.clear();
    for (i = 0; i < tempCount; i++)
        tempInfo[i]->partition = i;
}
static int LoopNesting(Loop* loop)
{
    unsigned rv = 1;
    while (loop)
    {
        if (rv >= UINT_MAX / 10)
        {
            rv = UINT_MAX;
            break;
        }
        rv *= 10;
        loop = loop->parent;
    }
    return rv;
}
static void CalculateNesting(void)
{
    int i;
    for (i = 0; i < blockCount; i++)
        if (blockArray[i])
            blockArray[i]->spillCost = LoopNesting(blockArray[i]->loopParent);
}
void Precolor(bool optimized)
{
    QUAD* head = intermed_head;
    int i;
    std::fill(colors.begin(), colors.end(), -1);
    std::fill(precolored.begin(), precolored.end(), false);
    for (i = 0; i < tempCount; i++)
    {
        if (tempInfo[i]->enode)
            tempInfo[i]->enode->sp->regmode = 0;
    }
    while (head)
    {
        if (head->dc.opcode != i_block && !head->ignoreMe && head->dc.opcode != i_label && head->dc.opcode != i_expressiontag)
        {
            FastcallColor(head);
            PreColor(head);
        }
        head = head->fwd;
    }
    for (i = 0; i < tempCount; i++)
    {
        tempInfo[i]->ircinitial = !precolored[i];
    }
}
void RearrangePrecolors(void)
{
    InstructionDefinesSsa.clear();
    InstructionDefinesSsa.resize(tempCount + 2000);
    InstructionUsesSsa.clear();
    InstructionUsesSsa.resize(tempCount + 2000);
    BlockDefinesSsa.clear();
    BlockDefinesSsa.resize(tempCount + 2000);
    QUAD* head = intermed_head;
    int i;
    for (i = 0; i < tempCount; i++)
        tempInfo[i]->temp = -1;
    while (head)
    {
        if ((head->precolored & TEMP_ANS) && !head->ans->retval)
        {
            QUAD* newIns = Allocate<QUAD>();
            i = head->ans->offset->sp->i;
            if (tempInfo[i]->temp < 0)
            {
                IMODE* newImode = InitTempOpt(head->ans->size, head->ans->size);
                tempInfo[i]->temp = newImode->offset->sp->i;
                newIns->dc.left = newImode;
            }
            else
            {
                newIns->dc.left = tempInfo[tempInfo[i]->temp]->enode->sp->imvalue;
            }
            newIns->dc.opcode = i_assn;
            newIns->ans = head->ans;
            head->ans = newIns->dc.left;
            InsertInstruction(head, newIns);
        }
        if ((head->precolored & TEMP_LEFT) && !head->dc.left->retval)
        {
            QUAD* newIns = Allocate<QUAD>();
            i = head->dc.left->offset->sp->i;
            if (tempInfo[i]->temp < 0)
            {
                IMODE* newImode = InitTempOpt(head->dc.left->size, head->dc.left->size);
                tempInfo[i]->temp = newImode->offset->sp->i;
                newIns->ans = newImode;
            }
            else
            {
                newIns->ans = tempInfo[tempInfo[i]->temp]->enode->sp->imvalue;
            }
            newIns->dc.opcode = i_assn;
            newIns->dc.left = head->dc.left;
            head->dc.left = newIns->ans;
            InsertInstruction(head->back, newIns);
        }
        if ((head->precolored & TEMP_RIGHT) && !head->dc.right->retval)
        {
            QUAD* newIns = Allocate<QUAD>();
            i = head->dc.right->offset->sp->i;
            if (tempInfo[i]->temp < 0)
            {
                IMODE* newImode = InitTempOpt(head->dc.right->size, head->dc.right->size);
                tempInfo[i]->temp = newImode->offset->sp->i;
                newIns->ans = newImode;
            }
            else
            {
                newIns->ans = tempInfo[tempInfo[i]->temp]->enode->sp->imvalue;
            }
            newIns->dc.opcode = i_assn;
            newIns->dc.left = head->dc.right;
            head->dc.right = newIns->ans;
            InsertInstruction(head->back, newIns);
        }
        head->precolored = 0;
        head = head->fwd;
    }
}
void retemp(void)
{
    std::vector<unsigned> map(tempCount);
    int i, cur = 0;
    QUAD* head;
    for (i = 0; i < tempCount; i++)
    {
        tempInfo[i]->inUse = false;
    }
    head = intermed_head;
    while (head)
    {
        if (head->dc.opcode == i_inlineasm)
        {
            for (int i = 0; i < head->assemblyRegCount; i++)
                tempInfo[head->assemblyTempRegStart + i]->inUse = true;
        }
        else if (head->dc.opcode != i_block && !head->ignoreMe && head->dc.opcode != i_inlineasm && head->dc.opcode != i_label)
        {
            if (head->ans)
            {
                if (head->ans->offset && head->ans->offset->type == se_tempref)
                    tempInfo[head->ans->offset->sp->i]->inUse = true;
                if (head->ans->offset2 && head->ans->offset2->type == se_tempref)
                    tempInfo[head->ans->offset2->sp->i]->inUse = true;
            }
            if (head->dc.left)
            {
                if (head->dc.left->offset && head->dc.left->offset->type == se_tempref)
                    tempInfo[head->dc.left->offset->sp->i]->inUse = true;
                if (head->dc.left->offset2 && head->dc.left->offset2->type == se_tempref)
                    tempInfo[head->dc.left->offset2->sp->i]->inUse = true;
            }
            if (head->dc.right)
            {
                if (head->dc.right->offset && head->dc.right->offset->type == se_tempref)
                    tempInfo[head->dc.right->offset->sp->i]->inUse = true;
                if (head->dc.right->offset2 && head->dc.right->offset2->type == se_tempref)
                    tempInfo[head->dc.right->offset2->sp->i]->inUse = true;
            }
        }
        head = head->fwd;
    }
    for (i = 0; i < tempCount; i++)
        if (tempInfo[i]->inUse)
        {
            map[i] = cur;
            tempInfo[cur] = tempInfo[i];
            tempInfo[cur]->enode->sp->retemp = true;
            tempInfo[cur]->enode->sp->i = cur;
            InstructionDefinesSsa[cur] = InstructionDefinesSsa[i];
            cur++;
        }
    for (i = cur; i < tempCount; i++)
        tempInfo[i] = nullptr;
    tempCount = cur;
    head = intermed_head;
    while (head)
    {
        if (head->dc.opcode == i_inlineasm)
        {
            head->assemblyTempRegStart = map[head->assemblyTempRegStart];
            for (int i = 0; i < head->assemblyRegCount; i++)
            {
                int n = head->assemblyTempRegStart + i;
                tempInfo[n]->enode->sp->retemp = true;
                tempInfo[n]->enode->sp->i = map[n];
            }
        }
        else if (head->dc.opcode != i_block && !head->ignoreMe && head->dc.opcode != i_inlineasm && head->dc.opcode != i_label)
        {
            if (head->ans)
            {
                if (head->ans->offset && head->ans->offset->type == se_tempref && !head->ans->offset->sp->retemp)
                {
                    head->ans->offset->sp->retemp = true;
                    head->ans->offset->sp->i = map[head->ans->offset->sp->i];
                }
                if (head->ans->offset2 && head->ans->offset2->type == se_tempref && !head->ans->offset2->sp->retemp)
                {
                    head->ans->offset2->sp->retemp = true;
                    head->ans->offset2->sp->i = map[head->ans->offset2->sp->i];
                }
            }
            if (head->dc.left)
            {
                if (head->dc.left->offset && head->dc.left->offset->type == se_tempref && !head->dc.left->offset->sp->retemp)
                {
                    head->dc.left->offset->sp->retemp = true;
                    head->dc.left->offset->sp->i = map[head->dc.left->offset->sp->i];
                }
                if (head->dc.left->offset2 && head->dc.left->offset2->type == se_tempref && !head->dc.left->offset2->sp->retemp)
                {
                    head->dc.left->offset2->sp->retemp = true;
                    head->dc.left->offset2->sp->i = map[head->dc.left->offset2->sp->i];
                }
            }
            if (head->dc.right)
            {
                if (head->dc.right->offset && head->dc.right->offset->type == se_tempref && !head->dc.right->offset->sp->retemp)
                {
                    head->dc.right->offset->sp->retemp = true;
                    head->dc.right->offset->sp->i = map[head->dc.right->offset->sp->i];
                }
                if (head->dc.right->offset2 && head->dc.right->offset2->type == se_tempref && !head->dc.right->offset2->sp->retemp)
                {
                    head->dc.right->offset2->sp->retemp = true;
                    head->dc.right->offset2->sp->i = map[head->dc.right->offset2->sp->i];
                }
            }
        }
        head = head->fwd;
    }
}
// note that once we get here, we start breaking the rule about how IMODES for the same
// temp should have the same address
void AllocateRegisters(QUAD* head)
{
    bool first = true;
    int firstSpill = true;
    int spills = 0;
    int passes = 0;
    (void)head;
    int n = tempCount;
    retemp();
    spilled.clear();
    triedSpill.clear();
    accesses = 0;
    Prealloc(3);
    CalculateNesting();
    Precolor(cparams.prm_optimize_for_speed || cparams.prm_optimize_for_size);
    tempRegClasses.clear();
    rawSqueezes.clear();
    tempRegCounts.clear();
    static int temp3 = tempCount;
    while (true)
    {
        int i;
        passes++;
        if (passes >= 100)
            break;
        ResetConflict();

        for (i = 0; i < tempCount; i++)
        {
            if (!precolored[i])
            {
                colors[i] = -1;
                if (tempInfo[i]->enode)
                    tempInfo[i]->enode->sp->regmode = 0;
            }
            tempInfo[i]->partition = i;
        }
        CountInstructions(first);
        tempWorkingMoves.clear();
        simplifyWorkList.clear();
        freezeWorkList.clear();
        spillWorkList.clear();
        spilledNodes.clear();
        coalescedNodes.clear();
        coalescedNodes.resize(tempCount + 5000);
        adjacent.clear();
        adjacent1.clear();
        stackedTemps.clear();
        stackedTemps.resize(tempCount + 200);
        tempStack.clear();
        liveVariables();
        CalculateFunctionFlags();
        InitClasses();
        definesInfo();
        usesInfo();
        std::unordered_set<int> temp;
        CalculateConflictGraph(temp, true);
        SqueezeInit();
        Build(nullptr);
        MkWorkList();
        for (i = 0; i < tempCount; i++)
        {
            spillCosts[i] *= 10;
            if (degrees[i])
                spillCosts[i] = spillCosts[i] / degrees[i];
            //            else
            //                spillCosts[i] = UINT_MAX - 1;  // no degrees = don't spill unless we absolutely have to.
        }
        first = false;
        while (true)
        {
            if (!simplifyWorkList.empty())
                Simplify();
            else if (Coalesce())
                ;
            else if (!freezeWorkList.empty())
                Freeze();
            else if (!spillWorkList.empty())
            {
                if (firstSpill)
                {
                    KeepCoalescedNodes();
                    firstSpill = false;
                }
                SelectSpill();
            }
            else
            {
                break;
            }
        }
        for (i = 0; i < tempCount; i++)
        {
            if (!precolored[i])
            {
                colors[i] = -1;
                if (tempInfo[i]->enode)
                    tempInfo[i]->enode->sp->regmode = 0;
            }
        }
        AssignColors();
        if (!spilledNodes.empty())
        {
            spills += spilledNodes.size();
            RewriteProgram();
        }
        else
        {
            break;
        }
    }

    KeepCoalescedNodes();
    CopyLocalColors();
    if (passes >= 100)
        diag("register allocator failed");
    ResetConflict();
    registersAssigned = true;
}
}  // namespace Optimizer
