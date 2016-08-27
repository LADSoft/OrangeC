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
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "be.h"

#define IEEE


extern int prm_assembler;
extern SYMBOL *theCurrentFunc;
extern LIST *temporarySymbols;
extern LIST *externals;
extern TYPE stdpointer;
extern INCLUDES *includes;
OCODE *peep_head, *peep_tail;
static int uses_float;

void puttypewrapped(TYPE *tp);

LIST *typeList;
static enum e_gt oa_gentype = nogen; /* Current DC type */
enum e_sg oa_currentSeg = noseg; /* Current seg */
static int oa_outcol = 0; /* Curront col (roughly) */
int newlabel;
static int virtual_mode;

BOOLEAN inASMdata = FALSE;
static BOOLEAN int8_used, int16_used, int32_used;
static struct asm_details instructions[] = {
    {
        "reserved"
    } , 
    {
        "line#"
    }
    , 
    {
        "blks#"
    }
    , 
    {
        "blke#"
    }
    , 
    {
        "vars#"
    }
    , 
    {
        "funcs#"
    }
    , 
    {
        "funce#"
    }
    , 
    {
        "void#"
    }
    , 
    {
        "cmt#"
    }
    , 
    {
        "label#"
    }
    , 
    {
        "flabel#"
    }
    , 
    {
        "seq@"
    }
    , 
    {
        "db"
    }
    , 
    {
        "dd"
    }
    , 
    {
        ".maxstack"
    }
    , 
    {
        ".entrypoint"
    }
    , 
	{ "add" }, 
	{ "add.ovf" }, 
	{ "add.ovf.un" }, 
	{ "and" }, 
	{ "arglist" }, 
	{ "beq" }, 
	{ "beq.s" }, 
	{ "bge" }, 
	{ "bge.s" }, 
	{ "bge.un" }, 
	{ "bge.un.s" }, 
	{ "bgt" }, 
	{ "bgt.s" }, 
	{ "bgt.un" }, 
	{ "bgt.un.s" }, 
	{ "ble" }, 
	{ "ble.s" }, 
	{ "ble.un" }, 
	{ "ble.un.s" }, 
	{ "blt" }, 
	{ "blt.s" }, 
	{ "blt.un" }, 
	{ "blt.un.s" }, 
	{ "bne.un" }, 
	{ "bne.un.s" }, 
	{ "box" }, 
	{ "br" }, 
	{ "br.s" }, 
	{ "break" }, 
	{ "brfalse" }, 
	{ "brfalse.s" }, 
	{ "brinst" }, 
	{ "brinst.s" }, 
	{ "brnull" }, 
	{ "brnull.s" }, 
	{ "brtrue" }, 
	{ "brtrue.s" }, 
	{ "brzero" }, 
	{ "brzero.s" }, 
	{ "call" }, 
	{ "calli" }, 
	{ "callvirt" }, 
	{ "castclass" }, 
	{ "ceq" }, 
	{ "cgt" }, 
	{ "cgt.un" }, 
	{ "ckfinite" }, 
	{ "clt" }, 
	{ "clt.un" }, 
	{ "constrained." }, 
	{ "conv.i" }, 
	{ "conv.i1" }, 
	{ "conv.i2" }, 
	{ "conv.i4" }, 
	{ "conv.i8" }, 
	{ "conv.ovf.i" }, 
	{ "conv.ovf.i.un" }, 
	{ "conv.ovf.i1" }, 
	{ "conv.ovf.i1.un" }, 
	{ "conv.ovf.i2" }, 
	{ "conv.ovf.i2.un" }, 
	{ "conv.ovf.i4" }, 
	{ "conv.ovf.i4.un" }, 
	{ "conv.ovf.i8" }, 
	{ "conv.ovf.i8.un" }, 
	{ "conv.ovf.u" }, 
	{ "conv.ovf.u.un" }, 
	{ "conv.ovf.u1" }, 
	{ "conv.ovf.u1.un" }, 
	{ "conv.ovf.u2" }, 
	{ "conv.ovf.u2.un" }, 
	{ "conv.ovf.u4" }, 
	{ "conv.ovf.u4.un" }, 
	{ "conv.ovf.u8" }, 
	{ "conv.ovf.u8.un" }, 
	{ "conv.r.un" }, 
	{ "conv.r4" }, 
	{ "conv.r8" }, 
	{ "conv.u" }, 
	{ "conv.u1" }, 
	{ "conv.u2" }, 
	{ "conv.u4" }, 
	{ "conv.u8" }, 
	{ "cpblk" }, 
	{ "cpobj" }, 
	{ "div" }, 
	{ "div.un" }, 
	{ "dup" }, 
	{ "endfault" }, 
	{ "endfilter" }, 
	{ "endfinally" }, 
	{ "initblk" }, 
	{ "initobj" }, 
	{ "isinst" }, 
	{ "jmp" }, 
	{ "ldarg" }, 
	{ "ldarg.0" }, 
	{ "ldarg.1" }, 
	{ "ldarg.2" }, 
	{ "ldarg.3" }, 
	{ "ldarg.s" }, 
	{ "ldarga" }, 
	{ "ldarga.s" }, 
	{ "ldc.i4" }, 
	{ "ldc.i4.0" }, 
	{ "ldc.i4.1" }, 
	{ "ldc.i4.2" }, 
	{ "ldc.i4.3" }, 
	{ "ldc.i4.4" }, 
	{ "ldc.i4.5" }, 
	{ "ldc.i4.6" }, 
	{ "ldc.i4.7" }, 
	{ "ldc.i4.8" }, 
	{ "ldc.i4.m1" }, 
	{ "ldc.i4.M1" }, 
	{ "ldc.i4.s" }, 
	{ "ldc.i8" }, 
	{ "ldc.r4" }, 
	{ "ldc.r8" }, 
	{ "ldelem" }, 
	{ "ldelem.i" }, 
	{ "ldelem.i1" }, 
	{ "ldelem.i2" }, 
	{ "ldelem.i4" }, 
	{ "ldelem.i8" }, 
	{ "ldelem.r4" }, 
	{ "ldelem.r8" }, 
	{ "ldelem.ref" }, 
	{ "ldelem.u1" }, 
	{ "ldelem.u2" }, 
	{ "ldelem.u4" }, 
	{ "ldelem.u8" }, 
	{ "ldelema" }, 
	{ "ldfld" }, 
	{ "ldflda" }, 
	{ "ldftn" }, 
	{ "ldind.i" }, 
	{ "ldind.i1" }, 
	{ "ldind.i2" }, 
	{ "ldind.i4" }, 
	{ "ldind.i8" }, 
	{ "ldind.r4" }, 
	{ "ldind.r8" }, 
	{ "ldind.ref" }, 
	{ "ldind.u1" }, 
	{ "ldind.u2" }, 
	{ "ldind.u4" }, 
	{ "ldind.u8" }, 
	{ "ldlen" }, 
	{ "ldloc" }, 
	{ "ldloc.0" }, 
	{ "ldloc.1" }, 
	{ "ldloc.2" }, 
	{ "ldloc.3" }, 
	{ "ldloc.s" }, 
	{ "ldloca" }, 
	{ "ldloca.s" }, 
	{ "ldnull" }, 
	{ "ldobj" }, 
	{ "ldsfld" }, 
	{ "ldsflda" }, 
	{ "ldstr" }, 
	{ "ldtoken" }, 
	{ "ldvirtftn" }, 
	{ "leave" }, 
	{ "leave.s" }, 
	{ "localloc" }, 
	{ "mkrefany" }, 
	{ "mul" }, 
	{ "mul.ovf" }, 
	{ "mul.ovf.un" }, 
	{ "neg" }, 
	{ "newarr" }, 
	{ "newobj" }, 
	{ "no." }, 
	{ "nop" }, 
	{ "not" }, 
	{ "or" }, 
	{ "pop" }, 
	{ "readonly." }, 
	{ "refanytype" }, 
	{ "refanyval" }, 
	{ "rem" }, 
	{ "rem.un" }, 
	{ "ret" }, 
	{ "rethrow" }, 
	{ "shl" }, 
	{ "shr" }, 
	{ "shr.un" }, 
	{ "sizeof" }, 
	{ "starg" }, 
	{ "starg.s" }, 
	{ "stelem" }, 
	{ "stelem.i" }, 
	{ "stelem.i1" }, 
	{ "stelem.i2" }, 
	{ "stelem.i4" }, 
	{ "stelem.i8" }, 
	{ "stelem.r4" }, 
	{ "stelem.r8" }, 
	{ "stelem.ref" }, 
	{ "stfld" }, 
	{ "stind.i" }, 
	{ "stind.i1" }, 
	{ "stind.i2" }, 
	{ "stind.i4" }, 
	{ "stind.i8" }, 
	{ "stind.r4" }, 
	{ "stind.r8" }, 
	{ "stind.ref" }, 
	{ "stloc" }, 
	{ "stloc.0" }, 
	{ "stloc.1" }, 
	{ "stloc.2" }, 
	{ "stloc.3" }, 
	{ "stloc.s" }, 
	{ "stobj" }, 
	{ "stsfld" }, 
	{ "sub" }, 
	{ "sub.ovf" }, 
	{ "sub.ovf.un" }, 
	{ "switch" }, 
	{ "tail." }, 
	{ "throw" }, 
	{ "unaligned." }, 
	{ "unbox" }, 
	{ "unbox.any" }, 
	{ "volatile." }, 
	{ "xor" }
};
char msil_bltins[] = " void exit(int); "
    "void __getmainargs(void *,void *,void*,int, void *); "
    "void *__iob_func(); "
    "void *__pctype_func(); "
    "int *_errno(); "
    "void *__OCCMSIL_GetProcThunkToManaged(void *proc); "
    "void *__OCCMSIL_GetProcThunkToUnmanaged(void *proc); ";

