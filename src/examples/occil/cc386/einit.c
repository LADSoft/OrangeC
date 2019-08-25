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
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "ccerr.h"
#include "lists.h"
#include "diag.h"

extern int prm_c99;
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
extern int stdinttype, stdunstype, stdintsize, stdldoublesize, stdaddrsize, stdwchar_tsize;
extern int stdshortsize, stdlongsize, stdlonglongsize, stdfloatsize, stddoublesize, stdenumsize;

static ENODE *basenode;
static ENODE *returnval, **sptr;
static int baseoffset;
static SYM *locsp;
static int allocated;
static int startbit, bits;
static int begin_nesting;

static void initassign(ENODE *node, int offset, TYP *tp)
{
    ENODE *ep1 = makenode(en_add,makeintnode(en_icon,offset),basenode); 
    deref(&ep1,tp);
    if (bits != -1) {
        ep1 = makenode(en_bits,ep1,0) ;
        ep1->bits = bits;
        ep1->startbit = startbit;
    }
    ep1 = makenode(en_assign,ep1,node);
    *sptr = makenode(en_void,ep1,0);
    sptr = &(*sptr)->v.p[1];
    bits = -1;
}
//-------------------------------------------------------------------------

ENODE *doeinit(ENODE *node,TYP *tp)
/*
 * Handle static variable initialize
 */
{
    int nbytes;
    ENODE *ep = makenode(en_clearblock, node,0);
    begin_nesting = 0;
    bits = -1;
    allocated = FALSE;
    ep->size = tp->size;
    locsp=node->v.sp;
    basenode = node;
    baseoffset = 0;
    sptr = &returnval;
    *sptr = makenode(en_void,ep,0) ;
    sptr = &(*sptr)->v.p[1];
    nbytes = einittype(tp);
    endinit();
    return returnval;
}

//-------------------------------------------------------------------------

int einittype(TYP *tp)
/*
 * Init for basic types
 */
{
    int nbytes;
    begin_nesting++;
    switch (tp->type)
    {

        case bt_float:
            nbytes = einitfloat();
            break;
        case bt_longdouble:
            nbytes = einitlongdouble();
            break;
        case bt_double:
            nbytes = einitdouble();
            break;
        case bt_fcomplex:
            nbytes = einitfcomplex();
            break;
        case bt_rcomplex:
            nbytes = einitrcomplex();
            break;
        case bt_lrcomplex:
            nbytes = einitlrcomplex();
            break;
        case bt_bool:
        case bt_char:
            nbytes = einitchar();
            break;
        case bt_unsignedchar:
            nbytes = einituchar();
            break;
        case bt_unsignedshort:
            nbytes = einitushort();
            break;
        case bt_short:
        case bt_segpointer:
            nbytes = einitshort();
            break;
        case bt_farpointer:
            nbytes = einitfarpointer();
            break;
        case bt_pointer:
            if (tp->btp->type == bt_func)
            {
                nbytes = einitpointerfunc();
                break;
            }
            else if (tp->val_flag)
            // VARARRY doesn't get this far
                nbytes = einitarray(tp);
            else
                nbytes = einitpointer();
            break;
            case bt_ref:
                nbytes = einitref(tp);
                break;
        case bt_unsignedlong:
            nbytes = einitulong();
            break;
        case bt_unsigned:
            nbytes = einituint();
            break;
        case bt_enum:
            nbytes = einitenum();
            break;
        case bt_long:
            nbytes = einitlong();
            break;
        case bt_int:
        case bt_matchall:
            nbytes = einitint();
            break;
        case bt_longlong:
            nbytes = einitlonglong();
            break;
        case bt_unsignedlonglong:
            nbytes = einitulonglong();
            break;
        case bt_struct:
            nbytes = einitstruct(tp);
            break;
        case bt_union:
            nbytes = einitunion(tp);
            break;
#ifdef XXXXX
            case bt_memberptr:
                nbytes = einitmemberptr(tp, tp->sp);
                break;
#endif
        default:
            gensymerror(ERR_NOINIT, locsp->name);
            nbytes = 0;
    }
    begin_nesting--;
    return nbytes;
}

