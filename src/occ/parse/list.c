/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
/* 
 * Listing file
 */
#include <stdio.h>
#include <string.h>
#include "compiler.h"

extern ARCH_ASM *chosenAssembler;
extern COMPILER_PARAMS cparams;
extern FILE *listFile;
extern char *registers[];

/* Unnamed structure tags */
char *tn_unnamed = "<no name> ";

void list_table(HASHTABLE *t, int j);

static char *unmangledname(char *str)
{
    static char name[1024];
    unmangle(name, str);
    return name;
}

/* Put the storage class */
void put_sc(int scl)
{
    if (!cparams.prm_listfile)
        return ;
    switch (scl)
    {
        case sc_static:
        case sc_localstatic:
            fprintf(listFile, "Static      ");
            break;
        case sc_auto:
            fprintf(listFile, "Auto        ");
            break;
        case sc_register:
            fprintf(listFile, "Register    ");
            break;
        case sc_global:
            fprintf(listFile, "Global      ");
            break;
        case sc_absolute:
            fprintf(listFile, "Absolute    ");
            break;
        case sc_external:
            fprintf(listFile, "External    ");
            break;
        case sc_type:
        case sc_typedef:
            fprintf(listFile, "Type        ");
            break;
        case sc_const:
            fprintf(listFile, "Constant    ");
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
void put_ty(TYPE *tp)
{
    if ((tp == 0) || (!cparams.prm_listfile))
        return ;
    switch (tp->type)
    {
        case bt_any:
            fprintf(listFile, "Undefined");
            break;
        case bt_char:
            fprintf(listFile, "Char");
            break;
        case bt_bool:
            fprintf(listFile, "Bool");
            break;
        case bt_bit:
            fprintf(listFile, "Bit");
            break;
        case bt_inative:
            fprintf(listFile, "Native int");
            break;
        case bt_unative:
            fprintf(listFile, "Native unsigned int");
            break;
        case bt_short:
            fprintf(listFile, "Short");
            break;
        case bt_wchar_t:
            fprintf(listFile, "Wchar_t");
            break;
        case bt_enum:
            fprintf(listFile, "enum ");
            goto ucont;
        case bt_long:
            fprintf(listFile, "Long");
            break;
        case bt_long_long:
            fprintf(listFile, "Long Long");
            break;
        case bt_int:
            fprintf(listFile, "Int");
            break;
        case bt_char16_t:
            fprintf(listFile, "Char16_t");
            break;
        case bt_char32_t:
            fprintf(listFile, "Char32_t");
            break;
        case bt_signed_char:
            fprintf(listFile, "Signed Char");
            break;
        case bt_unsigned_char:
            fprintf(listFile, "Unsigned Char");
            break;
        case bt_unsigned_short:
            fprintf(listFile, "Unsigned Short");
            break;
        case bt_unsigned:
            fprintf(listFile, "Unsigned");
            break;
        case bt_unsigned_long:
            fprintf(listFile, "Unsigned Long");
            break;
        case bt_unsigned_long_long:
            fprintf(listFile, "Unsigned Long Long");
            break;
        case bt_float_complex:
            fprintf(listFile, "Float Complex");
            break;
        case bt_double_complex:
            fprintf(listFile, "Double Complex");
            break;
        case bt_long_double_complex:
            fprintf(listFile, "Long Double Complex");
            break;
        case bt_float_imaginary:
            fprintf(listFile, "Float Imaginary");
            break;
        case bt_double_imaginary:
            fprintf(listFile, "Double Imaginary");
            break;
        case bt_long_double_imaginary:
            fprintf(listFile, "Long Double Imaginary");
            break;
        case bt_float:
            fprintf(listFile, "Float");
            break;
        case bt_double:
            fprintf(listFile, "Double");
            break;
        case bt_long_double:
            fprintf(listFile, "Long Double");
            break;
        case bt___string:
            fprintf(listFile, "__string");
            break;
        case bt___object:
            fprintf(listFile, "__object");
            break;

        case bt_far:
            fprintf(listFile, "Far ");
            /* fall through*/
        case bt_pointer:
            if (!tp->vla && !tp->array)
                fprintf(listFile, "Pointer to ");
            else
                if (tp->vla) {
                    fprintf(listFile,"VArray of ");
                    tp = tp->btp;
                } else
                    fprintf(listFile, "Array of ");
            put_ty(tp->btp);
            break;
        case bt_union:
            fprintf(listFile, "union ");
            goto ucont;
            case bt_lref:
                fprintf(listFile, "Reference to ");
                put_ty(tp->btp);
                break;
            case bt_rref:
                fprintf(listFile, "r-value Reference to ");
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
                fprintf(listFile, "%s", tn_unnamed);
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
        default:
            fprintf(listFile, "???");
            break;
            
    }
    if (tp->bits !=  0)
        fprintf(listFile, "  Bits %d to %d", tp->startbit, tp->startbit + tp
            ->bits - 1);
}

/* List a variable */
void list_var(SYMBOL *sp, int i)
{
    int j;
    long val;
    if (!cparams.prm_listfile)
        return ;
    if (sp->dontlist)
        return;
    if (sp->tp->type == bt_aggregate)
    {
        HASHREC *hr = sp->tp->syms->table[0];
        while (hr)
        {
            sp = (SYMBOL *)hr->p;
            list_var(sp, 0);
            hr = hr->next;
        }
        return ;
    }
    for (j = i; j; --j)
        fprintf(listFile, "    ");
    if (sp->storage_class == sc_auto && !sp->regmode)
        val = (long)getautoval(sp->offset);
    else
        val = sp->value.u;
    fprintf(listFile,"Identifier:   %s\n    ", unmangledname(sp->name));
    for (j = i; j; --j)
        fprintf(listFile, "    ");
    if (sp->regmode == 1) {
        fprintf(listFile,"Register: %-3s&     ",lookupRegName((-sp->offset) & 255));
    }
    else if (sp->regmode == 2) {
        fprintf(listFile,"Register: %-3s      ",lookupRegName((-sp->offset) & 255));
    }
    else
        fprintf(listFile,"Offset:   %08lX ", val);
    fprintf(listFile,"Storage: ");
    if (sp->tp->type == bt_ifunc)
        if (sp->isInline && !sp->noinline)
            fprintf(listFile,"%-7s","inline");
        else
            fprintf(listFile,"%-7s","code");
    else if (sp->storage_class == sc_auto)
        if (sp->regmode)
            fprintf(listFile,"%-7s","reg");
        else
            fprintf(listFile,"%-7s","stack");
    else if (sp->storage_class == sc_global || sp->storage_class == sc_static 
             || sp->storage_class == sc_localstatic)
        if (isconst(sp->tp))
            fprintf(listFile,"%-7s","const");
        else if (sp->init)
            fprintf(listFile,"%-7s","data");
        else
            fprintf(listFile,"%-7s","bss");
    else if (sp->storage_class == sc_constant || sp->storage_class == sc_enumconstant)
        fprintf(listFile,"%-7s","constant");
    else
        fprintf(listFile,"%-7s","none");
    put_sc(sp->storage_class);
    put_ty(sp->tp);
    fprintf(listFile, "\n");
    if (sp->tp == 0)
        return ;
    if (isstructured(sp->tp) && sp->storage_class == sc_type)
        list_table(sp->tp->syms, i + 1);
}

/* List an entire table */
/* won't do child namespaces */
void list_table(HASHTABLE *t, int j)
{
    SYMBOL *sp;
    int i;
    if (!cparams.prm_listfile)
        return ;
    while (t)
    {
        for (i=0; i < t->size; i++)
        {
            HASHREC *hr = t->table[i];
            while (hr)
            {
                list_var((SYMBOL *)hr->p, j);
                hr = hr->next;
            }
        }
        t = t->next;
    }
}