/* Init module */
void oa_ini(void)
{
    oa_gentype = nogen;
    oa_currentSeg = noseg;
    oa_outcol = 0;
    newlabel = FALSE;
}

/*-------------------------------------------------------------------------*/

void oa_nl(void)
/*
 * New line
 */
{
    if (cparams.prm_asmfile)
    {
        if (oa_outcol > 0)
        {
            beputc('\n');
            oa_outcol = 0;
            oa_gentype = nogen;
        }
    }
}

/* Put an opcode
 */
void outop(char *name)
{
    beputc('\t');
    while (*name)
        beputc(*name++);
}

/*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/

void oa_putconst(int sz, EXPRESSION *offset, BOOLEAN doSign)
/*
 *      put a constant to the outputFile file.
 */
{
    char buf[4096];
    SYMBOL *sp;
    char buf1[100];
    int toffs;
    switch (offset->type)
    {
        case en_auto:
            if (doSign)
            {
                if ((int)offset->v.sp->offset < 0)
                    bePrintf( "-0%lxh", -offset->v.sp->offset);
                else
                    bePrintf( "+0%lxh", offset->v.sp->offset);
            }
            else
                bePrintf( "0%lxh", offset->v.sp->offset);
                
            break;
        case en_c_i:
        case en_c_l:
        case en_c_ui:
        case en_c_ul:
        case en_c_ll:
        case en_c_ull:
        case en_absolute:
        case en_c_c:
        case en_c_uc:
        case en_c_u16:
        case en_c_u32:
        case en_c_bool:
        case en_c_s:
        case en_c_us:
        case en_c_wc:
            if (doSign)
            {
                if (offset->v.i == 0)
                    break;
                beputc('+');
            }
            {
                int n = offset->v.i;
//				if (sz == ISZ_UCHAR || sz == -ISZ_UCHAR)
//					n &= 0xff;
//				if (sz == ISZ_USHORT || sz == -ISZ_USHORT)
//					n &= 0xffff;
                bePrintf( "%d", n);
            }
            break;
        case en_c_fc:
        case en_c_dc:
        case en_c_ldc:
            if (doSign)
                beputc('+');
            FPFToString(buf,&offset->v.c.r);
            FPFToString(buf1, &offset->v.c.i);
            bePrintf( "%s,%s", buf, buf1);
            break;
        case en_c_f:
        case en_c_d:
        case en_c_ld:
        case en_c_fi:
        case en_c_di:
        case en_c_ldi:
            if (doSign)
                beputc('+');
            FPFToString(buf,&offset->v.f);
            bePrintf( "%s", buf);
            break;
        case en_label:
            if (doSign)
                beputc('+');
            bePrintf( "L_%d", offset->v.sp->label);
            break;
        case en_labcon:
            if (doSign)
                beputc('+');
            bePrintf( "L_%d", offset->v.i);
            break;
        case en_pc:
        case en_global:
        case en_threadlocal:
            if (doSign)
                beputc('+');
            sp = offset->v.sp;
            beDecorateSymName(buf, sp);
            bePrintf( "%s", buf);
            break;
        case en_add:
        case en_structadd:
        case en_arrayadd:
            oa_putconst(ISZ_ADDR, offset->left, doSign);
            oa_putconst(ISZ_ADDR, offset->right, TRUE);
            break;
        case en_sub:
            oa_putconst(ISZ_ADDR, offset->left, doSign);
            bePrintf( "-");
            oa_putconst(ISZ_ADDR, offset->right, FALSE);
            break;
        case en_uminus:
            bePrintf( "-");
            oa_putconst(ISZ_ADDR, offset->left, FALSE);
            break;
        default:
            diag("illegal constant node.");
            break;
    }
}

