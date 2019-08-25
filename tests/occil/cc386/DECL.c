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
 * handle ALL declarative statements
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "ccerr.h"
#include "diag.h"

#define LONGS /* enable 64 bit support */

extern int sys_inc;
extern int prm_c99;
extern int funcnesting;
extern ENODE *expr_runup[];
extern ENODE *expr_rundown[];
extern int expr_updowncount;
extern int incatchclause;
extern int intemplateargs;
extern LIST *local_using_list,  *local_tag_using_list;
extern TYP stdenum;
extern int prm_bss;
extern int bssbytes, databytes;
extern int statictemplate;
;
extern NAMESPACE *thisnamespace;
extern SYM *templateSP;
extern int prm_debug, prm_farkeyword;
extern int stdinttype, stdunstype, stdintsize, stdldoublesize, stdaddrsize, stdwchar_tsize;
extern int stdshortsize, stdlongsize, stdlonglongsize, stdfloatsize, stddoublesize, stdenumsize;
extern int strucadd, strucmod;
extern TYP stdint;
extern LIST *instantiated_inlines;
extern int classhead, vtabhead;
extern int lineno, errlineno;
extern int packdata[], packlevel;
extern int prm_cmangle, prm_microlink, prm_asmfile;
extern char laststr[];
extern int global_flag;
extern TABLE *gsyms;
extern TABLE lsyms, ltags, oldltag;
extern SNODE *cbautoinithead,  *cbautoinittail;
extern int prm_revbits;
extern int prm_68020;
extern int prm_ansi;
extern SYM *currentfunc;
extern int prm_cplusplus;
extern TYP stdconst;
extern enum e_sym lastst;
extern char lastid[];
extern int block_nesting;
extern long nextlabel;
extern char *cpp_funcname_tab[];
extern int stdmemberptrsize;
extern ENODE *block_rundown;
extern char *infile;
extern short *lptr;
extern int lastch;
extern TABLE *tagtable;
extern ENODE *thisenode;
extern TYP stdvoidfunc;
extern int skm_closepa[];
extern int nodeclfunc;
extern int virtualfuncs;
extern int infuncargs;
extern SYM *parm_namespaces[20][100];
extern int parm_ns_counts[20];
extern int parm_ns_level;
extern int parsing_params;
extern int prm_fardefault;

int inarrayindexparsing;
TABLE *funcparmtable;
int conscount;
int increatingtemplateargs;

int indefaultinit;
ILIST *deferredFunctions;
int structlevel;
int aFriend;
int recordingTemplate;
int templateFunc;
int templateLookup;
int recordingClassFunc;

char *Cstr = "C";
SYM *declclass;
SYM *typequal;


int unnamedstructcount;
int nullnamecount;
int mangleflag;
int skm_declopenpa[] = 
{
    semicolon, comma, openpa, eq, 0
};
int skm_declclosepa[] = 
{
    semicolon, comma, closepa, eq, 0
};
int skm_declcomma[] = 
{
    semicolon, comma, begin, eq, 0
};
int skm_declbegin[] = 
{
    begin, semicolon, 0
};
int skm_declclosebr[] = 
{
    comma, closebr, eq, semicolon, 0
};
int skm_declend[] = 
{
    semicolon, comma, end, eq, 0
};
int skm_tempend[] = 
{
    semicolon, comma, end, 0
};
int skm_decl[] = 
{
    semicolon, kw_int, kw_long, kw_short, kw_char, kw_float, kw_bool, kw_double,
        kw_struct, kw_union, kw_enum, kw_unsigned, kw_signed, kw_auto,
        kw_extern, kw_static, kw_restrict, kw_register, kw_typedef, id, kw_void,
        kw_wchar_t, 0
};

int mainfunc;
TYP *head = 0,  **headptr;
TYP *tail = 0;
char declid[100];
int ispascal, isstdcall, iscdecl, isindirect, isexport, isimport;
char *importname;
LIST *localfuncs,  *localdata; /* 68k */
struct template  *currentTemplate,  *searchTemplate;

int inprototype;
static int memberptr;
static SYM *lastdecl;
static int pcount = 0;
static int bittype =  - 1;
static int curbit = 0;
int sizeoflastbit;
int manglelevel;
static int intflag, farflag, segflag;
int cppflags;
int defaulttype;
static int decl3size;
static int templatebufSize;
static short *templatebuf;
static int templatebufIndex;
static int encounteredflexarray;

void templateWade(void);

void declini(void)
/*
 * init
 */
{
    funcparmtable = 0;
    inarrayindexparsing = FALSE;
    increatingtemplateargs = FALSE;
    bittype =  - 1;
    head = tail = 0;
    declid[0] = 0;
    pcount = 0;
    mangleflag = TRUE;
    manglelevel = 0;
    encounteredflexarray = FALSE;
    declclass = 0;
    localfuncs = 0;
    localdata = 0;
    unnamedstructcount = 0;
    templatebufSize = templatebufIndex = 0;
    structlevel = 0;
    deferredFunctions = 0;
    indefaultinit = 0;
} 
int imax(int i, int j)
{
    return (i > j) ? i : j;
}
int isfunc(TYP *tp)
{
	if (tp->type == bt_ref)
		tp = tp->btp;
	return (tp->type == bt_func || tp->type == bt_ifunc);
}

//-------------------------------------------------------------------------

char *litlate(char *s)
{
    char *p;
    p = xalloc(strlen(s) + 1);
    strcpy(p, s);
    return p;
}

//-------------------------------------------------------------------------

TYP *maketype(int bt, int siz)
/*
 * Create a type list entry
 */
{
    TYP *tp;

    tp = xalloc(sizeof(TYP));
    tp->val_flag = 0;
    tp->bits = tp->startbit =  - 1;
    tp->size = siz;
    tp->type = bt;
    tp->sp = 0;
    tp->cflags = 0;
    tp->uflags = 0;
    tp->lst.head = tp->lst.tail = 0;
    return tp;
}

//-------------------------------------------------------------------------

TYP *cponetype(TYP *t)
/*
 * Copy a entry
 */
{
    TYP *tp;
    tp = xalloc(sizeof(TYP));
    tp->type = t->type;
    tp->val_flag = t->val_flag;
    tp->cflags = t->cflags;
    tp->bits = t->bits;
    tp->startbit = t->startbit;
    tp->size = t->size;
    tp->lst = t->lst;
    tp->btp = t->btp;
    tp->sp = t->sp;
    tp->uflags = t->uflags;
    tp->typeindex = t->typeindex;
    tp->enumlist = t->enumlist;
    tp->esize = t->esize;
    return tp;
}

//-------------------------------------------------------------------------

TYP *copytype(TYP *itp, int flags)
/*
 * copy a type tree
 */
{
    TYP *head,  *tail,  *x = itp,  *reclone = 0;
    while (x->type == bt_pointer || x->type == bt_farpointer || x->type ==
        bt_segpointer || x->type == bt_ref)
        x = x->btp;
    if (isstructured(x))
    if (!x->lst.head)
    {
        reclone = x;
    }
    if (!itp->typeindex)
        itp->typeindex = xalloc(sizeof(int));
    head = tail = cponetype(itp);
    while (itp->type == bt_pointer || itp->type == bt_farpointer || itp->type 
        == bt_segpointer || itp->type == bt_ref)
    {
        tail = tail->btp = cponetype(itp->btp);
        itp = itp->btp;
    }
    if (itp->type == bt_func || itp->type == bt_ifunc)
    {
        tail->btp = cponetype(itp->btp);
    }
    else
    {
        tail->cflags |= flags;
    }
    if (reclone)
    {
        x = head;
        while (x->type == bt_pointer || x->type == bt_farpointer || x->type ==
            bt_segpointer)
            x = x->btp;
        x->reclone = reclone->reclone;
        reclone->reclone = x;
    }
    return head;
}

//-------------------------------------------------------------------------

SYM *parsetype(SYM *typesp, int pass)
{

    if (prm_cplusplus && typesp->istemplatevar && recordingTemplate) {
        while (lastst == classsel) {
            getsym();
            if (lastst != id)
                break;
            getsym();
        }
        return typesp;
    }
    while (prm_cplusplus && lastst == classsel)
    {
        if (typesp->istemplate)
            templateMatchCheck(typesp, currentTemplate);
        typequal = typesp;
        if (typequal->tp->type == bt_templateplaceholder)
            typequal = typequal->tp->lst.head;
        getsym();
        if (lastst == kw_operator || lastst == compl)
        {
            typesp = 0;
            break;
        }
        else if (lastst != id)
        {
            generror(ERR_IDEXPECT, 0, 0);
            break;
        }
        else if (!(typesp = tcsearch(lastid, FALSE)))
        {
            break;
        }
        else if (typesp->storage_class != sc_type && typesp->storage_class !=
            sc_namespace)
            break;
        else
        {
            if (pass)
                getsym();
            else
            {
                if (isspace(lastch))
                    break;
                getsym();
                if (lastst != classsel)
                {
                    backup(id);
                    break;
                }
            }
        }
    }
    if (lastst == classselstar)
    {
        typequal = typesp;
        typesp = 0;
    }
    return typesp;
}

//-------------------------------------------------------------------------

int decl(TABLE *table, int cppflags)
/*
 * parse the declare keywords and create a basic type
 */
{
    int tagsonly = FALSE;
    int tp;
    int flags = 0;
    SYM *typesp = 0,  *dc;
    int opened = FALSE;
    defaulttype = FALSE;

    while (lastst == kw_const || lastst == kw_volatile || lastst == kw_inline 
        || lastst == kw_register
        || lastst == kw__intrinsic || lastst == kw_restrict || lastst ==
        kw__import || lastst == kw__export || lastst == kw_static || (prm_cplusplus && 
        (lastst == kw_virtual || lastst == kw_friend || lastst ==
        kw_explicit || lastst == kw_typename)))
    {
        switch (lastst)
        {
            case kw_typename:
                getsym();
                break;
            case kw__import:
                isimport = TRUE;
                importname = 0;
                getsym();
                if (lastst == openpa)
                {
                    getsym();
                    if (lastst == sconst)
                    {
                        importname = litlate(laststr);
                        getsym();
                        needpunc(closepa, 0);
                    }
                    else
                        generror(ERR_NEEDCHAR, '"', 0);
                }
                break;
            case kw__export:
                isexport = TRUE;
                getsym();
                break;
            case kw_const:
                flags |= DF_CONST;
                getsym();
                break;
            case kw_register:
                flags |= DF_AUTOREG;
                getsym();
                break;
            case kw_restrict:
                flags |= DF_RESTRICT;
                errlineno = lineno;
                generror(ERR_RESTRICTPTR, 0, 0);
                getsym();
                break;
            case kw_volatile:
                flags |= DF_VOL;
                getsym();
                break;
            case kw__intrinsic:
                flags |= DF_INTRINS;
                getsym();
                break;
            case kw_inline:
                cppflags |= PF_INLINE;
                getsym();
                break;
                case kw_virtual:
                    cppflags |= PF_VIRTUAL;
                    getsym();
                    break;
                case kw_static:
                    cppflags |= PF_STATIC;
                    getsym();
                    break;
                case kw_friend:
                    getsym();
                    aFriend = TRUE;
                    break;
                case kw_explicit:
                    getsym();
                    cppflags |= PF_EXPLICIT;
                    break;
        }
    }
    switch (lastst)
    {
        case kw_void:
            head = tail = maketype(bt_void, 0);
            getsym();
            break;
        case kw_bool:
            head = tail = maketype(bt_bool, 1);
            getsym();
            break;
        case kw_char:
            head = tail = maketype(bt_char, 1);
            getsym();
            break;
        case kw_short:
            getsym();
            tp = bt_short;
            if (lastst == kw_unsigned)
            {
                tp = bt_unsignedshort;
                getsym();
            }
            if (lastst == kw_int)
                getsym();
            head = tail = maketype(tp, stdshortsize);
            break;
        case kw_wchar_t:
            getsym();
            head = tail = maketype(bt_unsignedshort, stdwchar_tsize);
            break;
        case kw_int:
            getsym();
            head = tail = maketype(stdinttype, stdintsize);
            break;
        case kw___int64:
                    getsym();
                    #ifdef LONGS
                        head = tail = maketype(bt_longlong, stdlonglongsize);
                    #else 
                        generror(ERR_LONGLONG, 0, 0);
                    #endif 
            break;
        default:
            defaulttype = TRUE;
            head = tail = maketype(stdinttype, stdintsize);
            break;
        case kw_unsigned:
            getsym();
            if (lastst == kw_const)
            {
                flags |= DF_CONST;
                getsym();
            }
            switch (lastst)
            {
            case kw___int64:
                    getsym();
                    #ifdef LONGS
                        head = tail = maketype(bt_unsignedlonglong, stdlonglongsize);
                    #else 
                        generror(ERR_LONGLONG, 0, 0);
                    #endif 
                break ;
            case kw_char:
                getsym();
                head = tail = maketype(bt_unsignedchar, 1);
                break;
            case kw_short:
                getsym();
                if (lastst == kw_int)
                    getsym();
                head = tail = maketype(bt_unsignedshort, stdshortsize);
                break;
            case kw_long:
                getsym();
                if (lastst == kw_long)
                {
					if (!prm_c99)
						gensymerror(ERR_TYPE_C99, "unsigned long long");
                    getsym();
                    #ifdef LONGS
                        head = tail = maketype(bt_unsignedlonglong, stdlonglongsize);
                    #else 
                        generror(ERR_LONGLONG, 0, 0);
                    #endif 
                }
                else
                    head = tail = maketype(bt_unsigned/*long*/, stdlongsize);
                if (lastst == kw_int)
                    getsym();
                break;
            case kw_int:
                getsym();
            default:
                head = tail = maketype(stdunstype, stdintsize);
                break;
            }
            break;
        case kw_signed:
            getsym();
            if (lastst == kw_const)
            {
                flags |= DF_CONST;
                getsym();
            }
            switch (lastst)
            {
            case kw___int64:
                    getsym();
                    #ifdef LONGS
                        head = tail = maketype(bt_longlong, stdlonglongsize);
                    #else 
                        generror(ERR_LONGLONG, 0, 0);
                    #endif 
                break ;
            case kw_char:
                getsym();
                head = tail = maketype(bt_char, 1);
                break;
            case kw_short:
                getsym();
                if (lastst == kw_int)
                    getsym();
                head = tail = maketype(bt_short, stdshortsize);
                break;
            case kw_long:
                getsym();
                if (lastst == kw_long)
                {
					if (!prm_c99)
						gensymerror(ERR_TYPE_C99, "long long");
                    getsym();
                    #ifdef LONGS
                        head = tail = maketype(bt_longlong, stdlonglongsize);
                    #else 
                        generror(ERR_LONGLONG, 0, 0);
                    #endif 
                }
                else
                    head = tail = maketype(bt_int/*long*/, stdlongsize);
                if (lastst == kw_int)
                    getsym();
                break;
            case kw_int:
                getsym();
            default:
                head = tail = maketype(bt_int, stdintsize);
                break;
            }
            break;
        case kw_class:
            if (prm_cplusplus)
            {
                getsym();
                if (aFriend)
                    table = gsyms;
                declstruct(table, bt_class, flags | DF_PRIVATE, cppflags);
                typequal = 0;
                break;
            }
        case id:
             /* no type declarator */
            defaulttype = TRUE;
            if ((typesp = typesearch(lastid)) && (typesp->storage_class ==
                sc_type || typesp->storage_class == sc_namespace))
            {
                getsym();
                if (prm_cplusplus)
                {
                    if (typesp && typesp->istemplate)
                        if (lastst == lt)
                            typesp = lookupTemplateType(typesp,cppflags);
                        else
                            if (lastst == classsel)
                                gensymerror(ERR_NEEDSPECIAL, typesp->name);
                    typesp = parsetype(typesp, TRUE);
                    if (lastst == openpa && typesp && declclass && typesp
                        ->mainsym == declclass->mainsym)
                    {
                        head = tail = maketype(bt_consplaceholder, stdaddrsize); 
                            // limited life
                        break;
                    }
                }
                    if (typesp && typesp->storage_class == sc_type)
                    {
                    //                                defaulttype = FALSE ;
                    typequal = 0;
                    head = tail = copytype(typesp->tp, flags);
                    }
                    else
                    {
                        head = tail = maketype(bt_int, stdintsize);
                        if (prm_c99)
                        {
                            errlineno = lineno;
                            generror(ERR_TYPENAMEEXP, 0, 0);
                        }

                    }
            }
            else
            {
                head = tail = maketype(bt_int, stdintsize);
                if (prm_c99)
                {
                    errlineno = lineno;
                    generror(ERR_TYPENAMEEXP, 0, 0);
                }
            }
            break;
        case kw_operator:
            head = tail = maketype(bt_int, stdintsize);
            break;
        case kw_float:
            getsym();
            if (lastst == kw__Complex)
            {
                getsym();
                head = tail = maketype(bt_fcomplex, stdfloatsize * 2);
            }
            else if (lastst == kw__Imaginary)
            {
                getsym();
                head = tail = maketype(bt_fimaginary, stdfloatsize);
            }
            else
                head = tail = maketype(bt_float, stdfloatsize);
            break;
        case kw_long:
            getsym();
            if (lastst == kw_double)
            {
                getsym();
                if (lastst == kw__Complex)
                {
                    getsym();
                    head = tail = maketype(bt_lrcomplex, stdldoublesize *2);
                }
                else if (lastst == kw__Imaginary)
                {
                    getsym();
                    head = tail = maketype(bt_lrimaginary, stdldoublesize);
                }
                else
                    head = tail = maketype(bt_longdouble, stdldoublesize);
                break;
            }
            if (lastst == kw_float)
            {
                getsym();
                if (lastst == kw__Complex)
                {
                    getsym();
                    head = tail = maketype(bt_rcomplex, stddoublesize * 2);
                }
                else if (lastst == kw__Imaginary)
                {
                    getsym();
                    head = tail = maketype(bt_rimaginary, stddoublesize);
                }
                else
                    head = tail = maketype(bt_double, stddoublesize);
                break;
            }
            if (lastst == kw_long)
            {
					if (!prm_c99)
						gensymerror(ERR_TYPE_C99, "long long");
                getsym();
                #ifdef LONGS
                    tp = bt_longlong;
                #else 
                    generror(ERR_LONGLONG, 0, 0);
                    tp = bt_int;
                #endif 
            }
            else
                tp = bt_int /*long*/;

            if (lastst == kw_unsigned)
            {
                #ifdef LONGS
                    if (tp == bt_longlong)
                        tp = bt_unsignedlonglong;
                    else
                #endif 
                    tp = bt_unsigned/*long*/;
                getsym();
            }
            if (lastst == kw_int)
                getsym();
            #ifdef LONGS
                if (tp == bt_longlong || tp == bt_unsignedlonglong)
                    head = tail = maketype(tp, stdlonglongsize);
                else
            #endif 
                head = tail = maketype(tp, stdintsize);
            break;
        case kw_double:
            getsym();
            if (lastst == kw__Complex)
            {
                getsym();
                head = tail = maketype(bt_rcomplex, stddoublesize * 2);
            }
            else if (lastst == kw__Imaginary)
            {
                getsym();
                head = tail = maketype(bt_rimaginary, stddoublesize);
            }
            else
                head = tail = maketype(bt_double, stddoublesize);
            break;
        case kw__Complex:
            getsym();
            if (lastst == kw_float)
            {
                getsym();
                head = tail = maketype(bt_fcomplex, stdfloatsize * 2);
            }
            else if (lastst == kw_double)
            {
                getsym();
                head = tail = maketype(bt_rcomplex, stddoublesize * 2);
            }
            else if (lastst == kw_long)
            {
                getsym();
                if (lastst == kw_double)
                {
                    getsym();
                    head = tail = maketype(bt_lrcomplex, stdldoublesize * 2);
                }
                else
                    generror(ERR_INCOMPATTYPECONV, 0, 0);
            }
            else
                generror(ERR_TYPENAMEEXP, 0, 0);
            break;
        case kw__Imaginary:
            getsym();
            if (lastst == kw_float)
            {
                getsym();
                head = tail = maketype(bt_fimaginary, stdfloatsize);
            }
            else if (lastst == kw_double)
            {
                getsym();
                head = tail = maketype(bt_rimaginary, stddoublesize);
            }
            else if (lastst == kw_long)
            {
                getsym();
                if (lastst == kw_double)
                {
                    getsym();
                    head = tail = maketype(bt_lrimaginary, stdldoublesize);
                }
                else
                    generror(ERR_INCOMPATTYPECONV, 0, 0);
            }
            else
                generror(ERR_TYPENAMEEXP, 0, 0);
            break;
        case kw_enum:
            getsym();
            declenum(table, cppflags);
            break;
        case kw_struct:
            getsym();
            declstruct(table, bt_struct, flags | DF_PUBLIC, cppflags);
            typequal = 0;
            break;
        case kw_union:
            getsym();
            declstruct(table, bt_union, flags, cppflags);
            typequal = 0;
            break;
        case kw_typeof:
            getsym();
            if (lastst == openpa)
            {
                opened = TRUE;
                getsym();
            }

            if (lastst == id)
            {
                typesp = typesearch(lastid);
                if (!typesp || typesp->storage_class == sc_namespace || typesp
                    ->istemplate || typesp->istemplatevar)
                {
                    generror(ERR_INVALIDARGTOTYPEOF, 0, 0);
                    head = tail = maketype(bt_int, stdintsize);
                }
                else
                    head = tail = copytype(typesp->tp, 0);
                getsym();
            }
            else
            {
                if (castbegin(lastst))
                {
                    decl(table, 0); /* do cast declaration */
                    headptr = &head;
                    decl1(sc_type, 0);
                }
                else
                {
                    generror(ERR_INVALIDARGTOTYPEOF, 0, 0);
                    head = tail = maketype(bt_int, stdintsize);
                }
            }
            if (opened)
                needpunc(closepa, skm_declend);
            break;
    }
    exit: head->cflags |= flags;
    head->uflags |= UF_CANASSIGN;
    return cppflags;
}

