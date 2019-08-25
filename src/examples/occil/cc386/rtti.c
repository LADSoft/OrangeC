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
#include "rtti.h"

extern char *cpp_funcname_tab[];
extern LIST *varlisthead;
extern TABLE lsyms,  *gsyms;
extern int prm_cmangle;
extern int global_flag;
extern LIST *instantiated_inlines;
extern TRYBLOCK *try_block_list;
extern SYM *typeinfo_class;
extern long nextlabel;

RTTI_LIST *list_of_rttis;
LIST *registered_rttis;
int xceptoffs;

static LIST *xtSymList;
void xceptinit(void)
{
    xtSymList = 0;
    list_of_rttis = 0;
    registered_rttis = 0;
    typeinfo_class = 0;
}

//-------------------------------------------------------------------------

static void getxtname(char *buf, TYP *tp)
{
    char *p;
    strcpy(buf, "@$xt");
    if (isstructured(tp) && tp->sp->parentclass)
        mangleclass(buf + 4, tp->sp->parentclass);
    p = buf + strlen(buf);
    p[0] = '@';
    p++;

    cpponearg(p, tp);
    if (tp->type == bt_pointer)
        if (tp->val_flag)
    if (!(tp->val_flag &VARARRAY))
    {
        p += strlen(p);
        *p++ = '@';
        sprintf(p, "%d", tp->size / tp->btp->size);
    }
    else
        strcat(p, "@varr");
}

//-------------------------------------------------------------------------

SYM *getxtsym(TYP *tp)
{
    char buf[256],  *p;
    LIST *l;
    SYM *sp1;
    SYM *sp = 0;
    char *nm;
    getxtname(buf, tp);
    if (!isstructured(tp))
    {
        if (sp1 = search(buf, gsyms))
            return sp1;
    }
    else
    {
        sp = tp->sp;
        if (sp->value.classdata.xtsym)
            return sp->value.classdata.xtsym;
    }
    global_flag++;
    sp1 = makesym(sc_type);
    sp1->tp = copytype(tp, 0);
    sp1->name = litlate(buf);
    insert(sp1, gsyms);
    l = xalloc(sizeof(LIST));
    global_flag--;
    l->data = sp1;
    l->link = xtSymList;
    xtSymList = l;
    if (sp)
        sp->value.classdata.xtsym = sp1;
    return sp1;
}

//-------------------------------------------------------------------------

static void xceptData(SYM *func, SYM *sp)
{
    TYP *tp = sp->tp;
    if (tp->type == bt_pointer)
    {
        while (tp->type == bt_pointer && tp->val_flag)
            tp = tp->btp;
    }
    if (tp && isstructured(tp) && !sp->iscatchvar && !sp->nullsym)
    {
        if (search(cpp_funcname_tab[CI_CONSTRUCTOR], tp->lst.head) || search
            (cpp_funcname_tab[CI_DESTRUCTOR], tp->lst.head))
        {
            int flags = XD_CL_PRIMARY;
            if (tp->sp->value.classdata.baseclass && tp->sp
                ->value.classdata.baseclass->flags &CL_VTAB)
                flags |= XD_CL_VTAB;
            nl();
            if (tp != sp->tp)
                genlong(flags | XD_ARRAY | 4);
            else
                genlong(flags);
            genref(getxtsym(tp), 0);
            genlong(sp->value.i);
            if (sp->mainsym->value.classdata.conslabel ==  - 1)
            // beginning of func
                genlong(0);
            else
                gen_labdifref(sp->mainsym->value.classdata.conslabel, func
                    ->value.classdata.destlabel);
            gen_labdifref(sp->mainsym->value.classdata.destlabel, func
                ->value.classdata.destlabel);
            if (tp != sp->tp)
                genlong(sp->tp->size / tp->size);
        }
    }
}

//-------------------------------------------------------------------------

static void trydata(SYM *func, TRYBLOCK *t)
{
    CATCHBLOCK *c = t->catchit;
    while (c)
    {
        nl();
        genlong(XD_CL_TRYBLOCK | 4);
        if (c->xtcatch)
        {
            genref(getxtsym(c->xtcatch), 0);
        }
        else
            genlong(0);
        genlong(c->dummy ? c->dummy->v.sp->value.i: 0);
        gen_labdifref(t->startlab, func->value.classdata.destlabel);
        gen_labdifref(t->endlab, func->value.classdata.destlabel);
        gen_labref(c->label);
        c = c->next;
    }
}

//-------------------------------------------------------------------------

