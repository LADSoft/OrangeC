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
/*
 * iflow.c
 *
 * create flow graph and dominator tree
 */
/* Define this to get a dump of the flow graph and dominator tree 
 * These are dumped into ccfg.$$$
 */
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "compiler.h"

extern COMPILER_PARAMS cparams;
extern int blockCount;
extern QUAD *intermed_head, *intermed_tail;
extern BLOCK *currentBlock;

int firstLabel;
int exitBlock;
BLOCK **blockArray;
QUAD *criticalThunks , **criticalThunkPtr;

typedef struct _edge {
    struct _edge *next;
    int first;
    int second;
    enum e_fgtype edgetype;
} EDGE;

#define EDGE_HASH_SIZE 256

static EDGE *edgeHash[EDGE_HASH_SIZE] ;

static BLOCK **labels ;
int walkPreorder, walkPostorder;

    extern FILE *icdFile;
    
void flow_init(void)
{
}
    /* dump the flow graph */
    
void dump_flowgraph(void)
{
    int i;
    for ( i =0; i < blockCount; i++)
    {
        BLOCK *b = blockArray[i];
        BLOCKLIST *list1 = b->pred;
        int i;
        fprintf(icdFile, "\n; %d: ", b->blocknum + 1);
        while (list1)
        {
            fprintf(icdFile, " %d", list1->block->blocknum + 1);
            list1 = list1->next;
        }
        fprintf(icdFile, " *");
        list1 = b->succ;
        while (list1)
        {
            fprintf(icdFile, " %d", list1->block->blocknum + 1);
            list1 = list1->next;
        }
        
        fprintf(icdFile, "\n\t : %d /", b->idom + 1);
        list1 = b->dominates;
        while (list1)
        {
            fprintf(icdFile, " %d", list1->block->blocknum + 1);
            list1 = list1->next;
        }
        
        fprintf(icdFile, "\n\t $");
        list1 = b->dominanceFrontier;
        while (list1)
        {
            fprintf(icdFile, " %d", list1->block->blocknum + 1);
            list1 = list1->next;
        }
    }
    fprintf(icdFile,"\n");
}
static void RemoveDuplicateGotos(void)
{
    QUAD *head = intermed_head;
    while (head)
    {
        if (head->dc.opcode == i_goto)
        {
            while (head->fwd && head->fwd->dc.opcode == i_goto)
                RemoveInstruction(head->fwd);
        }
        head = head->fwd;
    }
}
static void basicFlowInfo(void)
{
    QUAD *head = intermed_head, *block;
    int low = INT_MAX, high = 0;
    blockArray = oAlloc(sizeof(BLOCK *) * (blockCount + 1000));
    while (head)
    {
        criticalThunks = intermed_head;
        if (head->dc.opcode == i_block)
        {
            if (!head->block->dead)
                blockArray[head->dc.v.label] = head->block ;
        }
        if (head->dc.opcode == i_label)
        {
            if (head->dc.v.label < low)
                low = head->dc.v.label;
            if (head->dc.v.label >= high)
                high = head->dc.v.label + 1;
        }
        head = head->fwd ;
    }
    labels = oAlloc(sizeof(BLOCK *) * (high - low)) ;
    firstLabel = low ;
    head = intermed_head;
    block = NULL;
    while (head)
    {
        if (head->dc.opcode == i_block)
        {
            block = head ;
        }
        if (head->dc.opcode == i_label)
        {
            labels[head->dc.v.label - firstLabel] = block->block ; /* the block num*/
        }
        head = head->fwd ;
    }
    
}
/* find a label on the list */
static BLOCK *findlab(int labnum)
{
    return labels[labnum - firstLabel];
}

