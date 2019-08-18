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
/*
 * handles initialization of module-scoped declarations
 */
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "ccerr.h"
#include "lists.h"
#include "diag.h"

extern int prm_c99;
extern int prm_asmfile;
extern int structlevel;
extern TYP *head,  **headptr;
extern struct template  *currentTemplate;
extern SNODE *funcstmt;
extern int stdaddrsize;
extern int stdldoublesize;
extern enum e_sym lastst;
extern char lastid[], laststr[];
extern int skm_declend[];
extern long nextlabel;
extern TYP stdmatch;
extern int skm_declclosepa[];
extern int skm_closebr[];
extern int prm_cplusplus, prm_debug;
extern SYM *currentfunc;
extern int prm_bss;
extern int laststrlen;
extern LIST *varlisthead,  *varlisttail;
extern int stdmemberptrsize;
extern SYM *typequal;
extern int total_errors;
extern char *cpp_funcname_tab[];
extern int conscount;
extern TRYBLOCK *try_block_list;
extern LIST *localfuncs;
extern SYM *parm_namespaces[20][100];
extern int parm_ns_counts[20];
extern int parm_ns_level;
extern int parsing_params;

void insertSpace(SYM *sp, int *bytes);

int virtualdata;

static SYM *locsp;
static short startbit, bits, curbitsize;
static LLONG_TYPE totbits;
extern int stdinttype, stdunstype, stdintsize, stdldoublesize, stdaddrsize;
extern int stdshortsize, stdlongsize, stdlonglongsize, stdfloatsize, stddoublesize, stdenumsize;
static int allocated;
static ENODE *cpprefhead,  *cpprefrundown;
static ENODE **cppreftail;
int bssbytes, databytes, constbytes;

static struct decldata **sptr;
static int begin_nesting;

static struct declares
{
    struct declares *link;
    SYM *sp;
}  *declarations,  *decltail,  *virtualdecl,  *virtualdecltail;

static long gen(SYM *sp);
static long gen2(SYM *sp);
int skm_semicolon[] = 
{
    semicolon, comma, 0
};

void initini(void)
{
    cpprefhead = 0;
    cpprefrundown = 0;
    declarations = decltail = 0;
    virtualdecl = virtualdecltail = 0;
    databytes = bssbytes = constbytes = 0;
}

//-------------------------------------------------------------------------

void initrundown(void)
/*
 * Dump out C++ pointer and reference initializations functions 
 *
 * also dump out local vars
 *
 */
{
    int first, lbl;
    struct declares *q;
    long nbytes, obytes, val, onbytes, constnbytes;
    SNODE stmt;
        if (cpprefhead && !total_errors)
        {
            lbl = nextlabel++;
            varlisthead = varlisttail = 0;
            currentfunc = makesym(sc_static);
            currentfunc->tp = maketype(bt_func, 0);
            currentfunc->tp->btp = maketype(bt_void, 0);
            currentfunc->tp->lst.head = 0;
            currentfunc->intflag = 0;
            currentfunc->faultflag = 0;
            currentfunc->name = "__CPP_INIT_FUNC__";
            memset(&stmt, 0, sizeof(stmt));
            stmt.stype = st_expr;
            stmt.next = 0;
            stmt.exp = cpprefhead;
            conscount = 0;
            try_block_list = 0;
            cseg();
            genfunc(&stmt);
            #ifdef ICODE
                rewrite_icode();
            #else 
                flush_peep();
            #endif 
            startupseg();
            gensrref(currentfunc, 32);
            //      genlong(20) ;
            currentfunc = 0;
            cpprefhead = 0;
        } if (cpprefrundown && !total_errors)
        {
            lbl = nextlabel++;
            varlisthead = varlisttail = 0;
            currentfunc = makesym(sc_static);
            currentfunc->tp = maketype(bt_func, 0);
            currentfunc->tp->btp = maketype(bt_void, 0);
            currentfunc->tp->lst.head = 0;
            currentfunc->intflag = 0;
            currentfunc->faultflag = 0;
            currentfunc->name = "__CPP_RUNDOWN_FUNC__";
            memset(&stmt, 0, sizeof(stmt));
            stmt.stype = st_expr;
            stmt.next = 0;
            stmt.exp = cpprefrundown;
            conscount = 0;
            try_block_list = 0;
            cseg();

            genfunc(&stmt);
            #ifdef ICODE
                rewrite_icode();
            #else 
                flush_peep();
            #endif 
            rundownseg();
            gensrref(currentfunc, 32);
            //      genlong(100) ;
            currentfunc = 0;
            cpprefrundown = 0;
        }
    q = virtualdecl;
    first = TRUE;
    while (q)
    {
        SYM *sp = q->sp;
        SetExtFlag(sp, FALSE);
        if (first)
        {
            nl();
        }
        dseg();
        first = FALSE;
        gen_virtual(sp, TRUE);
        gen2(sp);
        gen_endvirtual(sp);
        //         if (prm_debug && !sp->staticlabel)
        //            debug_outdata(sp,FALSE) ;
        sp->offset = 0;
        q = q->link;
    }
    q = declarations;
    nbytes = 0;
    constnbytes = 0;
    first = TRUE;
    while (q)
    {
        SYM *sp = q->sp;
        if (first)
        {
            nl();
        }
        first = FALSE;
        if (sp && (sp->tp->size || !sp->tp->val_flag) && (sp->init || !prm_bss) && !sp->absflag)
        {
            int align = alignment(sc_global, sp->tp);
            if ((sp->tp->cflags &DF_CONST) && !(sp->tp->cflags &DF_VOL))
                xconstseg();
            else
                dseg();
            SetExtFlag(sp, FALSE);
            if ((sp->tp->cflags &DF_CONST) && !(sp->tp->cflags &DF_VOL))
                val = align - (constnbytes % align);
            else
                val = align - (nbytes % align);
            if (val != align)
            {
                genstorage(val);
                if ((sp->tp->cflags &DF_CONST) && !(sp->tp->cflags &DF_VOL))
                    constnbytes += val;
                else
                    nbytes += val;
                nl();
            }
            if ((sp->tp->cflags &DF_CONST) && !(sp->tp->cflags &DF_VOL))
            {
                onbytes = constnbytes;
                constnbytes += gen(sp);
            }
            else
            {
                onbytes = nbytes;
                nbytes += gen(sp);
            }
            sp->offset = onbytes;
            if (prm_debug && !sp->staticlabel)
                debug_outdata(sp, FALSE);
#if 0
            if (sp->offset != onbytes)
            {
                char buf[256];
                sprintf(buf, "initrundown: data position mismatch %s", sp->name)
                    ;
                ; /* + obytes */
                DIAG(buf);
            }
            sp->offset = onbytes;
#endif
        }
        if (sp && !sp->tp->size && sp->tp->val_flag) {
            TYP *tp = sp->tp, *tp1 = tp ;
            while (tp->val_flag && !tp->size && tp->type == bt_pointer)
                tp = tp->btp ;
            while (tp1 && tp1 != tp) {
                tp1->size = tp->size ;
                tp1 = tp1->btp;
            }
            if (sp->tp->size == 0 || !prm_c99)
                gensymerror(ERR_ZEROSTORAGE, sp->name);
            else {
                int *bytes;
                sp->indecltable = FALSE ;
                insert_decl_sp(sp) ; // relink it
                if ((sp->tp->cflags &DF_CONST) && !(sp->tp->cflags &DF_VOL))
                    bytes = &constbytes;
                else if ((sp->init || !prm_bss) && !sp->absflag)
                    bytes = &databytes;
                else
                    if (!sp->init && !sp->absflag)
                        bytes = &bssbytes;
                sp->offset = *bytes ;
                insertSpace(sp, bytes);
                q->sp = 0 ;
            }
        }
        q = q->link;
    }
    first = TRUE;
    if (prm_bss)
    {
        /*		obytes  = nbytes; */
        nbytes = 0;
//        constnbytes = 0;
        q = declarations;
        while (q)
        {
            SYM *sp = q->sp;
            if (first)
            {
                nl();
            }
            first = FALSE;
            if (sp && (sp->tp->size || !sp->tp->val_flag) && !sp->init && !sp->absflag)
            {
                int align = alignment(sc_global, sp->tp);
                SetExtFlag(sp, FALSE);
                if ((sp->tp->cflags &DF_CONST) && !(sp->tp->cflags &DF_VOL))
                    xconstseg();
                else
                    bssseg();
                if ((sp->tp->cflags &DF_CONST) && !(sp->tp->cflags &DF_VOL))
                    val = align - (constnbytes % align);
                else
                    val = align - (nbytes % align);
                if (val != align)
                {
                    genstorage(val);
                    if ((sp->tp->cflags &DF_CONST) && !(sp->tp->cflags &DF_VOL))
                        constnbytes += val;
                    else
                        nbytes += val;
                    nl();
                }
                if ((sp->tp->cflags &DF_CONST) && !(sp->tp->cflags &DF_VOL))
                {
                    onbytes = constnbytes;
                    constnbytes += gen(sp);
                }
                else
                {
                    onbytes = nbytes;
                    nbytes += gen(sp);
                }
                sp->offset = onbytes;
                if (prm_debug && !sp->staticlabel)
                    debug_outdata(sp, TRUE);
#if 0
                if (sp->offset != onbytes)
                {
                    char buf[256];
                    sprintf(buf, "initrundown: bss position mismatch %s", sp
                        ->name);
                    ; /* + obytes */
                    DIAG(buf);
                }
#endif
            }
            if (sp && !sp->tp->size && sp->tp->val_flag) {
                TYP *tp = sp->tp, *tp1 = tp ;
                while (tp->val_flag && !tp->size && tp->type == bt_pointer)
                    tp = tp->btp ;
                while (tp1 && tp1 != tp) {
                    tp1->size = tp->size ;
                    tp1 = tp1->btp;
                }
                if (sp->tp->size == 0 || !prm_c99)
                    gensymerror(ERR_ZEROSTORAGE, sp->name);
                else {
                    int *bytes;
                    sp->indecltable = FALSE ;
                    insert_decl_sp(sp) ; // relink it
                    if ((sp->tp->cflags &DF_CONST) && !(sp->tp->cflags &DF_VOL))
                        bytes = &constbytes;
                    else if ((sp->init || !prm_bss) && !sp->absflag)
                        bytes = &databytes;
                    else
                        if (!sp->init && !sp->absflag)
                            bytes = &bssbytes;
                    sp->offset = *bytes ;
                    insertSpace(sp, bytes);
                    q->sp = 0 ;
                }
            }
            q = q->link;
        }
    }
}

