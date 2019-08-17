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

#define VTABVALTHRESH (1024 * 1024)

extern ENODE *expr_runup[50];
extern ENODE *expr_rundown[50];
extern int expr_updowncount;

extern int prm_xcept;
extern int global_flag;
extern SYM * typequal;
extern int skm_closepa[], skm_declbegin[];
extern LIST *instantiated_inlines;
extern int prm_cplusplus;
extern ENODE *thisenode;
extern char *cpp_funcname_tab[];
extern enum e_sym lastst;
extern int skm_declcomma[];
extern int skm_declclosepa[];
extern int stdaddrsize;
extern ENODE *block_rundown;
extern SYM *declclass;
extern char lastid[];
extern TYP stdmatch;
extern TABLE lsyms;
extern SYM undef;
extern long nextlabel;
extern TYP stdvoidfunc;
extern SYM *currentfunc;

TABLE *baseasnsymtbl;

int classhead;
int vtabhead;
static TABLE vmptab;
static TYP functovoidtype = 
{
    bt_func, 0, 0,  - 1,  - 1, 4
};
static TYP vtyp;

void defclassini(void)
{
    vtyp.type = bt_func;
    vtyp.lst.head = vtyp.lst.tail =  - 1;
    vtyp.cflags = 0;
    vtabhead = stdaddrsize * 3;
    classhead = stdaddrsize;
    vmptab.head = 0;
    functovoidtype.lst.head =  - 1;
    functovoidtype.btp = maketype(bt_void, 0);
}

//-------------------------------------------------------------------------

void SetExtFlag(SYM *sp, int state)
{
    sp->mainsym->extflag = state;
    if (state && (sp->value.classdata.cppflags &PF_INLINE) && !(sp
        ->value.classdata.cppflags &PF_INSTANTIATED))
    {
        LIST *l;
        global_flag++;
        l = xalloc(sizeof(LIST));
        global_flag--;
        l->data = sp;
        l->link = instantiated_inlines;
        instantiated_inlines = l;
        sp->value.classdata.cppflags |= PF_INSTANTIATED;
        sp->value.classdata.cppflags &= ~PF_INLINE;
    }
}

//-------------------------------------------------------------------------

SYM *copysym(SYM *sp)
{
    SYM *rv = xalloc(sizeof(SYM));
    *rv =  *sp;
    rv->next = 0;
    return rv;
}