/*-------------------------------------------------------------------------*/



int islabeled(EXPRESSION *n)
{
    int rv = 0;
    switch (n->type)
    {
        case en_add:
        case en_structadd:
        case en_arrayadd:
        case en_sub:
//        case en_addstruc:
            rv |= islabeled(n->left);
            rv |= islabeled(n->right);
            break;
        case en_c_i:
        case en_c_c:
        case en_c_uc:
        case en_c_u16:
        case en_c_u32:
        case en_c_l:
        case en_c_ul:
        case en_c_ui:
        case en_c_bool:
        case en_c_wc:
        case en_c_s:
        case en_c_us:
            return 0;
        case en_labcon:
        case en_global:
        case en_auto:
        case en_absolute:
        case en_label:
        case en_pc:
        case en_threadlocal:
            return 1;
        default:
            diag("Unexpected node type in islabeled");
            break;
    }
    return rv;
}
void cacheType(TYPE *tp)
{
    LIST *p = beLocalAlloc(sizeof(LIST));
    p->data = tp;
    p->next = typeList;
    typeList = p;
}
void puttype(TYPE *tp)
{
    tp = basetype(tp);
    if (tp->type == bt_pointer)
    {
        if (isfuncptr(tp))
        {
            HASHREC *hr;
            BOOLEAN vararg = FALSE;
            tp = tp->btp;
            hr = basetype(tp)->syms->table[0];
            while (hr)
            {
                SYMBOL *sp = (SYMBOL *)hr->p;
                if (sp->tp->type == bt_ellipse)
                    vararg = TRUE;
                hr = hr->next;
            }
            bePrintf("\t");
            if (vararg)
                bePrintf("vararg ");
            puttypewrapped(isstructured(basetype(tp)->btp) ? &stdpointer : basetype(tp)->btp);
            bePrintf(" *(");
            hr = basetype(tp)->syms->table[0];
            if (isstructured(basetype(tp)->btp))
            {
                bePrintf("void *");
                if (!vararg && hr || vararg && hr && hr->next)
                    bePrintf(", ");
            }
            while (hr)
            {
                SYMBOL *sp = (SYMBOL *)hr->p;
                if (sp->tp->type != bt_void)
                {
                    puttypewrapped(isstructured(sp->tp) || isarray(sp->tp) ? &stdpointer : sp->tp);
                    if (!vararg && hr->next || vararg && hr->next && hr->next->next)
                        bePrintf(", ");
                }
                hr = hr->next;
            }
            bePrintf(")");
        }
        else if (tp->array)
        {
            char buf[1024];
            cacheType(tp);
            buf[0] = 0;
            while (tp->array)
            {
                sprintf(buf + strlen(buf), "[%d]", tp->size/tp->btp->size);
                tp = tp->btp;
            }
            puttype(tp);
            bePrintf("%s", buf);
        }
        else
            bePrintf("void*");
    }
    else if (isstructured(tp))
    {
        cacheType(tp);
        bePrintf("%s", basetype(tp)->sp->name);
    }
    else if (tp->type == bt_void)
        bePrintf("void");
    else if (tp->type == bt_ellipse)
        ;
    else if (tp->type < bt_float_complex)
    {
        static char *names[] = { "", "", "int8", "int8","uint8",
            "int16", "int16", "uint16", "uint16", "int32", "int32", "int32" ,"uint32", "int32", "uint32",
            "int64", "uint64", "float32", "float64", "float64", "float32", "float64", "float64"
        };
        bePrintf(names[tp->type]);
    }
}
void puttypewrapped(TYPE *tp)
{
    if (isfuncptr(tp))
        bePrintf("method ");
    else if (isstructured(tp) || isarray(tp))
        bePrintf("valuetype '");
    puttype(tp);
    if (isstructured(tp) || isarray(tp))
        bePrintf("'");
}
/*-------------------------------------------------------------------------*/
void gen_method_header(SYMBOL *sp, BOOLEAN pinvoke)
{
    BOOLEAN vararg = FALSE;
    HASHREC *hr = basetype(sp->tp)->syms->table[0];
    oa_enterseg(oa_currentSeg);
    bePrintf(".method public hidebysig static ");
    if (pinvoke)
    {
        if (strstr(sp->name, "OCCMSIL_"))
            bePrintf("pinvokeimpl(\"occmsil.dll\" cdecl) ");
        else
            bePrintf("pinvokeimpl(\"msvcrt.dll\" cdecl) ");
    }
    while (hr)
    {
        SYMBOL *sp = (SYMBOL *)hr->p;
        if (sp->tp->type == bt_ellipse)
            vararg = TRUE;
        hr = hr->next;
    }
    if (vararg)
        bePrintf("vararg ");
    puttypewrapped(isstructured(basetype(sp->tp)->btp) ? &stdpointer : basetype(sp->tp)->btp);
    bePrintf(" '%s'", sp->name);
    if (!strcmp(sp->decoratedName, "_main"))
    {
        HASHREC *hr = basetype(sp->tp)->syms->table[0];
        bePrintf("(int32 '");
        bePrintf("%s', ", hr ? hr->p->name : "argc");
        if (hr)
            hr = hr->next;
        bePrintf("void * '");
        bePrintf("%s') ", hr ? hr->p->name : "argv");
    }
    else
    {
        hr = basetype(sp->tp)->syms->table[0];
        bePrintf("(");
        if (isstructured(basetype(sp->tp)->btp))
        {
            bePrintf("void *'retblock'");
            if (!vararg && hr || vararg && hr && hr->next)
                bePrintf(", ");
        }
        while (hr)
        {
            SYMBOL *sp = (SYMBOL *)hr->p;
            if (sp->tp->type != bt_void)
            {
                puttypewrapped(isstructured(sp->tp) || isarray(sp->tp) ? &stdpointer : sp->tp);
                if (!pinvoke)
                {
                    bePrintf(" '%s' ",sp->name);
                }
                if (!vararg && hr->next || vararg && hr->next && hr->next->next)
                    bePrintf(", ");
            }
            hr = hr->next;
        }
        bePrintf(")");
    } 
    if (pinvoke)
        bePrintf(" preservesig {}\n");
    else
        bePrintf(" cil managed\n{\n");
}
void oa_gen_strlab(SYMBOL *sp)
/*
 *      generate a named label.
 */
{
    if (isfunction(sp->tp))
    {
        gen_method_header(sp, FALSE);

    }
    else
    {
        oa_enterseg(oa_currentSeg);
        inASMdata = TRUE;
        bePrintf(".field public static ");
        puttypewrapped(sp->tp);
        bePrintf(" '%s' at $%s\n", sp->name, sp->name);
        bePrintf(".data $%s = bytearray (", sp->name);
        oa_outcol = 0;
    }
}
/*-------------------------------------------------------------------------*/