//-------------------------------------------------------------------------

static long gen2(SYM *sp)
{
    struct decldata *decldata = sp->init;
    long nbytes = 0;
    if (decldata)
    {
        while (decldata)
        {
            switch (decldata->mode)
            {
				case dd_bool:
					genbool(decldata->val.i);
					nbytes ++;
					break;
                case dd_byte:
                    genbyte(decldata->val.i);
                    nbytes++;
                    ;
                    break;
                case dd_word:
                    genword(decldata->val.i);
                    nbytes += stdshortsize;
                    break;
                case dd_enum:
                    genenum(decldata->val.i);
                    nbytes += stdenumsize;
                    break;
                case dd_int:
                    genint(decldata->val.i);
                    nbytes += stdintsize;
                    break;
                case dd_long:
                    genlong(decldata->val.i);
                    nbytes += stdlongsize;
                    break;
                case dd_longlong:
                    genlonglong(decldata->val.i);
                    nbytes += stdlonglongsize;
                    break;
                case dd_float:
                    genfloat(&decldata->val.f[0]);
                    nbytes += stdfloatsize;
                    break;
                case dd_double:
                    gendouble(&decldata->val.f[0]);
                    nbytes += stddoublesize;
                    break;
                case dd_ldouble:
                    genlongdouble(&decldata->val.f[0]);
                    nbytes += stdldoublesize;
                    break;
                case dd_fcomplex:
                    genfloat(&decldata->val.f[0]);
                    genfloat(&decldata->val.f[1]);
                    nbytes += stdfloatsize * 2;
                    break;
                case dd_rcomplex:
                    gendouble(&decldata->val.f[0]);
                    gendouble(&decldata->val.f[1]);
                    nbytes += stddoublesize * 2;
                    break;
                case dd_lrcomplex:
                    genlongdouble(&decldata->val.f[0]);
                    genlongdouble(&decldata->val.f[1]);
                    nbytes += stdldoublesize * 2;
                    break;
                case dd_pcname:
                    genpcref(decldata->val.sp, decldata->offset);
                    nbytes += stdaddrsize;
                    break;
                case dd_dataname:
                    genref(decldata->val.sp, decldata->offset);
                    nbytes += stdaddrsize;
                    break;
                case dd_storage:
                    genstorage(decldata->val.i);
                    nbytes += decldata->val.i;
                    break;
                case dd_label:
                    gen_labref(decldata->val.i);
                    nbytes += stdaddrsize;
                    break;
                case dd_farpointer:
                    genlong(decldata->val.i &0xffffffff);
                    #if sizeof(ULLONG_TYPE) == 4
                        genword(0);
                    #else 
                        genword(decldata->val.i >> 32);
                    #endif 
                    nbytes += stdaddrsize + stdshortsize;
                    break;
            }
            decldata = decldata->link;
        }
    }
    else
    {
        if (sp->tp->size == 0) 
            gensymerror(ERR_ZEROSTORAGE, sp->name);
        else
            genstorage(nbytes += sp->tp->size);
    }
    nl();
    return nbytes;
}

//-------------------------------------------------------------------------

static long gen(SYM *sp)
{
    if (sp->storage_class == sc_global)
    {
        globaldef(sp);
    }
    if (sp->staticlabel)
        put_staticlabel(sp->value.i);
    else
        gen_strlab(sp);
    return gen2(sp);
}

//-------------------------------------------------------------------------

    void cppinitinsert(ENODE *node)
    /*
     * insert an initialization function on the list
     */
    {
        if (!cpprefhead)
        {
            cpprefhead = node;
            cppreftail = &cpprefhead;
        }
        else
        {
            *cppreftail = makenode(en_void,  *cppreftail, node);
            cppreftail = &(*cppreftail)->v.p[1];
        }
    }
    void cpprundowninsert(ENODE *node)
    {
        if (cpprefrundown)
            cpprefrundown = makenode(en_void, node, cpprefrundown);
        else
            cpprefrundown = node;
    }
    void cppinitassign(ENODE *node)
    {
        ENODE *node1 = makenode(en_a_ref, makenode(en_nacon, locsp, 0), 0);
        if (node->nodetype == en_void || node->nodetype == en_dvoid)
        {
            ENODE **n2 = actualexpr(&node);
            if (n2)
                (*n2)->v.p[0] = makenode(en_assign, node1, (*n2)->v.p[0]);
            else
            {
                DIAG("cppinitassign: can't find actual node");
            }
            node1 = node;
        }
        else
        {
            node1 = makenode(en_assign, node1, node);
        }
        cppinitinsert(node1);
    }
void insert_decl_sp(SYM *sp)
{
    struct declares *p;
    SYM *spx = sp->parentclass;
    if (sp->indecltable)
    {
        return ;
    } 
    while (spx)
    {
        if (spx->istemplate)
            return ;
        spx = spx->parentclass;
    }
    p = xalloc(sizeof(struct declares));
    p->sp = sp;
    p->link = 0;
    if (virtualdata)
    {
        if (virtualdecl)
            virtualdecltail = virtualdecltail->link = p;
        else
            virtualdecl = virtualdecltail = p;
    } 
    else if (declarations)
        decltail = decltail->link = p;
    else
        decltail = declarations = p;
    sp->indecltable = TRUE;
    if (sp->parentclass)
    {
        LIST *l = xalloc(sizeof(LIST));
        l->data = sp;
        l->link = localfuncs;
        localfuncs = l;
    }

}

//-------------------------------------------------------------------------