//-------------------------------------------------------------------------

SYM *makesym(enum e_sc sc)
{
    SYM *sp = xalloc(sizeof(SYM));
    sp->storage_class = sc;
    sp->mainsym = sp;
    sp->dontlist = sys_inc;
    return sp;
} 
    void overloadoperator(enum e_sc sc, SYM *sp)
    {
        char *q = lastid;
        int aftertype = 0;
        getsym(); /* past the operator keyword */
        if (castbegin(lastst))
        {
            aftertype = 1;
            decl(0, 0); /* do cast declaration */
            headptr = &head;
            decl1(sc_type, 0);
            if (head->type == bt_func || head->type == bt_ifunc)
            {
                backup(openpa);
            } 
            else
                goto error;
        }
        if (lastst >= id)
        {
            error: generror(ERR_OPERATOREXPECTED, 0, skm_declend);
            return ;
        }
        else
        {
            sp->value.i = lastst + IT_OV_THRESHOLD;
            sp->value.classdata.cppflags |= PF_OPERATOR;
            getsym(); /* past the operator */
            /* special case operator () and [] since they are two symbols */
            switch (sp->value.i)
            {
                case ov_ind:
                    if (lastst != closebr)
                        goto error;
                    if (aftertype)
                        goto error;
                    getsym();
                    decl2(sc, sp);
                    break;
                case ov_new:
                    sp->value.classdata.cppflags |= PF_STATIC;
                    if (aftertype)
                        goto error;
                    if (lastst == openbr)
                    {
                        sp->value.i = ov_newa;
                        getsym();
                        needpunc(closebr, 0);
                    }
                    decl2(sc, sp);
                    break;
                case ov_arg:
                    break;
                case ov_delete:
                    sp->value.classdata.cppflags |= PF_STATIC;
                    if (aftertype)
                        goto error;
                    if (lastst == openbr)
                    {
                        sp->value.i = ov_deletea;
                        getsym();
                        needpunc(closebr, 0);
                    }
                    decl2(sc, sp);
                    break;
                default:
                    if (aftertype)
                        goto error;
                    decl2(sc, sp);
                    break;
            }
            /* construct the function name */
            /*		if (prm_cmangle)
             *q++ = '_';
             */
            /* special case name for overloaded cast operators */

            if (sp->value.i == ov_arg)
            {
                int temp1 = head->type;
                int temp2 = head->btp->cflags;
                head->btp->cflags = 0;
                q[0] = '$';
                q[1] = 'o';
                cpponearg(q + 2, head->btp);
                head->btp->cflags = temp2;
            }
            else
                strcpy(q, cpp_funcname_tab[sp->value.i]);
            strcpy(declid, lastid);
        }

        return ;
    }
SYM *decl1(enum e_sc sc, SYM *sp)
/*
 * Modifiers that could occur BEFORE the name of the var
 */
{
    TYP *temp1,  *temp2,  *temp3,  *temp4,  *head1 =  *headptr;
    TYP **tempptr;
    SYM *typesp = 0;
    int ocppf;
    int special = FALSE;
    if (!sp)
        sp = makesym(sc);

    lp: switch (lastst)
    {
        case kw_inline:
            if (!prm_cplusplus && !prm_c99)
            {
                lastst = id;
                goto getid;
            }
            cppflags |= PF_INLINE;
            getsym();
            goto lp;
            case kw_virtual:
                if (!prm_cplusplus)
                {
                    lastst = id;
                    goto getid;
                }
                cppflags |= PF_VIRTUAL;
                getsym();
                goto lp;
            case kw_static:
                cppflags |= PF_STATIC;
                getsym();
                goto lp;
            case kw_friend:
                if (!prm_cplusplus)
                {
                    lastst = id;
                    goto getid;
                }
                getsym();
                aFriend = TRUE;
                goto lp;
        case kw__seg:
            #ifdef i386
                segflag = TRUE;
                head1->cflags |= DF_SEG;
            #endif 
            getsym();
            goto lp;
        case kw__far:
            #ifdef i386
                if (prm_farkeyword)
                {
                    farflag = TRUE;
                    head1->cflags |= DF_FAR;
                }
            #endif 
        case kw__near:
            getsym();
            goto lp;
        case kw__interrupt:
            intflag = 1;
            sp->intflag = 1;
            /// head1->cflags |= DF_INT;
            getsym();
            goto lp;
        case kw__fault:
            intflag = 1;
            sp->faultflag = 1;
            // head1->cflags |= DF_FAULT;
            getsym();
            goto lp;
        case kw_const:
            head1->cflags |= DF_CONST;
            getsym();
            goto lp;
        case kw_restrict:
            if (head1->type != bt_pointer)
            {
                errlineno = lineno;
                generror(ERR_RESTRICTPTR, 0, 0);
            }
            head1->cflags |= DF_RESTRICT;
            getsym();
            goto lp;
        case kw__intrinsic:
            head1->cflags |= DF_INTRINS;
            getsym();
            goto lp;
        case kw_volatile:
            head1->cflags |= DF_VOL;
            getsym();
            goto lp;
        case kw__stdcall:
            isstdcall = TRUE;
            getsym();
            goto lp;
        case kw__indirect:
            if (prm_microlink)
                isindirect = TRUE;
            getsym();
            goto lp;
        case kw__pascal:
            ispascal = TRUE;
            getsym();
            goto lp;
        case kw_explicit:
            cppflags |= PF_EXPLICIT;
            getsym();
            goto lp;
        case kw__export:
            isexport = TRUE;
            getsym();
            goto lp;
        case kw__import:
            isimport = TRUE;
            importname = 0;
            getsym();
            if (lastst == openpa)
            {
                getsym();
                if (lastst == sconst)
                {
                    importname = litlate(laststr);
                    getsym();
                    needpunc(closepa, 0);
                }
                else
                    generror(ERR_NEEDCHAR, '"', 0);
            }
            goto lp;
        case kw__cdecl:
            iscdecl = TRUE;
            getsym();
            goto lp;
            case compl:
                getsym();
                if (lastst == id)
                {
                    getsym();
                    if (lastst == lt)
                    {
                        SYM *tcq = typequal;
                        typequal = 0;
                        typesp = typesearch(lastid);
                        typequal = tcq;
                        if (typesp)
                        {
                            typesp = lookupTemplateType(typesp,cppflags);
                            if (typesp)
                                goto joincompl;
                        }
                    }
                    else
                    {
                        if (typequal)
                            typesp = typequal;
                        else if (declclass)
                            typesp = declclass;
                        else
                            typesp = 0;
                        if (typesp && !strcmp(typesp->name, lastid))
                        {
                            joincompl: if (!defaulttype || currentfunc)
                                gensymerror(ERR_CANTHAVETYPE, declclass->name);
                            strcpy(declid, cpp_funcname_tab[CI_DESTRUCTOR]);
                            cppflags |= PF_DESTRUCTOR;
                            typesp->value.classdata.cppflags |= PF_HASDEST;
                            head = tail = maketype(bt_void, 0);
                            decl2(sc, sp);
                            break;
                        }
                    }
                }
                if (typesp)
                    gensymerror(ERR_BADESTRUCT, typesp->name);
                else
                    gensymerror(ERR_BADESTRUCT, lastid);
                break;
            case kw_operator:
                overloadoperator(sc, sp);
                break;
            case classsel:
                 /* shouldn't get here */
            case classselstar:
                break;
        case id:
            if (declid[0])
                generror(ERR_EXTRAIDENT,0,0);
            getid: strcpy(declid, lastid);
            getsym();

                if (prm_cplusplus)
                {
                    if (lastst == lt)
                    {
                        SYM *tcq = typequal;
                        special = TRUE;
                        typequal = 0;
                        if (!(typesp = typesearch(lastid)) || typesp
                            ->storage_class != sc_type || !typesp->istemplate)
                        {
                            generror(ERR_INVALIDTEMPLATE, 0, 0);
                            tplskm(skm_tempend);
                        }
                        else
                        {
                            typequal = tcq;
                            typesp = lookupTemplateType(typesp,cppflags);
                            if (typesp == typequal)
                            {
                                if (lastst == openpa)
                                {
                                    if (!defaulttype || currentfunc)
                                        gensymerror(ERR_CANTHAVETYPE, typequal 
                                            ? typequal->name: declclass->name);
                                    strcpy(declid,
                                        cpp_funcname_tab[CI_CONSTRUCTOR]);
                                    cppflags |= PF_CONSTRUCTOR;
                                    head = tail = maketype(bt_void, 0);
                                    decl2(sc, sp);
                                    break;
                                }
                            }
                            if (lastst != classsel && lastst != classselstar)
                            {
                                generror(ERR_INVALIDTEMPLATE, 0, 0);
                                tplskm(skm_tempend);
                            }
                            else
                                if (typesp && typesp->storage_class == sc_type)
                                    goto tempjn;
                        }
                    }
                    if (lastst == classsel || lastst == classselstar)
                    {
                        if ((typesp = typesearch(lastid)) && (typesp
                            ->storage_class == sc_type || typesp->storage_class
                            == sc_namespace))
                        {
                            tempjn: if (!special && typesp->istemplate)
                                gensymerror(ERR_NEEDSPECIAL, typesp->name);
                            typesp = parsetype(typesp, TRUE);
                            if (lastst == classsel && declclass)
                            {
                                if (typequal != declclass)
                                {
                                    gensymerror(ERR_NOTYPEQUAL, lastid);
                                    basicskim(skm_declend);
                                    break;
                                }
                                else
                                {
                                    typequal = 0;
                                }
                            }
                            if (lastst == id)
                            {
                                strcpy(declid, lastid);
                                getsym();
                            }
                        }
                        else
                        {
                            gensymerror(ERR_NOTNSORTYPQUAL, lastid);
                            basicskim(skm_declend);
                            break;
                        }
                    }
                    if (lastst != classselstar && (((typequal && (!declclass ||
                        typequal == declclass)) || declclass && !typequal)))
                    {
                        if (typequal && !strcmp(lastid, typequal->name) ||
                            declclass && !strcmp(lastid, declclass->name))
                        {
                            if (lastst == id)
                                getsym();
                            if (lastst == openpa)
                            {
                                if (!defaulttype || currentfunc)
                                    gensymerror(ERR_CANTHAVETYPE, typequal ?
                                        typequal->name: declclass->name);
                                strcpy(declid, cpp_funcname_tab[CI_CONSTRUCTOR])
                                    ;
                                cppflags |= PF_CONSTRUCTOR;
                                if (declclass)
                                    declclass->value.classdata.cppflags |=
                                        PF_HASCONS;
                                head = tail = maketype(bt_void, 0);
                                decl2(sc, sp);
                                break;
                            }
                            goto lp;
                        }
                    }
                    if (lastst == classselstar)
                    {
                        getsym();
                        if (!typequal || typequal->tp->type != bt_class &&
                            typequal->tp->type != bt_struct)
                        {
                            generror(ERR_TYPEQUALEXP, 0, 0);
                        }
                        else
                        {
                            if (lastst != id)
                                generror(ERR_IDEXPECT, 0, 0);
                            else
                            {
                                strcpy(declid, lastid);
                                getsym();
                                memberptr = TRUE;
                                temp1 = maketype(bt_memberptr, stdmemberptrsize)
                                    ;
                                temp1->btp = head;
                                temp1->sp = typequal;
                                *headptr = temp1;
                                typequal = 0;
                            }
                        }
                        if (lastst == id)
                        {
                            strcpy(declid, lastid);
                            getsym();
                        }
                    }
                    else
                    if (lastst == kw_operator)
                    {
                        overloadoperator(sc, sp);
                        break;
                    }
                    else
                        if (typesp && typesp->storage_class == sc_type)
                    if (!strcmp(typesp->name, lastid))
                    {
                        if (!defaulttype || currentfunc)
                                gensymerror(ERR_CANTHAVETYPE, typesp->name);
                        strcpy(declid, cpp_funcname_tab[CI_CONSTRUCTOR]);
                        decl2(sc, sp);
                        break;
                    }
                    else
                        generror(ERR_IDEXPECT, 0, 0);

                }
            decl2(sc, sp);
            break;
        case colon:
            sprintf(declid, "_$$NULLNAME%d", nullnamecount++);
            decl2(sc, sp);
            break;
        case and:
                if (prm_cplusplus)
                {
                    getsym();
                    temp1 = maketype(bt_ref, stdaddrsize);
                    temp1->btp = head1;
                    *headptr = temp1;
                    if (tail == NULL)
                        tail = head1;
                    decl1(sc, sp);
                }
                else
            {
                decl2(sc, sp);
            }
            break;
        case star:
            head1->uflags &= ~UF_CANASSIGN;
            #ifdef i386
                if (farflag)
                    temp1 = maketype(bt_farpointer, stdaddrsize + stdshortsize);
                else if (segflag)
                    temp1 = maketype(bt_segpointer, stdshortsize);
                else
					if (prm_fardefault)
	                    temp1 = maketype(bt_farpointer, stdaddrsize + stdshortsize);
					else
            #endif 
                temp1 = maketype(bt_pointer, stdaddrsize);
            farflag = segflag = 0;
            temp1->btp = head1;
            *headptr = temp1;
            head1->uflags |= UF_CANASSIGN;
            if (tail == NULL)
                tail = head1;
            getsym();
            decl1(sc, sp);
            break;
        case openpa:
            if (nodeclfunc)
                break;
            getsym();
            if (declclass && head->type == bt_consplaceholder)
            {
                if (!defaulttype || currentfunc)
                    gensymerror(ERR_CANTHAVETYPE, declclass->name);
                strcpy(declid, cpp_funcname_tab[CI_CONSTRUCTOR]);
                cppflags |= PF_CONSTRUCTOR;
                declclass->value.classdata.cppflags |= PF_HASCONS;
                head = tail = maketype(bt_void, 0);
                backup(openpa);
            }
            else if (lastst == and && prm_cplusplus)
            {
                getsym();
                decl1(sc, sp);
                needpunc(closepa, 0);
                decl2(sc, sp);
                temp1 = maketype(bt_ref, stdaddrsize);
                temp1->btp =  *headptr;
                *headptr = temp1;
                if (tail == NULL)
                    tail = head1;
            }
            else if (lastst == star)
            {
starjn:
                decl1(sc, sp);
                if ((*headptr)->btp)
                {
                    for (temp1 =  *headptr; temp1->btp && temp1->btp != head1;
                        temp1 = temp1->btp)
                        ;
                    headptr = &temp1->btp;
                }
                else
                    if (head->type == bt_memberptr)
                        headptr = &head->btp;
                    else
                        headptr = &head;
                needpunc(closepa, 0);
            }
            else if (lastst == kw_const || lastst == kw_volatile ||
                lastst == kw__far || lastst == kw__stdcall ||
                lastst == kw__pascal || lastst == kw__cdecl ||
                lastst == kw__seg)
            {  /* type *(qualif func)() */
                int olastst = lastst, nlastst;
                getsym();
                nlastst = lastst;
                backup(olastst);
                if (nlastst == star)
                    goto starjn;
                decl1(sc, sp);
                needpunc(closepa, 0);
            }
            else if (prm_cplusplus && lastst == id && (typesp = typesearch
                (lastid)))
            {
                getsym();
                if (lastst == lt)
                {
                    if (!(typesp = typesearch(lastid)) || typesp->storage_class
                        != sc_type || !typesp->istemplate)
                    {
                        generror(ERR_INVALIDTEMPLATE, 0, &skm_tempend);
                        tplskm(skm_tempend);
                    }
                    else
                    {
                        typesp = lookupTemplateType(typesp,cppflags);
                        if (lastst != classsel && lastst != classselstar)
                        {
                            generror(ERR_INVALIDTEMPLATE, 0, 0);
                            tplskm(skm_tempend);
                        }
                    }
                }
                typesp = parsetype(typesp, TRUE);
                if (lastst == id)
                {
                    decl1(sc, sp);
                    needpunc(closepa, &skm_declend);
                    if (head->type == bt_func)
                    {
                        if (prm_cplusplus && lastst == colon || lastst == begin
                            || castbegin(lastst) || lastst == kw_register ||
                            lastst == kw_const || lastst == kw_volatile ||
                            lastst == kw_restrict)
                            head->type = bt_ifunc;
                    }
                    break;
                }
                else if (lastst != classselstar)
                {
                    generror(ERR_EXPMEMBERPTRFUNC, 0, skm_declend);
                    break;
                }
                else
                {
                    getsym();
                    if (lastst == id)
                    {
                        strcpy(declid, lastid);
                        getsym();
                    }
                    else
                        declid[0] = 0;
                    memberptr = TRUE;
                    temp1 = maketype(bt_memberptr, stdmemberptrsize);
                    temp1->btp = head; 
                        // will be overwritten if they define the thing right
                    temp1->sp = typequal;
                    *headptr = temp1;
                    decl1(sc, sp);
                    if ((*headptr)->btp)
                    {
                        for (temp1 =  *headptr; temp1->btp && temp1->btp !=
                            head1; temp1 = temp1->btp)
                            ;
                        headptr = &temp1->btp;
                    }
                    else
                        if (head->type == bt_memberptr)
                            headptr = &head->btp;
                        else
                            headptr = &head;
                    typequal = 0;
                    needpunc(closepa, skm_declend);
                }
            }
            else if (lastst == id && !castbegin(lastst))
            {
                decl1(sc, sp);
                needpunc(closepa, skm_declend);
				if (lastst == openpa)
					decl2(sc, sp);
				else
	                goto lp;
                break;
            }
            else
            {
                backup(openpa);
            }
            decl2(sc, sp);
            break;
        default:
            if (inprototype)
                decl2(sc, sp);
            break;
    }
    return sp;
}

