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
/* Handles symbol tables 
 */
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "ccerr.h"
#include "lists.h"
#include <time.h>
#include <stdlib.h>
#define ROTR(x,bits) (((x << (32 - bits)) | (x >> bits)) & 0xffff)
#define ROTL(x,bits) (((x << bits) | (x >> (32 - bits))) & 0xffff)

extern TABLE *funcparmtable;
extern int inarrayindexparsing;
extern int templateLookup;
extern char *infile;
extern TABLE oldlsym, oldltag;
extern int prm_cplusplus, prm_cmangle;
extern SYM *declclass,  *typequal,  *currentfunc;
extern struct template  *currentTemplate;
extern int global_flag;
extern char lastid[];
extern enum e_sym lastst;
extern TABLE *baseasnsymtbl;
extern int skm_declcomma[];
extern int funcnesting;
extern int structlevel;
extern SYM *parm_namespaces[20][100];
extern int parm_ns_counts[20];
extern int parm_ns_level;
extern int parsing_params;

HASHREC **defhash = 0;

NAMESPACE *thisnamespace;
TABLE *gsyms; /* this is really a hashrec ** but we use it as a table everywhere else
                 and I really need it to be the same for the MSIL compiler */
TABLE *tagtable; /* ditto */
HASHREC **templateFuncs;
TABLE lsyms, defsyms, ltags;

LIST **global_using_list,  **global_tag_using_list;
LIST *local_using_list,  *local_tag_using_list;
LIST *openns_list;
int nonslookup;

int unique_namespace_num;

static SYM *nssym1,  *nssym2;
static SYM *lastsearchsym;
static TABLE *lastsearchtable;
static char lastsearchname[256];

char searchname[256];

void symini(void)
{
    lsyms.head = lsyms.tail = defsyms.head = defsyms.tail = 0;
    ltags.head = ltags.tail = 0;
    if (!defhash)
    {
        gsyms = (HASHREC **)malloc(HASHTABLESIZE *sizeof(HASHREC*));
        templateFuncs = (HASHREC **)malloc(HASHTABLESIZE *sizeof(HASHREC*));
        defhash = (HASHREC **)malloc(HASHTABLESIZE *sizeof(HASHREC*));
        tagtable = (HASHREC **)malloc(HASHTABLESIZE *sizeof(HASHREC*));
        global_using_list = (LIST **)malloc((HASHTABLESIZE + 1) *sizeof(LIST*));
        global_tag_using_list = (LIST **)malloc((HASHTABLESIZE + 1) *sizeof
            (LIST*));
    } memset(defhash, 0, HASHTABLESIZE *sizeof(HASHREC*));
    memset(tagtable, 0, HASHTABLESIZE *sizeof(HASHREC*));
    memset(gsyms, 0, HASHTABLESIZE *sizeof(HASHREC*));
    memset(templateFuncs, 0, HASHTABLESIZE *sizeof(HASHREC*));
    memset(global_using_list, 0, (HASHTABLESIZE + 1) *sizeof(LIST*));
    memset(global_tag_using_list, 0, (HASHTABLESIZE + 1) *sizeof(LIST*));
    thisnamespace = 0;
    local_using_list = 0;
    local_tag_using_list = 0;
    srand(time(0));
    unique_namespace_num = rand() % 2000;
    nonslookup = FALSE;
    lastsearchname[0] = 0;
    lastsearchtable = 0;
}

/* Sym tab hash function */
static unsigned int ComputeHash(char *string, int size)
{
    unsigned i;
    char *s = string;
    for (i = 0;  *string; string++)
        i = i * 128+i * 2+i +  *string;
    //  printf("%d: %u %d: %s\n",i %size,i,size,s) ;
    return (i % size);

    #ifdef XXXXX
        unsigned int len = strlen(string), rv;
        char *pe = len + string;
        unsigned char blank = ' ';

        rv = len | blank;
        while (len--)
        {
            unsigned char cback = (unsigned char)(*--pe) | blank;
            rv = ROTL(rv, 2) ^ cback;
        }
        return (rv % size);
    #endif 
}