void ReshuffleDeclsp(SYM *sp)
{
    struct declares **p = &declarations;
    while (*p)
        if ((*p)->sp == sp)
            break;
        else
            p = &(*p)->link;
    if (*p)
    {
        struct declares *r =  *p;
        if (decltail == r)
            return ;
        *p = (*p)->link;
        r->link = 0;
        decltail = decltail->link = r;
    } 
}

//-------------------------------------------------------------------------

void insertSpace(SYM *sp, int *bytes)
{
    if (bytes)
    {
        int align = alignment(sc_global, sp->tp);
        int val;
        if (!align)
            align = 1;
        val = align - (*bytes % align);
        if (val != align)
        {
            *bytes += val;
        }
        sp->offset =  *bytes;
        *bytes += sp->tp->size;
    }
}

//-------------------------------------------------------------------------

int doinit(SYM *sp)
/*
 * Handle static variable initialize
 */
{
    int nbytes = 0,  *bytes = 0;
    int alreadyassigned = sp->indecltable, alreadyinitted = sp->init != 0;
    int reshuffle;
    begin_nesting = 0;
        if (currentTemplate)
        {
            expskim(skm_semicolon, FALSE);
            endinit();
            return sp->tp->size;
        }
        if (sp->value.classdata.gdeclare)
            sp = sp->value.classdata.gdeclare;
    if (currentfunc && (currentfunc->value.classdata.cppflags &PF_INLINE))
        if (!(sp->tp->cflags &DF_CONST) || (sp->tp->cflags &DF_VOL))
            if (prm_cplusplus)
                currentfunc->value.classdata.cppflags &= ~PF_INLINE;
            else
                generror(ERR_INLINENOSTATIC, 0, 0);

    typequal = 0;
    sp->tp->uflags |= UF_DEFINED;
    allocated = FALSE;
    totbits = 0;
    curbitsize =  - 1;
    bits =  - 1;
    locsp = sp;
    sptr = &sp->init;
    reshuffle = sp->indecltable && !sp->init;
//    if (!(sp->tp->cflags &DF_CONST) || (sp->tp->cflags &DF_VOL) ||
//        !scalarnonfloat(sp->tp) || sp->storage_class != sc_static)
    insert_decl_sp(sp);
    {
        TYP *tp;
            int opened = FALSE;
            if (prm_cplusplus && lastst == openpa)
            {
                opened = TRUE;
                lastst = assign;
            }
        tp = sp->tp;
        if (tp->type == bt_pointer && !(tp->val_flag &VARARRAY))
            tp = tp->btp;
        if (tp->type == bt_pointer && (tp->val_flag &VARARRAY))
        {
            if (tp == sp->tp)
                generror(ERR_VLABLOCKSCOPE, 0, 0);
            else if (sp->storage_class != sc_static)
                generror(ERR_VMSTATIC, 0, 0);
            cppinitinsert(createVarArray(sp, tp, TRUE, tp == sp->tp));
            if (lastst == assign)
            {
                gensymerror(ERR_NOINIT, sp->name);
                endinit();
                return sp->tp->size;
            }
        }
        if (lastst == assign)
        {
            getsym();
            if (sp->init)
                gensymerror(ERR_MULTIPLEINIT, sp->name);
            if (sp->absflag)
            {
                generror(ERR_NOINIT, 0, skm_declend);
                return nbytes;
            }
            nbytes = inittype(sp->tp);
            *sptr = 0;
        }
        if (reshuffle && sp->init)
            ReshuffleDeclsp(sp);
            if (opened)
                needpunc(closepa, skm_declclosepa);
    }
    endinit();
    if (virtualdata)
        return nbytes;
    /* Have to calculate position and debug output info now
     * because of local statics
     */
//    if (!(sp->tp->cflags &DF_CONST) || sp->tp->cflags &DF_VOL ||
//        !scalarnonfloat(sp->tp) || sp->storage_class != sc_static)
    {
        if (!alreadyassigned || sp->init && !alreadyinitted)
        {
            if ((sp->tp->cflags &DF_CONST) && !(sp->tp->cflags &DF_VOL))
                bytes = &constbytes;
            else if ((sp->init || !prm_bss) && !sp->absflag)
                bytes = &databytes;
            else
                if (!sp->init && !sp->absflag)
                    bytes = &bssbytes;
            insertSpace(sp, bytes);
        }
    }
    if (structlevel)
    {
        if (sp->init)
            gensymerror(ERR_NOINIT, sp->name);
    }
    return nbytes;
}

//-------------------------------------------------------------------------

int inittype(TYP *tp)
/*
 * Init for basic types
 */
{
    int nbytes;
    begin_nesting++;
    switch (tp->type)
    {

        case bt_float:
            nbytes = initfloat(FALSE);
            break;
        case bt_longdouble:
            nbytes = initlongdouble(FALSE);
            break;
        case bt_double:
            nbytes = initdouble(FALSE);
            break;
        case bt_fimaginary:
            nbytes = initfloat(TRUE);
            break;
        case bt_rimaginary:
            nbytes = initdouble(TRUE);
            break;
        case bt_lrimaginary:
            nbytes = initlongdouble(TRUE);
            break;
        case bt_fcomplex:
            nbytes = initfcomplex();
            break;
        case bt_rcomplex:
            nbytes = initrcomplex();
            break;
        case bt_lrcomplex:
            nbytes = initlrcomplex();
            break;
        case bt_bool:
			nbytes = initbool();
			break;
        case bt_char:
            nbytes = initchar();
            break;
        case bt_unsignedchar:
            nbytes = inituchar();
            break;
        case bt_unsignedshort:
            nbytes = initushort();
            break;
        case bt_short:
        case bt_segpointer:
            nbytes = initshort();
            break;
        case bt_farpointer:
            nbytes = initfarpointer();
            break;
        case bt_pointer:
            if (tp->btp->type == bt_func)
            {
                nbytes = initpointerfunc();
                break;
            }
            else if (tp->val_flag)
            // VARARRY doesn't get this far
                nbytes = initarray(tp);
            else
                nbytes = initpointer();
            break;
            case bt_ref:
                nbytes = initref(tp);
                break;
        case bt_unsigned:
            nbytes = inituint();
			break;
        case bt_unsignedlong:
            nbytes = initulong();
            break;
        case bt_enum:
            nbytes = initenum();
            break;
        case bt_int:
        case bt_matchall:
            nbytes = initint();
            break;
        case bt_long:
            nbytes = initlong();
            break;
        case bt_longlong:
            nbytes = initlonglong();
            break;
        case bt_unsignedlonglong:
            nbytes = initulonglong();
            break;
        case bt_struct:
            nbytes = initstruct(tp);
            break;
        case bt_union:
            nbytes = initunion(tp);
            break;
            case bt_memberptr:
                nbytes = initmemberptr(tp, tp->sp);
                break;
        default:
            gensymerror(ERR_NOINIT, locsp->name);
            nbytes = 0;
    }
    begin_nesting--;
    return nbytes;
}

//-------------------------------------------------------------------------