/* insert on a flowgraph node */
static void flowinsert(BLOCKLIST **pos, BLOCK *valuesource)
{
    BLOCKLIST *nblock = oAlloc(sizeof(BLOCKLIST));
    nblock->next = (*pos);
    (*pos) = nblock;
    nblock->block = valuesource;
}
/* create the flowgraph.  */
static void gather_flowgraph(void)
{
    BLOCK *temp;
    int i;
    for (i=0 ; i < blockCount; i++)
    {
        BLOCK *b = blockArray[i];
        if (!b->dead)
        {
            QUAD *tail = b->tail;
            while (tail->ignoreMe)
                tail = tail->back;
            switch (tail->dc.opcode)
            {
                case i_goto:
                case i_asmgoto:
                    temp = findlab(tail->dc.v.label);
                    if (temp)
                    {
                        flowinsert(&temp->pred, b);
                        flowinsert(&b->succ, temp);
                    }
                    else
                    {
                        diag("flow:unfound label");
    /*                    printf("g %d", tail->dc.v.label); */
                    }
                    break;
                case i_swbranch:
                    while (tail->dc.opcode == i_swbranch)
                    {
                        temp = findlab(tail->dc.v.label);
                        if (temp)
                        {
                            flowinsert(&temp->pred, b);
                            flowinsert(&b->succ, temp);
                        }
                        else
                        {
                            diag("flow:unfound label");
    /*                        printf("dc %d", tail->dc.v.label); */
                        }
                        tail = tail->back;
                    }
                    // fallthrough
                case i_coswitch:
                    temp = findlab(tail->dc.v.label);
                    if (temp)
                    {
                        flowinsert(&temp->pred, b);
                        flowinsert(&b->succ, temp);
                    }
                    else
                    {
                        diag("flow:unfound label");
    /*                    printf("cs %d", tail->dc.v.label); */
                    }
                    break;
                case i_asmcond:
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
                    temp = findlab(tail->dc.v.label);
                    if (temp)
                    {
                        flowinsert(&temp->pred, b);
                        flowinsert(&b->succ, temp);
                    }
                    else
                    {
                        diag("flow:unfound label");
    /*                    printf("j %d", tail->dc.v.label); */
                    }
                    /* fall through */
                default:
                    if (i != blockCount-1)
                    {
                        BLOCK *temp = blockArray[i+1];
                        flowinsert(&temp->pred, b);
                        flowinsert(&b->succ, temp);
                    }
                    break;
            }
        }
    }
    // associate live blocks without a predecessor with blockarray[0]
    for (i=0; i < blockCount; i++)
    {
        BLOCK *b = blockArray[i];
        if (b->alwayslive && !b->pred)
        {
            BLOCK *temp = blockArray[0];
            flowinsert(&temp->succ, b);
            flowinsert(&b->pred, temp);
        }
    }
}