void oa_put_label(int lab)
/*
 *      outputFile a compiler generated label.
 */
{
    if (cparams.prm_asmfile) {
        oa_nl();
        if (oa_currentSeg == dataseg || oa_currentSeg == bssxseg)
        {
            newlabel = TRUE;
            bePrintf( "\nL_%ld", lab);
            oa_outcol = 8;
        }
        else
            bePrintf( "L_%ld:\n", lab);
    }
}
void oa_put_string_label(int lab, int type)
{
    oa_enterseg(oa_currentSeg);
    inASMdata = TRUE;
    bePrintf(".field public static valuetype '");
    switch (type)
    {
        case l_ustr:
        case l_astr:
            bePrintf("int8[]");
            int8_used = TRUE;
            break;
        case l_Ustr:
            bePrintf("int32[]");
            int32_used = TRUE;
            break;
        case l_wstr:
            bePrintf("int16[]");
            int16_used = TRUE;
            break;
    }
    bePrintf("' 'L_%d' at $L_%d\n", lab, lab);
    bePrintf(".data $L_%d = bytearray (", lab);
    oa_outcol = 0;
}


/*-------------------------------------------------------------------------*/
void oa_genbyte(int bt)
{
    bePrintf("%x ", (unsigned char)bt);
    if (++oa_outcol >= 8)
    {
        oa_outcol = 0;
        bePrintf("\n\t");
    }
}

void oa_genfloat(enum e_gt type, FPF *val)
/*
 * Output a float value
 */
{
    if (cparams.prm_asmfile)
    {
        char buf[256];
        FPFToString(buf,val);
        switch(type) {
            case floatgen:
            {
                UBYTE dta[4];
                int i;
                FPFToFloat(dta, val);
                for (i=0; i < 4; i++)
                {
                    oa_genbyte(dta[i]);
                }
                break;
            }
            case doublegen:
            case longdoublegen:
            {
                UBYTE dta[8];
                int i;
                FPFToDouble(dta, val);
                bePrintf("\tdb\t");
                for (i=0; i < 8; i++)
                {
                    oa_genbyte(dta[i]);
                }
                break;
            }
            default:
                diag("floatgen - invalid type");
                break ;
        }
    }
}
/*-------------------------------------------------------------------------*/

void oa_genstring(LCHAR *str, int len)
/*
 * Generate a string literal
 */
{
    if (cparams.prm_asmfile)
    {
        int nlen = len;
        while (nlen--)
        {
            oa_genbyte(*str++);
        }
    }
}

