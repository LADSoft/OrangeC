/* 
CC386 C Compiler
Copyright 1994-2011 David Lindauer.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

This program is derived from the cc68k complier by 
Matthew Brandt (mailto::mattb@walkingdog.net) 

You may contact the author of this derivative at:

mailto::camille@bluegrass.net
 */
#include <stdio.h>
#include <string.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "ccerr.h"

extern TYP stdint;
extern ENODE *blockVarArraySP;
extern int prm_c99;
extern int stdpragmas;
extern LIST *local_using_list,  *local_tag_using_list;
extern char *templatePrefix;
extern int mainfunc;
extern int lineno;
extern SNODE *cbautoinithead,  *cbautoinittail;
extern int ispascal, isstdcall;
extern FILE *listFile;
extern enum e_sym lastst;
extern long firstlabel, nextlabel;
extern char lastid[];
extern TABLE *gsyms, lsyms, ltags;
extern int global_flag;
extern int stdaddrsize, stdretblocksize;
extern int stackadd, stackmod;
extern TYP *head,  *tail,  **headptr;
extern char declid[100];
extern int prm_listfile;
extern int prm_cplusplus, prm_debug;
extern int stdintsize;
extern int goodcode;
extern int prm_linkreg;
extern TABLE lsyms;
extern int total_errors;
extern int cppflags;
extern SYM *declclass,  *typequal;
extern int prm_cmangle;
extern LASTDEST *lastDestPointer;
extern int conscount;
extern int prm_xcept;
extern TRYBLOCK *try_block_list;
extern int switchbreak;
extern int asmMask, asmRMask;
#ifdef i386
extern int noStackFrame, stackframeoff;
#endif

typedef struct _dlist {
    struct _dlist *fwd, *back ;
    void *data, *current ;
} DLIST ;

int setjmp_used;

TABLE labelsyms;

DLIST *gotoList ;

ENODE *allocaSP;
int used_alloca;

int gotoCount;
int block_nesting, arg_nesting;
int infuncargs, infunclineno;
int funcnesting;
ENODE *block_rundown; /* maintains destructors for block, WILL maintain global
 * scope when no funcs in progress but we don't use that
 * now
 */
TABLE oldlsym, oldltag;
LIST *varlisthead,  *varlisttail;
LIST *instantiated_inlines;
SNODE *funcstmt,  *funcendstmt, *funcbrstmt;

static int skm_func[] = 
{
    closepa, begin, semicolon, 0
};

SYM *currentfunc = 0,  *lastfunc;

/*      function compilation routines           */
void funcini(void)
{
    currentfunc = lastfunc = 0;
    infuncargs = 0;
    funcnesting = 0;
    instantiated_inlines = 0;
}

//-------------------------------------------------------------------------

