/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "be.h"

extern int startlab, retlab;
extern OCODE *peep_head, *peep_tail;
extern BOOLEAN inASMdata;
extern LIST *typeList;
extern LIST *enumList;
extern int MSILLocalOffset;
extern TYPE stdint;
extern EXPRESSION *objectArray_exp;
#define MAX_ALIGNS 50

static int fstackid;
static int inframe;
static int switch_deflab;
static LLONG_TYPE switch_range, switch_case_count, switch_case_max;
static IMODE *switch_ip;
static AMODE *switch_ip_a;
static enum {swm_enumerate, swm_compactstart, swm_compact, swm_tree} switch_mode;
static int switch_lastcase;
static int *switchTreeLabels, *switchTreeBranchLabels ;
static LLONG_TYPE *switchTreeCases;
static int switchTreeLabelCount;
static int switchTreePos;
static AMODE *stackap;
static int returnCount;
static int hookCount;
static int stackpos = 0;

void increment_stack(void)
{
    if (++stackpos > stackap->offset->v.i)
        stackap->offset->v.i = stackpos;
}
void decrement_stack(void)
{
    --stackpos;
}
void add_peep(OCODE *code)
{
    if (peep_head)
    {
        code->back = peep_tail;
        peep_tail = peep_tail->fwd = code;
    }
    else
    {
        peep_head = peep_tail = code;
    }
}
void gen_code(enum e_op op, AMODE *ap)
{
    OCODE *code = beLocalAlloc(sizeof(OCODE));
    code->opcode = op;
    code->oper1 = ap;
    add_peep(code);
}
void oa_gen_label(int labno)
/*
 *      add a compiler generated label to the peep list.
 */
{
    OCODE *new;
    new = beLocalAlloc(sizeof(OCODE));
    new->opcode = op_label;
    new->oper1 = (AMODE*)labno;
    add_peep(new);
}

AMODE *make_label(int lab)
{
    EXPRESSION *lnode;
    AMODE *ap;
    lnode = beLocalAlloc(sizeof(EXPRESSION));
    lnode->type = en_labcon;
    lnode->v.i = lab;
    ap = beLocalAlloc(sizeof(AMODE));
    ap->mode = am_immed;
    ap->offset = lnode;
    ap->length = ISZ_UINT;
    return ap;
}
AMODE *make_constant(int sz, EXPRESSION *exp)
{
    AMODE *ap;
    ap = beLocalAlloc(sizeof(AMODE));
    ap->mode = am_immed;
    ap->offset = exp;
    ap->length = sz;
    return ap;
} 
AMODE *make_index(enum e_am am, int index, SYMBOL *sym)
{
    AMODE *ap;
    ap = beLocalAlloc(sizeof(AMODE));
    ap->mode = am;
    ap->index = index;
    ap->altdata = sym;
    ap->length = -ISZ_UINT;
    return ap;
}
/*-------------------------------------------------------------------------*/
BOOLEAN isauto(EXPRESSION *ep)
{
    if (ep->type == en_auto)
        return TRUE;
    if (ep->type == en_add || ep->type == en_structadd)
        return isauto(ep->left) || isauto(ep->right);
    if (ep->type == en_sub)
        return isauto(ep->left);
    return FALSE;
}
void compile_start(char *name)
{
    inASMdata = FALSE;
    typeList = NULL;
    enumList = NULL;
    _using_init();
    cparams.prm_asmfile = TRUE; // temporary
}
void include_start(char *name, int num)
{
}
static void callLibrary(char *name, int size)
{
}
void oa_gen_vtt(VTABENTRY *vt, SYMBOL *func)
{
}
void oa_gen_vc1(SYMBOL *func)
{
}
void oa_gen_importThunk(SYMBOL *func)
{
}
AMODE *getAmode(IMODE *oper)
{
    AMODE *rv = NULL;
    switch(oper->mode)
    {
        case i_ind:
            rv = beLocalAlloc(sizeof(AMODE));
            rv->mode = am_ind;
            break;
        case i_immed:
            rv = make_constant(oper->size, oper->offset);
            break;
        case i_direct:
        {
            EXPRESSION *en = GetSymRef(oper->offset);
            SYMBOL *sp;
            if (en)
            {
                sp = en->v.sp;
            }
            else if (oper->offset->type == en_tempref)
            {
                sp = (SYMBOL *)oper->offset->right;
            }
            if (sp)
            {
                if (sp->storage_class == sc_auto || sp->storage_class == sc_register)
                    rv = make_index(am_local, sp->offset, sp);
                else if (sp->storage_class == sc_parameter)
                    rv = make_index(am_param, sp->offset, sp);
                else
                {
                    rv = beLocalAlloc(sizeof(AMODE));
                    rv->mode = am_global;
                    rv->offset = oper->offset;
                }
            }
            else if (oper->offset->type != en_tempref)
            {
                if (oper->offset == objectArray_exp)
                {
                    rv = beLocalAlloc(sizeof(AMODE));
                    rv->mode = am_objectArray;

                }
                else
                {
                    diag("Invalid load node");
                }
            }
            break;
        }
    }
    if (rv)
        rv->length = oper->size;
    return rv;
}

