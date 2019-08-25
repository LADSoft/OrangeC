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
#include "ccerr.h"
#include "c.h"

extern int prm_cplusplus, prm_farkeyword;
extern char *tn_void;
extern char *tn_char;
extern char *tn_int;
extern char *tn_long;
extern char *tn_longlong;
extern char *tn_short;
extern char *tn_unsigned;
extern char *tn_ellipse;
extern char *tn_fcomplex;
extern char *tn_rcomplex;
extern char *tn_lrcomplex;
extern char *tn_float;
extern char *tn_double;
extern char *tn_longdouble;
extern char *tn_const;
extern char *tn_vol;
extern char *tn_class;
extern char *tn_union;
extern char *tn_struct;
extern int prm_cmangle;

int exactype(TYP *typ1, TYP *typ2, int asn)
{
    SYM *s1,  *s2;
    while (typ1 && typ2)
    {
        typ1 = basictype(typ1);
        typ2 = basictype(typ2);
        if (typ1->type == bt_ref)
            typ1 = typ1->btp;
        if (typ2->type == bt_ref)
            typ2 = typ2->btp;
        if (asn & 1) {
            if ((typ2->cflags & DF_CONST) && !(typ1->cflags & DF_CONST))
                return 0;
        }
		else if (asn & 2)
		{
            if ((typ2->cflags  ^typ1->cflags) & DF_CONST)
                return 0;
		}
        if (typ1->type != typ2->type)
        {
            if ((typ1->type == bt_pointer && (typ2->type == bt_farpointer ||
                typ2->type == bt_segpointer) || (typ1->type == bt_farpointer &&
                (typ2->type == bt_pointer || typ2->type == bt_segpointer)) || 
                (typ1->type == bt_segpointer && (typ2->type == bt_pointer ||
                typ2->type == bt_farpointer))))
            {
                typ1 = typ1->btp;
                typ2 = typ2->btp;
                continue;
            }
			else if (typ1->type == bt_func && typ2->type == bt_ifunc ||
					 typ1->type == bt_ifunc && typ2->type == bt_func)
				goto funcjn;
			return 0;
        }
        else
        switch (typ1->type)
        {
            case bt_void:
                if (typ2->type != bt_void)
                    return 0;
				return 1;
            case bt_ifunc:
            case bt_func:
            case bt_defunc:
funcjn:
                if (!exactype(typ1->btp, typ2->btp, asn))
                    return 0;
				if (!typ1->sp->tp->lst.head || !typ2->sp->tp->lst.head)
					return 1;
                s1 = typ1->lst.head;
				if (!s1)
					return 1;
                s2 = typ2->lst.head;
                while (s1 && s2 && (s1 != (SYM*) - 1) && (s2 != (SYM*) - 1))
                {
                    if (!exactype(s1->tp, s2->tp, asn))
                        return 0;
                    s1 = s1->next;
                    s2 = s2->next;
                }
                return s1 == s2 || (s1 == (SYM *) -1 && s2 == NULL)
						||(s2 == (SYM *)-1 && s1 == NULL);
            case bt_struct:
            case bt_union:
            case bt_class:
                if (!typ1->sp || !typ2->sp)
                    return 0;
                return (!strcmp(typ1->sp->name, typ2->sp->name));
            case bt_pointer:
            case bt_farpointer:
            case bt_segpointer:
				if (!prm_cplusplus)
				{
					TYP *typ3 = typ1;
					TYP *typ4 = typ2;
					do {
						typ1 = typ1->btp;
						typ2 = typ2->btp;
					    if (asn & 1) {
					        if ((typ2->cflags & DF_CONST) && !(typ1->cflags & DF_CONST))
					            return 0;
					    }
						else if (asn & 2)
						{
					        if ((typ2->cflags  ^typ1->cflags) & DF_CONST)
					            return 0;
						}
					} while ((typ1->type == bt_pointer || typ1->type == bt_farpointer ||
							 typ1->type == bt_segpointer) &&
							(typ2->type == bt_pointer || typ2->type == bt_farpointer ||
							 typ2->type == bt_segpointer)) ;
					return 1;
				}
                break;
        }
        typ1 = typ1->btp;
        typ2 = typ2->btp;
    }
    return !(typ1 || typ2);
}

//-------------------------------------------------------------------------

