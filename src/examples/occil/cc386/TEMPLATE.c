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
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "ccerr.h"

extern LIST *varlisthead,  *varlisttail;
extern int virtualdata;
extern int stdaddrsize;
extern int laststrlen;
extern char laststr[];
extern int intemplateargs;
extern int locsize;
extern BLK *locblk;
extern int lastch;
extern enum e_sym lastst;
extern int lineno, errlineno;
extern char *infile,  *errfile;
extern int prm_cmangle;
extern int global_flag;
extern HASHREC **templateFuncs;
extern TYP *head,  **headptr,  *tail;
extern TABLE *tagtable;
extern int prm_cmangle;
extern char lastid[];
extern SYM *currentfunc;
extern SYM *lastfunc;
extern TABLE lsyms, oldlsym, ltags, oldltag;
extern int goodcode;
extern TYP *asntyp,  *andtyp;
extern SYM  *typequal,  *declclass;
extern SNODE *funcendstmt;
extern SNODE *cbautoinithead,  *cbautoinittail,  *cbautorundownhead, 
    *cbautorundowntail;
extern int block_nesting;
extern ENODE *block_rundown;
extern int defaulttype;
extern int recordingTemplate;
extern short unalteredline[];
extern char *cpp_funcname_tab[];
extern int templateLookup;
extern struct template  *currentTemplate;
extern TABLE *gsyms;
extern int manglenamecount;
extern int conscount;
extern int cppflags;
extern ENODE *allocaSP, *blockVarArraySP;
extern int used_alloca;
int statictemplate;

char *templatePrefix;
short *interjectptr;
SYM *templateSP;
int virtualfuncs;

static int matching;

static struct codecontext
{
    struct codecontext *next;
    char id[256];
    int defaulttype;
    int blocknest;
    int blockrundown;
    SNODE *cbautoinithead,  *cbautoinittail,  *cbautorundownhead, 
        *cbautorundowntail;
    SNODE *funcendstmt;
    SYM *asntyp,  *andtyp,  *typequal,  *declclass;
    int goodcode;
    int chhold, sthold;
    int line, errline;
    char *file,  *errfile;
    SYM *cfhold;
    TABLE xlsyms, xoldlsyms, xltags, xoldltags;
    short *interject;
    TYP *head,  *tail,  **headptr;
    int oldvirtual;
    LIST *vlisthead,  *vlisttail;
    int conscount;
    int statictemplate;
	ENODE *allocaSP;
	ENODE *blockVarArraySP;
	int used_alloca;
}  *contextbase;

void template_init()
{
    interjectptr = 0;
    templateSP = 0;
    virtualfuncs = 0;
    virtualdata = 0;
    matching = FALSE;
    statictemplate = 0;
}

//-------------------------------------------------------------------------

void template_rundown(void){}
static void savecontext(void)
{
    struct codecontext *con = xalloc(sizeof(struct codecontext));
    con->next = contextbase;
    contextbase = con;
    con->head = head;
    con->tail = tail;
    con->headptr = headptr;
    headptr = &head;
    head = tail = 0;
    con->errfile = errfile;
    con->interject = interjectptr;
    con->line = lineno;
    con->errline = errlineno;
    con->file = infile;
    strcpy(con->id, lastid);
    con->cbautoinithead = cbautoinithead;
    con->cbautoinittail = cbautoinittail;
    con->cbautorundownhead = cbautorundownhead;
    con->cbautorundowntail = cbautorundowntail;
    cbautoinithead = 0;
    cbautoinittail = 0;
    cbautorundownhead = 0;
    cbautorundowntail = 0;
    con->blockrundown = block_rundown;
    con->blocknest = block_nesting;
    block_rundown = 0;
    block_nesting = 0;
    con->funcendstmt = funcendstmt;
    con->xlsyms = lsyms;
    con->xoldlsyms = oldlsym;
    con->xltags = ltags;
    con->xoldltags = oldltag;
    lsyms.head = lsyms.tail = 0;
    oldlsym.head = oldlsym.tail = 0;
    oldltag.head = oldltag.tail = 0;
    con->goodcode = goodcode;
    con->asntyp = asntyp;
    asntyp = 0;
    con->andtyp = andtyp;
    andtyp = 0;
    con->typequal = typequal;
    typequal = 0;
    con->declclass = declclass;
    declclass = 0;
    con->defaulttype = defaulttype;
    con->cfhold = currentfunc;
    currentfunc = 0;
    con->chhold = lastch;
    con->sthold = lastst;
    con->oldvirtual = virtualfuncs;
    con->vlisthead = varlisthead;
    varlisthead = 0;
    con->vlisttail = varlisttail;
    varlisttail = 0;
    con->conscount = conscount;
    conscount = 0;
    con->statictemplate = statictemplate;
	con->used_alloca = used_alloca;
	used_alloca = FALSE;
	con->allocaSP = allocaSP ;
	allocaSP = NULL;
	con->blockVarArraySP = blockVarArraySP ;
	blockVarArraySP = NULL;
}

//-------------------------------------------------------------------------

static void restorecontext(void)
{
    struct codecontext *con = contextbase;
    contextbase = contextbase->next;
    head = con->head;
    tail = con->tail;
    headptr = con->headptr;
    errfile = con->errfile;
    interjectptr = con->interject;
    lastst = con->sthold;
    lastch = con->chhold;
    currentfunc = con->cfhold;
    virtualfuncs = con->oldvirtual;
    lsyms = con->xlsyms;
    oldlsym = con->xoldlsyms;
    ltags = con->xltags;
    oldltag = con->xoldltags;
    goodcode = con->goodcode;
    asntyp = con->asntyp;
    andtyp = con->andtyp;
    typequal = con->typequal;
    declclass = con->declclass;
    funcendstmt = con->funcendstmt;
    cbautoinithead = con->cbautoinithead;
    cbautoinittail = con->cbautoinittail;
    cbautorundownhead = con->cbautorundownhead;
    cbautorundowntail = con->cbautorundowntail;
    block_nesting = con->blocknest;
    block_rundown = con->blockrundown;
    defaulttype = con->defaulttype;
    strcpy(lastid, con->id);
    lineno = con->line;
    errlineno = con->errline;
    infile = con->file;
    varlisthead = con->vlisthead;
    varlisttail = con->vlisttail;
    conscount = con->conscount;
    statictemplate = con->statictemplate ;
	used_alloca = con->allocaSP;
	allocaSP = con->allocaSP;
	blockVarArraySP = con->blockVarArraySP ;
}