void load_ind(int sz)
{
    enum e_op op;
    switch(sz)
    {
        case ISZ_BOOLEAN:
        case ISZ_UCHAR:
            op = op_ldind_u1;
            break;
        case -ISZ_UCHAR:
            op = op_ldind_i1;
            break;
        case ISZ_USHORT:
        case ISZ_WCHAR:
        case ISZ_U16:
            op = op_ldind_u2;
            break;
        case -ISZ_USHORT:
            op = op_ldind_i2;
            break;
        case ISZ_UINT:
        case ISZ_ULONG:
        case ISZ_U32:
            op = op_ldind_u4;
            break;
        case -ISZ_UINT:
        case -ISZ_ULONG:
            op = op_ldind_i4;
            break;
        case ISZ_ULONGLONG:
            op = op_ldind_u8;
            break;
        case -ISZ_ULONGLONG:
            op = op_ldind_i8;
            break;
        case ISZ_ADDR:
            // check for __va_arg__ on a pointer type
            if (peep_tail->opcode == op_call && peep_tail->oper1->mode == am_ptrunbox)
                return;
            op = op_ldind_u4;
            break;
        /* */
        case ISZ_FLOAT:
            op = op_ldind_r4;
            break;
        case ISZ_DOUBLE:
        case ISZ_LDOUBLE:
            op = op_ldind_r8;
            break;
        
        case ISZ_IFLOAT:
            op = op_ldind_r4;
            break;
        case ISZ_IDOUBLE:
        case ISZ_ILDOUBLE:
            op = op_ldind_r8;
            break;
        
        case ISZ_CFLOAT:
        case ISZ_CDOUBLE:
        case ISZ_CLDOUBLE:
            break;
    }
    gen_code(op, NULL);

}
void store_ind(int sz)
{
    enum e_op op;
    if (sz < 0)
        sz = - sz;
    switch(sz)
    {
        case ISZ_BOOLEAN:
        case ISZ_UCHAR:
            op = op_stind_i1;
            break;
        case ISZ_USHORT:
        case ISZ_WCHAR:
        case ISZ_U16:
            op = op_stind_i2;
            break;
        case ISZ_UINT:
        case ISZ_ULONG:
        case ISZ_U32:
            op = op_stind_i4;
            break;
        case ISZ_ULONGLONG:
            op = op_stind_i8;
            break;
        case ISZ_ADDR:
            op = op_stind_i4;
            break;
        /* */
        case ISZ_FLOAT:
            op = op_stind_r4;
            break;
        case ISZ_DOUBLE:
        case ISZ_LDOUBLE:
            op = op_stind_r8;
            break;
        
        case ISZ_IFLOAT:
            op = op_stind_r4;
            break;
        case ISZ_IDOUBLE:
        case ISZ_ILDOUBLE:
            op = op_stind_r8;
            break;
        
        case ISZ_CFLOAT:
        case ISZ_CDOUBLE:
        case ISZ_CLDOUBLE:
            break;
    }
    gen_code(op, NULL);
    decrement_stack();
    decrement_stack();

}
void load_constant(int sz, EXPRESSION *exp)
{
    int sz1;
    enum e_op op;
    AMODE *ap = NULL;
    EXPRESSION *en = GetSymRef(exp);
    sz1 = sz;
    if (sz < 0)
        sz1 = - sz;
    if (en)
    {
        if (en->type == en_labcon)
        {
            op = op_ldsflda;
        }
        else if (en->v.sp->storage_class == sc_auto || en->v.sp->storage_class == sc_register)
        {
            op = op_ldloca;
            ap = make_index(am_local, en->v.sp->offset, en->v.sp);
        }
        else if (en->v.sp->storage_class == sc_parameter)
        {
            // in this version of the compiler, blocked parameters are passed
            // as pointers.   So instead of loading the parameter's address from
            // the stack we load the pointer to the parameter
            op = isstructured(en->v.sp->tp) ? op_ldarg : op_ldarga;
            ap = make_index(am_param, en->v.sp->offset, en->v.sp);
        }
        else if (isfunction(en->v.sp->tp))
        {
            op = op_ldftn;
            ap = make_constant(sz, exp);
        }
        else
        {
            op = op_ldsflda;
        }
    }
    else switch(sz1)
    {
        case 0:
        case ISZ_BOOLEAN:
        case ISZ_UCHAR:
        case ISZ_USHORT:
        case ISZ_WCHAR:
        case ISZ_U16:
        case ISZ_UINT:
        case ISZ_ULONG:
        case ISZ_U32:
            op = op_ldc_i4;
            break;
        case ISZ_ULONGLONG:
            op = op_ldc_i8;
            break;
        case ISZ_ADDR:
            op = op_ldc_i4;
            break;
        /* */
        case ISZ_FLOAT:
            op = op_ldc_r4;
            break;
        case ISZ_DOUBLE:
        case ISZ_LDOUBLE:
            op = op_ldc_r8;
            break;
        
        case ISZ_IFLOAT:
            op = op_ldc_r4;
            break;
        case ISZ_IDOUBLE:
        case ISZ_ILDOUBLE:
            op = op_ldc_r8;
            break;
        
        case ISZ_CFLOAT:
        case ISZ_CDOUBLE:
        case ISZ_CLDOUBLE:
            break;
    }
    if (!ap)
        ap = make_constant(sz, exp);
    gen_code(op, ap);
    increment_stack();
}
void gen_load(AMODE *dest)
{
    if (!dest)
        return;
    switch(dest->mode)
    {
        case am_immed:
            load_constant(dest->length, dest->offset);
            break;
        case am_ind:
            load_ind(dest->length);
            break;
        case am_local:
            gen_code(op_ldloc, dest);
            increment_stack();
            break;
        case am_param:
            gen_code(op_ldarg, dest);
            increment_stack();
            break;
        case am_global:
            gen_code(op_ldsfld, dest);
            increment_stack();
            break;
    }
}
void gen_store(AMODE *dest)
{
    if (!dest)
        return;
    switch(dest->mode)
    {
        case am_ind:
            store_ind(dest->length);
            break;
        case am_local:
            gen_code(op_stloc, dest);
            decrement_stack();
            break;
        case am_param:
            gen_code(op_starg, dest);
            decrement_stack();
            break;
        case am_global:
            gen_code(op_stsfld, dest);
            decrement_stack();
            break;
    }
}
void gen_convert(AMODE *dest, int sz)
{
    enum e_op op;
    switch(sz)
    {
        case ISZ_BOOLEAN:
        case ISZ_UCHAR:
            op = op_conv_u1;
            break;
        case -ISZ_UCHAR:
            op = op_conv_i1;
            break;
        case ISZ_USHORT:
        case ISZ_WCHAR:
        case ISZ_U16:
            op = op_conv_u2;
            break;
        case -ISZ_USHORT:
            op = op_conv_i2;
            break;
        case ISZ_UINT:
        case ISZ_ULONG:
        case ISZ_U32:
            op = op_conv_u4;
            break;
        case -ISZ_UINT:
        case -ISZ_ULONG:
            op = op_conv_i4;
            break;
        case ISZ_ULONGLONG:
            op = op_conv_u8;
            break;
        case -ISZ_ULONGLONG:
            op = op_conv_i8;
            break;
        case ISZ_ADDR:
            op = op_conv_u4;
            break;
        /* */
        case ISZ_FLOAT:
            op = op_conv_r4;
            break;
        case ISZ_DOUBLE:
        case ISZ_LDOUBLE:
            op = op_conv_r8;
            break;
        
        case ISZ_IFLOAT:
            op = op_conv_r4;
            break;
        case ISZ_IDOUBLE:
        case ISZ_ILDOUBLE:
            op = op_conv_r8;
            break;
        
        case ISZ_CFLOAT:
        case ISZ_CDOUBLE:
        case ISZ_CLDOUBLE:
            break;
    }
    gen_code(op, NULL);
}
void gen_branch(enum e_op op, int label, BOOLEAN decrement)
{
    AMODE *ap = make_label(label);
    gen_code(op, ap);
    if (decrement)
    {
        switch (op)
        {
            case op_br:
            case op_br_s:
                break;
            case op_brtrue:
            case op_brtrue_s:
            case op_brfalse:
            case op_brfalse_s:
                decrement_stack();
                break;
            default:
                decrement_stack();
                decrement_stack();
                break;
        }
    }
}
void put_label(int label)
{
}

