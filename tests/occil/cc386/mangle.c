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
/* Handles name mangling
 */
#include <stdio.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "ccerr.h"

extern TABLE lsyms,  *gsyms;
extern int prm_cplusplus, prm_cmangle;
extern SYM *declclass;
extern SYM *typequal;
extern char *templatePrefix;
extern TYP stdvoidfunc;
extern char *cpponearg(char *buf, TYP *tp);
extern struct template  *currentTemplate,  *searchTemplate;
extern SYM *parm_namespaces[20][100];
extern int parm_ns_counts[20];
extern int parm_ns_level;
extern int parsing_params;


char *unmang1(char *buf, char *name, int firsttime);

    char *tn_void = "void";
    char *tn_char = "char";
    char *tn_int = "int";
    char *tn_long = "long";
    char *tn_longlong = "long long";
    char *tn_short = "short ";
    char *tn_unsigned = "unsigned ";
    char *tn_ellipse = "...";
    char *tn_float = "float";
    char *tn_double = "double";
    char *tn_longdouble = "long double";
    char *tn_vol = " volatile ";
    char *tn_const = " const ";
    char *tn_class = "class ";
    char *tn_struct = "struct ";
    char *tn_union = "union ";
    char *tn_fcomplex = "float complex";
    char *tn_rcomplex = "double complex";
    char *tn_lrcomplex = "long double complex";

    char *cpp_funcname_tab[] = 
    {
        "$bctr", "$bdtr", "$bnew", "$bdele", "$badd", "$bsub", "$bmul", "$bdiv",
            "$bshl", "$bshr", "$bmod", "$bequ", "$bneq", "$blt", "$bleq", 
            "$bgt", "$bgeq", "$basn", "$basadd", "$bassub", "$basmul", 
            "$basdiv", "$basmod", "$basshl", "$bsasshr", "$basand", "$basor", 
            "$basxor", "$binc", "$bdec", "$barray", "$bcast", "$bpstar", 
            "$barrow", "$blor", "$bland", "$bnot", "$bor", "$band", "$bxor", 
            "$bcpl", "$bnwa", "$bdla", 

    };
    char *xlate_tab[] = 
    {
        0, 0, "new", "delete", "+", "-", "*", "/", "<<", ">>", "%", "==", "!=",
            "<", "<=", ">", ">=", "=", "+=", "-=", "*=", "/=", "%=", "<<=", 
            ">>=", "&=", "|=", "^=", "++", "--", "[]", "()", "->*", "->", "||",
            "&&", "!", "|", "&", "^", "~", "new[]", "delete[]"
    };
    #define IT_THRESHOLD 2
    #define IT_OV_THRESHOLD 2
    #define IT_SIZE (sizeof(cpp_funcname_tab)/sizeof(char *))

#define MAX_MANGLE_NAME_COUNT 36

char *cppargs(char *buf, SYM *sp);
static char *unmangcppfunc(char *buf, char *name, int firsttime);
static char *unmangcpptype(char *buf, char *name, int firsttime);
char *cpponearg(char *buf, TYP *tp);
char *unmang1(char *buf, char *name, int firsttime);
int manglenamecount =  - 1;
static char manglenames[MAX_MANGLE_NAME_COUNT][512];

    void cpp_unmang_intrins(char **buf, char **name, char *last)
    {
        char cur[245],  *p = cur,  *q;
        int i;
        *p++ = *(*name)++;
        while (**name != '@' &&  **name != '$' &&  **name)
            *p++ = *(*name)++;
        *p = 0;
        if (cur[1] == 'o')
        {
            strcpy(*buf, "operator ");
            *buf += strlen(*buf);
            unmang1(*buf, cur + 2, FALSE);
        }
        else
        {
            for (i = 0; i < IT_SIZE; i++)
                if (!strcmp(cur, cpp_funcname_tab[i]))
                    break;
            if (i >= IT_SIZE)
                strcpy(*buf, cur);
            else
            {
                if (i < IT_THRESHOLD)
                {
                    switch (i)
                    {
                        case 1:
                            *(*buf)++ = '~';
                        case 0:
                            strcpy(*buf, last);
                            break;
                    }
                }
                else
                {
                    strcpy(*buf, "operator ");
                    strcat(*buf, xlate_tab[i]);
                }
            }
        }
        *buf += strlen(*buf);

    }
/* Insert an overloaded function ref into the function table
 */