void declfuncarg(int isint, int isfar, SYM *sp)
{
    char *names[50]; /* 50 parameters maximum */
    int nparms, poffset, i, isinline;
    SYM *sp1,  *sp2,  *spsave[50],  *oldargs = 0,  *typesp = typequal;
    TYP *temp,  *temp1,  *todo,  *oheadptr, *oldrv = 0;
    char oldeclare[100];
    char *nm;
    sp->hasproto = TRUE;
    sp2 = gsearch(declid);
	if (sp2)
		oldargs = sp2->tp->lst.head;
    if (!infuncargs)
    {
        if (lineno != infunclineno)
            funcstmt = snp_line(FALSE);
        // only want the first one
        infunclineno = lineno;
    }
    infuncargs++;
    if (prm_linkreg && !isint)
    {
        poffset = stdretblocksize; /* size of return block */
        if (isfar)
            poffset += stdintsize;
    }
    else
        poffset = 0;
     /* size of return block */
    nparms = 0;
    temp = head;
    temp1 = tail;
    oheadptr = headptr;
    todo = sp->tp;
    todo->lst.head = todo->lst.tail = 0;
    if (lastst == kw_void)
    {
        getsym();
        if (lastst != closepa)
        {
            backup(lastst);
            lastst = kw_void;
        }
        else
        {
            todo->lst.head = (SYM*) - 1;
            getsym();
            goto verify;
        }
    }
    else
    if (lastst == closepa)
    {
        if (prm_cplusplus)
            todo->lst.head = (SYM*) - 1;
        getsym();
        goto verify;
    } else
        if (lastst == id) 
        {
            if ((typesp = typesearch(lastid)) && (typesp->storage_class ==
                sc_type))
                if (typesp->tp->type == bt_void) 
                {
                    getsym();
                    if (lastst != closepa)
                    {
                        backup(lastst);
                        lastst = id;
                    }
                    else
                    {
                        todo->lst.head = (SYM*) - 1;
                        getsym();
                        goto verify;
                    }
                }
        }
    strcpy(oldeclare, declid);
    if (!prm_cplusplus && lastst == id && ((sp1 = search(lastid, gsyms)) == 0 
        || sp1->storage_class != sc_type) && ((sp1 = search(lastid, &lsyms)) ==
        0 || sp1->storage_class != sc_type))
    {
        int xglob = global_flag;
        /* declare parameters */
        global_flag = 0;
        while (lastst == id)
        {
            names[nparms++] = litlate(lastid);
            getsym();
            if (lastst == comma)
                getsym();
            else
                break;
        }
        needpunc(closepa, skm_func);
        sp->oldstyle = xalloc((nparms + 1) *sizeof(char*));
        sp->oldstyle[0] = nparms;
        memcpy(&sp->oldstyle[1], names, nparms *sizeof(char*));
        global_flag = xglob;
        infuncargs--;
        return ;
    }
    else
    {
        SYM *tq = typequal;
        doargdecl(sc_member, names, &nparms, &todo->lst, isinline = TRUE);
        needpunc(closepa, skm_func);
        replaceTemplateFunctionClasses(sp, tq);
    }
    strcpy(declid, oldeclare);
    if (!ispascal && isstructured(todo->btp))
    {
        poffset += stdaddrsize;
        sp->calleenearret = TRUE;
    }
    typequal = 0;
    for (i = 0; i < nparms; ++i)
    {
        if ((sp1 = search(names[i], &todo->lst)) == 0)
            sp1 = makeint(litlate(names[i]), &todo->lst);
        if (sp1->tp)
            sp1->tp->uflags |= UF_DEFINED;
        sp1->funcparm = TRUE;
        spsave[i] = sp1;
        sp1->storage_class = sc_auto;
    }
    typequal = typesp;
    /*
     * parameter allocation.  Have to do things backwards if this
     * function has the pascal declarator
     */
    if (!ispascal)
    for (i = 0; i < nparms; ++i)
    {
        sp1 = spsave[i];
        if (sp1->tp->type != bt_pointer && sp1->tp->type != bt_farpointer &&
            sp1->tp->type != bt_segpointer && sp1->tp->size < stdintsize)
        {
            sp1->value.i = poffset + funcvaluesize(sp1->tp->size);
            poffset += stdintsize;
        }
        else
        {
            sp1->value.i = poffset;
            if (sp1->tp->type == bt_pointer)
            {
                TYP *tp = sp1->tp;
                if (!(tp->val_flag &VARARRAY))
                    tp = tp->btp;
                if (tp->type != bt_pointer || !(tp->val_flag &VARARRAY))
                    poffset += stdaddrsize;
                else
                {
                    poffset += tp->size;
                }
            }
            else
                poffset += sp1->tp->size;
            if (poffset % stdintsize)
                poffset += stdintsize - poffset % stdintsize;
        }
    }
    else
    for (i = nparms - 1; i >= 0; --i)
    {
        sp1 = spsave[i];
        if (sp1->tp->type != bt_pointer && sp1->tp->type != bt_farpointer &&
            sp1->tp->type != bt_segpointer && sp1->tp->size < stdintsize)
        {
            sp1->value.i = poffset + funcvaluesize(sp1->tp->size);
            poffset += stdintsize;
        }
        else
        {
            sp1->value.i = poffset;
            if (sp1->tp->type == bt_pointer)
            {
                TYP *tp = sp1->tp;
                if (!(tp->val_flag &VARARRAY))
                    tp = tp->btp;
                if (tp->type != bt_pointer || !(tp->val_flag &VARARRAY))
                    poffset += stdaddrsize;
                else
                {
                    poffset += tp->size;
                }
            }
            else
                poffset += sp1->tp->size;
            if (poffset % stdintsize)
                poffset += stdintsize - poffset % stdintsize;
        }
    }
    sp->paramsize = poffset;
    if (!isinline)
    {
        todo->lst.head = todo->lst.tail = 0;
        for (i = 0; i < nparms; i++)
            insert(spsave[i], &todo->lst);
    }

    sp->recalculateParameters = TRUE;
        if (prm_cplusplus && todo->lst.head == 0)
            todo->lst.head = (SYM*) - 1;
         /* () is equiv to (void) in cpp */
    verify: head = temp;
    tail = temp1;
    headptr = oheadptr;

    if (prm_cplusplus)
    {
        infuncargs--;
        return ;
    }

    if (oldargs && head->lst.head)
    {
        SYM *newargs = head->lst.head;
        while (oldargs && newargs)
        {
            if (oldargs == (SYM*) - 1 || newargs == (SYM*) - 1)
                break;
            if (!exactype(oldargs->tp, newargs->tp, FALSE))
                gensymerror(ERR_DECLMISMATCH, sp2->name);
            oldargs = oldargs->next;
            newargs = newargs->next;
        }
        if (oldargs && oldargs != (SYM*) - 1)
            gensymerror(ERR_ARGLENSHORT, sp->name);
        if (newargs && newargs != (SYM*) - 1)
            gensymerror(ERR_ARGLENLONG, sp->name);
    }
    infuncargs--;
}