void asm_expressiontag(QUAD *q)
{
    if (!q->dc.v.label)
    {
        // expression tags can be nested...
        int n = 1;
        q = q->back;
        while (n && (q->dc.opcode == op_line || q->dc.opcode == i_expressiontag))
        {
            if (q->dc.opcode == i_expressiontag)
                if (q->dc.v.label)
                    n--;
                else
                    n++;
            q = q->back;
        }
        if (n)
        {

            gen_code(op_pop, NULL);
            decrement_stack();
        }
    }
}
void asm_line(QUAD *q)               /* line number information and text */
{
    OCODE *new = beLocalAlloc(sizeof(OCODE));
    new->opcode = op_line;
    new->oper1 = (AMODE*)(q->dc.left); /* line data */
    add_peep(new);
}
void asm_blockstart(QUAD *q)               /* line number information and text */
{
    OCODE *new = beLocalAlloc(sizeof(OCODE));
    new->opcode = op_blockstart;
    add_peep(new);
}
void asm_blockend(QUAD *q)               /* line number information and text */
{
    OCODE *new = beLocalAlloc(sizeof(OCODE));
    new->opcode = op_blockend;
    add_peep(new);
}
void asm_varstart(QUAD *q)               /* line number information and text */
{
}
void asm_func(QUAD *q)               /* line number information and text */
{
    OCODE *new = beLocalAlloc(sizeof(OCODE));
    new->opcode = q->dc.v.label ? op_funcstart : op_funcend;
    new->oper1 = (AMODE*)(q->dc.left->offset->v.sp); /* line data */
    add_peep(new);
}
void asm_passthrough(QUAD *q)        /* reserved */
{
}
void asm_datapassthrough(QUAD *q)        /* reserved */
{
}
void asm_label(QUAD *q)              /* put a label in the code stream */
{
    OCODE *out = beLocalAlloc(sizeof(OCODE));
    out->opcode = op_label;
    out->oper1 = (AMODE *)q->dc.v.label;
    add_peep(out);
}
void asm_goto(QUAD *q)               /* unconditional branch */
{
    if (q->dc.opcode == i_goto)
        gen_branch(op_br, q->dc.v.label, FALSE);
    else
    {
        // i don't know if this is kosher in the middle of a function...
        gen_code(op_tail_, 0);
        gen_code(op_calli, 0);
    }

}
// this implementation won't handle varag functions nested in other varargs...
void asm_parm(QUAD *q)               /* push a parameter*/
{
    if (q->vararg)
    {
        AMODE *ap = (AMODE *)beLocalAlloc(sizeof(AMODE));
        if (q->dc.left->size == ISZ_ADDR)
        {
            ap->mode = am_ptrbox;
            gen_code(op_call, ap);
        }
        else
        {

            ap->mode = am_sized;
            ap->length = q->dc.left->size;
            gen_code(op_box, ap);
        }
        gen_code(op_stelem_ref, NULL);
        decrement_stack();
        decrement_stack();
    }
    else if (q->valist && q->valist->type == en_l_p)
    {
        QUAD *find = q;
        while (find && find->dc.opcode != i_gosub)
            find = find->fwd;
        if (find)
        {
            FUNCTIONCALL *params = (FUNCTIONCALL *)find->altdata;
            if (!msil_managed(params->sp))
            {
                AMODE *ap = (AMODE *)beLocalAlloc(sizeof(AMODE));
                ap->mode = am_argit_unmanaged;
                gen_code (op_callvirt, ap);
            }
        }
    }
}
void asm_parmblock(QUAD *q)          /* push a block of memory */
{
    if (q->vararg)
    {
        gen_code(op_stelem_ref, NULL);
        decrement_stack();
        decrement_stack();
    }

}
void asm_parmadj(QUAD *q)            /* adjust stack after function call */
{
    int i;
    int n = beGetIcon(q->dc.left) - beGetIcon(q->dc.right);
    if (n > 0)
        for (i=0; i < n; i++)
            decrement_stack();
    else if (n < 0)
        increment_stack();
}
static BOOLEAN bltin_gosub(QUAD *q, AMODE *ap)
{
    if (!strcmp(ap->offset->v.sp->name, "__va_start__"))
    {
        AMODE *ap1 = Alloc(sizeof(AMODE));
        ap1->mode = am_argit_args;
        gen_code(op_ldarg, ap1);
        ap->offset->v.sp->genreffed = FALSE;
        ap->mode = am_argit_ctor;
        ap->offset = NULL;
        gen_code(op_newobj, ap);
        return TRUE;
    }
    else if (!strcmp(ap->offset->v.sp->name, "__va_arg__"))
    {
        FUNCTIONCALL *func = q->altdata;
        TYPE *tp = ap->offset->v.sp->tp;
        if (func->arguments->next)
            tp = func->arguments->next->tp;
        ap->offset->v.sp->genreffed = FALSE;
        ap->mode = am_argit_getnext;
        ap->offset = NULL;
        // the function pushes both an arglist val and a type to cast to on the stack
        // remove the type to cast to.
        peep_tail = peep_tail->back;
        peep_tail->fwd = NULL;
        gen_code(op_callvirt, ap);
        if (ispointer(tp))
        {
            ap = Alloc(sizeof(AMODE));
            ap->mode = am_ptrunbox;
            gen_code(op_call, ap);
        }
        else if (!isstructured(tp) && !isarray(tp))
        {

            EXPRESSION *exp = func->arguments->next->exp;
            ap = Alloc(sizeof(AMODE));
            ap->mode = am_type;
            ap->offset = exp;
            gen_code(op_unbox, ap);
        }
        return TRUE;
    }
    return FALSE;
}
void asm_gosub(QUAD *q)              /* normal gosub to an immediate label or through a var */
{
    if (q->dc.left->mode == i_immed)
    {
        AMODE *ap = getAmode(q->dc.left);
        if (!bltin_gosub(q, ap))
        {
            FUNCTIONCALL *func = q->altdata;
            ap->altdata = q->altdata;
            if (msil_managed(func->sp))
            {
                HASHREC *hr = basetype(func->sp->tp)->syms->table[0];
                while (hr && hr->next)
                    hr = hr->next;
                if (hr)
                    if (((SYMBOL *)hr->p)->tp->type == bt_ellipse)
                    {
                        if (objectArray_exp && !q->nullvararg)
                            gen_code(op_ldloc, make_index(am_local, objectArray_exp->v.sp->offset, objectArray_exp->v.sp));
                        else
                            gen_code(op_ldnull, 0);
                        increment_stack();
                        decrement_stack();
                    }
            }
            gen_code(op_call, ap);
        }
    }
    else
    {
        EXPRESSION *en = varNode(en_pc, ((FUNCTIONCALL *)q->altdata)->sp);
        AMODE *ap = make_constant(ISZ_UINT, en);
        ap->altdata = q->altdata;
        gen_code(op_calli, ap);
        decrement_stack();
    }
    if (q->novalue && q->novalue != -1)
    {
        gen_code(op_pop, NULL);
        decrement_stack();
    }
}
void asm_fargosub(QUAD *q)           /* far version of gosub */
{
}
void asm_trap(QUAD *q)               /* 'trap' instruction - the arg will be an immediate # */
{
}
void asm_int(QUAD *q)                /* 'int' instruction(QUAD *q) calls a labeled function which is an interrupt */
{
}
/* left will be a constant holding the number of bytes to pop
 * e.g. the parameters will be popped in stdcall or pascal type functions
 */
