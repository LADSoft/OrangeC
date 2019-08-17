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
 * Listing file
 */
#include <stdio.h>
#include <string.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
extern int prm_listfile;
extern HASHREC **globalhash;
extern FILE *listFile;
extern TABLE *gsyms,  *tagtable;
extern char *registers[];

/* Unnamed structure tags */
char *tn_unnamed = "<no name> ";


static char *unmangledname(char *str)
{
    static char name[256];
    unmangle(name, str);
    return name;
}

/* Put the storage class */
void put_sc(int scl)
{
    if (!prm_listfile)
        return ;
    switch (scl)
    {
        case sc_static:
            fprintf(listFile, "Static      ");
            break;
        case sc_auto:
            fprintf(listFile, "Auto        ");
            break;
        case sc_autoreg:
        case sc_memberreg:
            fprintf(listFile, "Register    ");
            break;
        case sc_global:
            fprintf(listFile, "Global      ");
            break;
        case sc_abs:
            fprintf(listFile, "Absolute    ");
            break;
        case sc_external:
        case sc_externalfunc:
            fprintf(listFile, "External    ");
            break;
        case sc_type:
            fprintf(listFile, "Type        ");
            break;
        case sc_const:
            fprintf(listFile, "Constant    ");
            break;
        case sc_member:
            fprintf(listFile, "Member      ");
            break;
        case sc_label:
            fprintf(listFile, "Label       ");
            break;
        case sc_ulabel:
            fprintf(listFile, "Undefined label");
            break;
    }
}

/* Put the type */
void put_ty(TYP *tp)
{
    if ((tp == 0) || (!prm_listfile))
        return ;
    switch (tp->type)
    {
        case bt_matchall:
            fprintf(listFile, "Undefined");
            break;
        case bt_char:
            fprintf(listFile, "Char");
            break;
        case bt_bool:
            fprintf(listFile, "Bool");
            break;
        case bt_short:
            fprintf(listFile, "Short");
            break;
        case bt_enum:
            fprintf(listFile, "enum ");
            goto ucont;
        case bt_long:
            fprintf(listFile, "Long");
            break;
        case bt_longlong:
            fprintf(listFile, "Long Long");
            break;
        case bt_int:
            fprintf(listFile, "Int");
            break;
        case bt_unsignedchar:
            fprintf(listFile, "Unsigned Char");
            break;
        case bt_unsignedshort:
            fprintf(listFile, "Unsigned Short");
            break;
        case bt_unsigned:
            fprintf(listFile, "Unsigned");
            break;
        case bt_unsignedlong:
            fprintf(listFile, "Unsigned Long");
            break;
        case bt_unsignedlonglong:
            fprintf(listFile, "Unsigned Long Long");
            break;
        case bt_fcomplex:
            fprintf(listFile, "Float Complex");
            break;
        case bt_rcomplex:
            fprintf(listFile, "Double Complex");
            break;
        case bt_lrcomplex:
            fprintf(listFile, "Long Double Complex");
            break;
        case bt_fimaginary:
            fprintf(listFile, "Float Imaginary");
            break;
        case bt_rimaginary:
            fprintf(listFile, "Double Imaginary");
            break;
        case bt_lrimaginary:
            fprintf(listFile, "Long Double Imaginary");
            break;
        case bt_float:
            fprintf(listFile, "Float");
            break;
        case bt_double:
            fprintf(listFile, "Double");
            break;
        case bt_longdouble:
            fprintf(listFile, "Long Double");
            break;
        case bt_farpointer:
            fprintf(listFile, "Far ");
            // fall through
        case bt_pointer:
            if (tp->val_flag == 0)
                fprintf(listFile, "Pointer to ");
            else
                if (tp->val_flag & VARARRAY) {
                    fprintf(listFile,"VArray of ");
                    tp = tp->btp;
                } else
                    fprintf(listFile, "Array of ");
            put_ty(tp->btp);
            break;
        case bt_union:
            fprintf(listFile, "union ");
            goto ucont;
            case bt_ref:
                fprintf(listFile, "Reference to ");
                put_ty(tp->btp);
                break;
            case bt_memberptr:
                fprintf(listFile, "Member Pointer to %s::", tp->sp->name);
                put_ty(tp->btp->btp);
                break;
            case bt_class:
                fprintf(listFile, "class ");
                goto ucont;
        case bt_struct:
            fprintf(listFile, "struct ");
            ucont: if (tp->sp == 0)
                fprintf(listFile, tn_unnamed);
            else
                fprintf(listFile, "%s ", unmangledname(tp->sp->name));
            break;
        case bt_void:
            fprintf(listFile, "Void");
            break;
        case bt_ifunc:
        case bt_func:
            fprintf(listFile, "Function returning ");
            put_ty(tp->btp);
            break;
    }
    if (tp->startbit !=  - 1)
        fprintf(listFile, "  Bits %d to %d", tp->startbit, tp->startbit + tp
            ->bits - 1);
}