//-------------------------------------------------------------------------

int einitarray(TYP *tp)
/*
 * Init for arrays
 */
{
    struct decldata **osptr = sptr, **sizearray;
    int nbytes, maxsize = -1;
    int index=0,size;
    int canpad = FALSE;
    char *p;
    int has_begin = TRUE ;
    nbytes = 0;
    if (lastst == begin)
        getsym() ;
    else
        has_begin = FALSE;
    if ((tp->btp->type == bt_char || tp->btp->type == bt_unsignedchar) &&
            lastst == sconst 
            || (tp->btp->type == bt_short || tp->btp->type == bt_unsignedshort) &&
            lastst == lsconst)
    {
        allocated = TRUE;
        if ((tp->btp->type == bt_char || tp->btp->type == bt_unsignedchar) &&
            lastst == sconst)
        {
            int label;
            ENODE *ep1,*ep2;
            char buf[MAX_STRLEN],*p = buf;
            buf[0] = 0;
            canpad = TRUE;
            nbytes = 1 ;
            while (lastst == sconst || lastst == lsconst)
            {
                if (lastst == sconst) {
                    memcpy(p,laststr,laststrlen);
                    p += laststrlen;
                    nbytes += laststrlen;
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
            *p = 0;
            if (has_begin)
                needpunc(end, skm_declend);
			if (!tp->size)
			{
	            tp->size = basenode->v.sp->tp->size = nbytes;
			}
			else
			{
				if (nbytes > tp->size + 1)
				{
					generror(ERR_INITSIZE, 0, 0);
					nbytes = tp->size + 1;
				}
			}
            label = stringlit(buf,FALSE,nbytes);
            ep1 = makenode(en_add,copynode(basenode),makeintnode(en_icon,baseoffset));
            ep2 = makenode(en_moveblock,ep1,makenode(en_labcon,(void *)label,0)) ;
            ep2->size = strlen(buf)+1 < tp->size ? strlen(buf)+1 : tp->size ;
            (*sptr) = makenode(en_void,ep2,0) ;
            sptr = &(*sptr)->v.p[1];
        }
        else if ((tp->btp->type == bt_short || tp->btp->type ==
            bt_unsignedshort) && lastst == lsconst)
        {
            int label;
            ENODE *ep1, *ep2;
            short buf[MAX_STRLEN], *p = buf;
            buf[0] = 0;
            nbytes = 2;
            while (lastst == lsconst || lastst == sconst)
            {
                if (lastst == lsconst) {
                    memcpy(p,laststr, laststrlen * 2) ;
                    p += laststrlen;
                } else {
                    int i;
                    for (i=0; i < laststrlen; i++)
                        *p++ = laststr[i];
                }
                nbytes += laststrlen * 2;
                getsym();
            }
            *p = 0;
            if (has_begin)
                needpunc(end, skm_declend);
		if (!tp->size)
                tp->size = basenode->v.sp->tp->size = nbytes;
		else
			if (nbytes > tp->size + 2)
			{
				generror(ERR_INITSIZE, 0, 0);
				nbytes = tp->size + 2;
			}
            label = stringlit(buf,TRUE,nbytes/2);
            ep1 = makenode(en_add,copynode(basenode),makeintnode(en_icon,baseoffset));
            ep2 = makenode(en_moveblock,ep1,makenode(en_labcon,(void *)label,0)) ;
            ep2->size = pstrlen(buf)*2+2 < tp->size ? pstrlen(buf)*2+2 : tp->size ;
            (*sptr) = makenode(en_void,ep2,0) ;
            sptr = &(*sptr)->v.p[1];
        }
        else
            generror(ERR_PUNCT, semicolon, 0);
    }
    else
    {            
        if (begin_nesting == 1 && !has_begin)
            needpunc(begin,0) ;
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
                int ofs = baseoffset ;
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
                baseoffset += index * tp->btp->size;
                einittype(tp->btp);
                index++;
                baseoffset = ofs ;
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
            while (*sptr)
                sptr = &(*sptr)->v.p[1];
        }
        nbytes = size * tp->btp->size;
        if (maxsize != -1)
            basenode->v.sp->tp->size = nbytes;
    }
    return tp->size;
}

//-------------------------------------------------------------------------

int einitunion(TYP *tp)
{
    int has_begin = TRUE;
    struct decldata **osptr = sptr;
    int ooffset;
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
    ooffset = baseoffset;
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
    baseoffset += sp->value.i;
    einittype(sp->tp);
    baseoffset = ooffset;
    sp = tp->lst.head; /* start at top of symbol table */
    sptr = osptr;
    while (sp) 
    {
        *sptr = sp->init;
        while (*sptr)
            sptr = &(*sptr)->v.p[1];
        if (sp->init) 
        {
            sp->init = 0;
            break;
        }
        sp = sp->next;
    }
    if (has_begin)
        needpunc(end, 0);
    return tp->size;
}

//-------------------------------------------------------------------------

int einitstruct(TYP *tp)
/*
 * Init for structures
 */
{
    SYM *sp;
    struct decldata *osptr = sptr;
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
    sp = tp->lst.head; /* start at top of symbol table */
    if (lastst == end)
        generror(ERR_ILLINIT,0,0);
    while (lastst != end && lastst != eof)
    {
        int ooffset = baseoffset;
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
                {
                    getsym() ;
                    baseoffset += intexpr(0);
                    needpunc(closebr,0);
                }
                needpunc(assign,0);
            }
            else
                gensymerror(ERR_UNDEFINED,lastid) ;
        } 
        if (sp == 0)
            break;
        sptr = &sp->init;
        sp->init = 0;
        baseoffset += sp->value.i;
        startbit = sp->tp->startbit;
        bits = sp->tp->bits;
        if (!strncmp(sp->name, "_$$NULLNAME", 11))
        {
            baseoffset = ooffset;
            sp = sp->next;
            continue;
        }
        einittype(sp->tp);

        baseoffset = ooffset ;    
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
        *sptr = sp->init;
        while (*sptr)
            sptr = &(*sptr)->v.p[1];
        sp->init = 0;
        sp = sp->next;
    }
    if (has_begin)
        needpunc(end, skm_declend);
    return tp->size;
}