//-------------------------------------------------------------------------

static int tcmp(short *p, short *str, int *len)
{
    int rv = 0;
    int i = 0;
    do
    {
        while (isspace(*p))p++, rv++;
        while (isspace(*str))
            str++, i++;
        if (i >=  *len)
        {
            *len = rv;
            return 0;
        } if (*p !=  *str)
            return 1;
        p++, str++, i++, rv++;
    }
    while (i <  *len)
        ;
    return 0;
}

//-------------------------------------------------------------------------

static short *matchname(short *s, short *str, int *len, int nolt)
{
    short *p = s;
    if (!p)
        return 0;
    while (*p)
    {
        if (p[0] == str[0])
        if (p == s || !issymchar(p[ - 1]))
        {
            int xlen =  *len;
            if (!tcmp(p, str, &xlen))
                if (!issymchar(p[xlen]))
            if (!nolt)
            {
                *len = xlen;
                return p;
            }
            else
            {
                char *t = p +  *len;
                while (isspace(*t))
                    t++;
                if (*t == '<')
                    return 0;
                *len = xlen;
                return p;
            }
        }
        p++;
    }
    return 0;
}

//-------------------------------------------------------------------------

static short *doreplace(short *buf, int *max, int *len, short *find, short
    *replace, int slen, int flen, int nolt)
{
    short *s = buf;
    int sslen = slen;
    while (s = matchname(s, find, &sslen, nolt))
    {
        if (*max <=  *len + flen - sslen)
        {
            short *buf2 = xalloc(*max + 1000 * sizeof(short));
            *max += 1000;
            memcpy(buf2, buf, (*len + 1) *sizeof(short));
            s = s + (int)(buf2 - buf);
            buf = buf2;
        }
        if (flen > sslen)
            memmove(s + flen - sslen, s, (*len - (s - buf) + 1) *sizeof(short));
        else
            memcpy(s, s + sslen - flen, (*len - (s + sslen - flen - buf) + 1)
                *sizeof(short));
        *len += flen - sslen;
        memcpy(s, replace, flen *sizeof(short));
        s += flen;
        sslen = slen;
    }
    return buf;
}

//-------------------------------------------------------------------------

static short *llreplace2(short *buf, int *max, int *len, char *find, char
    *replace)
{
    short formattedtype[512];
    short searchfor[256];
    char *p = replace;
    int flen = 0, slen = 0;
    while (*p)
        if (*p == '@' ||  *p == '$' ||  *p == '#')
            formattedtype[flen++] = '_', p++;
        else
            formattedtype[flen++] =  *p++;
    p = find;
    while (*p)
        searchfor[slen++] =  *p++;
    if (prm_cmangle && searchfor[0] == '_')
    {
        memcpy(searchfor, searchfor + 1, slen *sizeof(short));
        slen--;
    }
    return doreplace(buf, max, len, searchfor, formattedtype, slen, flen, TRUE);
}

//-------------------------------------------------------------------------

static short *llreplace(short *buf, int *max, int *len, TYP *to, TYP *tn)
{
    char type[512];
    short formattedtype[512];
    short searchfor[256];
    int slen;
    int flen;
    int i;
    //   if (to->type != bt_class)
    //   	return buf ;
    slen = strlen(to->sp->name);
    for (i = 0; i < slen; i++)
        searchfor[i] = to->sp->name[i];
    if (prm_cmangle && searchfor[0] == '_')
    {
        memcpy(searchfor, searchfor + 1, slen *sizeof(short));
        slen--;
    }
    if (to->type != bt_class)
    {
        if (scalarnonfloat(to))
        {
            if (to->type == bt_char || to->type == bt_unsignedchar)
            {
                #ifdef CC68K
                    sprintf(type, "'%c'", tn->sp->value.i + 3);
                #else 
                    sprintf(type, "'%c'", tn->sp->value.i);
                #endif 
            }
            else
                sprintf(type, "%d", tn->sp->value.i);
        }
        else
            sprintf(type, "%f", tn->sp->value.i);
        flen = strlen(type);
        for (i = 0; i < flen; i++)
            formattedtype[i] = type[i];
    }
    else if (tn->type == bt_string)
    {
        flen = tn->size;
        for (i = 0; i < flen; i++)
            formattedtype[i] = ((char*)tn->lst.head)[i];
    }
    else
    {
        typenum(type, tn);
        flen = strlen(type);
        for (i = 1; i < flen - 1; i++)
            formattedtype[i - 1] = type[i];
        flen -= 2;
    }
    return doreplace(buf, max, len, searchfor, formattedtype, slen, flen, FALSE)
        ;
}

//-------------------------------------------------------------------------

static short *replace2(char *buf, int *max, int *len, LIST *to, LIST *tn)
{
    short *buf2;
    global_flag++;
    buf2 = xalloc(*max * sizeof(short));
    memcpy(buf2, buf,  *max * sizeof(short));
    buf = buf2;
    while (to && tn)
    {
        if (((TYP*)to->data)->sp->istemplatevar)
            buf = llreplace(buf, max, len, to->data, tn->data);
        to = to->link;
        tn = tn->link;
    }
    global_flag--;
    return buf;
}