//-------------------------------------------------------------------------

    /* Some overloaded functions require a member of type class */
    int isclassarg(SYM *sp)
    {
        SYM *head = sp->tp->lst.head;
        while (head && head != (SYM*) - 1)
        {
            TYP *tp = head->tp;
            while (tp->type == bt_ref)
                tp = tp->btp;
            if (isstructured(tp) || tp->type == bt_templateplaceholder)
                return TRUE;
            head = head->next;
        }
        return FALSE;
    }
    /* Depending on the type of function being overloaded, there are
     * different error checks to be performed
     */
    void CheckOverloadedArgs(SYM *sp)
    {
        int argcount = 0;
        SYM *head = sp->tp->lst.head;
        while (head && head != (SYM*) - 1)
        {
            argcount++;
            head = head->next;
        }
        switch (sp->value.i)
        {
            case ov_new:
            case ov_newa:

                if (!sp->tp->lst.head || sp->tp->lst.head->tp->type !=
                    bt_unsigned)
                    generror(ERR_SIZE_T_NEW, 0, 0);
                //            else
                //               if (sp->tp->btp->type != bt_pointer && sp->tp->type != bt_farpointer &&sp->tp->btp->type != bt_segpointer || sp->tp->btp->btp->type != bt_void)
                //                  generror(ERR_VOIDSTAR_NEW,0,0);
                break;
            case ov_delete:
            case ov_deletea:
                //     if (argcount != 1)
                //           gensymerror(ERR_DECL1ARG,sp->name);
                //        else
                if (!sp->tp->lst.head || sp->tp->lst.head->tp->type !=
                    bt_pointer && sp->tp->lst.head->tp->type != bt_farpointer
                    && sp->tp->btp->type != bt_segpointer || sp->tp->lst.head
                    ->tp->btp->type != bt_void)
                    generror(ERR_VOIDSTAR_DELETE, 0, 0);
                else
                    if (sp->tp->btp->type != bt_void)
                        generror(ERR_VOID_DELETE, 0, 0);
                break;
            case ov_div:
            case ov_shl:
            case ov_shr:
            case ov_mod:
            case ov_lor:
            case ov_land:
            case ov_or:
            case ov_xor:
            case ov_equ:
            case ov_neq:
            case ov_lt:
            case ov_le:
            case ov_gt:
            case ov_ge:
            case ov_asn:
            case ov_aasadd:
            case ov_assub:
            case ov_asmul:
            case ov_asdiv:
            case ov_asmod:
            case ov_asshl:
            case ov_asshr:
            case ov_asand:
            case ov_asor:
            case ov_asxor:
                if (typequal || declclass)
                {
                    if (argcount != 1)
                        gensymerror(ERR_DECL1ARG, sp->name);
                }
                else
                    if (argcount != 2)
                        gensymerror(ERR_DECL2ARG, sp->name);
                    else
                        if (!isclassarg(sp))
                            gensymerror(ERR_OPERATORMUSTCLASS, sp->name);
                break;

            case ov_and:
            case ov_mul:
            case ov_add:
            case ov_sub:
                if (typequal || declclass)
                {
                    if (argcount > 1)
                        gensymerror(ERR_DECL1ARG, sp->name);
                }
                else
                    if (argcount != 1 && argcount != 2)
                        gensymerror(ERR_DECL2ARG, sp->name);
                    else
                        if (!isclassarg(sp))
                            gensymerror(ERR_OPERATORMUSTCLASS, sp->name);
                break;
            case ov_autoinc:
            case ov_autodec:
                if (typequal || declclass)
                {
                    if (argcount > 1)
                        gensymerror(ERR_DECL0OR1ARG, sp->name);
                    else if (argcount && sp->tp->lst.tail->tp->type != bt_int)
                        gensymerror(ERR_OVPOSTFIXINTLAST, sp->name);
                }
                else
                    if (argcount > 2 || argcount < 1)
                        gensymerror(ERR_DECL1OR2ARG, sp->name);
                    else
                {
                    if (!isclassarg(sp))
                            gensymerror(ERR_OPERATORMUSTCLASS, sp->name);
                        else if (argcount == 2 && sp->tp->lst.tail->tp->type !=
                            bt_int)
                            gensymerror(ERR_OVPOSTFIXINTLAST, sp->name);
                }
                break;
            case ov_cpl:
            case ov_not:
                if (typequal || declclass)
                {
                    if (argcount)
                        gensymerror(ERR_DECL0ARG, sp->name);
                }
                else
                    if (argcount != 1)
                        gensymerror(ERR_DECL1ARG, sp->name);
                    else
                        if (!isclassarg(sp))
                            gensymerror(ERR_OPERATORMUSTCLASS, sp->name);
                break;

            case ov_ind:
                if (!typequal && !declclass)
                    gensymerror(ERR_OPMUSTBEMEMBER, sp->name);
                else
                    if (argcount != 1)
                        gensymerror(ERR_DECL1ARG, sp->name);
                break;
            case ov_arg:
                if (!typequal && !declclass)
                    gensymerror(ERR_OPMUSTBEMEMBER, sp->name);
                else
                    if (argcount)
                        gensymerror(ERR_DECL0ARG, sp->name);
                break;
            case ov_pstar:
            case ov_arr:
                if (!typequal && !declclass)
                    gensymerror(ERR_OPMUSTBEMEMBER, sp->name);
                else
                    if (argcount)
                        gensymerror(ERR_DECL0ARG, sp->name);
                    else
                {
                    TYP *tp = sp->tp->btp;
                    if (tp->type != bt_segpointer && tp->type != bt_pointer &&
                        tp->type != bt_farpointer || !isstructured(tp->btp))
                            gensymerror(ERR_POINTTOCLASS, sp->name);
                }
                break;
        }
    }
    ENODE *do_constructor(SYM *sp_in, TYP *tp, TYP *head1, ENODE *node, int
        size, int offset, int assn, int implicit)
    {
        char buf[256];
        int voidfunc = FALSE;
        SYM *sp;
        ENODE *pnode,  *rnode,  *tcnode = 0;
        if (incatchclause)
            return 0;
        if (tp->lst.head == (SYM*) - 1)
            voidfunc = TRUE;
        {
            SYM *sp1 = search(cpp_funcname_tab[CI_CONSTRUCTOR], &head1->lst);
            if (!sp1)
            {
                if (!voidfunc && !assn)

                    gensymerrorexp(ERR_NOFUNCMATCH, fullcppmangle(head1->sp,
                        cpp_funcname_tab[CI_CONSTRUCTOR], tp));
                return 0;
            }
            sp = funcovermatch(sp1, tp, node, FALSE, FALSE);
            if (!sp)
            {
                if (!assn)
                    gensymerrorexp(ERR_NOFUNCMATCH, fullcppmangle(head1->sp,
                        cpp_funcname_tab[CI_CONSTRUCTOR], tp));
                return 0;
            }
            pnode = makenode(en_napccon, sp, 0);
        }
        if (!isaccessible(sp))
            genclasserror(ERR_NOTACCESSIBLE, fullcppmangle(sp->parentclass
                ->parentclass, sp->name, 0));
        if (implicit && (sp->value.classdata.cppflags &PF_EXPLICIT))
            generror(ERR_NOEXPLICITHERE, 0, 0);
        sp->mainsym->extflag = TRUE;
        parmlist(&node, tp, sp->tp);
        rnode = makenode(en_void, pnode, pnode);
        pnode = makenode(en_void, rnode, node);
        rnode = makeintnode(en_icon, sp->tp->btp->size);
        if (sp->pascaldefn)
            pnode = makenode(en_pfcall, rnode, pnode);
        else
            if (sp->isstdcall)
                pnode = makenode(en_sfcall, rnode, pnode);
            else
                pnode = makenode(en_fcall, rnode, pnode);
        if (!(sp->value.classdata.cppflags &PF_STATIC))
            if (sp_in->storage_class == sc_type)
                tcnode = makenode(en_a_ref, copynode(thisenode), 0);
            else if (sp_in->storage_class == sc_auto)
                tcnode = makenode(en_autocon, sp_in, 0);
            else if (sp_in->storage_class == sc_autoreg)
                tcnode = makenode(en_autoreg, sp_in, 0);
            else
                tcnode = makenode(en_nacon, sp_in, 0);
        if (offset)
            tcnode = makenode(en_addstruc, (void*)tcnode, makeintnode(en_icon,
                offset));
            if (prm_cplusplus && tcnode && (sp->value.classdata.cppflags
                &PF_MEMBER) && !(sp->value.classdata.cppflags &PF_STATIC))
            {
                pnode = makenode(en_thiscall, tcnode, pnode);
            }
            if (size == 1)
                pnode = doinline(pnode);
            else
            {
                ENODE *snode;
                snode = makeintnode(en_icon, head1->size);
                snode = makenode(en_void, makeintnode(en_icon, size), (void*)
                    snode);
                pnode = makenode(en_repcons, (void*)snode, (void*)pnode);
                SetExtFlag(sp, TRUE);
            }
            pnode->cflags = head1->cflags;
            pnode = makenode(en_conslabel, pnode, makenode(en_autocon, (void*)
                sp_in->mainsym, 0));
        conscount++;
        sp_in->mainsym->constructed = 1 ;
        return pnode;

    }
    ENODE *do_destructor(SYM *sp_in, TYP *tp, TYP *head1, ENODE *xnode, int
        size, int offset, int assn)
    {
        char buf[256];
        ENODE *node = 0;
        SYM *sp;
        ENODE *pnode,  *rnode,  *tcnode = 0;
        if (incatchclause)
            return 0;
        {
            SYM *sp1 = search(cpp_funcname_tab[CI_DESTRUCTOR], &head1->lst);
            if (!sp1)
            {
                return 0;
            }
            sp = funcovermatch(sp1, tp, xnode, FALSE, FALSE);
            if (!sp)
            {
                if (!assn)
                    gensymerrorexp(ERR_NOFUNCMATCH, fullcppmangle(tp->sp,
                        cpp_funcname_tab[CI_DESTRUCTOR], tp));
                return 0;
            }
            pnode = makenode(en_napccon, sp, 0);
        }
        sp->mainsym->extflag = TRUE;
        if (xnode)
            tcnode = xnode;
        else
        {
            if (!(sp->value.classdata.cppflags &PF_STATIC))
                if (sp_in->storage_class == sc_auto)
                    tcnode = makenode(en_autocon, sp_in, 0);
                else if (sp_in->storage_class == sc_autoreg)
                    tcnode = makenode(en_autoreg, sp_in, 0);
                else
                    tcnode = makenode(en_nacon, sp_in, 0);
            if (offset)
                tcnode = makenode(en_addstruc, (void*)tcnode, makeintnode
                    (en_icon, offset));
        }
        if (!isaccessible(sp))
            genclasserror(ERR_NOTACCESSIBLE, fullcppmangle(sp->parentclass
                ->parentclass, sp->name, 0));
        if (sp->value.classdata.cppflags &PF_VIRTUAL)
        {
            ENODE *inode,  *nnode;
            inode = makeintnode(en_icon, sp->value.classdata.vtabindex);
            if (sp->value.classdata.vtaboffs)
                tcnode = makenode(en_add, tcnode, makeintnode(en_icon, sp
                    ->value.classdata.vtaboffs));
            nnode = makenode(en_a_ref, (void*)copy_enode(tcnode), 0);
            pnode = makenode(en_add, inode, nnode);
            pnode = makenode(en_a_ref, pnode, 0);
        }
        parmlist(&node, tp, sp->tp);
        rnode = makenode(en_void, pnode, pnode);
        pnode = makenode(en_void, rnode, node);
        rnode = makeintnode(en_icon, sp->tp->btp->size);
        if (sp->pascaldefn)
            pnode = makenode(en_pfcall, rnode, pnode);
        else
            if (sp->isstdcall)
                pnode = makenode(en_sfcall, rnode, pnode);
            else
                pnode = makenode(en_fcall, rnode, pnode);
            if (prm_cplusplus && tcnode && (sp->value.classdata.cppflags
                &PF_MEMBER) && !(sp->value.classdata.cppflags &PF_STATIC))
            {
                pnode = makenode(en_thiscall, tcnode, pnode);
            }
            if (size == 1)
                pnode = doinline(pnode);
            else
            {
                ENODE *snode;
                snode = makeintnode(en_icon, head1->size);
                snode = makenode(en_void, makeintnode(en_icon, size), (void*)
                    snode);
                pnode = makenode(en_repcons, (void*)snode, (void*)pnode);
                SetExtFlag(sp, TRUE);
            }
            pnode = makenode(en_destlabel, pnode, makenode(en_autocon, (void*)
                sp_in->mainsym, 0));
        pnode->cflags = head1->cflags;
        conscount++;
        return pnode;

    }
    ENODE **actualexpr(ENODE **qnode)
    {
        ENODE **rv;
        if (! *qnode)
            return 0;
        if ((*qnode)->nodetype == en_void || ((*qnode)->nodetype == en_dvoid))
        {
            if ((*qnode)->cflags &DF_ACTUALEXPR)
                return qnode;
            rv = actualexpr(&(*qnode)->v.p[0]);
            if (rv)
                return rv;
            rv = actualexpr(&(*qnode)->v.p[1]);
            if (rv)
                return rv;
        }
        return 0;
    }
    int conspair(SYM *sp, enum e_sc al, int flag, TYP *tp1, ENODE *qnode, int
        assn, int offs, int implicit)
    {
        ENODE **actual = actualexpr(&qnode),  *qn1,  *qnode1;
        TYP tp2,  *tp3 =  *headptr;
        SYM *sp2;
        ENODE *en2,  *en3;
        int size = 1;
        if (infuncargs && !indefaultinit)
            return 1;
        if (indefaultinit)
        {
            sp->value.classdata.defalt = xalloc(sizeof(DEFVALUE));
            en2 = dummyvar(head->size, head, 0);
            sp2 = en2->v.sp;
            //                                          en3 = makenode(en_autocon,(void *)sp,0) ;
            //                                          en3 = makenode(en_moveblock,en3,en2) ;
            //                                          en3->size = head->size ;
            sp->value.classdata.defalt->ep = en2;
        } 
        else
            sp2 = sp;
        if (flag)
        {
            while (tp3->type == bt_pointer || tp3->type == bt_farpointer || tp3
                ->type == bt_segpointer)
            {
                size = decl3size;
                tp3 = tp3->btp;
            }
        }
        expr_runup[expr_updowncount] = 0;
        expr_rundown[expr_updowncount++] = 0;
        qn1 = do_constructor(sp2, tp1, tp3, actual ? (*actual): qnode, flag < 0
            ?  - flag: size, offs, assn, implicit);
        expr_updowncount--;
        if (expr_runup[expr_updowncount] || expr_rundown[expr_updowncount])
        {
            qn1 = makenode(en_dvoid, qn1, expr_rundown[expr_updowncount]);
            (qn1)->cflags |= DF_ACTUALEXPR;
        }
        if (expr_runup[expr_updowncount])
            qn1 = makenode(en_void, expr_runup[expr_updowncount], qn1);
        if (actual)
            (*actual)->v.p[0] = qn1;
        else
            qnode = qn1;
        tp2.lst.head =  - 1;
        tp2.lst.tail =  - 1;
        tp2.type = bt_func;
        qnode1 = do_destructor(sp2, &tp2, tp3, 0, flag < 0 ?  - flag: size,
            offs, FALSE);
        if (qnode1 && !qnode)
            qnode = makenode(en_conslabel, makeintnode(en_icon, 0), makenode
                (en_autocon, (void*)sp2->mainsym, 0));

        if (qnode)
        {
            if (indefaultinit)
            {
                sp->value.classdata.defalt->cons = qnode;
            }
            else if (al == sc_auto)
            {
                SNODE *snp = xalloc(sizeof(SNODE));
                snp->next = 0;
                snp->stype = st_expr;
                snp->exp = qnode;
                if (cbautoinithead == 0)
                    cbautoinithead = cbautoinittail = snp;
                else
                {
                    cbautoinittail->next = snp;
                    cbautoinittail = snp;
                }
            }
            else
            {
                cppinitinsert(qnode);
            }
        }
        if (qnode && !qnode1)
            qnode1 = makenode(en_destlabel, makeintnode(en_icon, 0), makenode
                (en_autocon, (void*)sp2->mainsym, 0));
        if (qnode1)
        {
            if (indefaultinit)
            {
                sp->value.classdata.defalt->dest = makenode(en_destlabel,
                    makeintnode(en_icon, 0), makenode(en_autocon, (void*)sp2
                    ->mainsym, 0));
            }
            else if (al == sc_auto)
            {
                if (block_rundown)
                    block_rundown = makenode(en_void, qnode1, block_rundown);
                else
                    block_rundown = qnode1;
            }
            else
            {
                cpprundowninsert(qnode1);
            }
        }
        return 1;
    }