void asm_ret(QUAD *q)                /* return from subroutine */
{
    gen_code(op_ret, NULL);    
}
/* left will be a constant holding the number of bytes to pop
 * e.g. the parameters will be popped in stdcall or pascal type functions
 */
void asm_fret(QUAD *q)                /* far return from subroutine */
{
}
/*
 * this can be either a fault or iret return
 * for processors that char, the 'left' member will have an integer
 * value that is true for an iret or false or a fault ret
 */
void asm_rett(QUAD *q)               /* return from trap or int */
{
}
void asm_add(QUAD *q)                /* evaluate an addition */
{
    decrement_stack();
    gen_code(op_add, NULL);
}
void asm_sub(QUAD *q)                /* evaluate a subtraction */
{
    decrement_stack();
    gen_code(op_sub, NULL);
}
void asm_udiv(QUAD *q)               /* unsigned division */
{
    decrement_stack();
    gen_code(op_div_un, NULL);
}
void asm_umod(QUAD *q)               /* unsigned modulous */
{
    decrement_stack();
    gen_code(op_rem_un, NULL);
}
void asm_sdiv(QUAD *q)               /* signed division */
{
    decrement_stack();
    gen_code(op_div, NULL);
}
void asm_smod(QUAD *q)               /* signed modulous */
{
    decrement_stack();
    gen_code(op_rem, NULL);
}
void asm_muluh(QUAD *q)
{
    EXPRESSION *en = intNode(en_c_i, 32);
    AMODE *ap = make_constant(ISZ_UINT, en);
    gen_code(op_mul,NULL);
    gen_code(op_ldc_i4, ap);
    gen_code(op_shr_un, NULL);
    decrement_stack();
}
void asm_mulsh(QUAD *q)
{
    EXPRESSION *en = intNode(en_c_i, 32);
    AMODE *ap = make_constant(ISZ_UINT, en);
    gen_code(op_mul,NULL);
    gen_code(op_ldc_i4, ap);
    gen_code(op_shr, NULL);
    decrement_stack();
}
void asm_mul(QUAD *q)               /* signed multiply */
{
    decrement_stack();
    gen_code(op_mul, NULL);
}
void asm_lsr(QUAD *q)                /* unsigned shift right */
{
    decrement_stack();
    gen_code(op_shr_un, NULL);
}
void asm_lsl(QUAD *q)                /* signed shift left */
{
    decrement_stack();
    gen_code(op_shl, NULL);
}
void asm_asr(QUAD *q)                /* signed shift right */
{
    decrement_stack();
    gen_code(op_shr, NULL);
}
void asm_neg(QUAD *q)                /* negation */
{
    gen_code(op_neg, NULL);
}
void asm_not(QUAD *q)                /* complement */
{
    gen_code(op_not, NULL);
}
void asm_and(QUAD *q)                /* binary and */
{
    decrement_stack();
    gen_code(op_and, NULL);
}
void asm_or(QUAD *q)                 /* binary or */
{
    decrement_stack();
    gen_code(op_or, NULL);
}
void asm_eor(QUAD *q)                /* binary exclusive or */
{
    decrement_stack();
    gen_code(op_xor, NULL);
}
void asm_setne(QUAD *q)              /* evaluate a = b != c */
{
    gen_code(op_ceq, NULL);
    gen_code(op_ldc_i4_1, NULL);
    gen_code(op_xor, NULL);
    increment_stack();
    decrement_stack();
    decrement_stack();
}
void asm_sete(QUAD *q)               /* evaluate a = b == c */
{
    gen_code(op_ceq, NULL);
    decrement_stack();
}
void asm_setc(QUAD *q)               /* evaluate a = b U< c */
{
    gen_code(op_clt_un, NULL);
    decrement_stack();
}
void asm_seta(QUAD *q)               /* evaluate a = b U> c */
{
    gen_code(op_cgt_un, NULL);
    decrement_stack();
}
void asm_setnc(QUAD *q)              /* evaluate a = b U>= c */
{
    gen_code(op_clt_un, NULL);
    gen_code(op_ldc_i4_1, NULL);
    gen_code(op_xor, NULL);
    increment_stack();
    decrement_stack();
    decrement_stack();
}
void asm_setbe(QUAD *q)              /* evaluate a = b U<= c */
{
    gen_code(op_cgt_un, NULL);
    gen_code(op_ldc_i4_1, NULL);
    gen_code(op_xor, NULL);
    increment_stack();
    decrement_stack();
    decrement_stack();
}
void asm_setl(QUAD *q)               /* evaluate a = b S< c */
{
    gen_code(op_clt, NULL);
    decrement_stack();
}
void asm_setg(QUAD *q)               /* evaluate a = b s> c */
{
    gen_code(op_cgt, NULL);
    decrement_stack();
}
void asm_setle(QUAD *q)              /* evaluate a = b S<= c */
{
    gen_code(op_cgt, NULL);
    gen_code(op_ldc_i4_1, NULL);
    gen_code(op_xor, NULL);
    increment_stack();
    decrement_stack();
    decrement_stack();
}
void asm_setge(QUAD *q)              /* evaluate a = b S>= c */
{
    gen_code(op_clt, NULL);
    gen_code(op_ldc_i4_1, NULL);
    gen_code(op_xor, NULL);
    increment_stack();
    decrement_stack();
    decrement_stack();
}
void asm_assn(QUAD *q)               /* assignment */
{
    AMODE *ap;
    if (q->ans->mode == i_direct && !(q->temps & TEMP_ANS) && q->ans->offset->type == en_auto)
    {
        TYPE *tp = q->ans->offset->v.sp->tp;
        TYPE *tp1 = basetype(tp);
        while (tp != tp1)
        {
            if (tp->type == bt_objectArray)
            {
                // assign to object array, call the ctor here
                // count is already on the stack
                AMODE *ap = Alloc(sizeof(AMODE));
                ap->mode = am_objectArray_ctor;
                gen_code(op_newarr, ap);
                ap = getAmode(q->ans);
                gen_store(ap);
                return;
            }
            tp = tp->btp;
        }
    }
    ap = getAmode(q->dc.left);
    gen_load(ap);
    if (q->dc.left->size != 0 && q->dc.left->size != q->ans->size)
    {
        gen_convert(ap, q->ans->size);
    }
    ap = getAmode(q->ans);
    gen_store(ap);
    if (q->ans->retval)
        returnCount++;
    if (q->hook)
        hookCount++;
}
void asm_genword(QUAD *q)            /* put a byte or word into the code stream */
{
}
void compactgen(AMODE *ap, int lab)
{

    struct swlist *lstentry = beLocalAlloc(sizeof(struct swlist));
    lstentry->lab = lab;
    if (ap->switches)
    {
        ap->switchlast = ap->switchlast->next = lstentry;
    }
    else
    {
        ap->switches = ap->switchlast = lstentry;
    }
}
void bingen(int lower, int avg, int higher)
{
    int nelab = beGetLabel;
    if (switchTreeBranchLabels[avg] !=  0)
        oa_gen_label(switchTreeBranchLabels[avg]);
    gen_load(switch_ip_a);
    load_constant(switch_ip->size, intNode(en_c_i, switchTreeCases[avg]));
    gen_branch(op_beq,  switchTreeLabels[avg], FALSE);
    if (avg == lower)
    {
        gen_branch(op_br, switch_deflab, FALSE);
    }
    else
    {
        int avg1 = (lower + avg) / 2;
        int avg2 = (higher + avg + 1) / 2;
        int lab;
        if (avg + 1 < higher)
            lab = switchTreeBranchLabels[avg2] = beGetLabel;
        else
            lab = switch_deflab;
        gen_load(switch_ip_a);
        load_constant(switch_ip->size, intNode(en_c_i, switchTreeCases[avg]));
        if (switch_ip->size < 0)
            gen_branch(op_bgt, lab, FALSE);
        else
            gen_branch(op_bgt_un, lab, FALSE);
        bingen(lower, avg1, avg);
        if (avg + 1 < higher)
            bingen(avg + 1, avg2, higher);
    }
}