//-------------------------------------------------------------------------

static short *replace(struct _templatelist *tl, struct template  *tm)
{
    int max = tm->linemax;
    int len = tm->linelen;
    short *buf;
    LIST *to = tl->oldtypes;
    LIST *tn = tl->newtypes;
    return replace2(tm->lines, &max, &len, to, tn);
} void setunaltered(void)
{
    short *p = interjectptr;
    short *q = unalteredline;
    while (*p &&  *p != '\n')
        *q++ =  *p++;
    *q++ = '\n';
    *q++ = 0;
}

//-------------------------------------------------------------------------

int tlookup(TYP *t, struct template  *tpl)
{
    SYM *s1;
    SYM *s2 = tpl->classes.head;
    int i = 0;
    if (t->type == bt_ref)
        t = t->btp;
    if (!isstructured(t))
        return  - 1;
    s1 = t->sp;
    for (i = 0; s2; i++, s2 = s2->next)
    {
        if (s2->tp->type != bt_class)
            continue;
        if (exactype(s1, s2, FALSE))
            return i;
    } return  - 1;
}

//-------------------------------------------------------------------------

int tlookuptp(TYP *f, TYP *t, struct template  *tpl, TYP **rv)
{
    if (t->type == bt_ref)
        t = t->btp;
    if (t->type == bt_templateplaceholder)
    {
        if (f->sp->value.classdata.templateparent && f->sp
            ->value.classdata.templateparent->mainsym == t->lst.head->mainsym)
        {
            LIST *tn = t->lst.tail;
            LIST *fn = f->sp->value.classdata.templateargs;
            while (tn && fn)
            {
                int index = tlookup(tn->data, tpl);
                if (index !=  - 1)
                {
                    *rv = fn->data;
                    return index;
                } if (!isstructured(tn->data))
                {
                    SYM *s2 = tpl->classes.head;
                    int i = 0;
                    while (s2)
                    {
                        if (!isstructured(s2->tp) && !strcmp(s2->name, ((TYP*)
                            tn->data)->sp->name))
                        {
                            *rv = fn->data;
                            return i;
                        }
                        i++;
                        s2 = s2->next;
                    }
                }
                index = tlookuptp(fn, tn, tpl, rv);
                if (index >= 0)
                    return index;
                fn = fn->link;
                tn = tn->link;
            }
        }
    }
    return  - 1;
}

//-------------------------------------------------------------------------

static struct _templatelist *conjoint(SYM *fi, SYM *ti, SYM *sp, char *name)
{
    char mname[512], mname2[512],  *p;
    struct _templatelist *rv;
    struct template  *tpl = sp->value.classdata.templatedata;
    TYP *tp[100];
    LIST **t1,  **x1;
    SYM *f = fi;
    SYM *t = ti;
    int i, partial = 0;
    int max = 0;
    SYM *s = tpl->classes.head;
    #ifdef XXXXX
        while (s)
        {
            if (s->tp->type != bt_class)
                return 0;
            s = s->next;
        } 
    #endif 
    memset(tp, 0, sizeof(tp));
    while (f && t)
    {
        int index;
        index = tlookup(t->tp, tpl);
        if (index < 0)
        {
            if (!exactype(f->tp, t->tp, FALSE))
            {
                TYP *x;
                index = tlookuptp(f->tp, t->tp, tpl, &x);
                if (index < 0)
                    return 0;
                if (index >= max)
                    max = index + 1;
                if (tp[index])
                {
                    if (!exactype(x, tp[index], FALSE))
                        return 0;
                }
                else
                    tp[index] = x;
            }
            else
                partial++;
        }
        else
        {
            if (index >= max)
                max = index + 1;
            if (tp[index])
            {
                if (!exactype(f->tp, tp[index], FALSE))
                    return 0;
            }
            else
                tp[index] = f->tp;
        }
        f = f->next;
        t = t->next;
    }
    if (f || t)
        return 0;
    t = tpl->classes.head;
    for (i = 0; t; i++, t = t->next)
    {
        if (!tp[i])
        {
            if (t->defalt)
                tp[i] = t->defalt;
            else
                return 0;
        }
    }
    if (prm_cmangle && name[0] == '_')
        name++;
    sprintf(mname, "@#%s$t", name);
    p = mname + strlen(mname);
    manglenamecount = 0;
    for (i = 0; i < max; i++)
    {
        p = cpponearg(p, tp[i]);
    }
    *p++ = '#';
    *p = 0;
    manglenamecount = 0;
    strcpy(mname2, mname);
    p = mname2 + strlen(mname2);
    *p++ = '$';
    *p++ = 'q';
    p = cppargs(p, fi);
    *p = 0;
    manglenamecount =  - 1;
    rv = (struct _templatelist*)search(mname2, (TABLE*)templateFuncs);
    if (rv)
        return rv;

    t = tpl->classes.head;
    global_flag++;
    rv = xalloc(sizeof(struct _templatelist));
    rv->partialcount = partial;
    rv->oldtypes = 0;
    rv->newtypes = 0;
    t1 = &rv->oldtypes;
    x1 = &rv->newtypes;
    for (i = 0; t; i++, t = t->next)
    {
        LIST *l = xalloc(sizeof(LIST));
        l->data = t->tp;
        *t1 = l;
        t1 = &(*t1)->link;
        l = xalloc(sizeof(LIST));
        l->data = tp[i];
        *x1 = l;
        x1 = &(*x1)->link;
    } rv->sp = sp;
    rv->partialcount = partial;
    rv->name = litlate(mname2);
    rv->prefixname = litlate(mname);
    global_flag--;
    return rv;
}

//-------------------------------------------------------------------------