/*-------------------------------------------------------------------------*/

void oa_genint(enum e_gt type, LLONG_TYPE val)
{
    if (cparams.prm_asmfile) {
        switch (type) {
            case chargen:
                oa_genbyte(val);
                break ;
            case shortgen:
            case u16gen:
                oa_genbyte(val & 0xff);
                oa_genbyte((val >> 8) & 0xff);
                break ;
            case longgen:
            case enumgen:
            case intgen:
            case u32gen:
                oa_genbyte(val & 0xff);
                oa_genbyte((val >> 8) & 0xff);
                oa_genbyte((val >> 16) & 0xff);
                oa_genbyte((val >> 24) & 0xff);
                break ;
            case longlonggen:
                oa_genbyte(val & 0xff);
                oa_genbyte((val >> 8) & 0xff);
                oa_genbyte((val >> 16) & 0xff);
                oa_genbyte((val >> 24) & 0xff);
                oa_genbyte((val >> 32) & 0xff);
                oa_genbyte((val >> 40) & 0xff);
                oa_genbyte((val >> 48) & 0xff);
                oa_genbyte((val >> 56) & 0xff);
                break ;
            case wchar_tgen:
                oa_genbyte(val & 0xff);
                oa_genbyte((val >> 8) & 0xff);
                break ;
            default:
                diag("genint - unknown type");
                break ;
        }
    }
}
void oa_genaddress(ULLONG_TYPE val)
{
    if (cparams.prm_asmfile) {
        oa_genint(intgen, val);
    }
}
/*-------------------------------------------------------------------------*/

void oa_gensrref(SYMBOL *sp, int val)
{
    diag ("oa_gen_srref");
}

/*-------------------------------------------------------------------------*/

void oa_genref(SYMBOL *sp, int offset)
/*
 * Output a reference to the data area (also gens fixups )
 */
{
    diag ("oa_genref");
}

/*-------------------------------------------------------------------------*/

void oa_genpcref(SYMBOL *sp, int offset)
/*
 * Output a reference to the code area (also gens fixups )
 */
{
    diag("oa_genpcref");
}

/*-------------------------------------------------------------------------*/

void oa_genstorage(int nbytes)
/*
 * Output bytes of storage
 */
{
    if (cparams.prm_asmfile)
    {
        int i;
        if (!newlabel)
            oa_nl();
        else
            newlabel = FALSE;
        for (i=0; i < nbytes; i++)
            oa_genbyte(0);
        oa_gentype = nogen;
    }
}

/*-------------------------------------------------------------------------*/

void oa_gen_labref(int n)
/*
 * Generate a reference to a label
 */
{
    diag("oa_gen_labref");
}

/*-------------------------------------------------------------------------*/

void oa_gen_labdifref(int n1, int n2)
{
    diag("oa_gen_labdifref");
}

/*
 * Exit if from a special segment
 */
void oa_exitseg(enum e_sg seg)
{
}

/*
 * Switch to cseg 
 */
void oa_enterseg(enum e_sg seg)
{
    oa_currentSeg = seg ;
    if (inASMdata)
    {
        bePrintf("\n)\n");
        inASMdata = FALSE;
    }
}

/*-------------------------------------------------------------------------*/

    void oa_gen_virtual(SYMBOL *sp, int data)
    {
        virtual_mode = data;
        oa_currentSeg = virtseg;
        if (cparams.prm_asmfile)
        {
            oa_nl();
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
            {
                oa_currentSeg = noseg;
#ifdef IEEE
                if (virtual_mode)
                    bePrintf( "\tsection vsd%s virtual\n", sp->decoratedName);
                else
                    bePrintf( "\tsection vsc%s virtual\n", sp->decoratedName);
#else
                bePrintf( "\tSECTION @%s VIRTUAL\n", sp->decoratedName);
#endif
            }
            else
                bePrintf( "@%s\tsegment virtual\n", sp->decoratedName);
            bePrintf( "%s:\n", sp->decoratedName);
        }
    }
    void oa_gen_endvirtual(SYMBOL *sp)
    {
        if (cparams.prm_asmfile)
        {
            oa_nl();
            if (!(prm_assembler == pa_nasm || prm_assembler == pa_fasm))
            {
                bePrintf( "@%s\tends\n", sp->decoratedName);
            }
            else
                if (virtual_mode)
                    oa_enterseg(dataseg);
                else
                    oa_enterseg(codeseg);
        }
        else
        {
            if (virtual_mode)
                oa_enterseg(dataseg);
            else
                oa_enterseg(codeseg);
        }
    }
/*
 * Align
 * not really honorign the size... all alignments are mod 4...
 */
void oa_align(int size)
{
    if (cparams.prm_asmfile)
    {
        oa_nl();
        if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
        /* NASM 0.91 wouldn't let me use parenthesis but this should work
         * according to the documented precedence levels
         */
            bePrintf( "\ttimes $$-$ & %d nop\n", size-1);
        else
            bePrintf( "\talign\t%d\n", size);
    }
}


/*-------------------------------------------------------------------------*/

void dump_browsedata(BROWSEINFO *bri)
{
//    if (!cparams.prm_asmfile)
//        omf_dump_browsedata(bri);
}
void dump_browsefile(BROWSEFILE *brf)
{
//    if (!cparams.prm_asmfile)
//        omf_dump_browsefile(brf);
}

/*-------------------------------------------------------------------------*/