//-------------------------------------------------------------------------

void check_funcused(TABLE *oldlsym, TABLE *lsyms)
{
    /* oldlsym Must BE 0 at the end of a function */
    SYM *sym1;
    if (oldlsym && oldlsym->head == lsyms->head)
    {
        return ;
    }
    sym1 = lsyms->head;
    while (sym1 && (!oldlsym || sym1 != oldlsym->head))
    {
        if (sym1->tp->type != bt_func && sym1->tp->type != bt_ifunc && sym1
            ->storage_class != sc_type)
        {
            if (!(sym1->tp->uflags &UF_USED))
            {
                if (sym1->storage_class == sc_label)
                {
                    if (!oldlsym)
                        gensymerror(ERR_UNUSEDLABEL, sym1->name);
                }
                else
                    if (sym1->funcparm)
                        gensymerror(ERR_PARAMUNUSED, sym1->name);
                    else
                        gensymerror(ERR_SYMUNUSED, sym1->name);
            }
            if (sym1->tp->uflags &UF_ASSIGNED)
                gensymerror(ERR_SYMASSIGNED, sym1->name);
            if (!oldlsym && sym1->storage_class == sc_ulabel)
                gensymerror(ERR_UNDEFLABEL, sym1->name);
        }
        sym1 = sym1->next;
    }
}

//-------------------------------------------------------------------------

void dump_instantiates(void)
{
    SYM *oldfunc = currentfunc;
    LIST *oldvar = varlisthead;
    while (instantiated_inlines)
    {
        currentfunc = instantiated_inlines->data;
        instantiated_inlines = instantiated_inlines->link;
        if (!currentfunc->value.classdata.inlinefunc) {
            char *nm = currentfunc->name;
            currentfunc = 0 ;
            gensymerror(ERR_INLINENOBODY,nm);
        } else {
            conscount = 0;
            varlisthead = currentfunc->value.classdata.inlinefunc->syms;
        
            genfunc(currentfunc->value.classdata.inlinefunc->stmt);
        }
        //		release_local() ;
    }
    varlisthead = oldvar;
    currentfunc = oldfunc;
}

//-------------------------------------------------------------------------