void asm_coswitch(QUAD *q)           /* switch characteristics */
{
    enum e_op op;
     switch_deflab = q->dc.v.label;
    switch_range = q->dc.right->offset->v.i;
    switch_case_max = switch_case_count = q->ans->offset->v.i;
    switch_ip = q->dc.left;
    switch_ip_a = getAmode(switch_ip);
    if (switch_ip->size == ISZ_ULONGLONG || switch_ip->size == - ISZ_ULONGLONG || switch_case_max <= 5)
    {
        switch_mode = swm_enumerate;
    }
    else if (switch_case_max * 10 / switch_range > 8)
    {
        switch_mode = swm_compactstart;
    }
    else
    {
        switch_mode = swm_tree;
        if (!switchTreeLabelCount || switchTreeLabelCount  < switch_case_max)
        {
            free(switchTreeCases);
            free(switchTreeLabels);
            free(switchTreeBranchLabels);
            switchTreeLabelCount = (switch_case_max + 1024) & ~1023;
            switchTreeCases = (LLONG_TYPE *)calloc(switchTreeLabelCount, sizeof (LLONG_TYPE));
            switchTreeLabels = (int *)calloc(switchTreeLabelCount, sizeof (int));
            switchTreeBranchLabels = (int *)calloc(switchTreeLabelCount, sizeof (int));
        }
        switchTreePos = 0;
        memset(switchTreeBranchLabels, 0, sizeof(int) * switch_case_max);
    }
}
void asm_swbranch(QUAD *q)           /* case characteristics */
{
    static AMODE *swap;
    ULLONG_TYPE swcase = q->dc.left->offset->v.i;
    int labin = q->dc.v.label, lab;
    if (switch_case_count == 0)
    {
/*		diag("asm_swbranch, count mismatch"); in case only a default */
        return;
    }

    if (switch_mode == swm_compactstart)
    {
        swap = beLocalAlloc(sizeof(AMODE));
        swap->mode = am_switch;
        gen_load(switch_ip_a);
        if (swcase != 0)
        {
            load_constant(switch_ip->size, intNode(en_c_i, swcase));
            gen_code(op_sub, NULL);
        }
        gen_code(op_switch, swap);
        decrement_stack();
        gen_branch(op_br, switch_deflab, FALSE);
    }
    switch(switch_mode)
    {
        int lab;
        case swm_enumerate:
        default:

            gen_load(switch_ip_a);
            load_constant(switch_ip->size, intNode(en_c_i, swcase));
            gen_branch(op_beq, labin, TRUE);
            if (-- switch_case_count == 0)
            {
                gen_branch(op_br, switch_deflab, FALSE);
            }
            break ;
        case swm_compact:
            while(switch_lastcase < swcase)
            {
                compactgen(swap, switch_deflab);
                switch_lastcase++;
            }
            // fall through
        case swm_compactstart:
            compactgen(swap, labin);
            switch_lastcase = swcase + 1;
            switch_mode = swm_compact;
            -- switch_case_count;
            if (!switch_case_count)
                decrement_stack();
            break ;
        case swm_tree:
            switchTreeCases[switchTreePos] = swcase;
            switchTreeLabels[switchTreePos++] = labin;
            if (--switch_case_count == 0)
            {
                bingen(0, switch_case_max / 2, switch_case_max);
                increment_stack();
                increment_stack();
                decrement_stack();
                decrement_stack();
            }
            break ;
    }
    
}
void asm_dc(QUAD *q)                 /* unused */
{
}
void asm_assnblock(QUAD *q)          /* copy block of memory*/
{
    EXPRESSION *size = q->ans->offset;
    load_constant(-ISZ_UINT, size);
    gen_code(op_cpblk, 0);
    decrement_stack();
    decrement_stack();
    decrement_stack();
}
void asm_clrblock(QUAD *q)           /* clear block of memory */
{
    // the 'value' field is loaded by examine_icode...
    gen_code(op_initblk, 0);
    decrement_stack();
    decrement_stack();
    decrement_stack();
}
void asm_jc(QUAD *q)                 /* branch if a U< b */
{
    gen_branch(op_blt_un, q->dc.v.label, TRUE);
}
void asm_ja(QUAD *q)                 /* branch if a U> b */
{
    gen_branch(op_bgt_un, q->dc.v.label, TRUE);
    
}
void asm_je(QUAD *q)                 /* branch if a == b */
{
    if (q->dc.right->mode == i_immed && isconstzero(&stdint, q->dc.right->offset))
        gen_branch(op_brfalse, q->dc.v.label, TRUE);
    else
        gen_branch(op_beq, q->dc.v.label, TRUE);
    
}
void asm_jnc(QUAD *q)                /* branch if a U>= b */
{
    gen_branch(op_bge_un, q->dc.v.label, TRUE);
    
}
void asm_jbe(QUAD *q)                /* branch if a U<= b */
{
    gen_branch(op_ble_un, q->dc.v.label, TRUE);
    
}
void asm_jne(QUAD *q)                /* branch if a != b */
{
    if (q->dc.right->mode == i_immed && isconstzero(&stdint, q->dc.right->offset))
        gen_branch(op_brtrue, q->dc.v.label, TRUE);
    else
        gen_branch(op_bne_un, q->dc.v.label, TRUE);
    
}
void asm_jl(QUAD *q)                 /* branch if a S< b */
{
    gen_branch(op_blt, q->dc.v.label, TRUE);

}
void asm_jg(QUAD *q)                 /* branch if a S> b */
{
    gen_branch(op_bgt, q->dc.v.label, TRUE);

}
void asm_jle(QUAD *q)                /* branch if a S<= b */
{
    gen_branch(op_ble, q->dc.v.label, TRUE);
    
}
void asm_jge(QUAD *q)                /* branch if a S>= b */
{
    gen_branch(op_bge, q->dc.v.label, TRUE);
    
}
void asm_cppini(QUAD *q)             /* cplusplus initialization (historic)*/
{
    (void)q;    
}
/*
 * function prologue.  left has a constant which is a bit mask
 * of registers to push.  It also has a flag indicating whether frames
 * are absolutely necessary
 *
 * right has the number of bytes to allocate on the stack
 */