/* Add a hash item to the table */
HASHREC *AddHash(HASHREC *item, HASHREC **table, int size)
{
    int index = ComputeHash(item->key, size);
    HASHREC **p;
    item->link = 0;

    if (*(p = &table[index]))
    {
        HASHREC *q =  *p,  *r =  *p;
        while (q)
        {
            r = q;
            if (!strcmp(r->key, item->key))
                return (r);
            q = q->link;
        }
        r->link = item;
    }
    else
        *p = item;
    return (0);
}

/*
 * Find something in the hash table
 */
HASHREC **LookupHash(char *key, HASHREC **table, int size)
{
    int index = ComputeHash(key, size);
    HASHREC **p;

    if (*(p = &table[index]))
    {
        HASHREC *q =  *p;
        while (q)
        {
            if (!strcmp(q->key, key))
                return (p);
            p =  *p;
            q = q->link;
        }
    }
    return (0);
}

//-------------------------------------------------------------------------

void pushusing(SYM *sp, int tag)
{
    LIST **l,  *m = xalloc(sizeof(LIST)),  *li;
    if (openns_list)
        if (tag)
            l = ((SYM*)openns_list->data)->value.classdata.parentns
                ->tag_using_list;
        else
            l = ((SYM*)openns_list->data)->value.classdata.parentns->using_list;
        else if (!funcnesting && !structlevel)
            if (tag)
                l = global_tag_using_list;
            else
                l = global_using_list;
            else
                if (tag)
                    l = &local_tag_using_list;
                else
                    l = &local_using_list;

    if (l !=  &local_using_list && l !=  &local_tag_using_list)
    {
        if (sp->storage_class == sc_namespace)
            l = &l[HASHTABLESIZE];
        else
        {
            int index = ComputeHash(sp->name, HASHTABLESIZE);
            l = &l[index];
        }
    }
    li =  *l;
    while (li)
    {
        if (li->data == sp)
            break;
        li = li->link;
    }
    if (!li)
    {
        m->data = sp;
        m->link =  *l;
        *l = m;
    }
}

//-------------------------------------------------------------------------

void aliasnamespace(char *name)
{
    char buf[256];
    strcpy(buf, name);
    getsym();
    if (lastst == id)
    {
        SYM *s;
        SYM *typesp;
        if ((typesp = typesearch(lastid)) && typesp->storage_class ==
            sc_namespace)
        {
            getsym();
            typesp = parsetype(typesp, TRUE);
        }
        if (typesp->storage_class == sc_namespace)
        {
            s = makesym(sc_namespace);
            s->name = litlate(buf);
            s->tp = maketype(bt_int, 4);
            s->value.classdata.parentns = typesp->value.classdata.parentns;
            insert(s, gsyms);
        }
        else
            generror(ERR_NAMESPACEEXP, 0, 0);
    }
    else
        generror(ERR_IDEXPECT, 0, 0);
}

//-------------------------------------------------------------------------

void switchtonamespace(char *name)
{
    NAMESPACE *q;
    char buf[256];
    int anon = 0;
    SYM *s;
    if (name == 0)
    {
        int i = 0;
        name = infile;
        buf[i++] = '_';
        buf[i++] = 'n';
        buf[i++] = 's';
        while (*name)
        {
            if (isalnum(name))
                buf[i++] =  *name++;
            else
                buf[i++] = '_', name++;
            sprintf(buf + i, "%d%d_", unique_namespace_num++, rand() % 1000);
        }
        anon = 1;
    }
    else
        strcpy(buf, name);
    if (!openns_list)
        s = basesearch(buf, gsyms, 0);
    else
        s = namespace_search(buf, ((SYM*)openns_list->data)
            ->value.classdata.parentns, 0);
    if (!s)
    {
        NAMESPACE *ns;
        global_flag++;
        s = makesym(sc_namespace);
        s->name = litlate(buf);
        s->tp = maketype(bt_int, 4);
        ns = s->value.classdata.parentns = xalloc(sizeof(NAMESPACE));
        ns->table = (HASHREC **)xalloc(HASHTABLESIZE *sizeof(HASHREC*));
        ns->tagtable = (HASHREC **)xalloc(HASHTABLESIZE *sizeof(HASHREC*));
        ns->using_list = (LIST **)xalloc((HASHTABLESIZE + 1) *sizeof(LIST*));
        ns->tag_using_list = (LIST **)xalloc((HASHTABLESIZE + 1) *sizeof(LIST*))
            ;
        ns->sp = s;
        ns->anonymous = anon;
        ns->guardian = FALSE;
        ns->next = thisnamespace;
        insert(s, gsyms);
        global_flag--;
    }
    else if (s->storage_class != sc_namespace)
    {
        generror(ERR_NAMESPACEEXP, 0, 0);
        s = 0;
    }
    if (s)
    {
        LIST *l = xalloc(sizeof(LIST));
        l->link = openns_list;
        l->data = s;
        thisnamespace = s->value.classdata.parentns;
        if (anon)
        {
            pushusing(s, 0);
            pushusing(s, 1);
        }
        openns_list = l;
    }
}