void funcrefinsert(char *name, SYM *refsym, TABLE *tbl, SYM *insp)
{
        char buf[100];
        SYM *sp,  *sp1;
        sp1 = search2(name, tbl, FALSE, TRUE);
        if (sp1)
            insert(refsym, &sp1->tp->lst);
        else
        {
            sp = makesym(sc_defunc);
            sp->name = litlate(name);
            sp->tp = maketype(bt_defunc, 0);
            sp->tp->sp = sp;
            sp->tp->lst.head = sp->tp->lst.tail = refsym;
            sp->parentclass = insp;
            insert(sp, tbl);
        }
}

//-------------------------------------------------------------------------

    static int templateNameSearch(TYP *typ1, TYP *typ2)
    {
        while (typ1->type == bt_pointer && typ2->type == bt_pointer)
        {
            typ1 = typ1->btp, typ2 = typ2->btp;
        }
        if (isstructured(typ1) && isstructured(typ2))
        {
            SYM *sp2 = currentTemplate->classes.head;
            SYM *sp1 = searchTemplate->classes.head;
            while (sp1 && sp2)
            {
                if (!strcmp(sp2->name, typ2->sp->name))
                    if (!strcmp(sp1->name, typ1->sp->name))
                        return TRUE;
                    else
                        return FALSE;
                sp1 = sp1->next;
                sp2 = sp2->next;
            }

        }
        return FALSE;
    }
    int matchone(SYM *sp1, SYM *sp2, ENODE *node, int nodefault, int any)
    {
        int rv = 1;
        CLASSLIST *l;
        ENODE *ep = node;
        /* don't match templates here */
        while (sp1 && sp1 != (SYM*) - 1 && sp2 && sp2 != (SYM*) - 1)
        {
            if (!exactype(sp1->tp, sp2->tp, FALSE))
            {
                TYP *typ1 = sp1->tp;
                TYP *typ2 = sp2->tp;
                if (typ1->type == bt_ellipse || typ2->type == bt_ellipse)
                    return rv;
                if (typ1->type == bt_ref)
                    typ1 = typ1->btp;
                if (typ2->type == bt_ref)
                    typ2 = typ2->btp;
                if (typ1->type == bt_pointer && typ2->type == bt_pointer &&
                    typ1->btp->type == bt_void)
                    rv = 2;
                else if (isscalar(typ1) && isscalar(typ2) && typ1->type !=
                    bt_enum)
                    rv = 2;
                else if (typ2->type == bt_defunc)
                {
                    if (typ1->type == bt_pointer && (typ1->btp->type == bt_func
                        || typ1->btp->type == bt_ifunc))
                    {
                        if (!funcovermatch2(typ2->sp, typ1->btp, 0, FALSE,
                            FALSE, FALSE))
                            return 0;
                    }
                    else
                        return 0;
                }
                else if (typ1->type == bt_memberptr)
                {
                    if (typ2->type == bt_memberptr && typ2->btp && typ2->btp
                        ->type == bt_defunc)
                    {
                        if (!funcovermatch2(typ2->btp->sp, typ1->btp, node,
                            FALSE, FALSE, FALSE))
                            return 0;
                    }
                    else
                        if (!isscalar(typ2) || !ep || ep->v.p[0]->nodetype !=
                            en_icon || ep->v.p[0]->v.i != 0)
                            return 0;
                }
                else if (currentTemplate && searchTemplate &&
                    templateNameSearch(typ1, typ2))
                {
                    ; // empty statement
                }
                else if (any && isstructured(typ1))
                {
                    SYM *sp3 = search(cpp_funcname_tab[CI_CONSTRUCTOR], &typ1
                        ->lst);
                    if (!sp3)
                        return 0;
                    sp3 = sp3->tp->lst.head;
                    while (sp3)
                    {
                        if (sp3 && sp3->tp->lst.head == sp3->tp->lst.tail)
                        {
                            if (exactype(sp3->tp->lst.head->tp, typ2, FALSE))
                                goto join;
                        }
                        sp3 = sp3->next;
                    }
                    if (isstructured(typ2) && typ2->sp
                        ->value.classdata.baseclass)
                    {
                        l = typ2->sp->value.classdata.baseclass->link;
                        while (l)
                        {
                            if (l->data->mainsym == typ1->sp->mainsym)
                                break;
                            l = l->link;
                        }
                        if (!l)
                            return 0;
                    }
                    else
                        return 0;
                }
                else if (typ1->type == bt_pointer && isstructured(typ1->btp))
                {
                    if (typ2->type == bt_pointer && isstructured(typ2->btp))
                    {
                        if (typ2->btp->sp->value.classdata.baseclass)
                        {
                            l = typ2->btp->sp->value.classdata.baseclass->link;
                            while (l)
                            {
                                if (l->data->mainsym == typ1->btp->sp->mainsym)
                                    break;
                                l = l->link;
                            }
                            if (!l)
                                return 0;
                        }
                        else
                            return 0;
                    }
                    else
                        if (!isscalar(typ2) || !ep || ep->v.p[0]->nodetype !=
                            en_icon || ep->v.p[0]->v.i != 0)
                            return 0;
                }
                else
                    if (typ1->type != bt_pointer || !isscalar(typ2) || !ep ||
                        ep->v.p[0]->nodetype != en_icon || ep->v.p[0]->v.i != 0)
                        return 0;
            } else if (!checktypeassign(sp1->tp,sp2->tp))
                return 0;
            join: sp1 = sp1->next;
            sp2 = sp2->next;
            if (ep)
                ep = ep->v.p[1];
        }
        /* If we get here one of the lists has quit.  If it isn't sp2
         * or if there is no defalt value, bail
         */
        if (!sp1 && !sp2)
            return rv;
        if (sp1 == (SYM*) - 1)
            if (sp2 == (SYM*) - 1 || sp2 && sp2->tp->type == bt_void)
                return rv;
        if (sp2 && sp2 != (SYM*) - 1)
            return 0;
        if (sp1 && sp1 != (SYM*) - 1 && !nodefault && sp1
            ->value.classdata.defalt)
            return rv;
        return 0;
    }