void funcbody(SYM *sp)
/*
 */
{
    SYM *sp1,  *sp2;
    int xglob = global_flag;
    SNODE *vlaparminit = 0;
	if (currentfunc)
	{
		generror(ERR_NONESTEDFUNC,0,0);
		return;
	}
	setjmp_used = FALSE;
    if (prm_debug)
        debug_outfunc(sp);
    funcnesting++;
    //				instantiated_inlines = 0 ;
	labelsyms.head = labelsyms.tail = NULL;
    goodcode = 0;
	switchbreak = 0;
    typequal = 0;
    conscount = 0;
    try_block_list = 0;
	allocaSP = 0;
	asmMask = asmRMask = 0;
    /* 1 if inline, 0 if local define */
    global_flag = !!(sp->value.classdata.cppflags &PF_INLINE);
    /* Have to defer old style argument gathering until now
     * because of the pointer-to func syntax
     */
    if (sp->oldstyle)
    {
        SYM *spsave[50],  *sp1;
        int nparms, i;
        int poffset = stdretblocksize; /* size of return block */
        char **names;
        global_flag++;
        doargdecl(sc_member, 0, 0, &sp->tp->lst, FALSE); /* declare parameters
            */
        global_flag--;
        if (!ispascal && isstructured(sp->tp->btp))
        {
            poffset += stdaddrsize;
            sp->calleenearret = TRUE;
        }
        if (sp->farproc)
            poffset += stdintsize;
        nparms = (int)sp->oldstyle[0];
        names = &sp->oldstyle[1];
        for (i = 0; i < nparms; ++i)
        {
            if ((sp1 = search(names[i], &sp->tp->lst)) == 0)
                sp1 = makeint(litlate(names[i]), &sp->tp->lst);
            if (sp1->tp)
                sp1->tp->uflags |= UF_DEFINED;
            sp1->funcparm = TRUE;
            spsave[i] = sp1;
            sp1->storage_class = sc_auto;
        }
        /*
         * parameter allocation.  Have to do things backwards if this
         * function has the pascal declarator
         */
        for (i = 0; i < nparms; ++i)
        {
            sp1 = spsave[i];
            if (sp1->tp->type != bt_pointer && sp1->tp->type != bt_farpointer
                && sp1->tp->type != bt_segpointer && sp1->tp->size <= stdintsize)
            {
                sp1->value.i = poffset + funcvaluesize(sp1->tp->size);
                poffset += stdintsize;
            }
            else
            {
                sp1->value.i = poffset;
                if (sp1->tp->type == bt_pointer)
                    poffset += stdaddrsize;
                else
                {
                    poffset += sp1->tp->size;
                }
                if (poffset % stdintsize)
                    poffset += stdintsize - poffset % stdintsize;
            }
        }
        sp->paramsize = poffset;
    }
    else
    {
        if (sp->tp->type == bt_func || sp->tp->type == bt_ifunc)
            if (!sp->pascaldefn && (sp->value.classdata.cppflags &PF_MEMBER) &&
                !(sp->value.classdata.cppflags &PF_STATIC))
            {
                SYM *s1 = sp->tp->lst.head;
                while (s1 && s1 != (SYM*) - 1)
                {
                    s1->value.i += stdaddrsize;
                    s1 = s1->next;
                }
            }
    }
    sp1 = sp->tp->lst.head;
    if (sp1 != (SYM*) - 1)
    {
        SNODE *vlaparmtail;
        while (sp1)
        {
            if (sp1->name[0] != '*' || sp1->tp->type == bt_ellipse)
            {
                TYP *tp;
                sp2 = copysym(sp1);
                insert(sp2, &lsyms);
                tp = sp2->tp;
                if (tp->type == bt_pointer && !(tp->val_flag &VARARRAY))
                    tp = tp->btp;
                if (tp->val_flag &VARARRAY)
                {
                    SNODE *snp = xalloc(sizeof(SNODE));
                    snp->next = 0;
                    snp->stype = st_expr;
                    snp->exp = createVarArray(sp2, tp, FALSE, FALSE);
                    snp->flags |= ST_VARARRAY ;
                    if (vlaparminit == 0)
                        vlaparminit = vlaparmtail = snp;
                    else
                    {
                        vlaparmtail->next = snp;
                        vlaparmtail = snp;
                    }
                }
                browse_variable(sp2, lineno);
            }
            else
                if (!prm_cplusplus)
                    generror(ERR_PARAMMISSINGNAME, 0, 0);

            sp1 = sp1->next;
        }
    }
    currentfunc = sp;
    firstlabel = nextlabel;
    goodcode &= ~GF_RETURN;
    block(vlaparminit);
    check_funcused(0, &lsyms);
    if (prm_listfile && varlisthead)
    {
        LIST *q = varlisthead;
        fprintf(listFile, "\n\n*** local symbol table ***\n\n");
        while (q)
        {
            SYM *sp = q->data;
            while (sp)
            {
                list_var(sp, 0);
                sp = sp->next;
            }
            q = q->link;
        }
        list_table(&ltags,0);
    }
    if (sp->tp->btp->type != bt_void && !(goodcode &(GF_RETURN | GF_UNREACH)))
        if (!prm_cplusplus || strcmp(currentfunc->name + prm_cmangle, "main"))
            generror(ERR_FUNCRETVAL, 0, 0);
    nl();

    local_using_list = 0;
    local_tag_using_list = 0;
    #ifdef ICODE
        dag_rundown();
    #endif 
    lsyms.head = 0;
    ltags.head = 0;
    oldlsym.head = oldlsym.tail = 0;
    lastfunc = currentfunc;

    //        dump_instantiates() ;
    while (lastst == semicolon)
        getsym();
    varlisthead = varlisttail = 0;
    if (!--funcnesting)
    {
        release_local(); /* release local symbols */
        release_opt();
        release_oc();
    }
    currentfunc = 0;
    global_flag = xglob;
}