void decl2(enum e_sc sc, SYM *sp)
/*
 * type modifiers that come AFTER the name of the variable
 */
{
    TYP *temp1,  *head1 =  *headptr;
    TYP *otc = typequal;
    int ocppf;
    int odflt;
    int otemp;
    char localid[128];
    typequal = 0;
    if (prm_cplusplus)
    {
        temp1 = head;
        while (temp1->type == bt_ref || temp1->type == bt_pointer || temp1
            ->type == bt_farpointer || temp1->type == bt_segpointer)
        {
            if (temp1->btp->type == bt_ref)
            {
                if (temp1->val_flag)
                    generror(ERR_NOREFARRAY, 0, 0);
                else
                    generror(ERR_CANTREF, 0, 0);
                break;
            } temp1 = temp1->btp;
        }
    }
    lp: 

    switch (lastst)
    {
        case kw_const:
            head1->cflags |= DF_CONST;
            getsym();
            goto lp;
        case kw_restrict:
            if (head->type != bt_pointer)
            {
                errlineno = lineno;
                generror(ERR_RESTRICTPTR, 0, 0);
            }
            head1->cflags |= DF_RESTRICT;
            getsym();
            goto lp;
        case kw__intrinsic:
            head1->cflags |= DF_INTRINS;
            getsym();
            goto lp;
        case kw_volatile:
            head1->cflags |= DF_VOL;
            getsym();
            goto lp;
        case openbr:
                if (head->type == bt_func) {
                    generror(ERR_FUNCBADRET,0,0);
                    expskim(skm_declend,FALSE);
                    break;
                }
            decl3(sc);
            decl2(sc, sp);
            break;
        case openpa:
                if ((*headptr)->type == bt_func) {
                    generror(ERR_FUNCBADRET,0,0);
                    expskim(skm_declend,FALSE);
                    break;
                }
            getsym();
                if (prm_cplusplus)
                {
                    if (!castbegin(lastst) && lastst != ellipse)
                    {
                        if (sc != sc_external && (sc != sc_member || head->sp
                            && head->sp->mainsym != declclass->mainsym) &&
                            isstructured(head) && lastst != closepa && (head
                            ->sp->value.classdata.cppflags &PF_CPPSTRUCT))
                        {
                            ENODE *qn;
                            TYP *tp,  *hp =  *headptr,  **oh = headptr;
                            tp = gatherparms(&qn, FALSE);
                            headptr = oh;
                            *headptr = hp;
                            conspair(sp, sc, FALSE, tp, qn, FALSE, 0, FALSE);
                            parm_ns_level--;
                            break;
                        }
                        if (lastst != closepa)
                        {
                            backup(openpa);
                            break;
                        }
                    }
                }
            temp1 = maketype(bt_func, 0);
            if (head1->cflags &DF_INTRINS)
                temp1->cflags |= DF_INTRINS;
            {
                temp1->uflags |= UF_DEFINED;
                temp1->val_flag = NORMARRAY;
                temp1->btp = head1;
                *headptr = temp1;
                ocppf = cppflags;
                strcpy(localid, declid);
                sp->name = localid;
                temp1->sp = sp;
                sp->tp = temp1;
                    sp->value.classdata.cppflags |= cppflags;
                odflt = defaulttype;
                otemp = templateFunc;
                templateFunc = 0;
                declfuncarg(intflag, farflag, sp);
                templateFunc = otemp;
                defaulttype = odflt;
                sp->name = 0;
                cppflags = ocppf;
                    if (prm_cplusplus)
                    {
                        while (lastst == kw_const || lastst == kw_volatile ||
                            lastst == kw_restrict || lastst == kw_throw)
                        {
                            if (lastst == kw_const)
                                temp1->cflags |= DF_CONST;
                            else if (lastst == kw_restrict)
                            {
                                if (temp1->type != bt_pointer)
                                {
                                    errlineno = lineno;
                                    generror(ERR_RESTRICTPTR, 0, 0);
                                }
                                temp1->cflags |= DF_RESTRICT;
                            }
                            else if (lastst == kw_throw)
                            {
                                TYP *oldhead = head,  *oldheadptr = headptr;
                                char declbuf[256];
                                strcpy(declbuf, declid);

                                getsym();
                                if (needpunc(openpa, skm_closepa))
                                {
                                    if (lastst != closepa)
                                    {
                                        while (TRUE)
                                        {
                                            LIST *l;
                                            typequal = 0;
                                            head = 0;
                                            declid[0] = 0;
                                            decl(0, 0); /* do cast declaration
                                                */
                                            headptr = &head;
                                            decl1(sc_type, 0);
                                            l = xalloc(sizeof(LIST));
                                            l->data = head;
                                            l->link = sp
                                                ->value.classdata.throwlist;
                                            sp->value.classdata.throwlist = l;
                                            //                                                         if (head && defaulttype)
                                            //                                                            generror(ERR_TYPENAMEEXP,0,0) ;
                                            if (declid[0])
                                                generror(ERR_DECLNOTALLOWED, 0,
                                                    0);
                                            if (lastst == closepa)
                                                break;
                                            if (!needpunc(comma, skm_closepa))
                                                break;
                                        }
                                    }
                                    else
                                    {
                                        sp->value.classdata.throwlist = xalloc
                                            (sizeof(LIST));
                                    }
                                    if (lastst != closepa)
                                        expecttoken(closepa, skm_closepa);
                                }
                                strcpy(declid, declbuf);
                                head = oldhead;
                                headptr = oldheadptr;

                            }
                            else
                                temp1->cflags |= DF_VOL;
                            getsym();
                        }
                        if (!declclass && !otc && (temp1->cflags &(DF_CONST |
                            DF_VOL)))
                            generror(ERR_CONSTNOMEMBER, 0, 0);
                    }

                if (lastst == kw__cdecl)
                {
                    iscdecl = TRUE;
                    getsym();
                }
                else if (lastst == kw__stdcall)
                {
                    isstdcall = TRUE;
                    getsym();
                }
                if (prm_cplusplus && lastst == colon || lastst == begin ||
                    castbegin(lastst) || lastst == kw_register || lastst ==
                    kw_const || lastst == kw_volatile || lastst == kw_restrict)
                {
                    if (head->type == bt_func)
                        head->type = bt_ifunc;
                    else
                        temp1->type = bt_ifunc;
                    break;
                }
            }
            break;
        case colon:
            getsym();
            head1->bits = intexpr(0);
            if (head1->bits == bitsize(head1->type) && head1->bits != 0)
                head1->bits = head1->startbit = -1;
            if (!scalarnonfloat(head1))
            {
                if (prm_ansi)
                    generror(ERR_INTBITFIELDS, 0, 0);
                else
                    generror(ERR_BFTYPE, 0, 0);
                head1->bits =  - 1;
            }
            else
            if (prm_ansi && head1->size != stdintsize)
            {
                generror(ERR_INTBITFIELDS, 0, 0);
                head1->bits =  - 1;
            }
            break;
        default:
            if (head->type == bt_func) {
                if (lastst == openpa || lastst == openbr) {
                    generror(ERR_FUNCBADRET,0,0);
                    expskim(skm_declend,FALSE);
                    break;
                }
                if (prm_cplusplus && lastst == colon || lastst == begin ||
                        castbegin(lastst) || lastst == kw_register || lastst ==
                        kw_const || lastst == kw_volatile || lastst == kw_restrict) {
                    head->type = bt_ifunc;
                    break;
                }
            }

            temp1 = head;
            while ((temp1->type == bt_pointer || temp1->type == bt_farpointer 
                || temp1->type == bt_segpointer) && temp1->val_flag)
                temp1 = temp1->btp;
            if (prm_cplusplus && sc != sc_external && sc != sc_member &&
                isstructured(temp1) && (temp1->sp->value.classdata.cppflags
                &PF_CPPSTRUCT))
            {
                ENODE *qn;
                TYP *tp;
                if (lastst == assign)
                {
                    TYP *tp1;
                    SYM *sm;
                    getsym();
                    if (lastst == id && !strcmp(lastid, temp1->sp->name))
                    {
                        TYP *hp =  *headptr;
                        getsym();
                        if (!needpunc(openpa, skm_declclosepa))
                            break;
                        tp = gatherparms(&qn, FALSE);
                        *headptr = hp;
                        conspair(sp, sc, TRUE, tp, qn, FALSE, 0, FALSE);
                        parm_ns_level--;
                    }
                    else
                    {
                        ENODE *xnode = 0;
                        tp = exprnc(&qn);
                        if (tp && isstructured(tp) && exactype(temp1, tp, FALSE))
                        {
                            int thiscall = 0;
                            ENODE *ep1,  *qn1;
                            ENODE *epn = qn;
                            TYP tp2;
                            if (sc == sc_auto)
                                ep1 = makenode(en_autocon, sp, 0);
                            else
                                ep1 = makenode(en_nacon, sp, 0);
                                if (qn->nodetype == en_conslabel)
                                {
                                    qn = qn->v.p[0];
                                    epn = qn;
                                }
                                if (qn->nodetype == en_thiscall)
                                {
                                    qn = qn->v.p[1];
                                    thiscall = 1;
                                }
                            if (qn->nodetype == en_callblock)
                            {
                                qn->v.p[0]->v.sp->nullsym = TRUE;
                                qn->v.p[0] = ep1;
                            }
                            else if (qn->nodetype == en_moveblock)
                            {
                                qn->v.p[1] = ep1;
                            }
                            else if (qn->nodetype == en_fcallb)
                            {
                                qn = cbll(ep1, qn, tp);
                            }
                            else
                            {
                                SYM fsym; /* fsym and ftype only used for error
                                    messages */
                                TYP ftype;
                                xnode = makenode(en_void, qn, 0);
                                fsym.tp = tp;
                                fsym.next = 0;
                                ftype.type = bt_func;
                                ftype.lst.head = ftype.lst.tail = &fsym;
                                xnode = do_constructor(sp, &ftype, tp, xnode, 1,
                                    0, TRUE, TRUE);
                                if (!xnode)
                                {
                                    qn = makenode(en_moveblock, ep1, qn);
                                    qn->size = tp->size;
                                }
                                else
                                    qn = xnode;
                            }
                                if (thiscall)
                                {
                                    epn->v.p[1] = qn;
                                    qn = epn;
                                }
                            if (!xnode)
                                qn = makenode(en_conslabel, qn, makenode
                                    (en_autocon, (void*)sp->mainsym, 0));
                            if (sc == sc_auto)
                            {
                                SNODE *snp = xalloc(sizeof(SNODE));
                                snp->next = 0;
                                snp->stype = st_expr;
                                snp->exp = qn;
                                if (cbautoinithead == 0)
                                    cbautoinithead = cbautoinittail = snp;
                                else
                                {
                                    cbautoinittail->next = snp;
                                    cbautoinittail = snp;
                                }
                            }
                            else
                            {
                                cppinitinsert(qn);
                            }
                            tp2.lst.head =  - 1;
                            tp2.lst.tail =  - 1;
                            tp2.type = bt_func;
                            qn1 = do_destructor(sp, &tp2,  *headptr, 0, 1, 0,
                                FALSE);
                            if (qn && !qn1)
                                qn1 = makenode(en_destlabel, makeintnode
                                    (en_icon, 0), makenode(en_autocon, (void*)
                                    sp->mainsym, 0));
                            if (qn1)
                            {
                                if (sc == sc_auto)
                                {
                                    if (block_rundown)
                                        block_rundown = makenode(en_void, qn1,
                                            block_rundown);
                                    else
                                        block_rundown = qn1;
                                }
                                else
                                {
                                    cpprundowninsert(qn1);
                                }
                            }
                        }
                        else if (tp && tp->type == bt_ref && isstructured(tp
                            ->btp) && exactype(temp1, tp->btp, FALSE))
                        {
                            int thiscall = 0;
                            ENODE *ep1,  *qn1;
                            ENODE *epn = qn;
                            TYP tp2;
                            if (sc == sc_auto)
                                ep1 = makenode(en_autocon, sp, 0);
                            else
                                ep1 = makenode(en_nacon, sp, 0);
                            qn = makenode(en_movebyref, qn, 0);
                            qn = makenode(en_moveblock, ep1, qn);
                            qn->size = tp->btp->size;
                            qn = makenode(en_conslabel, qn, makenode(en_autocon,
                                (void*)sp->mainsym, 0));
                            if (sc == sc_auto)
                            {
                                SNODE *snp = xalloc(sizeof(SNODE));
                                snp->next = 0;
                                snp->stype = st_expr;
                                snp->exp = qn;
                                if (cbautoinithead == 0)
                                    cbautoinithead = cbautoinittail = snp;
                                else
                                {
                                    cbautoinittail->next = snp;
                                    cbautoinittail = snp;
                                }
                            }
                            else
                            {
                                cppinitinsert(qn);
                            }
                            tp2.lst.head =  - 1;
                            tp2.lst.tail =  - 1;
                            tp2.type = bt_func;
                            qn1 = do_destructor(sp, &tp2,  *headptr, 0, 1, 0,
                                FALSE);
                            if (qn && !qn1)
                                qn1 = makenode(en_destlabel, makeintnode
                                    (en_icon, 0), makenode(en_autocon, (void*)
                                    sp->mainsym, 0));

                            if (qn1)
                            {
                                if (sc == sc_auto)
                                {
                                    if (block_rundown)
                                        block_rundown = makenode(en_void, qn1,
                                            block_rundown);
                                    else
                                        block_rundown = qn1;
                                }
                                else
                                {
                                    cpprundowninsert(qn1);
                                }
                            }


                        }
                        else if (tp)
                        {
                            joinxx: tp1 = maketype(bt_func, 0);
                            sm = makesym(0);
                            qn = makenode(en_void, qn, 0);
                            tp1->lst.head = tp1->lst.tail = sm;
                            sm->tp = tp;
                            conspair(sp, sc, FALSE, tp1, qn, TRUE, FALSE, TRUE);
                        }
                        else
                        {
                            backup(assign);
                        }
                    }
                }
                else
                {
                    TYP *hp =  *headptr;
                    tp = gatherparms(&qn, TRUE);
                    *headptr = hp;
                    conspair(sp, sc, TRUE, tp, qn, FALSE, 0, FALSE);
                    parm_ns_level--;
                }
            }
            break;
    }
    typequal = otc;
}

//-------------------------------------------------------------------------

void decl3(enum e_sc sc)
/*
 * Array index handling
 */
{
    TYP *temp1,  *list[40],  *head =  *headptr;
    TYP *otc;
    char buf[100];
    int count = 0, i;
    int mustsize = 0;
    int varisize = FALSE;
    inarrayindexparsing = TRUE;
    strcpy(buf, declid);
    head->uflags |= UF_DEFINED;
    head->uflags &= ~UF_CANASSIGN;
    decl3size = 1;
    if (head->size == 0 && sc != sc_external && (sc != sc_auto || block_nesting != 0))
        generror(ERR_ARRELEMSIZE,0,0);
    while (lastst == openbr)
    {
        if (currentTemplate)
        {
            expskim(skm_declclosebr, FALSE);
            needpunc(closebr, 0);
            list[count++] = maketype(bt_pointer, 1);
        }
        else
        {
            otc = typequal;
            typequal = 0;
            getsym();
            temp1 = maketype(bt_pointer, 0);
            temp1->val_flag = NORMARRAY;
            temp1->cflags = head->cflags; // inherit volatile and const
            if (lastst == closebr)
            {
                if (mustsize)
                    generror(ERR_SIZE, 0, 0);
                if (structlevel && prm_c99 && !infuncargs)
                {
                    temp1->size = 0;
                    encounteredflexarray = TRUE;
//                    temp1->cflags |= DF_FLEXARRAY;
                }
                else
                {
                    temp1->size = 0;
                }
                getsym();
            }
            else if (lastst == star)
            {
                getsym();
                if (lastst == closebr)
                {
                    if (!prm_c99)
                        generror(ERR_IDEXPECT, 0, 0);
                    varisize = TRUE;
                    temp1->esize = (ENODE*) - 1;
                    getsym();
                }
                else
                {
                    backup(star);
                    goto join;
                }
            }
            else if (lastst == kw_restrict)  /* C99? array[restrict] */
            {
#if 0 /* do no support */
                errlineno = lineno;
                generror(ERR_RESTRICTPTR, 0, 0);
#else /* FIXME?, handled as *restrict */
                getsym();
                if (lastst == closebr)
                {
                    head->cflags |= DF_RESTRICT;
                    getsym();
                }
#endif
            }
            else
            {
                ENODE *ep;
                int val;
                TYP *t1;
                join: val = 0;
                t1 = exprnc(&ep);
                if (!t1)
                {
                    generror(ERR_EXPREXPECT, 0, 0);
                    t1 = &stdint;
                    ep = makenode(en_icon, 0, 0);
                }
                opt4(&ep, FALSE);
                temp1->esize = ep;
                while (castvalue(ep))
                    ep = ep->v.p[0];
                if (isintconst(ep->nodetype)) {
                    val = ep->v.i;
                    if (val < 0)
					{
                        generror(ERR_ARRAYMISMATCH,0,0);
						val = 1;
					}
                } else if (isfloatconst(ep->nodetype)) {
                    generror(ERR_ARRAYMISMATCH,0,0);
                } else
                {
                    if ((t1->cflags &DF_CONST) && scalarnonfloat(t1))
                    {
                        SYM *sp;
                        while (castvalue(ep))
                            ep = ep->v.p[0];
                        sp = ep->v.p[0]->v.sp;
                        if (sp->init)
                        {
                            if (!(t1->uflags & UF_ASSIGNED))
                                val = sp->init->val.i;
                            if (val < 0) {
                                generror(ERR_ARRAYMISMATCH,0,0);
								val = 1;
							}
                        } 
                        else
                        {
                            val = 1;
                            varisize = TRUE;
                        }
                    }
                    else
                    {
                        val = 1;
                        varisize = TRUE;
                        if (!prm_c99)
                            generror(ERR_NEEDCONST, 0, 0);
                        else if (!scalarnonfloat(t1))
                            generror(ERR_ARRAYMISMATCH, 0, 0);
                    }
                }
                decl3size *= temp1->size = val;
                needpunc(closebr, skm_declclosebr);
            }
            list[count++] = temp1;
            mustsize = 1;
            typequal = otc;
        }
    }
    strcpy(declid, buf);
    if (head != NULL)
    {
        list[count - 1]->size *= head->size;
        if (tail == NULL)
            tail = head;
    }
    for (i = count - 1; i > 0; i--)
    {
        list[i - 1]->size *= list[i]->size;
    }
    list[count - 1]->uflags |= UF_DEFINED;

    for (i = 0; i < count - 1; i++)
    {
        list[i]->btp = list[i + 1];
        if (!varisize)
            list[i]->esize = 0;
    }
    list[count - 1]->btp = head;
    if (!varisize)
        list[count - 1]->esize = 0;

    if (varisize)
    {
        *headptr = tail = maketype(bt_pointer, stdaddrsize + (count + 2)
            *stdintsize);
        tail->val_flag = VARARRAY;
        tail->btp = list[0];
    }
    else
    {
        *headptr = list[0];
        if (tail == NULL)
            tail = list[count - 1];
    }
    inarrayindexparsing = FALSE;
}

//-------------------------------------------------------------------------

int bitsize(int type)
/*
 * Max bit field depends on the type
 */
{
    switch (type)
    {
        case bt_bool:
        case bt_char:
        case bt_unsignedchar:
            return 8;
        case bt_short:
        case bt_unsignedshort:
            return 16;
        case bt_unsigned:
        case bt_long:
        case bt_unsignedlong:
        case bt_int:
        case bt_enum:
            return 32;
        case bt_longlong:
        case bt_unsignedlonglong:
            DIAG("bit-field type larger than 32 bits not supported");
            return 64;
    }
    return 0;
}

//-------------------------------------------------------------------------

int oksize(void)
/*
 * See if the size field is ok or if we should gen a message
 */
{
    TYP *q = head;
    while (q->type == bt_pointer || q->type == bt_farpointer || q->type ==
        bt_segpointer)
    {
        if (q->val_flag)
            return 1;
        q = q->btp;
    }
    if (head->sp && (head->sp->value.classdata.cppflags & PF_NOTFULLYDEFINED))
        return 1;
//    if (head->sp && head->sp == declclass)
//        return 1;
    return head->size != 0;
}

//-------------------------------------------------------------------------

void checkDefaults(SYM *oldf, SYM *newf)
{
    if (newf->tp->type == bt_func || newf->tp->type == bt_ifunc)
    if (oldf->tp->type == bt_func || oldf->tp->type == bt_ifunc)
    {
        SYM *spo = oldf->tp->lst.head;
        SYM *spn = newf->tp->lst.head;
        while (spo && spn && spo != (SYM*) - 1 && spn != (SYM*) - 1)
        {
            if (spn->value.classdata.defalt && spo->value.classdata.defalt)
                gensymerror(ERR_REDECLDEFAULT, spn->name);
            spn->value.classdata.defalt = spo->value.classdata.defalt;
            spo = spo->next;
            spn = spn->next;
        }
    }

}

//-------------------------------------------------------------------------

int basedeclare(TABLE *table, int al, long ilc, int ztype, int flags, int
    xcppflags)