static struct _templatelist *search_template_func(SYM *tbsym, TYP *tp, char
    *name)
{
    SYM *sp1 = tbsym->tp->lst.head;
    int found1 = FALSE;
    struct _templatelist *hold = 0,  *hold1 = 0;
    char intrnm[256],  *p = intrnm,  *nm = tbsym->name;
    cpp_unmang_intrins(&p, &nm, tbsym->parentclass ? tbsym->parentclass->name:
        0);
    while (sp1)
    {
        if (sp1->value.classdata.templatedata)
        {
            struct _templatelist *s;
            found1 = TRUE;
            // not sure templateFuncs is used...
            s = conjoint(tp->lst.head, sp1->tp->lst.head, sp1, tbsym->name);
            if (s)
            {
                if (!hold)
                    hold = s;
                else if (s->sp->value.classdata.templatedata->partials > hold
                    ->sp->value.classdata.templatedata->partials)
                {
                    hold = s;
                    hold1 = 0;
                } 
                else if (!hold1)
                    hold1 = s;
                else if (s->sp->value.classdata.templatedata->partials > hold1
                    ->sp->value.classdata.templatedata->partials)
                {
                    hold1 = s;
                }
            }
        }
        sp1 = sp1->next;
    }
    if (hold == 0)
        if (found1)
            gensymerror(ERR_AMBIGTEMPLATE, intrnm);
        else
            gensymerror(ERR_NOFUNCMATCH, intrnm);
        else if (hold1 == 0)
            return hold;
        else if (hold1->sp->value.classdata.templatedata->partials > hold->sp
            ->value.classdata.templatedata->partials)
            return hold1;
        else if (hold->sp->value.classdata.templatedata->partials > hold1->sp
            ->value.classdata.templatedata->partials)
            return hold;
        else
            gensymerror(ERR_AMBIGTEMPLATE, intrnm);
    return 0;
}

//-------------------------------------------------------------------------

static TYP *tplconst(void)
{
    ENODE *node;
    TYP *tp;
    if (lastst == sconst)
    {
        tp = maketype(bt_pointer, laststrlen);
        tp->btp = maketype(bt_char, laststrlen);
        tp->sp = makesym(sc_tconst);
        tp->sp->value.s = litlate(laststr);
        getsym();
    }
    else if (lastst == lsconst)
    {
        tp = maketype(bt_pointer, laststrlen);
        tp->btp = maketype(bt_short, laststrlen *2);
        tp->sp = makesym(sc_tconst);
        tp->sp->value.s = xalloc(laststrlen *2+2);
        memcpy(tp->btp->sp->value.s, laststr, laststrlen *2+2);
        getsym();
    }
    else
    {
        intemplateargs = TRUE;
        tp = exprnc(&node);
        opt4(&node, TRUE);
        intemplateargs = FALSE;
        if (!tp)
        {
            tp = maketype(bt_int, 4);
            tp->sp = makesym(sc_tconst);
            generror(ERR_NEEDCONST, 0, 0);
        }
        else if (!node)
        {
            tp->sp = makesym(sc_tconst);
            generror(ERR_NEEDCONST, 0, 0);
        }
        else if (isintconst(node->nodetype))
        {
            tp = copytype(tp, 0);
            tp->sp = makesym(sc_tconst);
            tp->sp->value.i = node->v.i;
            tp->cflags &= ~DF_CONST;
        }
        else if (isfloatconst(node->nodetype))
        {
            tp = copytype(tp, 0);
            tp->sp = makesym(sc_tconst);
            tp->sp->value.f = node->v.f;
            tp->cflags &= ~DF_CONST;
        }
        else
        {
            tp->sp = makesym(sc_tconst);
            generror(ERR_NEEDCONST, 0, 0);
        }
    }
    return tp;
}

//-------------------------------------------------------------------------

static void matchandmangle(char *mangname, char *nm, LIST **tail, SYM *tplargs)
{
    char *p;
    int err = 0;
    matching++;
    if (nm)
    {
        manglenamecount = 0;
        if (prm_cmangle &&  *nm == '_')
            nm++;
        sprintf(mangname, "@#%s$t", nm);
        p = mangname + strlen(mangname);
    }
    if (lastst != gt)
    do
    {
        if (castbegin(lastst))
        {
            if (typequal && typequal->istemplate)
            {
                needpunc(gt, 0);
                head = typequal->tp;
                typequal = 0;
            }
            else
            {
                decl(0, 0);
                headptr = &head;
                decl1(sc_type, 0);
            }
            if (tail)
            {
                *tail = xalloc(sizeof(LIST));
                (*tail)->data = head;
                tail = &(*tail)->link;
            }
        }
        else
        {
            head = tplconst();
            if (tplargs != (SYM*) - 1)
                head->type = tplargs->tp->type;
            if (tail)
            {
                *tail = xalloc(sizeof(LIST));
                (*tail)->data = head;
                tail = &(*tail)->link;
            }
        }
        if (nm)
            p = cpponearg(p, head);
        if (tplargs != (SYM*) - 1)
        {
            if (!tplargs)
                err |= 1;
            else
                tplargs = tplargs->next;
        }
        if (lastst != comma)
            break;
        getsym();
    }
    while (TRUE);
    needpunc(gt, 0);
    if (err)
        genclasserror(ERR_TEMPLATEEXTRAARG, nm);
    if (tplargs && tplargs != (SYM*) - 1)
    {
        while (tplargs && tplargs->defalt)
        {
            if (tail)
            {
                *tail = xalloc(sizeof(LIST));
                (*tail)->data = tplargs->defalt;
                tail = &(*tail)->link;
            }
            p = cpponearg(p, tplargs->tp);
            tplargs = tplargs->next;
        }
        if (tplargs)
            genclasserror(ERR_TEMPLATEFEWARG, nm);
    }
    if (nm)
    {
        *p++ = '#';
        *p = 0;
        manglenamecount =  - 1;
    }
    matching--;
}