//-------------------------------------------------------------------------

SYM *makeint(char *name, TABLE *table)
{
    SYM *sp;
    TYP *tp;
    global_flag++;
    sp = makesym(sc_auto);
    tp = xalloc(sizeof(TYP));
    global_flag--;
    tp->type = bt_int;
    tp->size = stdintsize;
    tp->btp = tp->lst.head = 0;
    tp->sp = 0;
    sp->name = name;
    tp->bits = tp->startbit =  - 1;
    sp->tp = tp;
    insert(sp, table);
    return sp;
}

/* a little ditty to add C++ destructors on the tail of a block
 */
void addrundown(SNODE *snp)
{
    if (block_rundown && snp)
    {
        SNODE *snp2;
        snp2 = xalloc(sizeof(SNODE));
        snp2->next = 0;
        snp2->stype = st_expr;
        snp2->exp = block_rundown;
        block_rundown = 0;
        while (snp->next)
            snp = snp->next;
        snp->next = snp2;
    }
}

//-------------------------------------------------------------------------

void addblocklist(SYM *sp, SYM *old)
{
    LIST *l;
    if (!sp)
        return ;
    if (!currentfunc)
        return ;
    if (varlisthead && sp == varlisttail->data)
        return ;
    if (sp == old)
        return ;
    if (currentfunc->value.classdata.cppflags &PF_INLINE)
    {
        global_flag++;
        l = xalloc(sizeof(LIST));
        global_flag--;
    }
    else
        l = xalloc(sizeof(LIST));
    l->link = 0;
    l->data = sp;
    if (varlisthead)
        varlisttail = varlisttail->link = l;
    else
        varlisthead = varlisttail = l;
    if (old)
    while (sp)
    {
        if (sp->next == old)
        {
            sp->next = 0;
            break;
        }
        sp = sp->next;
    }
}

