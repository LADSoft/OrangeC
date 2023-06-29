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
#include <cstdlib>
#include <climits>
#include "ioptimizer.h"
#include "beinterfdefs.h"
#include <vector>
#include <deque>
#include <map>
#include "irc.h"
#include "config.h"
#include "iblock.h"
#include "ilazy.h"
#include "OptUtils.h"
#include "ildata.h"
#include "ioptutil.h"
#include "memory.h"
#include "ilive.h"
#include "optmain.h"
#include "irewrite.h"
#include "iconfl.h"
#include "ilocal.h"

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

static int* tempStack;
static int tempStackcount;
static int lc_maxauto;
static unsigned long long regmask;
static int localspill, spillcount;
static unsigned short* simplifyWorklist;
static int simplifyBottom, simplifyTop;
static BRIGGS_SET* freezeWorklist;
static BRIGGS_SET* spillWorklist;
static BRIGGS_SET* spilledNodes;
static BITARRAY* coalescedNodes;
static BITARRAY* stackedTemps;
static BITARRAY *adjacent, *adjacent1;
static BITARRAY* workingMoves;
static BITARRAY* activeMoves;
static BITARRAY* coalescedMoves;
static BITARRAY* constrainedMoves;
static BITARRAY* frozenMoves;
static BITARRAY* tempMoves[2];
static QUAD** instructionList;
static BRIGGS_SET* spillProcessed;
static short* hiMoves;
static SPILL* spillList;

static int classCount, vertexCount;
static unsigned* worstCase;
static ARCH_REGCLASS** classes;
static ARCH_REGVERTEX** vertexes;

static int instructionCount;
static int instructionByteCount;

static int accesses;
#define PUSH(t)                          \
    {                                    \
        tempStack[tempStackcount++] = t; \
        setbit(stackedTemps, t);         \
    }
#define POP() (tempStackcount == 0 ? -1 : tempStack[--tempStackcount])

static const UBYTE bitCounts[256] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3,
    4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4,
    4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 1,
    2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5,
    4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5,
    6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
};