//-------------------------------------------------------------------------

TYP *InstantiateTemplateFuncDirect(ENODE **pnode, TYP *tptr)
{
    TYP *ohead = head,  *otail = tail,  **oheadptr = headptr;
    ENODE *tcnode = 0;
    int odflt = defaulttype;
    char mangname[512], mangname2[512];
    SYM *s;
    LIST *tn = 0,  **tntail = &tn;
    struct _templatelist *tl;
    ENODE *fep;
    TYP *ftp;
    if ((*pnode)->nodetype == en_thiscall)
    {
        tcnode =  *pnode;
        *pnode = (*pnode)->v.p[1];
    } if (*pnode && (*pnode)->nodetype != en_placeholder)
        return tptr;
    getsym();
    global_flag++;
    matchandmangle(0, 0, tntail, (SYM*) - 1);
    global_flag--;
    if (lastst != openpa)
    {
        generror(ERR_PUNCT, openpa, 0);
        return tptr;
    }
    getsym();
    ftp = gatherparms(&fep, FALSE);
    {
        char *p, intrnm[256],  *nm;
        int count;
        SYM *s1;
        LIST *t = tn,  *ot = 0,  **otn = &ot;
        strcpy(intrnm, tptr->sp->name);
        s = tptr->lst.head;
        for (count = 0; t; count++, t = t->link)
            ;
        while (s)
        {
            if (s->istemplate)
            {
                if (count == s->value.classdata.templatedata->tplargcount)
                {
                    t = tn;
                    s1 = s->value.classdata.templatedata->classes.head;
                    while (t)
                    {
                        if (s1->tp->type != bt_class)
                        {
                            if (!((TYP*)t->data)->sp || ((TYP*)t->data)->sp
                                ->storage_class != sc_tconst)
                                break;
                        }
                        else if (((TYP*)t->data)->sp && ((TYP*)t->data)->sp
                            ->storage_class == sc_tconst)
                            if (scalarnonfloat(((TYP*)t->data)) !=
                                scalarnonfloat(s1->tp) && isscalar(((TYP*)t
                                ->data)) != isscalar(s1->tp))
                                break;
                        t = t->link;
                        s1 = s1->next;
                    }
                    if (!t)
                        break;
                }
            }
            s = s->next;
        }
        if (!s)
        {
            gensymerror(ERR_AMBIGTEMPLATE, intrnm);
            return tptr;
        }
        t = tn;
        s1 = s->value.classdata.templatedata->classes.head;
        p = intrnm, nm = tptr->sp->name;
        if (*nm == '$')
            cpp_unmang_intrins(&p, &nm, tptr->sp->parentclass ? tptr->sp
                ->parentclass->name: 0);
        p += prm_cmangle && p[0] == '_';
        sprintf(mangname, "@#%s$t", p);
        p = mangname + strlen(mangname);
        manglenamecount = 0;
        while (t)
        {
            if (s1->tp->type != bt_class)
                ((TYP*)t->data)->type = s1->tp->type;
            p = cpponearg(p, t->data);
            t = t->link;
            s1 = s1->next;
        }
        *p++ = '#';
        *p = 0;
        strcpy(mangname2, mangname);
        p = mangname2 + strlen(mangname2);
        *p++ = '$';
        *p++ = 'q';
        manglenamecount = 0;
        p = cppargs(p, ftp->lst.head);
        *p = 0;
        manglenamecount =  - 1;
        tl = search(mangname2, (TABLE*)templateFuncs);
        if (!tl)
        {
            global_flag++;
            tl = xalloc(sizeof(struct _templatelist));
            tl->name = litlate(mangname2);
            tl->prefixname = litlate(mangname);
            tl->sp = s;
            s1 = s->value.classdata.templatedata->classes.head;
            while (s1)
            {
                *otn = xalloc(sizeof(LIST));
                (*otn)->data = s1->tp;
                otn = &(*otn)->link;
                s1 = s1->next;
            } tl->oldtypes = ot;
            tl->newtypes = tn;
            global_flag--;
        }
    }
    if (!tl->finalsp)
    {
        savecontext();
        if (tcnode)
            declclass = tcnode->v.p[0]->v.sp;
        virtualfuncs = 1;
        interjectptr = replace(tl, tl->sp->value.classdata.templatedata);
        setunaltered();
        templatePrefix = tl->prefixname;
        templateSP = tl->finalsp;
        errfile = tl->sp->value.classdata.templatedata->filename;
        infile = tl->sp->value.classdata.templatedata->filename;
        lineno = errlineno = tl->sp->value.classdata.templatedata->lineno;
        global_flag++;
        getch();
        getsym();
        dodecl(sc_global);
        global_flag--;
        restorecontext();
        templatePrefix = 0;
        templateSP = 0;
        tl->finalsp = lastfunc;
        tl->finalsp->isinsttemplate = TRUE;
        insert((SYM*)tl, templateFuncs);
    }
    defaulttype = odflt;
    head = ohead;
    tail = otail;
    headptr = oheadptr;
    *pnode = makenode(en_napccon, (void*)tl->finalsp, 0);
    tptr = tl->finalsp->tp;
    if (tcnode)
    {
        tcnode->v.p[1] =  *pnode;
        *pnode = tcnode;
    }
    *pnode = onefunc(*pnode, &tptr, fep, ftp);
    return tptr;
}

//-------------------------------------------------------------------------