//-------------------------------------------------------------------------
int scanforgoto(SNODE *block, SNODE *root, DLIST **data, int id)
/*
 *  scan a statement tree for gotos.  We are going to put out an error if
 *  a goto tries to bypass a VLA initialization
 */
{
    static DLIST *labelList;
    static SNODE *gotoPos ;
    int rv = TRUE ;
    DLIST *m = xalloc(sizeof(DLIST)) ;
    m->fwd = *data ;
    m->back = 0 ;
    if (*data)
        (*data)->back = m ;
    *data = m ;
    m->data = block ;
    while (block != 0 && rv)
    {
        m->current = block ;
        switch (block->stype)
        {
            case st_goto:
                if (id == -1) 
                {
                    labelList = 0 ;
                    gotoPos = block ;
                    scanforgoto(root,root,&labelList,(int)block->label) ;
                }
                break ;
            case st_label:
                if (id != -1 && id == (int)block->label) 
                {
                    int toerr = FALSE;
                    SNODE *s = 0;
                    DLIST *l = labelList, *g = gotoList;
                    while (l && l->fwd)
                        l = l->fwd ;
                    while (g && g->fwd)
                        g = g->fwd ;
                    while (l && g && l->data == g->data) 
                    {
                        l = l->back ;
                        g = g->back ;
                    }
                    if (!l && !g) 
                    { /* both in same block */
                        s = gotoPos;
                        while (s && s != block)
                        {
                            if (s->flags & ST_VARARRAY)
                                toerr = TRUE;
                            s = s->next;
                        }
                    }
                    else if (l)
                    { /* label in lower block, or two in disjoint blocks */
                        s = labelList->data;
                        while (s && s != block)
                        {
                            if (s->flags & ST_VARARRAY)
                                toerr = TRUE;
                            s = s->next;
                        }
                    }
                    else if (g)
                    { /* goto in lower block */
                        s = g->fwd->current;
                        while (s && s != block)
                        {
                            if (s->flags & ST_VARARRAY)
                                toerr = TRUE;
                            s = s->next;
                        }
                    }
                    if (s && toerr)
                        generror(ERR_VLAGOTO,0,0);
                    rv = FALSE ;
                }
                break ;
            case st_tryblock:
                break;
            case st_throw:
                break;
            case st_return:
            case st_expr:
                break;
            case st_while:
            case st_do:
                rv &= scanforgoto(block->s1,root,data,id);
                break;
            case st_for:
                rv &= scanforgoto(block->s1,root,data,id);
                break;
            case st_if:
                rv &= scanforgoto(block->s1,root,data,id);
                if (rv)
                    rv &= scanforgoto(block->s2,root,data,id);
                break;
            case st_switch:
                rv &= scanforgoto(block->s1,root,data,id);
                break;
            case st_case:
                rv &= scanforgoto(block->s1,root,data,id);
                break;
            case st_block:
                rv &= scanforgoto(block->exp,root,data,id);
                break;
            case st_asm:
                break;
        }
        block = block->next;
    }
    (*data) = (*data)->fwd ;
    if (*data)
        (*data)->back = 0 ;
    return rv ;
}