/* List a variable */
void list_var(SYM *sp, int i)
{
    int j;
    long val;
    if (!prm_listfile)
        return ;
    if (sp->dontlist)
        return;
    if (sp->tp->type == bt_defunc)
    {
        sp = sp->tp->lst.head;
        while (sp)
        {
            list_var(sp, 0);
            sp = sp->next;
        }
        return ;
    }
    for (j = i; j; --j)
        fprintf(listFile, "    ");
    if ((sp->storage_class == sc_auto || sp->storage_class == sc_autoreg) &&
        !sp->inreg)
        val = (long)getautoval(sp->value.i);
    else if (sp->storage_class == sc_static || sp->storage_class == sc_global)
        val = sp->offset;
    else
        val = sp->value.u;
    fprintf(listFile,"Identifier:   %s\n    ", unmangledname(sp->name));
    for (j = i; j; --j)
        fprintf(listFile, "    ");
    if (sp->inreg) {
        fprintf(listFile,"Register: %-3s      ",
            registers[( - val) & 255]);
    }
    else
        fprintf(listFile,"Offset:   %08X ", val);
    fprintf(listFile,"Storage: ");
    if (sp->tp->type == bt_ifunc)
        if (sp->value.classdata.cppflags &PF_INLINE)
            fprintf(listFile,"%-7s","inline");
        else
            fprintf(listFile,"%-7s","code");
    else if (sp->storage_class == sc_auto)
        if (sp->inreg)
            fprintf(listFile,"%-7s","reg");
        else
            fprintf(listFile,"%-7s","stack");
    else if (sp->storage_class == sc_global || sp->storage_class == sc_static)
        if ((sp->tp->cflags &DF_CONST) && !(sp->tp->cflags &DF_VOL))
            fprintf(listFile,"%-7s","const");
        else if (sp->init)
            fprintf(listFile,"%-7s","data");
        else
            fprintf(listFile,"%-7s","bss");
    else if (sp->storage_class == sc_const)
        fprintf(listFile,"%-7s","inline");
    else
        fprintf(listFile,"%-7s","none");
    put_sc(sp->storage_class);
    put_ty(sp->tp);
    fprintf(listFile, "\n");
    if (sp->tp == 0)
        return ;
    if (isstructured(sp->tp) && sp->storage_class == sc_type)
        list_table(&(sp->tp->lst), i + 1);
}

/* List an entire table */
void list_table(TABLE *t, int j)
{
    SYM *sp;
    int i;
    if (!prm_listfile)
        return ;
    if (t == gsyms || t == tagtable)
    {
        for (i = 0; i < HASHTABLESIZE; i++)
        {
            if ((sp = ((SYM **)t)[i]) != 0)
            {
                while (sp)
                {
                    if (sp->storage_class != sc_type || t == tagtable)
                        list_var(sp, j);
                    sp = sp->next;
                }
            }
        }
    }
    else
    {
        sp = t->head;
        while (sp != NULL)
        {
            list_var(sp, j);
            sp = sp->next;
        }
    }

}