void oa_header(char *filename, char *compiler_version)
{
    oa_nl();
    bePrintf("//File %s\n",filename);
    bePrintf("//Compiler version %s\n",compiler_version);
    bePrintf("\n.corflags 2 // 32-bit");
    bePrintf("\n.assembly test { }\n");
    bePrintf("\n.assembly extern mscorlib { }\n\n\n");
}
void oa_trailer(void)
{
}
/*-------------------------------------------------------------------------*/
void oa_localdef(SYMBOL *sp)
{
    if (!cparams.prm_asmfile)
    {
//        omf_globaldef(sp);
    }
}
void oa_localstaticdef(SYMBOL *sp)
{
    if (!cparams.prm_asmfile)
    {
//        omf_globaldef(sp);
    }
}
void oa_globaldef(SYMBOL *sp)
{
}

/*-------------------------------------------------------------------------*/

void oa_output_alias(char *name, char *alias)
{
    if (cparams.prm_asmfile)
    {
        oa_nl();
        if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
            bePrintf( "%%define %s %s\n", name, alias);
        else
            bePrintf( "%s equ\t<%s>\n", name, alias);
    }
}


/*-------------------------------------------------------------------------*/

void put_pinvoke(SYMBOL *sp)
{
    gen_method_header(sp, TRUE);
}
void oa_put_extern(SYMBOL *sp, int code)
{
    if (isfunction(sp->tp))
        put_pinvoke(sp);
    else if (cparams.prm_asmfile) {
        if (strcmp(sp->name, "_pctype") && strcmp(sp->name, "__stdin") && strcmp(sp->name, "__stdout") && strcmp(sp->name, "__stderr"))
        {
            fatal ("Undefined external %s\n", sp->name);
        }
    }
}
/*-------------------------------------------------------------------------*/

void oa_put_impfunc(SYMBOL *sp, char *file)
{
    if (cparams.prm_asmfile) 
    {
        bePrintf( "\timport %s %s\n", sp->decoratedName, file);
    } 
}

/*-------------------------------------------------------------------------*/

void oa_put_expfunc(SYMBOL *sp)
{
    char buf[4096];
    if (cparams.prm_asmfile) {
        beDecorateSymName(buf, sp);
        if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
            bePrintf( "\texport %s\n", buf);
        else
            bePrintf( "\tpublicdll %s\n", buf);
    }
}