int genthrowdata(SYM *sp)
{
    int rv = 0;
    if (sp->value.classdata.throwlist && sp->value.classdata.throwlist->data)
    {
        rv = nextlabel++;
        put_staticlabel(rv);
        if (sp->value.classdata.throwlist->data)
        {
            LIST *l = sp->value.classdata.throwlist;
            while (l)
            {
                genref(getxtsym(l->data), 0);
                l = l->link;
            }
        }
        genlong(0);
    }
    return rv;
}

//-------------------------------------------------------------------------

void dumpXceptBlock(SYM *sp)
{
    LIST *l = varlisthead;
    TRYBLOCK *t = try_block_list;
    int label;
    try_block_list = 0;
    varlisthead = 0;
    cseg();
    label = genthrowdata(sp);
    put_staticlabel(sp->value.classdata.conslabel);
    nl();
    if (label)
        gen_labref(label);
    else
        genlong(0);
    genlong( - xceptoffs);
    //
    while (t)
    {
        trydata(sp, t);
        t = t->next;
    }
    nl();
    //
    while (l)
    {
        SYM *sp1 = l->data;
        while (sp1)
        {
            xceptData(sp, sp1);
            sp1 = sp1->next;
        }
        l = l->link;
    }
    nl();
    genlong(0);
}

//-------------------------------------------------------------------------

void dumpname(SYM *sp)
{
    char buf[256],  *p;
    int l, i;
    buf[0] = 0;
    if (sp->parentclass)
    {
        mangleclass(buf, sp->parentclass);
        p = buf + strlen(buf);
        *p++ = '@';
    }
    else
        p = buf;
    strcpy(p, sp->name + (sp->name[0] == '_' && prm_cmangle));
    genbyte(l = strlen(buf));
    for (i = 0; i < l; i++)
        genbyte(buf[i]);
}

//-------------------------------------------------------------------------

void dumpdest(SYM *sp)
{
    SYM *sp1 = search(cpp_funcname_tab[CI_DESTRUCTOR], &sp->tp->lst);
    if (sp1)
    {
        SYM *sp2 = sp1->tp->lst.head;
        genref(sp2, 0);
        global_flag++;
        SetExtFlag(sp2, TRUE);
        global_flag--;
    }
    else
        genlong(0);
}

//-------------------------------------------------------------------------

void dumpClassData(CLASSLIST *l, int flags)
{
    nl();
    genlong(flags);
    genref(getxtsym(((SYM*)(l->data))->tp), 0);
    genlong(l->offset);
}

//-------------------------------------------------------------------------

void dumpSymData(SYM *sp, int flags)
{
    TYP *tp = sp->tp;
    if (tp->type == bt_pointer)
    {
        while (tp->type == bt_pointer && tp->val_flag)
            tp = tp->btp;
    }
    if (tp && isstructured(tp))
    {
        if (tp != sp->tp)
        {
            genlong(flags | XD_ARRAY | 4);
        }
        else
            genlong(flags);
        genref(getxtsym(tp), 0);
        genlong(sp->value.i);
        if (tp != sp->tp)
        {
            genlong(sp->tp->size / tp->size);
        }
    }
}

//-------------------------------------------------------------------------

void insertRTTI(SYM *label, TYP *tp)
{
    RTTI_LIST *l;
    global_flag++;
    l = xalloc(sizeof(RTTI_LIST));
    l->label = label;
    l->tp = copytype(tp, 0);
    global_flag--;
    l->next = list_of_rttis;
    list_of_rttis = l;
}

//-------------------------------------------------------------------------

int registeredBaseRTTI(char *name)
{
    LIST **l = &registered_rttis;
    while (*l)
    {
        if (!strcmp(((SYM*)((*l)->data))->name, name))
            return (*l)->data;
        l = &(*l)->link;
    }
    return 0;
}

//-------------------------------------------------------------------------

void registerBaseRTTI(SYM *sp)
{
    LIST *l;
    global_flag++;
    l = xalloc(sizeof(LIST));
    l->data = sp;
    l->link = registered_rttis;
    registered_rttis = l;
    global_flag--;
}

//-------------------------------------------------------------------------