//-------------------------------------------------------------------------


/* handle floating point einit
 */
ENODE *ecppfloat(void)
{
    TYP *t1;
    ENODE *ep;
    allocated = TRUE;

        t1 = exprnc(&ep);
        if (isintconst(ep->nodetype))
            return ep;
        else if (isfloatconst(ep->nodetype))
            return ep;
        else
        {
            if (!isscalar(t1) || !ep)
                generror(ERR_NEEDCONST, 0, 0);
            else
            {
                return ep ;
            }
        }
        return 0;
}

//-------------------------------------------------------------------------

ENODE *ecppint(void)
{
    TYP *t1;
    ENODE *ep;
    LLONG_TYPE val = 0;
    int done = TRUE;
    allocated = TRUE;
        {
            int began = 0;
            done = FALSE;
            if (lastst == begin)
            {
                began++;
                getsym();
            }
            t1 = exprnc(&ep);
            opt4(&ep, FALSE);
            if (began)
                needpunc(end, 0);
            if (!ep)
            {
                generror(ERR_ILLINIT, 0, 0);
                return 0;
            }
            if (isintconst(ep->nodetype))
                return ep ;
            else if (isfloatconst(ep->nodetype)) {
                ep->nodetype = en_icon;
                ep->v.i = FPFToLongLong(&ep->v.f);
                return ep;
            } 
            else
            {
                val = 0;
                if (!isscalar(t1) || !ep)
                    generror(ERR_NEEDCONST, 0, 0);
                else
                {
                    return ep;
                }
            }
        }
        return 0;
}