SYM *lookupTemplateFunc(SYM *tbsym, TYP *tp, char *name)
{
    struct _templatelist *val = search_template_func(tbsym, tp, tbsym->name);
    if (val && !val->finalsp)
    {
        savecontext();
        virtualfuncs = 1;
        interjectptr = replace(val, val->sp->value.classdata.templatedata);
        setunaltered();
        templatePrefix = val->prefixname;
        templateSP = val->finalsp;
        errfile = val->sp->value.classdata.templatedata->filename;
        infile = val->sp->value.classdata.templatedata->filename;
        lineno = errlineno = val->sp->value.classdata.templatedata->lineno;
        global_flag++;
        getch();
        getsym();
        dodecl(sc_global);
        global_flag--;
        restorecontext();
        templatePrefix = 0;
        templateSP = 0;
        val->finalsp = lastfunc;
        val->finalsp->isinsttemplate = TRUE;
        insert((SYM*)val, templateFuncs);
        return val->finalsp;
    } if (val)
        return val->finalsp;
    return 0;
}

//-------------------------------------------------------------------------

void instantiateTemplateFuncs(SYM *tempsym, SYM *actual, LIST *tn, int recons)
{
    SYM *sp = tempsym->tp->lst.head;
    while (sp)
    {
        if (sp->storage_class == sc_defunc)
        {
            SYM *sp1 = sp->tp->lst.head;
            while (sp1)
            {
                if (sp1->value.classdata.templatedata)
                {
                    char unmangname[256],  *p;
                    int len, max;
                    LIST *to = 0,  **ton = &to;
                    SYM *sp2 = sp1->value.classdata.templatedata->classes.head;
                    while (sp2)
                    {
                        *ton = xalloc(sizeof(LIST));
                        (*ton)->data = sp2->tp;
                        ton = &(*ton)->link;
                        sp2 = sp2->next;
                    }
                    savecontext();
                    virtualfuncs = 1;
                    infile = errfile = sp1->value.classdata.templatedata
                        ->filename;
                    lineno = errlineno = sp1->value.classdata.templatedata
                        ->lineno;
                    len = sp1->value.classdata.templatedata->linelen;
                    max = sp1->value.classdata.templatedata->linemax;
                    interjectptr = replace2(sp1->value.classdata.templatedata
                        ->lines, &max, &len, to, tn);
                    if (recons && strstr(sp1->name,
                        cpp_funcname_tab[CI_CONSTRUCTOR]) || strstr(sp1->name,
                        cpp_funcname_tab[CI_DESTRUCTOR]))
                    {
                        int slen, ylen;
                        short *buf;
                        short name[256],  *p1 = name;
                        char *q1 = tempsym->name + prm_cmangle;
                        while (*q1)
                             *p1++ =  *q1++;
                        *p1 =  *q1;
                        slen = pstrlen(name);
                        // slen *shouldn't* change as a result of these calls
                        buf = matchname(interjectptr, name, &slen, FALSE);
                        if (buf)
                        {
                            buf = matchname(buf + slen, name, &slen, TRUE);
                            if (buf)
                            {
                                buf += slen;
                                unmangle(unmangname, actual->name);
                                p = strchr(unmangname, '>');
                                if (p)
                                    *(p + 1) = 0;
                                p = strchr(unmangname, '<');
                                p1 = name;
                                while (*p)
                                     *p1++ =  *p++;
                                *p1 =  *p;
                                slen = pstrlen(name);
                                if (max <= len + slen)
                                {
                                    short *buf2 = xalloc(max + 1000 * sizeof
                                        (short));
                                    max += 1000;
                                    memcpy(buf2, buf, (len + 1) *sizeof(short));
                                    buf = buf2;
                                }
                                memmove(buf + slen, buf, (len - (buf -
                                    interjectptr) + 1) *sizeof(short));
                                memcpy(buf, name, slen *sizeof(short));
                            }
                        }
                    }
                    setunaltered();
                    //               templatePrefix = fullcppmangle(0,name,sp1->tp) ;
                    templateSP = 0;
                    getch();
                    getsym();
                    dodecl(sc_global);
                    restorecontext();
                    templatePrefix = 0;
                    templateSP = 0;
                }
                sp1 = sp1->next;
            }
        }
        else if (sp->storage_class == sc_type)
        {
            if (sp->tp->type == bt_struct || sp->tp->type == bt_class)
                instantiateTemplateFuncs(sp, actual, tn, recons);
        }
        else if (sp->value.classdata.templatedata)
        {
            char unmangname[256],  *p;
            int len, max;
            LIST *to = 0,  **ton = &to;
            SYM *sp2 = sp->value.classdata.templatedata->classes.head;
            while (sp2)
            {
                *ton = xalloc(sizeof(LIST));
                (*ton)->data = sp2->tp;
                ton = &(*ton)->link;
                sp2 = sp2->next;
            }
            savecontext();
            virtualdata = 1;
            infile = errfile = sp->value.classdata.templatedata->filename;
            lineno = errlineno = sp->value.classdata.templatedata->lineno;
            len = sp->value.classdata.templatedata->linelen;
            max = sp->value.classdata.templatedata->linemax;
            interjectptr = replace2(sp->value.classdata.templatedata->lines,
                &max, &len, to, tn);
            setunaltered();
            //               templatePrefix = fullcppmangle(0,name,sp1->tp) ;
            templateSP = 0;
            getch();
            getsym();
            dodecl(sc_global);
            restorecontext();
            templatePrefix = 0;
            templateSP = 0;
            virtualdata = 0;
        }
        sp = sp->next;
    }
}

//-------------------------------------------------------------------------

