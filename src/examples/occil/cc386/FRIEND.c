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

extern SYM *currentfunc,  *declclass;

void insertfriend(SYM *decl, SYM *tfriend)
{
        char buf[100];
        SYM *sp,  *sp1;
        sp1 = search("$$FRIENDS$$", &decl->tp->lst);
        if (sp1)
        {
            sp1->tp->lst.tail = sp1->tp->lst.tail->next = tfriend;
            //      if (!search(tfriend->name,&sp1->tp->lst))
            //         insert(tfriend,&sp1->tp->lst) ;
        }
        else
        {
            sp = makesym(sc_friendlist);
            sp->name = "$$FRIENDS$$";
            sp->tp = maketype(bt_defunc, 0);
            sp->tp->sp = sp;
            sp->tp->lst.head = sp->tp->lst.tail = tfriend;
            sp->parentclass = declclass;
            insert(sp, &decl->tp->lst);
        }
}

//-------------------------------------------------------------------------

int friendsearch(SYM *spx, char *name, SYM *sp1)
{
    SYM *sp2;
    sp1 = sp1->tp->lst.head;
    while (sp1)
    {
        if (!strcmp(sp1->name, name))
        {
            if (!strcmp(sp1->parentclass->name, spx->originalparentclass->name))
                return 1;
        }
        sp1 = sp1->next;
    }
    return 0;
}

//-------------------------------------------------------------------------

int isfriend(SYM *spx, TABLE *tbl)
{
    SYM *sp1 = search("$$FRIENDS$$", tbl);
    if (sp1)
    {
        if (currentfunc && friendsearch(spx, currentfunc->name, sp1))
            return 1;
        if (declclass && friendsearch(spx, declclass->name, sp1))
            return 1;
    }
    return 0;
}
