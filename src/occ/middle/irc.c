/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <limits.h>
#include "compiler.h"

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
extern BITINT bittab[BITINTBITS];
extern QUAD *intermed_head;
extern ARCH_ASM *chosenAssembler; 
extern COMPILER_PARAMS cparams;
extern TEMP_INFO **tempInfo;
extern int exitBlock, blockCount;
extern BLOCK **blockArray;
extern int tempCount;
extern int maxAllocationSpills, maxAllocationPasses, maxAllocationAccesses;
extern int has_double;
extern SYMBOL *theCurrentFunc;

int maxAddr = 0;
LIST *temporarySymbols;

typedef struct _spill_
{
    struct _spill_ *next;
    IMODE *imode;
    LIST *uses;
} SPILL;


static int *tempStack;
static int tempStackcount;
static int lc_maxauto;
static ULLONG_TYPE regmask;
static int localspill, spillcount;
static unsigned short *simplifyWorklist;
static int simplifyBottom, simplifyTop;
static BRIGGS_SET *freezeWorklist;
static BRIGGS_SET *spillWorklist;
static BRIGGS_SET *spilledNodes;
static BITARRAY *coalescedNodes;
static BITARRAY *stackedTemps;
static BITARRAY *adjacent, *adjacent1;
static BITARRAY *workingMoves;
static BITARRAY *activeMoves;
static BITARRAY *coalescedMoves;
static BITARRAY *constrainedMoves;
static BITARRAY *frozenMoves;
static BITARRAY *tempMoves[2];
static QUAD **instructionList;
static BRIGGS_SET *spillProcessed;
static short *hiMoves;
static SPILL *spillList;

static int classCount ,vertexCount;
static unsigned *worstCase;
static ARCH_REGCLASS **classes;
static ARCH_REGVERTEX **vertexes;

static int instructionCount;
static int instructionByteCount;

static int accesses;
#define PUSH(t) { tempStack[tempStackcount++] = t; setbit(stackedTemps, t); }
#define POP()  (tempStackcount == 0 ? -1 : tempStack[--tempStackcount]	)

static const UBYTE bitCounts[256] = 
{
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8, 
};