LIST *getTemplateName(SYM *sp, char *name)
{
    TYP *thead = head,  **theadptr = headptr,  *ttail = tail;
    SYM *rv;
    int err = 0;
    int odlft = defaulttype;
    LIST *tn = 0,  **tntail = &tn,  *tnorig;
    struct template  *tm = sp->value.classdata.templatedata;
    char mangname[512],  *p = name;
    getsym();
    matchandmangle(mangname, name, tntail, tm->classes.head);
    strcpy(name, mangname);
    defaulttype = odlft;
    head = thead;
    tail = ttail;
    headptr = theadptr;
    tnorig = tn;
    #ifdef XXXXX
        if (currentTemplate)
        {
            SYM *s1 = 0,  **ps = &s1;
            SYM *s2 = tm->classes.head;
            global_flag++;
            while (tn && s2)
            {
                TYP *tp = tn->data;
                SYM *sx = currentTemplate->classes.head;
                while (sx)
                {
                    if (sx->name && tp->sp && tp->sp->name && !strcmp(sx->name,
                        tp->sp->name))
                        break;
                    sx = sx->next;
                } if (!sx)
                {
                    if (tp->type == bt_templateplaceholder)
                        *ps = copysym(tp->lst.head);
                    else
                        *ps = copysym(tp->sp);
                }
                else
                {
                    (*ps) = copysym(sx);
                }
                (*ps)->next = 0;
                ps = &(*ps)->next;
                s2 = s2->next;
                tn = tn->link;
            }
            global_flag--;
            currentTemplate->classes.head = s1;
        }
    #endif 
    return tnorig;
}

//-------------------------------------------------------------------------

void tsname(char *unmang, char *name)
{
    char *p = unmang,  *q;
    unmangle(unmang, name);
    while (*p &&  *p != '<')
        p++;
    q = p;
    while (*p &&  *p != '>')
    {
        if (*p == '(')
        {
            p++;
            do
            {
                q--;
            }
            while (*q != '<' &&  *q != ',');
            q++;
            while (*p &&  *p != ')')
                *q++ =  *p++;
            if (*p)
                p++;
        }
        else
            *q++ =  *p++;
    }

    while (*p)
        *q++ =  *p++;
    *q =  *p;
}

//-------------------------------------------------------------------------

static int spcomp(TYP **lst, int *count, int *partials, struct template  *tm,
    LIST *fi, LIST *ti)
{
    while (ti && fi)
    {
        TYP *t = ti->data,  *f = fi->data;
        if (t->type == bt_class)
        {
            int index = tlookup(t, tm);
            if (index !=  - 1)
            {
                if (index >=  *count)
                    *count = index + 1;
                else if (lst[index] && !exactype(lst[index], f, FALSE))
                    return FALSE;
                lst[index] = f;
            } 
            else
            {
                return FALSE;
            }
        }
        else if (t->type == bt_templateplaceholder)
        {
            if (f->type != bt_templateplaceholder)
                return FALSE;
            if (strcmp(t->lst.head->name, f->lst.head->name))
                return FALSE;
            if (!spcomp(lst, count, partials, tm, (LIST*)f->lst.tail, t
                ->lst.tail))
                return FALSE;
        }
        else
        {
            if (!exactype(fi, ti, FALSE))
                return FALSE;
        }
        (*partials)++;
        ti = ti->link;
        fi = fi->link;
    }
    return !(fi || ti);
}

//-------------------------------------------------------------------------

static SYM *specialize(SYM *sp, TYP **tn)
{
    SYM *spx = search("$$DETEMP", &sp->tp->lst.head);
    int maxpartials = 0;
    SYM *hold;
    TYP *maxtyplst[100];
    int maxcount = 0;
    int i;
    if (spx)
    {
        spx = spx->tp->lst.head;
        while (spx)
        {
            TYP *typlst[100];
            int typcount = 0;
            int partials = 0;
            struct template  *tm = spx->value.classdata.templatedata;
            memset(typlst, 0, sizeof(typlst));
            if (spcomp(typlst, &typcount, &partials, tm,  *tn, spx
                ->value.classdata.templateargs))
            {
                SYM *t = tm->classes.head;
                for (i = 0; i<typcount || t; i++, t = t ? t->next: 0)
                if (!typlst[i])
                {
                    if (t && t->defalt)
                    {
                        typlst[i] = t->defalt;
                        if (i >= typcount)
                            typcount++;
                    } break;
                }
                if (i >= typcount)
                {
                    if (partials && partials == maxpartials)
                        gensymerror(ERR_AMBIGTEMPLATE, sp->name);
                    if (partials > maxpartials)
                    {
                        maxpartials = partials;
                        maxcount = typcount;
                        memcpy(maxtyplst, typlst, typcount *sizeof(int));
                        hold = spx;
                    }
                }
            }
            spx = spx->next;
        }
    }
    if (maxpartials)
    {
        LIST **x = tn;
        for (i = 0; i < maxcount; i++)
        {
            *x = xalloc(sizeof(LIST));
            (*x)->data = maxtyplst[i];
            x = &(*x)->link;
        }
        return hold;
    }
    return sp;
}

//-------------------------------------------------------------------------