static void FGWalker(int (*func)(enum e_fgtype type, BLOCK *parent, BLOCK *b), 
                     BLOCK *cur, int fwd)
{
    BLOCKLIST *l ;
    if (fwd)
        l = cur->succ;
    else 
        l = cur->pred;
    cur->preWalk = walkPreorder++ ;
    while (l)
    {
        if (l->block->preWalk == 0)
        {
            /* tree*/
            if (func(F_TREE, cur, l->block))
                FGWalker(func, l->block, fwd);
        } 
        else if (l->block->postWalk == 0)
        {
            /* back edge*/
            func(F_BACKEDGE, cur, l->block);
        } else if (l->block->preWalk > cur->preWalk)
        {
            /* forward edge*/
            func(F_FORWARDEDGE, cur, l->block);
        } else
        {
            /* cross edge*/
            func(F_CROSSEDGE, cur, l->block);
        }
        l = l->next ;
    }
    cur->postWalk = walkPostorder++;	
}
/* depth first search entry point */
void WalkFlowgraph(BLOCK *b, 
                   int (*func)(enum e_fgtype type, BLOCK *parent, BLOCK *b), int fwd)
{
    int i;
    walkPreorder = 1 ;
    walkPostorder = 1;
    
    for (i=0; i < blockCount; i++)
        if (blockArray[i])
            blockArray[i]->preWalk = blockArray[i]->postWalk = 0;

    if (func(F_TREE, NULL, b))
        FGWalker(func, b, fwd);
}
// have to do this to make the rtti happy in the face of infinite blocks
static void MoveLabelsToExit(BLOCK *b)
{
    QUAD *head = b->head;
    while (head && head != b->tail)
    {
        QUAD *next = head->fwd;
        if (head->dc.opcode == i_label)
        {
            QUAD *q = blockArray[exitBlock]->head;
            head->fwd->back = head->back;
            head->back->fwd = head->fwd;
            q->fwd->back = head;
            head->fwd = q->fwd;
            q->fwd = head;
            head->back = q;
        }
        head = next;
    }
}
static void removeDeadBlocks(BRIGGS_SET *brs, BLOCK *back, BLOCK *b)
{
    if (b->temp)
        return;
    b->temp = TRUE;
    if (!briggsTest(brs, b->blocknum) && b->blocknum != exitBlock)
    {
        BLOCKLIST *bl2 = b->succ;
        while (bl2)
        {
            removeDeadBlocks(brs, b, bl2->block);
            if (!bl2->block->dead)
            {
                BLOCKLIST **bl3 = &bl2->block->pred;
                int n = 0;
                while (*bl3)
                {
                    if ((*bl3)->block == b)
                    {
                        QUAD *head = bl2->block->head->fwd;
                        *bl3 = (*bl3)->next;
                        while (head != bl2->block->tail->fwd && (head->dc.opcode == i_label ||
                               head->ignoreMe))
                        {
                            head = head->fwd;
                        }
                        while (head->dc.opcode == i_phi && head != bl2->block->tail->fwd)
                        {
                            PHIDATA *pd = head->dc.v.phi;
                            struct _phiblock **pb = &pd->temps;
                            int i;
                            for (i=0; i < n; i++, pb = &(*pb)->next) ;
                            (*pb) = (*pb)->next;			
                            head = head->fwd;
                        }
                        break;
                    }
                    n++;
                    bl3 = &(*bl3)->next;
                }
            }
            bl2 = bl2->next;
        }
        if (cparams.prm_cplusplus)
            MoveLabelsToExit(b);
        b->head->fwd = b->tail->fwd;
        b->tail->fwd->back = b->head;
        b->tail = b->head;
        b->succ = b->pred = NULL;
        b->dead = TRUE;
        b->critical = FALSE;
        blockArray[b->blocknum] = NULL;
    }
    else
    {
        QUAD *head ;
        int n = 0;
        BLOCKLIST **bl = &b->pred;
        while (bl)
        {
            if ((*bl)->block == back)
            {
                *bl = (*bl)->next;
                break;
            }
            n++;
            bl = &(*bl)->next;
        }
        
        head = b->head->fwd;
        while (head != b->tail->fwd && (head->dc.opcode == i_label || 
               head->ignoreMe))
        {
            head = head->fwd;
        }
        while (head->dc.opcode == i_phi && head != b->tail->fwd)
        {
            PHIDATA *pd = head->dc.v.phi;
            struct _phiblock **pb = &pd->temps;
            int i;
            for (i=0; i < n; i++, pb = &(*pb)->next) ;
            (*pb) = (*pb)->next;			
            head = head->fwd;
        }
    }
}
static void removeMark(BRIGGS_SET *bls, BLOCK *b)
{
    BLOCKLIST *bl;
    if (b->blocknum == exitBlock || b->temp)
        return;
    b->temp = TRUE;
    briggsSet(bls, b->blocknum);
    bl = b->succ;
    while (bl)
    {
        removeMark(bls, bl->block);
        bl = bl->next;
    }
}
void reflowConditional(BLOCK *src, BLOCK *dst)
{
    BLOCKLIST *bl, *bl1 = src->succ;
    BLOCKLIST temp;
    BRIGGS_SET *bls = briggsAlloc(blockCount);
    int i;
    temp.block = dst;
    temp.next = NULL;
    src->succ = &temp;
    for (i=0; i < blockCount; i++)
        if (blockArray[i])
            blockArray[i]->temp = FALSE;
    removeMark(bls, blockArray[0]);
    for (i=1; i < blockCount; i++)
        if (blockArray[i] && blockArray[i]->alwayslive)
            removeMark(bls, blockArray[i]);
    src->succ = bl1;
    for (i=0; i < blockCount; i++)
        if (blockArray[i])
            blockArray[i]->temp = FALSE;
    bl = bl1;
    while (bl)
    {
        if (bl->block != dst)
            removeDeadBlocks(bls, src, bl->block);
        bl = bl->next;
    }
    if (dst->critical)
    {
        BLOCK *crit = dst;
        BLOCKLIST *bl = crit->succ->block->pred;
        while (bl)
        {
            if (bl->block == crit)
            {
                bl->block = src;
                break;
            }
            bl = bl->next;
        }
        dst = crit->succ->block;
        crit->succ = crit->pred = NULL;
        blockArray[crit->blocknum] = NULL;	
    }
    src->succ->block = dst;
    src->succ->next = NULL;
}
/* not even thinking about assert() or longjmp() */
static int RemoveCriticalEdges(enum e_fgtype type, BLOCK *parent, BLOCK *in)
{
    (void)parent;
    if (type == F_TREE)
    {
        BLOCKLIST *f = in->succ;
        if (f && f->next)
        {
            while (f)
            {
                BLOCKLIST *b = f->block->pred;
                if (b && b->next)
                {
                    /* critical edge, we need to insert something... */
                    /* we won't need to revist this so we don't care what
                     * it does to the presetn tree walk
                     */
                    BLOCKLIST *m = newBlock();
                    BLOCKLIST *q ;

                    QUAD *quad, *quad2;
                    
                    m->block->critical = TRUE;
                    
                    /* note : the following does NOT insert jmps for the
                     * newly added block
                     */
                    *criticalThunkPtr = quad = Alloc(sizeof(QUAD));
                    criticalThunkPtr = & quad->fwd;
                    quad->dc.opcode = i_block;
                    quad->block = m->block;
                    quad->dc.v.label = m->block->blocknum;

                    *criticalThunkPtr = quad2 = Alloc(sizeof(QUAD));
                    criticalThunkPtr = & quad2->fwd;
                    quad2->back = quad;
                    quad2->dc.opcode = i_blockend;
                        quad2->block = m->block;
//					quad2->dc.v.label = m->block->blocknum;
                                        
                    m->block->head = quad;
                    m->block->tail = quad2;

                    q = oAlloc(sizeof(BLOCKLIST));
                    q->block = in;
                    m->block->pred = q;
                    q = oAlloc(sizeof(BLOCKLIST));
                    q->block = f->block;
                    m->block->succ = q;

                    f->block = m->block;
                    q = b;
                    while (q)
                    {
                        if (q->block->blocknum == in->blocknum)
                        {
                            q->block = m->block;
                            break;
                        }
                        q = q->next;
                    }
                    
                }
                f = f->next;
            }
        }
    }
    return TRUE;
}
/* Lengauer-tarjan method for computing the dominator tree */
/* this implementation based on a paper by Martin Richards */
static int domCount ;