void block(SNODE *vlainit)
{
    SNODE *snp3 = NULL;
    SNODE *funcsym = funcstmt; // in case they declare function ptrs
    int checkthunkret = mainfunc && (prm_cplusplus || prm_c99); 
        // && prm_cplusplus ; /* blockdecl resets mainfunc */
    int oldpragma = stdpragmas;
	TYP *tp = maketype(bt_pointer, stdaddrsize);
#ifdef i386
	stackframeoff = noStackFrame;
	noStackFrame = FALSE;
#endif
	allocaSP = dummyvar(stdaddrsize, &stdint, 0);
	deref(&allocaSP, tp);
    block_rundown = 0;
    block_nesting = 1;
	blockVarArraySP = 0;
    arg_nesting = 0;
    lastst = 0;
    cbautoinithead = 0;
    gotoCount = 0 ;
    if (vlainit)
    {
        snp3 = vlainit;
        while (vlainit->next)
            vlainit = vlainit->next;
        funcbrstmt = snp_line(TRUE);
    }
    else
        funcbrstmt = snp_line(TRUE);
    lastst = begin;
    needpunc(begin, 0);
    if (prm_debug)
        debug_beginblock(lsyms);
    browse_blockstart(lineno);
    if (prm_cplusplus && (currentfunc->value.classdata.cppflags &PF_CONSTRUCTOR)
        )
    {
        SNODE *snp4 = xalloc(sizeof(SNODE));
        snp4->stype = st_expr;
        snp4->exp = 0;
        // thunk flags reset in decl.c before loading initializers
        thunkConstructors(snp4, currentfunc->parentclass, 0, FALSE);
        RefErrs(currentfunc->parentclass);
        if (snp3)
        {
            snp3->next = snp4;
        }
        else
            snp3 = snp4;
    }
    blockdecl();

    if (snp3)
    {
        SNODE *snp5 = snp3;
        while (snp5->next)
            snp5 = snp5->next;
        snp5->next = compound();
    }
    else
        snp3 = compound();

	if (!used_alloca) 
	{
		allocaSP->v.p[0]->v.sp->value.i = -2; // won't be allocated
		allocaSP->v.p[0]->v.sp->dontlist = TRUE;
	}
	if (blockVarArraySP)
	{
		SNODE *snp2 = xalloc(sizeof(SNODE)), *snp;
        snp2->stype = st_expr;
        snp2->exp = makenode(en_savestack, blockVarArraySP, 0);
		snp2->next = snp3;
		snp3 = snp = snp2;
		while (snp->next && snp->next->stype != st_return)
			snp = snp->next;
		snp2 = xalloc(sizeof(SNODE));
        snp2->stype = st_expr;
        snp2->exp = makenode(en_loadstack, blockVarArraySP, 0);
		snp2->next = snp->next;
		snp->next = snp2 ;
	}
    /* Thunk in a return value of zero if this is the main func
     * and there was no return statement
     */
    if (checkthunkret)
    {
        SNODE *snp4 = snp3;
		if (snp4)
		{
	        while (snp4->next)
    	        snp4 = snp4->next;
        	if (snp4->stype != st_return)
        	{
            	snp4->next = xalloc(sizeof(SNODE));
		    	snp4 = snp4->next;
		    	snp4->stype = st_return;
    			snp4->exp = makeintnode(en_icon, 0);
			}
        }
		else
		{
			snp3 = snp4 = xalloc(sizeof(SNODE));
	    	snp4->stype = st_return;
    		snp4->exp = makeintnode(en_icon, 0);
		}
    }
    if (prm_c99 && gotoCount) {
        gotoList = 0 ;
        scanforgoto(snp3,snp3,&gotoList,-1) ;
    }
    parameter_rundown(&lsyms);
    /* create destructor call tree for end of function */
    if (prm_cplusplus && (currentfunc->value.classdata.cppflags &PF_DESTRUCTOR))
    {
        SNODE snp4;
        snp4.stype = st_expr;
        snp4.exp = 0;
        setthunkflags(currentfunc->parentclass, FALSE);
        thunkDestructors(&snp4, currentfunc->parentclass, 0, FALSE, TRUE);
        if (block_rundown)
            block_rundown = makenode(en_void, (void*)block_rundown, (void*)
                snp4.exp);
        else
            block_rundown = snp4.exp;
    }
    currentfunc->value.classdata.eofdest = block_rundown;
    block_rundown = 0;
    /*            addrundown(snp3); */
    addblocklist(lsyms.head, 0); // Add block level declarations
    if (!total_errors)
    {
        /* this unqualified the current function if it has structured
         * args or return value, or if it has nested declarations
         */
        if (currentfunc->value.classdata.cppflags &PF_INLINE)
        {
            if (isstructured(currentfunc->tp->btp))
                currentfunc->value.classdata.cppflags &= ~PF_INLINE;
            else
            {
                SYM *head = currentfunc->tp->lst.head;
                if (head != (SYM*) - 1)
                while (head)
                {
                    if (isstructured(head->tp))
                    {
                        currentfunc->value.classdata.cppflags &= ~PF_INLINE;
                        break;
                    }
                    head = head->next;
                }
                if (currentfunc->value.classdata.cppflags &PF_INLINE)
                {
                    if (arg_nesting)
                        currentfunc->value.classdata.cppflags &= ~PF_INLINE;
                }
            }
        }
        if (currentfunc->value.classdata.cppflags &PF_INLINE)
        {
            currentfunc->mainsym->value.classdata.inlinefunc = xalloc(sizeof
                (INLINEFUNC));
            currentfunc->mainsym->value.classdata.inlinefunc->stmt = snp3;
            currentfunc->mainsym->value.classdata.inlinefunc->syms =
                varlisthead;
            if (currentfunc->value.classdata.eofdest)
            {
                SNODE *snp4 = xalloc(sizeof(SNODE));
                snp4->stype = st_expr;
                snp4->exp = currentfunc->value.classdata.eofdest;
                while (snp3 && snp3->next)
                    snp3 = snp3->next;
                if (snp3)
                    snp3->next = snp4;
            }

            funcstmt = funcsym;
        }
        else
        {
            funcstmt = funcsym;
            genfunc(snp3);
        }
    }
        else
            currentfunc->value.classdata.cppflags &= ~PF_INLINE;
#ifdef i386
	stackframeoff = FALSE;
#endif
    if (prm_debug)
        debug_endblock(lsyms,oldlsym);
    browse_blockend(lineno);
    block_nesting = 0;
    block_rundown = 0;
    funcstmt = funcendstmt = 0;
    stdpragmas = oldpragma;
}