/* Search the tables looking for a match for an argument type list */
int funciterate(SYM *sp1, TYP *tp, ENODE *node, int nodefault, int any, SYM
    **sp2, SYM **sp3, SYM **sp4)
{
    while (sp1 && sp1 != (SYM*) - 1)
    {
        if (!sp1->istemplate && !sp1->isinsttemplate)
        {
            switch (matchone(sp1->tp->lst.head, tp->lst.head, node, nodefault,
                any))
            {
                case 0:
                default:
                    break;
                case 1:
                    if (*sp2)
                    {
                        genfunc2error(ERR_AMBIGFUNC, funcwithns(sp1),
                            funcwithns(*sp2));
                        return TRUE;
                    }
                    else
                         *sp2 = sp1;
                    break;
                case 2:
                    if (! *sp3)
                        *sp3 = sp1;
                    else
                        if (! *sp4)
                            *sp4 = sp1;
                    break;
            }
        }
        sp1 = sp1->next;
    }
    return FALSE;
}

//-------------------------------------------------------------------------

SYM *funcovermatch2(SYM *tbsym, TYP *tp, ENODE *node, int exact, int nodefault,
    int any)
{
        int i;
        SYM *sp1 = tbsym->tp->lst.head;
        SYM *sp2 = 0;
        SYM *sp3 = 0,  *sp4 = 0;
        if (funciterate(sp1, tp, node, nodefault, any, &sp2, &sp3, &sp4))
            return sp2;
        for (i = 0; i < parm_ns_counts[parm_ns_level - 1]; i++)
        {
            SYM *spx = namespace_search(tbsym->name,
                parm_namespaces[parm_ns_level - 1][i]->value.classdata.parentns,
                FALSE);
            if (spx && spx->mainsym != tbsym->mainsym)
                if (funciterate(spx->tp->lst.head, tp, node, nodefault, any,
                    &sp2, &sp3, &sp4))
                    return sp2;
        }
        if (sp2 || exact)
            return sp2;
        if (sp3 && sp4)
            genfunc2error(ERR_AMBIGFUNC, funcwithns(sp3), funcwithns(sp4));
        return sp3;
}

//-------------------------------------------------------------------------

SYM *funcovermatch(SYM *tbsym, TYP *tp, ENODE *ep, int exact, int nodefault)
{

    SYM *sp;
    sp = funcovermatch2(tbsym, tp, ep, exact, nodefault, FALSE);
    if (sp)
        return sp;
    return funcovermatch2(tbsym, tp, ep, exact, nodefault, TRUE);

}

//-------------------------------------------------------------------------