//-------------------------------------------------------------------------

void switchfromnamespace(void)
{
    if (openns_list)
    {
        thisnamespace = openns_list = openns_list->link;
    }
    else
        thisnamespace = 0;
    // safety

}

//-------------------------------------------------------------------------

void inject_namespace(int ns)
{
    SYM *sp = search(lastid, gsyms),  *sp1;
    if (!sp)
    {
        gensymerror(ERR_UNDEFINED, lastid);
        return ;
    }
    getsym();
    if (sp->storage_class == sc_namespace && lastst == classsel)
    {
        do
        {
            getsym();
            if (lastst != id)
            {
                generror(ERR_IDEXPECT, 0, 0);
                return ;
            }
            sp1 = namespace_search(lastid, sp->value.classdata.parentns, 0);
            if (!sp1)
                sp1 = namespace_search(lastid, sp->value.classdata.parentns, 1);
            if (!sp1)
            {
                gensymerror(ERR_UNDEFINED, lastid);
                return ;
            }
            sp = sp1;
            getsym();
        }
        while (sp->storage_class == sc_namespace && lastst == classsel);
    }
    if (ns)
    {
        if (sp->storage_class != sc_namespace)
            generror(ERR_NAMESPACEEXP, 0, 0);
        pushusing(sp, 0);
        pushusing(sp, 1);
    }
    else
    {
        if (sp->storage_class == sc_namespace)
            gensymerror(ERR_ILLEGALNAMESPACEUSE, lastid);
        pushusing(sp, sp->intagtable);
    }
}

//-------------------------------------------------------------------------

SYM *namespace_search(char *name, NAMESPACE *data, int tags)
{
    SYM **p;
    if (tags)
        p = ((SYM **)LookupHash(name, data->tagtable, HASHTABLESIZE));
    else
        p = ((SYM **)LookupHash(name, data->table, HASHTABLESIZE));
    if (p)
    {
        p =  *p;
    }
    return (SYM*)p;
}

//-------------------------------------------------------------------------

static SYM *namespace_search_full(char *name, LIST **l, TABLE *table)
{
    LIST *list,  *list2 = 0;
    if (l !=  &local_using_list && l !=  &local_tag_using_list)
    {
        int index = ComputeHash(name, HASHTABLESIZE);
        list2 = l[HASHTABLESIZE];
        l = &l[index];
    }
    list =  *l;
    if (!list)
    {
        list = list2;
        list2 = 0;
    }
    while (list)
    {
        SYM *sp = list->data;
        SYM *main = 0,  *temp = 0;
        if (sp->storage_class == sc_namespace)
        {
            if (!sp->value.classdata.parentns->guardian)
            {
                sp->value.classdata.parentns->guardian++;
                main = namespace_search(name, sp->value.classdata.parentns,
                    table == tagtable);
                temp = namespace_search_full(name, table == tagtable ? sp
                    ->value.classdata.parentns->tag_using_list: sp
                    ->value.classdata.parentns->using_list, table);
                sp->value.classdata.parentns->guardian--;
            }
        }
        else if (!strcmp(name, sp->name))
            main = sp;
        if (main)
        {
            if (nssym1)
            {
                if (!nssym2 &nssym1 != main)
                {
                    nssym2 = main;
                    return nssym1;
                }
            }
            else
                nssym1 = main;
        }
        if (temp)
        {
            if (nssym1)
            {
                if (!nssym2 && nssym1 != temp)
                {
                    nssym2 = temp;
                    return nssym1;
                }
            }
            else
                nssym1 = temp;
        }
        list = list->link;
        if (!list)
        {
            list = list2;
            list2 = 0;
        }
    }
    return 0;
}