//-------------------------------------------------------------------------

void gather_labels(TABLE *oldlsym, TABLE *lsyms)
{
    TABLE sym;
    sym.head = 0;
    if (oldlsym->head == lsyms->head)
    {
        return ;
    }
    else
    {
        SYM *sp = lsyms->head,  *r = oldlsym->head;
        lsyms->head = 0;
        while (sp && sp != r)
        {
            SYM *q = sp->next;
            sp->next = 0;
            if (sp->storage_class == sc_label || sp->storage_class == sc_ulabel)
            {
                if (!oldlsym->head)
                {
                    oldlsym->head = oldlsym->tail = sp;
                }
                else
                {
                    oldlsym->tail->next = sp;
                    oldlsym->tail = sp;
                }
            }
            else
                if (!lsyms->head)
                    lsyms->head = lsyms->tail = sp;
                else
                    lsyms->tail = lsyms->tail->next = sp;
            sp = q;
        }
        if (oldlsym->head)
            oldlsym->tail->next = 0;
    }
}

//-------------------------------------------------------------------------

SNODE *compoundblock(void)
{
    SNODE *snp;
	ENODE *oldblockva;
    TABLE oldoldlsym, oldoldltag;
    SYM *q;
    LIST *nsusing = local_using_list;
    LIST *nstusing = local_tag_using_list;
    LASTDEST rundown;
    int oldpragma = stdpragmas;
    rundown.chain = rundown.last = lastDestPointer;
    rundown.block = block_rundown;
	oldblockva = blockVarArraySP;
	blockVarArraySP = 0;
    lastDestPointer = &rundown;
    block_rundown = 0;
    block_nesting++;
    oldoldltag = oldltag;
    oldltag = ltags;
    oldoldlsym = oldlsym;
    oldlsym = lsyms;
    if (prm_debug)
        debug_beginblock(lsyms);
    browse_blockstart(lineno);
    cbautoinithead = 0;
    blockdecl();
    snp = compound();
    addrundown(snp);
    check_funcused(&oldlsym, &lsyms);
    q = lsyms.head;
    gather_labels(&oldlsym, &lsyms);
    while (q && q->next != oldlsym.head)
        q = q->next;
    if (q)
        q->next = 0;
    addblocklist(lsyms.head, oldlsym.head);
    if (oldlsym.head != lsyms.head)
        arg_nesting++;
    if (prm_debug)
        debug_endblock(lsyms, oldlsym);
    browse_blockend(lineno);
    stdpragmas = oldpragma;
    lsyms = oldlsym;
    oldlsym.head = oldoldlsym.head;
    ltags = oldltag;
    oldltag.head = oldoldltag.head;
    local_using_list = nsusing;
    local_tag_using_list = nstusing;
    block_nesting--;
    block_rundown = rundown.block;
    lastDestPointer = rundown.last;
	if (blockVarArraySP)
	{
		SNODE *snp2 = xalloc(sizeof(SNODE)), *snp3;
        snp2->stype = st_expr;
        snp2->exp = makenode(en_savestack, blockVarArraySP, 0);
		snp2->next = snp;
		snp3 = snp = snp2;
		while (snp3->next)
			snp3 = snp3->next;
		snp2 = xalloc(sizeof(SNODE));
        snp2->stype = st_expr;
        snp2->exp = makenode(en_loadstack, blockVarArraySP, 0);
		snp3->next = snp2 ;
	}
	blockVarArraySP = oldblockva;
    return (snp);
}