/* this algorithm could be implemented more efficiently with individual arrays
 * than with a structure like this, but, for now we'll take a minor penalty
 * for structured coding
 */
static struct _tarjan
{
    int blocknum;
    int parent;
    BLOCKLIST *preds, *succs;
    int semi;
    int idom;
    int ancestor;
    int best;
    BRIGGS_SET *bucket;
} **vectorData;
static int domNumber(enum e_fgtype t, BLOCK *parent, BLOCK *b)
{
    (void)parent;
    if (t == F_TREE)
    {
        b->dfstOrder = ++domCount;
    }
    return TRUE;
}
static int domInit(enum e_fgtype t, BLOCK *parent, BLOCK *b)
{

    if (t == F_TREE && parent != NULL)
    {
        struct _tarjan *t = vectorData[b->dfstOrder];
        t->blocknum = b->blocknum;
        t->parent = parent->dfstOrder;
        t->succs = b->succ;
        t->preds = b->pred;
        t->semi = b->dfstOrder;
        t->idom = 0;
        t->ancestor = 0;
        t->best = b->dfstOrder;
    }
    return TRUE;
}
#define domLink(v, w) (vectorData[w]->ancestor = (v))

#ifdef SLOW
static int domEval(int v)
{
    int a = vectorData[v]->ancestor;
    while (vectorData[a]->ancestor)
    {
        if (vectorData[v]->semi > vectorData[a]->semi)
            v = a;
        a = vectorData[a]->ancestor;
    }
    /* v now is vertex with smallest semidominator */
    return v;
}
#else
static void domCompress(int v)
{
    int a = vectorData[v]->ancestor;
    if (vectorData[a]->ancestor)
    {
        domCompress(a);
        if (vectorData[vectorData[v]->best]->semi > vectorData[vectorData[a]->best]->semi)
            vectorData[v]->best = vectorData[a]->best;
        vectorData[v]->ancestor = vectorData[a]->ancestor;
    }
        
}
static int domEval(int v)
{
    if (!vectorData[v]->ancestor)
        return v;
    domCompress(v);
    return vectorData[v]->best;
}
#endif
static void PostDominators(void)
{
    int w, i;
    domCount = 0;
    WalkFlowgraph(blockArray[exitBlock], domNumber, FALSE);
    vectorData = tAlloc(sizeof(struct _tarjan *) * (domCount + 1));
    for (i=0; i <= domCount; i++)
    {
        vectorData[i] = tAlloc(sizeof(struct _tarjan));
        vectorData[i]->bucket = briggsAlloct(domCount+1);
    }
    WalkFlowgraph(blockArray[exitBlock], domInit, FALSE);
    
    for (w = domCount; w>=2; w--)
    {
        int p = vectorData[w]->parent;
        int j;
        struct _tarjan *v = vectorData[w];
        BLOCKLIST *bl = v->preds;
        while (bl)
        {
            int u = domEval(bl->block->dfstOrder);
            if (vectorData[w]->semi > vectorData[u]->semi)
                vectorData[w]->semi = vectorData[u]->semi;
            bl = bl->next;
        }
        briggsSet(vectorData[vectorData[w]->semi]->bucket, w);
        domLink(p, w);
        for (j = vectorData[p]->bucket->top -1 ; j >= 0; j--)
        {
            int v = vectorData[p]->bucket->data[j];
            int u = domEval(v);
            if (vectorData[u]->semi >= p)
                vectorData[v]->idom = p;
            else
                vectorData[v]->idom = u;
        }
        briggsClear(vectorData[p]->bucket);		
    }
    for (w=2; w <=domCount; w++)
    {
        if (vectorData[w]->idom != vectorData[w]->semi)
        {
            vectorData[w]->idom = vectorData[vectorData[w]->idom]->idom;
        }
        /* transfer idom to our block struct */
        blockArray[vectorData[w]->blocknum]->pdom = vectorData[vectorData[w]->idom]->blocknum;
    }
    tFree();
}
static void Dominators(void)
{
    int w, i;
    domCount = 0;
    WalkFlowgraph(blockArray[0], domNumber, TRUE);
    vectorData = tAlloc(sizeof(struct _tarjan *) * (domCount + 1));
    for (i=0; i <= domCount; i++)
    {
        vectorData[i] = tAlloc(sizeof(struct _tarjan));
        vectorData[i]->bucket = briggsAlloct(domCount+1);
    }
    WalkFlowgraph(blockArray[0], domInit, TRUE);
    
    for (w = domCount; w>=2; w--)
    {
        int p = vectorData[w]->parent;
        int j;
        struct _tarjan *v = vectorData[w];
        BLOCKLIST *bl = v->preds;
        while (bl)
        {
            int u = domEval(bl->block->dfstOrder);
            if (vectorData[w]->semi > vectorData[u]->semi)
                vectorData[w]->semi = vectorData[u]->semi;
            bl = bl->next;
        }
        briggsSet(vectorData[vectorData[w]->semi]->bucket, w);
        domLink(p, w);
        for (j = vectorData[p]->bucket->top -1 ; j >= 0; j--)
        {
            int v = vectorData[p]->bucket->data[j];
            int u = domEval(v);
            if (vectorData[u]->semi >= p)
                vectorData[v]->idom = p;
            else
                vectorData[v]->idom = u;
        }
        briggsClear(vectorData[p]->bucket);		
    }
    for (w=2; w <=domCount; w++)
    {
        if (vectorData[w]->idom != vectorData[w]->semi)
        {
            vectorData[w]->idom = vectorData[vectorData[w]->idom]->idom;
        }
        /* transfer idom to our block struct */
        blockArray[vectorData[w]->blocknum]->idom = vectorData[vectorData[w]->idom]->blocknum;
    }
    /* now make the forward tree */
    for (i=blockCount-1; i >=1; i--)
    {
        /* make dominates lists by walking the idom tree backwards from each node*/
        if (blockArray[i] && blockArray[i]->pred)
        {
            int w = blockArray[i]->idom;
            BLOCK *ub = blockArray[w];
            BLOCKLIST *bl = oAlloc(sizeof(BLOCKLIST));
            bl->block = blockArray[i];
            bl->next = ub->dominates;
            ub->dominates = bl;
        }
    }
    tFree();
}
BOOLEAN dominatedby(BLOCK *src, BLOCK *ancestor)
{
    int idom = src->idom;
    while (idom > 0)
    {
        if (idom == ancestor->blocknum)
            return TRUE;
        idom = blockArray[idom]->idom;
    }
    return FALSE;
}
/* we are also calculating the reverse postorder here
 * it will be needed later for the loop handling
 */