/*
 *  Once a type declarator is found we come here to get the remainder of the
 * declaration and allocate spae
 *
 */
{
    SYM *sp = 0,  *sp1;
    TYP *dhead;
    int nbytes, ufsave, staticerror = 0;
    int declcount = 0;
    nbytes = 0;
    dhead = head;
    ufsave = head->uflags;
    if (aFriend && (!declclass || al != sc_member))
        generror(ERR_FRIENDMUSTCLASS, 0, 0);
    else if (aFriend && isstructured(head))
    {
        SYM *sp = copysym(head->sp);
        if (declclass)
            sp->parentclass = declclass;
        else
            sp->parentclass = typequal;
        sp->originalparentclass = declclass;
        insertfriend(declclass, sp);
        needpunc(semicolon, 0);
        return 0;
    }
    for (;;)
    {
        int reffed = FALSE;
        int varlineno = lineno;
		int localssearched = FALSE;
        //                int reassigned = FALSE ;
        sp1 = 0;
        declid[0] = 0;
        memberptr = FALSE;
        headptr = &head;
        sp = decl1(al, 0);
        //				if (isstructured(head->type) && !currentTemplate && !isaccessible(head->sp)) {
        //            if (!isaccessible(p) && !isfriend(p,table)) 
        //        			genclasserror(ERR_NOTACCESSIBLE,typesp->name);
        //				}
        farflag = FALSE;
        segflag = FALSE;
        mainfunc = FALSE;
            sp->value.classdata.cppflags |= xcppflags;
            sp->value.classdata.cppflags |= cppflags;
            if (sp->value.classdata.cppflags &PF_EXPLICIT)
            {
                if (!(sp->value.classdata.cppflags &PF_CONSTRUCTOR))
                    generror(ERR_EXPLICITNOTALLOWED, 0, 0);
            }
            if (head->type != bt_func && head->type != bt_ifunc)
                sp->value.classdata.cppflags &= ~(PF_CONSTRUCTOR |
                    PF_DESTRUCTOR);
        /* stupid C standard uses a proper function name
         ** as an alias for a pointer to a func when is a member...
         */
        if (al == sc_member && !prm_cplusplus)
        if (head->type == bt_func)
        {
            TYP *temp = maketype(bt_pointer, stdaddrsize);
            temp->btp = head;
            head = temp;
        }
        else if (head->type == bt_ifunc)
            generror(ERR_NOFUNCSTRUCT, 0, 0);

        /* In case they put the extern tag on an ifunc */
        if (head->type == bt_ifunc && al == sc_external)
            sp->storage_class = sc_global;

        if (declid[0] == 0)
        {
            if ((flags &DF_FUNCPARMS))
            {
                sprintf(declid, "**ARG%d**", pcount++);
            }
        }
        head->cflags |= flags;
        if (declid[0])
        {
             /* otherwise just struct tag... */
            declcount++;
            if (head->type == bt_memberptr)
            {
                sp->hasproto = TRUE;
                if (head->sp->tp->type == bt_memberptr)
                    sp->value.classdata.memptrclass = head->sp
                        ->value.classdata.memptrclass;
                else
                    sp->value.classdata.memptrclass = head->sp;
            }
            if (!isstructured(head) && head->type != bt_memberptr)
                head->sp = sp;

                if (aFriend)
                {
                    sp1 = declclass;
                    declclass = 0;
                }

                if (head->type == bt_func || head->type == bt_ifunc)
                if (prm_cplusplus)
                {
                    char *p = declid;
                    if (prm_cmangle &&  *p == '_')
                        p++;
                    if (!declclass && !strcmp(p, "main"))
                    {
                        SYM *sp1;
                        mainfunc = TRUE;
                        sp->name = litlate(declid);
                        sp1 = search(sp->name, gsyms);
                        if (sp1 && sp1->tp->type == bt_ifunc)
                            generror(ERR_NOOVERMAIN, 0, 0);
                    }
                    else if (mangleflag && !ispascal && !isstdcall && !iscdecl
                        && !(head->cflags &DF_INTRINS)
                        )
                    {
                        sp->mangled = TRUE;
                        if (templateSP)
                            sp->name = templateSP->name;
                        else if (declid[0] == '$' && declid[1] == 'o')
                            sp->name = fullcppmangle(0, declid, &stdvoidfunc);
                        else
                            sp->name = fullcppmangle(0, declid, head);
                    }
                    else
                        sp->name = litlate(declid);
                    if (typequal)
                        if (typequal->storage_class == sc_namespace)
                            table = typequal->value.classdata.parentns->table;
                        else
                            table = &typequal->tp->lst;
                }
                else
                {
                    char *p = declid;
                    if (prm_cmangle &&  *p == '_')
                        p++;
                    if (!strcmp(p, "main"))
                        mainfunc = TRUE;
                    sp->name = litlate(declid);
                }
                else
            {
                    if (prm_cplusplus)
                    {
                        if (typequal)
                            table = &typequal->tp->lst;
                        sp->mangled = mangleflag;
                    }
                sp->name = litlate(declid);

            }
                if (aFriend)
                {
                    declclass = sp1;
                }
                if (al == sc_static)
                    sp->value.classdata.cppflags |= PF_STATIC;
                //                              sp->value.classdata.defalt = 0;
            if (declclass && !funcnesting)
                sp->parentclass = sp->originalparentclass = declclass;
            else
                if (typequal && typequal->storage_class != sc_namespace)
                    sp->parentclass = sp->originalparentclass = typequal;
            sp->value.classdata.parentns = thisnamespace;
                if (templateFunc)
                {
                    switch (head->type)
                    {
                        case bt_func:
                        case bt_ifunc:
                            sp->value.classdata.templatedata = currentTemplate;
                            sp->istemplate = TRUE;
                            {
                                SYM *sp2 = currentTemplate->classes.head;
                                int done = FALSE;
                                while (sp2)
                                {
                                    if (sp2->defalt)
                                    {
                                        if (!done)
                                            generror(ERR_TEMPLATEDEFAULTLIMITED,
                                                0, 0);
                                        sp2->defalt = 0;
                                        done = TRUE;
                                    }
                                    sp2 = sp2->next;
                                }
                            }
                            break;
                        case bt_class:
                        case bt_struct:
                            generror(ERR_TEMPLATENOINSTANTIATION, 0, 0);
                            currentTemplate = 0;
                            templateFunc = 0;
                            recordingTemplate = 0;
                            templateLookup = 0;
                            break;
                        default:
                            if (typequal && typequal->istemplate)
                            {
                                sp->value.classdata.templatedata =
                                    currentTemplate;
                                sp->istemplate = TRUE;
                            }
                            else
                            {
                                generror(ERR_TEMPLATECLASSFUNC, 0, 0);
                                currentTemplate = 0;
                                templateFunc = 0;
                                recordingTemplate = 0;
                                templateLookup = 0;
                            }
                            break;
                    }
                }
            sp->tp = head;
            sp->extflag = FALSE;
            sp->absflag = (al == sc_abs);
            sp->intflag |= (flags &DF_INT) ? 1 : 0;
            sp->faultflag |= (flags &DF_FAULT) ? 1 : 0;
            sp->loadds = (flags &DF_LOADDS) ? 1 : 0;
            sp->pascaldefn = ispascal;
            sp->exportable = isexport;
            sp->importable = isimport;
            sp->importfile = importname;
            sp->isstdcall = isstdcall;
            sp->indirect = isindirect;

            sp->init = 0;
            sp->indecltable = 0;
            sp->funcparm = 0;
            sp->inreg = 0;
            sp->staticlabel = FALSE;
            sp->mainsym = sp;
            sp->dontlist = sys_inc;
            if (prm_cplusplus && (head->cflags &DF_CONST) && scalarnonfloat
                (head) && sp->storage_class != sc_external && al != sc_auto)
            {
                sp->storage_class = sc_static;
            }
            if (aFriend)
            {
                if (sp->tp->type == bt_func)
                    insertfriend(declclass, sp);
                else
                    generror(ERR_FRIENDNEEDFUNC, 0, 0);
                needpunc(semicolon, 0);
                return 0;
            }
            if ((sp->tp->type == bt_func || sp->tp->type == bt_ifunc) && (sp
                ->tp->cflags &DF_FAR || sp->tp->btp->cflags &DF_FAR))
            {
                sp->farproc = TRUE;
            }

            if (al != sc_type)
            if (sp->tp->type == bt_func || sp->tp->type == bt_ifunc)
            {
                setalias(sp);
            }
            else if (xcppflags &PF_STATIC)
                sp->alias = fullcppmangle(0, declid, 0);
             /* fake out static vars by using the alias field... */


            if (al == sc_external && (lastst == assign || lastst == openpa))
            {
                sp->storage_class = sc_global;
            }


            if (sp->intflag && (ispascal || isstdcall || isexport || isimport))
                generror(ERR_PASCAL_NO_INT, 0, 0);

            //										if (al == sc_autoreg) {
            //											if (head->size > 4) {
            //												sp->storage_class = sc_auto;
            //												gensymerror(ERR_ILLREGISTER,sp->name);
            //											}
            //										}	
            if (al != sc_type && (oksize() || sp->tp->type == bt_func || sp->tp
                ->type == bt_ifunc))
            {
                if (al == sc_abs)
                    sp->value.i = ilc;
                else
                    if (!(sp->value.classdata.cppflags &PF_OPERATOR))
                        if (head->type != bt_func && head->type != bt_ifunc)
			                if (sp->storage_class == sc_static && table ==  &lsyms)
			                {
			                    sp->value.i = nextlabel++;
			                    sp->staticlabel = TRUE;
			                }
			                else
							{
			                    sp->value.i = block_nesting;
							}
		                else
						{
		                    sp->value.i = 0;
						}
                {
                    int align = getalign(al, head);
                    int noadj = FALSE;

                    if (al != sc_member || head->bits ==  - 1)
                    {
                        if (curbit < 100)
                            if (ztype == bt_union)
                                nbytes = imax(nbytes, sizeoflastbit);
                            else
                                nbytes += sizeoflastbit;
                        curbit = 100;
                        sizeoflastbit = 0;
                        bittype =  - 1;
                    }
                    else if (bittype != head->type || head->bits + curbit >
                        bitsize(bittype) || head->bits == 0)
                    {
                        if (bittype !=  - 1)
                            if (ztype == bt_union)
                                nbytes = imax(nbytes,(curbit-1)/8 + 1);
                            else
                                nbytes += (curbit - 1) / 8+1;
                        sizeoflastbit = sp->tp->size;
                        bittype = head->type;
                        curbit = 0;
                        noadj = TRUE;
                    }
                    else
                        noadj = head->bits !=  - 1;

                    if (al == sc_member && sp->tp->type != bt_func && sp->tp
                        ->type != bt_ifunc && !(sp->value.classdata.cppflags
                        &PF_OPERATOR))
                    {
                        int val;
                        if (packdata[packlevel] != 1 && (!noadj || !curbit))
                        {
                            val = (ilc + nbytes) % align;
                            if (val)
                                val = align - val;
                            nbytes += val;
                        }
                        sp->value.i = nbytes + ilc;
                        if (!noadj)
                        {
                            int size;
                            if (curbit < 100)
                                size = (curbit - 1) / 8+1;
                            else
                                size = sp->tp->size;
                            if (ztype == bt_union)
                                nbytes = imax(nbytes,size);
                            else
                                nbytes += size;
                        }
                    }
                }
                if (head->bits !=  - 1)
                {
                    if (al != sc_member)
                    {
                        generror(ERR_BFILLEGAL, 0, 0);
                        head->bits =  - 1;
                    }
                    else if (head->bits > bitsize(bittype))
                    {
                        generror(ERR_BFTOOBIG, 0, 0);
                        head->bits =  - 1;
                    }
                    else
                    {
                        if (prm_revbits)
                            head->startbit = head->size *8-curbit - head->bits;
                        else
                            head->startbit = curbit;
                        curbit += head->bits;
                    }
                }

                if (sp->absflag && (sp->tp->type == bt_func || sp->tp->type ==
                    bt_ifunc))
                    gensymerror(ERR_ILLCLASS, lastid);
                if (sp->intflag && (sp->tp->type != bt_func && sp->tp->type !=
                    bt_ifunc))
                    if (sp->tp->type != bt_pointer || sp->tp->btp->type !=
                        bt_func)
                        gensymerror(ERR_ILLCLASS, lastid);
                    /* kill the inline flag for cases we can't handle */
                    if ((sp->parentclass && (sp->value.classdata.cppflags &
                        (PF_DESTRUCTOR | PF_VIRTUAL | PF_INSTANTIATED))))
                        sp->value.classdata.cppflags &= ~PF_INLINE;
                    if (mainfunc && (sp->value.classdata.cppflags &PF_INLINE))
                    {
                        generror(ERR_MAINNOINLINE, 0, 0);
                    }
                    if (sp->tp->type == bt_union && (sp->tp->type == bt_func ||
                        sp->tp->type == bt_ifunc))
                        gensymerror(ERR_UNIONNOVIRT, lastid);
                    if (sp->tp->type != bt_func && sp->tp->type != bt_ifunc)
                    {
                        if ((sp->value.classdata.cppflags &PF_VIRTUAL) &&
                            !declclass)
                            generror(ERR_VIRTFUNC, 0, 0);
                        if (sp->value.classdata.cppflags &PF_INLINE)
                            generror(ERR_INLINEFUNC, 0, 0);
                        /*												if ((sp->value.classdata.cppflags & (PF_CONSTRUCTOR | PF_DESTRUCTOR)) && !declclass)
                        generror(ERR_CONSFUNC,0,0);
                         */
                    }
                    if (sp->value.classdata.cppflags &(PF_CONSTRUCTOR |
                        PF_DESTRUCTOR))
                    {
                        if (sp->value.classdata.cppflags &(PF_STATIC |
                            PF_FRIEND))
                            generror(ERR_NODESTRUCTQUAL, 0, 0);
                        //                                    if (!defaulttype)
                        //                                       gensymerror(ERR_CANTHAVETYPE,declclass->name);
                    }
                    if (isstructured(head) && !infuncargs && (head->sp->mainsym
                        ->value.classdata.cppflags &PF_NOTFULLYDEFINED) &&
                            !(sp->value.classdata.cppflags & PF_STATIC) && !statictemplate)
                        gensymerror(ERR_SIZE, head->sp->name);
                    if (sp->value.classdata.cppflags &PF_DESTRUCTOR)
                        if (sp->tp->lst.head != (void*) - 1)
                            generror(ERR_NODESTRUCTARG, 0, 0);
                    if (sp->value.classdata.cppflags &PF_OPERATOR)
                        CheckOverloadedArgs(sp);
                    if ((sp->tp->type == bt_func || sp->tp->type == bt_ifunc)
                        && sp->istemplate)
                        checkTemplateSpecialization(sp, declid);
                if (sp->tp->type == bt_func && al != sc_static)
                    sp->storage_class = sc_externalfunc;
                    //                                            if (memberptr && typequal)
                    //                                    sp1 = basesearch(declid, &typequal->value.classdata.memberptrs,FALSE);
                    //                                            else
                    if (templateLookup && typequal)
                        if (typequal->tp->type == bt_templateplaceholder)
                            sp1 = basesearch(declid, &typequal->tp->lst.head
                                ->tp->lst.head, FALSE);
                        else if (typequal->storage_class == sc_namespace)
                            sp1 = namespace_search(declid, typequal
                                ->value.classdata.parentns, 0);
                        else
                            sp1 = basesearch(declid, &typequal->tp->lst.head,
                                FALSE);
                        else
						{
							localssearched = TRUE;
	                        sp1 = search2(declid, table, FALSE, TRUE);
						}
                    if (sp1 && sp1->tp->type == bt_defunc)
                    {
                        if (currentTemplate && typequal)
                            searchTemplate = typequal
                                ->value.classdata.templatedata;
                        sp1 = funcovermatch2(sp1, sp->tp, 0, TRUE, FALSE, FALSE)
                            ;
                        searchTemplate = 0;
                        if (sp1 && (sp1->value.classdata.cppflags &PF_MEMBER))
                        {
                            //                                       reassigned = TRUE ;
                            sp->mainsym = sp1->mainsym = sp1;
                            sp->value.classdata.cppflags |= sp1
                                ->value.classdata.cppflags &PF_VIRTUAL;
                            if (sp->value.classdata.cppflags &PF_VIRTUAL)
                                sp->value.classdata.cppflags &= ~PF_INLINE;

                            sp1->value.classdata.cppflags |= sp
                                ->value.classdata.cppflags;
                            sp1->parentclass = sp->parentclass;
                            sp1->value.classdata.inlinefunc = 0;
                            sp1->storage_class = sp->storage_class;
                            sp1->tp->btp = copytype(sp->tp->btp, 0);
                            if (strcmp(sp1->name, sp->name))
                            {
                                // in case of overriding a virtual func
                                sp1->tp->type = bt_func;
                                sp1->value.classdata.cppflags = sp
                                    ->value.classdata.cppflags;
                            }
                            sp1->name = sp->name;
                            if (table != gsyms && sp->tp->type == bt_func)
                            {
                                LIST *l = xalloc(sizeof(LIST));
                                l->data = sp1;
                                l->link = localfuncs;
                                localfuncs = l;
                            }
                        }
                    }
                    else if (sp && !sp1 && sp->tp->type != bt_func && sp->tp
                        ->type != bt_ifunc && (sp->value.classdata.cppflags &
                        (PF_STATIC | PF_MEMBER)) == (PF_STATIC | PF_MEMBER))
                    {
                        if (table != gsyms)
                        {
                            LIST *l = xalloc(sizeof(LIST));
                            l->data = sp;
                            l->link = localdata;
                            localdata = l;
                            sp->mainsym->extflag = TRUE;
                        }
                    }
				if (sp1 && isfunc(sp1->tp) && isfunc(sp->tp))
				{
					TYP *tp1 = sp1->tp;
					TYP *tp = sp->tp;
					if (tp1->type == bt_ref)
						tp1 = tp1->btp;
					if (tp->type == bt_ref)
						tp = tp->btp;
			        if (!exactype(tp->btp, tp1->btp, 4) && !strcmp(sp->name, sp1->name))
	        			gensymerror(ERR_DECLMISMATCH, sp1->name);
//			        if (prm_cplusplus)
//	        			sp1 = 0;
			}
                if (sp1)
					if (sp1->tp->type != bt_defunc && sp->tp->type != bt_func && sp->tp->type != bt_ifunc)

                        if (!exactype(sp->tp, sp1->tp, FALSE)) 
                        {

							if (sp->tp->type == bt_func || sp->tp->type == bt_ifunc || 
									( !localssearched  || !block_nesting))
	                            gensymerror(ERR_DECLMISMATCH, sp->name);
                        }
                if (sp->name[0] == '@' && !sp1)
                {
                    if (typequal && typequal != declclass)
                    {
                        if (templateLookup)
                            genclasserror(ERR_TEMPLATEFUNCMATCHCLASS, sp->name);
                        else
                            genclasserror(ERR_NOCLASSMEMBERFUNC, sp->name);
                    }
                    if (declid[0] == '$' && declid[1] == 'o')
                        funcrefinsert("$casts$", sp, table, declclass);
//                    else if (prm_cplusplus)
//                        funcrefinsert(sp->name, sp, table, declclass);
                    else
                        funcrefinsert(declid, sp, table, declclass);
                    reffed = TRUE;
                }
                if (!sp1 || (sp1->tp->type != bt_func && sp1->tp->type !=
                    bt_ifunc))
                {
                    if (!sp1 || al == sc_auto || al == sc_autoreg || al ==
                        sc_member)
                    {
                            //                                                    if (memberptr && typequal)
                            //                                                        insert(sp, &typequal->value.classdata.memberptrs);
                            //                                                    else
                        {
							if (table == &lsyms && sp->tp->type == bt_func)
							{
								SYM *sp1 = sp->tp->lst.head;
								global_flag++;
								sp = copysym(sp);
								sp->mainsym = sp;
								sp->tp = copytype(sp->tp, 0);
								sp->tp->sp = sp;
								sp->name = litlate(sp->name);
								if (sp1 && sp1 != (SYM *)-1)
								{
									sp->tp->lst.head = sp->tp->lst.tail = NULL;
									while (sp1)
									{
										SYM *sp2 = copysym(sp1);
										sp2->mainsym = sp2;
										sp2->tp = copytype(sp2->tp, 0);
										sp2->name = litlate(sp2->name);
										insert(sp2, &sp->tp->lst);
										sp1 = sp1->next;
									}
								}
								global_flag--;
								if (!reffed)
								{
									SYM *sp1 = gsearch(sp->name);
									if (!sp1)
										insert(sp, gsyms);
									else
									{
										TYP *tp1 = sp1->tp;
										TYP *tp = sp->tp;
										if (tp1->type == bt_ref)
											tp1 = tp1->btp;
										if (tp->type == bt_ref)
											tp = tp->btp;
										if (!exactype(tp->btp, tp1->btp, 4))
											gensymerror(ERR_DUPSYM, sp->name);
									}
								}
							}
                            else
							{
								if (!reffed)
                                	insert(sp, table);
	                            if (table != gsyms && sp->tp->type == bt_func)
    	                        {
        	                        LIST *l = xalloc(sizeof(LIST));
                    	            l->data = sp;
                        	        l->link = localfuncs;
                            	    localfuncs = l;
								}
                            }
                        }

                            if (prm_cplusplus && al == sc_member && (xcppflags
                                &PF_STATIC))
                            {
                                SYM *sp2 = copysym(sp);
                                sp2->storage_class = lastst == assign || lastst
                                    == openpa ? sc_global : sc_external;
                                //                                          sp2->name = sp->alias;
                                sp->value.classdata.gdeclare = sp2;
                                //                                                        if (sp->tp->type != bt_func && sp->tp->type != bt_ifunc)
                                //                                                          insert(sp2,gsyms) ;
                            }
                        #ifndef ICODE
                            if (sp->tp->cflags &DF_INTRINS)
                                SearchIntrins(sp);
                        #else 
                            sp->tp->cflags &= ~DF_INTRINS;
                        #endif 
                    }
                    else
                    {
                        if (sp1->tp->size == 0) {
                            sp1->tp = sp->tp;
                            if (sp1->value.classdata.gdeclare)
                                sp1->value.classdata.gdeclare->tp = sp1->tp;
                        }
                        if (sp1->storage_class == sc_external || sp1
                            ->storage_class == sc_externalfunc)
                        {
                            sp->value.classdata.parentns = sp1
                                ->value.classdata.parentns;
                            sp1->storage_class = sp->storage_class;
                            sp1->tp = sp->tp;
                            sp1->value = sp->value;
                            sp->mainsym = sp1;
                            if (sp->value.classdata.cppflags &PF_INLINE)
                                sp1->value.classdata.cppflags |= PF_INLINE;
                        }
                            else if (sp1->storage_class == sc_member)
                            {
                                if (sp->storage_class == sc_static &&
                                    !staticerror++)
                                    generror(ERR_NOSTATIC, 0, 0);
                                if (sp1->value.classdata.gdeclare)
                                    sp1->value.classdata.gdeclare
                                        ->storage_class = sp->storage_class;
                                sp1->mainsym->extflag = TRUE;
                            }
                    }
                }
                else
                {
                    join: 

                    if (prm_cplusplus && (sp1->tp->type == bt_func || sp1->tp
                        ->type == bt_ifunc))
                        checkDefaults(sp1, sp);
                    if (sp1->tp->type == bt_func && sp->tp->type == bt_ifunc)
                    {
                            sp1->exportable |= sp->exportable;
                            sp1->importable |= sp->importable;
                            if (sp->importfile)
                                sp1->importfile = sp->importfile;
                        if (sp1->storage_class == sc_external || sp1
                            ->storage_class == sc_externalfunc || sp1
                            ->storage_class == sc_static)
                            sp1->oldstyle = sp->oldstyle;
                        if (al == sc_static || sp1->storage_class == sc_static)
                            sp->storage_class = sp1->storage_class = sc_static;
                        else
                            sp->storage_class = sp1->storage_class = sc_global;
                    }
                }
                if (sp->tp->type == bt_ifunc)
                {
                     /* function body follows */
                    int pushed = FALSE;
                    SYM *dc;
                    if (sp1)
                    {
                        if (sp->value.classdata.cppflags &PF_INLINE)
                            sp1->value.classdata.cppflags |= PF_INLINE;
                        if (sp1->tp->type == bt_ifunc)
                        {
                            //                                                if (!reassigned)
                            if (!templateFunc)
                                gensymerror(ERR_MULTIPLEINIT, sp->name);
                            else
                                gensymerror(ERR_TEMPLATEBODYDECLARED, sp->name);
                        }
                        else
                        {
                            if (templateFunc)
                                sp1->value.classdata.templatedata =
                                    currentTemplate;
                            sp1->tp->type = bt_ifunc;
                        }
                        sp1->tp->lst = sp->tp->lst;
                    }
                    else
                        sp1 = sp;
                        if (sp1 && (sp1->value.classdata.cppflags &PF_VIRTUAL)
                            && declclass)
                            addvtabentry(declclass, sp1->mainsym);
                        else if (sp && (sp->value.classdata.cppflags
                            &PF_VIRTUAL) && declclass)
                            addvtabentry(declclass, sp->mainsym);
                        if (declclass)
                        {
                            if (!(sp1->value.classdata.cppflags &(PF_DESTRUCTOR
                                | PF_VIRTUAL)))
                                if (lastst == begin)
                                    sp->mainsym->value.classdata.cppflags |=
                                        PF_INLINE;
                            sp->mainsym->value.classdata.cppflags |=
                                PF_HEADERFUNC;
                        }
                        if (typequal && typequal->storage_class != sc_namespace)
                        {
                            dc = declclass;
                            if (typequal->tp->type == bt_templateplaceholder)
                                declclass = typequal->tp->lst.head;
                            else
                                declclass = typequal;
                            pushed = TRUE;
                        }
                        if (sp1->value.classdata.cppflags &PF_CONSTRUCTOR)
                        {
                            setRefFlags(declclass);
                            setthunkflags(declclass, FALSE);
                            if (lastst == colon && !structlevel)
                            {
                                classbaseasn(declclass, sp1);
                                if (lastst != begin)
                                    generror(ERR_NEEDCHAR, '{', 0);
                            }
                        }
                    if (recordingTemplate || structlevel)
                    {
                        if (!recordingTemplate)
                        {
                            global_flag++;
                            recordingClassFunc = TRUE;
                            currentTemplate = xalloc(sizeof(struct template ));
                            currentTemplate->lineno = lineno;
                            currentTemplate->filename = litlate(infile);
                            if (lastst == colon)
                                insertTemplateChar(':');
                            else
                                insertTemplateChar('{');
                            insertTemplateChar(lastch);
                        }
                        templateWade();
                        currentTemplate->hasbody = TRUE;
                        if (!recordingTemplate)
                        {
                            ILIST *l = xalloc(sizeof(ILIST));
                            insertTemplateChar(0);
                            sp1->value.classdata.templatedata = currentTemplate;
                            currentTemplate = 0;
                            if (lastst != semicolon)
                            {
                                backup(semicolon);
                            } if (virtualfuncs)
                                sp1->value.classdata.cppflags |=
                                    PF_INSTANTIATED;
                            l->sp = sp1;
                            l->dc = declclass;
                            l->next = deferredFunctions;
                            deferredFunctions = l;
                            global_flag--;
                        }
                        recordingClassFunc = 0;
                    }
                    else
                    {
                        browse_startfunc(sp1, varlineno);
                        funcbody(sp1);
                        browse_endfunc(sp1, lineno);
                    }
                        if (pushed)
                            declclass = dc;
                    lastdecl = sp1;
                    if (al == sc_member && (xcppflags &PF_STATIC))
                        return 0;
                    return nbytes;
                }
                else
                {
                    if (sp1 && (sp1->value.classdata.cppflags &PF_VIRTUAL) &&
                        declclass)
                        addvtabentry(declclass, sp1->mainsym);
                    else if (sp && (sp->value.classdata.cppflags &PF_VIRTUAL)
                        && declclass)
                        addvtabentry(declclass, sp->mainsym);
                }
                if ((sp->storage_class == sc_global || sp->storage_class ==
                    sc_static) && sp->tp->type != bt_func && sp->tp->type !=
                    bt_ifunc && !(flags &DF_FUNCPARMS))
                {
                    int pushed = FALSE;
                    SYM *dc;
                    if (sp->tp->type == bt_ref && lastst != assign && lastst !=
                        openpa)
                        gensymerror(ERR_REFMUSTINIT, declid);
                    if (declclass && typequal && typequal->storage_class !=
                        sc_namespace)
                        gensymerror(ERR_NOINIT, sp->name);
                    if (!sp1)
                        sp1 = sp;
                        if (typequal && typequal->storage_class != sc_namespace)
                        {
                            dc = declclass;
                            declclass = typequal;
                            pushed = TRUE;
                        }
                        if (lastst == openpa)
                            if (!prm_cplusplus || sp1->tp->type != bt_class &&
                                sp1->tp->type != bt_struct)
                                generror(ERR_OLDFUNCSTYLE, 0, skm_declcomma);
                            else
                                doinit(sp1);
                            else
                            doinit(sp1);
                    sp1->mainsym->extflag = TRUE;
                    sp1->value.classdata.templatedata = currentTemplate;
                    browse_variable(sp1, varlineno);
                        if (pushed)
                            declclass = dc;
                }
                else
                if (al == sc_auto || al == sc_autoreg)
                {
                    int pushed = FALSE;
                        SYM *dc;
                        if (typequal && typequal->storage_class != sc_namespace)
                        {
                            dc = declclass;
                            declclass = typequal;
                            pushed = TRUE;
                        }
                        if (prm_cplusplus && (flags &DF_FUNCPARMS))
                            dodefaultinit(sp);
                        else
                            if (sp->tp->type == bt_ref && lastst != assign &&
                                lastst != openpa)
                                gensymerror(ERR_REFMUSTINIT, lastid);
                    if (!(flags &DF_FUNCPARMS))
                        browse_variable(sp, varlineno);
                    doautoinit(sp);
                        if (pushed)
                            declclass = dc;
                }
                else if (sp->storage_class == sc_external || sp->storage_class 
                    == sc_externalfunc)
                {
                    if (!prm_cplusplus)
                    {

                        if (sp1 && sp1->tp->type == bt_ifunc && (sp1
                            ->value.classdata.cppflags &PF_INLINE))
                        {
                            SetExtFlag(sp1, TRUE);
                        }
                        else
                            browse_variable(sp, varlineno);
                    }
                    else
                        browse_variable(sp, varlineno);
                }
            }
            else if (al != sc_type && al != sc_external)
            {
                gensymerror(ERR_SIZE, declid);
                expskim(skm_declclosepa, FALSE);
            }
            else
            {
                SYM *sp1;
                if (sp->tp->size)
                {
                    sp->tp = copytype(sp->tp, 0);
                }
                if ((sp->tp->val_flag & VARARRAY )
                        || sp->tp->type == bt_pointer && (sp->tp->btp->val_flag & VARARRAY))
				{
					if (!prm_c99)
						generror(ERR_VLA_c99,0,0);
					if (!funcnesting)
	                    generror(ERR_VLABLOCKSCOPE,0,0);
				}
                evaluateVLAtypedef(sp) ;
                sp1 = search(sp->name, table);
                if (sp1)
                {
                    if (!exactype(sp->tp, sp1->tp, FALSE))
                        gensymerror(ERR_DUPSYM, sp->name);
                }
                else
                {
                    insert(sp, table);

                    //                                    if (sp->tp->sp->name[0] == '$')
                    //                                       sp->tp->sp = sp ;
                }
                browse_variable(sp, varlineno);
                if (prm_debug)
                    sp->istypedef = TRUE;
            }
        }
        if (!(flags &DF_FUNCPARMS))
        {
            if (lastst == semicolon || lastst == eof)
                break;
            dhead = copytype(dhead, 0);
            needpunc(comma, skm_declend);
			if (lastst == semicolon || lastst == eof)
			{
				generror(ERR_DECLEXPECT,0, 0);
				break;
			}
        }
        else
        {
            if (lastst == comma || lastst == eof)
                break;
            if (sp1)
                lastdecl = sp1;
            else
                lastdecl = sp;
            if (al == sc_member && (xcppflags &PF_STATIC))
                return 0;
            return (nbytes);
        }
        if (declbegin(lastst) == 0 && lastst != colon)
            break;
        head = dhead;
        head->uflags = ufsave;
    }
    getsym();
    if (sp1)
        lastdecl = sp1;
    else
        lastdecl = sp;
    if (al == sc_member && declcount == 0)
    {
        if (head->type == bt_union || !prm_ansi && head->type == bt_struct)
        {
            /* handle anonymous unions and structs here... */
            char buf[40];
            SYM *lst = head->lst.head,  *lst1;
            {
                int align = getalign(al, head);
                int val;
                {
                    if (curbit < 100)
                        if (ztype == bt_union)
                            ilc = imax(ilc,(curbit-1)/8 + 1);
                        else
                            ilc += (curbit - 1) / 8+1;
                }
                if (packdata[packlevel] != 1)
                {
                    val = (ilc + nbytes) % align;
                    if (val)
                        val = align - val;
                    nbytes += val;
                }
                sp->value.i = nbytes + ilc;
                {
                    int size;
                    size = head->size;
                    nbytes += size;
                }
            }
            lastdecl->isunnamed = TRUE;
            sprintf(buf, "$$UNNAMEDSTRUCT%d", unnamedstructcount++);
            lastdecl->name = litlate(buf);
            lastdecl->tp = head;
            while (lst)
            {
                if (strcmp(lst->name, cpp_funcname_tab[CI_CONSTRUCTOR]) &&
                    strcmp(lst->name, cpp_funcname_tab[CI_DESTRUCTOR]))
                {
                    if (search(lst->name, table))
                    {
                        gensymerror(ERR_DUPSYM, lst->name);
                    }
                    else
                    {
                        lst1 = table->head;
                        while (lst1)
                        {
                            if (lst1->isunnamed)
                            if (search(lst->name, &lst1->tp->lst))
                            {
                                gensymerror(ERR_DUPSYM, lst->name);
                                break;
                            }
                            lst1 = lst1->next;
                        }
                    }
                }
                lst = lst->next;
            }

            if (table->head)
                table->tail = table->tail->next = lastdecl;
            else
                table->tail = table->head = lastdecl;
        }
    }
    if (al == sc_member && (xcppflags &PF_STATIC))
        return 0;
    return nbytes;
}