int initarray(TYP *tp)
/*
 * Init for arrays
 */
{
    struct decldata **osptr = sptr, **sizearray;
    int nbytes, maxsize = -1;
    int index=0,size;
    int canpad = FALSE;
    char *p;
    int has_begin = FALSE ;
    nbytes = 0;
    if (lastst == begin) {
        has_begin = TRUE ;
        getsym();
    }
    if ((tp->btp->type == bt_char || tp->btp->type == bt_unsignedchar) &&
        lastst == sconst 
        || (tp->btp->type == bt_short || tp->btp->type == bt_unsignedshort) &&
        lastst == lsconst)
    {
        allocated = TRUE;
        if ((tp->btp->type == bt_char || tp->btp->type == bt_unsignedchar) &&
            lastst == sconst)
        {
            canpad = TRUE;
            while (lastst == lsconst || lastst == sconst)
            {
                if (lastst == sconst) {
                    p = laststr;
                    while (laststrlen--)
                    {
                        nbytes++;
                        agflush(1,  *p++);
                    }
                } else {
                    p = laststr;
                    while (laststrlen--)
                    {
                        nbytes++;
                        agflush(1,  *((short *)p)++);
                    }
                }
                getsym();
            }
            if (has_begin)
                needpunc(end, skm_declend);
            if (!tp->size || nbytes < tp->size)
            {
                agflush(1, 0);
                nbytes++;
            }
        }
        else if ((tp->btp->type == bt_short || tp->btp->type ==
            bt_unsignedshort) && lastst == lsconst)
        {
            while (lastst == lsconst || lastst == sconst)
            {
                if (lastst == lsconst) {
                    short *p = laststr;
                    while (laststrlen--)
                    {
                        nbytes += 2;
                        agflush(2,  *p++);
                    }
                } else {
                    char *p = laststr;
                    while (laststrlen--)
                    {
                        nbytes += 2;
                        agflush(2, *p++);
                    }
                }
                getsym();
            }
            if (has_begin)
                needpunc(end, skm_declend);
            if (!tp->size || nbytes < tp->size)
            {
                agflush(2, 0);
                nbytes += 2;
            }
        }
        else
            generror(ERR_PUNCT, semicolon, 0);
    }
    else
    {
        if (begin_nesting == 1 && !has_begin)
            generror(ERR_PUNCT,(int)begin,0) ;
        if (!tp->btp->size)  /* for user defined types */
            fatal("Size of '%s' is unknown or zero", tp->btp->sp->name);
        size = tp->size/tp->btp->size ;
        if (size ==0)
            maxsize = 50;
        sizearray = xalloc(sizeof(struct decldata *) * (size == 0 ? maxsize : size));
        while (lastst != end && lastst != eof)
        {
            if (prm_cplusplus && isstructured(tp->btp) && search
                (cpp_funcname_tab[CI_CONSTRUCTOR], &tp->btp->lst))
            {
                if (!strcmp(lastid, tp->btp->sp->name))
                {
                    TYP *tp1;
                    ENODE *qn;
                    getsym();
                    if (lastst == openpa)
                    {
                        getsym();
                        tp1 = gatherparms(&qn, FALSE);
                        headptr = &head;
                        head = tp->btp;
                        conspair(locsp, sc_global,  - 1, tp1, qn, FALSE, nbytes,
                            FALSE);
                        parm_ns_level--;
                        nbytes += tp->btp->size;
                    }
                    else
                    {
                        backup(id);
                        goto join;
                    }
                }
                else
                {
                    ENODE *qn;
                    TYP *tp2;
                    join: tp2 = exprnc(&qn);
                    if (tp2)
                    {
                        SYM *sm;
                        TYP *tp1 = maketype(bt_func, 0);
                        qn = makenode(en_void, qn, 0);
                        sm = makesym(0);
                        tp1->lst.head = tp1->lst.tail = sm;
                        sm->tp = tp2;
                        headptr = &head;
                        head = tp->btp;
                        conspair(locsp, sc_global,  - 1, tp1, qn, FALSE, nbytes,
                            TRUE);
                        nbytes += tp->btp->size;
                    }
                    else
                        generror(ERR_ILLINIT, 0, 0);
                }
            }
            else {
                if (lastst == openbr) 
                {
                    int l;
					if (!prm_c99)
						generror(ERR_C99_STYLE_INITIALIZATION_USED, 0, 0);
                    getsym() ;
                    l = intexpr(0);
                    index = l;
                    needpunc(closebr,0);
                    needpunc(assign,0);
                }
                    if (index >= size) 
                    {
                        if (maxsize == -1) 
                        {
                            index = 0;
                            generror(ERR_INITSIZE, 0, 0);
                        } 
                        else
                        {
                            size = index + 1 ;
                            if (size > maxsize)
                            {
                                struct decldata *newarray ;
                                int oldsize = maxsize;
                                maxsize = size + 50 ;
                                
                                newarray = xalloc(sizeof(struct decldata *) * maxsize);
                                memcpy(newarray,sizearray,sizeof(struct decldata *) * oldsize) ;
                                sizearray = newarray;
                            }  
                        }
                    }
                sptr = &sizearray[index];
                *sptr = 0;
                inittype(tp->btp);
                index++;
            }
            if (maxsize == -1 && !has_begin && index >= size)
                break;
            if (lastst == comma)
                getsym();
            else if (lastst != end)
            {
                expecttoken(end, 0);
                break;
            }
        }
        if (has_begin)
            needpunc(end,0);
      
        sptr = osptr;
        nbytes = 0 ;
        for (index=0;index < size;index++) 
        {
            *sptr = sizearray[index];
            while (*sptr) {
                switch((*sptr)->mode) 
                {
                    case dd_bool:
                    case dd_byte:
                        nbytes++;
                        ;
                        break;
                    case dd_word:
                        nbytes += stdshortsize;
                        break;
	                case dd_enum:
        	            nbytes += stdenumsize;
            	        break;
					case dd_int:
						nbytes += stdintsize ;
						break;
                    case dd_long:
                        nbytes += stdlongsize;
                        break;
                    case dd_longlong:
                        nbytes += stdlonglongsize;
                        break;
                    case dd_float:
                        nbytes += stdfloatsize;
                        break;
                    case dd_double:
                        nbytes += stddoublesize;
                        break;
                    case dd_ldouble:
                        nbytes += stdldoublesize;
                        break;
                    case dd_fcomplex:
                        nbytes += stdfloatsize * 2;
                        break;
                    case dd_rcomplex:
                        nbytes += stddoublesize * 2;
                        break;
                    case dd_lrcomplex:
                        nbytes += stdldoublesize * 2;
                        break;
                    case dd_pcname:
                        nbytes += stdaddrsize;
                        break;
                    case dd_dataname:
                        nbytes += stdaddrsize;
                        break;
                    case dd_storage:
                        nbytes += (*sptr)->val.i;
                        break;
                    case dd_label:
                        nbytes += stdaddrsize;
                        break;
                    case dd_farpointer:
                        nbytes += stdaddrsize + stdshortsize;
                        break;
                }
                sptr = &(*sptr)->link;
            }
            if (nbytes < tp->btp->size)
                makestorage(tp->btp->size-nbytes);
            nbytes = 0 ;
        }
        nbytes = size * tp->btp->size;
    }
    if (nbytes < tp->size)
    {
        if (prm_cplusplus && isstructured(tp->btp) && search
            (cpp_funcname_tab[CI_CONSTRUCTOR], &tp->btp->lst))
        {
            int num = (tp->size - nbytes) / tp->btp->size;
            TYP *tp1 = maketype(bt_func, 0);
            ENODE *qn = makenode(en_void, 0, 0);
            tp1->lst.head = tp1->lst.tail =  - 1;
            conspair(locsp, sc_global,  - num, tp1, qn, FALSE, nbytes, TRUE);
        }
        else
        {
            nl();
            makestorage(tp->size - nbytes);
        }
        nbytes = tp->size;
    }
    else if (tp->size == 0)
        tp->size = nbytes;
    else if (nbytes > tp->size && (!canpad || nbytes > tp->size + 1) && !(tp->cflags & DF_FLEXARRAY))
        generror(ERR_INITSIZE, 0, 0);
     /* too many initializers */
    return nbytes;
}

//-------------------------------------------------------------------------