static void DominanceFrontier(BLOCK *b, int *count)
{
    BLOCKLIST *c = b->dominates, *s;
    while (c)
    {
        DominanceFrontier(c->block, count);
        c = c->next;
    }
    b->reversePostOrder = (*count)++;
    s = b->succ;
    while (s)
    {
        if (s->block->idom != b->blocknum)
        {
            BLOCKLIST *t = oAlloc(sizeof(BLOCKLIST));
            t->next = b->dominanceFrontier;
            t->block = s->block;
            b->dominanceFrontier = t;
        }
        s = s->next;
    }
    c = b->dominates;
    while (c)
    {
        BLOCKLIST *bl = c->block->dominanceFrontier;
        int i;
        while (bl)
        {
            if (!dominatedby(bl->block, b))
            {
                BLOCKLIST *t = oAlloc(sizeof(BLOCKLIST));
                t->next = b->dominanceFrontier;
                t->block = bl->block;
                b->dominanceFrontier = t;
            }
            bl = bl->next;
        }
        c = c->next;
    }
}
static int hashfunc(int i, int j)
{
    return (j * (j- 1) / 2 + i) % EDGE_HASH_SIZE;
}
static int gatherEdges(enum e_fgtype type, BLOCK *parent, BLOCK *in)
{
    if (parent)
    {
        EDGE *edge = oAlloc(sizeof(EDGE));
        int bucket = hashfunc(parent->blocknum, in->blocknum);
        edge->first = parent->blocknum;
        edge->second = in->blocknum;
        edge->edgetype = type;
        edge->next = edgeHash[bucket];
        edgeHash[bucket] = edge;
    }
    return TRUE;
}
static void CalculateEdges(BLOCK *b)
{
    memset(edgeHash, 0, EDGE_HASH_SIZE * sizeof(EDGE *));
    WalkFlowgraph(b, gatherEdges, TRUE);
}
enum e_fgtype getEdgeType(int first, int second)
{
    int bucket= hashfunc(first, second);
    EDGE *p = edgeHash[bucket];
    while (p)
    {
        if (p->first == first && p->second == second)
            return p->edgetype;
        p = p->next;
    }
    return F_NONE;
}
/* SSA doesn't work well when there are dead paths */
static void removeDeadBlock(BLOCK *b)
{
    if (b->pred == NULL && !b->alwayslive)
    {
        BLOCKLIST *bl = b->succ;
        while (bl)
        {
            unlinkBlock(bl->block, b);
            bl = bl->next;
        }
		if (b->critical)
		{
			b->critical = FALSE;
			blockArray[b->blocknum] = 0;
		}
        if (b->head != b->tail && b->head->fwd != b->tail)
        {   QUAD *p = b->head->fwd;
            QUAD *q = b->tail->fwd;
            while (q != p)
            {
                if (p->dc.opcode != i_dbgblock && p->dc.opcode != i_dbgblockend 
                    && p->dc.opcode != i_var && p->dc.opcode != i_func && p->dc.opcode != i_label)
                {
                    RemoveInstruction(p);
                }
                p = p->fwd;
            }
        }
    }
}
static void InsertLabel(BLOCK *b, int label)
{
    // insert a label in the target block
    QUAD *head = b->head;
    QUAD *lbl = Alloc(sizeof(QUAD));
    lbl->dc.opcode = i_label;
    lbl->dc.v.label = label;
    while (head->ignoreMe || head->dc.opcode == i_block)
        head = head->fwd;
    lbl->back = head->back;
    lbl->fwd = head;
    if (head->back == b->tail)
    {
        b->tail = lbl;
    }
    head->back = head->back->fwd = lbl;
    lbl->block = b;
}
static void MoveBlockTo(BLOCK *b)
{
    BLOCK *prev = b->pred->block;
    BLOCK *succ = b->succ->block;
    QUAD *head;
    QUAD *tail = prev->tail;
    QUAD *jmp = Alloc(sizeof(QUAD));
    QUAD *insert = b->tail;
    int label = nextLabel++;
    if (b->tail->dc.opcode == i_blockend)
        RemoveInstruction(b->tail);
    // insert a jump at the end of the block
    jmp->dc.opcode = i_goto;
    jmp->block = b;
    jmp->dc.v.label = label;
    jmp->fwd = b->tail->fwd;
    jmp->back = b->tail;
    if (jmp->fwd)
        jmp->fwd->back = jmp;
    jmp->back->fwd = jmp;
    b->tail = jmp;
    
    InsertLabel(succ, label);
    
    // unlink
    if (b->head == criticalThunks)
        criticalThunks = b->tail->fwd;
    if (b->head->back)
        b->head->back->fwd = b->tail->fwd;
    if (b->tail->fwd)
        b->tail->fwd->back = b->head->back;
    
    // relink
    prev->tail->fwd->back = b->tail;
    b->tail->fwd = prev->tail->fwd;
    prev->tail->fwd = b->head;
    b->head->back = prev->tail;
}
static void SwapBranchSense(QUAD *jmp)
{
    BLOCK *b = jmp->block;
    BLOCKLIST *f = b->succ;
    BLOCKLIST *s = f->next;
    s->next = f;
    f->next = NULL;
    b->succ = s;
    switch (jmp->dc.opcode)
    {
        case i_je:
            jmp->dc.opcode = i_jne;
            break;
        case i_jne:
            jmp->dc.opcode = i_je;
            break;
        case i_jl:
            jmp->dc.opcode = i_jge;
            break;
        case i_jc:
            jmp->dc.opcode = i_jnc;
            break;
        case i_jg:
            jmp->dc.opcode = i_jle;
            break;
        case i_ja:
            jmp->dc.opcode = i_jbe;
            break;
        case i_jle:
            jmp->dc.opcode = i_jg;
            break;
        case i_jbe:
            jmp->dc.opcode = i_ja;
            break;
        case i_jge:
            jmp->dc.opcode = i_jl;
            break;
        case i_jnc:
            jmp->dc.opcode = i_jc;
            break;
        default:
            diag("SwapBranchSense: non-branch");
            break;
    }
    
}
void UnlinkCritical(BLOCK *s)
{
    BLOCKLIST *bl;
    s->critical = FALSE;
    s->dead = TRUE;
    blockArray[s->blocknum] = NULL;
    bl = s->pred->block->succ;
    while (bl)
    {
        if (bl->block == s)
        {
            bl->block = s->succ->block;
            break;
        }
        bl = bl->next;
    }
    bl = s->succ->block->pred;
    while (bl)
    {
        if (bl->block == s)
        {
            bl->block = s->pred->block;
            break;
        }
        bl = bl->next;
    }
}
void RemoveCriticalThunks(void)
{
    int i;
    for (i=0; i < blockCount; i++)
    {
        BLOCK *b = blockArray[i];
        if (b && i != exitBlock && !b->critical && !b->dead)
        {
            QUAD *bjmp = beforeJmp(b->tail, FALSE);
            if (bjmp->dc.opcode == i_coswitch)
            {
                
                QUAD *i = bjmp;
                BLOCKLIST *sl = b->succ;
                while (sl)
                {
                    BLOCK *s = sl->block;
                    if (s->critical)
                    {
                        s->critical = FALSE;
                        if (s->head->fwd != s->tail)
                        {
                            int lbl = nextLabel++;
                            MoveBlockTo(s);
                            InsertLabel(s, lbl);
                            i->dc.v.label = lbl;
                        }
                        else
                        {
                            UnlinkCritical(s);
                        }
                    }
                    i = i->fwd;
                    sl = sl->next;
                }
            }
            else if (bjmp->dc.opcode >= i_jne && bjmp->dc.opcode <= i_jge)
            {
                BLOCK *s = b->succ->block;
                BLOCK *n = b->succ->next->block;
                if (s->critical)
                {
                    s->critical = FALSE;
                    if (s->head->fwd != s->tail)
                    {
                        MoveBlockTo(s);
                        if (n->critical)
                        {
                            n->critical = FALSE;
                            if (n->head->fwd != n->tail)
                            {
                                int lbl = nextLabel++;
                                InsertLabel(b->succ->block, lbl);
                                bjmp->dc.v.label = lbl;
                                SwapBranchSense(bjmp);
                                MoveBlockTo(n);
                            }
                            else
                            {
                                UnlinkCritical(n);
                            }
                        }
                    }
                    else 
                    {
                        UnlinkCritical(s);
                        if (n->critical)
                        {
                            n->critical = FALSE;
                            if (n->head->fwd != n->tail)
                            {
                                int lbl = nextLabel ++;
                                bjmp->dc.v.label = lbl;
                                InsertLabel(b->succ->block, lbl);			
                                SwapBranchSense(bjmp);
                                MoveBlockTo(n);
                            }
                            else
                            {
                                UnlinkCritical(n);
                            }
                        }
                    }
                }
                else if (n->critical)
                {
                    n->critical = FALSE;
                    if (n->head->fwd != n->tail)
                    {
                        int lbl = nextLabel ++;
                        bjmp->dc.v.label = lbl;
                        InsertLabel(b->succ->block, lbl);			
                        SwapBranchSense(bjmp);
                        MoveBlockTo(n);
                    }
                    else
                    {
                        UnlinkCritical(n);
                    }
                }
            }
            else if (b->succ->block->critical)
            {
                BLOCK *s = b->succ->block;
                s->critical = FALSE;
                if (b->succ->next)
                    diag("RemoveCritical - invalid flow");
                if (s->head->fwd != s->tail)
                {
                    MoveBlockTo(s);
                    if (bjmp->dc.opcode == i_goto)
                    {
                        RemoveInstruction(bjmp);
                    }
                }
                else
                {
                    UnlinkCritical(s);
                }
            }
        }
    }
}
void unlinkBlock(BLOCK *succ, BLOCK *pred)
{
    BLOCKLIST **bl = &succ->pred;
    while (*bl && (*bl)->block != pred)
        bl = &(*bl)->next;
    if (*bl)
    {
        *bl = (*bl)->next;	
        removeDeadBlock(succ);
    }
}
void doms_only(BOOLEAN always)
{
    int dfsCount = 0;
    int i;
    int n = blockCount;
    (void)always;
    if (always)
    {
        criticalThunks = NULL;
        criticalThunkPtr = &criticalThunks;
    }
    WalkFlowgraph(blockArray[0], RemoveCriticalEdges, TRUE);
    if (blockCount != n)
    {
        QUAD *head = intermed_head;
        blockArray = oAlloc(sizeof(BLOCK *) * (blockCount + 1000)); // fixme...
        while (head)
        {
            if (head->dc.opcode == i_block)
            {
                blockArray[head->dc.v.label] = head->block ;
            }
            head = head->fwd ;
        }
        head = criticalThunks;
        while (head)
        {
            if (head->dc.opcode == i_block)
            {
                blockArray[head->dc.v.label] = head->block ;
            }
            head = head->fwd ;
        }
        *criticalThunkPtr = Alloc(sizeof(QUAD));
        (*criticalThunkPtr)->dc.opcode = i_label;
        (*criticalThunkPtr)->dc.v.label = -1;
        criticalThunkPtr = (QUAD **)*criticalThunkPtr;
    }
    CancelInfinite(blockCount);

    blockArray[0]->dominates = NULL;
    for (i=1; i < blockCount; i++)
    {
        if (blockArray[i])
        {
            removeDeadBlock(blockArray[i]);
			if (blockArray[i])
			{
	            blockArray[i]->idom = 0;
		        blockArray[i]->dominates = NULL;
			}
        }
    }
    Dominators();
    PostDominators();
    for (i=0; i < blockCount; i++)
    {
        if (blockArray[i])
        {
            blockArray[i]->visiteddfst = blockArray[i]->pred != NULL;
            blockArray[i]->dominanceFrontier = NULL;
        }
    }
    DominanceFrontier(blockArray[0], &dfsCount);
    CalculateEdges(blockArray[0]);
    BuildLoopTree();
}
/* main routine for creating flowgraph and dominator info */
void flows_and_doms(void)
{
    BLOCKLIST *bl;
    int i;
    if (exitBlock == 0)
    {
        exitBlock = blockCount-1;
        currentBlock->tail = intermed_tail;
    }
    else
    {
        int i;
        for (i=0; i < blockCount; i++)
        {
            blockArray[i]->succ = blockArray[i]->pred = NULL;
        }
    }

    RemoveDuplicateGotos();        
    basicFlowInfo();
    gather_flowgraph();
    doms_only(TRUE);
    if (icdFile)
    {
        fprintf(icdFile, "; Flowgraph dump");
        dump_flowgraph();
    }
}