//-------------------------------------------------------------------------

int declare(TABLE *table, int al, int ztype, int flags, int cppflags)
/*
 * In this wrapper we do an ENTIRE declaration
 */
{
    int old_gf = global_flag, rv;
    if (al == sc_static)
        global_flag++;
    cppflags |= decl(table, cppflags);
    rv = basedeclare(table, al, 0, ztype, flags, cppflags);
    global_flag = old_gf;
    return rv;
}

//-------------------------------------------------------------------------

int declare2(TABLE *table, int al, int ztype, int flags, int cppflags, long val)
/*
 * In this wrapper we do an ENTIRE declaration
 */
{
    int old_gf = global_flag, rv;
    if (al == sc_static)
        global_flag++;
    cppflags |= decl(table, cppflags);
    rv = basedeclare(table, al, val, ztype, flags, cppflags);
    global_flag = old_gf;
    return rv;
}

//-------------------------------------------------------------------------

int declbegin(int st)
/*
 * This determines if another variable is being declared of the same type
 */
{
    switch(st) {
        case star:
        case id:
        case openpa:
        case kw__far:
        case kw__near:
        case kw__seg:
        case openbr:
        case kw_const:
        case kw_volatile:
        case kw__export:
        case kw__import:
            return TRUE;
        case and:
            return prm_cplusplus;
		default:
			return FALSE;
    }
}

//-------------------------------------------------------------------------

void declenum(TABLE *table, int cppflags)
/*
 * declare enums
 */
{
    SYM *sp = 0;
    TYP *tp;
    int declaring = FALSE;
    char *nm;
    int enumline = lineno;
    int xglob = global_flag;
    if (lastst == id)
    {
        /* tagged */
        global_flag++;
            if (prm_cplusplus)
            {
                nm = litlate(lastid);
                if (!sp)
                    sp = search2(nm, table, FALSE, TRUE);
                getsym();
                if (sp && sp->storage_class == sc_type)
                {
                    sp = parsetype(sp, TRUE);
                    if (sp->storage_class != sc_type)
                        generror(ERR_TYPENAMEEXP, 0, 0);
                    else if (sp->tp->type != bt_enum)
                        gensymerror(ERR_DECLMISMATCH, sp->name);
                }
            }
            else
        {
            sp = search2(nm = litlate(lastid), tagtable, FALSE, TRUE);
            getsym();
        }
        if (sp == 0)
        {
            declaring = TRUE;
            sp = makesym(sc_type);
            sp->tp = xalloc(sizeof(TYP));
            sp->tp->btp = sp;
            sp->tp->type = bt_enum;
            sp->tp->size = stdenumsize;
            sp->tp->lst.head = 0;
            sp->name = nm;
            sp->tp->sp = sp;
            sp->tp->bits = sp->tp->startbit =  - 1;
            sp->dontlist = sys_inc;
                sp->value.classdata.cppflags |= cppflags;
                /* tags are also typedefs in C++ */
                sp->value.classdata.parentns = thisnamespace;
                if (prm_cplusplus)
                    insert(sp, table);
                else
                insert(sp, tagtable);
            browse_variable(sp, enumline);
        }

        if (lastst == begin)
            sp->tp->enumlist = enumbody(sp, table);
        else if (declaring && prm_ansi)
        {
            generror(ERR_DEFINEENUM, 0, 0);
        }
        if (prm_debug && prm_cplusplus)
            sp->istypedef = TRUE;
        global_flag--;
        head = copytype(sp->tp, 0);
    }
    else
    {
        /* untagged */
        tp = xalloc(sizeof(TYP));
        tp->type = bt_enum;
        tp->lst.head = tp->btp = 0;
        tp->size = stdenumsize;
        tp->bits = tp->startbit =  - 1;
        tp->sp = 0;
        if (lastst != begin)
            generror(ERR_PUNCT, begin, skm_declend);
        else
        {
            global_flag++;
            tp->enumlist = enumbody(0, table); /* just ignore the enum list */
            global_flag--;
        }
        head = tp;
    }
    global_flag = xglob;
}

//-------------------------------------------------------------------------

char *nexttagname(void)
{
    static int nexttag = 0;
    char buf[256];
    sprintf(buf, "$$UNNAMEDTAG%d", nexttag++);
    return litlate(buf);
}

//-------------------------------------------------------------------------

SYM *enumbody(SYM *sp_in, TABLE *table)
/*
 * read the enumeration constants in
 */
{
    long evalue;
    SYM *sp,  *list = 0,  **next = &list;
    evalue = 0;
    if (recordingTemplate)
    {
        templateWade();
        return list ;
    }
    getsym();
    while (lastst == id)
    {

        sp = makesym(sc_const);
        sp->value.i = evalue++;
        sp->name = litlate(lastid);
        sp->value.classdata.parentns = thisnamespace;
        if (sp_in)
            if (prm_cplusplus)
                sp->tp = sp_in->tp;
            else
                sp->tp = maketype(bt_int, stdintsize);
            else
        {
            if (prm_cplusplus)
                    sp->tp = copytype(&stdenum, 0);
                else
                    sp->tp = maketype(bt_int, stdintsize);
            sp->tp->sp = makesym(sc_type);
            sp->tp->sp->name = litlate("UNTAGGED ENUM");
            sp->tp->sp->tp = maketype(bt_enum, stdenumsize);
        }
        (*next) = sp;
        next = &(*next)->enumlist;
        if (prm_cplusplus || table ==  &lsyms)
            insert(sp, table);
        else
            insert(sp, gsyms);
        getsym();
        if (lastst == assign)
        {
            TYP *tp;
            ENODE *node;
            getsym();
            tp = exprnc(&node);
            if (node)
            {
                opt4(&node, FALSE);
                if (isintconst(node->nodetype))
                    evalue = node->v.i;
                else
                    generror(ERR_NEEDCONST, 0, 0);
            }
            else
                generror(ERR_NEEDCONST, 0, 0);
            sp->value.i = evalue++;
        }
        if (lastst == comma)
        {
            getsym();
            if (!prm_c99 && prm_ansi && lastst != id)
                generror(ERR_IDEXPECT, 0, 0);
        }
        else if (lastst != end)
            break;
    }
    needpunc(end, skm_declend);
    return list;
}

//-------------------------------------------------------------------------

void doreclone(TYP *tp)
{
    struct stab lst = tp->lst;
    int size = tp->size;
    while (tp->reclone)
    {
        TYP *s = tp->reclone;
        tp->reclone = 0;
        tp = s;
        tp->lst = lst;
        tp->size = size;
        tp->val_flag = NORMARRAY;
    } 
}

//-------------------------------------------------------------------------