static void InitRegAliases(ARCH_REGDESC *desc)
{
    int i;
    for (i=0; i < REG_MAX; i++)
    {
        int j;
        desc[i].aliasBits = calloc(sizeof(BITINT), sizeof(BITARRAY) + (REG_MAX + BITINTBITS-1)/ BITINTBITS);
#ifdef TESTBITS
        desc[i].aliasBits->count = REG_MAX;
#endif
        setbit(desc[i].aliasBits, i);
        for (j= 0; j < desc[i].aliasCount; j++)
        {
            setbit(desc[i].aliasBits, desc[i].aliases[j]);
        }
    }
}
static void InitTree(ARCH_REGVERTEX *parent, ARCH_REGVERTEX *child)
{
    ARCH_REGCLASS *c = child->cls;
    ARCH_REGVERTEX *v;
    int i;
    child->parent = parent;
    child->index = vertexCount++;
    while (c)
    {
        c->vertex = child->index;
        c->index = classCount++;
        c->aliasBits = calloc(sizeof(BITINT), sizeof(BITARRAY) + (REG_MAX + BITINTBITS-1)/ BITINTBITS);
#ifdef TESTBITS
        c->aliasBits->count = REG_MAX;
#endif
        c->regBits = calloc(sizeof(BITINT), sizeof(BITARRAY) + (REG_MAX + BITINTBITS-1)/ BITINTBITS);
#ifdef TESTBITS
        c->regBits->count = REG_MAX;
#endif
        for (i=0; i < c->regCount; i++)
        {
            int j;
            setbit(c->regBits, c->regs[i]);
            for (j=0; j < (REG_MAX + BITINTBITS-1)/ BITINTBITS; j++)
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
static void LoadVertexes(ARCH_REGVERTEX *child)
{
    ARCH_REGCLASS *c = child->cls;
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
    int i,j, k, m;
    worstCase = calloc(classCount * classCount, sizeof(unsigned));
    for (i=0; i < classCount; i++)
    {
        for (j=0; j < classCount; j++)
        {
            int max = 0;
            for (k=0; k < classes[j]->regCount; k++)
            {
                int count = 0;
                for (m=0; m < (REG_MAX + BITINTBITS-1)/ BITINTBITS; m++)
                {
                    BITINT x = bits(classes[i]->regBits)[m] & bits(chosenAssembler->arch->regNames[classes[j]->regs[k]].aliasBits)[m];
                    int y;
                    for (y=0; y < sizeof(BITINT); y++)
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
static void LoadAliases(ARCH_REGVERTEX *v)
{
    ARCH_REGCLASS *c = v->cls;
    int i;
    int j;
    int k;
    if (v->left)
        LoadAliases(v->left);
    if (v->right)
        LoadAliases(v->right);
    v->aliasBits = calloc(sizeof(BITINT), sizeof(BITARRAY) + (REG_MAX + BITINTBITS-1)/ BITINTBITS);
#ifdef TESTBITS
    v->aliasBits->count = REG_MAX;
#endif
    if (v->left)
    {
        for (i=0; i < (REG_MAX + BITINTBITS-1)/ BITINTBITS; i++)
            bits(v->aliasBits)[i] |= bits(v->left->aliasBits)[i];
    }
    if (v->right)
    {
        for (i=0; i < (REG_MAX + BITINTBITS-1)/ BITINTBITS; i++)
            bits(v->aliasBits)[i] |= bits(v->right->aliasBits)[i];
    }
    while (c)
    {
        for (i=0; i < (REG_MAX + BITINTBITS-1)/ BITINTBITS; i++)
            bits(v->aliasBits)[i] |= bits(c->aliasBits)[i];
        c = c->next;
    }
}
void LoadSaturationBounds(void)
{
    int i,j;
    for (i=0; i < classCount; i++)
    {
        classes[i]->saturationBound = (short *)calloc(sizeof(unsigned short), vertexCount);
        for (j=0; j < vertexCount; j++)
        {
            int k;
            int rv = 0;
            // this MIGHT be endian dependent...
            for (k=0 ; k < (REG_MAX + BITINTBITS-1)/BITINTBITS; k++)
            {
                BITINT x = bits(classes[i]->regBits)[k] & bits(vertexes[j]->aliasBits)[k];
                int y;
                for (y=0; y < sizeof(BITINT); y++)
                {
                    rv += bitCounts[x & 0xff];
                    x >>= 8;
                }
            }
            classes[i]->saturationBound[j] = rv;
        }
    }
}
void regInit(void)
{
    int i,j;
    classCount = vertexCount = 0;
    InitRegAliases(chosenAssembler->arch->regNames);
    InitTree(NULL, chosenAssembler->arch->regRoot);
    vertexes = calloc(sizeof(ARCH_REGVERTEX *), vertexCount);
    classes = calloc(sizeof(ARCH_REGCLASS *), classCount);
    LoadVertexes(chosenAssembler->arch->regRoot);
    LoadWorstCase();
    LoadAliases(chosenAssembler->arch->regRoot);
    LoadSaturationBounds();
}

void alloc_init(void)
{
    spillcount = maxAllocationSpills = 0;
}
static void cacheTempSymbol(SYMBOL *sp)
{
    if (sp->anonymous && !sp->stackblock && sp->storage_class != sc_parameter)
    {
        if (sp->allocate && !sp->inAllocTable)
        {
            LIST *lst = Alloc(sizeof(LIST));
            lst->data = sp;
            lst->next = temporarySymbols;
            temporarySymbols = lst;
            sp->inAllocTable = TRUE;
        }
    }
}
static void ScanForVar(EXPRESSION *exp)
{
    if (exp)
    {
        if (exp->type == en_auto)
        {
            cacheTempSymbol(exp->v.sp);
        }
        else if (exp->type != en_tempref)
        {
            ScanForVar(exp->left);
            ScanForVar(exp->right);
        }
    }
}
static void ScanForAnonymousVars(void)
{
    static QUAD *head ;
    head = intermed_head;
    while (head)
    {
        if (head->dc.opcode != i_block && head->dc.opcode != i_blockend 
            && head->dc.opcode != i_dbgblock && head->dc.opcode != i_dbgblockend && head->dc.opcode != i_var
            && head->dc.opcode != i_label && head->dc.opcode != i_line && head->dc.opcode != i_passthrough)
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
void AllocateStackSpace(SYMBOL *funcsp)
/*
 * This allocates space for local variables
 * we do this AFTER the register optimization so that we can
 * avoid allocating space on the stack twice
 */
{
    int maxlvl = -1; 
        /* base assignments are at level 0, function body starts at level 1*/
    int oldauto, max;
    int i;
    HASHTABLE *syms = funcsp->inlineFunc.syms;
    LIST **temps = &temporarySymbols;
    BOOLEAN show = FALSE;
    ScanForAnonymousVars();
    while (syms)
    {
        if (syms->blockLevel >= maxlvl)
            maxlvl = syms->blockLevel + 1;
        syms = syms->next;
    }
    
    while (*temps)
    {
        SYMBOL *sym = (SYMBOL *)(*temps)->data;
        if (sym->storage_class == sc_auto && sym->value.i >= maxlvl)
            maxlvl = sym->value.i + 1;
        temps = &(*temps)->next;
    }
    lc_maxauto = max = oldauto = 0;
    
    for (i=0; i < maxlvl; i++)
    {
        syms = funcsp->inlineFunc.syms;
        while (syms)
        {
            if (syms->blockLevel == i)
            {
                HASHREC *hr = syms->table[0];
                lc_maxauto = oldauto;
                while (hr)
                {
                    
                    SYMBOL *sp = (SYMBOL *)hr->p;
                    if (!sp->regmode && (sp->storage_class == sc_auto 
                                         || sp->storage_class == sc_register)
                                         && sp->allocate && !sp->anonymous)
                    {
                        int val, align = getAlign(sc_auto, basetype(sp->tp));
                        lc_maxauto += basetype(sp->tp)->size;
                        val = lc_maxauto % align;
                        if (val != 0)
                            lc_maxauto += align - val;
                        sp->offset =  - lc_maxauto;					
                        if (lc_maxauto > max)
                            max = lc_maxauto;
                    }
                    hr = hr->next;
                }
            }
            oldauto = max;
            syms = syms->next;
        }
        temps = &temporarySymbols;
        while (*temps)
        {
            SYMBOL *sp = (SYMBOL *)(*temps)->data;
            if (sp->storage_class != sc_static && (sp->storage_class == sc_constant || sp->value.i == i))
            {
                int val, align = getAlign(sc_auto, basetype(sp->tp));
                lc_maxauto += basetype(sp->tp)->size;
                val = lc_maxauto % align;
                if (val != 0)
                    lc_maxauto += align - val;
                sp->offset =  - lc_maxauto;					
                if (lc_maxauto > max)
                    max = lc_maxauto;
                oldauto = max;
                *temps = (*temps)->next;
                sp->inAllocTable = FALSE; // needed because due to inlining a temp may be used across multiple function bodies
            }
            else
            {
                temps = &(*temps)->next;
            }
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
void FillInPrologue(QUAD *head, SYMBOL *funcsp)
{
    IMODE *ip, *ip1;
    /*
    if (cparams.prm_cplusplus && cparams.prm_xcept || (lc_maxauto || funcsp
        ->tp->lst.head && funcsp->tp->lst.head != (SYM*) - 1)
         || (funcsp->value.classdata.cppflags &PF_MEMBER) && !(funcsp
             ->value.classdata.cppflags &PF_STATIC)
             || !cparams.prm_smartframes)
        regmask |= FRAME_FLAG_NEEDS_FRAME;
    */
    if (regmask || lc_maxauto || (basetype(funcsp->tp)->syms->table[0] 
                                  && ((SYMBOL *)basetype(funcsp->tp)->syms->table[0]->p)->tp->type != bt_void))
        regmask |= FRAME_FLAG_NEEDS_FRAME;
    while (head->dc.opcode != i_prologue)
    {
        head = head->fwd;
    }
    head->dc.left = ip = make_immed(ISZ_UINT,regmask);
    head->dc.right = ip1 = make_immed(ISZ_UINT,lc_maxauto);
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
static EXPRESSION *spillVar( enum e_sc storage_class, TYPE *tp)
{
    extern int unnamed_id;
    EXPRESSION *rv = anonymousVar(storage_class, tp);
    SYMBOL *sp = rv->v.sp;
    deref(tp, &rv);
    sp->spillVar = TRUE;
    sp->anonymous = FALSE;
    return rv;
}
static void GetSpillVar(int i)
{
    SPILL *spill;
    EXPRESSION *exp;
    exp = spillVar(sc_auto,tempInfo[i]->enode->v.sp->tp);
    spill = tAlloc(sizeof(SPILL));
    tempInfo[i]->spillVar = spill->imode = make_ioffset(exp);
    spill->imode->size = tempInfo[i]->enode->v.sp->imvalue->size;
    spill->uses = tAlloc(sizeof(LIST));
    spill->uses->data = (void *)i;
}
static void CopyLocalColors(void)
{
    QUAD *head = intermed_head;
    regmask = 0;
    while (head)
    {
        if  (head->dc.opcode != i_block && !head->ignoreMe && head->dc.opcode != i_passthrough && head->dc.opcode !=
            i_label)
        {
            if ((head->temps & TEMP_ANS) || (head->ans && head->ans->retval))
            {
                if (head->ans->offset)
                {
                    head->ansColor = tempInfo[head->ans->offset->v.sp->value.i]->color;
                    if (head->ansColor >= 0)
                        regmask |= chosenAssembler->arch->regNames[head->ansColor].pushMask;
                }
                if (head->ans->offset2)
                {
                    head->scaleColor = tempInfo[head->ans->offset2->v.sp->value.i]->color;
                    if (head->scaleColor >= 0)
                        regmask |= chosenAssembler->arch->regNames[head->scaleColor].pushMask;
                }
            }
            if ((head->temps & TEMP_LEFT) || (head->dc.left && head->dc.left->retval))
            {
                if (head->dc.left->offset)
                {
                    head->leftColor = tempInfo[head->dc.left->offset->v.sp->value.i]->color;
                    if (head->leftColor >= 0)
                        regmask |= chosenAssembler->arch->regNames[head->leftColor].pushMask;
                }
                if (head->dc.left->offset2)
                {
                    head->scaleColor = tempInfo[head->dc.left->offset2->v.sp->value.i]->color;
                    if (head->scaleColor >= 0)
                        regmask |= chosenAssembler->arch->regNames[head->scaleColor].pushMask;
                }
            }
            if ((head->temps & TEMP_RIGHT) || (head->dc.right && head->dc.right->retval))
            {
                if (head->dc.right->offset)
                {
                    head->rightColor = tempInfo[head->dc.right->offset->v.sp->value.i]->color;
                    if (head->rightColor >= 0)
                        regmask |= chosenAssembler->arch->regNames[head->rightColor].pushMask;
                }
                if (head->dc.right->offset2)
                {
                    head->scaleColor = tempInfo[head->dc.right->offset2->v.sp->value.i]->color;
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
    ARCH_REGVERTEX *v = vertexes[t];
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
    for (i=0; i < tempCount; i++)
        if (tempInfo[i]->doGlobal && !tempInfo[i]->precolored)
        {
            tempInfo[i]->squeeze = 0;
            if (tempInfo[i]->rawSqueeze)
                memset(tempInfo[i]->rawSqueeze, 0, sizeof(tempInfo[0]->rawSqueeze[0]) * vertexCount);
            else
                tempInfo[i]->rawSqueeze = aAlloc(sizeof(tempInfo[0]->rawSqueeze[0]) * vertexCount);
            tempInfo[i]->degree = 0;
        }
    for (i=0; i < tempCount; i++)
    {
        if (tempInfo[i]->doGlobal && !tempInfo[i]->precolored)
        {
            UBYTE regs[MAX_INTERNAL_REGS];
            int j;
            int k,n;
            BITINT *confl = tempInfo[i]->conflicts;
            memset(regs, 0, sizeof(regs));
            for (k=0; k < (tempCount + BITINTBITS-1)/BITINTBITS; k++)
                if (confl[k])
                    for (n=k *BITINTBITS; n < k * BITINTBITS + BITINTBITS; n++)
                        if (isset(confl, n))
                        {
                            if (!tempInfo[n]->precolored)
                            {
                                tempInfo[i]->squeeze += SqueezeChange(i, tempInfo[n]->regClass->vertex, 
                                                                  +worstCase[ tempInfo[i]->regClass->index * classCount + tempInfo[n]->regClass->index]);
                                tempInfo[i]->degree++;
                            }
                            else
                            {
                                regs[tempInfo[n]->color] = TRUE;
                            }
                        }
            tempInfo[i]->regCount = tempInfo[i]->regClass->regCount;
            for (j=0; j < REG_MAX; j++)
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
    BRIGGS_SET *exposed = briggsAlloct(tempCount);
    int i,j;
    for (j=0; j < tempCount; j++)
        tempInfo[j]->liveAcrossFunctionCall = FALSE;
    for (i=0; i < blockCount; i++)
    {
        struct _block *blk = blockArray[i];
        if (blk && blk->head)
        {
            QUAD *tail = blk->tail;
            BITINT *p = blk->liveOut;
            int j,k;
            briggsClear(exposed);
            for (j=0; j < (tempCount + BITINTBITS-1)/BITINTBITS; j++,p++)
                if (*p)
                    for (k=0; k < BITINTBITS; k++)
                        if (*p & (1 << k))
                        {
                            briggsSet(exposed, j*BITINTBITS + k);
                        }
            while (tail != blk->head)
            {
                if (tail->dc.opcode == i_gosub)
                {
                    for (j = 0; j < exposed->top; j++)
                        tempInfo[exposed->data[j]]->liveAcrossFunctionCall=TRUE;
                }
                if (tail->temps & TEMP_ANS)
                {
                    if (tail->ans->mode == i_direct)
                    {
                        int tnum = tail->ans->offset->v.sp->value.i;
                        briggsReset(exposed, tnum);
                    }
                    else if (tail->ans->mode == i_ind)
                    {
                        if (tail->ans->offset)
                            briggsSet(exposed, tail->ans->offset->v.sp->value.i);
                        if (tail->ans->offset2)
                            briggsSet(exposed, tail->ans->offset2->v.sp->value.i);
                    }
                }
                if (tail->temps & TEMP_LEFT)
                    if (tail->dc.left->mode == i_ind || tail->dc.left->mode == i_direct)
                    {
                        if (!tail->dc.left->retval)
                        {
                            if (tail->dc.left->offset)
                                briggsSet(exposed, tail->dc.left->offset->v.sp->value.i);
                            if (tail->dc.left->offset2)
                                briggsSet(exposed, tail->dc.left->offset2->v.sp->value.i);
                        }
                    }
                if (tail->temps & TEMP_RIGHT)
                    if (tail->dc.right->mode == i_ind || tail->dc.right->mode == i_direct)
                    {
                        if (tail->dc.right->offset)
                            briggsSet(exposed, tail->dc.right->offset->v.sp->value.i);
                        if (tail->dc.right->offset2)
                            briggsSet(exposed, tail->dc.right->offset2->v.sp->value.i);
                    }
                tail = tail->back;
            }
        }
    }
    {
        static int count =0;
    }
}
static void InitClasses(void)
{
    int i;
    for (i=0; i < tempCount; i++)
    {
        if (tempInfo[i]->inUse)
        {
            if  (!tempInfo[i]->liveAcrossFunctionCall)
            {
                tempInfo[i]->regClass = chosenAssembler->arch->regClasses[abs(tempInfo[i]->size)*2];
            }
            else
            {
                tempInfo[i]->regClass = chosenAssembler->arch->regClasses[abs(tempInfo[i]->size)*2 + 1];
            }
        }
        if (!tempInfo[i]->regClass)
            tempInfo[i]->doGlobal = FALSE;
    }
}
static void CountInstructions(BOOLEAN first)
{
    QUAD *head = intermed_head;
    int i;
    for (i=0; i < tempCount; i++)
    {
        TEMP_INFO *t = tempInfo[i];
        t->doGlobal = FALSE;
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
                        tempInfo[head->ans->offset->v.sp->value.i]->doGlobal = TRUE;
                    }
                    if (head->ans->offset2)
                    {
                        tempInfo[head->ans->offset2->v.sp->value.i]->doGlobal = TRUE;
                    }
                }
                if (head->temps & TEMP_LEFT)
                {
                    if (head->dc.left->offset)
                    {
                        tempInfo[head->dc.left->offset->v.sp->value.i]->doGlobal = TRUE;
                    }
                    if (head->dc.left->offset2)
                    {
                        tempInfo[head->dc.left->offset2->v.sp->value.i]->doGlobal = TRUE;
                    }
                }
                if (head->temps & TEMP_RIGHT)
                {
                    if (head->dc.right->offset)
                    {
                        tempInfo[head->dc.right->offset->v.sp->value.i]->doGlobal = TRUE;
                    }
                    if (head->dc.right->offset2)
                    {
                        tempInfo[head->dc.right->offset2->v.sp->value.i]->doGlobal = TRUE;
                    }
                }
                break;
        }
        head = head->fwd;
    }
    for (i=0; i < tempCount; i++)
        tempInfo[i]->inUse = tempInfo[i]->doGlobal;
    instructionByteCount = (instructionCount + BITINTBITS-1)/BITINTBITS;

    instructionCount += 1000;
    workingMoves = tallocbit(instructionCount);
    activeMoves = tallocbit(instructionCount);
    coalescedMoves = aallocbit(instructionCount);
    constrainedMoves = tallocbit(instructionCount);
    frozenMoves = tallocbit(instructionCount);
    tempMoves[0] = tallocbit(instructionCount);
    tempMoves[1] = tallocbit(instructionCount);
    hiMoves = tAlloc(sizeof(short) * (instructionCount));
    instructionList = tAlloc(sizeof(QUAD *) * (instructionCount));
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
static BOOLEAN BriggsCoalesceInit(int u, int v, int n)
{
    int k = 0;
    int i, t;
    int K;
    Adjacent1(u);
    Adjacent(v);
    for (i=0; i < (tempCount + BITINTBITS-1)/BITINTBITS; i++)
    {
        // this is endian dependent sad to say... the new compiler ought to do things in 32 bit words ALWAYS
        BITINT x = adjacent[i] |= adjacent1[i];
        if (x)
        {
            int n = i * BITINTBITS;
            for (t= n; t< n +BITINTBITS; t++, x >>= 1)
                if (x & 1)
                    if (tempInfo[t]->squeeze >= tempInfo[t]->regCount)
                        k++;
        }
    }
    K = imin(tempInfo[u]->regClass->regCount, tempInfo[v]->regClass->regCount);		
    if (k >= K)
    {
        hiMoves[n] = k - (K - 1);
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}
static BOOLEAN GeorgeCoalesceInit(int u, int v, int n)
{
    /*u is precolored, v is not */
    int k = 0, i, t;
    Adjacent(v);
    for (i=0; i < (tempCount + BITINTBITS-1)/BITINTBITS; i++)
        if (adjacent[i])
            for (t= i * BITINTBITS; t < i * BITINTBITS + BITINTBITS; t++)
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
        return FALSE;
    }
    return TRUE;
}
static int tiny(QUAD *head, IMODE *compare)
{
    QUAD *tail = head->back;
    while (tail->spill)
    {
        tail = tail->back;
    }
    if ((tail->temps & TEMP_ANS) && tail->ans->offset)
        return tail->ans->offset->v.sp->value.i == compare->offset->v.sp->value.i;
    else
        return FALSE;
}
/* we are doing the conflict graph (interference graph)
 * according to Morgan instead of the way it is done in George & Appel
 * accordingly, this function just gets a list of moves
 */
static void Build(BLOCK *b)
{
    QUAD *head;
    BLOCKLIST *bl;
    int i;
    if (b == NULL)
    {
        for (i=0; i < tempCount; i++)
        {
            TEMP_INFO *t = tempInfo[i];
            t->workingMoves = NULL;
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
                if (head->ans->mode == i_direct && head->dc.left->mode == i_direct && !head->ans->bits && !head->dc.left->bits && !head->dc.left->retval)
                {
                    int u = head->ans->offset->v.sp->value.i;
                    int v = head->dc.left->offset->v.sp->value.i;
                    if (tempInfo[u]->regClass && (tempInfo[u]->regClass == tempInfo[v]->regClass ||
                        sizeFromISZ(tempInfo[u]->size) == sizeFromISZ(tempInfo[v]->size)))
                    {
                        TEMP_INFO *t;
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
                            BriggsCoalesceInit(u,v, head->index);
                    }
                }
            }
        }
        if (head->temps & TEMP_ANS)
        {
            if (head->ans->offset)
            {
                tempInfo[head->ans->offset->v.sp->value.i]->spillCost += head->block->spillCost;
            }
            if (head->ans->offset2)
            {
                tempInfo[head->ans->offset2->v.sp->value.i]->spillCost += head->block->spillCost;
            }
        }
        if (head->temps & TEMP_LEFT)
        {
            if (head->dc.left->offset)
            {
                tempInfo[head->dc.left->offset->v.sp->value.i]->spillCost += head->block->spillCost;
                if (!tiny(head, head->dc.left))
                    tempInfo[head->dc.left->offset->v.sp->value.i]->temp = 1;
            }				
            if (head->dc.left->offset2)
            {
                tempInfo[head->dc.left->offset2->v.sp->value.i]->spillCost += head->block->spillCost;
                tempInfo[head->dc.left->offset2->v.sp->value.i]->temp = 1;
            }
        }
        if (head->temps & TEMP_RIGHT)
        {
            if (head->dc.right->offset)
            {
                tempInfo[head->dc.right->offset->v.sp->value.i]->spillCost += head->block->spillCost;
                if (!tiny(head, head->dc.right))
                    tempInfo[head->dc.right->offset->v.sp->value.i]->temp = 1;
            }				
            if (head->dc.right->offset2)
            {
                tempInfo[head->dc.right->offset2->v.sp->value.i]->spillCost += head->block->spillCost;
                tempInfo[head->dc.right->offset2->v.sp->value.i]->temp = 1;
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
    BITINT *confl = tempInfo[n]->conflicts;
    int i;
    int x = (tempCount + BITINTBITS-1)/BITINTBITS ;
    memset(adjacent, 0, x*sizeof(BITINT));
    for (i=0; i < x; i++)
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
    BITINT *confl = tempInfo[n]->conflicts;
    int i;
    int x = (tempCount + BITINTBITS-1)/BITINTBITS ;
    memset(adjacent1, 0, x*sizeof(BITINT));
    for (i=0; i < x; i++)
    {
        BITINT v;
        if (confl[i])
        {
            adjacent1[i] = confl[i] & ~(stackedTemps[i] | coalescedNodes[i]);
        }
    }
}
static BITARRAY *NodeMoves(int n, int index)
{
    if (tempInfo[n]->workingMoves)
    {
        int i;
        BITARRAY *rv = tempMoves[index];
        BITINT *p = bits(rv);
        BITINT *w = bits(tempInfo[n]->workingMoves);
        memset(p, 0, instructionByteCount * sizeof(BITINT));
        for (i=0; i < instructionByteCount; i++)
        {
            if (w[i])
                p[i] = w[i] &   (activeMoves[i] | workingMoves[i]);
        }
        return rv;
    }
    return NULL;
}
static BOOLEAN MoveRelated(int n, int index)
{
    BITARRAY *data = NodeMoves(n, index);
    if (data)
    {
        int i;
        for (i=0; i < instructionByteCount; i++)
            if (bits(data)[i])
                return TRUE;
    }
    return FALSE;
}
static void MkWorklist(void)
{
    int i;
    /* going backwatds to get spill temps first */
    for (i=0; i < tempCount; i++)
    {
        TEMP_INFO *t = tempInfo[i];
        if (t->doGlobal && !t->precolored && t->ircinitial)
        {
            t->doGlobal = FALSE;
            t->ircinitial = FALSE;
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
static void EnableMoves(BITINT *nodes, int index)
{
    int i, t;
    for (i=0; i < (tempCount + BITINTBITS-1)/BITINTBITS; i++)
        if (nodes[i])
            for (t= i * BITINTBITS; t < i * BITINTBITS + BITINTBITS; t++)
                if (isset(nodes, t))
                {
                    BITARRAY *nm = NodeMoves(t, index);
                    int j;
                    if (nm)
                    {
                        for (j=0; j < instructionByteCount; j++)
                        {
                            if (bits(activeMoves)[j] & bits(nm)[j])
                            {
                                int k;
                                for (k=j*BITINTBITS; k < j*BITINTBITS + BITINTBITS; k++)
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
        tempInfo[m]->squeeze += SqueezeChange(m, tempInfo[n]->regClass->vertex, 
                -worstCase[tempInfo[m]->regClass->index * classCount + tempInfo[n]->regClass->index]);
        tempInfo[m]->degree--;
        if (cur == tempInfo[m]->regCount 
            && tempInfo[m]->squeeze < tempInfo[m]->regCount)
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
        briggsReset(freezeWorklist, n); // DAL I added this because it seemed needed
        if (tempInfo[n]->squeeze >= tempInfo[n]->regCount)
            EnableMoves(adjacent1, 0);
        for (i=0; i < (tempCount + BITINTBITS-1)/BITINTBITS; i++)
            if (adjacent1[i])
                for (t= i * BITINTBITS; t < i * BITINTBITS + BITINTBITS; t++)
                    if (isset(adjacent1, t))
                        DecrementDegree(t, n);
    }
}
static void AddWorkList(int u)
{
    if (!tempInfo[u]->precolored && !MoveRelated(u,1) && 
        tempInfo[u]->squeeze < tempInfo[u]->regCount)
    {
        briggsReset(freezeWorklist, u);
        simplifyWorklist[simplifyTop++] = u;
    }
}
static int Combine(int u, int v)
{
    int i, t;
    unsigned z;
    int max = (tempCount + BITINTBITS-1)/BITINTBITS;
    BOOLEAN losingHiDegreeNode;
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
        tempInfo[u]->liveAcrossFunctionCall = TRUE;
    }
    else if (tempInfo[u]->liveAcrossFunctionCall)
    {
        tempInfo[v]->regClass = tempInfo[u]->regClass;
        tempInfo[v]->liveAcrossFunctionCall = TRUE;
    }
       if (tempInfo[u]->precolored)
    {
        BITARRAY *nm = tempInfo[v]->workingMoves;
        int j;
        for (j=0; j < instructionByteCount; j++)
        {
            int z;
            if (z = bits(nm)[j])
            {
                int k ;
                int m = j * BITINTBITS;
                for (k= m; k < m + BITINTBITS; k++, z>>=1)
                {
                    if ((z & 1) && instructionList[k])
                    {
                        int x = instructionList[k]->ans->offset->v.sp->value.i;
                        int y = instructionList[k]->dc.left->offset->v.sp->value.i;
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
                tempInfo[u] = tu = tallocbit(instructionCount);
            }
            for (i=0; i < instructionByteCount; i++)
                (bits(tu))[i] |= (bits(tv))[i];
        }
    }
    losingHiDegreeNode = tempInfo[u]->squeeze >= tempInfo[u]->regCount 
                         && tempInfo[v]->squeeze >= tempInfo[v]->regCount;
    for (i=0; i < max; i++)
        if (z = tempInfo[v]->conflicts[i] & ~coalescedNodes[i])
            for (t= i * BITINTBITS; t < i * BITINTBITS + BITINTBITS; t++, z>>=1)
                if (z & 1)
                {
                    if (t != u && !isConflicting(t, u))
                    {
                        insertConflict(t, u);
                        if (!tempInfo[u]->precolored)
                        {
                            tempInfo[u]->squeeze += SqueezeChange(u, tempInfo[t]->regClass->vertex, 
                                +worstCase[tempInfo[u]->regClass->index * classCount + tempInfo[t]->regClass->index]);
                            tempInfo[u]->degree++;
                        }
                        if (!tempInfo[t]->precolored)
                        {
                            tempInfo[t]->squeeze += SqueezeChange(t, tempInfo[u]->regClass->vertex, 
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
static BOOLEAN AllOK(int u, int v)
{
    int i,t;
    if (tempInfo[u]->enode->v.sp->imvalue->retval)
        return FALSE;
    Adjacent(v);
    for (i=0; i < (tempCount + BITINTBITS-1)/BITINTBITS; i++)
        if (adjacent[i])
            for (t= i * BITINTBITS; t < i * BITINTBITS + BITINTBITS; t++)
                if (isset(adjacent, t))
                    if (!tempInfo[t]->precolored && tempInfo[t]->squeeze >= tempInfo[t]->regCount &&
                        !isConflicting(t, u))
                            return FALSE;
    return TRUE;
}
static BOOLEAN Conservative(int u, int v)
{
    int i, t, k= 0;
    Adjacent1(u);
    Adjacent(v);
    for (i=0; i < (tempCount + BITINTBITS-1)/BITINTBITS; i++)
    {
        adjacent[i] |= adjacent1[i];
        if (adjacent[i])
            for (t= i * BITINTBITS; t < i * BITINTBITS + BITINTBITS; t++)
                if (isset(adjacent, t))
                    if (tempInfo[t]->squeeze >= tempInfo[t]->regCount)
                        ++k;
    }
    return k < tempInfo[u]->regClass->regCount && k < tempInfo[v]->regClass->regCount;
}
static BOOLEAN conflictsWithSameColor(int u, int v)
{
    if (tempInfo[u]->precolored)
    {
        int i,j;
        BITINT *confl = tempInfo[v]->conflicts;
        for (i=0; i < (tempCount + BITINTBITS-1)/BITINTBITS; i++)
            if (confl[i])
                for (j=i *BITINTBITS; j < i * BITINTBITS + BITINTBITS; j++)
                    if (isset(confl, j))
                    {
                        int k = findPartition(j);
                        if (tempInfo[k]->precolored && tempInfo[k]->color == tempInfo[u]->color)
                        {
                            return TRUE;
                        }
                    }
    }	
    return FALSE;
}
static BOOLEAN inBothRegClasses(int u, int v)
{
    if (tempInfo[u]->precolored)
    {
        int n = tempInfo[u]->color;
        int i;
        for (i=0; i < tempInfo[v]->regCount; i++)
            if (n == tempInfo[v]->regClass->regs[i])
                return TRUE;
        return FALSE;
    }
    return TRUE;
}
static BOOLEAN Coalesce(void)
{
    int i;
    int sel = -1;

    for (i=0; i < instructionByteCount; i++)
    {
        BITINT x;
        if ((x = (bits(workingMoves))[i]))
        {
            int j;
            for (j= 0; x; j++, x>>=1)
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
        int u,v;
        u = findPartition(instructionList[sel]->ans->offset->v.sp->value.i);
        v = findPartition(instructionList[sel]->dc.left->offset->v.sp->value.i);
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
        else if (tempInfo[v]->precolored || isConflicting(u, v)
                 || conflictsWithSameColor(u,v) || !inBothRegClasses(u,v))
        {
            /* both precolored or otherwise in conflict, can't do anything with it */
            setbit(constrainedMoves, sel);
            AddWorkList(u);
            AddWorkList(v);
        }
        else if ((tempInfo[u]->precolored && AllOK(u, v)) || 
                 (!tempInfo[u]->precolored && Conservative(u, v)))
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
                BriggsCoalesceInit(u,v, sel);
            }
        return TRUE;
    }
    /* no moves buffered for attempted coalesce at this time */
    return FALSE;
}
static void FreezeMoves( int u)
{
    BITARRAY *nm;
    int i;
    nm = NodeMoves(u, 0);
    if (nm)
    {
        for (i=0; i < instructionByteCount; i++)
        {
            if (bits(nm)[i])
            {
                int j;
                for (j=i*BITINTBITS; j < i * BITINTBITS + BITINTBITS; j++)
                {
                    if (isset(nm, j))
                    {
                        int v ;
                        int n = instructionList[j]->ans->offset->v.sp->value.i;
                        if (findPartition(n) == u)
                        {
                            v = instructionList[j]->dc.left->offset->v.sp->value.i;
                        }
                        else
                        {
                            v = instructionList[j]->ans->offset->v.sp->value.i;
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
                            if (tempInfo[v]->squeeze < tempInfo[v]->regCount
                                 && !MoveRelated(v, 1))
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
    LIST *def = tempInfo[i]->idefines;
    LIST *use = tempInfo[i]->iuses;
    if (!def || !use)
        return UINT_MAX;
    if (!def->next && !use->next)
    {
        QUAD *q = (QUAD *)use->data;
        if (((QUAD *)def->data)->index == q->index-1)
            if (!(q->temps & TEMP_ANS) && q->dc.opcode == i_assn 
                && q->ans->offset->type != en_add
                && q->ans->mode == i_direct && (q->ans->offset->v.sp->spillVar))
                return UINT_MAX;
    }
//	if (!use)
//	{
//		return tempInfo[i]->regCount - tempInfo[i]->squeeze;
//	}
    cost = tempInfo[i]->spillCost ;// tempInfo[i]->degree;
    if (cost <= UINT_MAX - 1 - REG_MAX)
        return cost + REG_MAX;
    return UINT_MAX - 1;
}
static void SelectSpill(void)
{
    int m = -1;
    unsigned priority = UINT_MAX;
    int i;
    BOOLEAN printing = FALSE;
    for (i=0; i < spillWorklist->top; i++)
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
        for (i=0; i < spillWorklist->top; i++)
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
    simplifyWorklist[simplifyTop++] = m;
    FreezeMoves(m);
}
static void AssignColors(void)
{
    int n;
    int i,j;
    int m;
    UBYTE *order = chosenAssembler->arch->regOrder;
    int count = *order++;
    {
        int pos;
        for (pos = tempStackcount-1; pos >= 0; pos--)
        {
            n = tempStack[pos];
            if (n != -1)
            {
                BOOLEAN regs[MAX_INTERNAL_REGS];
                BITINT *confl = tempInfo[n]->conflicts;
                ARCH_REGCLASS *cls = tempInfo[n]->regClass;
                tempStack[pos] = -1;
                for (i=0; i < sizeof(regs); i++)
                    regs[i] = TRUE;
                for (i=0; i < (tempCount + BITINTBITS-1)/BITINTBITS; i++)
                    if (confl[i])
                        for (j=i *BITINTBITS; j < i * BITINTBITS + BITINTBITS; j++)
                            if (isset(confl, j))
                            {
                                int u = findPartition(j);
                                if (tempInfo[u]->color >= 0)
                                {
                                    int x = tempInfo[u]->color;
                                    int k;
                                    regs[x] = FALSE;
                                    for (k=0; k < chosenAssembler->arch->regNames[x].aliasCount; k++)
                                    {
                                        regs[chosenAssembler->arch->regNames[x].aliases[k]] = FALSE;
                                    }
                                }
                            }
                for (i=0; i < cls->regCount; i++)
                    if (regs[cls->regs[i]])
                    {
                        tempInfo[n]->color = cls->regs[i];
                        tempInfo[n]->enode->v.sp->regmode = 2;
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
    for (i=0; i < tempCount; i++)
        tempInfo[i]->color = tempInfo[findPartition(i)]->color;
}
static IMODE * InsertLoad(QUAD *head, IMODE *mem)
{
    QUAD *insert;
    IMODE *t = InitTempOpt(mem->size, mem->size);
    tempInfo[t->offset->v.sp->value.i]->spilled = TRUE;
    tempInfo[t->offset->v.sp->value.i]->ircinitial = TRUE;
    head = head->back;
    insert = Alloc(sizeof(QUAD));
    insert->dc.opcode = i_assn;
    insert->ans = t;
    insert->dc.left = mem;
    insert->block = head->block;
    insert->index = -1;
    insert->spill = TRUE;
    InsertInstruction(head, insert);
    accesses++;
    return t;	
}
static void InsertStore(QUAD *head, IMODE **im, IMODE *mem)
{
    IMODE *t = InitTempOpt(mem->size, mem->size);
    QUAD *insert;
    int tn = t->offset->v.sp->value.i;
    int ta = (*im)->offset->v.sp->value.i;
    tempInfo[tn]->spilled = TRUE;
    tempInfo[tn]->precolored = tempInfo[ta]->precolored;
    tempInfo[tn]->enode->v.sp->regmode = tempInfo[ta]->enode->v.sp->regmode;
    tempInfo[tn]->color = tempInfo[ta]->color;
    tempInfo[tn]->ircinitial = TRUE;
    *im = t;
    insert = Alloc(sizeof(QUAD));
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
    QUAD *head = intermed_head;
    int spillVars[6],i,j;
    IMODE *spillNodes[6];
//    IMODE *mem = tempInfo[n]->spillVar;
    while (head)
    {
        IMODE *t;
        memset(spillVars, 0xff, sizeof(spillVars));
        memset(spillNodes, 0, sizeof(spillNodes));
        if (head->temps & TEMP_ANS)
        {
            if (head->ans->mode == i_direct)
            {
                if (tempInfo[head->ans->offset->v.sp->value.i]->spilling)
                {
                    InsertStore(head, &head->ans, tempInfo[head->ans->offset->v.sp->value.i]->spillVar);
                }
            }
            else
            {
                if (head->ans->offset && head->ans->offset->type == en_tempref && tempInfo[head->ans->offset->v.sp->value.i]->spilling)
                    spillVars[0] = head->ans->offset->v.sp->value.i;
                if (head->ans->offset2 && head->ans->offset2->type == en_tempref && tempInfo[head->ans->offset2->v.sp->value.i]->spilling)
                    spillVars[1] = head->ans->offset2->v.sp->value.i;
            }
        }
        if (head->temps & TEMP_LEFT)
        {
            if (head->dc.left->offset && head->dc.left->offset->type == en_tempref && tempInfo[head->dc.left->offset->v.sp->value.i]->spilling)
                spillVars[2] = head->dc.left->offset->v.sp->value.i;
            if (head->dc.left->offset2 && head->dc.left->offset2->type == en_tempref && tempInfo[head->dc.left->offset2->v.sp->value.i]->spilling)
                spillVars[3] = head->dc.left->offset2->v.sp->value.i;
        }
        if (head->temps & TEMP_RIGHT)
        {
            if (head->dc.right->offset && head->dc.right->offset->type == en_tempref && tempInfo[head->dc.right->offset->v.sp->value.i]->spilling)
                spillVars[4] = head->dc.right->offset->v.sp->value.i;
            if (head->dc.right->offset2 && head->dc.right->offset2->type == en_tempref && tempInfo[head->dc.right->offset2->v.sp->value.i]->spilling)
                spillVars[5] = head->dc.right->offset2->v.sp->value.i;
        }
        // this is all to guard against loading the same var two or more times for the same instruction
        // which is both optimally bad and can also lead to insidious bugs
        // because of some assumptions made by the rewrite module
        for (i=0; i < 6; i++)
            if (spillVars[i] != -1)
            {
                for (j=0; j < i; j++)
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
            IMODE *im = Alloc(sizeof(IMODE));
            *im = *head->ans;
            head->ans = im;
        }
        if (spillNodes[0])
            head->ans->offset = spillNodes[0]->offset;
        if (spillNodes[1])
            head->ans->offset2 = spillNodes[1]->offset;
        if (spillNodes[2] || spillNodes[3])
        {
            IMODE *im = Alloc(sizeof(IMODE));
            *im = *head->dc.left;
            head->dc.left = im;
        }
        if (spillNodes[2])
            head->dc.left->offset = spillNodes[2]->offset;
        if (spillNodes[3])
            head->dc.left->offset2 = spillNodes[3]->offset;
        if (spillNodes[4] || spillNodes[5])
        {
            IMODE *im = Alloc(sizeof(IMODE));
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
    for (i=0; i < spilledNodes->top; i++)
    {
        int n = spilledNodes->data[i];
        if (!tempInfo[n]->spillVar)
            GetSpillVar(n);
    }
}
/* attempt to use the same spill var for any spilled variables
 * which are connected by an uncoalesced move
 */
static void SpillCoalesce(BRIGGS_SET *C, BRIGGS_SET *S)
{
    typedef struct _move_
    {
        struct _move_ *next;
        int a;
        int b;
        int cost;
        LIST *uses;
    } MOVE;
    MOVE *moves = NULL, **mt;
    int i;
    for (i=0; i < S->top; i++)
    {
        int n = S->data[i];
        BITARRAY *nm = tempInfo[n]->workingMoves;
        int j;
        if (nm)
        {
            for (j = 0; j < instructionByteCount; j++)
            {
                if (bits(nm)[j] & ~bits(coalescedMoves)[j])
                {
                    int k;
                    for (k = j *BITINTBITS; k < j * BITINTBITS + BITINTBITS; k++)
                    {
                        if (isset(nm, k) && !isset(coalescedMoves, k))
                        {
                            QUAD *head = instructionList[k];
                            if (head)
                            {
                                BOOLEAN test ;
                                int a = head->ans->offset->v.sp->value.i;
                                int b = head->dc.left->offset->v.sp->value.i;
                                if (sizeFromISZ(tempInfo[a]->size) == sizeFromISZ(tempInfo[b]->size)
                                    && !tempInfo[a]->directSpill && !tempInfo[b]->directSpill)
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
                                                    a = b ;
                                                    b = temp;
                                                }
                                                mt = &moves;
                                                while (*mt)
                                                {
                                                    if ((*mt)->a == a && (*mt)->b == b)
                                                    {
                                                        LIST **l = &(*mt)->uses;
                                                        while (*l)
                                                        {
                                                            if ((*l)->data == (void *)head)
                                                                break;
                                                            l = &(*l)->next;
                                                        }
                                                        if (!*l)
                                                        {
                                                            *l = tAlloc(sizeof(LIST));
                                                            (*l)->data = (void *)head;
                                                            (*mt)->cost += head->block->spillCost;
                                                        }
                                                        break;
                                                    }
                                                    mt = &(*mt)->next;
                                                }
                                                if (!*mt)
                                                {
                                                    *mt = tAlloc(sizeof(MOVE));
                                                    (*mt)->a = a;
                                                    (*mt)->b = b;
                                                    (*mt)->cost = head->block->spillCost;
                                                    (*mt)->uses = tAlloc(sizeof(LIST));
                                                    (*mt)->uses->data = (void *)head;													
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
        MOVE **mv = NULL , *ml;
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
        if (!isConflicting(ml->a, ml->b) )
        {
            LIST *uses = ml->uses;
            if (tempInfo[ml->a]->spillVar)
            {
                if (tempInfo[ml->b]->spillVar) // just in case...
                {
                        // deallocate one of the spills
                        QUAD *head;
                        IMODE *spill = tempInfo[ml->b]->spillVar;
                        IMODE *nw = tempInfo[ml->a]->spillVar;
                        int i;
                        if (nw != spill)
                        {
                            spill->offset->v.sp->allocate = FALSE;
                            // make sure all uses of the deallocated spill get renamed
                            for (i=0; i < tempCount; i++)
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
static void InsertCandidates(int W, BRIGGS_SET *L)
{
    BITARRAY *moves = tempInfo[W]->workingMoves;
    int i;
    if (moves)
    {
        for (i=0; i < instructionByteCount; i++)
        {
            if (bits(moves)[i] & ~bits(coalescedMoves)[i])
            {
                int j;
                for (j=i*BITINTBITS; j < i *BITINTBITS + BITINTBITS; j++)
                    if (isset(moves, j) && !isset(coalescedMoves, j))
                    {
                        QUAD *head = instructionList[j];
                        if (head)
                        {
                            int n = head->ans->offset->v.sp->value.i;
                            if (n == W)
                                n = head->dc.left->offset->v.sp->value.i;
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
    ARCH_SIZING *p = chosenAssembler->arch->regCosts;
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
/*        case ISZ_:*/
/*            return p->a_wchar_t;*/
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
static unsigned SpillPropagateSavings(int w, BRIGGS_SET *S)
{
    unsigned savings = 0;
    BITARRAY *moves = tempInfo[w]->workingMoves;
    int i;
    for (i=0; i < instructionByteCount; i++)
    {
        if (bits(moves)[i] & ~bits(coalescedMoves)[i])
        {
            int j;
            for (j=i*BITINTBITS; j < i *BITINTBITS + BITINTBITS; j++)
                if (isset(moves, j) && !isset(coalescedMoves, j))
                {
                    QUAD *head = instructionList[j];
                    if (head)
                    {
                        int n = head->ans->offset->v.sp->value.i;
                        if (n == w)
                            n = head->dc.left->offset->v.sp->value.i;
                        if (briggsTest(S, n) && !isConflicting(n, w) )
                        {
                            int sv ;
                            int cost = lscost(abs(tempInfo[n]->enode->v.sp->imvalue->size));
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
static void SpillPropagate(BRIGGS_SET *P, BRIGGS_SET *S, BRIGGS_SET *L, BRIGGS_SET *NP)
{
    int i;
    briggsClear(L);
    briggsClear(NP);
    for (i=0; i < P->top; i++)
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
    for (i=0; i < NP->top; i++)
    {
        briggsSet(P, NP->data[i]);
    }
}
static void SpillPropagateAndCoalesce(void)
{
    BRIGGS_SET *P = briggsAlloct(tempCount);
    BRIGGS_SET *np = briggsAlloct(tempCount);
    BRIGGS_SET *L = briggsAlloct(tempCount);
    int i;
    spillList = NULL;
    spillProcessed = briggsAlloct(tempCount);
    for (i=0; i < spilledNodes->top; i++)briggsSet(P, spilledNodes->data[i]);
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
    for (i=0; i < tempCount; i++)
    {
        tempInfo[i]->spilling = FALSE;
        tempInfo[i]->iuTemp = tempInfo[i]->inUse;
    }
    for (i=0; i < spilledNodes->top; i++)
    {
        int n = spilledNodes->data[i];
        tempInfo[n]->spilling = TRUE;
        tempInfo[n]->spilled = TRUE;
    }
    RewriteAllSpillNodes();	
    for (i=0; i < tempCount; i++)
    {
        if (tempInfo[i]->color != -1 || isset(coalescedNodes, i))
            tempInfo[i]->ircinitial = TRUE;
            
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
static IMODE *copyImode(IMODE *in)
{
    IMODE *im = Alloc(sizeof(IMODE));
    *im = *in;
    if (im->offset)
    {
        int tnum = im->offset->v.sp->value.i;
        if (tnum < spilledNodes->size && isset(coalescedNodes, tnum))
        {
            im->offset = tempInfo[findPartition(tnum)]->enode;
        }
    }
    if (im->offset2)
    {
        int tnum = im->offset2->v.sp->value.i;
        if (tnum < spilledNodes->size && isset(coalescedNodes, tnum))
        {
            im->offset2 = tempInfo[findPartition(tnum)]->enode;
        }
    }
    return im;
}
static void KeepCoalescedNodes(void)
{
    QUAD *head = intermed_head;
    int i;
    while (head)
    {
        QUAD *next = head->fwd;
        if (head->index != -1 && isset(coalescedMoves, head->index))
        {
            instructionList[head->index] = NULL;
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
    for (i=0; i < tempCount; i++)
        tempInfo[i]->partition = i;
}
static int LoopNesting(LOOP *loop)
{
    unsigned rv = 1;
    while (loop)
    {
        if (rv >= UINT_MAX/10)
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
    for (i=0; i < blockCount; i++)
        if (blockArray[i])
            blockArray[i]->spillCost = LoopNesting(blockArray[i]->loopParent);
}
void Precolor(void)
{
    QUAD *head = intermed_head;
    int i;
    if (!chosenAssembler->gen->preColor)
        return;
    for (i=0; i < tempCount; i++)
    {
        tempInfo[i]->color = -1;
        tempInfo[i]->precolored = FALSE;
        if (tempInfo[i]->enode)
            tempInfo[i]->enode->v.sp->regmode = 0;
    }
    while (head)
    {
        if (head->dc.opcode != i_block && !head->ignoreMe &&
            head->dc.opcode != i_label && head->dc.opcode != i_expressiontag)
        {
            chosenAssembler->gen->preColor(head);
        }
        head = head->fwd;
    }
    for (i=0; i < tempCount; i++)
    {
        tempInfo[i]->ircinitial = !tempInfo[i]->precolored; 
    }
}
void retemp(void)
{
    unsigned short map[65536];
    int i, cur = 0;
    QUAD *head;
    memset(map, 0, sizeof(map));
    for (i=0; i < tempCount; i++)
    {
        tempInfo[i]->inUse = FALSE;
    }
    head = intermed_head;
    while (head)
    {
        if  (head->dc.opcode != i_block && !head->ignoreMe && head->dc.opcode != i_passthrough && head->dc.opcode !=
            i_label)
        {
            if (head->ans)
            {
                if (head->ans->offset && head->ans->offset->type == en_tempref)
                    tempInfo[head->ans->offset->v.sp->value.i]->inUse = TRUE;
                if (head->ans->offset2 && head->ans->offset2->type == en_tempref)
                    tempInfo[head->ans->offset2->v.sp->value.i]->inUse = TRUE;
            }
            if (head->dc.left)
            {
                if (head->dc.left->offset && head->dc.left->offset->type == en_tempref)
                    tempInfo[head->dc.left->offset->v.sp->value.i]->inUse = TRUE;
                if (head->dc.left->offset2 && head->dc.left->offset2->type == en_tempref)
                    tempInfo[head->dc.left->offset2->v.sp->value.i]->inUse = TRUE;
            }
            if (head->dc.right)
            {
                if (head->dc.right->offset && head->dc.right->offset->type == en_tempref)
                    tempInfo[head->dc.right->offset->v.sp->value.i]->inUse = TRUE;
                if (head->dc.right->offset2 && head->dc.right->offset2->type == en_tempref)
                    tempInfo[head->dc.right->offset2->v.sp->value.i]->inUse = TRUE;
            }
        }
        head = head->fwd;
    }
    for (i=0; i < tempCount; i++)
        if (tempInfo[i]->inUse)
        {
            map[i] = cur;
            tempInfo[cur] = tempInfo[i];
            tempInfo[cur]->enode->v.sp->retemp = TRUE;
            tempInfo[cur]->enode->v.sp->value.i = cur;
            cur ++;
        }
    for (i=cur; i < tempCount; i++)
        tempInfo[i] = NULL;         
    tempCount = cur;
    head = intermed_head;
    while (head)
    {
        if  (head->dc.opcode != i_block && !head->ignoreMe && head->dc.opcode != i_passthrough && head->dc.opcode !=
            i_label)
        {
            if (head->ans)
            {
                if (head->ans->offset && head->ans->offset->type == en_tempref && !head->ans->offset->v.sp->retemp)
                {
                    head->ans->offset->v.sp->retemp = TRUE;
                    head->ans->offset->v.sp->value.i = map[head->ans->offset->v.sp->value.i];
                }
                if (head->ans->offset2 && head->ans->offset2->type == en_tempref && !head->ans->offset2->v.sp->retemp)
                {
                    head->ans->offset2->v.sp->retemp = TRUE;
                    head->ans->offset2->v.sp->value.i = map[head->ans->offset2->v.sp->value.i];
                }
            }
            if (head->dc.left)
            {
                if (head->dc.left->offset && head->dc.left->offset->type == en_tempref && !head->dc.left->offset->v.sp->retemp)
                {
                    head->dc.left->offset->v.sp->retemp = TRUE;
                    head->dc.left->offset->v.sp->value.i = map[head->dc.left->offset->v.sp->value.i];
                }
                if (head->dc.left->offset2 && head->dc.left->offset2->type == en_tempref && !head->dc.left->offset2->v.sp->retemp)
                {
                    head->dc.left->offset2->v.sp->retemp = TRUE;
                    head->dc.left->offset2->v.sp->value.i = map[head->dc.left->offset2->v.sp->value.i];
                }
            }
            if (head->dc.right)
            {
                if (head->dc.right->offset && head->dc.right->offset->type == en_tempref && !head->dc.right->offset->v.sp->retemp)
                {
                    head->dc.right->offset->v.sp->retemp = TRUE;
                    head->dc.right->offset->v.sp->value.i = map[head->dc.right->offset->v.sp->value.i];
                }
                if (head->dc.right->offset2 && head->dc.right->offset2->type == en_tempref && !head->dc.right->offset2->v.sp->retemp)
                {
                    head->dc.right->offset2->v.sp->retemp = TRUE;
                    head->dc.right->offset2->v.sp->value.i = map[head->dc.right->offset2->v.sp->value.i];
                }
            }
        }
        head = head->fwd;
    }
}
// note that once we get here, we start breaking the rule about how IMODES for the same
// temp should have the same address
void AllocateRegisters(QUAD *head)
{
    BOOLEAN first = TRUE;
    int firstSpill = TRUE;
    int spills = 0;
    int passes = 0;
    (void)head;
//	printf("%s:%d\n", theCurrentFunc->name, tempCount);
    retemp();
    accesses = 0;
    Prealloc(3);
    CalculateNesting();
    Precolor();
    while (TRUE)
    {
        int i;
        passes++;
        if (passes >= 100)
            break;
        for (i=0; i < tempCount; i++)
        {
            if (!tempInfo[i]->precolored)
            {
                tempInfo[i]->color = -1;
                if (tempInfo[i]->enode)
                    tempInfo[i]->enode->v.sp->regmode = 0;
            }
            tempInfo[i]->workingMoves = NULL;
            tempInfo[i]->partition = i;
        }
        CountInstructions(first);
        simplifyBottom = simplifyTop = 0;
        tempCount += 1000;
        simplifyWorklist = tAlloc(tempCount * sizeof(unsigned short));
        freezeWorklist = briggsAlloct(tempCount);
        spillWorklist = briggsAlloct(tempCount);
        spilledNodes = briggsAlloct(tempCount);
        coalescedNodes = aallocbit(tempCount);
        adjacent = tallocbit(tempCount);
        adjacent1 = tallocbit(tempCount);
        stackedTemps = tallocbit(tempCount);
        tempStack = tAlloc(tempCount * sizeof(int));
        tempCount -= 1000;
        liveVariables();
        CalculateFunctionFlags();
        InitClasses();
        definesInfo();
        usesInfo();
        CalculateConflictGraph(NULL, TRUE);
        SqueezeInit();
        Build(NULL);
        MkWorklist();
        for (i=0; i < tempCount; i++)
        {
            if (tempInfo[i]->degree)
                tempInfo[i]->spillCost = tempInfo[i]->spillCost / tempInfo[i]->degree;
        }
        first = FALSE;
        while (TRUE)
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
                    firstSpill = FALSE;
                }
                SelectSpill();
            }
            else 
            {
                break;
            }
        }
        for (i=0; i < tempCount; i++)
        {
            if (!tempInfo[i]->precolored)
            {
                tempInfo[i]->color = -1;
                if (tempInfo[i]->enode)
                    tempInfo[i]->enode->v.sp->regmode = 0;
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
        tFree();
        cFree();
    }
    KeepCoalescedNodes();
    CopyLocalColors();
    if (passes > maxAllocationPasses)
        maxAllocationPasses = passes;
    if (spills > maxAllocationSpills)
        maxAllocationSpills = spills;
    if (accesses > maxAllocationAccesses)
        maxAllocationAccesses = accesses;
    if (passes >= 100)
        diag("register allocator failed");
//	printf("%s:%d\n", theCurrentFunc->name, tempCount);
    tFree();
    aFree();
    cFree();
}