//-------------------------------------------------------------------------

    static int copydata(SYM *sym, SYM *from, int mode)
    {
        int size = sym->value.classdata.size;

        TABLE *dest = &sym->tp->lst;

        if (from)
        {
            SYM *head = from->tp->lst.head;
            while (head)
            {
                SYM *sp;
                if (head->storage_class == sc_friendlist)
                {
                    TABLE *t;
                    SYM *sp1;
                    if (!(sp = basesearch(head->name, dest, FALSE)))
                    {
                        sp = copysym(head);
                        sp->tp = cponetype(sp->tp);
                        sp->tp->sp = sp;
                        sp->tp->lst.head = sp->tp->lst.tail = 0;
                    }
                    t = &sp->tp->lst;
                    sp1 = head->tp->lst.head;
                    while (sp1)
                    {
                        SYM *sp2 = copysym(sp1);
                        if (!t->head)
                            t->head = t->tail = sp2;
                        else
                            t->tail = t->tail->next = sp2;
                        sp1 = sp1->next;
                    }
                    if (!dest->head)
                        dest->head = dest->tail = sp;
                    else
                        dest->tail = dest->tail->next = sp;
                    head = head->next;
                    continue;
                }
                sp = copysym(head);
                if (!sp->mainsym)
                    sp->mainsym = head;
                sp->next = 0;
                sp->parentclass = sym;
                sp->value.classdata.cppflags |= PF_INHERITED;
                if (sp->value.classdata.cppflags &PF_VIRTUAL)
                    sp->value.classdata.cppflags |= PF_NODEF;
                if (sp->value.classdata.cppflags &PF_PRIVATE)
                    sp->value.classdata.cppflags |= PF_UNREACHABLE;

                if (sp->tp->type == bt_func || sp->tp->type == bt_ifunc)
                    sp->value.classdata.vtaboffs += size;
                else if (sp->tp->type == bt_defunc)
                {
                    SYM *head2 = dest->head;
                    SYM *head3 = sp->tp->lst.head;
                    int retagged = 0;
                    while (head2)
                    {
                        if (!strcmp(head2->name, sp->name) && head2->tp->type 
                            == bt_defunc)
                        {
                            sp = head2;
                            retagged = TRUE;
                            break;
                        }
                        head2 = head2->next;
                    }
                    if (!retagged)
                    {
                        sp->tp = cponetype(sp->tp);
                        sp->tp->sp = sp;
                        sp->tp->lst.head = sp->tp->lst.tail = 0;
                    }
                    head2 = head3;
                    while (head2)
                    {
                        if (!(head2->value.classdata.cppflags &(PF_CONSTRUCTOR 
                            | PF_DESTRUCTOR)))
                        {
                            SYM *s = copysym(head2);
                            s->value.classdata.cppflags |= PF_INHERITED;
                            if (!s->mainsym)
                                s->mainsym = head2;
                            if (s->value.classdata.cppflags &PF_VIRTUAL)
                                s->tp = copytype(s->tp, 0);
                            s->value.classdata.vtaboffs += size;
                            if (sp->tp->lst.head)
                                sp->tp->lst.tail = sp->tp->lst.tail->next = s;
                            else
                                sp->tp->lst.head = sp->tp->lst.tail = s;
                            s->next = 0;
                        }
                        head2 = head2->next;
                    }
                    if (retagged || sp->tp->lst.head == 0)
                    {
                        head = head->next;
                        continue;
                    }
                }
                else
                    sp->value.i += size;

                if (mode == BM_PRIVATE)
                {
                    sp->value.classdata.cppflags &= ~(PF_PUBLIC | PF_PROTECTED);
                    sp->value.classdata.cppflags |= PF_PRIVATE;
                }
                else
                {
                    if (mode == BM_PROTECTED && (sp->value.classdata.cppflags
                        &PF_PUBLIC))
                    {
                        sp->value.classdata.cppflags &= ~PF_PUBLIC;
                        sp->value.classdata.cppflags |= PF_PROTECTED;
                    }
                }
                if (!dest->head)
                    dest->head = dest->tail = sp;
                else
                    dest->tail = dest->tail->next = sp;
                head = head->next;
            }
        }
        return size;
    }
    int loadclassdata(SYM *sp, SYM *spc, int mode, int virtual)
    {
        CLASSLIST *l = spc->value.classdata.baseclass,  *lo = sp
            ->value.classdata.baseclass,  **lp = &lo;
        CLASSLIST *x = lo->link;
        int rv = copydata(sp, spc, mode);
        int first = 2, primary = 1, firstrun;
        int ofs = lo->vtabsize - 12;
        while (*lp)
        {
            first--;
            if (!first)
                break;
            lp = &(*lp)->link;
        }
        {
            LIST **vlist = &lo->vtablist,  *slst = l->vtablist;
            int voffs = lo->vtabsize - 12;
            int offs = sp->value.classdata.size;
            while (*vlist)
                vlist =  *vlist;
            if (!first)
            {
                *vlist = xalloc(sizeof(LIST));
                (*vlist)->data = rv;
                vlist = &(*vlist)->link;
                *vlist = xalloc(sizeof(LIST));
                (*vlist)->data = lo->vtabsize - 12+8;
                vlist = &(*vlist)->link;
                lo->vtabsize += 2 * stdaddrsize;
                ofs += 2 * stdaddrsize;
                voffs += 2 * stdaddrsize;
            }
            while (slst)
            {
                if ((int)slst->data < VTABVALTHRESH)
                {
                    *vlist = xalloc(sizeof(LIST));
                    (*vlist)->data = (void*)((int)slst->data + offs);
                    vlist = &(*vlist)->link;
                    slst = slst->link;
                    *vlist = xalloc(sizeof(LIST));
                    (*vlist)->data = (void*)((int)slst->data + voffs);
                    vlist = &(*vlist)->link;
                    slst = slst->link;
                }
                else
                {
                    *vlist = xalloc(sizeof(LIST));
                    (*vlist)->data = slst->data;
                    vlist = &(*vlist)->link;
                    slst = slst->link;
                }
            }
        }
        firstrun = first;
        lp = &lo->link;
        while (l)
        {
            LIST *vlst = 0,  **vlstp = &vlst,  *slst;
            *lp = xalloc(sizeof(CLASSLIST));
            (*lp)->data = l->data;
            (*lp)->offset = l->offset + rv;
            (*lp)->vtabsize = l->vtabsize;
            if (primary)
                lo->vtabsize += l->vtabsize - vtabhead;
            if (l->flags &CL_VTAB)
            {
                (*lp)->flags |= CL_VTAB;
                lo->flags |= CL_VTAB;
            }
            (*lp)->vtaboffs = l->vtaboffs + ofs;
            (*lp)->vtabsp = l->vtabsp;
            if (primary)
                (*lp)->flags |= CL_PRIMARY;
            if (firstrun && l->offset + rv == 0)
                (*lp)->flags |= CL_TOPCLASS;
            if (l->mode == BM_PRIVATE || mode == BM_PRIVATE)
                (*lp)->mode = BM_PRIVATE;
            else if (l->mode == BM_PROTECTED || mode == BM_PROTECTED)
                (*lp)->mode = BM_PROTECTED;
            else
                (*lp)->mode = BM_PUBLIC;
            (*lp)->vtablist = vlst;
            (*lp)->link = x;
            lp = &(*lp)->link;
            first = FALSE;
            primary = FALSE;
            l = l->link;
        }
        sp->value.classdata.size = rv + spc->value.classdata.size;
        return sp->value.classdata.size;
    }
	int insertVirtualData(SYM *sp)
	{
		return 0 ;
	}
    static char *findname(char *buf)
    {
        char *buf1 = buf;
        while (TRUE)
        {
            buf1 = strchr(buf1 + 1, '$');
            if (!buf1)
                return FALSE;

            if (*(buf1 - 1) != '@')
            {
                while (*(buf1 - 1) != '@' && buf1 != buf)
                    buf1--;
                return buf1;
            }
        }
    }
    static int vtabmatch(SYM *candidate, SYM *current)
    {
        char *cbuf = candidate->name;
        char *ubuf = current->name;
        if (!exactype(candidate->tp, current->tp, FALSE))
            return FALSE;
        cbuf = findname(cbuf);
        ubuf = findname(ubuf);
        if (!cbuf || !ubuf)
            return FALSE;
        return (!strcmp(cbuf, ubuf));
    }
    void addvtabentry(SYM *cl, SYM *sp)
    {
        CLASSLIST *l = cl->value.classdata.baseclass;
        LIST **lst;
        SetExtFlag(sp, TRUE);
        if (sp->tp->type == bt_func)
            sp->storage_class = sc_externalfunc;
        if (l->flags &CL_VTAB)
        {
            LIST *lst = l->vtablist;
            while (lst)
            {
                SYM *sp1 = lst->data;
                if (sp1 > VTABVALTHRESH)
                {
                    if (vtabmatch(sp, sp1))
                    {
                        CLASSLIST *l1 = l;
                        while (l1 && l1->data->mainsym != sp1->parentclass
                            ->mainsym)
                            l1 = l1->link;
                        if (l1 && l1->offset)
                        {
                            SYM *sp2 = gen_vsn_virtual_thunk(sp, l1->offset);
                            lst->data = sp2;
                        }
                        else
                            lst->data = sp;
                        SetExtFlag(sp, TRUE);
                        sp->value.classdata.vtabindex = sp1
                            ->value.classdata.vtabindex;
                        sp->value.classdata.vtaboffs = sp1
                            ->value.classdata.vtaboffs;
                        return ;
                    }
                }
                lst = lst->link;
            }
        }
        l = cl->value.classdata.baseclass;
        l->flags |= CL_VTAB;
        lst = &l->vtablist;
        while (*lst)
            lst = &(*lst)->link;
        *lst = xalloc(sizeof(LIST));
        (*lst)->link = 0;
        (*lst)->data = sp;
        sp->value.classdata.vtabindex = l->vtabsize - vtabhead;
        sp->value.classdata.vtaboffs = l->offset;
        l->vtabsize += stdaddrsize;
    }
    void classerrs(SYM *sp)
    {
        SYM *head = sp->tp->lst.head;
        int hasref = FALSE;
        while (head)
        {
            if (!(head->value.classdata.cppflags &PF_INHERITED) && !(sp
                ->value.classdata.cppflags &PF_HASCONS))
            {
                if (head->tp->type == bt_ref)
                    genclass2error(ERR_REFNOCONS, head->parentclass->name, head
                        ->name);
                else if (head->tp->cflags &DF_CONST)
                    genclass2error(ERR_CONSTNOCONS, head->parentclass->name,
                        head->name);
            }
            head->parentclass = sp;
            head = head->next;
        }
    }
    ENODE *conscall(ENODE *tcnode, SYM *sp, ENODE *ep)
    {
        ENODE *pnode,  *rnode,  *runup = 0,  *rundown = 0;
        pnode = makenode(en_napccon, sp, 0);
        sp->mainsym->extflag = TRUE;
        if (!ep)
        {
            SYM *t = sp->tp->lst.head;
            if (!t || t == (SYM*) - 1)
                ep = makenode(en_void, 0, 0);
            else
            {
                while (t)
                {
                    ep = makenode(en_void, copy_default(t
                        ->value.classdata.defalt->ep), ep);
                    if (t->value.classdata.defalt->cons)
                        if (runup)
                            runup = makenode(en_void, runup, copy_default(t
                                ->value.classdata.defalt->cons));
                        else
                            runup = copy_default(t->value.classdata.defalt
                                ->cons);
                    if (t->value.classdata.defalt->dest)
                        rundown = makenode(en_void, copy_default(t
                            ->value.classdata.defalt->dest), rundown);
                    t = t->next;
                }
                ep = makenode(en_void, ep, 0);
            }
        }
		else
			ep = copynode(ep);
        rnode = makenode(en_void, pnode, pnode);
        pnode = makenode(en_void, rnode, ep);
        rnode = makeintnode(en_icon, sp->tp->btp->size);
        if (sp->pascaldefn)
            pnode = makenode(en_pfcall, rnode, pnode);
        else
            if (sp->isstdcall)
                pnode = makenode(en_sfcall, rnode, pnode);
            else
                pnode = makenode(en_fcall, rnode, pnode);
        pnode = makenode(en_thiscall, tcnode, pnode);
        pnode = doinline(pnode);
        if (runup || rundown)
        {
            pnode = makenode(en_dvoid, pnode, rundown);
            pnode->cflags |= DF_ACTUALEXPR;
        }
        if (runup)
            pnode = makenode(en_void, runup, pnode);
        return pnode;
    }
    void setthunkflags(SYM *sp_in, int state)
    {
        CLASSLIST *l = sp_in->value.classdata.enclosedclass;
        return ;
        while (l)
        {
            setthunkflags(l->data->tp->sp, state);
            l = l->link;
        }
        l = sp_in->value.classdata.baseclass;
        while (l)
        {
            if (!state)
                l->data->value.classdata.cppflags &= ~PF_TEMPCONSALREADY;
            else
                l->data->value.classdata.cppflags |= PF_TEMPCONSALREADY;
            l = l->link;
        }
    }
    void curcons(SYM *sp_in, SYM *vtabsp, ENODE **exps, CLASSLIST *l, ENODE
        *thisn, int checkaccess, int ofs)
    {
        ENODE *exp1,  *exp2,  *ts;
        //   if (l->link)
        //      curcons(sp_in,vtabsp,exps,l->link,thisn,checkaccess,ofs) ;
        while (l)
        {
            if ((l->flags &CL_VTAB) && !(l->flags &CL_TOPCLASS))
            {
                if (checkaccess)
                {
                    SYM *sp1 = search(cpp_funcname_tab[CI_CONSTRUCTOR], &l
                        ->data->tp->lst);
                    if (sp1)
                    {
                        sp1 = funcovermatch(sp1, &functovoidtype, 0, FALSE,
                            FALSE);
                        if (sp1)
                            if (!isaccessible(sp1))
                                genclasserror(ERR_NOTACCESSIBLE, fullcppmangle
                                    (sp1->parentclass, sp1->name, 0));

                    }
                    else
                    // should never get here
                        gensymerror(ERR_NOFUNCMATCH, fullcppmangle(sp_in,
                            cpp_funcname_tab[CI_CONSTRUCTOR], &stdvoidfunc));
                }
                ts = makenode(en_nacon, vtabsp, 0);
                exp1 = makenode(en_addstruc, ts, makeintnode(en_icon, l
                    ->vtaboffs + vtabhead));
                exp2 = makenode(en_addstruc, thisn, makeintnode(en_icon, (l
                    ->offset + ofs)));
                exp2 = makenode(en_a_ref, exp2, 0);
                exp2 = makenode(en_assign, exp2, exp1);
                if (! *exps)
                    *exps = exp2;
                else
                    *exps = makenode(en_void, (void*)exp2, (void*) * exps);
            }
            l = l->link;
        }
    }
    void prevcons(SYM *sp_in, ENODE **exps, CLASSLIST *l, ENODE *thisn, int
        checkaccess, int ofs)
    {
        ENODE *exp1,  *exp2,  *ts;
        while (l)
        {
            if (l->data->value.classdata.consbuild)
            {
                exp2 = l->data->value.classdata.consbuild;
                l->data->value.classdata.consbuild = 0;
                setthunkflags(l->data->tp->sp, TRUE);
                if (! *exps)
                    *exps = exp2;
                else
                    *exps = makenode(en_void, (void*)exp2, (void*) * exps);
            }
            else
            {
                if (l->flags &CL_PRIMARY)
                {
                    SYM *sp1 = search(cpp_funcname_tab[CI_CONSTRUCTOR], &l
                        ->data->tp->lst);
                    if (sp1)
                    {
                        SYM *sp2 = funcovermatch(sp1, &functovoidtype, 0, FALSE,
                            FALSE);
                        if (!sp2)
                            gensymerror(ERR_NOFUNCMATCH, fullcppmangle(sp1,
                                cpp_funcname_tab[CI_CONSTRUCTOR], &stdvoidfunc))
                                ;
                        else
                        {
                            if (!isaccessible(sp2))
                                genclasserror(ERR_NOTACCESSIBLE, fullcppmangle
                                    (sp1->parentclass, sp1->name, 0));
                            exp2 = makenode(en_addstruc, thisn, makeintnode
                                (en_icon, (l->offset + ofs)));
                            exp2 = conscall(exp2, sp2, 0);
                            setthunkflags(l->data->tp->sp, TRUE);
                            if (! *exps)
                                *exps = exp2;
                            else
                                *exps = makenode(en_void, (void*)exp2, (void*)
                                    * exps);
                        }
                    } //else // should never get here
                    //        gensymerror(ERR_NOFUNCMATCH,fullcppmangle(l->data,cpp_funcname_tab[CI_CONSTRUCTOR],&stdvoidfunc)) ; 
                }
            }
            l = l->link;
        }
    }
    void embeddedcons(SYM *sp_in, SNODE *snp3, ENODE **exps, ENODE *thisn)
    {
        SYM *spt = sp_in->tp->lst.head;
        ENODE *exp1,  *exp2;
        while (spt)
        {
            if (!(spt->value.classdata.cppflags &PF_INHERITED))
            {
                if (spt->value.classdata.consbuild)
                {
                    exp1 = spt->value.classdata.consbuild;
                    if (isstructured(spt->tp))
                    {
                        setthunkflags(spt->tp->sp, TRUE);
                    }
                    else
                    {
                        if (spt->tp->type == bt_ref)
                        {
                            spt->value.classdata.cppflags |=
                                PF_TEMPCONSREFALREADY;
                            if (!lvalue(exp1))
                                generror(ERR_REFLVALUE, 0, 0);
                            else
                                exp1 = exp1->v.p[0];
                        }
                        if (spt->tp->cflags &DF_CONST)
                            spt->value.classdata.cppflags |=
                                PF_TEMPCONSREFALREADY;
                        exp2 = makenode(en_addstruc, (void*)thisn, (void*)
                            makeintnode(en_icon, spt->value.i));
                        exp2 = makenode(en_a_ref, (void*)exp2, 0);
                        exp1 = makenode(en_assign, (void*)exp2, (void*)exp1);
                    }
                    if (!snp3->exp)
                        snp3->exp = exp1;
                    else
                        snp3->exp = makenode(en_void, (void*)snp3->exp, (void*)
                            exp1);
                    spt->value.classdata.consbuild = 0;
                }
                else if (spt->storage_class == sc_member && isstructured(spt
                    ->tp))
                {
                    SYM *sp1 = search(cpp_funcname_tab[CI_CONSTRUCTOR], &spt
                        ->tp->lst);
                    if (sp1)
                    {
                        SYM *sp2 = funcovermatch(sp1, &functovoidtype, 0, FALSE,
                            FALSE);
                        if (!sp2)
                            gensymerror(ERR_NOFUNCMATCH, fullcppmangle(sp1,
                                cpp_funcname_tab[CI_CONSTRUCTOR], &stdvoidfunc))
                                ;
                        else
                        {
                            if (!isaccessible(sp2))
                                genclasserror(ERR_NOTACCESSIBLE, fullcppmangle
                                    (sp1->parentclass, sp1->name, 0));
                            exp2 = makenode(en_addstruc, thisn, makeintnode
                                (en_icon, (spt->value.i)));
                            exp2 = conscall(exp2, sp2, 0);
                            setthunkflags(spt, TRUE);
                            if (! *exps)
                                *exps = exp2;
                            else
                                *exps = makenode(en_void, (void*)exp2, (void*)
                                    * exps);
                        }
                    } //else // should never get here
                    //              gensymerror(ERR_NOFUNCMATCH,fullcppmangle(spt,cpp_funcname_tab[CI_CONSTRUCTOR],&stdvoidfunc)) ; 
                }
            }
            spt = spt->next;
        }
    }
    SYM *lookupcopycons(SYM *sp_in, SYM **spr)
    {
        SYM *sp1 = search(cpp_funcname_tab[CI_CONSTRUCTOR], &sp_in->tp->lst);
        if (spr)
            *spr = sp1;
        if (sp1)
        {
            sp1 = sp1->tp->lst.head;
            while (sp1)
            {
                SYM *sp2 = sp1->tp->lst.head;
                if (sp2 != 0 && sp2 !=  - 1 && sp2->next == 0)
                    if (sp2->tp->type == bt_ref && isstructured(sp2->tp->btp)
                        && sp2->tp->btp->sp->mainsym == sp_in->mainsym)
                        return sp1;
                sp1 = sp1->next;
            }
        }
        return 0;
    }
    void prevcopycons(SYM *sp_in, ENODE *argn, ENODE **exps, CLASSLIST *l,
        ENODE *thisn, int checkaccess, int ofs)
    {
        ENODE *exp1,  *exp2,  *ts;
        while (l)
        {
            if (l->flags &CL_PRIMARY)
            {
                    SYM *sp1;
                    SYM *sp2 = lookupcopycons(l->data, &sp1);
                    if (!sp2) {
                        gensymerror(ERR_NOFUNCMATCH, fullcppmangle(sp1,
                            cpp_funcname_tab[CI_CONSTRUCTOR], &stdvoidfunc));
                        
                    }
                    else
                    {
                        if (!isaccessible(sp2))
                            genclasserror(ERR_NOTACCESSIBLE, fullcppmangle(sp1
                                ->parentclass, sp1->name, 0));
                        exp2 = makenode(en_addstruc, thisn, makeintnode(en_icon,
                            (l->offset + ofs)));
                        exp2 = conscall(exp2, sp2, argn);
                        if (! *exps)
                            *exps = exp2;
                        else
                            *exps = makenode(en_void, (void*)exp2, (void*) *
                                exps);
                    }
            }
            l = l->link;
        }
    }
    void embeddedcopycons(SYM *sp_in, ENODE *argn, SNODE *snp3, ENODE **exps,
        ENODE *thisn)
    {
        SYM *spt = sp_in->tp->lst.head;
        ENODE *exp1,  *exp2;
        int offset =  - 1;
        while (spt)
        {
            if (!(spt->value.classdata.cppflags &PF_INHERITED))
            {
                if (spt->storage_class == sc_member && isstructured(spt->tp))
                {
                    SYM *sp1;
                    SYM *sp2 = lookupcopycons(spt->tp->sp, &sp1);
                    if (offset !=  - 1)
                    {
                        exp2 = makenode(en_addstruc, thisn, makeintnode(en_icon,
                            (offset)));
                        exp1 = makenode(en_addstruc, argn, makeintnode(en_icon,
                            (offset)));
                        exp2 = makenode(en_moveblock, exp2, exp1);
                        exp2->size = spt->value.i - offset;
                        if (! *exps)
                            *exps = exp2;
                        else
                            *exps = makenode(en_void, (void*)exp2, (void*) *
                                exps);
                        offset =  - 1;
                    }
                    if (!sp2)
                    {
                        gensymerror(ERR_NOFUNCMATCH, fullcppmangle(sp1,
                            cpp_funcname_tab[CI_CONSTRUCTOR], &stdvoidfunc));
                    }
                    else
                    {
                        if (!isaccessible(sp2))
                            genclasserror(ERR_NOTACCESSIBLE, fullcppmangle(sp1
                                ->parentclass, sp1->name, 0));
                        exp2 = makenode(en_addstruc, thisn, makeintnode(en_icon,
                            (spt->value.i)));
                        exp2 = conscall(exp2, sp2, makenode(en_void,makenode(en_a_ref,argn,0),0));
                        if (! *exps)
                            *exps = exp2;
                        else
                            *exps = makenode(en_void, (void*)exp2, (void*) *
                                exps);
                    }
                }
                else if (offset ==  - 1 && spt->storage_class == sc_member &&
                    isscalar(spt->tp))
                    offset = spt->value.i ;
            }
            else if (isscalar(spt->tp) || isstructured(spt->tp))
                offset = spt->value.i ;
            spt = spt->next;
        }
        if (offset !=  - 1)
        {
            exp2 = makenode(en_addstruc, thisn, makeintnode(en_icon, (offset)));
            exp1 = makenode(en_addstruc, argn, makeintnode(en_icon, (offset)));
            exp2 = makenode(en_moveblock, exp2, exp1);
            exp2->size = sp_in->tp->size - offset;
            if (! *exps)
                *exps = exp2;
            else
                *exps = makenode(en_void, (void*)exp2, (void*) * exps);
        }
    }
    /* things are placed in the constructor expression in backwards order */
    void thunkConstructors(SNODE *snp3, SYM *sp_in, int ofs, int checkaccess)
    {
        CLASSLIST *l;
        ENODE *thisn = copynode(thisenode);
        ENODE *exp1,  *exp2,  *exps;
        sp_in = sp_in->mainsym;
        thisn = makenode(en_a_ref, thisn, 0);
        /* prev constructors */
        l = sp_in->value.classdata.baseclass->link;
        exps = 0;
        if (l)
            prevcons(sp_in, &exps, l, thisn, checkaccess, ofs);
        if (exps)
            if (!snp3->exp)
                snp3->exp = exps;
            else
                snp3->exp = makenode(en_void, (void*)snp3->exp, (void*)exps);
        /* current constructor vtab updates */
        l = sp_in->value.classdata.baseclass;
        exps = 0;
        if (l)
            curcons(sp_in, sp_in->value.classdata.baseclass->vtabsp, &exps, l,
                thisn, checkaccess, ofs);
        if (exps)
            if (!snp3->exp)
                snp3->exp = exps;
            else
                snp3->exp = makenode(en_void, (void*)snp3->exp, (void*)exps);
        /* embedded data for this class */
        exps = 0;
        embeddedcons(sp_in, snp3, &exps, thisn);
        if (exps)
            if (!snp3->exp)
                snp3->exp = exps;
            else
                snp3->exp = makenode(en_void, (void*)snp3->exp, (void*)exps);
    }
    void thunkCopyConstructors(SNODE *snp3, SYM *sp_in, SYM *arg, int ofs, int
        checkaccess)
    {
        CLASSLIST *l;
        ENODE *thisn = copynode(thisenode);
        ENODE *argn = makenode(en_autocon, (void*)arg, 0);
        ENODE *exp1,  *exp2,  *exps;
        argn = makenode(en_a_ref, argn, 0);
//        argn = makenode(en_l_ref, argn, 0);
//        argn = makenode(en_void, argn, 0);
        sp_in = sp_in->mainsym;
        thisn = makenode(en_a_ref, thisn, 0);
        /* prev constructors */
        l = sp_in->value.classdata.baseclass->link;
        exps = 0;
        if (l)
            prevcopycons(sp_in, argn, &exps, l, thisn, checkaccess, ofs);
        if (exps)
            if (!snp3->exp)
                snp3->exp = exps;
            else
                snp3->exp = makenode(en_void, (void*)snp3->exp, (void*)exps);
        /* embedded data for this class */
        exps = 0;
        embeddedcopycons(sp_in, argn, snp3, &exps, thisn);
        if (exps)
            if (!snp3->exp)
                snp3->exp = exps;
            else
                snp3->exp = makenode(en_void, (void*)snp3->exp, (void*)exps);
    }
    void thunkDestructors(SNODE *snp3, SYM *sp_in, int ofs, int checkaccess,
        int skipself)
    {
        CLASSLIST *l;
        ENODE *thisn = copynode(thisenode);
        ENODE *exp1,  *exp2;
        SYM *spt = sp_in->tp->lst.head;
        sp_in = sp_in->mainsym;
        thisn = makenode(en_a_ref, thisn, 0);
        // embedded classes
        while (spt)
        {
            if (!(spt->value.classdata.cppflags &PF_INHERITED) && isstructured
                (spt->tp))
                thunkDestructors(snp3, spt->tp->sp, spt->value.i, TRUE, FALSE);
            spt = spt->next;
        }
        l = sp_in->value.classdata.baseclass;
        if (skipself)
            l = l->link;
        /* base classes */
        while (l)
        {
            SYM *sp1 = search(cpp_funcname_tab[CI_DESTRUCTOR], &l->data->tp
                ->lst);
            if (sp1)
            {
                sp1 = funcovermatch(sp1, &functovoidtype, 0, FALSE, FALSE);
                if (sp1 && !(sp1->value.classdata.cppflags &PF_DUMDEST))
                {
                    if (!(l->data->value.classdata.cppflags &PF_TEMPCONSALREADY)
                        )
                    {
                        if (!isaccessible(sp1))
                            genclasserror(ERR_NOTACCESSIBLE, fullcppmangle(sp1
                                ->parentclass, sp1->name, 0));
                        exp2 = makenode(en_addstruc, thisn, makeintnode(en_icon,
                            (l->offset + ofs)));
                        exp2 = conscall(exp2, sp1, 0);
                        if (!snp3->exp)
                            snp3->exp = exp2;
                        else
                            snp3->exp = makenode(en_void, (void*)exp2, (void*)
                                snp3->exp);
                        setthunkflags(l->data->tp->sp, TRUE);
                    }
                }
            }

            l = l->link;
        }
    }
    void CreateBaseConstructor(SYM *sp_in)
    {
        SYM *sp = search(cpp_funcname_tab[CI_CONSTRUCTOR], &sp_in->tp->lst);
        if (!sp)
        {
            SNODE *snp3;
            sp = makesym(sc_externalfunc);
            sp->name = fullcppmangle(sp_in, cpp_funcname_tab[CI_CONSTRUCTOR],
                &vtyp);
            sp->tp = copytype(&functovoidtype, 0);
            sp->value.classdata.cppflags = PF_INLINE | PF_CONSTRUCTOR |
                PF_MEMBER | PF_PUBLIC;
            sp->parentclass = sp_in;
            snp3 = xalloc(sizeof(SNODE));
            snp3->stype = st_expr;
            snp3->exp = 0;
            setthunkflags(sp_in, FALSE);
            thunkConstructors(snp3, sp_in, 0, FALSE);
            sp->value.classdata.inlinefunc = xalloc(sizeof(INLINEFUNC));
            sp->value.classdata.inlinefunc->stmt = snp3;
            sp->value.classdata.inlinefunc->syms = 0;
            funcrefinsert(cpp_funcname_tab[CI_CONSTRUCTOR], sp, &sp_in->tp->lst,
                sp_in);
        }
        if (!lookupcopycons(sp_in, 0))
        {
            {
                SNODE *snp3;
                SYM *fsym = makesym(sc_auto);
                TYP *ftype = maketype(bt_ifunc, 0);
                ftype->lst.head = ftype->lst.tail = fsym;
                fsym->tp = maketype(bt_ref, 4);
                ;
                fsym->tp->btp = copytype(sp_in->tp, 0);
                fsym->tp->btp->cflags |= DF_CONST;
                fsym->value.i = stdaddrsize * 3;
                fsym->funcparm = TRUE;
                fsym->next = 0;
                fsym->name = "dumcopyconsarg";
				ftype->btp = fsym->tp;
                snp3 = xalloc(sizeof(SNODE));
                snp3->stype = st_expr;
                snp3->exp = 0;
                thunkCopyConstructors(snp3, sp_in, fsym, 0, FALSE);
                sp = makesym(sc_externalfunc);
                sp->name = fullcppmangle(sp_in,
                    cpp_funcname_tab[CI_CONSTRUCTOR], ftype);
                sp->tp = ftype;
                sp->value.classdata.cppflags = PF_INLINE | PF_CONSTRUCTOR |
                    PF_MEMBER | PF_PUBLIC;
                sp->parentclass = sp_in;
                sp->value.classdata.inlinefunc = xalloc(sizeof(INLINEFUNC));
                sp->value.classdata.inlinefunc->stmt = snp3;
                sp->value.classdata.inlinefunc->syms = xalloc(sizeof(LIST));
                sp->value.classdata.inlinefunc->syms->data = fsym;
                funcrefinsert(cpp_funcname_tab[CI_CONSTRUCTOR], sp, &sp_in->tp
                    ->lst, sp_in);
            }
        }
        sp = search(cpp_funcname_tab[CI_DESTRUCTOR], &sp_in->tp->lst);
        if (!sp)
        {
            SNODE *snp3;
            snp3 = xalloc(sizeof(SNODE));
            snp3->stype = st_expr;
            snp3->exp = 0;
            setthunkflags(sp_in, FALSE);
            thunkDestructors(snp3, sp_in, 0, FALSE, FALSE);
            if (snp3->exp)
            {
                sp = makesym(sc_externalfunc);
                sp->name = fullcppmangle(sp_in, cpp_funcname_tab[CI_DESTRUCTOR],
                    &vtyp);
                sp->tp = copytype(&functovoidtype, 0);
                sp->value.classdata.cppflags = PF_INLINE | PF_DESTRUCTOR |
                    PF_MEMBER | PF_PUBLIC | PF_DUMDEST;
                sp->parentclass = sp_in;
                sp->value.classdata.inlinefunc = xalloc(sizeof(INLINEFUNC));
                sp->value.classdata.inlinefunc->stmt = snp3;
                sp->value.classdata.inlinefunc->syms = 0;
                funcrefinsert(cpp_funcname_tab[CI_DESTRUCTOR], sp, &sp_in->tp
                    ->lst, sp_in);
            }
        }
    }
    void classbaseasn(SYM *sp, SYM *func)
    /*
     *      handles a list of constructor base assignment operators
     */
    {
        CLASSLIST *ls;
        sp = sp->mainsym;
        getsym();
        if (func->tp->lst.head != (SYM*) - 1)
            baseasnsymtbl = &func->tp->lst;
        currentfunc = func;
        ls = sp->value.classdata.baseclass;
        while (TRUE)
        {
            ls = 0;
            if (lastst != id)
            {
                generror(ERR_IDEXPECT, 0, 0);
                basicskim(skm_declbegin);
                baseasnsymtbl = 0;
                return ;
            }
            else
            {
                int discard = FALSE;
                SYM *sp1 = search(lastid, &sp->tp->lst);
                if (sp1)
                {
                    if (sp1->value.classdata.cppflags &PF_INHERITED)
                    {
                        discard = TRUE;
                        doubleerr(ERR_NOTUNAMBIGUOUSBASE, lastid, sp->name);
                    }
                }
                else
                {
                    CLASSLIST *l = sp->value.classdata.baseclass->link;
                    while (l)
                    {
                        if (!strcmp(l->data->name, lastid))
                            break;
                        l = l->link;
                    }
                    if (!l /* || !(l->flags & CL_VTAB)*/)
                    {
                        discard = TRUE;
                        doubleerr(ERR_NOTUNAMBIGUOUSBASE, lastid, sp->name);
                    }
                    else
                    {
                        sp1 = l->data->tp->sp;
                        ls = l;
                    }
                }
                getsym();
                if (needpunc(openpa, skm_closepa))
                {
                    ENODE *expr;
                    TYP *tp;
                    if (!discard)
                    {
                        typequal = 0;
                        if (sp1->value.classdata.consbuild)
                            gensymerror(ERR_MEMBERREINIT, sp1->name);
                        if (isstructured(sp1->tp))
                        {
                            ENODE *qnode;
                            TYP *tp1;
                            expr_runup[expr_updowncount] = 0;
                            expr_rundown[expr_updowncount++] = 0;
                            tp1 = gatherparms(&qnode, FALSE);
                            if (ls)
                                expr = do_constructor(sp, tp1, sp1->tp, qnode,
                                    1, ls->offset, FALSE, FALSE);
                            else
                                expr = do_constructor(sp, tp1, sp1->tp, qnode,
                                    1, sp1->value.i, FALSE, FALSE);
                            expr_updowncount--;
                            if (expr_runup[expr_updowncount] ||
                                expr_rundown[expr_updowncount])
                            {
                                expr = makenode(en_dvoid, expr,
                                    expr_rundown[expr_updowncount]);
                                (expr)->cflags |= DF_ACTUALEXPR;
                            }
                            if (expr_runup[expr_updowncount])
                                expr = makenode(en_void,
                                    expr_runup[expr_updowncount], expr);
                            if (expr)
                                expr = repcallb(expr);
                            sp1->value.classdata.consbuild = expr;
                        }
                        else
                        {
                            tp = exprnc(&expr);
                            if (!exactype(sp1->tp, tp, FALSE) && (expr->nodetype !=
                                en_icon || expr->v.i != 0))
                                genmismatcherror(tp, sp1->tp);
                            else if (sp1->value.classdata.cppflags &PF_STATIC)
                                gensymerror(ERR_NOINITSTATICMEMBER, sp1->name);
                            else
                                sp1->value.classdata.consbuild = expr;
                            needpunc(closepa, 0);
                        }
                    }
                    else
                    {
                        tp = expression(&expr, 0);
                        needpunc(closepa, 0);
                    }
                }
            }
            if (lastst != comma)
                break;
            getsym();
        }
        baseasnsymtbl = 0;
        currentfunc = 0;
    }
    void setRefFlags(SYM *sp)
    {
        sp = sp->tp->lst.head;
        while (sp)
        {
            if (sp->tp->type == bt_ref || (sp->tp->cflags &DF_CONST))
                sp->value.classdata.cppflags &= ~PF_TEMPCONSREFALREADY;
            sp = sp->next;
        }
    }
    void RefErrs(SYM *sp)
    {
        sp = sp->tp->lst.head;
        while (sp)
        {
            if (!(sp->value.classdata.cppflags &(PF_TEMPCONSREFALREADY |
                PF_INHERITED)))
            {
                if (sp->tp->type == bt_ref)
                    gensymerror(ERR_CONSNOREFINIT, sp->name);
                else if (sp->tp->cflags &DF_CONST)
                    gensymerror(ERR_CONSNOCONSTINIT, sp->name);
            }

            sp = sp->next;
        }
    }
    void gen_vtab(SYM *sp)
    {
        SYM *head;
        LIST *lst;
        CLASSLIST *l;
        l = sp->value.classdata.baseclass;
        if ((l->flags &CL_VTAB))
        {
            sp->value.classdata.baseclass->vtabsp->gennedvirtfunc = TRUE;
            gen_virtual(sp->value.classdata.baseclass->vtabsp, FALSE);
            if (prm_xcept)
                genref(getxtsym(sp->tp), 0);
             /* space for exception handling */
            else
                genlong(0);
            genlong(0);
            genlong(0); /* reserved as a beginning of vtab marker */
            lst = l->vtablist;
            while (lst)
            {
                SYM *sp = lst->data;
                if ((int)sp < VTABVALTHRESH)
                {
                    genlong((int)sp);
                }
                else
                {
                    SetExtFlag(sp, TRUE);
                    genpcref(sp, 0);
                }
                lst = lst->link;
            }
            gen_endvirtual(sp->value.classdata.baseclass->vtabsp);
        }
    }