int initunion(TYP *tp)
{
    int nbytes=0, has_begin = TRUE;
    struct decldata **osptr = sptr;
    SYM *sp = tp->lst.head;
    if (begin_nesting == 1)
    {
        if (!needpunc(begin,skm_declend))
            return tp->size;       
    }
    else
        if (lastst == begin)
            getsym() ;
        else
            has_begin = FALSE;
    if (lastst == dot) 
    {
		if (!prm_c99)
			generror(ERR_C99_STYLE_INITIALIZATION_USED, 0, 0);
        getsym() ;
        if (lastst == id) 
        {
            SYM *sp1 = tp->lst.head ;
            while (sp1) {
                if (!strcmp(sp1->name,lastid)) 
                    break ;
                sp1 = sp1->next;
            }
            if (!sp1)
                gensymerror(ERR_UNDEFINED,lastid) ;
            else
                sp = sp1;
            getsym();
            needpunc(assign,0);
        }
        else
            gensymerror(ERR_UNDEFINED,lastid) ;
    } 
    sptr = &sp->init;
    sp->init = 0;
    inittype(sp->tp);

    sp = tp->lst.head; /* start at top of symbol table */
    sptr = osptr;
    while (sp) {
        *sptr = sp->init;
        while (*sptr) {
            switch((*sptr)->mode) 
            {
                case dd_bool:
                case dd_byte:
                    nbytes++;
                    ;
                    break;
                case dd_word:
                    nbytes += stdshortsize;
                    break;
                case dd_enum:
       	            nbytes += stdenumsize;
           	        break;
				case dd_int:
					nbytes += stdintsize ;
					break;
                case dd_long:
                    nbytes += stdlongsize;
                    break;
                case dd_longlong:
                    nbytes += stdlonglongsize;
                    break;
                case dd_float:
                    nbytes += stdfloatsize;
                    break;
                case dd_double:
                    nbytes += stddoublesize;
                    break;
                case dd_ldouble:
                    nbytes += stdldoublesize;
                    break;
                case dd_fcomplex:
                    nbytes += stdfloatsize * 2;
                    break;
                case dd_rcomplex:
                    nbytes += stddoublesize * 2;
                    break;
                case dd_lrcomplex:
                    nbytes += stdldoublesize * 2;
                    break;
                case dd_pcname:
                    nbytes += stdaddrsize;
                    break;
                case dd_dataname:
                    nbytes += stdaddrsize;
                    break;
                case dd_storage:
                    nbytes += (*sptr)->val.i;
                    break;
                case dd_label:
                    nbytes += stdaddrsize;
                    break;
                case dd_farpointer:
                    nbytes += stdaddrsize + stdshortsize;
                    break;
            }
            sptr = &(*sptr)->link;
        }
        if (sp->init) 
        {
            sp->init = 0;
            break;
        }
        sp = sp->next;
    }
    if (has_begin)
        needpunc(end, 0);
    if (nbytes < tp->size)
        makestorage(tp->size - nbytes);
    return tp->size;
}

//-------------------------------------------------------------------------

int initstruct(TYP *tp)
/*
 * Init for structures
 */
{
    SYM *sp;
    struct decldata *osptr = sptr;
    int nbytes;
    int has_begin = TRUE;
    if (prm_cplusplus && lastst == id)
    {
        if (lastst != id)
            generror(ERR_IDEXPECT, 0, skm_declend);
        else
        {
            ENODE *ep1,  *ep2;
            TYP *tp2;
            ep1 = makenode(en_nacon, (void*)locsp, 0);
            if (tp->val_flag == 0)
                tp = deref(&ep1, tp);
            tp2 = exprnc(&ep2);
            if (!exactype(tp, tp2, FALSE))
                cppcast(tp2, tp, &ep2, FALSE);
            if (ep2->nodetype == en_callblock)
            {
                ep2->v.p[0] = ep1;
                ep1 = ep2;
            }
            else
            {
                ep1 = makenode(en_moveblock, ep1, ep2);
                ep1->size = tp->size;
            }
            cppinitinsert(ep1);
        }
        return tp->size;
    }
    if (begin_nesting == 1)
    {
        if (!needpunc(begin,skm_declend))
            return tp->size;       
    }
    else
        if (lastst == begin)
            getsym() ;
        else
            has_begin = FALSE;
    nbytes = 0;
    sp = tp->lst.head; /* start at top of symbol table */
    if (lastst == end)
        generror(ERR_ILLINIT,0,0);
    while (lastst != end && lastst != eof)
    {
        if (lastst == dot) 
         {
			if (!prm_c99)
				generror(ERR_C99_STYLE_INITIALIZATION_USED, 0, 0);
		 	sp = tp->lst.head;
            getsym() ;
            if (lastst == id) 
            {
                SYM *sp1 = tp->lst.head ;
                while (sp1) {
                    if (!strcmp(sp1->name,lastid)) 
                        break ;
                    sp1 = sp1->next;
                }
                if (!sp1)
                    gensymerror(ERR_UNDEFINED,lastid) ;
                else
                    sp = sp1;
                getsym();
                if (lastst == openbr) 
                    goto initjn;
                needpunc(assign,0);
            }
            else
                gensymerror(ERR_UNDEFINED,lastid) ;
        } 
initjn:
        if (sp == 0)
            break;
        sptr = &sp->init;
        sp->init = 0;
        if (!strncmp(sp->name, "_$$NULLNAME", 11))
        {
            sp = sp->next;
            continue;
        }
        inittype(sp->tp);
        sp = sp->next;
        if (lastst == comma)
            if (sp)
                getsym();
            else if (has_begin)
                    getsym();
            else
                break;
        else if (!sp || lastst == end)
            break;
        else
            expecttoken(end, 0);
    }
    sp = tp->lst.head; /* start at top of symbol table */
    sptr = osptr;
    while (sp) {
        startbit = sp->tp->startbit;
        if (nbytes < sp->value.i)
            makestorage(sp->value.i - nbytes);
        bits = sp->tp->bits;
        nbytes = sp->value.i;
        if (bits !=  - 1)
        {
            int newbits;
            newbits = sp->tp->size;
            curbitsize = newbits;
            if (sp->init)
			{
                nbytes += agbits(newbits,sp->init->val.i) ;
			}
            else
			{
                nbytes += agbits(newbits,0);
			}
        } 
        else
        {
            nbytes += agbits( - 1, 0);
            *sptr = sp->init;
            while (*sptr) {
                switch((*sptr)->mode)
                {
                    case dd_bool:
                    case dd_byte:
                        nbytes++;
                        ;
                        break;
                    case dd_word:
                        nbytes += stdshortsize;
                        break;
	                case dd_enum:
        	            nbytes += stdenumsize;
            	        break;
					case dd_int:
						nbytes += stdintsize ;
						break;
                    case dd_long:
                        nbytes += stdlongsize;
                        break;
                    case dd_longlong:
                        nbytes += stdlonglongsize;
                        break;
                    case dd_float:
                        nbytes += stdfloatsize;
                        break;
                    case dd_double:
                        nbytes += stddoublesize;
                        break;
                    case dd_ldouble:
                        nbytes += stdldoublesize;
                        break;
                    case dd_fcomplex:
                        nbytes += stdfloatsize * 2;
                        break;
                    case dd_rcomplex:
                        nbytes += stddoublesize * 2;
                        break;
                    case dd_lrcomplex:
                        nbytes += stdldoublesize * 2;
                        break;
                    case dd_pcname:
                        nbytes += stdaddrsize;
                        break;
                    case dd_dataname:
                        nbytes += stdaddrsize;
                        break;
                    case dd_storage:
                        nbytes += (*sptr)->val.i;
                        break;
                    case dd_label:
                        nbytes += stdaddrsize;
                        break;
                    case dd_farpointer:
                        nbytes += stdaddrsize + stdshortsize;
                        break;
                }
                sptr = &(*sptr)->link;
            }
        }
        sp->init = 0;
        sp = sp->next;
    }
    nbytes += agbits( - 1, 0);
    if (nbytes < tp->size)
    {
        makestorage(tp->size - nbytes);
    }
    if (has_begin)
        needpunc(end, skm_declend);
    return tp->size;
}

//-------------------------------------------------------------------------

int makelabel(int id)
{
    struct decldata *q =  *sptr = xalloc(sizeof(struct decldata));
    q->val.i = id;
    sptr = &(*sptr)->link;
    q->mode = dd_label;
    return q->val.i;
}

//-------------------------------------------------------------------------

int makeref(int ispc, SYM *sp, int offset)
{
    struct decldata *q =  *sptr = xalloc(sizeof(struct decldata));
    SetExtFlag(sp, TRUE);
        if (sp->value.classdata.gdeclare)
            sp->value.classdata.gdeclare->mainsym->extflag = TRUE;
    q->val.sp = sp;
    q->offset = offset;
    sptr = &(*sptr)->link;
    q->mode = ispc ? dd_pcname : dd_dataname;
}