SYM *lookupTemplateType(SYM *tempsym, int cppflags)
{
    SYM *sp,  *rv;
    TYP *ohead = head,  *otail = tail,  **oheadptr = headptr;
    int odlft = defaulttype;
    struct template  *tm;
    char name[256], unmangname[512],  *p,  *nm = tempsym->name;
    LIST *tn = 0,  **tnail = &tn;
    int len, max;
    char *buf;
    LIST *to = 0,  **ton = &to;
    SYM *sp1,  *s1;
    tm = tempsym->value.classdata.templatedata;
    getsym();
    if (recordingTemplate || matching)
    {
        // need to make a template placeholder type
        rv = tm->classes.head;
        global_flag++;
        matchandmangle(name, nm, tnail, tm->classes.head);
        tempsym = specialize(tempsym, &tn);
        rv = copysym(tempsym);
        rv->tp = maketype(bt_templateplaceholder, 0);
        rv->tp->lst.head = tempsym; /* so we can declclass it */
        rv->tp->lst.tail = tn; /* for type matching later if we need it */
        global_flag--;
    } 
    else
    {
        // else are instantiating a template
        TYP *thead = head,  **theadptr = headptr,  *ttail = tail;
        TYP *tcq = typequal;
        typequal = 0;
        matchandmangle(name, nm, tnail, tm->classes.head);
        tempsym = specialize(tempsym, &tn);
        tm = tempsym->value.classdata.templatedata;
        tsname(unmangname, name);
        head = thead;
        tail = ttail;
        headptr = theadptr;
        rv = search(name, tagtable);
        typequal = tcq;
        if (!rv)
        {
            LIST *t = tn;
            s1 = tm->classes.head;
            while (t && s1)
            {
                if (s1->tp->type != bt_class)
                {
                    if (!((TYP*)t->data)->sp || ((TYP*)t->data)->sp
                        ->storage_class != sc_tconst)
                        break;
                }
                else if (((TYP*)t->data)->sp && ((TYP*)t->data)->sp
                    ->storage_class == sc_tconst)
                    break;
                t = t->link;
                s1 = s1->next;
            }
            if (t || s1)
            {
                gensymerror(ERR_AMBIGTEMPLATE, nm);
                return 0;
            }
            sp1 = tm->classes.head;
            while (sp1)
            {
                *ton = xalloc(sizeof(LIST));
                (*ton)->data = sp1->tp;
                ton = &(*ton)->link;
                sp1 = sp1->next;
            }
            buf = xalloc(tm->linemax *sizeof(short));
            memcpy(buf, tm->lines, tm->linemax *sizeof(short));
            len = tm->linelen;
            max = tm->linemax;
            savecontext();
            statictemplate |= cppflags & PF_STATIC;
            interjectptr = llreplace2(buf, &max, &len, tempsym->name,
                unmangname);
            interjectptr = replace2(interjectptr, &max, &len, to, tn);
            setunaltered();
            infile = errfile = tm->filename;
            lineno = errlineno = tm->lineno;
            global_flag++;
            getch();
            getsym();
            dodecl(sc_global);
            restorecontext();
            rv = search(name, tagtable);
            //		 if (rv) {
            //			 rv->value.classdata.templateparent = tempsym ;
            //			 rv->value.classdata.templateargs = tn ;
            //		 }
            instantiateTemplateFuncs(tempsym, rv, tn, TRUE);
            global_flag--;
        } 
//        else
//            rv->tp->size = rv->mainsym->tp->size;
        defaulttype = odlft;
    }
    head = ohead;
    tail = otail;
    headptr = oheadptr;
    return rv;

}

//-------------------------------------------------------------------------

void replaceTemplateFunctionClasses(SYM *sp, SYM *typequal)
{
    SYM *s,  *t;
    if (!templateLookup || !typequal || !typequal->istemplate)
        return ;
    sp = sp->tp->lst.head;
    while (sp)
    {
        if (sp->tp->type == bt_class)
        {
            s = currentTemplate->classes.head;
            t = typequal->value.classdata.templatedata->classes.head;
            while (s && t)
            {
                if (!strcmp(s->name, sp->tp->sp->name))
                {
                    sp->tp = t->tp;
                    break;
                }
                s = s->next;
                t = t->next;
            }
        }
        sp = sp->next;
    }
}

//-------------------------------------------------------------------------

void templateMatchCheck(SYM *tq, struct template  *p)
{
    SYM *s,  *t;
    if (!tq || !p || !tq->istemplate|| !tq->value.classdata.templatedata)
        return ;
    s = p->classes.head;
    t = tq->value.classdata.templatedata->classes.head;
    while (s && t)
    {
        s = s->next;
        t = t->next;
    } if (s || t)
        generror(ERR_TEMPLATEFUNCMATCHCLASS, 0, 0);
}

//-------------------------------------------------------------------------

void replaceSize(TYP *candidate)
{
    // this still be buggy for nested classes that return structs corresponding to their parents...
    while (candidate->type == bt_pointer || candidate->type == bt_ref ||
        candidate->type == bt_farpointer || candidate->type == bt_segpointer)
        candidate = candidate->btp;
    if (isstructured(candidate) && !candidate->size)
        candidate->size = candidate->sp->mainsym->tp->size;
}

//-------------------------------------------------------------------------

void parseClassFuncs(ILIST *l)
{
    while (l)
    {
        SYM *sp1 = (SYM*)l->sp;
        if (sp1->value.classdata.templatedata)
        {
            savecontext();
            sp1->parentclass = declclass = l->dc;
            infile = errfile = sp1->value.classdata.templatedata->filename;
            lineno = errlineno = sp1->value.classdata.templatedata->lineno;
            interjectptr = sp1->value.classdata.templatedata->lines;
            setunaltered();
            getch();
            if (lastch == ':')
            {
                lastst = colon;
                getch();
                classbaseasn(declclass, sp1);
                if (lastst != begin)
                    generror(ERR_NEEDCHAR, '{', 0);
            }
            else
            {
                lastst = begin;
                getch();
            }
            funcbody(sp1);
            interjectptr = 0;
            restorecontext();
            sp1->value.classdata.templatedata = 0;
        }
        l = l->next;
    }
}

//-------------------------------------------------------------------------

void checkTemplateSpecialization(SYM *sp, char *name)
{
    struct template  *tpl = sp->value.classdata.templatedata;
    char nm[256];
    SYM *sp1 = tpl->classes.head;
    tpl->partials = tpl->argcount = 0;
    while (sp1 && sp1 != (SYM*) - 1)
    {
        if (tlookup(sp1->tp, tpl) ==  - 1 && sp1->tp->sp->storage_class ==
            sc_const)
            tpl->partials++;
        tpl->argcount++;
        sp1 = sp1->next;
    } if (tpl->partials)
    {
        matchandmangle(nm, name, 0, tpl->classes.head);
        if (sp == specialize(sp, nm))
        {
            gensymerror(ERR_SPECIALIZATION, sp->name);
        }
    }
}