int checktype(TYP *typ1, TYP *typ2, int scalarok)
{
        if (prm_cplusplus)
            return exactype(typ1, typ2,FALSE);
    typ1 = basictype(typ1);
    typ2 = basictype(typ2);
    if ((typ1->type == bt_pointer && (typ2->type == bt_farpointer || typ2->type
        == bt_segpointer) || (typ1->type == bt_farpointer && (typ2->type ==
        bt_pointer || typ2->type == bt_segpointer)) || (typ1->type ==
        bt_segpointer && (typ2->type == bt_pointer || typ2->type ==
        bt_farpointer))))
        return TRUE;
    if (typ1->type == typ2->type || isscalar(typ2) && isscalar(typ1))
        return TRUE;
    else
        if (((typ2->type == bt_segpointer || typ2->type == bt_pointer || typ2
            ->type == bt_farpointer) && (typ1->type == bt_ifunc || typ1->type 
            == bt_func || scalarok && isscalar(typ1))) || ((typ1->type ==
            bt_segpointer || typ1->type == bt_pointer || typ1->type ==
            bt_farpointer) && (typ2->type == bt_ifunc || typ2->type == bt_func 
            || scalarok && isscalar(typ2))))
            return (TRUE);
        else
    if (typ1->type == bt_enum)
    {
        switch (typ2->type)
        {
            case bt_long:
            case bt_unsigned:
            case bt_unsignedlonglong:
            case bt_longlong:
            case bt_unsignedlong:
            case bt_int:
            case bt_short:
            case bt_unsignedshort:
            case bt_char:
            case bt_unsignedchar:
                return (TRUE);
        }
    }
    else
    if (typ2->type == bt_enum)
    {
        switch (typ1->type)
        {
            case bt_long:
            case bt_unsigned:
            case bt_unsignedlonglong:
            case bt_longlong:
            case bt_unsignedlong:
            case bt_int:
            case bt_short:
            case bt_unsignedshort:
            case bt_char:
            case bt_unsignedchar:
                return (TRUE);
        }
    }
    else
    /* this is so we can put an ampersand in front of a func name we are using */
        if ((typ1->type == bt_pointer && typ2->type == bt_pointer && typ1->btp
            ->type == bt_func) || (typ2->type == bt_pointer && typ1->type ==
            bt_pointer && typ2->btp->type == bt_func))
            return TRUE;
    if ((typ1->type == bt_farpointer && typ2->type == bt_farpointer && typ1
        ->btp->type == bt_func) || (typ2->type == bt_farpointer && typ1->type 
        == bt_farpointer && typ2->btp->type == bt_func))
        return TRUE;
    return FALSE;
}

//-------------------------------------------------------------------------

int checktypeassign(TYP *typ1, TYP *typ2)
{
        if (prm_cplusplus)
            return exactype(typ1, typ2, TRUE);
    /* this is so we can put an ampersand in front of a func name we are using */
    if ((typ1->type == bt_pointer && typ2->type == bt_pointer && typ1->btp
        ->type == bt_func) || (typ2->type == bt_pointer && typ1->type ==
        bt_pointer && typ2->btp->type == bt_func))
        return TRUE;
    if ((typ1->type == bt_farpointer && typ2->type == bt_farpointer && typ1
        ->btp->type == bt_func) || (typ2->type == bt_farpointer && typ1->type 
        == bt_farpointer && typ2->btp->type == bt_func))
        return TRUE;
    while (typ1 && typ2 && (typ1->type == typ2->type || typ1->type == bt_farpointer && typ2->type == bt_pointer))
    {
        if (isscalar(typ1))
            return TRUE;
		if (isstructured(typ1) && !strcmp(typ1->sp->name, typ2->sp->name))
			return TRUE;
        typ1 = typ1->btp;
        typ2 = typ2->btp;
    }
    if (!typ1 && !typ2)
        return (TRUE);
    return (FALSE);
}