//-------------------------------------------------------------------------

int makestorage(long val)
{
    struct decldata *q = xalloc(sizeof(struct decldata));
    if (bits !=  - 1)
    {
        val -= agflush(curbitsize, totbits);
        curbitsize =  - 1;
        bits = -1;
        totbits = 0;
    } 
	if (val < 0)
        DIAG("makestorage - bit field mismatch");
    if (val)
    {
        *sptr = q;
        sptr = &(*sptr)->link;
        q->val.i = val;
        q->mode = dd_storage;
    }
}

//-------------------------------------------------------------------------

int agflush(int size, LLONG_TYPE val)
/*
 * flush a bit field when it is full
 */
{
    struct decldata *q =  *sptr = xalloc(sizeof(struct decldata));
    q->val.i = val;
    sptr = &(*sptr)->link;
	if (size == 1) {
	    q->mode = dd_byte;
		return 1;
	} else if (size == stdaddrsize + stdshortsize) {
            q->mode = dd_farpointer;
            return stdaddrsize + stdshortsize;
	} else if (size <= stdshortsize) {
            q->mode = dd_word;
            return stdshortsize;
	} else if (size <= stdintsize) {
            q->mode = dd_int;
            return stdintsize;
	} else if (size <= stdlongsize) {
            q->mode = dd_long;
            return stdlongsize;
	} else if (size <= stdlonglongsize) {
            q->mode = dd_longlong;
            return stdlonglongsize;
	}
    q->mode = dd_byte;
    return 1;
}

/* Aggregate bits */
int agbits(int size, LLONG_TYPE value)
/*
 * combine bit declarations into a bit field
 */
{
    long rv = 0;
    if (size ==  - 1 && bits !=  - 1)
    {
        rv = agflush(curbitsize, totbits);
        curbitsize =  - 1;
        totbits = 0;
        bits =  - 1;
    }
    else
    {
        if (bits ==  - 1 && size !=  - 1)
            rv = agflush(size, value);
        else if (bits !=  - 1)
        {
            totbits |= (value &mod_mask(bits)) << startbit;
            if (bits + startbit > size *8)
            {
                rv = agflush(curbitsize, totbits);
                totbits = 0;
            }
        }
    }
    startbit =  - 1;
    return (rv);
}

/* handle floating point init
 */
FPF cppfloat(int size, int imaginary)
{
    TYP *t1;
    ENODE *ep;
	FPF zero;
	memset(&zero, 0, sizeof(zero));
	zero.type = IFPF_IS_ZERO;
    allocated = TRUE;

        t1 = exprnc(&ep);
        opt4(&ep, TRUE);
        if (isintconst(ep->nodetype))
            if (imaginary)
                return zero;
            else
			{
				FPF aa;
				LongLongToFPF(&aa, ep->v.i);
				return aa;
			}
        else if (isfloatconst(ep->nodetype))
            if (imaginary)
                return zero;
            else
                return ep->v.f;
        else if (isimaginaryconst(ep->nodetype))
            if (!imaginary)
                return zero;
            else
                return ep->v.f;
        else if (iscomplexconst(ep->nodetype))
            if (imaginary)
                return ep->v.c.i ;
            else
                return ep->v.c.r ;
        else
        {
            if (!isscalar(t1) || !ep || !prm_cplusplus)
                generror(ERR_NEEDCONST, 0, 0);
            else
            {
                ENODE *node1;
                int typ;
                switch (size)
                {
                    case 6:
                        typ = en_floatref;
                        break;
                    case 8:
                        typ = en_doubleref;
                        break;
                    case 10:
                        typ = en_longdoubleref;
                        break;
                }
                cppinitassign(ep);
                #ifdef XXXXX
                    node1 = makenode(typ, makenode(en_nacon, locsp, 0), 0);
                    node1 = makenode(en_assign, node1, ep);
                    cppinitinsert(node1);
                #endif 
            }
        }
        return zero;
}

//-------------------------------------------------------------------------

int arr(ENODE *ep)
{
    if (lvalue(ep))
        return TRUE;
    switch (ep->nodetype)
    {
        case en_nacon:
        case en_napccon:
        case en_nalabcon:
        case en_labcon:
        case en_absacon:
            return TRUE;
        default:
            return FALSE;
    }
}

//-------------------------------------------------------------------------

int intconstinit(int type, LLONG_TYPE *val)
{
    TYP *t1;
    ENODE *ep;
    int done = TRUE;

    int began = 0;
    if (lastst == begin)
    {
        began++;
        getsym();
    }
    t1 = exprnc(&ep);
    opt4(&ep,TRUE);
    if (!ep)
    {
        generror(ERR_ILLINIT, 0, 0);
        return TRUE;
    }
    if (began)
        if (!needpunc(end, 0))
            return TRUE;
	if (t1->type == bt_cond)
		t1 = t1->btp;
    if (!type)
    {
        if (t1->type == bt_pointer || t1->type == bt_segpointer || t1->type ==
            bt_farpointer || t1->type == bt_func || t1->type == bt_ifunc)
        {
            generror(ERR_NONPORT, 0, 0);
        }
        else
        {
            if (!isscalar(t1))
                generror(ERR_NEEDCONST, 0, 0);
        }
    }
    else
    {
		if (t1->type == bt_cond)
			t1 = t1->btp;
        if (t1->type != bt_pointer && t1->type != bt_farpointer && t1->type !=
            bt_segpointer && t1->type != bt_func && t1->type != bt_ifunc)
        if (isscalar(t1))
        {
            if (ep->v.i)
                generror(ERR_NONPORT, 0, 0);
        }
        else
            generror(ERR_NEEDCONST, 0, 0);
    }
    while (castvalue(ep))
        ep = ep->v.p[0];
    if (ep && ep->nodetype == en_labcon)
    {
        makelabel(ep->v.i);
    }
    else if (isintconst(ep->nodetype))
    {
        *val = ep->v.i;
        done = FALSE;
    }
    else if (isfloatconst(ep->nodetype))
    {
        *val = FPFToLongLong(&ep->v.f);
        done = FALSE;
    }
    else
    {
        if (lvalue(ep))
        {
            ep = ep->v.p[0];
        }
        if (arr(ep))
        {
            makeref(ep->nodetype == en_napccon, ((SYM*)(ep->v.sp)), 0);
        }
        else
        {
            if (ep->nodetype == en_add || ep->nodetype == en_addstruc || ep->nodetype == en_array)
            {
                ENODE *ep3,  *ep4;
                ep3 = ep->v.p[0];
                ep4 = ep->v.p[1];
                while (castvalue(ep3))
                    ep3 = ep3->v.p[0];
                while (castvalue(ep4))
                    ep4 = ep4->v.p[0];
                if (!arr(ep3))
                {
                    ep = ep3;
                    ep3 = ep4;
                    ep4 = ep;
                }
                if (arr(ep3))
                {
                    if (lvalue(ep3))
                        ep3 = ep3->v.p[0];
                    if (arr(ep4))
                        generror(ERR_NEEDCONST, 0, 0);
                    else if (isintconst(ep4->nodetype))
                    {
                        makeref(ep3->nodetype == en_napccon ? 1 : 0, ((SYM*)
                            (ep3->v.sp)), ep4->v.i);
                    }
                    else if (isfloatconst(ep4->nodetype))
                    {
                        makeref(ep3->nodetype == en_napccon ? 1 : 0, ((SYM*)
                            (ep3->v.sp)), FPFToLongLong(&ep4->v.f));
                    }
                    else
                        generror(ERR_NEEDCONST, 0, 0);
                }
                else
                    generror(ERR_NEEDCONST, 0, 0);

            }
            else if (ep->nodetype == en_sub)
            {
                ENODE *ep3,  *ep4;
                ep3 = ep->v.p[0];
                ep4 = ep->v.p[1];
                while (castvalue(ep3))
                    ep3 = ep3->v.p[0];
                while (castvalue(ep4))
                    ep4 = ep4->v.p[0];
                if (!arr(ep3))
                {
                    generror(ERR_NEEDCONST, 0, 0);
                }
                else
                {
                    if (lvalue(ep3))
                        ep3 = ep3->v.p[0];
                    if (arr(ep4))
                        generror(ERR_NEEDCONST, 0, 0);
                    else if (isintconst(ep4->nodetype))
                    {
                        makeref(ep3->nodetype == en_napccon ? 1 : 0, ((SYM*)
                            (ep3->v.sp)),  - ep4->v.i);
                    }
                    else if (isfloatconst(ep4->nodetype))
                    {
                        makeref(ep3->nodetype == en_napccon ? 1 : 0, ((SYM*)
                            (ep3->v.sp)),  - FPFToLongLong(&ep4->v.f));
                    }
                    else
                        generror(ERR_NEEDCONST, 0, 0);
                }
            }
            else
            {
                generror(ERR_NEEDCONST, 0, 0);
            }
        }
    }
    return done;
}