void oa_output_includelib(char *name)
{
    if (cparams.prm_asmfile)
    {
        if (!(prm_assembler == pa_nasm || prm_assembler == pa_fasm))
            bePrintf( "\tincludelib %s\n", name);
    }
}
void putfieldname(AMODE *arg)
{
    EXPRESSION *en = GetSymRef(arg->offset);
    bePrintf("\t");
    puttypewrapped(en->v.sp->tp);
    bePrintf(" '%s'\n", en->v.sp->name);
}
void putfunccall(AMODE *arg)
{
    EXPRESSION *en = GetSymRef(arg->offset);
    SYMBOL *sp = en->v.sp;
    HASHREC *hr = basetype(sp->tp)->syms->table[0];
    BOOLEAN vararg = FALSE;
    INITLIST *il = arg->altdata ? ((FUNCTIONCALL *)arg->altdata)->arguments : NULL;
    while (hr)
    {
        SYMBOL *sp = (SYMBOL *)hr->p;
        if (sp->tp->type == bt_ellipse)
            vararg = TRUE;
        hr = hr->next;
    }
    bePrintf("\t");
    if (vararg)
        bePrintf("vararg ");
    puttypewrapped(isstructured(basetype(sp->tp)->btp) ? &stdpointer : basetype(sp->tp)->btp);
    bePrintf(" '%s'", sp->name);
    bePrintf("(");
    hr = basetype(sp->tp)->syms->table[0];
    if (isstructured(basetype(sp->tp)->btp))
    {
        bePrintf("void *");
        if (!vararg && hr || vararg && hr && hr->next)
            bePrintf(", ");
    }
    while (hr)
    {
        SYMBOL *sp = (SYMBOL *)hr->p;
        if (sp->tp->type != bt_void)
        {
            puttypewrapped(isstructured(sp->tp) || isarray(sp->tp) ? &stdpointer : sp->tp);
            if (il && sp->tp->type != bt_ellipse)
                il = il->next;
            if (!vararg && hr->next || vararg && hr->next && hr->next->next)
                bePrintf(", ");
        }
        hr = hr->next;
    }
    if (vararg)
    {
        bePrintf(", ...");
        while (il)
        {
            bePrintf(", ");
            if (isarray(il->tp))
                puttypewrapped(&stdpointer); // convert arrays to void *...
            else
                puttypewrapped(il->tp);
            il = il->next;
        }
    }
    bePrintf(")");
}
void putlocals(void)
{
    HASHTABLE *temp = theCurrentFunc->inlineFunc.syms;
    LIST *lst = NULL;
    while (temp)
    {
        HASHREC *hr = temp->table[0];
        while (hr)
        {
            SYMBOL *sym = (SYMBOL *)hr->p;
            if (sym->storage_class != sc_parameter)
                break;
            hr = hr->next;
        }
        if (hr)
            break;
        temp = temp->next;
    }
    if (!temp)
    {
        lst = temporarySymbols;
        while (lst)
        {
            SYMBOL *sym = (SYMBOL *)lst->data;
            if (!sym->anonymous)
            {
                break;
            }
            lst = lst->next;
        }
    }
    if (lst || temp)
    {
        BOOLEAN first = TRUE;
        while (temp)
        {
            HASHREC *hr = temp->table[0];
            while (hr)
            {
                SYMBOL *sym = (SYMBOL *)hr->p;
                sym->temp = FALSE;
                hr = hr->next;
            }
            temp = temp->next;
        }
        lst = temporarySymbols;
        while (lst)
        {
            SYMBOL *sym = (SYMBOL *)lst->data;
            sym->temp = FALSE;
            lst = lst->next;
        }
        bePrintf("\t.locals\n\t(\n");
        temp = theCurrentFunc->inlineFunc.syms;
        while (temp)
        {
            HASHREC *hr = temp->table[0];
            while (hr)
            {
                SYMBOL *sym = (SYMBOL *)hr->p;
                if (sym->storage_class != sc_parameter && !sym->temp)
                {
                    sym->temp = TRUE;
                    if (!first)
                        bePrintf(",\n");
                    first = FALSE;
                    bePrintf("\t\t[%d] ",sym->offset);
                    puttypewrapped(sym->tp);
                    bePrintf(" '%s/%d'", sym->name, sym->offset);
                }
                hr = hr->next;
            }
            temp = temp->next;
        }
        lst = temporarySymbols;
        while (lst)
        {
            SYMBOL *sym = (SYMBOL *)lst->data;
            if (!sym->anonymous && !sym->temp)
            {
                sym->temp= TRUE;
                if (!first)
                    bePrintf(",\n");
                first = FALSE;
                bePrintf("\t\t[%d] ",sym->offset);
               puttypewrapped(sym->tp);
                    bePrintf(" '%s/%d'", sym->name, sym->offset);
            }
            lst = lst->next;
        }
        bePrintf("\n\t)\n");
    }
}
void putarg(AMODE *arg)
{
    switch (arg->mode)
    {
        int i;
        struct swlist *pass;
        case am_immed:
            if (arg->offset->type == en_labcon)
            {
                if (arg->offset->altdata)
                {
                    bePrintf("\tvaluetype '");
                    switch (arg->offset->altdata)
                    {
                        case l_ustr:
                        case l_astr:
                            bePrintf("int8[]");
                            break;
                        case l_Ustr:
                            bePrintf("int32[]");
                            break;
                        case l_wstr:
                            bePrintf("int16[]");
                            break;
                    }
                    bePrintf("' 'L_%Ld'\n", arg->offset->v.i);
                }
                else
                {
                    bePrintf("\t'L_%Ld'", arg->offset->v.i);
                }
            }
            else if (arg->offset->type == en_pc || arg->offset->type == en_global)
                if (isfunction(arg->offset->v.sp->tp))
                {
                    putfunccall(arg);
                }
                else {
                    TYPE *tp1 = arg->offset->v.sp->tp;
                    while (isarray(tp1))
                        tp1 = basetype(tp1)->btp;
                    if (arg->offset->type == en_pc /*calli */ && isfuncptr(tp1))
                    {
                        bePrintf("\t");
                        puttype(tp1);
                    }
                    else
                    {
                        bePrintf("\t");
                        puttypewrapped(arg->offset->v.sp->tp);
                        bePrintf(" '%s'\n", arg->offset->v.sp->name);
                    }
                }
            else if (isfloatconst(arg->offset))
            {
                char buf[256];
                FPFToString(buf,&arg->offset->v.f);
                bePrintf("\t%s", buf);
            }
            else
            {
                bePrintf("\t%d", arg->offset->v.i);
            }
            break; 
        case am_local:
            bePrintf("\t'%s/%d'", ((SYMBOL *)arg->altdata)->name, arg->index);
            break;
        case am_param:
            bePrintf("\t'%s'", ((SYMBOL *)arg->altdata)->name);
            break;
        case am_global:
            putfieldname(arg);
            break;
        case am_switch:
            bePrintf("\t( ");
            pass = arg->switches;
            i = 0;
            while (pass)
            {
                bePrintf("L_%d ", pass->lab);
                if (pass->next);
                {
                    bePrintf(", ");
                    if (++i %8 == 0)
                        bePrintf("\n\t");
                }
                pass = pass->next;
            }
            bePrintf("\n\t)");
            break;
    }
}
void oa_put_code(OCODE *ocode)
{
    enum e_op op = ocode->opcode;
    if (op == op_blockstart || op == op_blockend || op == op_varstart || op == op_funcstart || op == op_funcend)
        return;
    if (op == op_line)
    {
        LINEDATA *ld = (LINEDATA *)ocode->oper1;
        oa_nl();
        while (ld)
        {
            bePrintf( "// Line %d:\t%s\n", ld->lineno, ld->line);
            ld = ld->next;
        }
        return ;
    }
    else if (op == op_comment)
    {
        if (!cparams.prm_lines)
            return ;
        bePrintf( "%s", ocode->oper1);
        return ;
    }
    else if (op == op_void)
        return ;
    if (op == op_maxstack)
    {
        putlocals();
    }
    bePrintf("\t%s", instructions[op].name);
    if (ocode->oper1)
        putarg(ocode->oper1);
    bePrintf("\n");
}
void dumpTypes()
{
    LIST *lst = typeList;
    LIST **pList = &lst;
    typeList = NULL;

    if (int8_used)
    {
        bePrintf(".class private value explicit ansi sealed 'int8[]' {.pack 1 .size 1}\n");
    }
    if (int16_used)
    {
        bePrintf(".class private value explicit ansi sealed 'int16[]' {.pack 2 .size 1}\n");
    }
    if (int32_used)
    {
        bePrintf(".class private value explicit ansi sealed 'int32[]' {.pack 4 .size 1}\n");
    }
    while (*pList)
    {
        // weed
        LIST **qList = &(*pList)->next;
        TYPE *tp = ((TYPE *)(*pList)->data);
        while (*qList)
        {
            TYPE *tpq = (TYPE *)(*qList)->data;
            TYPE *tpp = ((TYPE *)(*pList)->data);
            if (comparetypes((*qList)->data, tpp, TRUE))
                if (!tpq->array || !tpp->array)
                {
                    *qList = (*qList)->next;
                }
                else
                {
                    while (tpq && tpp)
                    {
                        if (tpq->size != tpp->size)
                            break;
                        tpq = basetype(tpq)->btp;
                        tpp = basetype(tpp)->btp;
                    }
                    if (tpq || tpp)
                    {
                        qList = &(*qList)->next;
                    }
                    else
                    {
                        *qList = (*qList)->next;
                    }
                }
            else
            {
                qList = &(*qList)->next;
            }
        }
        bePrintf(".class private value explicit ansi sealed '");
        puttype(tp);
        bePrintf("' {.pack 1 .size %d}\n", tp->size ? tp->size : 1);
        pList = &(*pList)->next;
    }

    typeList = NULL;
}
void oa_load_funcs(void)
{
    SYMBOL *sp;
    sp = gsearch("exit");
    if (sp)
        ((SYMBOL *)sp->tp->syms->table[0]->p)->genreffed = TRUE;
    sp = gsearch("__getmainargs");
    if (sp)
        ((SYMBOL *)sp->tp->syms->table[0]->p)->genreffed = TRUE;
    sp = gsearch("__pctype_func");
    if (sp)
        ((SYMBOL *)sp->tp->syms->table[0]->p)->genreffed = TRUE;
    sp = gsearch("__iob_func");
    if (sp)
        ((SYMBOL *)sp->tp->syms->table[0]->p)->genreffed = TRUE;
    sp = gsearch("_errno");
    if (sp)
        ((SYMBOL *)sp->tp->syms->table[0]->p)->genreffed = TRUE;
    sp = gsearch("__GetErrno");
    if (sp)
        ((SYMBOL *)sp->tp->syms->table[0]->p)->genreffed = FALSE;
    oa_enterseg(oa_currentSeg);
    bePrintf("\n\t.field public static void *'__stdin'\n");
    bePrintf("\n\t.field public static void *'__stdout'\n");
    bePrintf("\n\t.field public static void *'__stderr'\n");
    bePrintf("\n\t.field public static void *'_pctype'\n");
}
void oa_end_generation(void)
{
    SYMBOL *start = NULL, *end = NULL;
    LIST *externalList = externals;
    oa_load_funcs();
    oa_enterseg(oa_currentSeg);
    while (externalList)
    {
        SYMBOL *sym = (SYMBOL *)externalList->data;
        if (!strncmp(sym->name, "__DYNAMIC", 9))
        {
            if (strstr(sym->name, "STARTUP"))
                start = sym;
            else
                end = sym;
        }
        externalList = externalList->next;
    }
    bePrintf(".method public hidebysig static void * __GetErrno() cil managed {\n");
    bePrintf("\t.maxstack 1\n\n");
    bePrintf("\tcall void * '_errno'()\n");
    bePrintf("\tret\n");
    bePrintf("}\n");

    bePrintf(".method public hidebysig static void $Main() cil managed {\n");
    bePrintf("\t.entrypoint\n");
    bePrintf("\t.locals (\n");
    bePrintf("\t\t[0] int32 'argc',\n");
    bePrintf("\t\t[1] void * 'argv',\n");
    bePrintf("\t\t[2] void * 'environ',\n");
    bePrintf("\t\t[3] void * 'newmode'\n");
    bePrintf("\t)\n");
    bePrintf("\t.maxstack 5\n\n");
    bePrintf("\tcall void *'__pctype_func'()\n");
    bePrintf("\tstsfld void * '_pctype'\n");
    bePrintf("\tcall void *'__iob_func'()\n");
    bePrintf("\tdup\n");
    bePrintf("\tstsfld void * '__stdin'\n");
    bePrintf("\tdup\n");
    bePrintf("\tldc.i4 32\n");
    bePrintf("\tadd\n");
    bePrintf("\tstsfld void * '__stdout'\n");
    bePrintf("\tldc.i4 64\n");
    bePrintf("\tadd\n");
    bePrintf("\tstsfld void * '__stderr'\n");
    if (start)
        bePrintf("\tcall void %s()\n", start->name);
    bePrintf("\tldloca 'argc'\n");
    bePrintf("\tldloca 'argv'\n");
    bePrintf("\tldloca 'environ'\n");
    bePrintf("\tldc.i4  0\n");
    bePrintf("\tldloca 'newmode'\n");
    bePrintf("\tcall void __getmainargs(void *, void *, void *, int32, void *);\n");
    bePrintf("\tldloc 'argc'\n");
    bePrintf("\tldloc 'argv'\n");
    bePrintf("\tcall int32 'main'(int32, void *)\n");
    if (end)
        bePrintf("\tcall void %s()\n", end->name);
    bePrintf("\tcall void exit(int32)\n");
    bePrintf("\tret\n");
    bePrintf("}\n");
//    bePrintf(".method public hidebysig static pinvokeimpl(\"msvcrt.dll\" cdecl) void exit(int32) preservesig {}\n");
//    bePrintf(".method public hidebysig static pinvokeimpl(\"msvcrt.dll\" cdecl) void __getmainargs(void *,void *,void*,int32, void *) {}\n");

    dumpTypes();    
}
void flush_peep(SYMBOL *funcsp, QUAD *list)
{
    (void)funcsp;
    (void) list;
    if (cparams.prm_asmfile)
    {
        while (peep_head != 0)
        {
            switch (peep_head->opcode)
            {
                case op_label:
                    oa_put_label((int)peep_head->oper1);
                    break;
                case op_funclabel:
                    oa_gen_strlab((SYMBOL *)peep_head->oper1);
                    break;
                default:
                    oa_put_code(peep_head);
                    break;

            }
            peep_head = peep_head->fwd;
        }
    }
    bePrintf("\n}\n ");
    peep_head = 0;

}