//-------------------------------------------------------------------------

int isaccessible(SYM *sp)
{
        //   if (!currentfunc)
        //      return TRUE ;
        //   if (!isstructured(sp->tp))
        //      return TRUE ;
        if (declclass && sp->mainsym == declclass->mainsym)
            return TRUE;
        if ((sp->value.classdata.cppflags &PF_MEMBER) && sp->parentclass)
        {
            if (sp->value.classdata.cppflags &PF_UNREACHABLE)
                return FALSE;
            if (sp->parentclass != declclass)
            {
                if (sp->parentclass && declclass && sp->parentclass->mainsym ==
                    declclass->mainsym)
                    return TRUE;
                if (declclass)
                {
                    if (!(sp->value.classdata.cppflags &(PF_PUBLIC |
                        PF_PROTECTED)))
                        return FALSE;
                }
                else if (!(sp->value.classdata.cppflags &PF_PUBLIC))
                    return FALSE;
            }
        }
    return TRUE;
}

/*
 * Some tables use hash tables and some use linked lists
 * This is the global symbol search routine
 */
SYM *basesearch(char *na, TABLE *table, int checkaccess)
{
    SYM *thead = table->head;
    SYM **p,  *sp;
    if (table == gsyms)
    {
        p = ((SYM **)LookupHash(na, table, HASHTABLESIZE));
        if (p)
        {
            p =  *p;
            //         if (checkaccess)
            //            if (!isaccessible(p) && !isfriend(p,table)) 
            //               genclasserror(ERR_NOTACCESSIBLE,sp->name);
        }
        return (SYM*)p;
    }
    else if (table == tagtable)
    {
        if (funcnesting)
        {
            SYM *rv = basesearch(na, &ltags, checkaccess);
            if (rv)
                return rv;
        }
        p = ((SYM **)LookupHash(na, table, HASHTABLESIZE));
        if (p)
        {
            p =  *p;
            //         if (checkaccess)
            //            if (!isaccessible(p) && !isfriend(p,table)) 
            //               genclasserror(ERR_NOTACCESSIBLE,sp->name);
        }
        return (SYM*)p;
    }
    else if (table ==  &defsyms)
    {
        p = ((SYM **)LookupHash(na, defhash, HASHTABLESIZE));
        if (p)
        {
            p =  *p;
            //         if (checkaccess)
            //            if (!isaccessible(p) && !isfriend(p,table)) 
            //               genclasserror(ERR_NOTACCESSIBLE,sp->name);
        }
        return (SYM*)p;
    }
    else if (table == templateFuncs)
    {
        p = ((SYM **)LookupHash(na, templateFuncs, HASHTABLESIZE));
        if (p)
            p =  *p;
        return (SYM*)p;
    }
    else
    {
        finish: 
        while (thead != 0)
        {
            if (strcmp(thead->name, na) == 0)
            {
                if (checkaccess)
                    if (!isaccessible(thead) && (!declclass || !isfriend(thead,
                        &declclass->tp->lst.head)) && !isfriend(thead, table))
                        genclasserror(ERR_NOTACCESSIBLE, fullcppmangle(thead
                            ->tp->sp->parentclass, thead->name, 0));
                return thead;
            }
            thead = thead->next;
        }
    }
    return 0;
}

//-------------------------------------------------------------------------