/* Basic type subroutines */
int einitfcomplex(void)
{
    TYP tp;
    tp.type = bt_float;
    if (lastst == begin)
    {
        initassign(ecppfloat(),baseoffset,&tp) ;
        if (lastst == comma)
        {
            getsym();
            initassign(ecppfloat(),baseoffset+stdfloatsize,&tp) ;
        } 
        else {
            ENODE *ep = makenode(en_fcon,0,0) ;
            ep->v.f.type = IFPF_IS_ZERO;
            initassign(ep,baseoffset+stdfloatsize,&tp) ;
        }
        needpunc(end, 0);
    }
    else
    {
        ENODE *ep = makenode(en_fcon,0,0) ;
        initassign(ecppfloat(),baseoffset,&tp) ;
        ep->v.f.type = IFPF_IS_ZERO;
        initassign(ep,baseoffset+stdfloatsize,&tp) ;
    }
    return stdfloatsize * 2;
}

//-------------------------------------------------------------------------

/* Basic type subroutines */
int einitlrcomplex(void)
{
    TYP tp;
    tp.type = bt_longdouble;
    if (lastst == begin)
    {
        initassign(ecppfloat(),baseoffset,&tp) ;
        if (lastst == comma)
        {
            getsym();
            initassign(ecppfloat(),baseoffset+stdldoublesize,&tp) ;
        } 
        else {
            ENODE *ep = makenode(en_fcon,0,0) ;
            ep->v.f.type = IFPF_IS_ZERO;
            initassign(ep,baseoffset+stdldoublesize,&tp) ;
        }
        needpunc(end, 0);
    }
    else
    {
        ENODE *ep = makenode(en_fcon,0,0) ;
        initassign(ecppfloat(),baseoffset,&tp) ;
        ep->v.f.type = IFPF_IS_ZERO;
        initassign(ep,baseoffset+stdldoublesize,&tp) ;
    }
    return stdldoublesize * 2;
}

//-------------------------------------------------------------------------

/* Basic type subroutines */
int einitrcomplex(void)
{
    TYP tp;
    tp.type = bt_double;
    if (lastst == begin)
    {
        initassign(ecppfloat(),baseoffset,&tp) ;
        if (lastst == comma)
        {
            getsym();
            initassign(ecppfloat(),baseoffset+stddoublesize,&tp) ;
        } 
        else {
            ENODE *ep = makenode(en_fcon,0,0) ;
            ep->v.f.type = IFPF_IS_ZERO;
            initassign(ep,baseoffset+stddoublesize,&tp) ;
        }
        needpunc(end, 0);
    }
    else
    {
        ENODE *ep = makenode(en_fcon,0,0) ;
        initassign(ecppfloat(),baseoffset,&tp) ;
        ep->v.f.type = IFPF_IS_ZERO;
        initassign(ep,baseoffset+stddoublesize,&tp) ;
    }
    return stddoublesize * 2;
}

//-------------------------------------------------------------------------

int einitfloat(void)
{
    TYP tp;
    tp.type = bt_float;

    initassign(ecppfloat(),baseoffset,&tp);
    return (stdfloatsize);
}

//-------------------------------------------------------------------------

int einitlongdouble(void)
{
    TYP tp;
    tp.type = bt_longdouble;
    initassign(ecppfloat(),baseoffset,&tp);
    return (stdldoublesize);
}

//-------------------------------------------------------------------------

int einitdouble(void)
{
    TYP tp;
    tp.type = bt_double;
    initassign(ecppfloat(),baseoffset,&tp);
    return (stddoublesize);
}

//-------------------------------------------------------------------------

int einitchar(void)
{
    TYP tp ;
    tp.type = bt_char;
    initassign(ecppint(),baseoffset,&tp) ;
    return 1;
} 
int einitshort(void)
{
    TYP tp ;
    tp.type = bt_short;
    initassign(ecppint(),baseoffset,&tp) ;
    return stdshortsize;
}

//-------------------------------------------------------------------------

int einituchar(void)
{
    TYP tp ;
    tp.type = bt_unsignedchar;
    initassign(ecppint(),baseoffset,&tp) ;
    return 1;
}

//-------------------------------------------------------------------------

int einitushort(void)
{
    TYP tp ;
    tp.type = bt_unsignedshort;
    initassign(ecppint(),baseoffset,&tp) ;
    return stdshortsize;
}

//-------------------------------------------------------------------------