//-------------------------------------------------------------------------

    static void unm2name(char *buf, SYM *sp)
    {
        char *v;
        if (!sp)
            return ;
        if (sp->value.classdata.parentns)
        {
            unm2name(buf, sp->value.classdata.parentns);
            strcat(buf, "::");
            buf += strlen(buf);
        }
        else if (sp->parentclass)
        {
            unm2name(buf, sp->parentclass);
            strcat(buf, "::");
            buf = buf + strlen(buf);
        }
        v = sp->name;
        if (prm_cmangle)
            v++;
        strcpy(buf, v);
    }
    TYP *typenum2(char *buf, TYP *tp)
    {
        SYM *sp;
        char name[100];
        switch (tp->type)
        {
            case bt_func:
                typenum2(buf, tp->btp);
                buf = buf + strlen(buf);
                *buf++ = '(';
                sp = tp->lst.head;
                while (sp && sp != (SYM*) - 1)
                {
                    *buf = 0;
                    typenum2(buf, sp->tp);
                    buf = buf + strlen(buf);
                    sp = sp->next;
                    if (sp)
                        *buf++ = ',';
                }
                *buf++ = ')';
                *buf++ = 0;
                break;
            case bt_fcomplex:
                strcpy(buf, tn_fcomplex);
                break;
            case bt_rcomplex:
                strcpy(buf, tn_rcomplex);
                break;
            case bt_lrcomplex:
                strcpy(buf, tn_lrcomplex);
                break;
            case bt_float:
                strcpy(buf, tn_float);
                break;
            case bt_double:
                strcpy(buf, tn_double);
                break;
            case bt_longdouble:
                strcpy(buf, tn_longdouble);
                break;
            case bt_unsigned:
                strcpy(buf, tn_unsigned);
                buf = buf + strlen(buf);
            case bt_int:
                strcpy(buf, tn_int);
                break;
            case bt_unsignedlonglong:
                strcpy(buf, tn_unsigned);
                buf = buf + strlen(buf);
            case bt_longlong:
                strcpy(buf, tn_longlong);
                break;
            case bt_unsignedlong:
                strcpy(buf, tn_unsigned);
                buf = buf + strlen(buf);
            case bt_long:
                strcpy(buf, tn_long);
                break;
            case bt_unsignedshort:
                strcpy(buf, tn_unsigned);
                buf = buf + strlen(buf);
            case bt_short:
                strcpy(buf, tn_short);
                break;
            case bt_unsignedchar:
                strcpy(buf, tn_unsigned);
                buf = buf + strlen(buf);
            case bt_char:
                strcpy(buf, tn_char);
                break;
            case bt_bool:
                strcpy(buf, "bool");
                break;
            case bt_void:
                strcpy(buf, tn_void);
                break;
            case bt_pointer:
                typenum2(buf, tp->btp);
                buf += strlen(buf);
                *buf++ = ' ';
                *buf++ = '*';
                *buf++ = 0;
                break;
            case bt_farpointer:
                typenum2(buf, tp->btp);
                buf += strlen(buf);
                strcpy(buf, " far *");
                break;
            case bt_segpointer:
                typenum2(buf, tp->btp);
                buf += strlen(buf);
                strcpy(buf, " _seg *");
                break;
            case bt_ref:
                typenum2(buf, tp->btp);
                buf += strlen(buf);
                *buf++ = ' ';
                *buf++ = '*';
                *buf++ = 0;
                break;
            case bt_ellipse:
                strcpy(buf, tn_ellipse);
                break;
            case bt_memberptr:
                typenum2(buf, tp->btp);
                strcat(buf, " ");
                buf = buf + strlen(buf);
                unm2name(buf, tp->sp);
                buf = buf + strlen(buf);
                strcpy(buf, "::*");
                break;
            case bt_matchall:
                strcpy(buf, "???");
                break;
            case bt_class:
                strcpy(buf, tn_class);
                unmangle(name, tp->sp->name);
                strcat(buf, name);
                break;
            case bt_struct:
                strcpy(buf, tn_struct);
                unmangle(name, tp->sp->name);
                strcat(buf, name);
                break;
            case bt_union:
                strcpy(buf, tn_union);
                unmangle(name, tp->sp->name);
                strcat(buf, name);
                break;
        }
        if (tp->cflags &DF_CONST)
            strcat(buf, tn_const);
        if (tp->cflags &DF_VOL)
            strcat(buf, tn_vol);
        return 0;
    }
    char *typenum(char *buf, TYP *typ)
    {
        *buf++ = '(';
        *buf = 0;
        while (typ)
        {
            typ = typenum2(buf, typ);
            buf = buf + strlen(buf);
            if (typ)
                *buf++ = ',';
            else
                *buf++ = ')';
        }
        *buf = 0;
        return buf;
    }