void asm_prologue(QUAD *q)           /* function prologue */
{
    EXPRESSION *exp = intNode(en_c_i , 0);
    stackap = make_constant(ISZ_UINT, exp);
//    if (!strcmp(theCurrentFunc->decoratedName, "_main"))
//        gen_code(op_entrypoint, NULL);
    gen_code(op_maxstack, stackap);
    stackpos = 0;
    returnCount = 0;
    hookCount = 0;
}
/*
 * function epilogue, left holds the mask of which registers were pushed
 */
void asm_epilogue(QUAD *q)           /* function epilogue */
{
    if (basetype(theCurrentFunc->tp)->btp->type != bt_void)
        stackpos--;
    if (returnCount)
        stackpos -= returnCount -1;
    stackpos -= hookCount/2;
    if (stackpos != 0)
        diag("asm_epilogue: stack mismatch");
}
/*
 * in an interrupt handler, push the current context
 */
void asm_pushcontext(QUAD *q)        /* push register context */
{
}
/*
 * in an interrupt handler, pop the current context
 */
void asm_popcontext(QUAD *q)         /* pop register context */
{
}
/*
 * loads a context, e.g. for the loadds qualifier
 */
void asm_loadcontext(QUAD *q)        /* load register context (e.g. at interrupt level ) */
{
    
}
/*
 * unloads a context, e.g. for the loadds qualifier
 */