void declstruct(TABLE *table, int ztype, int flags, int cppflags)
/*
 * declare a structure or union type
 */
{
    SYM *sp = 0;
    TYP *tp;
    TABLE *desttable = tagtable;
    LIST *tempargs = 0;
    SYM *tempparent = 0;
    char *nm, name[512];
    int xglob = global_flag;
    int structline = lineno;
    if (templateFunc)
    {
        if (ztype != bt_class && ztype != bt_struct)
            generror(ERR_TEMPLATECLASSFUNC, 0, 0);
    }
    if (lastst == id)
    {
        global_flag++;
        strcpy(name, lastid);
        /* tagged */
            if (prm_cplusplus)
            {
                getsym();
                if (currentTemplate && structlevel && table)
                    desttable = table;

                if (lastst == lt)
                {
                    SYM *tempsym = search2(name, tagtable, FALSE, TRUE);
                    if (tempsym && tempsym->istemplate)
                    {
                        if (currentTemplate)
                        {
                            SYM *spx = search("$$DETEMP", &tempsym->tp
                                ->lst.head);
                            if (!spx)
                            {
                                spx = makesym(sc_type);
                                spx->tp = maketype(bt_char, 0); // placeholder
                                spx->name = "$$DETEMP";
                                insert(spx, &tempsym->tp->lst.head);
                                spx->value.classdata.templateparent = tempsym;
                            }
                            desttable = &spx->tp->lst.head;
                        }
                        else
                            desttable = tagtable;
                        tempparent = tempsym;
                        tempargs = getTemplateName(tempsym, name);
                    }
                    else
                    {
                        generror(ERR_INVALIDTEMPLATE, 0, 0);
                        tplskm(skm_tempend);
                    }
                }
                nm = litlate(name);
                if (!increatingtemplateargs)
                {
                    if (prm_cplusplus && desttable && !sp && (!declclass || 
                        (lastst != begin && lastst != colon)))
                        sp = search2(nm, desttable, FALSE, TRUE);
                    if (sp && sp->storage_class == sc_type)
                    {
                        sp = parsetype(sp, TRUE);
                        if (sp->storage_class != sc_type)
                            generror(ERR_TYPENAMEEXP, 0, 0);
                        else if (sp->tp->type != ztype)
                            gensymerror(ERR_DECLMISMATCH, sp->name);
                    }
                    if (!sp && desttable)
                        sp = search2(nm, desttable, FALSE, TRUE);
                }
            }
            else
        {
            if (!increatingtemplateargs && desttable)
                sp = search2(nm = litlate(name), desttable, FALSE, TRUE);
            getsym();
        }
        if (!sp && declclass)
        {
            SYM *sp2 = declclass->parentclass;
            while (!sp && sp2)
            {
                sp = search2(nm, &sp2->tp->lst.head, FALSE, TRUE);
                sp2 = sp2->parentclass;
            }
        }

        if (sp && sp->istemplate && currentTemplate)
        {
            SYM *sp1 = sp->value.classdata.templatedata->classes.head;
            SYM *sp2 = currentTemplate->classes.head;
            while (sp1 && sp2)
            {
                if (sp2->defalt && sp1->defalt)
                {
                    generror(ERR_REDEFTEMPLATEDEFAULTS, 0, 0);
                    break;
                }
                sp1 = sp1->next;
                sp2 = sp2->next;
            }
        }
        if (sp && table ==  &lsyms && (lastst == semicolon || lastst == begin))
        {
            SYM *sp1 = 0;
            if (ltags.head != oldltag.head)
            {
                sp1 = ltags.head;
                while (sp1 && sp1 != oldltag.head)
                {
                    if (!strcmp(sp1->name, nm))
                        break;
                    sp1 = sp1->next;
                }
            }
            if (sp1 == 0 || sp1 == oldltag.head)
                sp = 0;
        }
        if (sp == 0)
        {
            nulljn: 
            /* if tag was never defined */
            sp = makesym(sc_type);
            sp->name = nm;
            sp->tp = xalloc(sizeof(TYP));
            sp->tp->type = ztype;
            sp->tp->lst.head = 0;
            sp->value.classdata.templatedata = templateFunc ? currentTemplate :
                0;
            sp->value.classdata.parentns = thisnamespace;
            sp->tp->sp = sp;
            sp->tp->cflags = flags;
            sp->tp->uflags = UF_DEFINED;
            sp->tp->size = 0;
            sp->tp->bits = sp->tp->startbit =  - 1;
            sp->dontlist = sys_inc;
                sp->value.classdata.cppflags |= cppflags;
                sp->value.classdata.templateparent = tempparent;
                sp->value.classdata.templateargs = tempargs;
            global_flag--;
            if (lastst == begin || ((ztype == bt_class || ztype == bt_struct)
                && lastst == colon))
            {
                    /* tags are also typedefs in C++ */
                    //                                    if (prm_cplusplus && table != gsyms)
                    //													insert(sp,table);
                    //												else
                if (!increatingtemplateargs && desttable)
                    insert(sp, desttable);
                sp->intagtable = TRUE;
                structbody(sp, ztype);
                browse_variable(sp, structline);
                if (prm_debug && prm_cplusplus)
                    sp->istypedef = TRUE;
            }
            else
            {
                //							if (!currentTemplate) {
                if (!increatingtemplateargs && desttable)
                    insert(sp, desttable);
                sp->intagtable = TRUE;
                //							}
                sp->tp->reclone = sp->tp;
            }
            if (prm_cplusplus && !currentTemplate && structlevel && desttable 
                != table)
            {
                sp = copysym(sp);
                insert(sp, table);
            }

        }
        else
        {
            if (ztype != sp->tp->type)
                generror(ERR_MISMATCH, 0, 0);

            global_flag--;
            /* Allow redefinition if it was forward declared */
            if (lastst == begin || ((ztype == bt_class || ztype == bt_struct)
                && lastst == colon))
            {
                if (sp->tp->size == 0)
                {
                    if (currentTemplate && sp->istemplate)
                    {
                        currentTemplate->classes.head = sp
                            ->value.classdata.templatedata->classes.head;
                        sp->value.classdata.templatedata = currentTemplate;
                    }

                    structbody(sp, ztype);
                    doreclone(sp->tp);
                    browse_variable(sp, structline);
                }
                else
                {
                    if (prm_cplusplus && (sp->parentclass || declclass))
                    {
                        if (sp->parentclass)
                            unlinkTag(sp);
                        desttable = 0;
                        goto nulljn;
                    }
                    else
                    {
                        gensymerror(ERR_DUPSYM, sp->name);
                        expskim(skm_declclosebr, FALSE);
                    }

                }
            }
        }
        head = copytype(sp->tp, flags);
    }
    else
    {
        global_flag++;
        /* untagged */
        tp = xalloc(sizeof(TYP));
        tp->type = ztype;
        tp->cflags = flags;
        tp->uflags = UF_DEFINED;
        tp->sp = sp;
        tp->lst.head = 0;
        tp->bits = tp->startbit =  - 1;
        sp = makesym(sc_type);
        sp->name = nexttagname();
        sp->tp = tp;
        sp->value.classdata.templatedata = templateFunc ? currentTemplate : 0;
        sp->dontlist = sys_inc;
        tp->sp = sp;
        if (lastst != begin)
            generror(ERR_PUNCT, begin, skm_declend);
        else
        {
            if (templateFunc)
                generror(ERR_TYPENAMEEXP, 0, 0);
            structbody(sp, ztype);
        }
        head = tp;
    }
    global_flag = xglob;
}

//-------------------------------------------------------------------------

int scanbases(CLASSLIST *l0, SYM *sp)
{
    CLASSLIST *l1;

    if (!l0)
        return 0;
    l1 = l0->link;
    while (l1)
    {
        if (!strcmp(l1->data->name, sp->name))
        {
            doubleerr(ERR_ALSOBASECLASS, sp->name, l0->data->name);
            return 1;
        }
        else
            if (scanbases(l1->data->value.classdata.baseclass, sp))
                return 1;
        l1 = l1->link;
    }
    return 0;
}

//-------------------------------------------------------------------------

void crossrefbaseclasses(CLASSLIST *l0, SYM *sp)
{
    if (l0)
    {
        l0 = l0->link;
        while (l0)
        {
            if (scanbases(l0, sp))
                return ;
            if (scanbases(sp->value.classdata.baseclass, l0->data))
                return ;
            l0 = l0->link;
        }
    }
}

//-------------------------------------------------------------------------

int replaceAllSizes(SYM *sp_in)
{
    int rv = 0;
    SYM *sp = sp_in->tp->lst.head;
    while (sp)
    {
        if (sp->storage_class == sc_defunc)
        {
            SYM *spc = sp->tp->lst.head;
            while (spc)
            {
                replaceSize(spc->tp->btp);
                spc = spc->next;
            }
        }
        else if (isstructured(sp->tp)) 
		{
            replaceSize(sp->tp);
            if (!(sp->tp->sp->value.classdata.cppflags & PF_NOTFULLYDEFINED))
                sp->tp->size = sp->mainsym->tp->size = replaceAllSizes(sp);
            if (sp_in->tp->type == bt_union)
                rv = max(rv,sp->tp->size);
            else
                rv = sp->value.i + sp->tp->size;
		}
		else if (sp->tp->type == bt_pointer)
		{
			TYP *t = sp->tp->btp;
			while (t->type == bt_pointer || t->type == bt_farpointer)
				t = t->btp;
			if (isstructured(t))
			{
				t->size = t->sp->mainsym->tp->size ;
			}
            if (sp_in->tp->type == bt_union)
                rv = max(rv,sp->tp->size);
            else
                rv = sp->value.i + sp->tp->size;
		}
        else
            if (sp_in->tp->type == bt_union)
                rv = max(rv,sp->tp->size);
            else
                rv = sp->value.i + sp->tp->size;
        sp = sp->next;
    }
//	rv += sp->value.classdata.virtualstructdatasize;
    return rv ;
}

//-------------------------------------------------------------------------

void structbody(SYM *sp, int ztype)
/*
 * read in the structure/union elements and calculate the total size
 */
{
    static struct _defFunc
    {
        struct _defFunc *next;
        SYM *sp;
        ILIST *l;
    }  *defFuncs;
    ILIST *odf = deferredFunctions;
    int slc, vv;
    int mode, basemode;
    int hascons = FALSE, hasvtab = FALSE;
    int flags = 0;
    SYM *dc,  *dco = declclass;
    int otlf = templateFunc;
    int pushed = FALSE;
    int decltype;
    int ocppflags = cppflags;
    int olastflexarray = encounteredflexarray;
    int sbittype = bittype ;
    int scurbit = curbit ;
    int ssizeoflastbit = sizeoflastbit;
	int vbc = 0;
        int otemp = templateLookup;
        LIST *nsusing = local_using_list;
        LIST *nstusing = local_tag_using_list;
        sp->mainsym->value.classdata.cppflags |= PF_NOTFULLYDEFINED;
        encounteredflexarray = 0;
        curbit = 100;
        bittype = -1;
        sizeoflastbit = 0;
        if (!structlevel)
            defFuncs = 0;
        structlevel++;
        deferredFunctions = 0;
        templateLookup = 0;
        templateFunc = 0;
    cppflags = 0;
        if (prm_cplusplus)
        {
            global_flag++;
            sp->value.classdata.size += classhead;
        }
        if (sp != declclass)
            sp->parentclass = declclass;
        else
            gensymerror(ERR_MEMBERSAMENAME, sp->name);
        if (ztype == bt_class)
        {
            sp->value.classdata.baseclass = xalloc(sizeof(CLASSLIST));
            sp->value.classdata.baseclass->data = sp;
            sp->value.classdata.baseclass->vtabsize = vtabhead;
            sp->value.classdata.baseclass->mode = BM_PUBLIC;
            dc = declclass;
            declclass = sp;
            flags |= PF_PRIVATE;
            basemode = mode = BM_PRIVATE;
            pushed = TRUE;
        }
        else if (prm_cplusplus && (ztype == bt_struct || ztype == bt_union ||
            ztype == bt_class))
        {
            sp->value.classdata.baseclass = xalloc(sizeof(CLASSLIST));
            sp->value.classdata.baseclass->data = sp;
            sp->value.classdata.baseclass->vtabsize = vtabhead;
            sp->value.classdata.baseclass->mode = BM_PUBLIC;
            dc = declclass;
            declclass = sp;
            if (ztype == bt_struct)
                ztype = bt_class;
            flags |= PF_PUBLIC;
            basemode = mode = BM_PUBLIC;
            pushed = TRUE;
        }
        if (sp->name && prm_cplusplus)
        {
            SYM *sp1 = makesym(sc_type);
            sp->value.classdata.baseclass->vtabsp = sp1;
            sp1->tp = &stdint;
            sp1->name = fullcppmangle(0, "$vtab", 0);
        }
    sp->tp->size = 0;
    sp->tp->val_flag = NORMARRAY;
    sp->tp->uflags &= ~UF_CANASSIGN;
        if (prm_cplusplus && lastst == colon)
        {
            int first = TRUE;
            sp->value.classdata.size = 0;
            while (TRUE)
            {
                getsym();
				if (lastst == kw_virtual)
					vbc =1 ;
                switch (lastst)
                {
                    case kw_private:
                        mode = BM_PRIVATE;
                        getsym();
                        break;
                    case kw_protected:
                        mode = BM_PROTECTED;
                        getsym();
                        break;
                    case kw_public:
                        mode = BM_PUBLIC;
                        getsym();
                        break;
                }
				if (lastst == kw_virtual) {
					if (vbc)
						generror(ERR_DUPVIRTUAL,0,0);
					vbc =1 ;
				}
                if (lastst != id)
                {
                    generror(ERR_IDEXPECT, 0, 0);
                }
                else
                {
                    SYM *odc = declclass;
                    SYM *spx;
                    declclass = 0;
                    spx = typesearch(lastid);
                    declclass = odc;

                    if (!spx || spx->storage_class != sc_type)
                        generror(ERR_TYPENAMEEXP, 0, skm_declbegin);
                    else
                    {
                        if (spx->tp->type == bt_union)
                            generror(ERR_UNIONNOBASE, 0, 0);
                        getsym();
                        if (first && !(spx->value.classdata.baseclass->flags
                            &CL_VTAB))
                            sp->value.classdata.size = classhead;
                        first = FALSE;
                        if (!strcmp(sp->name, spx->name))
                        {
                            doubleerr(ERR_ALSOBASECLASS, sp->name, sp->name);
                        }
                        else
                        {
                            crossrefbaseclasses(sp->value.classdata.baseclass,
                                spx);
                            loadclassdata(sp, spx, mode, vbc);								
                        }
                    }
                }
                if (lastst != comma)
                    break;
                mode = basemode;
            }
            if (lastst != begin)
                generror(ERR_PUNCT, begin, 0);
        }
        templateLookup = otemp;
        if (recordingTemplate && !dc)
        {
            sp->istemplate = TRUE;
            currentTemplate->hasbody = TRUE;
        }
    getsym();
    slc = sp->value.classdata.size;
    while (lastst != end && lastst != eof)
    {
        if (encounteredflexarray)
        {
            generror(ERR_FLEXARRAYLAST, 0, 0);
            encounteredflexarray = FALSE;
        }
            cppflags = 0;
            if (prm_cplusplus)
            {
                switch (lastst)
                {
                    case kw_private:
                        flags &= ~(PF_PRIVATE | PF_PUBLIC | PF_PROTECTED);
                        flags |= PF_PRIVATE;
                        getsym();
                        needpunc(colon, 0);
                        continue;
                    case kw_protected:
                        flags &= ~(PF_PRIVATE | PF_PUBLIC | PF_PROTECTED);
                        flags |= PF_PROTECTED;
                        getsym();
                        needpunc(colon, 0);
                        continue;
                    case kw_public:
                        flags &= ~(PF_PRIVATE | PF_PUBLIC | PF_PROTECTED);
                        flags |= PF_PUBLIC;
                        getsym();
                        needpunc(colon, 0);
                        continue;
                    case kw_using:
                        using_keyword();
                        continue;
                }
            }

            if (lastst == kw_typedef)
            {
                decltype = sc_type;
                getsym();
            }
            else
                decltype = sc_member;
            if (prm_cplusplus)
                flags |= PF_MEMBER;
            isexport = FALSE;
            isimport = FALSE;
            ispascal = FALSE;
            isstdcall = FALSE;
            isindirect = FALSE;
            iscdecl = FALSE;
            cppflags = 0;
            aFriend = FALSE;
            if (lastst == kw_template)
                createTemplate(&sp->tp->lst, decltype);
            else if (ztype == bt_struct || ztype == bt_class)
                slc += declare2(&sp->tp->lst, decltype, ztype, 0, flags, slc);
            else
                slc = imax(slc, vv = declare2(&sp->tp->lst, decltype, ztype, 0,
                    flags, 0));
            typequal = 0;
            if (sp->tp->lst.tail)
            {
				if (ztype == bt_union)
				{
					if (sp->structAlign == 0)
					{
						sp->structAlign = getalign(sc_member, sp->tp->lst.tail->tp);
					}
				}
				else
				{
					sp->structAlign = max(sp->structAlign, getalign(sc_member, sp->tp->lst.tail->tp));
				}
                sp->tp->lst.tail->tp->uflags |= UF_DEFINED;
                if (sp->tp->lst.tail->tp->type == bt_struct || sp->tp->lst.tail
                    ->tp->type == bt_class)
                {
                    CLASSLIST **l = &sp->value.classdata.enclosedclass;
                    while (*l)
                        l = &(*l)->link;
                    *l = xalloc(sizeof(CLASSLIST));
                    (*l)->data = sp->tp->lst.tail;
                    (*l)->offset = sp->tp->lst.tail->value.i;
                    (*l)->flags = CL_SECONDARY;
                }
                if (currentfunc && (sp->tp->lst.tail->tp->type == bt_ifunc ||
                    sp->tp->lst.tail->tp->type == bt_func))
                    generror(ERR_LOCALCLASSNOFUNC, 0, 0);
                if (((sp->tp->lst.tail->tp->val_flag & VARARRAY) || 
                    sp->tp->lst.tail->tp->type == bt_pointer &&
                    (sp->tp->lst.tail->tp->btp->val_flag & VARARRAY)))
                    generror(ERR_VLABLOCKSCOPE,0,0);
            }
//            if (lastst == kw_template)
//                createTemplate(&sp->tp->lst, sc_member);
//            else if (ztype == bt_struct || ztype == bt_class)
//                slc += declare2(&sp->tp->lst, sc_member, ztype, 0, flags, slc);
//            else
//                slc = imax(slc, vv = declare2(&sp->tp->lst, sc_member, ztype, 0,
//                    flags, 0));
        if (sp->tp->lst.tail && sp->tp->lst.tail->isunnamed)
            sp->hasunnamed = TRUE;
    }
    if (curbit < 100)
    {
        int size = (curbit - 1) / 8+1;
        if (packdata[packlevel] != 1)
        {
            int align = sp->structAlign;
            int val = size % align;
            if (val)
                size += align - val;
        }
        if (ztype == bt_struct || ztype == bt_class)
            slc += size;
        else
            slc = imax(slc, vv + size);
        curbit = 100;
    }
        if (prm_cplusplus)
        {
            CLASSLIST *l = sp->value.classdata.baseclass;
            while (l)
            {
                SYM *spc,  *spd;
                if (l->flags &CL_VTAB)
                {
                    hasvtab = !l->offset;
                    if (hasvtab)
                    {
                        hascons = TRUE;
                        break;
                    }
                }
                spc = search(cpp_funcname_tab[CI_CONSTRUCTOR], &l->data->tp
                    ->lst.head);
                spd = search(cpp_funcname_tab[CI_DESTRUCTOR], &l->data->tp
                    ->lst.head);
                if (spc || spd)
                    hascons = TRUE;
                l = l->link;
            }
            if (!hascons)
            {
                SYM *spc = search(cpp_funcname_tab[CI_CONSTRUCTOR], &sp->tp
                    ->lst.head);
                SYM *spd = search(cpp_funcname_tab[CI_DESTRUCTOR], &sp->tp
                    ->lst.head);
                if (spc || spd)
                    hascons = TRUE;
            }
            if (!hascons)
            {
                dc = sp->tp->lst.head;
                while (dc && !hascons)
                {
                    if (isstructured(dc->tp))
                    {
                        SYM *spc = search(cpp_funcname_tab[CI_CONSTRUCTOR], &dc
                            ->tp->lst.head);
                        SYM *spd = search(cpp_funcname_tab[CI_DESTRUCTOR], &dc
                            ->tp->lst.head);
                        if (spc || spd)
                            hascons = TRUE;
                    }
                    dc = dc->next;
                }
            }
            if (!hasvtab)
            {
                dc = sp->tp->lst.head;
                while (dc)
                {
                    if (dc->tp->type != bt_func && dc->tp->type != bt_ifunc &&
                        dc->tp->type != bt_defunc)
                        if (dc->value.i && dc->storage_class != sc_const)
                            dc->value.i -= classhead;
                    if (dc->tp->type == bt_defunc)
                    {
                        SYM *dc1 = dc->tp->lst.head;
                        while (dc1)
                        {
                            if (dc1->value.classdata.vtaboffs)
                                dc1->value.classdata.vtaboffs -= classhead;
                            dc1 = dc1->next;
                        }
                    }
                    else if (dc->tp->type == bt_ifunc || dc->tp->type ==
                        bt_func)
                        dc->value.classdata.vtaboffs -= classhead;
                    dc = dc->next;
                }
                l = sp->value.classdata.baseclass;
                while (l)
                {
                    if (l->offset)
                        l->offset -= classhead;
                    l = l->link;
                }
                slc -= classhead;
            }
            else
                hascons = TRUE;
        }
        if (slc && packdata[packlevel] != 1)  /* for array of structs */
        {
            int align = sp->structAlign;
            int val = slc % align;
            if (val)
                slc += align - val;
        }
        sp->tp->size = sp->value.classdata.size = slc ;

        if (!recordingTemplate)
        {
            if (deferredFunctions)
            {
                struct _defFunc *t = xalloc(sizeof(struct _defFunc));
                t->next = defFuncs;
                t->sp = sp;
                t->l = deferredFunctions;
                defFuncs = t;
            }
            if (structlevel == 1)
            {
                struct _defFunc *t = defFuncs;
                replaceAllSizes(sp);
                sp->mainsym->value.classdata.cppflags &= ~PF_NOTFULLYDEFINED;
                while (defFuncs)
                {
                    declclass = defFuncs->sp;
                    parseClassFuncs(defFuncs->l);
                    defFuncs = defFuncs->next;
                } 
            }
            sp->mainsym->value.classdata.cppflags &= ~PF_NOTFULLYDEFINED;
        }
        if (pushed)
        {
            if (!recordingTemplate)
            {
                classerrs(sp);
                gen_vtab(sp);
            }
            declclass = dco;
        }
        if (prm_cplusplus)
        {
            if (!recordingTemplate)
            {
                if (sp->tp->size == 0)
                    sp->tp->size++;
                if (hascons)
                {
                    sp->value.classdata.cppflags |= PF_CPPSTRUCT;
                    CreateBaseConstructor(sp);
                }
                //                  dump_instantiates() ;
            }
            global_flag--;
        }
        local_using_list = nsusing;
        local_tag_using_list = nstusing;
    templateFunc = otlf;
    getsym();
    structlevel--;
    cppflags = ocppflags;
    aFriend = FALSE;
    deferredFunctions = odf;
    encounteredflexarray = olastflexarray;
    bittype = sbittype ;
    curbit = scurbit ;
    sizeoflastbit = ssizeoflastbit;
}