SYM *castmatch(SYM *sp, TYP *tp, TABLE *tb)
{
    SYM *sp1,  *sp2 = 0;
    char name[256],  *nm1;
    int temp = tp->cflags;
    tp->cflags = 0;
    name[0] = '$';
    name[1] = 'o';
    cpponearg(name + 2, tp);
    nm1 = fullcppmangle(sp, name, &stdvoidfunc);
    sp1 = search(nm1, tb);
    tp->cflags = temp;
    return sp1;
}

//-------------------------------------------------------------------------

    /* Mangle one C++ argument */
    static void mname(char *buf, SYM *sp)
    {
        char *v;
        char *bufin = buf;
        char ibuf[512];
        if (!sp)
            return ;
        buf[0] = 0;
        if (sp->parentclass)
        {
            mname(buf, sp->parentclass);
            strcat(buf, "@");
            buf = buf + strlen(buf);
        }
        v = sp->name;
        if (!v)
            v = "$$bad";
        if (prm_cmangle &&  *v == '_')
            v++;
        strcat(buf, v);
        if (sp->value.classdata.parentns)
        {
            v = sp->value.classdata.parentns->sp->name;
            if (prm_cmangle &&  *v == '_')
                v++;
            strcpy(ibuf, v);
            strcat(ibuf, "@");
            strcat(ibuf, bufin);
            strcpy(bufin, ibuf);
        }
    }
    int lookupname(char *name)
    {
        int i;
        if (manglenamecount < 0)
            return 0;
        for (i = 0; i < manglenamecount; i++)
        if (!strcmp(manglenames[i], name))
        {
            if (i >= 10)
                i += 7;
            return i + '0';
        }
        if (manglenamecount >= MAX_MANGLE_NAME_COUNT)
            return 0;
        strcpy(manglenames[manglenamecount++], name);
        return 0;
    }
    static char *putvc(char *buf, TYP *tp)
    {
        if (tp->cflags &DF_CONST)
            *buf++ = 'x';
        if (tp->cflags &DF_VOL)
            *buf++ = 'y';
        return buf;
    }
    char *cppval(char *buf, TYP *tp)
    {
        int i;
        if (tp->sp && tp->sp->storage_class == sc_tconst)
        {
            *buf++ = '$';
            switch (tp->type)
            {
                case bt_pointer:
                    if (tp->btp->type == bt_char)
                    {
                        sprintf(buf, "s%s", tp->sp->value.s);
                    }
                    else if (tp->btp->type == bt_short)
                    {
                        *buf++ = 'w';
                        for (i = 0; i < tp->size; i++)
                        {
                            sprintf(buf, "%04X", ((short*)tp->sp->value.s)[i]);
                            buf += 4;
                        }
                        break;
                    }
                    break;
                case bt_bool:
                    if (tp->sp->value.i)
                        strcpy(buf, "btrue");
                    else
                        strcpy(buf, "bfalse");
                    break;
                case bt_char:
                case bt_short:
                case bt_enum:
                case bt_unsignedchar:
                case bt_unsignedshort:
                case bt_int:
                case bt_long:
                case bt_longlong:
                case bt_unsignedlonglong:
                case bt_unsigned:
                case bt_unsignedlong:
                    //#if sizeof(ULLONG_TYPE) == 4
                    sprintf(buf, "i%d", tp->sp->value.i);
                    //#else
                    //			sprintf(buf,"i%lld",tp->sp->value.i) ;
                    //#endif
                case bt_fcomplex:
                case bt_rcomplex:
                case bt_lrcomplex:
                    break;
                case bt_float:
                case bt_fimaginary:
                case bt_rimaginary:
                case bt_lrimaginary:
                case bt_double:
                case bt_longdouble:
                {
                    double aa;
                    FPFToDouble((char *)&aa, &tp->sp->value.f);
                    sprintf(buf, "f%f", aa);
                    break;
                }
            }

            buf += strlen(buf);
            *buf++ = '$';
            *buf = 0;
        }
        return buf;
    }
    void tplPlaceholder(char *buf, char *nm, LIST *tn)
    {
        *buf++ = '#';
        nm += prm_cmangle && nm[0] == '_';
        strcpy(buf, nm);
        buf += strlen(buf);
        *buf++ = '$';
        *buf++ = 't';
        while (tn)
        {
            buf = cpponearg(buf, tn->data);
            tn = tn->link;
        }
        *buf++ = '#';
        *buf = 0;
    }
    char *cpponearg(char *buf, TYP *tp)
    {
        char nm[512];
        int i;
        start: if (tp->type != bt_pointer || tp->btp->type != bt_func)
            buf = putvc(buf, tp);
        switch (tp->type)
        {
            case bt_templateplaceholder:
                tplPlaceholder(nm, tp->lst.head->name, tp->lst.tail);
                sprintf(buf, "%d%s", strlen(nm), nm);
                buf += strlen(buf);
                break;
            case bt_memberptr:
                *buf++ = 'M';
                mname(nm, tp->sp);
                if (!(i = lookupname(nm)))
                    sprintf(buf, "%d%s", strlen(nm), nm);
                else
                    sprintf(buf, "n%c", i);
                buf = buf + strlen(buf);
                if (tp->btp->type == bt_func)
                {
                    buf = putvc(buf, tp->btp);
                    *buf++ = 'q';
                    buf = cppargs(buf, tp->btp->lst.head);
                    *buf++ = '$';
                    buf = cpponearg(buf, tp->btp->btp);
                }
                else
                {
                    buf = cpponearg(buf, tp->btp);
                }
                break;
            case bt_enum:
                mname(nm, tp->btp);
                if (!(i = lookupname(nm)))
                    sprintf(buf, "%d%s", strlen(nm), nm);
                else
                    sprintf(buf, "n%c", i);
                buf = buf + strlen(buf);
                break;
            case bt_struct:
            case bt_union:
            case bt_class:
                mname(nm, tp->sp);
                if (!(i = lookupname(nm)))
                    sprintf(buf, "%d%s", strlen(nm), nm);
                else
                    sprintf(buf, "n%c", i);
                buf = buf + strlen(buf);
                break;
            case bt_bool:
                strcpy(buf, "4bool");
                buf += 5;
                break;
            case bt_unsignedshort:
                *buf++ = 'u';
            case bt_short:
                *buf++ = 's';
                buf = cppval(buf, tp);
                break;
            case bt_unsigned:
                *buf++ = 'u';
            case bt_int:
                *buf++ = 'i';
                buf = cppval(buf, tp);
                break;
            case bt_unsignedlong:
                *buf++ = 'u';
            case bt_long:
                *buf++ = 'l';
                buf = cppval(buf, tp);
                break;
            case bt_unsignedlonglong:
                *buf++ = 'u';
            case bt_longlong:
                *buf++ = 'L';
                buf = cppval(buf, tp);
                break;
            case bt_unsignedchar:
                *buf++ = 'u';
            case bt_char:
                *buf++ = 'c';
                buf = cppval(buf, tp);
                break;
            case bt_fcomplex:
                *buf++ = 'F';
                buf = cppval(buf, tp);
                break;
            case bt_rcomplex:
                *buf++ = 'R';
                buf = cppval(buf, tp);
                break;
            case bt_lrcomplex:
                *buf++ = 'G';
                buf = cppval(buf, tp);
                break;
            case bt_float:
                *buf++ = 'f';
                buf = cppval(buf, tp);
                break;
            case bt_double:
                *buf++ = 'd';
                buf = cppval(buf, tp);
                break;
            case bt_longdouble:
                *buf++ = 'g';
                buf = cppval(buf, tp);
                break;
            case bt_pointer:
                if (tp->btp->type == bt_func)
                {
                    buf = putvc(buf, tp->btp);
                    *buf++ = 'p';
                    *buf++ = 'q';
                    buf = cppargs(buf, tp->btp->lst.head);
                    *buf++ = '$';
                    tp = tp->btp->btp;
                }
                else
                {
                    *buf++ = 'p';
                    buf = cpponearg(buf, tp->btp);
                    buf = cppval(buf, tp);
                    break;
                }
                goto start;
            case bt_farpointer:
                if (tp->btp->type == bt_func)
                {
                    buf = putvc(buf, tp->btp);
                    *buf++ = 'Q';
                    buf = cppargs(buf, tp->lst.head);
                    *buf++ = '$';
                    tp = tp->btp->btp;
                }
                else
                {
                    *buf++ = 'P';
                    tp = tp->btp;
                }
                goto start;
            case bt_ref:
                *buf++ = 'r';
                tp = tp->btp;
                goto start;
            case bt_ellipse:
                *buf++ = 'e';
                break;
            case bt_void:
                *buf++ = 'v';
                break;
        }
        *buf = 0;
        return buf;
    }
    /* Mangle an entire C++ function */
    char *cppargs(char *buf, SYM *sp)
    {
        if (sp == (SYM*) - 1)
        {
            *buf++ = 'v';
        }
        else
        while (sp)
        {
            buf = cpponearg(buf, sp->tp);
            sp = sp->next;
        }
        *buf = 0;
        return buf;
    }
    /* Wrapper for function name mangling */
        char *cppmangle(char *name, TYP *tp)
        {
            char buf[100],  *p = buf;
            if (*name == 0)
                return 0;
            if (prm_cmangle &&  *name == '_')
                name++;
            if (tp)
            {
                sprintf(p, "@%s$", name);
                p = p + strlen(p);
                p = putvc(p, tp);
                *p++ = 'q';
                cppargs(p, tp->lst.head);
            }
            else
                sprintf(p, "@%s", name);
            return (litlate(buf));
        }
    static char *manglens(char *buf, NAMESPACE *ns)
    {

        if (!ns)
            return buf;
        buf = manglens(buf, ns->next);
        buf = buf + strlen(buf);
        *buf++ = '@';
        strcpy(buf, ns->sp->name + prm_cmangle);
        return buf + strlen(buf);
    }
    void mangleclass(char *buf, SYM *sl)
    {
        char *p;
        if (!sl)
            return ;
        manglens(buf, sl->value.classdata.parentns);
        if (!isstructured(sl->tp))
            return ;
        mangleclass(buf, sl->parentclass);
        p = buf + strlen(buf);
        if (sl->storage_class == sc_type)
        {
            *p++ = '@';
            strcpy(p, sl->name + (sl->name[0] == '_' && prm_cmangle));
        }
    }
    /* Wrapper for function name mangling */
    char *fullcppmangle(SYM *class , char *name, TYP *tp)
    {
            char buf[200],  *p = buf;
            *p = 0;
            if (*name == 0)
                return 0;
            if (prm_cmangle &&  *name == '_')
                name++;
            if (class )
                mangleclass(buf, class );
            else if (typequal)
                mangleclass(buf, typequal);
            else if (declclass)
                mangleclass(buf, declclass);
            p = buf + strlen(buf);
            manglenamecount = 0;
            if (templatePrefix)
            {
                    strcpy(p, templatePrefix);
                    p = p + strlen(p);
                    if (tp)
                    {
                            *p++ = '$';
                            p = putvc(p, tp);
                            *p++ = 'q';
                            cppargs(p, tp->lst.head);
                    }
            }
            else if (tp && (tp->type == bt_func || tp->type == bt_ifunc))
            {
                    sprintf(p, "@%s$", name);
                    p = p + strlen(p);
                    p = putvc(p, tp);
                    *p++ = 'q';
                    cppargs(p, tp->lst.head);
            }
            else
                sprintf(p, "@%s", name);
            manglenamecount =  - 1;
            return (litlate(buf));
    }
    static void unmangdollars(char *buf, char **nm)
    {
        int quote = FALSE;
        if (**nm == '$')
        {
            buf += strlen(buf);
            (*nm) += 2;

            *buf++ = '(';
            if ((*nm)[ - 1] == 's')
            {
                quote = TRUE;
                *buf++ = '"';
            }
            while (**nm &&  **nm != '$')
                *buf++ = *(*nm)++;
            if (quote)
                *buf++ = '"';
            *buf++ = ')';
            *buf = 0;
            if (**nm)
                (*nm)++;
        }
    }

    /* Argument unmangling for C++ */
    char *unmang1(char *buf, char *name, int firsttime)
    {
        int v;
        int cvol = 0, cconst = 0;
        char buf1[256],  *p, buf2[256];
        while (*name == 'x' ||  *name == 'y')
        {
            if (*name == 'y')
                cvol++;
            if (*name == 'x')
                cconst++;
            name++;
        }
        start: if (isdigit(*name))
        {
            v =  *name++ - '0';
            while (isdigit(*name))
                v = v * 10+ *name++ - '0';
            if (name[0] == '#')
            {
                name++;
                while (*name &&  *name != '$' &&  *name != '#')
                    *buf++ =  *name++;
                *buf = 0;
                if (*name == '$')
                {
                    name++;
                    name = unmang1(buf, name, 0);
                }
                name++;
                buf += strlen(buf);
            }
            else
            {
                char *s = buf;
                while (v--)
                if (*name == '@')
                {
                    *buf++ = ':';
                    *buf++ = ':';
                    name++;
                }
                else
                    *buf++ =  *name++;
                *buf = 0;
                if (manglenamecount < MAX_MANGLE_NAME_COUNT)
                    strcpy(manglenames[manglenamecount++], s);
            }
        }
        else
        switch (*name++)
        {
            case 'Q':
                if (!firsttime)
                {
                    strcpy(buf, " (far *) ");
                    buf += 5;
                }
                name = unmangcppfunc(buf, name, FALSE);
                break;
            case 'q':
                {
                    char *s = buf;
                    if (!firsttime)
                    {
                        strcpy(buf, " (*) ");
                        buf += 5;
                    }
                    if (*name == 'p' && *(name + 1) == 'q')
                        name++;
                    name = unmangcppfunc(buf, name, FALSE);
                    if (*(name - 1) == '$')
                    {
                        name = unmang1(buf1, name, FALSE);
                        strcpy(buf2, s);
                        sprintf(s, "%s %s", buf1, buf2);
                        buf = s + strlen(s);
                    }
                }
                break;
            case 't':
                name = unmangcpptype(buf, name, FALSE);
                break;
            case 'u':
                strcpy(buf, "unsigned ");
                buf = buf + 9;
                switch (*name++)
                {
                case 'i':
                    strcpy(buf, tn_int);
                    unmangdollars(buf, &name);
                    break;
                case 'l':
                    strcpy(buf, tn_long);
                    unmangdollars(buf, &name);
                    break;
                case 'L':
                    strcpy(buf, tn_longlong);
                    unmangdollars(buf, &name);
                    break;
                case 's':
                    strcpy(buf, tn_short);
                    unmangdollars(buf, &name);
                    break;
                case 'c':
                    strcpy(buf, tn_char);
                    unmangdollars(buf, &name);
                    break;
                }
                break;
            case 'M':
                if (*name == 'n')
                {
                    name++;
                    v =  *name++ - '0';
                    if (v > 9)
                        v -= 7;
                    strcpy(buf1, manglenames[v]);
                    p = buf1 + strlen(buf1);
                }
                else
                {
                    v =  *name++ - '0';
                    while (isdigit(*name))
                        v = v * 10+ *name++ - '0';
                    p = buf1;
                    while (v--)
                    {
                        if (*name == '@')
                        {
                            name++;
                            *p++ = ':';
                            *p++ = ':';
                        }
                        else
                            *p++ =  *name++;
                        *p = 0;
                    }
                    if (manglenamecount < MAX_MANGLE_NAME_COUNT)
                        strcpy(manglenames[manglenamecount++], buf1);
                }

                strcpy(p, "::*");
                if (*name == 'q')
                {
                    p += strlen(p);
                    *p++ = ')';
                    *p = 0;
                    name++;
                    name = unmangcppfunc(p, name, FALSE);
                    name = unmang1(buf2, name, FALSE);
                    sprintf(buf, "%s (%s", buf2, buf1);

                }
                else
                {
                    name = unmang1(buf2, name, FALSE);
                    sprintf(buf, "%s %s", buf2, buf1);
                }
                buf = buf + strlen(buf);
                break;
            case 'n':
                v =  *name++ - '0';
                if (v > 9)
                    v -= 7;
                strcpy(buf, manglenames[v]);
                break;
            case 'v':
                strcpy(buf, tn_void);
                break;
            case 'F':
                strcpy(buf, tn_fcomplex);
                unmangdollars(buf, &name);
                break;
            case 'D':
                strcpy(buf, tn_rcomplex);
                unmangdollars(buf, &name);
                break;
            case 'G':
                strcpy(buf, tn_lrcomplex);
                unmangdollars(buf, &name);
                break;
            case 'f':
                strcpy(buf, tn_float);
                unmangdollars(buf, &name);
                break;
            case 'd':
                strcpy(buf, tn_double);
                unmangdollars(buf, &name);
                break;
            case 'g':
                strcpy(buf, tn_longdouble);
                unmangdollars(buf, &name);
                break;
            case 'i':
                strcpy(buf, tn_int);
                unmangdollars(buf, &name);
                break;
            case 'l':
                strcpy(buf, tn_long);
                unmangdollars(buf, &name);
                break;
            case 'L':
                strcpy(buf, tn_longlong);
                unmangdollars(buf, &name);
                break;
            case 's':
                strcpy(buf, tn_short);
                unmangdollars(buf, &name);
                break;
            case 'c':
                strcpy(buf, tn_char);
                unmangdollars(buf, &name);
                break;
            case 'p':
                name = unmang1(buf, name, FALSE);
                buf = buf + strlen(buf);
                *buf++ = '*';
                *buf = 0;
                break;
            case 'P':
                name = unmang1(buf, name, FALSE);
                buf = buf + strlen(buf);
                strcpy(buf, "far * ");
                break;
            case 'r':
                name = unmang1(buf, name, FALSE);
                buf = buf + strlen(buf);
                *buf++ = '&';
                *buf = 0;
                break;
            case 'e':
                strcpy(buf, tn_ellipse);
                break;
            case '$':
                name--;
                return name;
        }
        if (cconst)
            strcat(buf, tn_const);
        if (cvol)
            strcat(buf, tn_vol);
        return name;
    }
    /* Unmangle an entire C++ function */
    static char *unmangcppfunc(char *buf, char *name, int firsttime)
    {
        int i;
        *buf++ = '(';
        while (*name &&  *name != '$')
        {
            name = unmang1(buf, name, firsttime);
            buf = buf + strlen(buf);
            if (*name &&  *name != '$')
            {
                *buf++ = ',';
            }
            else
            {
                *buf++ = ')';
            }
        }
        if (*name &&  *name == '$')
            name++;
        *buf = 0;
        return name;
    }
    static char *unmangcpptype(char *buf, char *name, int firsttime)
    {
        int i;
        *buf++ = '<';
        while (*name &&  *name != '$' &&  *name != '@' &&  *name != '#')
        {
            name = unmang1(buf, name, firsttime);
            buf = buf + strlen(buf);
            if (*name &&  *name != '$' &&  *name != '@' &&  *name != '#')
            {
                *buf++ = ',';
            }
            else
            {

                *buf++ = '>';
                *buf++ = ' ';
            }
        }
        if (*name &&  *name == '$')
            name++;
        *buf = 0;
        return name;
    }
    static void xlate_cppname(char **buf, char **name, char *lastname)
    {
        char classname[256],  *p = classname;
        *p = 0;
        if (**name == '@')
            (*name)++;
        if (**name == '$')
            cpp_unmang_intrins(buf, name, lastname);
        else
        {
            while (**name == '@')
                (*name)++;
            if (**name == '#')
                (*name)++;
            while (**name != '$' &&  **name)
            {
                if (**name == '@')
                {
                    *(*buf)++ = ':';
                    *(*buf)++ = ':';
                    *p = 0;
                    xlate_cppname(buf, name, classname);
                }
                else if (**name == '#')
                    (*name)++;
                else
                    *p++ = *(*buf)++ = *(*name)++;
            }
        }
        **buf = 0;
    }