int einitenum(void)
{
    TYP tp ;
    tp.type = bt_int;
    initassign(ecppint(),baseoffset,&tp) ;
    return stdenumsize;
}
int einitint(void)
{
    TYP tp ;
    tp.type = bt_int;
    initassign(ecppint(),baseoffset,&tp) ;
    return stdintsize;
}
int einitlong(void)
{
    TYP tp ;
    tp.type = bt_int;
    initassign(ecppint(),baseoffset,&tp) ;
    return stdlongsize;
}

//-------------------------------------------------------------------------

int einituint(void)
{
    TYP tp ;
    tp.type = bt_unsigned;
    initassign(ecppint(),baseoffset,&tp) ;
    return stdintsize;
}
int einitulong(void)
{
    TYP tp ;
    tp.type = bt_unsigned;
    initassign(ecppint(),baseoffset,&tp) ;
    return stdlongsize;
}

//-------------------------------------------------------------------------

int einitlonglong(void)
{
    TYP tp ;
    tp.type = bt_longlong;
    initassign(ecppint(),baseoffset,&tp) ;
    return stdlonglongsize;
}

//-------------------------------------------------------------------------

int einitulonglong(void)
{
    TYP tp ;
    tp.type = bt_unsignedlonglong;
    initassign(ecppint(),baseoffset,&tp) ;
    return stdlonglongsize;
}

//-------------------------------------------------------------------------

#ifdef CPLUSPLUSXXX
    int einitmemberptr(TYP *tp, SYM *btypequal)
    {
        TYP *t1;
        ENODE *node,  *node1;
        typequal = 0;
        t1 = asnop(&node, tp);
        if (t1->type != bt_memberptr)
        if (isintconst(node->nodetype) && node->v.i == 0)
        {
            agflush(4, 0);
            agflush(4, 0);
        }
        else
            generror(ERR_MEMBERPTRMISMATCH, 0, 0);
        else
        {
            ENODE *ep3 = makenode(en_nacon, locsp, 0);
            t1 = FindMemberPointer(tp, t1, &node, &node);
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
#endif
    int einitref(TYP *tp)
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
            node, sc_auto)))
        {
            tp = tp->btp;
            if (matchreflvalue(0, locsp->tp, tp1))
            {
                while (castvalue(node))
                    node = node->v.p[0];
                if (lvalue(node))
                    node = node->v.p[0];
                {
                    TYP tp;
                    tp.type = bt_pointer;
                    initassign(node,baseoffset,&tp);
                }
            }
        }
        else
        {
            TYP tp;
            tp.type = bt_pointer;
            initassign(node1,baseoffset,&tp);
        }
        endinit();
        return stdaddrsize;
    }
void edopointer(int makextern)
{
    SYM *sp;
    ENODE *node = 0,  *node1;
    TYP *tp1;
    char *nm;
    int lst = lastst;
        {
            tp1 = exprnc(&node);
            opt4(&node, FALSE);
            if (node && node->nodetype == en_labcon)
            {
                TYP tp;
                tp.type = bt_pointer;
				initassign(node, baseoffset, &tp);
            }
            else if (node && isintconst(node->nodetype))
            {
                long temp;
                TYP tp;
                tp.type = bt_pointer;
                initassign(node,baseoffset,&tp);
                temp = node->v.i;
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
                    TYP tp;
                    tp.type = bt_pointer;
                    initassign(node,baseoffset,&tp);
                    node->v.sp->tp->uflags |= UF_USED;
                }
                else
                {
                    {
                        TYP tp;
                        tp.type = bt_pointer;
                        initassign(node,baseoffset,&tp);
                    }
                }
            }
        }
}

//-------------------------------------------------------------------------

int einitpointer(void)
{
    allocated = TRUE;
    edopointer(FALSE);
    endinit();
    return stdaddrsize; /* pointers are 4 bytes long */
}

//-------------------------------------------------------------------------

int einitfarpointer(void)
{
    TYP tp;
    tp.type = bt_farpointer;
    initassign(ecppint(),baseoffset,&tp);
    return stdaddrsize + stdshortsize;
}

//-------------------------------------------------------------------------

int einitpointerfunc(void)
{
    allocated = TRUE;
    edopointer(TRUE);
    endinit();
    return stdaddrsize; /* pointers are 4 bytes long */
}