SYM *search2(char *na, TABLE *table, int checkaccess, int definition)
{

    SYM *sp1 = 0,  *sp2 = 0,  *sp3 = 0;
    nssym1 = nssym2 = 0;
    if (typequal)
        return tcsearch(na, checkaccess);
    if (!definition || !openns_list || table != gsyms && table != tagtable)
        sp1 = basesearch(na, table, checkaccess);
    if (sp1)
        nssym1 = sp1;
    if (!sp1 && openns_list && (table == gsyms || table == tagtable))
    {
        sp1 = openns_list->data;
        sp1 = namespace_search(na, sp1->value.classdata.parentns, table ==
            tagtable);
        if (sp1)
            return sp1;
        if (definition)
            return 0;
    }
    if (!nonslookup)
    if (table == gsyms || table == tagtable)
    {
        if (table == tagtable && (funcnesting || structlevel))
            namespace_search_full(na, &local_tag_using_list, table);
        namespace_search_full(na, table == tagtable ? global_tag_using_list :
            global_using_list, table);
        if (openns_list && openns_list->link) {
            SYM *sp = openns_list->link->data;
            namespace_search_full(na, table == tagtable ? sp
                    ->value.classdata.parentns->tag_using_list: sp
                    ->value.classdata.parentns->using_list,table);
        }
    }
    else if (table ==  &lsyms)
        namespace_search_full(na, &local_using_list, table);
    nonslookup = FALSE;
    if (nssym1 && nssym2)
        genfunc2error(ERR_AMBIGFUNC, fullcppmangle(nssym1, nssym1->name, nssym1
            ->tp), fullcppmangle(nssym2, nssym2->name, nssym2->tp));
    if (nssym1)
        return nssym1;
    else
        return nssym2;
}

//-------------------------------------------------------------------------

SYM *search(char *na, TABLE *table)
{
    SYM *sp;
    int lastl;
    if (table == lastsearchtable && lastsearchsym)
    {
        if (!strcmp(na, lastsearchname))
            return lastsearchsym;
    }
    searchname[0] = 0;
    sp = search2(na, table, TRUE, FALSE);
    if (table == gsyms || table == tagtable)
    {
        lastsearchtable = table;
        strcpy(lastsearchname, na);
        lastsearchsym = sp;
    }
    return sp;
}

//-------------------------------------------------------------------------

SYM *losearch(char *na)
{
    SYM *sp = 0;
    if (templateLookup)
        sp = search(na, &currentTemplate->classes);
    if (!sp && baseasnsymtbl)
        sp = search(na, baseasnsymtbl);
    if (!sp && inarrayindexparsing && funcparmtable)
        sp = search(na, funcparmtable);
    if (!sp)
        sp = search(na, &lsyms);
    if (!sp && declclass)
        sp = search(na, &declclass->tp->lst);
    return sp;
}

//-------------------------------------------------------------------------

SYM *tcsearch(char *na, int checkaccess)
{
    SYM *rv;
    if (!typequal)
        return 0;
    if (typequal->storage_class == sc_type)
        return basesearch(na, &typequal->tp->lst, checkaccess);
    else if (typequal->storage_class == sc_namespace)
    {
        if (parsing_params)
        {
            parm_namespaces[parm_ns_level][parm_ns_counts[parm_ns_level]++] =
                typequal;
        }
        if (rv = namespace_search(na, typequal->value.classdata.parentns, 0))
            return rv;
        else
            return namespace_search(na, typequal->value.classdata.parentns, 1);
    }
    return 0;
}

//-------------------------------------------------------------------------

SYM *gsearch(char *na)
{
    SYM *sp;
    if ((sp = losearch(na)) == 0)
        sp = search(na, gsyms);
    return sp;
}

//-------------------------------------------------------------------------

SYM *typesearch(char *na)
{
    SYM *sp;
    sp = gsearch(na);
    if (sp || !prm_cplusplus)
    {
        return sp;
    }
    return search(na, tagtable);
}

/* The global symbol insert routine */
void insert(SYM *sp, TABLE *table)