/* Name unmangling in general */
void unmangle(char *buf, char *name)
{
    char classname[256];
    manglenamecount = 0;
    classname[0] = 0;
    if (name[0] == '_' && prm_cmangle)
    {
        strcpy(buf, &name[1]);
    }
        else
            if (name[0] != '@' && name[0] != '#')
                strcpy(buf, name);
            else
        {
            int done = FALSE;
            char *p = buf;
            xlate_cppname(&buf, &name, 0);
            strcpy(classname, p);
            while (!done &&  *name)
            {
                if (*name == '$')
                {
                    name++;
                    done = !(*name == 't');
                    name = unmang1(buf, name, TRUE);
                    buf += strlen(buf);
                }
                else if (*name == '@')
                {
                    name++;
                    if (*name != '$' || name[1] == 'o' || name[1] == 'b')
                    {
                        buf[0] = buf[1] = ':';
                        buf += 2;
                        p = buf;
                        xlate_cppname(&buf, &name, classname);
                        strcpy(classname, p);
                    }
                }
                else if (*name == '#')
                    name++;
                else
                {
                    done = TRUE;
                    *buf++ = 0;
                }
            }
        }
//        else
//            strcpy(buf, &name[0]);
    manglenamecount =  - 1;
}

//-------------------------------------------------------------------------

char *funcwithns(SYM *sp)
{
    char buf[512];
    buf[0] = 0;
    strcpy(manglens(buf, sp->value.classdata.parentns), sp->name);
    return litlate(buf);
}