//-------------------------------------------------------------------------

long cppint(int size)
{
    TYP *t1;
    ENODE *ep;
    LLONG_TYPE val = 0;
    int done = TRUE;
    allocated = TRUE;
        if (!prm_cplusplus)
    {
        done = intconstinit(0, &val);
    }
        else
        {
            int began = 0;
            done = FALSE;
            if (lastst == begin)
            {
                began++;
                getsym();
            }
            t1 = exprnc(&ep);
            opt4(&ep, TRUE);
            if (began)
                needpunc(end, 0);
            if (!ep)
            {
                generror(ERR_ILLINIT, 0, 0);
                return 0;
            }
            if (isintconst(ep->nodetype))
                val = ep->v.i;
            else if (isfloatconst(ep->nodetype))
                val = FPFToLongLong(&ep->v.f);
            else
            {
                val = 0;
                if (!isscalar(t1) || !ep)
                    generror(ERR_NEEDCONST, 0, 0);
                else
                {
                    ENODE *node1;
                    int typ;
                    switch (size)
                    {
                        case  - 8: typ = en_ll_ref;
                        break;
                        case 8:
                            typ = en_ull_ref;
                            break;
                            case  - 4: typ = en_l_ref;
                            break;
                        case 4:
                            typ = en_ul_ref;
                            break;
                            case  - 2: typ = en_w_ref;
                            break;
                        case 2:
                            typ = en_uw_ref;
                            break;
                            case  - 1: typ = en_b_ref;
                            break;
                        case 1:
                            typ = en_ub_ref;
                            break;
                    }
                    cppinitassign(ep);
                    #ifdef XXXXX
                        node1 = makenode(typ, makenode(en_nacon, locsp, 0), 0);
                        node1 = makenode(en_assign, node1, ep);
                        cppinitinsert(node1);
                    #endif 
                }
            }
        }
    if (size < 0)
        size =  - size;
    #ifdef XXXXX
        switch (0 *size)
        {
            case 1:
                if ((unsigned)val < 0 || (unsigned)val > UCHAR_MAX)
                    generror(ERR_CONSTTOOLARGE, 0, 0);
                break;
                case  - 1: if (val < SCHAR_MIN || val > SCHAR_MAX)
                    generror(ERR_CONSTTOOLARGE, 0, 0);
                break;
            case 2:
                if ((unsigned)val < 0 || (unsigned)val > USHRT_MAX)
                    generror(ERR_CONSTTOOLARGE, 0, 0);
                break;
                case  - 2: if (val < SHRT_MIN || val > SHRT_MAX)
                    generror(ERR_CONSTTOOLARGE, 0, 0);
                break;
                case  - 4: 
                //         if (val < INT_MIN || val > INT_MAX)
                //           generror(ERR_CONSTTOOLARGE,0,0) ;
                break;
            case 4:
                //         if (val < 0 ||val > UINT_MAX)
                //            generror(ERR_CONSTTOOLARGE,0,0) ;
            case 8:
            case  - 8: default:
                break;
        }
    #endif 
    if (!done)
        return agbits(size, val);
    else
        return stdaddrsize;
}

/* Basic type subroutines */
int initfcomplex(void)
{
    struct decldata *q = xalloc(sizeof(struct decldata));
    TYP *tpx;
    ENODE *en=0;
    LLONG_TYPE val =0;
	FPF zero;
	memset(&zero, 0, sizeof(zero));
	zero.type = IFPF_IS_ZERO;
    tpx = exprnc(&en);
    opt4(&en, TRUE);
    if (en) {
        if (isintconst(en->nodetype))
        {
            LongLongToFPF(&q->val.f[0], en->v.i);
            q->val.f[1] = zero;
        }
        else if (isfloatconst(en->nodetype))
        {
            q->val.f[0] = en->v.f;
            q->val.f[1] = zero;
        }
        else if (isimaginaryconst(en->nodetype))
        {
            q->val.f[0] = zero;
            q->val.f[1] = en->v.f;
        }
        else if (iscomplexconst(en->nodetype))
        {
            q->val.f[0] = en->v.c.r;
            q->val.f[1] = en->v.c.i;
        }
        else if (prm_cplusplus)
        {
            cppinitassign(en);
            return 8;
        }
        else
            generror(ERR_ILLINIT, 0, 0);
    }
    *sptr = q;
    sptr = &(*sptr)->link;
    q->mode = dd_fcomplex;
    return (stdfloatsize * 2);
}

//-------------------------------------------------------------------------

int initlrcomplex(void)
{
    struct decldata *q = xalloc(sizeof(struct decldata));
    TYP *tpx;
    ENODE *en=0;
    LLONG_TYPE val =0;
	FPF zero;
	memset(&zero, 0, sizeof(zero));
	zero.type = IFPF_IS_ZERO;
    tpx = exprnc(&en);
    opt4(&en, TRUE);
    if (en) {
        if (isintconst(en->nodetype))
        {
            LongLongToFPF(&q->val.f[0],en->v.i);
            q->val.f[1] = zero;
        }
        else if (isfloatconst(en->nodetype))
        {
            q->val.f[0] = en->v.f;
            q->val.f[1] = zero;
        }
        else if (isimaginaryconst(en->nodetype))
        {
            q->val.f[0] = zero;
            q->val.f[1] = en->v.f;
        }
        else if (iscomplexconst(en->nodetype))
        {
            q->val.f[0] = en->v.c.r;
            q->val.f[1] = en->v.c.i;
        }
        else if (prm_cplusplus)
        {
            cppinitassign(en);
            return 20;
        }
        else
            generror(ERR_ILLINIT, 0, 0);
    }
    *sptr = q;
    sptr = &(*sptr)->link;
    q->mode = dd_lrcomplex;
    return (stdldoublesize * 2);
}

//-------------------------------------------------------------------------

int initrcomplex(void)
{
    struct decldata *q = xalloc(sizeof(struct decldata));
    TYP *tpx;
    ENODE *en=0;
    LLONG_TYPE val =0;
	FPF zero;
	memset(&zero, 0, sizeof(zero));
	zero.type = IFPF_IS_ZERO;
    tpx = exprnc(&en);
    opt4(&en, TRUE);
    if (en) {
        if (isintconst(en->nodetype))
        {
            LongLongToFPF(&q->val.f[0], en->v.i);
            q->val.f[1] = zero;
        }
        else if (isfloatconst(en->nodetype))
        {
            q->val.f[0] = en->v.f;
            q->val.f[1] = zero;
        }
        else if (isimaginaryconst(en->nodetype))
        {
            q->val.f[0] = zero;
            q->val.f[1] = en->v.f;
        }
        else if (iscomplexconst(en->nodetype))
        {
            q->val.f[0] = en->v.c.r;
            q->val.f[1] = en->v.c.i;
        }
        else if (prm_cplusplus)
        {
            cppinitassign(en);
            return 16;
        }
        else
            generror(ERR_ILLINIT, 0, 0);
    }
    *sptr = q;
    sptr = &(*sptr)->link;
    q->mode = dd_rcomplex;
    return (stddoublesize * 2);
}

//-------------------------------------------------------------------------