{
    SYM *sp1;
    lastsearchname[0] = 0;
    lastsearchtable = 0;
    lastsearchsym = 0;
    if (table == NULL)
        return ;
    // Assume some error led to this
    if (table == gsyms || table == tagtable)
    {
        if (table == tagtable && !global_flag)
        {
            SYM *thead = ltags.head,  *qhead = 0,  **oldhead = &ltags.head;
            table = &ltags;
            /* Only check the current local block... */
            while (thead != oldltag.head)
            {
                if (strcmp(thead->name, sp->name) == 0)
                {
                    qhead = thead;
                    break;
                }
                oldhead = &thead->next;
                thead = thead->next;
            }
            if (qhead && !(qhead->value.classdata.cppflags &PF_INHERITED))
                gensymerror(ERR_DUPSYM, sp->name);
            else
            {
                if (qhead)
                    *oldhead = qhead->next;
                /* Putting local symbols in backwards */
                if (table->head == 0)
                {
                    table->head = table->tail = sp;
                    sp->next = 0;
                }
                else
                {
                    sp->next = table->head;
                    table->head = sp;
                }
            }
        }
        else if (openns_list)
        {
            NAMESPACE *s = ((SYM*)openns_list->data)->value.classdata.parentns;
            if (AddHash(sp, table == gsyms ? s->table: s->tagtable,
                HASHTABLESIZE))
                gensymerror(ERR_DUPSYM, fullcppmangle(sp, sp->name, sp->tp));
        }
        else if (AddHash(sp, table, HASHTABLESIZE))
            gensymerror(ERR_DUPSYM, sp->name);
    }
    else if (table ==  &defsyms)
    {
        AddHash(sp, defhash, HASHTABLESIZE);
    }
    else
    {
        if (table ==  &lsyms)
        {
            SYM *thead = table->head,  *qhead = 0,  **oldhead = &table->head;
            /* Only check the current local block... */
            while (thead != oldlsym.head)
            {
                if (strcmp(thead->name, sp->name) == 0)
                {
                    qhead = thead;
                    break;
                }
                oldhead = &thead->next;
                thead = thead->next;
            }
            if (qhead && !(qhead->value.classdata.cppflags &PF_INHERITED))
                gensymerror(ERR_DUPSYM, sp->name);
            else
            {
                if (qhead)
                    *oldhead = qhead->next;
                /* Putting local symbols in backwards */
                if (table->head == 0)
                {
                    table->head = table->tail = sp;
                    sp->next = 0;
                }
                else
                {
                    sp->next = table->head;
                    table->head = sp;
                }
            }
        }
        else if (table == templateFuncs)
        {
            if (AddHash(sp, templateFuncs, HASHTABLESIZE))
                gensymerror(ERR_DUPSYM, sp->name);
        }
        else if ((sp1 = search2(sp->name, table, FALSE, FALSE)) == 0 || (sp1
            ->value.classdata.cppflags &PF_INHERITED))
        {
            if (table->head == 0)
                table->head = table->tail = sp;
            else
            {
                table->tail->next = sp;
                table->tail = sp;
            }
            sp->next = 0;
            if (sp1)
            {
                SYM *sp2 = table->head,  **spo = &table->head;
                while (sp2)
                {
                    if (sp2 == sp1)
                    {
                        *spo = sp2->next;
                        break;
                    }
                    spo = &sp2->next;
                    sp2 = sp2->next;
                }
            }
        }
        else
            gensymerror(ERR_DUPSYM, sp->name);
    }
}

//-------------------------------------------------------------------------

int unlinkTag(SYM *sp)

{
    int rv = FALSE;
    LIST **ul = 0;
    SYM *sp1;
    if (!global_flag)
    {
        SYM *thead = ltags.head,  *qhead = 0,  **oldhead = &ltags.head;
        /* Only check the current local block... */
        while (thead != oldltag.head)
        {
            if (strcmp(thead->name, sp->name) == 0)
            {
                qhead = thead;
                break;
            }
            oldhead = &thead->next;
            thead = thead->next;
        }
        if (qhead)
        {
            *oldhead = qhead->next;
            rv = TRUE;
        }
        else
        {
            ul = &local_using_list;
        }
    }
    else if (openns_list)
    {
        NAMESPACE *s = ((SYM*)openns_list->data)->value.classdata.parentns;
        SYM **p = LookupHash(sp->name, s->tagtable, HASHTABLESIZE);
        if (p)
        {
            *p = (*p)->next;
            rv = TRUE;
        }
        else
        {
            ul = &s->tag_using_list;
        }
    }
    else
    {
        SYM **p = LookupHash(sp->name, tagtable, HASHTABLESIZE);
        if (p)
        {
            *p = (*p)->next;
            rv = TRUE;
        }
        else
        {
            ul = &global_tag_using_list;
        }
    }
    if (!rv && ul)
    {
        if (ul !=  &local_tag_using_list)
            ul = LookupHash(sp->name,  *ul, HASHTABLESIZE);
        if (ul)
        {
            while (*ul)
            {
                if (((SYM*)(*ul)->data)->mainsym == sp->mainsym)
                {
                    *ul = (*ul)->link;
                    rv = TRUE;
                    break;
                }
                ul = &(*ul)->link;
            }
        }
    }
    return rv;
}