void asm_unloadcontext(QUAD *q)        /* load register context (e.g. at interrupt level ) */
{
    
}
void asm_tryblock(QUAD *q)			 /* try/catch */
{
}
void asm_stackalloc(QUAD *q)         /* allocate stack space - positive value = allocate(QUAD *q) negative value deallocate */
{
}
void asm_loadstack(QUAD *q)			/* load the stack pointer from a var */
{
}
void asm_savestack(QUAD *q)			/* save the stack pointer to a var */
{
}
void asm_functail(QUAD *q, int begin, int size)	/* functail start or end */
{
}
void asm_atomic(QUAD *q)
{
}
QUAD * leftInsertionPos(QUAD *head, IMODE *im)
{
    QUAD *rv = head;
    head = head->back;
    while (head && head->dc.opcode != i_block)
    {
        if (head->temps & TEMP_ANS)
        {
            if (im->offset->v.sp->value.i == head->ans->offset->v.sp->value.i)
            {
                rv = head;
                if (!(head->temps & TEMP_LEFT))
                    break;
                im = head->dc.left;
                if (im->offset->v.sp->pushedtotemp)
                    break;
            }
        }
        head = head->back;
    }
    return rv;
}
int examine_icode(QUAD *head)
{
    int parmIndex = 0;
    IMODE *fillinvararg = NULL;
    while (head)
    {
        if (head->dc.opcode == i_gosub)
        {
            if (fillinvararg)
                fillinvararg->offset->v.i = parmIndex;
            if (!parmIndex && ((FUNCTIONCALL *)head->altdata)->vararg)
                head->nullvararg = TRUE;
            fillinvararg = NULL;
            parmIndex = 0;
        }
        if (head->dc.opcode != i_block && head->dc.opcode != i_blockend 
            && head->dc.opcode != i_dbgblock && head->dc.opcode != i_dbgblockend && head->dc.opcode != i_var
            && head->dc.opcode != i_label && head->dc.opcode != i_line && head->dc.opcode != i_passthrough
            && head->dc.opcode != i_func && head->dc.opcode != i_gosub && head->dc.opcode != i_parmadj
            && head->dc.opcode != i_ret && head->dc.opcode != i_varstart
            && head->dc.opcode != i_coswitch && head->dc.opcode != i_swbranch
            && head->dc.opcode != i_expressiontag)
        {
            if (head->dc.opcode == i_muluh || head->dc.opcode == i_mulsh)
            {
                int sz = head->dc.opcode == i_muluh ? ISZ_ULONGLONG : - ISZ_ULONGLONG;
                IMODE *ap = InitTempOpt(sz, sz);
                QUAD *q = Alloc(sizeof(QUAD));
                q->dc.opcode = i_assn;
                q->ans = ap;
                q->temps = TEMP_ANS;
                q->dc.left = head->dc.left;
                head->dc.left = ap;
                head->temps |= TEMP_LEFT;
                InsertInstruction(head->back, q);
                head->dc.right->size = sz;
            }
            if (head->dc.left && head->dc.left->mode == i_immed && head->dc.opcode != i_assn)
            {
                IMODE *ap = InitTempOpt(head->dc.left->size, head->dc.left->size);
                QUAD *q = Alloc(sizeof(QUAD)), *t;
                q->dc.opcode = i_assn;
                q->ans = ap;
                q->temps = TEMP_ANS;
                q->dc.left = head->dc.left;
                head->dc.left = ap;
                head->temps |= TEMP_LEFT;
                t = head;
                if (head->temps & TEMP_RIGHT)
                    t = leftInsertionPos(head, head->dc.right);
                InsertInstruction(t->back, q);
            }
            if (head->dc.opcode == i_clrblock)
            {
                // insert the value to clear it to, e.g. zero
                IMODE *ap = InitTempOpt(head->dc.right->size, head->dc.right->size);
                QUAD *q = Alloc(sizeof(QUAD));
                q->alwayslive = TRUE;
                q->dc.opcode = i_assn;
                q->ans = ap;
                q->temps = TEMP_ANS;
                q->dc.left = beLocalAlloc(sizeof(AMODE));
                q->dc.left->mode = i_immed;
                q->dc.left->offset = intNode(en_c_i, 0);
                InsertInstruction(head->back, q);
            }
            if (head->dc.right && head->dc.right->mode == i_immed)
            {
                if (head->dc.opcode != i_je && head->dc.opcode != i_jne || !isconstzero(&stdint, head->dc.right->offset))
                {
                    IMODE *ap = InitTempOpt(head->dc.right->size, head->dc.right->size);
                    QUAD *q = Alloc(sizeof(QUAD));
                    q->dc.opcode = i_assn;
                    q->ans = ap;
                    q->temps = TEMP_ANS;
                    q->dc.left = head->dc.right;
                    head->dc.right = ap;
                    head->temps |= TEMP_RIGHT;
                    InsertInstruction(head->back, q);
                }
            }
            if (head->vararg)
            {
                // handle varargs... this won't work in the case of nested vararg funcs 
                QUAD *q = Alloc(sizeof(QUAD));
                QUAD *q1 = Alloc(sizeof(QUAD));
                IMODE *ap = InitTempOpt(ISZ_ADDR, ISZ_ADDR);
                IMODE *ap1 = Alloc(sizeof(IMODE));
                IMODE *ap2 = InitTempOpt(-ISZ_UINT, -ISZ_UINT);
                IMODE *ap3 = make_immed(-ISZ_UINT, parmIndex++);
                QUAD *prev = head;
                ap1->offset = objectArray_exp;
                ap1->mode = i_direct;
                ap1->size = ISZ_ADDR;
                while (prev->back && !prev->back->varargPrev)
                    prev = prev->back;
                if (parmIndex - 1 == 0)
                {
                    // this is for the initialization of the object array
                    QUAD *q2 = Alloc(sizeof(QUAD));
                    QUAD *q3 = Alloc(sizeof(QUAD));
                    IMODE *ap4 = InitTempOpt(ISZ_ADDR, ISZ_ADDR);
                    IMODE *ap5 = Alloc(sizeof(IMODE));
                    IMODE *ap6 = Alloc(sizeof(IMODE));
                    ap6->offset = objectArray_exp;
                    ap6->mode = i_direct;
                    ap6->size = ISZ_ADDR;
                    ap5->mode = i_immed;
                    ap5->size = -ISZ_UINT;
                    ap5->offset = intNode(en_c_i, 0);
                    fillinvararg = ap5;
                    q2->dc.opcode = i_assn;
                    q2->ans = ap4;
                    q2->dc.left = ap5;
                    q2->temps = TEMP_ANS;
                    q3->dc.opcode = i_assn;
                    q3->ans = ap6;
                    q3->dc.left = ap4;
                    q3->temps = TEMP_LEFT;
                    InsertInstruction(prev->back, q2);
                    InsertInstruction(prev->back, q3);
                }
                // this is to load this param into the object array
                // it inserts the params need by the stelem.ref
                // the stelem.ref and any boxing are done later...
                q->dc.opcode = i_assn;
                q->ans = ap;
                q->dc.left = ap1;
                q->temps = TEMP_ANS;
                q->alwayslive = TRUE;
                q1->dc.opcode = i_assn;
                q1->ans = ap2;
                q1->dc.left = ap3;
                q1->temps = TEMP_ANS;
                q1->alwayslive = TRUE;
                InsertInstruction(prev->back, q);
                InsertInstruction(prev->back, q1);
            }
        }
        head = head->fwd;
    }
}