int initfloat(int imaginary)
{
    struct decldata *q =  *sptr = xalloc(sizeof(struct decldata));
    q->val.f[0] = cppfloat(6,imaginary);
    sptr = &(*sptr)->link;
    q->mode = dd_float;
    return (stdfloatsize);
}

//-------------------------------------------------------------------------

int initlongdouble(int imaginary)
{
    struct decldata *q =  *sptr = xalloc(sizeof(struct decldata));
    q->val.f[0] = cppfloat(10,imaginary);
    sptr = &(*sptr)->link;
    q->mode = dd_ldouble;
    return (stdldoublesize);
}

//-------------------------------------------------------------------------

int initdouble(int imaginary)
{
    struct decldata *q =  *sptr = xalloc(sizeof(struct decldata));
    q->val.f[0] = cppfloat(8,imaginary);
    sptr = &(*sptr)->link;
    q->mode = dd_double;
    return (stddoublesize);
}

//-------------------------------------------------------------------------

int initchar(void)
{
    int rv;
    rv = cppint( - 1);
    return rv;
}
int initbool()
{
    struct decldata *q =  *sptr = xalloc(sizeof(struct decldata));
	LLONG_TYPE ival;
    intconstinit(0, &ival);
    q->val.i = !!ival;
    sptr = &(*sptr)->link;
    q->mode = dd_bool;
    return (1);
}

int initshort(void)
{
    int rv;
    rv = cppint( - stdshortsize);
    return rv;
}

//-------------------------------------------------------------------------

int inituchar(void)
{
    int rv;
    rv = cppint(1);
    return rv;
}

//-------------------------------------------------------------------------

int initushort(void)
{
    int rv;
    rv = cppint(stdshortsize);
    return rv;
}

//-------------------------------------------------------------------------

int initint(void)
{
    int rv;
    rv = cppint( - stdintsize);
    return rv;
}

//-------------------------------------------------------------------------

int inituint(void)
{
    int rv;
    rv = cppint(stdintsize);
    return rv;
}
int initlong(void)
{
    int rv;
    rv = cppint( - stdlongsize);
    return rv;
}

//-------------------------------------------------------------------------

int initulong(void)
{
    int rv;
    rv = cppint(stdlongsize);
    return rv;
}

//-------------------------------------------------------------------------

int initlonglong(void)
{
    int rv;
    rv = cppint( - stdlonglongsize);
    return rv;
}

//-------------------------------------------------------------------------

int initulonglong(void)
{
    int rv;
    rv = cppint(stdlonglongsize);
    return rv;
}
int initenum(void)
{
    int rv=0;
	if (bits != -1) {
		rv = agflush(curbitsize, totbits);
        curbitsize =  - 1;
        totbits = 0;
        bits =  - 1;
	}
    rv += cppint(stdenumsize);
    return rv;
}

//-------------------------------------------------------------------------

    int initmemberptr(TYP *tp, SYM *btypequal)
    {
        TYP *t1;
        ENODE *node,  *node1;
        typequal = 0;
        t1 = asnop(&node, tp);
        if (t1->type != bt_memberptr)
        if (isintconst(node->nodetype) && node->v.i == 0)
        {
            agflush(stdintsize, 0);
            agflush(stdintsize, 0);
        }
        else
            generror(ERR_MEMBERPTRMISMATCH, 0, 0);
        else
        {
            ENODE *ep3 = makenode(en_nacon, locsp, 0);
            t1 = FindMemberPointer(tp, t1, &node);
            if (!exactype(tp, t1, FALSE))
                generror(ERR_MEMBERPTRMISMATCH, 0, 0);
            while (castvalue(node))
                node = node->v.p[0];
            if (lvalue(node))
                node = node->v.p[0];
            node = makenode(en_moveblock, ep3, node);
            node->size = stdmemberptrsize;
            cppinitinsert(node);
        }

    }
    int initref(TYP *tp)
    /*
     * init for reference variables
     */
    {
        SYM *sp;
        TYP *tp1;
        ENODE *node = 0,  *node1;
        allocated = TRUE;

        tp1 = exprnc(&node);
        if (exactype(locsp->tp, tp1, FALSE) || !(node1 = declRef(locsp, tp, tp1, 0,
            node, sc_global)))
        {
            tp = tp->btp;
            if (matchreflvalue(0, locsp->tp, tp1))
            {
                while (castvalue(node))
                    node = node->v.p[0];
                if (lvalue(node))
                    node = node->v.p[0];
                if (node->nodetype == en_nacon)
                {
                    sp = gsearch(((SYM*)node->v.sp)->name);
                    makeref(FALSE, sp, 0);
                }
                else
                {
                    cppinitassign(node);
                    #ifdef XXXXX
                        node1 = makenode(en_a_ref, makenode(en_nacon, locsp, 0),
                            0);
                        node1 = makenode(en_assign, node1, node);
                        cppinitinsert(node1);
                    #endif 
                    genstorage(stdaddrsize);
                }
            }
        }
        else
        {
            cppinitassign(node1);
            #ifdef XXXXX
                node = makenode(en_a_ref, makenode(en_nacon, locsp, 0), 0);
                node1 = makenode(en_assign, node, node1);
                cppinitinsert(node1);
            #endif 
            genstorage(stdaddrsize);
        }
        nl();
        endinit();
        return stdaddrsize;
    }
void dopointer(int makextern)
{
    SYM *sp;
    ENODE *node = 0,  *node1;
    TYP *tp1;
    char *nm;
    int lst = lastst;
        if (!prm_cplusplus)
    {
        long val;
        int done = intconstinit(TRUE, &val);
        if (!done)
            agflush(stdaddrsize, val);
    }
        else
        {
            tp1 = exprnc(&node);
            opt4(&node, FALSE);
            if (node && node->nodetype == en_labcon)
            {
                makelabel(node->v.i);
            }
            else if (node && isintconst(node->nodetype))
            {
                long temp;
                TYP *tp;
                agflush(stdaddrsize, temp = node->v.i);
                if (temp || (tp1->type != bt_pointer) && tp1->type !=
                    bt_farpointer && tp1->type != bt_segpointer && (node->v.i))
                    generror(ERR_NONPORT, 0, 0);
            }
            else if (!node || (tp1->type != bt_pointer && tp1->type !=
                bt_farpointer && tp1->type != bt_segpointer && tp1->type !=
                bt_func && tp1->type != bt_ifunc && !tp1->val_flag))
                gensymerror(ERR_NOINIT, locsp->name);
            else
            {
                if (node->nodetype == en_nacon)
                {
                    sp = gsearch(((SYM*)node->v.sp)->name);
                    if (sp->tp->type == bt_func || sp->tp->type == bt_ifunc)
                    {
                        makeref(TRUE, sp, 0);
                    }
                    else
                        makeref(FALSE, sp, 0);
                    sp->tp->uflags |= UF_USED;
                }
                else
                {
                    if (!prm_cplusplus)
                        gensymerror(ERR_NOINIT, locsp->name);
                    else
                    {
                        cppinitassign(node);
                        #ifdef XXXXX
                            node1 = makenode(en_a_ref, makenode(en_nacon, locsp,
                                0), 0);
                            node1 = makenode(en_assign, node1, node);
                            cppinitinsert(node1);
                        #endif 
                    }
                    makestorage(stdaddrsize);
                }
            }
        }
}

//-------------------------------------------------------------------------

int initpointer(void)
{
    allocated = TRUE;
    dopointer(FALSE);
    endinit();
    return stdaddrsize; /* pointers are 4 bytes long */
}

//-------------------------------------------------------------------------

int initfarpointer(void)
{
    return cppint(6);
}

//-------------------------------------------------------------------------

int initpointerfunc(void)
{
    allocated = TRUE;
    dopointer(TRUE);
    endinit();
    return stdaddrsize; /* pointers are 4 bytes long */
}

/* Finish init */
void endinit(void)
{
    if (lastst != comma && lastst != semicolon && lastst != end)
    {
        expecttoken(end, 0);
        while (lastst != comma && lastst != semicolon && lastst != end &&
            lastst != eof)
            getsym();
    }
}