static void InitRegAliases(ARCH_REGDESC* desc)
{
    int i;
    for (i = 0; i < REG_MAX; i++)
    {
        int j;
        desc[i].aliasBits = (BITARRAY*)calloc(sizeof(BITINT), sizeof(BITARRAY) + (REG_MAX + BITINTBITS - 1) / BITINTBITS);
#ifdef TESTBITS
        desc[i].aliasBits->count = REG_MAX;
#endif
        setbit(desc[i].aliasBits, i);
        for (j = 0; j < desc[i].aliasCount; j++)
        {
            setbit(desc[i].aliasBits, desc[i].aliases[j]);
        }
    }
}
static void InitTree(ARCH_REGVERTEX* parent, ARCH_REGVERTEX* child)
{
    ARCH_REGCLASS* c = child->cls;
    ARCH_REGVERTEX* v;
    int i;
    child->parent = parent;
    child->index = vertexCount++;
    while (c)
    {
        c->vertex = child->index;
        c->index = classCount++;
        c->aliasBits = (BITARRAY*)calloc(sizeof(BITINT), sizeof(BITARRAY) + (REG_MAX + BITINTBITS - 1) / BITINTBITS);
#ifdef TESTBITS
        c->aliasBits->count = REG_MAX;
#endif
        c->regBits = (BITARRAY*)calloc(sizeof(BITINT), sizeof(BITARRAY) + (REG_MAX + BITINTBITS - 1) / BITINTBITS);
#ifdef TESTBITS
        c->regBits->count = REG_MAX;
#endif
        for (i = 0; i < c->regCount; i++)
        {
            int j;
            setbit(c->regBits, c->regs[i]);
            for (j = 0; j < (REG_MAX + BITINTBITS - 1) / BITINTBITS; j++)
            {
                bits(c->aliasBits)[j] |= bits(chosenAssembler->arch->regNames[c->regs[i]].aliasBits)[j];
            }
        }
        c = c->next;
    }
    if (child->left)
        InitTree(child, child->left);
    if (child->right)
        InitTree(child, child->right);
}
static void LoadVertexes(ARCH_REGVERTEX* child)
{
    ARCH_REGCLASS* c = child->cls;
    while (c)
    {
        classes[c->index] = c;
        c = c->next;
    }
    vertexes[child->index] = child;
    if (child->left)
        LoadVertexes(child->left);
    if (child->right)
        LoadVertexes(child->right);
}
static void LoadWorstCase(void)
{
    int i, j, k, m;
    worstCase = (unsigned*)calloc(classCount * classCount, sizeof(unsigned));
    for (i = 0; i < classCount; i++)
    {
        for (j = 0; j < classCount; j++)
        {
            int max = 0;
            for (k = 0; k < classes[j]->regCount; k++)
            {
                int count = 0;
                for (m = 0; m < (REG_MAX + BITINTBITS - 1) / BITINTBITS; m++)
                {
                    BITINT x =
                        bits(classes[i]->regBits)[m] & bits(chosenAssembler->arch->regNames[classes[j]->regs[k]].aliasBits)[m];
                    int y;
                    for (y = 0; y < sizeof(BITINT); y++)
                    {
                        count += bitCounts[x & 0xff];
                        x >>= 8;
                    }
                }
                if (count > max)
                    max = count;
            }
            worstCase[i * classCount + j] = max;
        }
    }
}
static void LoadAliases(ARCH_REGVERTEX* v)
{
    ARCH_REGCLASS* c = v->cls;
    int i;
    int j;
    int k;
    if (v->left)
        LoadAliases(v->left);
    if (v->right)
        LoadAliases(v->right);
    v->aliasBits = (BITARRAY*)calloc(sizeof(BITINT), sizeof(BITARRAY) + (REG_MAX + BITINTBITS - 1) / BITINTBITS);
#ifdef TESTBITS
    v->aliasBits->count = REG_MAX;
#endif
    if (v->left)
    {
        for (i = 0; i < (REG_MAX + BITINTBITS - 1) / BITINTBITS; i++)
            bits(v->aliasBits)[i] |= bits(v->left->aliasBits)[i];
    }
    if (v->right)
    {
        for (i = 0; i < (REG_MAX + BITINTBITS - 1) / BITINTBITS; i++)
            bits(v->aliasBits)[i] |= bits(v->right->aliasBits)[i];
    }
    while (c)
    {
        for (i = 0; i < (REG_MAX + BITINTBITS - 1) / BITINTBITS; i++)
            bits(v->aliasBits)[i] |= bits(c->aliasBits)[i];
        c = c->next;
    }
}
void LoadSaturationBounds(void)
{
    int i, j;
    for (i = 0; i < classCount; i++)
    {
        classes[i]->saturationBound = (short*)calloc(sizeof(unsigned short), vertexCount);
        for (j = 0; j < vertexCount; j++)
        {
            int k;
            int rv = 0;
            // this MIGHT be endian dependent...
            for (k = 0; k < (REG_MAX + BITINTBITS - 1) / BITINTBITS; k++)
            {
                BITINT x = bits(classes[i]->regBits)[k] & bits(vertexes[j]->aliasBits)[k];
                int y;
                for (y = 0; y < sizeof(BITINT); y++)
                {
                    rv += bitCounts[x & 0xff];
                    x >>= 8;
                }
            }
            classes[i]->saturationBound[j] = rv;
        }
    }
}
}  // namespace Optimizer
void regInit(void)
{
    using namespace Optimizer;
    int i, j;
    classCount = vertexCount = 0;
    InitRegAliases(chosenAssembler->arch->regNames);
    InitTree(nullptr, chosenAssembler->arch->regRoot);
    vertexes = (ARCH_REGVERTEX**)calloc(sizeof(ARCH_REGVERTEX*), vertexCount);
    classes = (ARCH_REGCLASS**)calloc(sizeof(ARCH_REGCLASS*), classCount);
    LoadVertexes(chosenAssembler->arch->regRoot);
    LoadWorstCase();
    LoadAliases(chosenAssembler->arch->regRoot);
    LoadSaturationBounds();
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
            head->dc.opcode != i_line && head->dc.opcode != i_passthrough)
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
void AllocateStackSpace()
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
        if (sym->storage_class != scc_constant)
        {
            int lvl = sym->i;
            queue[lvl].push_back(sym);
            modes[sym] = sym->anonymous ? 2 : (lvl > oldlvl);
        }
    }
    for (auto sym : temporarySymbols)
    {
        if (sym->storage_class != scc_constant)
        {
            int lvl = sym->i;
            queue[lvl].push_back(sym);
            modes[sym] = 2;
        }
    }
    bool show = false;
    lc_maxauto = max = 0;

    for (auto&& dq : queue)
    {
        int oldauto = lc_maxauto;
        lc_maxauto = max;
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
            if (doit && sym->offset == 0 && sym->tp->size)
            {
                int val;
                lc_maxauto += sym->tp->size;
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
    SPILL* spill;
    SimpleExpression* exp;
    SimpleType* tp = tempInfo[i]->enode->sp->tp;
    if (tp->type == st_pointer && tp->isarray)
    {
        // if we get here with an array type, it is a pointer to an array which was in an argument
        tp = Allocate<SimpleType>();
        tp->type = st_pointer;
        tp->sizeFromType = ISZ_ADDR;
        tp->size = sizeFromISZ(ISZ_ADDR);
    }
    exp = spillVar(scc_auto, tp);
    spill = tAllocate<SPILL>();
    tempInfo[i]->spillVar = spill->imode = make_ioffset(exp);
    spill->imode->offset->sp->spillVar = true;
    spill->imode->size = tempInfo[i]->enode->sp->imvalue->size;
    spill->uses = tAllocate<LIST>();
    spill->uses->data = (void*)i;
}
static void CopyLocalColors(void)
{
    QUAD* head = intermed_head;
    regmask = 0;
    while (head)
    {
        if (head->dc.opcode != i_block && !head->ignoreMe && head->dc.opcode != i_passthrough && head->dc.opcode != i_label)
        {
            if ((head->temps & TEMP_ANS) || (head->ans && head->ans->retval))
            {
                if (head->ans->offset)
                {
                    head->ansColor = tempInfo[head->ans->offset->sp->i]->color;
                    if (head->ansColor >= 0)
                        regmask |= chosenAssembler->arch->regNames[head->ansColor].pushMask;
                }
                if (head->ans->offset2)
                {
                    head->scaleColor = tempInfo[head->ans->offset2->sp->i]->color;
                    if (head->scaleColor >= 0)
                        regmask |= chosenAssembler->arch->regNames[head->scaleColor].pushMask;
                }
            }
            if ((head->temps & TEMP_LEFT) || (head->dc.left && head->dc.left->retval))
            {
                if (head->dc.left->offset)
                {
                    head->leftColor = tempInfo[head->dc.left->offset->sp->i]->color;
                    if (head->leftColor >= 0)
                        regmask |= chosenAssembler->arch->regNames[head->leftColor].pushMask;
                }
                if (head->dc.left->offset2)
                {
                    head->scaleColor = tempInfo[head->dc.left->offset2->sp->i]->color;
                    if (head->scaleColor >= 0)
                        regmask |= chosenAssembler->arch->regNames[head->scaleColor].pushMask;
                }
            }
            if ((head->temps & TEMP_RIGHT) || (head->dc.right && head->dc.right->retval))
            {
                if (head->dc.right->offset)
                {
                    head->rightColor = tempInfo[head->dc.right->offset->sp->i]->color;
                    if (head->rightColor >= 0)
                        regmask |= chosenAssembler->arch->regNames[head->rightColor].pushMask;
                }
                if (head->dc.right->offset2)
                {
                    head->scaleColor = tempInfo[head->dc.right->offset2->sp->i]->color;
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
        int alpha = tempInfo[temp]->rawSqueeze[v->index];

        tempInfo[temp]->rawSqueeze[v->index] += delta;

        if (delta >= 0)
            delta = imax(0, imin(delta, tempInfo[temp]->regClass->saturationBound[v->index] - alpha));
        else
            delta = imin(0, imax(delta, delta - (tempInfo[temp]->regClass->saturationBound[v->index] - alpha)));

        v = v->parent;
    } while (delta && v);
    return delta;
}
void SqueezeInit(void)
{
    int i;
    for (i = 0; i < tempCount; i++)
        if (tempInfo[i]->doGlobal && !tempInfo[i]->precolored)
        {
            tempInfo[i]->squeeze = 0;
            if (tempInfo[i]->rawSqueeze)
                memset(tempInfo[i]->rawSqueeze, 0, sizeof(tempInfo[0]->rawSqueeze[0]) * vertexCount);
            else
                tempInfo[i]->rawSqueeze = aAllocate<int>(vertexCount);
            tempInfo[i]->degree = 0;
        }
    for (i = 0; i < tempCount; i++)
    {
        if (tempInfo[i]->doGlobal && !tempInfo[i]->precolored)
        {
            UBYTE regs[MAX_INTERNAL_REGS];
            int j;
            int k, n;
            BITINT* confl = tempInfo[i]->conflicts;
            memset(regs, 0, sizeof(regs));
            for (k = 0; k < (tempCount + BITINTBITS - 1) / BITINTBITS; k++)
                if (confl[k])
                    for (n = k * BITINTBITS; n < k * BITINTBITS + BITINTBITS; n++)
                        if (isset(confl, n))
                        {
                            if (!tempInfo[n]->precolored)
                            {
                                tempInfo[i]->squeeze += SqueezeChange(
                                    i, tempInfo[n]->regClass->vertex,
                                    +worstCase[tempInfo[i]->regClass->index * classCount + tempInfo[n]->regClass->index]);
                                tempInfo[i]->degree++;
                            }
                            else
                            {
                                regs[tempInfo[n]->color] = true;
                            }
                        }
            tempInfo[i]->regCount = tempInfo[i]->regClass->regCount;
            for (j = 0; j < REG_MAX; j++)
                if (regs[j])
                {
                    if (isset(tempInfo[i]->regClass->aliasBits, j))
                    {
                        tempInfo[i]->regCount--;
                    }
                }
        }
    }
}
static void CalculateFunctionFlags(void)
{
    BRIGGS_SET* exposed = briggsAlloct(tempCount);
    int i, j;
    for (j = 0; j < tempCount; j++)
        tempInfo[j]->liveAcrossFunctionCall = false;
    for (i = 0; i < blockCount; i++)
    {
        struct _block* blk = blockArray[i];
        if (blk && blk->head)
        {
            QUAD* tail = blk->tail;
            BITINT* p = blk->liveOut;
            int j, k;
            briggsClear(exposed);
            for (j = 0; j < (tempCount + BITINTBITS - 1) / BITINTBITS; j++, p++)
                if (*p)
                    for (k = 0; k < BITINTBITS; k++)
                        if (*p & (1 << k))
                        {
                            briggsSet(exposed, j * BITINTBITS + k);
                        }
            while (tail != blk->head)
            {
                if (tail->dc.opcode == i_gosub)
                {
                    for (j = 0; j < exposed->top; j++)
                        tempInfo[exposed->data[j]]->liveAcrossFunctionCall = true;
                }
                if (tail->temps & TEMP_ANS)
                {
                    if (tail->ans->mode == i_direct)
                    {
                        int tnum = tail->ans->offset->sp->i;
                        briggsReset(exposed, tnum);
                    }
                    else if (tail->ans->mode == i_ind)
                    {
                        if (tail->ans->offset)
                            briggsSet(exposed, tail->ans->offset->sp->i);
                        if (tail->ans->offset2)
                            briggsSet(exposed, tail->ans->offset2->sp->i);
                    }
                }
                if (tail->temps & TEMP_LEFT)
                    if (tail->dc.left->mode == i_ind || tail->dc.left->mode == i_direct)
                    {
                        if (!tail->dc.left->retval)
                        {
                            if (tail->dc.left->offset)
                                briggsSet(exposed, tail->dc.left->offset->sp->i);
                            if (tail->dc.left->offset2)
                                briggsSet(exposed, tail->dc.left->offset2->sp->i);
                        }
                    }
                if (tail->temps & TEMP_RIGHT)
                    if (tail->dc.right->mode == i_ind || tail->dc.right->mode == i_direct)
                    {
                        if (tail->dc.right->offset)
                            briggsSet(exposed, tail->dc.right->offset->sp->i);
                        if (tail->dc.right->offset2)
                            briggsSet(exposed, tail->dc.right->offset2->sp->i);
                    }
                tail = tail->back;
            }
        }
    }
    {
        static int count = 0;
    }
}
static void InitClasses(void)
{
    int i;
    for (i = 0; i < tempCount; i++)
    {
        if (tempInfo[i]->inUse)
        {
            if (!tempInfo[i]->liveAcrossFunctionCall)
            {
                tempInfo[i]->regClass = chosenAssembler->arch->regClasses[abs(tempInfo[i]->size) * 2];
            }
            else
            {
                tempInfo[i]->regClass = chosenAssembler->arch->regClasses[abs(tempInfo[i]->size) * 2 + 1];
            }
        }
        if (!tempInfo[i]->regClass)
            tempInfo[i]->doGlobal = false;
    }
}
static void CountInstructions(bool first)
{
    QUAD* head = intermed_head;
    int i;
    for (i = 0; i < tempCount; i++)
    {
        TEMP_INFO* t = tempInfo[i];
        t->doGlobal = false;
    }
    instructionCount = 0;
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
                head->index = instructionCount++;
                if (head->temps & TEMP_ANS)
                {
                    if (head->ans->offset)
                    {
                        tempInfo[head->ans->offset->sp->i]->doGlobal = true;
                    }
                    if (head->ans->offset2)
                    {
                        tempInfo[head->ans->offset2->sp->i]->doGlobal = true;
                    }
                }
                if (head->temps & TEMP_LEFT)
                {
                    if (head->dc.left->offset)
                    {
                        tempInfo[head->dc.left->offset->sp->i]->doGlobal = true;
                    }
                    if (head->dc.left->offset2)
                    {
                        tempInfo[head->dc.left->offset2->sp->i]->doGlobal = true;
                    }
                }
                if (head->temps & TEMP_RIGHT)
                {
                    if (head->dc.right->offset)
                    {
                        tempInfo[head->dc.right->offset->sp->i]->doGlobal = true;
                    }
                    if (head->dc.right->offset2)
                    {
                        tempInfo[head->dc.right->offset2->sp->i]->doGlobal = true;
                    }
                }
                break;
        }
        head = head->fwd;
    }
    for (i = 0; i < tempCount; i++)
        tempInfo[i]->inUse = tempInfo[i]->doGlobal;
    instructionByteCount = (instructionCount + BITINTBITS - 1) / BITINTBITS;

    instructionCount += 1000;
    workingMoves = tallocbit(instructionCount);
    activeMoves = tallocbit(instructionCount);
    coalescedMoves = aallocbit(instructionCount);
    constrainedMoves = tallocbit(instructionCount);
    frozenMoves = tallocbit(instructionCount);
    tempMoves[0] = tallocbit(instructionCount);
    tempMoves[1] = tallocbit(instructionCount);
    hiMoves = tAllocate<short>(instructionCount);
    instructionList = tAllocate<QUAD*>(instructionCount);
    instructionCount -= 1000;
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
    int i, t;
    int K;
    Adjacent1(u);
    Adjacent(v);
    for (i = 0; i < (tempCount + BITINTBITS - 1) / BITINTBITS; i++)
    {
        // this is endian dependent sad to say... the new compiler ought to do things in 32 bit words ALWAYS
        BITINT x = adjacent[i] |= adjacent1[i];
        if (x)
        {
            int n = i * BITINTBITS;
            for (t = n; t < n + BITINTBITS; t++, x >>= 1)
                if (x & 1)
                    if (tempInfo[t]->squeeze >= tempInfo[t]->regCount)
                        k++;
        }
    }
    K = imin(tempInfo[u]->regClass->regCount, tempInfo[v]->regClass->regCount);
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
    int k = 0, i, t;
    Adjacent(v);
    for (i = 0; i < (tempCount + BITINTBITS - 1) / BITINTBITS; i++)
        if (adjacent[i])
            for (t = i * BITINTBITS; t < i * BITINTBITS + BITINTBITS; t++)
                if (isset(adjacent, t))
                    if (tempInfo[t]->squeeze >= tempInfo[t]->regCount)
                    {
                        if (!isConflicting(u, t))
                        {
                            k++;
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
/* we are doing the conflict graph (interference graph)
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
static void Build(BLOCK* b)
{
    QUAD* head;
    BLOCKLIST* bl;
    int i;
    if (b == nullptr)
    {
        for (i = 0; i < tempCount; i++)
        {
            TEMP_INFO* t = tempInfo[i];
            t->workingMoves = nullptr;
            t->spillCost = 0;
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
                    if (tempInfo[u]->regClass && (tempInfo[u]->regClass == tempInfo[v]->regClass ||
                                                  fsizeFromISZ(tempInfo[u]->size) == fsizeFromISZ(tempInfo[v]->size)))
                    {
                        TEMP_INFO* t;
                        setbit(workingMoves, head->index);
                        t = tempInfo[u];
                        if (!t->workingMoves)
                            t->workingMoves = tallocbit(instructionCount);
                        setbit(t->workingMoves, head->index);

                        t = tempInfo[v];
                        if (!t->workingMoves)
                            t->workingMoves = tallocbit(instructionCount);
                        setbit(t->workingMoves, head->index);
                        if (tempInfo[u]->precolored)
                        {
                            if (!tempInfo[v]->precolored)
                                GeorgeCoalesceInit(u, v, head->index);
                        }
                        else if (tempInfo[v]->precolored)
                            GeorgeCoalesceInit(v, u, head->index);
                        else
                            BriggsCoalesceInit(u, v, head->index);
                    }
                }
            }
        }
        if (head->temps & TEMP_ANS)
        {
            if (head->ans->offset)
            {
                tempInfo[head->ans->offset->sp->i]->spillCost += head->block->spillCost;
            }
            if (head->ans->offset2)
            {
                tempInfo[head->ans->offset2->sp->i]->spillCost += head->block->spillCost;
            }
        }
        if (head->temps & TEMP_LEFT)
        {
            if (head->dc.left->offset)
            {
                tempInfo[head->dc.left->offset->sp->i]->spillCost += head->block->spillCost;
                if (!tiny(head, head->dc.left))
                    tempInfo[head->dc.left->offset->sp->i]->temp = 1;
            }
            if (head->dc.left->offset2)
            {
                tempInfo[head->dc.left->offset2->sp->i]->spillCost += head->block->spillCost;
                tempInfo[head->dc.left->offset2->sp->i]->temp = 1;
            }
        }
        if (head->temps & TEMP_RIGHT)
        {
            if (head->dc.right->offset)
            {
                tempInfo[head->dc.right->offset->sp->i]->spillCost += head->block->spillCost;
                if (!tiny(head, head->dc.right))
                    tempInfo[head->dc.right->offset->sp->i]->temp = 1;
            }
            if (head->dc.right->offset2)
            {
                tempInfo[head->dc.right->offset2->sp->i]->spillCost += head->block->spillCost;
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
    BITINT* confl = tempInfo[n]->conflicts;
    int i;
    int x = (tempCount + BITINTBITS - 1) / BITINTBITS;
    memset(adjacent, 0, x * sizeof(BITINT));
    for (i = 0; i < x; i++)
    {
        BITINT v;
        if (confl[i])
        {
            adjacent[i] = confl[i] & ~(stackedTemps[i] | coalescedNodes[i]);
        }
    }
}
static void Adjacent1(int n)
{
    BITINT* confl = tempInfo[n]->conflicts;
    int i;
    int x = (tempCount + BITINTBITS - 1) / BITINTBITS;
    memset(adjacent1, 0, x * sizeof(BITINT));
    for (i = 0; i < x; i++)
    {
        BITINT v;
        if (confl[i])
        {
            adjacent1[i] = confl[i] & ~(stackedTemps[i] | coalescedNodes[i]);
        }
    }
}
static BITARRAY* NodeMoves(int n, int index)
{
    if (tempInfo[n]->workingMoves)
    {
        int i;
        BITARRAY* rv = tempMoves[index];
        BITINT* p = bits(rv);
        BITINT* w = bits(tempInfo[n]->workingMoves);
        memset(p, 0, instructionByteCount * sizeof(BITINT));
        for (i = 0; i < instructionByteCount; i++)
        {
            if (w[i])
                p[i] = w[i] & (activeMoves[i] | workingMoves[i]);
        }
        return rv;
    }
    return nullptr;
}
static bool MoveRelated(int n, int index)
{
    BITARRAY* data = NodeMoves(n, index);
    if (data)
    {
        int i;
        for (i = 0; i < instructionByteCount; i++)
            if (bits(data)[i])
                return true;
    }
    return false;
}
static void MkWorklist(void)
{
    int i;
    /* going backwatds to get spill temps first */
    for (i = 0; i < tempCount; i++)
    {
        TEMP_INFO* t = tempInfo[i];
        if (t->doGlobal && !t->precolored && t->ircinitial)
        {
            t->doGlobal = false;
            t->ircinitial = false;
            if (t->squeeze >= t->regCount)
            {
                briggsSet(spillWorklist, i);
            }
            else if (MoveRelated(i, 0))
            {
                briggsSet(freezeWorklist, i);
            }
            else
            {
                simplifyWorklist[simplifyTop++] = i;
            }
        }
    }
}
static void EnableMoves(BITINT* nodes, int index)
{
    int i, t;
    for (i = 0; i < (tempCount + BITINTBITS - 1) / BITINTBITS; i++)
        if (nodes[i])
            for (t = i * BITINTBITS; t < i * BITINTBITS + BITINTBITS; t++)
                if (isset(nodes, t))
                {
                    BITARRAY* nm = NodeMoves(t, index);
                    int j;
                    if (nm)
                    {
                        for (j = 0; j < instructionByteCount; j++)
                        {
                            if (bits(activeMoves)[j] & bits(nm)[j])
                            {
                                int k;
                                for (k = j * BITINTBITS; k < j * BITINTBITS + BITINTBITS; k++)
                                {
                                    if (isset(activeMoves, k) && isset(nm, k))
                                    {
                                        --hiMoves[k];
                                        if (hiMoves[k] <= 0)
                                        {
                                            clearbit(activeMoves, k);
                                            setbit(workingMoves, k);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
}
static void DecrementDegree(int m, int n)
{
    if (!tempInfo[m]->precolored)
    {
        int cur = tempInfo[m]->squeeze;
        tempInfo[m]->squeeze +=
            SqueezeChange(m, tempInfo[n]->regClass->vertex,
                          -(int)worstCase[tempInfo[m]->regClass->index * classCount + tempInfo[n]->regClass->index]);
        tempInfo[m]->degree--;
        if (cur == tempInfo[m]->regCount && tempInfo[m]->squeeze < tempInfo[m]->regCount)
        {
            Adjacent(m);
            //            setbit(adjacent, m);
            EnableMoves(adjacent, 1);
            briggsReset(spillWorklist, m);
            if (MoveRelated(m, 1))
            {
                briggsSet(freezeWorklist, m);
            }
            else
            {
                simplifyWorklist[simplifyTop++] = m;
            }
        }
    }
}
static void Simplify(void)
{
    int n = simplifyWorklist[simplifyBottom++];
    int i, t;
    if (simplifyBottom == simplifyTop)
        simplifyBottom = simplifyTop = 0;
    if (tempInfo[n]->regClass)
    {
        PUSH(n);
        Adjacent1(n);
        briggsReset(freezeWorklist, n);  // DAL I added this because it seemed needed
        if (tempInfo[n]->squeeze >= tempInfo[n]->regCount)
            EnableMoves(adjacent1, 0);
        for (i = 0; i < (tempCount + BITINTBITS - 1) / BITINTBITS; i++)
            if (adjacent1[i])
                for (t = i * BITINTBITS; t < i * BITINTBITS + BITINTBITS; t++)
                    if (isset(adjacent1, t))
                        DecrementDegree(t, n);
    }
}
static void AddWorkList(int u)
{
    if (!tempInfo[u]->precolored && !MoveRelated(u, 1) && tempInfo[u]->squeeze < tempInfo[u]->regCount)
    {
        briggsReset(freezeWorklist, u);
        simplifyWorklist[simplifyTop++] = u;
    }
}
static int Combine(int u, int v)
{
    int i, t;
    unsigned z;
    int max = (tempCount + BITINTBITS - 1) / BITINTBITS;
    bool losingHiDegreeNode;
    BITARRAY *tu, *tv;
    /*
    if (!tempInfo[v]->precolored && !tempInfo[u]->precolored)
        if (tempInfo[v]->neighbors > tempInfo[u]->neighbors)
        {
            int n = v;
            v = u;
            u = n;
        }
    */
    if (briggsTest(freezeWorklist, v))
        briggsReset(freezeWorklist, v);
    else
        briggsReset(spillWorklist, v);
    setbit(coalescedNodes, v);
    tempInfo[v]->partition = u;
    // assumes that the more stringent set has less elements
    if (tempInfo[v]->liveAcrossFunctionCall)
    {
        tempInfo[u]->regClass = tempInfo[v]->regClass;
        tempInfo[u]->liveAcrossFunctionCall = true;
    }
    else if (tempInfo[u]->liveAcrossFunctionCall)
    {
        tempInfo[v]->regClass = tempInfo[u]->regClass;
        tempInfo[v]->liveAcrossFunctionCall = true;
    }
    if (tempInfo[u]->precolored)
    {
        BITARRAY* nm = tempInfo[v]->workingMoves;
        int j;
        for (j = 0; j < instructionByteCount; j++)
        {
            int z;
            if ((z = bits(nm)[j]))
            {
                int k;
                int m = j * BITINTBITS;
                for (k = m; k < m + BITINTBITS; k++, z >>= 1)
                {
                    if ((z & 1) && instructionList[k])
                    {
                        int x = instructionList[k]->ans->offset->sp->i;
                        int y = instructionList[k]->dc.left->offset->sp->i;
                        if (!tempInfo[x]->precolored && !tempInfo[y]->precolored)
                        {
                            hiMoves[k] = 0;
                            setbit(tempInfo[u]->workingMoves, k);
                        }
                    }
                }
            }
        }
    }
    else
    {
        tu = tempInfo[u]->workingMoves;
        tv = tempInfo[v]->workingMoves;
        if (tv)
        {
            if (!tu)
            {
                tempInfo[u]->workingMoves = tu = tallocbit(instructionCount);  // DAL this was modified....
            }
            for (i = 0; i < instructionByteCount; i++)
                (bits(tu))[i] |= (bits(tv))[i];
        }
    }
    losingHiDegreeNode = tempInfo[u]->squeeze >= tempInfo[u]->regCount && tempInfo[v]->squeeze >= tempInfo[v]->regCount;
    for (i = 0; i < max; i++)
        if ((z = tempInfo[v]->conflicts[i] & ~coalescedNodes[i]))
            for (t = i * BITINTBITS; t < i * BITINTBITS + BITINTBITS; t++, z >>= 1)
                if (z & 1)
                {
                    if (t != u && !isConflicting(t, u))
                    {
                        insertConflict(t, u);
                        if (!tempInfo[u]->precolored)
                        {
                            tempInfo[u]->squeeze +=
                                SqueezeChange(u, tempInfo[t]->regClass->vertex,
                                              +worstCase[tempInfo[u]->regClass->index * classCount + tempInfo[t]->regClass->index]);
                            tempInfo[u]->degree++;
                        }
                        if (!tempInfo[t]->precolored)
                        {
                            tempInfo[t]->squeeze +=
                                SqueezeChange(t, tempInfo[u]->regClass->vertex,
                                              +worstCase[tempInfo[t]->regClass->index * classCount + tempInfo[u]->regClass->index]);
                            tempInfo[t]->degree++;
                        }
                    }
                    DecrementDegree(t, v);
                }
    if (tempInfo[u]->squeeze >= tempInfo[u]->regCount)
    {
        if (briggsTest(freezeWorklist, u))
        {
            briggsReset(freezeWorklist, u);
            briggsSet(spillWorklist, u);
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
    for (i = 0; i < (tempCount + BITINTBITS - 1) / BITINTBITS; i++)
        if (adjacent[i])
            for (t = i * BITINTBITS; t < i * BITINTBITS + BITINTBITS; t++)
                if (isset(adjacent, t))
                    if (!tempInfo[t]->precolored && tempInfo[t]->squeeze >= tempInfo[t]->regCount && !isConflicting(t, u))
                        return false;
    return true;
}
static bool Conservative(int u, int v)
{
    int i, t, k = 0;
    Adjacent1(u);
    Adjacent(v);
    for (i = 0; i < (tempCount + BITINTBITS - 1) / BITINTBITS; i++)
    {
        adjacent[i] |= adjacent1[i];
        if (adjacent[i])
            for (t = i * BITINTBITS; t < i * BITINTBITS + BITINTBITS; t++)
                if (isset(adjacent, t))
                    if (tempInfo[t]->squeeze >= tempInfo[t]->regCount)
                        ++k;
    }
    return k < tempInfo[u]->regClass->regCount && k < tempInfo[v]->regClass->regCount;
}
static bool conflictsWithSameColor(int u, int v)
{
    if (tempInfo[u]->precolored)
    {
        int i, j;
        BITINT* confl = tempInfo[v]->conflicts;
        for (i = 0; i < (tempCount + BITINTBITS - 1) / BITINTBITS; i++)
            if (confl[i])
                for (j = i * BITINTBITS; j < i * BITINTBITS + BITINTBITS; j++)
                    if (isset(confl, j))
                    {
                        int k = findPartition(j);
                        if (tempInfo[k]->precolored && tempInfo[k]->color == tempInfo[u]->color)
                        {
                            return true;
                        }
                    }
    }
    return false;
}
static bool inBothRegClasses(int u, int v)
{
    if (tempInfo[u]->precolored)
    {
        int n = tempInfo[u]->color;
        int i;
        for (i = 0; i < tempInfo[v]->regCount; i++)
            if (n == tempInfo[v]->regClass->regs[i])
                return true;
        return false;
    }
    return true;
}
static bool Coalesce(void)
{
    int i;
    int sel = -1;

    for (i = 0; i < instructionByteCount; i++)
    {
        BITINT x;
        if ((x = (bits(workingMoves))[i]))
        {
            int j;
            for (j = 0; x; j++, x >>= 1)
            {
                if (x & 1)
                {
                    sel = i * BITINTBITS + j;
                    goto join;
                }
            }
        }
    }
join:
    if (sel != -1)
    {
        int u, v;
        u = findPartition(instructionList[sel]->ans->offset->sp->i);
        v = findPartition(instructionList[sel]->dc.left->offset->sp->i);
        clearbit(workingMoves, sel);
        clearbit(tempInfo[u]->workingMoves, sel);
        clearbit(tempInfo[v]->workingMoves, sel);
        if (tempInfo[v]->precolored || (!tempInfo[u]->precolored && tempInfo[u]->spilled))
        {
            int n = u;
            u = v;
            v = n;
        }
        if (u == v)
        {
            /* same reg, discard it */
            setbit(coalescedMoves, sel);
            AddWorkList(u);
        }
        else if (tempInfo[v]->precolored || isConflicting(u, v) || conflictsWithSameColor(u, v) || !inBothRegClasses(u, v))
        {
            /* both precolored or otherwise in conflict, can't do anything with it */
            setbit(constrainedMoves, sel);
            AddWorkList(u);
            AddWorkList(v);
        }
        else if ((tempInfo[u]->precolored && AllOK(u, v)) || (!tempInfo[u]->precolored && Conservative(u, v)))
        {
            /* zero or one precolored reg, constraints ok, coalesce */
            setbit(coalescedMoves, sel);
            AddWorkList(Combine(u, v));
        }
        else
        {
            /* zero or one precolored reg, constraints not ok, we can't coalesce at this time */
            setbit(activeMoves, sel);
            hiMoves[sel] = 0;
            if (tempInfo[u]->precolored)
            {
                if (!tempInfo[v]->precolored)
                    GeorgeCoalesceInit(u, v, sel);
            }
            else if (tempInfo[v]->precolored)
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
    BITARRAY* nm;
    int i;
    nm = NodeMoves(u, 0);
    if (nm)
    {
        for (i = 0; i < instructionByteCount; i++)
        {
            if (bits(nm)[i])
            {
                int j;
                for (j = i * BITINTBITS; j < i * BITINTBITS + BITINTBITS; j++)
                {
                    if (isset(nm, j))
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
                            if (isset(activeMoves, j))
                            {
                                clearbit(activeMoves, j);
                            }
                            else
                            {
                                clearbit(workingMoves, j);
                                clearbit(tempInfo[v]->workingMoves, j);
                                clearbit(tempInfo[u]->workingMoves, j);
                            }
                            setbit(frozenMoves, j);
                            if (tempInfo[v]->squeeze < tempInfo[v]->regCount && !MoveRelated(v, 1))
                            {
                                briggsReset(freezeWorklist, v);
                                simplifyWorklist[simplifyTop++] = v;
                            }
                        }
                    }
                }
            }
        }
    }
}
static void Freeze(void)
{
    int u = freezeWorklist->data[0];
    briggsReset(freezeWorklist, u);
    simplifyWorklist[simplifyTop++] = u;
    FreezeMoves(u);
}
static unsigned SpillCost(int i)
{
    int cost;
    LIST* def = tempInfo[i]->idefines;
    LIST* use = tempInfo[i]->iuses;
    if (!def || !use)
        return UINT_MAX;
    if (!def->next && !use->next)
    {
        QUAD* q = (QUAD*)use->data;
        if (((QUAD*)def->data)->index == q->index - 1)
            if (!(q->temps & TEMP_ANS) && q->dc.opcode == i_assn && q->ans->offset->type != se_add && q->ans->mode == i_direct &&
                (q->ans->offset->sp && q->ans->offset->sp->spillVar))
                return UINT_MAX;
    }
    //	if (!use)
    //	{
    //		return tempInfo[i]->regCount - tempInfo[i]->squeeze;
    //	}
    cost = tempInfo[i]->spillCost;  // tempInfo[i]->degree;
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
    for (i = 0; i < spillWorklist->top; i++)
    {
        int s = spillWorklist->data[i];
        if (!tempInfo[s]->spilled)
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
        m = spillWorklist->data[0];
        sz = 0;
        // no good candidates, select the first...
        for (i = 0; i < spillWorklist->top; i++)
        {
            int s = spillWorklist->data[i];
            if (!tempInfo[s]->spilled && sizeFromISZ(tempInfo[s]->size) > sz)
            {
                m = s;
                sz = sizeFromISZ(tempInfo[s]->size);
            }
        }
    }
    briggsReset(spillWorklist, m);
    if (tempInfo[m]->triedSpill)
        briggsSet(spilledNodes, m);
    else
        simplifyWorklist[simplifyTop++] = m;
    tempInfo[m]->triedSpill = true;
    FreezeMoves(m);
}
static void AssignColors(void)
{
    int n;
    int i, j;
    int m;
    UBYTE* order = chosenAssembler->arch->regOrder;
    int count = *order++;
    {
        int pos;
        for (pos = tempStackcount - 1; pos >= 0; pos--)
        {
            n = tempStack[pos];
            if (n != -1)
            {
                bool regs[MAX_INTERNAL_REGS];
                BITINT* confl = tempInfo[n]->conflicts;
                ARCH_REGCLASS* cls = tempInfo[n]->regClass;
                tempStack[pos] = -1;
                for (i = 0; i < sizeof(regs); i++)
                    regs[i] = true;
                for (i = 0; i < (tempCount + BITINTBITS - 1) / BITINTBITS; i++)
                    if (confl[i])
                        for (j = i * BITINTBITS; j < i * BITINTBITS + BITINTBITS; j++)
                            if (isset(confl, j))
                            {
                                int u = findPartition(j);
                                if (tempInfo[u]->color >= 0)
                                {
                                    int x = tempInfo[u]->color;
                                    int k;
                                    regs[x] = false;
                                    for (k = 0; k < chosenAssembler->arch->regNames[x].aliasCount; k++)
                                    {
                                        regs[chosenAssembler->arch->regNames[x].aliases[k]] = false;
                                    }
                                }
                            }
                for (i = 0; i < cls->regCount; i++)
                    if (regs[cls->regs[i]])
                    {
                        tempInfo[n]->color = cls->regs[i];
                        tempInfo[n]->enode->sp->regmode = 2;
                        break;
                    }
                if (i == cls->regCount)
                {
                    briggsSet(spilledNodes, n);
                }
            }
        }
    }
    tempStackcount = 0;
    for (i = 0; i < tempCount; i++)
        tempInfo[i]->color = tempInfo[findPartition(i)]->color;
}
static IMODE* InsertLoad(QUAD* head, IMODE* mem)
{
    QUAD* insert;
    IMODE* t = InitTempOpt(mem->size, mem->size);
    tempInfo[t->offset->sp->i]->spilled = true;
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
    tempInfo[tn]->spilled = true;
    tempInfo[tn]->precolored = tempInfo[ta]->precolored;
    tempInfo[tn]->enode->sp->regmode = tempInfo[ta]->enode->sp->regmode;
    tempInfo[tn]->color = tempInfo[ta]->color;
    tempInfo[tn]->ircinitial = true;
    *im = t;
    insert = Allocate<QUAD>();
    insert->dc.opcode = i_assn;
    insert->ans = mem;
    insert->dc.left = t;
    insert->block = head->block;
    insert->index = -1;
    if (tempInfo[tn]->precolored)
        insert->precolored |= TEMP_LEFT;
    InsertInstruction(head, insert);
    accesses++;
}
static void RewriteAllSpillNodes(void)
{
    QUAD* head = intermed_head;
    int spillVars[6], i, j;
    IMODE* spillNodes[6];
    //    IMODE *mem = tempInfo[n]->spillVar;
    while (head)
    {
        IMODE* t;
        memset(spillVars, 0xff, sizeof(spillVars));
        memset(spillNodes, 0, sizeof(spillNodes));
        if (head->temps & TEMP_ANS)
        {
            if (head->ans->mode == i_direct)
            {
                if (tempInfo[head->ans->offset->sp->i]->spilling)
                {
                    InsertStore(head, &head->ans, tempInfo[head->ans->offset->sp->i]->spillVar);
                }
            }
            else
            {
                if (head->ans->offset && head->ans->offset->type == se_tempref && tempInfo[head->ans->offset->sp->i]->spilling)

                    spillVars[0] = head->ans->offset->sp->i;
                if (head->ans->offset2 && head->ans->offset2->type == se_tempref && tempInfo[head->ans->offset2->sp->i]->spilling)
                    spillVars[1] = head->ans->offset2->sp->i;
            }
        }
        if (head->temps & TEMP_LEFT)
        {
            if (head->dc.left->offset && head->dc.left->offset->type == se_tempref &&
                tempInfo[head->dc.left->offset->sp->i]->spilling)
                spillVars[2] = head->dc.left->offset->sp->i;
            if (head->dc.left->offset2 && head->dc.left->offset2->type == se_tempref &&
                tempInfo[head->dc.left->offset2->sp->i]->spilling)
                spillVars[3] = head->dc.left->offset2->sp->i;
        }
        if (head->temps & TEMP_RIGHT)
        {
            if (head->dc.right->offset && head->dc.right->offset->type == se_tempref &&
                tempInfo[head->dc.right->offset->sp->i]->spilling)
                spillVars[4] = head->dc.right->offset->sp->i;
            if (head->dc.right->offset2 && head->dc.right->offset2->type == se_tempref &&
                tempInfo[head->dc.right->offset2->sp->i]->spilling)
                spillVars[5] = head->dc.right->offset2->sp->i;
        }
        // this is all to guard against loading the same var two or more times for the same instruction
        // which is both optimally bad and can also lead to insidious bugs
        // because of some assumptions made by the rewrite module
        for (i = 0; i < 6; i++)
            if (spillVars[i] != -1)
            {
                for (j = 0; j < i; j++)
                    if (spillVars[j] == spillVars[i])
                    {
                        break;
                    }
                if (j >= i)
                {
                    spillNodes[i] = InsertLoad(head, tempInfo[spillVars[i]]->spillVar);
                }
                else
                {
                    spillNodes[i] = spillNodes[j];
                }
            }
        if (spillNodes[0] || spillNodes[1])
        {
            IMODE* im = Allocate<IMODE>();
            *im = *head->ans;
            head->ans = im;
        }
        if (spillNodes[0])
            head->ans->offset = spillNodes[0]->offset;
        if (spillNodes[1])
            head->ans->offset2 = spillNodes[1]->offset;
        if (spillNodes[2] || spillNodes[3])
        {
            IMODE* im = Allocate<IMODE>();
            *im = *head->dc.left;
            head->dc.left = im;
        }
        if (spillNodes[2])
            head->dc.left->offset = spillNodes[2]->offset;
        if (spillNodes[3])
            head->dc.left->offset2 = spillNodes[3]->offset;
        if (spillNodes[4] || spillNodes[5])
        {
            IMODE* im = Allocate<IMODE>();
            *im = *head->dc.right;
            head->dc.right = im;
        }
        if (spillNodes[4])
            head->dc.right->offset = spillNodes[4]->offset;
        if (spillNodes[5])
            head->dc.right->offset2 = spillNodes[5]->offset;
        head = head->fwd;
    }
}
/* give any remaining spill vars a color */
static void SpillColor(void)
{
    int i;
    for (i = 0; i < spilledNodes->top; i++)
    {
        int n = spilledNodes->data[i];
        if (!tempInfo[n]->spillVar)
            GetSpillVar(n);
    }
}
/* attempt to use the same spill var for any spilled variables
 * which are connected by an uncoalesced move
 */
static void SpillCoalesce(BRIGGS_SET* C, BRIGGS_SET* S)
{
    typedef struct _move_
    {
        struct _move_* next;
        int a;
        int b;
        int cost;
        LIST* uses;
    } MOVE;
    MOVE *moves = nullptr, **mt;
    int i;
    for (i = 0; i < S->top; i++)
    {
        int n = S->data[i];
        BITARRAY* nm = tempInfo[n]->workingMoves;
        int j;
        if (nm)
        {
            for (j = 0; j < instructionByteCount; j++)
            {
                if (bits(nm)[j] & ~bits(coalescedMoves)[j])
                {
                    int k;
                    for (k = j * BITINTBITS; k < j * BITINTBITS + BITINTBITS; k++)
                    {
                        if (isset(nm, k) && !isset(coalescedMoves, k))
                        {
                            QUAD* head = instructionList[k];
                            if (head)
                            {
                                bool test;
                                int a = head->ans->offset->sp->i;
                                int b = head->dc.left->offset->sp->i;
                                if (fsizeFromISZ(tempInfo[a]->size) == fsizeFromISZ(tempInfo[b]->size) &&
                                    !tempInfo[a]->directSpill && !tempInfo[b]->directSpill)
                                {
                                    {
                                        if (n == a)
                                            test = briggsTest(S, b);
                                        else
                                            test = briggsTest(S, a);
                                        if (test)
                                        {
                                            if (briggsTest(C, a) || briggsTest(C, b))
                                            {
                                                if (a > b)
                                                {
                                                    int temp = a;
                                                    a = b;
                                                    b = temp;
                                                }
                                                mt = &moves;
                                                while (*mt)
                                                {
                                                    if ((*mt)->a == a && (*mt)->b == b)
                                                    {
                                                        LIST** l = &(*mt)->uses;
                                                        while (*l)
                                                        {
                                                            if ((*l)->data == (void*)head)
                                                                break;
                                                            l = &(*l)->next;
                                                        }
                                                        if (!*l)
                                                        {
                                                            *l = tAllocate<LIST>();
                                                            (*l)->data = (void*)head;
                                                            (*mt)->cost += head->block->spillCost;
                                                        }
                                                        break;
                                                    }
                                                    mt = &(*mt)->next;
                                                }
                                                if (!*mt)
                                                {
                                                    *mt = tAllocate<MOVE>();
                                                    (*mt)->a = a;
                                                    (*mt)->b = b;
                                                    (*mt)->cost = head->block->spillCost;
                                                    (*mt)->uses = tAllocate<LIST>();
                                                    (*mt)->uses->data = (void*)head;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    while (moves)
    {
        unsigned cost = 0;
        MOVE **mv = nullptr, *ml;
        mt = &moves;
        while (*mt)
        {
            if (cost < (*mt)->cost)
            {
                mv = mt;
                cost = (*mt)->cost;
            }
            mt = &(*mt)->next;
        }
        ml = *mv;
        *mv = (*mv)->next;
        if (!isConflicting(ml->a, ml->b))
        {
            LIST* uses = ml->uses;
            if (tempInfo[ml->a]->spillVar)
            {
                if (tempInfo[ml->b]->spillVar)  // just in case...
                {
                    // deallocate one of the spills
                    QUAD* head;
                    IMODE* spill = tempInfo[ml->b]->spillVar;
                    IMODE* nw = tempInfo[ml->a]->spillVar;
                    int i;
                    if (nw != spill)
                    {
                        spill->offset->sp->allocate = false;
                        // make sure all uses of the deallocated spill get renamed
                        for (i = 0; i < tempCount; i++)
                            if (tempInfo[i]->spillVar == spill)
                                tempInfo[i]->spillVar = nw;
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
                    tempInfo[ml->b]->spillVar = tempInfo[ml->a]->spillVar;
            }
            else if (tempInfo[ml->b]->spillVar)
            {
                tempInfo[ml->a]->spillVar = tempInfo[ml->b]->spillVar;
            }
            else
            {
                GetSpillVar(ml->a);
                tempInfo[ml->b]->spillVar = tempInfo[ml->a]->spillVar;
            }
            JoinConflictLists(ml->a, ml->b);
        }
    }
}
static void InsertCandidates(int W, BRIGGS_SET* L)
{
    BITARRAY* moves = tempInfo[W]->workingMoves;
    int i;
    if (moves)
    {
        for (i = 0; i < instructionByteCount; i++)
        {
            if (bits(moves)[i] & ~bits(coalescedMoves)[i])
            {
                int j;
                for (j = i * BITINTBITS; j < i * BITINTBITS + BITINTBITS; j++)
                    if (isset(moves, j) && !isset(coalescedMoves, j))
                    {
                        QUAD* head = instructionList[j];
                        if (head)
                        {
                            int n = head->ans->offset->sp->i;
                            if (n == W)
                                n = head->dc.left->offset->sp->i;
                            if (!tempInfo[n]->spilled && !briggsTest(spillProcessed, n))
                            {
                                briggsSet(L, n);
                                briggsSet(spillProcessed, n);
                            }
                        }
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
            return 0;
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
static unsigned SpillPropagateSavings(int w, BRIGGS_SET* S)
{
    unsigned savings = 0;
    BITARRAY* moves = tempInfo[w]->workingMoves;
    int i;
    for (i = 0; i < instructionByteCount; i++)
    {
        if (bits(moves)[i] & ~bits(coalescedMoves)[i])
        {
            int j;
            for (j = i * BITINTBITS; j < i * BITINTBITS + BITINTBITS; j++)
                if (isset(moves, j) && !isset(coalescedMoves, j))
                {
                    QUAD* head = instructionList[j];
                    if (head)
                    {
                        int n = head->ans->offset->sp->i;
                        if (n == w)
                            n = head->dc.left->offset->sp->i;
                        if (briggsTest(S, n) && !isConflicting(n, w))
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
    }
    return savings;
}
static void SpillPropagate(BRIGGS_SET* P, BRIGGS_SET* S, BRIGGS_SET* L, BRIGGS_SET* NP)
{
    int i;
    briggsClear(L);
    briggsClear(NP);
    for (i = 0; i < P->top; i++)
    {
        InsertCandidates(P->data[i], L);
    }
    while (L->top)
    {
        int w = L->data[0];
        briggsReset(L, w);
        if (SpillPropagateSavings(w, S) >= tempInfo[w]->spillCost)
        {
            briggsSet(S, w);
            briggsSet(NP, w);
            InsertCandidates(w, L);
        }
    }
    briggsClear(P);
    for (i = 0; i < NP->top; i++)
    {
        briggsSet(P, NP->data[i]);
    }
}
static void SpillPropagateAndCoalesce(void)
{
    BRIGGS_SET* P = briggsAlloct(tempCount);
    BRIGGS_SET* np = briggsAlloct(tempCount);
    BRIGGS_SET* L = briggsAlloct(tempCount);
    int i;
    spillList = nullptr;
    spillProcessed = briggsAlloct(tempCount);
    for (i = 0; i < spilledNodes->top; i++)
        briggsSet(P, spilledNodes->data[i]);
    do
    {
        SpillCoalesce(P, spilledNodes);
        SpillPropagate(P, spilledNodes, L, np);
    } while (P->top);
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
    for (i = 0; i < tempCount; i++)
    {
        tempInfo[i]->spilling = false;
        tempInfo[i]->iuTemp = tempInfo[i]->inUse;
    }
    for (i = 0; i < spilledNodes->top; i++)
    {
        int n = spilledNodes->data[i];
        tempInfo[n]->spilling = true;
        tempInfo[n]->spilled = true;
    }
    RewriteAllSpillNodes();
    for (i = 0; i < tempCount; i++)
    {
        if (tempInfo[i]->color != -1 || isset(coalescedNodes, i))
            tempInfo[i]->ircinitial = true;
    }
    /*
    for (i=0; i < tempCount; i++)
    {
            tempInfo[i]->doGlobal = tempInfo[i]->iuTemp != tempInfo[i]->inUse
                briggsSet(coalescedNodes, i)
                || tempInfo[i]->color >= 0;
    }
    */
    // letting restart of the algorithm clear necessary variables
}
static IMODE* copyImode(IMODE* in)
{
    IMODE* im = Allocate<IMODE>();
    *im = *in;
    if (im->offset)
    {
        int tnum = im->offset->sp->i;
        if (tnum < spilledNodes->size && isset(coalescedNodes, tnum))
        {
            im->offset = tempInfo[findPartition(tnum)]->enode;
        }
    }
    if (im->offset2)
    {
        int tnum = im->offset2->sp->i;
        if (tnum < spilledNodes->size && isset(coalescedNodes, tnum))
        {
            im->offset2 = tempInfo[findPartition(tnum)]->enode;
        }
    }
    return im;
}
static void KeepCoalescedNodes(void)
{
    QUAD* head = intermed_head;
    int i;
    while (head)
    {
        QUAD* next = head->fwd;
        if (head->index != -1 && isset(coalescedMoves, head->index))
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
    bitarrayClear(coalescedNodes, tempCount);
    bitarrayClear(coalescedMoves, instructionCount);
    for (i = 0; i < tempCount; i++)
        tempInfo[i]->partition = i;
}
static int LoopNesting(LOOP* loop)
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
    for (i = 0; i < tempCount; i++)
    {
        tempInfo[i]->color = -1;
        tempInfo[i]->precolored = false;
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
        tempInfo[i]->ircinitial = !tempInfo[i]->precolored;
    }
}
void retemp(void)
{
    unsigned short map[65536];
    int i, cur = 0;
    QUAD* head;
    memset(map, 0, sizeof(map));
    for (i = 0; i < tempCount; i++)
    {
        tempInfo[i]->inUse = false;
    }
    head = intermed_head;
    while (head)
    {
        if (head->dc.opcode != i_block && !head->ignoreMe && head->dc.opcode != i_passthrough && head->dc.opcode != i_label)
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
            cur++;
        }
    for (i = cur; i < tempCount; i++)
        tempInfo[i] = nullptr;
    tempCount = cur;
    head = intermed_head;
    while (head)
    {
        if (head->dc.opcode != i_block && !head->ignoreMe && head->dc.opcode != i_passthrough && head->dc.opcode != i_label)
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
    retemp();
    accesses = 0;
    Prealloc(3);
    CalculateNesting();
    Precolor(cparams.prm_optimize_for_speed || cparams.prm_optimize_for_size);
    while (true)
    {

        int i;
        passes++;
        if (passes >= 100)
            break;
        tFree();
        cFree();
        for (i = 0; i < tempCount; i++)
        {
            if (!tempInfo[i]->precolored)
            {
                tempInfo[i]->color = -1;
                if (tempInfo[i]->enode)
                    tempInfo[i]->enode->sp->regmode = 0;
            }
            tempInfo[i]->workingMoves = nullptr;
            tempInfo[i]->partition = i;
        }
        CountInstructions(first);
        simplifyBottom = simplifyTop = 0;
        tempCount += 3000;
        simplifyWorklist = tAllocate<unsigned short>(tempCount);
        freezeWorklist = briggsAlloct(tempCount);
        spillWorklist = briggsAlloct(tempCount);
        spilledNodes = briggsAlloct(tempCount);
        coalescedNodes = aallocbit(tempCount);
        adjacent = tallocbit(tempCount);
        adjacent1 = tallocbit(tempCount);
        stackedTemps = tallocbit(tempCount);
        tempStack = tAllocate<int>(tempCount);
        tempCount -= 3000;
        liveVariables();
        CalculateFunctionFlags();
        InitClasses();
        definesInfo();
        usesInfo();
        CalculateConflictGraph(nullptr, true);
        SqueezeInit();
        Build(nullptr);
        MkWorklist();
        for (i = 0; i < tempCount; i++)
        {
            if (tempInfo[i]->degree)
                tempInfo[i]->spillCost = tempInfo[i]->spillCost / tempInfo[i]->degree;
        }
        first = false;
        while (true)
        {
            if (simplifyBottom != simplifyTop)
                Simplify();
            else if (Coalesce())
                ;
            else if (freezeWorklist->top)
                Freeze();
            else if (spillWorklist->top != 0)
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
            if (!tempInfo[i]->precolored)
            {
                tempInfo[i]->color = -1;
                if (tempInfo[i]->enode)
                    tempInfo[i]->enode->sp->regmode = 0;
            }
        }
        AssignColors();
        if (spilledNodes->top != 0)
        {
            spills += spilledNodes->top;
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
    tFree();
    aFree();
    cFree();
    registersAssigned = true;
}
}  // namespace Optimizer