void dumpClassRTTI(SYM *sp_in, SYM **spo)
{
    CLASSLIST *l1;
    SYM *sp = sp_in->tp->sp;
    if (*spo = registeredBaseRTTI(sp_in->name))
        return ;
    *spo = sp_in;
    registerBaseRTTI(sp_in);
    gen_virtual(sp_in->mainsym, FALSE);
    sp_in->mainsym->gennedvirtfunc = TRUE;
    dumpdest(sp);
    genlong(sp->tp->size);
    genlong(XD_CL_PRIMARY);
    dumpname(sp);
    l1 = sp->value.classdata.baseclass;
    while (l1)
    {
        if (l1->flags &(CL_PRIMARY | CL_SECONDARY))
            dumpClassData(l1, XD_CL_BASE);
        l1 = l1->link;
    }
    sp = sp->tp->lst.head;
    while (sp)
    {
        dumpSymData(sp, XD_CL_ENCLOSED);
        sp = sp->next;
    }
    nl();
    genlong(0);
    gen_endvirtual(sp_in);
}

//-------------------------------------------------------------------------

void dumpBaseRTTI(TYP *tp, SYM **spo)
{
    char name[256], mname[256];
    SYM *sp;
    int len, i;
    getxtname(mname, tp);
    if (*spo = registeredBaseRTTI(mname))
        return;
    typenum2(name, tp);
    *spo = sp = getxtsym(tp);
    sp->mainsym->gennedvirtfunc = TRUE;
    registerBaseRTTI(sp);
    gen_virtual(sp, FALSE);
    genlong(0);
    genlong(tp->size);
    genlong(XD_CL_BASETYPE); // reserved for flags
    genbyte(len = strlen(name));
    for (i = 0; i < len; i++)
        genbyte(name[i]);
    gen_endvirtual(sp);
}

//-------------------------------------------------------------------------

int dumpPointerRTTI(TYP *tp, SYM *spbase, SYM **spo)
{
    int flags = XD_CL_BASETYPE;
    char name[256], mname[256];
    SYM *sp;
    getxtname(mname, tp);
    if (*spo = registeredBaseRTTI(mname))
        return 1;
    typenum2(name, tp);
    if (!(sp = search(mname, gsyms)))
    {
        global_flag++;
        sp = makesym(sc_type);
        sp->name = litlate(mname);
        global_flag--;
        sp->tp = tp;
    }
    sp->mainsym->gennedvirtfunc = TRUE;
    registerBaseRTTI(sp);
    gen_virtual(sp->mainsym, FALSE);
    genref(spbase, 0);
    genlong(tp->size);
    if (tp->type == bt_pointer)
        if (tp->val_flag)
            flags |= XD_ARRAY;
        else
            flags |= XD_POINTER;
        else
            flags |= XD_REF;
    if (tp->cflags &DF_CONST)
        flags |= XD_CL_CONST;
    genlong(flags);
    genbyte(0); // no name
    if (flags &XD_ARRAY)
        genlong(tp->size / tp->btp->size);
    gen_endvirtual(sp);
    *spo = sp;
    return 0;
}

//-------------------------------------------------------------------------

void dumpRTTI(TYP *tp, SYM **spo)
{
    SYM *spx;
    if (tp->type == bt_pointer || tp->type == bt_ref)
    {
        dumpRTTI(tp->btp, &spx);
    }
    if (tp->type == bt_pointer || tp->type == bt_ref)
        dumpPointerRTTI(tp, spx, spo);
    else if (isstructured(tp))
        dumpClassRTTI(getxtsym(tp), spo);
    else
        dumpBaseRTTI(tp, spo);
}

//-------------------------------------------------------------------------

void dumpRTTIDescript(int label, SYM *destructor, SYM *xceptlst)
{
    cseg();
    put_label(label);
    genref(destructor, 0);
    genref(xceptlst, 0);
}

//-------------------------------------------------------------------------

void dumpXceptTabs(void)
{
    SYM *spdest = 0,  *sp;
    RTTI_LIST *l1 = list_of_rttis;
    list_of_rttis = 0;
    if (typeinfo_class)
        spdest = search("$bdtr", &typeinfo_class->tp->lst);
    if (spdest)
        spdest = spdest->tp->lst.head;
    // should have gotten an error if the class wasn't defined and we need this
    while (l1)
    {
        dumpRTTI(l1->tp, &sp);
        if (spdest)
            dumpRTTIDescript(l1->label->value.i, spdest, sp);
        l1 = l1->next;
    }
    while (xtSymList)
    {
        // This function may generate new things on the sym list
        // we gotta keep going until the list is empty
        LIST *l = xtSymList;
        xtSymList = 0;
        while (l)
        {
            dumpRTTI(((SYM*)l->data)->tp, &sp);
            l = l->link;
        }
    }
}