//-------------------------------------------------------------------------

void check_used(void)
/*
 * At the end of compilition we check for some common cases where 
 * module-scoped variables are either missing or unused
 */
{
    int i;
    SYM *sp;
    for (i = 0; i < HASHTABLESIZE; i++)
    {
        if ((sp = (SYM*)((SYM **)gsyms)[i]) != 0)
        {
            while (sp)
            {
                if (sp->storage_class == sc_static)
                {
                    if (sp->tp->uflags &UF_USED)
                    {
                        if (sp->tp->type == bt_func)
                            gensymerror(ERR_NOSTATICFUNC, sp->name);
                    }
                    else
                        if (sp->tp->type == bt_ifunc || sp->tp->type == bt_func)
                            gensymerror(ERR_FUNCUNUSED, sp->name);
                        else
                            gensymerror(ERR_STATICSYMUNUSED, sp->name);
                }
                else if (isstructured(sp->tp))
                {
                    if (sp->tp->size == 0)
                        gensymerror(ERR_NEVERSTRUCT, sp->name);
                }
                sp = sp->next;
            }
        }
    }
    if (!prm_cplusplus)
    for (i = 0; i < HASHTABLESIZE; i++)
    {
        sp = ((HASHREC **)tagtable)[i];
        while (sp)
        {
            if (sp->tp->size == 0)
                gensymerror(ERR_NEVERSTRUCT, sp->name);
            sp = sp->next;
        }
    }
}

//-------------------------------------------------------------------------

void compile(void)
/*
 * Main compiler routine
 */
{
    while (lastst != eof)
    {
        dodecl(sc_global);
        if (lastst != eof)
        {
            generror(ERR_DECLEXPECT, 0, skm_declend);
            getsym();
        }
    }
    #ifdef ICODE
        rewrite_icode();
    #else 
        //            flush_peep();
    #endif 
    dumpXceptTabs();
    template_rundown();
    dump_instantiates();
    dumplits();
    initrundown();
    #ifndef ICODE
        dump_muldivval();
    #endif 
    dumpstartups();
    if (prm_debug)
        debug_dumpglobaltypedefs();
    check_used();
    putexterns();
    if (!prm_asmfile)
        output_obj_file();
}

//-------------------------------------------------------------------------

void insertTemplateChar(short t)
{
    if (templatebufIndex >= templatebufSize)
    {
        short *buf1 = xalloc((templatebufSize + 1000) *sizeof(short));
        memcpy(buf1, templatebuf, templatebufIndex *sizeof(short));
        templatebufSize += 1000;
        templatebuf = buf1;
    }
    if (t == 0)
    {
        while (templatebufIndex && templatebuf[templatebufIndex - 1] != ';' &&
            templatebuf[templatebufIndex - 1] != '}')
            templatebufIndex--;
        templatebuf[templatebufIndex++] = '\n';
    }
    templatebuf[templatebufIndex++] = t;
    if (t == 0)
    {
        if (currentTemplate)
        {
            currentTemplate->lines = templatebuf;
            currentTemplate->linemax = templatebufSize;
            currentTemplate->linelen = templatebufIndex;
        }
        templatebufSize = templatebufIndex = 0;
    }
}

//-------------------------------------------------------------------------

void templateWade(void)
{
    int indent = lastst == begin ? 1 :  - 1;
    if (lastst == colon || lastst == begin)
    {
        while (indent)
        {
            getsym();
            switch (lastst)
            {
                case begin:
                    if (indent < 0)
                        indent++;
                    indent++;
                    break;
                case end:
                    indent--;
                    break;
                default:
                    break;
                case eof:
                    generror(ERR_TEMPLATEEOF, 0, 0);
                    templateFunc = recordingTemplate = FALSE;
                    currentTemplate = 0;
                    return ;
            }

        }
    }
    getsym();
}

//-------------------------------------------------------------------------

TYP *wadeTemplateArg(SYM *sp)
{
    TYP *tp;
    int i = 0;
    char buf[512];
    char nesting[512],  *n = nesting;
    while (i < 511)
    {
        int rv = getch();
        if (rv == EOF)
            return 0;
        if (rv == '(' || rv == '[')
            *n++ = rv;
        else if (rv == ')')
            while (n > nesting &&  *--n != '(')
                ;
            else if (rv == ']')
                while (n > nesting &&  *--n != '[')
                    ;
        if (n == nesting && (rv == ',' || rv == '>'))
            break;
        buf[i++] = rv;
    }
    buf[i] = 0;
    tp = maketype(bt_string, i);
    tp->sp = makesym(sc_tconst);
    tp->lst.tail = litlate(buf);
    tp->lst.head = sp;
    getsym();
    return tp;
}

//-------------------------------------------------------------------------

void createTemplate(TABLE *tbl, enum e_sc sc)
{
    SYM *sp;
    int count = 0;
    int hasdefalt = 0;
    getsym();
    if (currentTemplate)
    {
        expskim(skm_declend, FALSE);
        getsym();
        return ;
    } if (lastst != lt)
    {
        generror(ERR_INVALIDTEMPLATE, 0, 0);
        tplskm(skm_tempend);
    }
    getsym();
    currentTemplate = xalloc(sizeof(struct template ));
    increatingtemplateargs = TRUE;
    while (TRUE)
    {
        if (lastst == kw_typename)
            lastst = kw_class;
        if (!castbegin(lastst))
            break;
        nodeclfunc = TRUE;
        declid[0] = 0;
        typequal = 0;
        decl(0, 0); /* do cast declaration */
        headptr = &head;
        decl1(sc_type, 0);
        nodeclfunc = FALSE;
        if (head->type == bt_struct || head->type == bt_union || head->type ==
            bt_ref || head->type == bt_void)
            generror(ERR_SIMPLETEMPLATEARGS, 0, 0);
        if (head->type == bt_class)
        {
            sp = head->sp;
            sp->tp->size = stdintsize;
        } 
        else
        {
            sp = makesym(sc_type);
            sp->name = litlate(declid);
            sp->tp = head;
            sp->tp->sp = sp;
        }
        if (lastst == assign)
        {
            sp->defalt = wadeTemplateArg(sp);
            hasdefalt = 1;
        }
        else if (hasdefalt)
            gensymerror(ERR_MISSINGDEFAULT, sp->name);
        sp->istemplatevar = TRUE;
        if (currentTemplate->classes.head)
            currentTemplate->classes.tail = currentTemplate->classes.tail->next
                = sp;
        else
            currentTemplate->classes.head = currentTemplate->classes.tail = sp;
        count++;
        if (lastst == comma)
        {
            getsym();
        }
        else
            break;
    }
    increatingtemplateargs = FALSE;
    if (lastst != gt)
    {
        generror(ERR_INVALIDTEMPLATE, 0, 0);
        tplskm(skm_tempend);
        currentTemplate = 0;
    }
    else
    {
        recordingTemplate = TRUE;
        templateFunc = TRUE;
        templateLookup = TRUE;
        currentTemplate->lineno = lineno;
        currentTemplate->filename = litlate(infile);
        currentTemplate->tplargcount = count;
        insertTemplateChar(lastch);
        getsym();
    }
    declare(tbl, sc, bt_struct, DF_GLOBAL, 0);
    recordingTemplate = FALSE;
    insertTemplateChar(0); /* null terminator = end of stream */
    templateFunc = FALSE;
    templateLookup = FALSE;
    currentTemplate = 0;
}

//-------------------------------------------------------------------------

void using_keyword(void)
{
    if (prm_cplusplus)
    {
        getsym();
        if (lastst == kw_namespace)
        {
            getsym();
            if (lastst != id)
                generror(ERR_NAMESPACEEXP, 0, 0);
            else
            {
                inject_namespace(TRUE);
            }
        }
        else
        {
            if (lastst != id)
                generror(ERR_IDEXPECT, 0, 0);
            else
            {
                inject_namespace(FALSE);
            }
        }
        needpunc(semicolon, 0);
    }
}

//-------------------------------------------------------------------------

void dodecl(int defclass)
/*
 * Declarations come here, ALWAYS
 */
{
    SYM *sp;
    int flags = 0;
    long val;
    char *nm;
    cbautoinithead = 0;
    farflag = FALSE;
    segflag = FALSE;
    for (;;)
    {
        isexport = FALSE;
        isimport = FALSE;
        ispascal = FALSE;
        isstdcall = FALSE;
        isindirect = FALSE;
        iscdecl = FALSE;
        aFriend = FALSE;
        cppflags = 0;
        typequal = 0;
        switch (lastst)
        {
            case semicolon:
                getsym();
                break;
            case kw_typedef:
                getsym();
                if (defclass == sc_global)
                    declare(gsyms, sc_type, bt_struct, 0, 0);
                else
                    declare(&lsyms, sc_type, bt_struct, 0, 0);
                break;
                case kw_template:
                    if (lastst == kw_template)
                        if (defclass == sc_global)
                            createTemplate(gsyms, defclass);
                        else
                            createTemplate(&lsyms, defclass);
                    break;
            case kw_register:
                if (defclass != sc_auto || flags &DF_VOL)
                {
                    gensymerror(ERR_ILLCLASS, lastid);
                    getsym();
                }
                else
                {
                    getsym();
                    declare(&lsyms, sc_autoreg, bt_struct, flags | DF_AUTOREG,
                        0);
                }
                ;
                break;
            case kw_using:
                using_keyword();
                break;
                case kw_namespace:
                    if (prm_cplusplus)
                    {
                        char *name = lastid;
                        int braceok = TRUE;
                        getsym();
                        if (global_flag)
                        {
                            if (lastst != id)
                            {
                                name = 0;
                            }
                            else
                            {
                                getsym();
                                if (lastst == assign)
                                {
                                    braceok = FALSE;
                                    aliasnamespace(name);
                                    needpunc(semicolon, 0);
                                }
                            }
                            if (braceok && lastst == begin)
                            {
                                switchtonamespace(name);
                                getsym();
                                dodecl(defclass);
                                needpunc(end, 0);
                                if (lastst == semicolon)
                                    getsym();
                                switchfromnamespace();
                            }
                        }
                        else
                            generror(ERR_NOLOCALNAMESPACE, 0, 0);
                        break;
                    }
                    lastst = id;
            case id:
                if (defclass == sc_auto)
                    if (!castbegin(lastst))
                        return ;
                /* fall through */
                /* fall through */
            case kw_restrict:
            case kw_explicit:
            case kw__import:
            case kw__export:
            case kw_class:
            case kw_operator:
            case kw_volatile:
            case kw__intrinsic:
            case kw_const:
            case kw_virtual:
            case kw_inline:
            case kw_friend:
            case kw_typeof:
            case kw_char:
            case kw_int:
            case kw_short:
            case kw_unsigned:
            case kw_long:
            case kw_struct:
            case kw_union:
            case kw_signed:
            case kw_enum:
            case kw_void:
            case kw_bool:
            case kw_float:
            case kw_double:
            case kw_wchar_t:
            case kw___int64:
            case kw_typename:
                if (defclass == sc_global)
                    declare(gsyms, sc_global, bt_struct, flags | DF_GLOBAL, 0);
                else if (defclass == sc_auto)
                    declare(&lsyms, sc_auto, bt_struct, flags, 0);
                else
                    declare(&lsyms, sc_auto, bt_struct, flags, 0);
                break;
            case kw_static:
                if (defclass == sc_member)
                {
                    gensymerror(ERR_ILLCLASS, lastid);
                    getsym();
                    break;
                }
                getsym();
                if (defclass == sc_auto)
                    declare(&lsyms, sc_static, bt_struct, flags | DF_GLOBAL, 0);
                else
                    declare(gsyms, sc_static, bt_struct, flags | DF_GLOBAL, 0);
                break;
            case kw_auto:
                if (defclass != sc_auto)
                {
                    errlineno = lineno;
                    gensymerror(ERR_ILLCLASS, lastid);
                    getsym();
                    break;
                }
                getsym();
                declare(&lsyms, sc_auto, bt_struct, flags, 0);
                break;
            case kw_extern:
                {
                    int cblock = FALSE;
                    getsym();
                    if (defclass == sc_member)
                        gensymerror(ERR_ILLCLASS, lastid);
                        if (prm_cplusplus && lastst == sconst)
                        {
                            if (!strcmp(laststr, Cstr))
                            {
                                mangleflag = !++manglelevel;
                                cblock = TRUE;
                                getsym();
                                if (lastst == begin)
                                {
                                    getsym();
                                    dodecl(defclass);
                                    needpunc(end, 0);
                                }
                                else
                                {
                                    ++global_flag;
                                    declare(gsyms, sc_external, bt_struct,
                                        flags, 0);
                                    --global_flag;
                                }
                                mangleflag = !--manglelevel;
                            }
                        }
                    if (!cblock)
                    {
                        ++global_flag;
                        declare(gsyms, sc_external, bt_struct, flags, 0);
                        --global_flag;
                    }
                }
                break;
            case kw__interrupt:
                intflag = 1;
                flags |= DF_INT;
                getsym();
                continue;
            case kw__fault:
                intflag = 1;
                flags |= DF_FAULT;
                getsym();
                continue;
            case kw__loadds:
                flags |= DF_LOADDS;
                getsym();
                continue;
            case kw__seg:
                segflag = 1;
                flags |= DF_SEG;
                getsym();
                continue;
            case kw__far:
                #ifdef i386
                    if (prm_farkeyword)
                    {
                        farflag = 1;
                        flags |= DF_FAR;
                    }
                    getsym();
                    continue;
                #endif 
            case kw__near:
                getsym();
                continue;
            case kw__abs:
                ++global_flag;
                getsym();
                if (lastst != openpa)
                {
                    generror(ERR_PUNCT, openpa, skm_declend);
                }
                else
                {
                    getsym();
                    val = intexpr(0);
                    if (lastst != closepa)
                        generror(ERR_PUNCT, closepa, skm_declend);
                    else
                    {
                        getsym();
                        declare2(gsyms, sc_abs, bt_struct, flags, 0, val);
                    }
                }
                --global_flag;
                break;
            default:
                return ;
        }
        flags = 0;
        intflag = 0;
        farflag = 0;
        segflag = 0;
    }
}

/*
 * in C++, a class/struct declarator can come first on a line but not
 * be a type specifier
 */
int blockdecl(void)
{
    if (lastst != kw_operator)
        dodecl(sc_auto);
    return 0;
}

//-------------------------------------------------------------------------

void doargdecl(int defclass, char *names[], int *nparms, TABLE *table, int
    isinline)
/*
 * Function arguments are declared here
 */
{
	int xispascal = ispascal;
	int xisstdcall =  isstdcall;
	int xiscdecl = iscdecl;
	int xisindirect = isindirect;
	int xisexport = isexport;
	int xisimport = isimport;
    SYM *sp;
    int flags = isinline ? DF_FUNCPARMS : 0;
    int ocppf = cppflags, ofr = aFriend;
    aFriend = FALSE;
    cppflags = 0;
    inprototype = isinline;
    funcparmtable = table; // for VLA support
    for (;;)
    {
		ispascal = 0;
		isstdcall =  0;
		iscdecl = 0;
		isindirect = 0;
		isexport = 0;
		isimport = 0;
        switch (lastst)
        {
            case ellipse:
                {
                    sprintf(declid, "**ELLIPSE%d**", pcount++);
                    sp = makesym(sc_auto);
                    sp->name = litlate(declid);
                    sp->tp = maketype(bt_ellipse, 0);
                    sp->tp->uflags |= UF_DEFINED | UF_USED;
                    insert(sp, table);
                    if (ispascal || isstdcall)
                        generror(ERR_PASCAL_NO_ELLIPSE, 0, 0);
                    getsym();
                    goto exit;
                }
            case kw_const:
            case kw_volatile:
            case kw_restrict:
            case kw_explicit:
            case id:
            case kw_typeof:
            case kw_char:
            case kw_int:
            case kw_short:
            case kw_unsigned:
            case kw_long:
            case kw_struct:
            case kw_union:
            case kw_signed:
            case kw_enum:
            case kw_void:
            case kw_bool:
            case kw_float:
            case kw_double:
            case kw_wchar_t:
            case kw_class:
            case kw___int64:
            case kw__Complex:
            case kw__Imaginary:
                declare(table, sc_auto, bt_struct, flags, 0);
                break;
            case kw_register:
                getsym();
//                #ifdef COLDFIRE
                    if (lastst >= kw_D0 && lastst <= kw_A8)
                    {
                        flags |= DF_FORCEREG;
                        flags |= (lastst - kw_D0) << 24;
                        getsym();
                        declare(&lsyms, sc_autoreg, bt_struct, flags |
                            DF_AUTOREG, 0);
                    }
                    else
//                #endif 
                    continue;
            case kw_static:
            case kw_auto:
                errlineno = lineno;
                gensymerror(ERR_ILLCLASS, lastid);
                getsym();
                continue;
            default:
                goto exit;
        }
        if (isinline)
        {
            names[(*nparms)++] = litlate(declid);
        }

        flags &= ~DF_CONST;
        typequal = 0;
    }
    exit: typequal = 0;
    inprototype = FALSE;
    {
            SYM *sp = table->head;
            int found = FALSE;
            while (sp)
            {
                if (prm_cplusplus) {
                    if (sp->value.classdata.defalt)
                        found = TRUE;
                    else
                        if (found)
                            gensymerror(ERR_MISSINGDEFAULT, sp->name);
                }
                // if an argument is a plain function, it must be converted to
                // a pointer to function
                if (sp->tp->type == bt_func) {
                    TYP *tp = maketype(bt_pointer,stdaddrsize);
                    tp->btp = sp->tp;
                    sp->tp = tp;
                }
                sp = sp->next;
            }
    }
    funcparmtable = 0;
    cppflags = ocppf;
    aFriend = ofr;
	ispascal = xispascal;
	isstdcall =  xisstdcall;
	iscdecl = xiscdecl;
	isindirect = xisindirect;
	isexport = xisexport;
	isimport = xisimport;
}
