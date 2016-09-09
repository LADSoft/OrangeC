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
#include "winmode.h"

#define IEEE


extern int prm_targettype;
extern int prm_assembler;
extern SYMBOL *theCurrentFunc;
extern LIST *temporarySymbols;
extern LIST *externals;
extern TYPE stdpointer, stdint;
extern INCLUDES *includes;
extern char namespaceAndClass[512];
extern char namespaceAndClassForNestedType[512];
OCODE *peep_head, *peep_tail;
static int uses_float;

void puttypewrapped(TYPE *tp);

LIST *typeList;
LIST *enumList;
static enum e_gt oa_gentype = nogen; /* Current DC type */
enum e_sg oa_currentSeg = noseg; /* Current seg */
static int oa_outcol = 0; /* Curront col (roughly) */
int newlabel;
static int virtual_mode;
static int corflags = 2; // x86

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
    "void *__OCCMSIL_GetProcThunkToUnmanaged(void *proc); "
    "void *malloc(unsigned); "
    "void free(void *); "
    "void *__va_start__(); "
    "void *__va_arg__(void *, ...); ";

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
void puttype(TYPE *tp_in)
{
    TYPE *tp = basetype(tp_in);
    if (tp->type == bt_pointer || isfunction(tp))
    {
        if (isfuncptr(tp) || isfunction(tp))
        {
            HASHREC *hr;
            BOOLEAN vararg = FALSE;
            if (isfuncptr(tp))
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
                if (tp->vla)
                {
                    sprintf(buf + strlen(buf), "[vla]");
                }
                else
                {
                    sprintf(buf + strlen(buf), "[%d]", tp->size/tp->btp->size);
                }
                tp = tp->btp;
            }
            if (namespaceAndClass[0] && !isstructured(tp))
            {
                bePrintf("%s", namespaceAndClassForNestedType);
            }
            puttype(tp);
            bePrintf("%s", buf);
        }
        else
        {
            while (tp_in->type != bt_pointer)
            {
                if (tp_in->type == bt_va_list)
                {
                    bePrintf("class [lsmsilcrtl]lsmsilcrtl.args ");
                    return;
                }
                else if (tp_in->type == bt_objectArray)
                {
                    bePrintf("object[] ");
                    return;
                }
                tp_in = tp_in->btp;
            }
            bePrintf("void*");
        }
    }
    else if (isstructured(tp))
    {
        cacheType(tp);
        if (namespaceAndClass[0])
        {
            bePrintf("%s%s", namespaceAndClassForNestedType, basetype(tp)->sp->name);
        }
        else
        {
            bePrintf("%s", basetype(tp)->sp->name);
        }
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
    if (isfuncptr(tp) || isfunction(tp))
        bePrintf("method ");
    else if (isstructured(tp) || isarray(tp))
        bePrintf("valuetype '");
    puttype(tp);
    if (isstructured(tp) || isarray(tp))
        bePrintf("'");
}
void putunmanagedtype(TYPE *tp)
{
    TYPE *tp1 = tp;
    while (tp1 && tp1->type != bt_pointer)
    {
        if (tp1->type == bt_va_list)
        {
            bePrintf("void*");
            return;
        }
        tp1 = tp1->btp;
    }
    puttypewrapped(tp);
}
/*-------------------------------------------------------------------------*/
void gen_method_header(SYMBOL *sp, BOOLEAN pinvoke)
{
    BOOLEAN vararg = FALSE;
    HASHREC *hr = basetype(sp->tp)->syms->table[0];
    int count = 0;
    oa_enterseg(oa_currentSeg);
    if (sp->storage_class == sc_static)
    {
        bePrintf(".method private hidebysig static ");
    }
    else
    {
        bePrintf(".method public hidebysig static ");
    }
    if (pinvoke)
    {
        char * name = _dll_name(sp->name);
        if (name)
        {
            bePrintf("pinvokeimpl(\"%s\" %s) ", name, sp->linkage == lk_stdcall ? "stdcall" : "cdecl");
        }
        else
        {
            errorsym(ERR_UNDEFINED_EXTERNAL, sp);
        }
    }
    while (hr)
    {
        SYMBOL *sp = (SYMBOL *)hr->p;
        if (sp->tp->type == bt_ellipse)
            vararg = TRUE;
        hr = hr->next;
    }
    if (vararg && pinvoke)
        bePrintf("vararg ");
    if (!strcmp(sp->decoratedName, "_main"))
    {
        HASHREC *hr = basetype(sp->tp)->syms->table[0];
        if (isvoid(basetype(sp->tp)->btp))
            bePrintf("void ");
        else
            puttypewrapped(&stdint);
        bePrintf(" '%s'", sp->name);
        bePrintf("(int32 '");
        bePrintf("%s', ", hr ? hr->p->name : "argc");
        if (hr)
            hr = hr->next;
        bePrintf("void * '");
        bePrintf("%s') ", hr ? hr->p->name : "argv");
    }
    else
    {
        puttypewrapped(isstructured(basetype(sp->tp)->btp) ? &stdpointer : basetype(sp->tp)->btp);
        bePrintf(" '%s'", sp->name);
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
                if (pinvoke)
                    putunmanagedtype(isstructured(sp->tp) || isarray(sp->tp) ? &stdpointer : sp->tp);
                else
                    puttypewrapped(isstructured(sp->tp) || isarray(sp->tp) ? &stdpointer : sp->tp);
                if (!pinvoke && sp->tp->type != bt_ellipse)
                {
                    bePrintf(" '%s' ",sp->name);
                    count++;
                }
                if (!vararg && hr->next || vararg && hr->next && hr->next->next)
                    bePrintf(", ");
            }
            hr = hr->next;
        }
        if (vararg && !pinvoke)
        {
            count ++;
            bePrintf(", object [] '__varargs__'");
        }
        bePrintf(")");
    } 
    if (pinvoke)
        bePrintf(" preservesig {}\n");
    else
        bePrintf(" cil managed\n{\n");
    if (vararg && !pinvoke)
    {
        bePrintf("\t.param\t[%d]\n", count);
        bePrintf("\t.custom instance void [mscorlib]System.ParamArrayAttribute::.ctor() = ( 01 00 00 00 )\n"); 
    }
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
//        inASMdata = TRUE;
        if (sp->storage_class == sc_localstatic || sp->storage_class == sc_constant)
            bePrintf(".field private static ");
        else
            bePrintf(".field public static ");
        puttypewrapped(sp->tp);
        if (sp->storage_class == sc_localstatic || sp->storage_class == sc_constant)
        {
            bePrintf(" 'L_%d'\n", sp->label);
//            bePrintf(" 'L_%d' at $L_%d\n", sp->label, sp->label);
//            bePrintf(".data $L_%d = bytearray (", sp->label);
        }
        else
        {
            bePrintf(" '%s'\n", sp->name);
//            bePrintf(" '%s' at $%s\n", sp->name, sp->name);
//            bePrintf(".data $%s = bytearray (", sp->name);
        }
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
    if (namespaceAndClass[0])
    {
        bePrintf("%s", namespaceAndClassForNestedType);
    }
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
//    bePrintf("' 'L_%d'\n", lab);
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
    /*
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
    */
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
    diag("oa_align");
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
    _apply_global_using();
    if (namespaceAndClass[0])
        corflags |= 1; // accessible assembly (ILONLY)

    oa_nl();
    bePrintf("//File %s\n",filename);
    bePrintf("//Compiler version %s\n",compiler_version);
    bePrintf("\n.corflags %d // 32-bit", corflags);
    {
        char *p = strrchr(filename, '.');
        if (p)
        {
            char *q = strrchr(filename, '\\');
            if (!q)
                q = filename;
            else
                q++;
            *p = 0;
            bePrintf("\n.assembly %s { }\n",q);
            *p++ = '.';
        }
        else
        {
            bePrintf("\n.assembly %s { }\n",filename);
        }
    }
    bePrintf("\n.assembly extern mscorlib { }\n");
    bePrintf("\n.assembly extern lsmsilcrtl { }\n\n\n");
    if (namespaceAndClass[0])
    {
        char *p = strchr(namespaceAndClass, '.'), *q;
        if (p)
        {
            *p = 0;
            bePrintf("\n.namespace '%s' { \n", namespaceAndClass);
            *p++ = '.';

            q = strchr(p, ':');
            if (q)
            {
                *q = 0;
                bePrintf("\n.class public explicit ansi sealed '%s' { \n", p);
                *q = ':';
            }
        }
    }
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
    {
        if (sp->linkage2 != lk_msil_rtl)
            put_pinvoke(sp);
    }
    else {
        if (cparams.prm_asmfile) {
            if (strcmp(sp->name, "_pctype") && strcmp(sp->name, "__stdin") && strcmp(sp->name, "__stdout") && strcmp(sp->name, "__stderr"))
            {
                fatal ("Undefined external %s\n", sp->name);
            }
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
    if (!namespaceAndClass[0])
        bePrintf("'");
    else
        bePrintf(" ");
    if (en->v.sp->storage_class == sc_localstatic || en->v.sp->storage_class == sc_constant)
        bePrintf("%sL_%d", namespaceAndClass, en->v.sp->label);
    else
        bePrintf("%s%s", namespaceAndClass, en->v.sp->name);
    if (!namespaceAndClass[0])
        bePrintf("'");
}
void putfunccall(AMODE *arg)
{
    EXPRESSION *en = GetSymRef(arg->offset);
    SYMBOL *spi = en->v.sp;
    HASHREC *hr = basetype(spi->tp)->syms->table[0];
    BOOLEAN vararg = FALSE;
    BOOLEAN managed = msil_managed(spi);
    INITLIST *il = arg->altdata ? ((FUNCTIONCALL *)arg->altdata)->arguments : NULL;
    while (hr)
    {
        SYMBOL *sp = (SYMBOL *)hr->p;
        if (sp->tp->type == bt_ellipse)
            vararg = TRUE;
        hr = hr->next;
    }
    bePrintf("\t");
    if (vararg && !managed)
        bePrintf("vararg ");
    puttypewrapped(isstructured(basetype(spi->tp)->btp) ? &stdpointer : basetype(spi->tp)->btp);
    if (managed)
        if (spi->linkage2 == lk_msil_rtl)
        {
                bePrintf(" [lsmsilcrtl]lsmsilcrtl.rtl::%s", spi->name);
        }
        else
        {
            if (namespaceAndClass[0])
                bePrintf(" %s%s", namespaceAndClass, spi->name);  
            else
                bePrintf(" '%s'", spi->name);
        }
    else
        bePrintf(" '%s'", spi->name);
    bePrintf("(");
    hr = basetype(spi->tp)->syms->table[0];
    if (isstructured(basetype(spi->tp)->btp))
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
            if (!msil_managed(spi) )
                putunmanagedtype(isstructured(sp->tp) || isarray(sp->tp) ? &stdpointer : sp->tp);
            else
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
        if (managed)
        {
            bePrintf(", object[]");
        }
        else
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
            if (sym->storage_class == sc_auto || sym->storage_class == sc_register)
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
                if ((sym->storage_class == sc_auto || sym->storage_class == sc_register) && !sym->temp)
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
void putarg(enum e_op op, AMODE *arg)
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
                    if (namespaceAndClass[0])
                    {
                        bePrintf("%s", namespaceAndClassForNestedType);
                    }
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
                    if (op == op_ldsflda || op == op_ldsfld || op == op_stsfld)
                        bePrintf("' %sL_%Ld\n", namespaceAndClass, arg->offset->v.i);
                    else
                        bePrintf("' 'L_%Ld'\n", arg->offset->v.i);
                }
                else
                {
                    if (op == op_ldsflda || op == op_ldsfld || op == op_stsfld)
                        bePrintf(" %sL_%Ld\n", namespaceAndClass, arg->offset->v.i);
                    else
                        bePrintf(" 'L_%Ld'\n", arg->offset->v.i);
                }
            }
            else if (arg->offset->type == en_pc || arg->offset->type == en_global || arg->offset->type == en_label)
                if (op != op_calli && isfunction(arg->offset->v.sp->tp))
                {
                    putfunccall(arg);
                }
                else {
                    TYPE *tp1 = arg->offset->v.sp->tp;
                    HASHREC *hr;
                    while (isarray(tp1))
                        tp1 = basetype(tp1)->btp;
                    if (op == op_calli)
                    {
                        bePrintf("\t");
                        if (isfuncptr(tp1))
                            tp1 = basetype(tp1)->btp;
                        puttypewrapped(isstructured(basetype(tp1)->btp) ? &stdpointer : basetype(tp1)->btp);
                        bePrintf(" (");
                        hr = basetype(tp1)->syms->table[0];
                        if (isstructured(basetype(tp1)->btp))
                        {
                            bePrintf("void *");
                            if (hr)
                                bePrintf(", ");
                        }
                        while (hr)
                        {
                            SYMBOL *sp = (SYMBOL *)hr->p;
                            if (sp->tp->type != bt_void)
                            {
                                puttypewrapped(isstructured(sp->tp) || isarray(sp->tp) ? &stdpointer : sp->tp);
                                if (hr->next)
                                    bePrintf(", ");
                            }
                            hr = hr->next;
                        }
                        bePrintf(")");
                    }
                    else
                    {
                        bePrintf("\t");
                        puttypewrapped(arg->offset->v.sp->tp);
                        if (arg->offset->type == en_label)
                        {
                            bePrintf(" %sL_%d\n", namespaceAndClass, arg->offset->v.sp->label);
                        }
                        else
                        {
                            if (namespaceAndClass[0])
                                bePrintf(" %s%s\n", namespaceAndClass, arg->offset->v.sp->name);
                            else
                                bePrintf(" '%s'\n", arg->offset->v.sp->name);
                        }
                    }
                }
            else if (isfloatconst(arg->offset))
            {
                char buf[256];
                FPFToString(buf,&arg->offset->v.f);
                if (!strcmp(buf,"inf") || !strcmp(buf, "nan")
                    || !strcmp(buf,"-inf") || !strcmp(buf, "-nan"))
                {
                    UBYTE dta[8];
                    int count = arg->offset->type == en_c_f ? 4 : 8;
                    int i;
                    if (count == 4)
                        FPFToFloat(dta, &arg->offset->v.f);
                    else
                        FPFToDouble(dta, &arg->offset->v.f);
                    bePrintf("\t(");
                    for (i=0; i < count; i++)
                    {
                        bePrintf( "%02X ", dta[i]);
                    }
                    bePrintf(")");
                }
                else
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
        case am_argit_args:
            bePrintf("\t'__varargs__'");
            break;
        case am_argit_ctor:
            bePrintf("\tinstance void [lsmsilcrtl]lsmsilcrtl.args::.ctor(object[])");
            break;
        case am_argit_getnext:
            bePrintf("\tinstance object [lsmsilcrtl]lsmsilcrtl.args::GetNextArg()");
            break;
        case am_argit_unmanaged:
            bePrintf("\tinstance void *[lsmsilcrtl]lsmsilcrtl.args::GetUnmanaged()");
            break;
        case am_ptrbox:
            bePrintf("\tobject [lsmsilcrtl]lsmsilcrtl.pointer::'box'(void *)");
            break;
        case am_ptrunbox:
            bePrintf("\tvoid * [lsmsilcrtl]lsmsilcrtl.pointer::'unbox'(object)");
            break;
        case am_type:
        {
            TYPE *tp = arg->offset->v.sp->tp;
            bePrintf("\t");
            puttypewrapped(tp);
        }
            break;
        case am_sized:
        {
            static char *names[] = { "", "", "UInt8", "UInt8",
                "UInt16", "UInt16", "UInt16", "UInt32", "UInt32", "UInt32",
                "UInt64", "VoidPtr", "VoidPtr", "UInt16", "UInt16", "Float", "Double", 
                "Double", "Float", "Double", "Double"
            };
            static char *mnames[] = { "", "", "Int8", "Int8",
                "Int16", "Int16", "Int16", "Int32", "Int32", "Int32",
                "Int64", "VoidPtr", "VoidPtr", "Int16", "Int16", "Float", "Double", 
                "Double", "Float", "Double", "Double"
            };
            bePrintf("\t");
            bePrintf("[mscorlib]System.");
            if (arg->length< 0)
                bePrintf(mnames[-arg->length]);
            else
                bePrintf(names[arg->length]);
        }
            break;
        case am_objectArray:
            bePrintf("\tobject[] ");
            break;
        case am_objectArray_ctor:
            bePrintf("\[mscorlib]System.Object ");
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
        putarg(op,ocode->oper1);
    bePrintf("\n");
}
// weed out unions, structures with nested structures or bit fields
static BOOLEAN qualifiedStruct(SYMBOL *sp)
{
    HASHREC *hr = basetype(sp->tp)->syms->table[0];
    if (basetype(sp->tp)->type == bt_union)
        return FALSE;
    while (hr)
    {
        SYMBOL *check = (SYMBOL *)hr->p;
        if (basetype(check->tp)->bits)
            return FALSE;
        if (isstructured(check->tp))
            return FALSE;
        if (basetype(check->tp)->array)
            return FALSE;
        hr = hr->next;
    }
    return TRUE;

}
void DumpClassFields(SYMBOL *sp)
{
    HASHREC *hr = basetype(sp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *check = (SYMBOL *)hr->p;
        bePrintf("\n\t.field public ");
        if (isfunction(check->tp) || isfuncptr(check->tp))
            bePrintf("void *");
        else
            puttype(check->tp);
        bePrintf(" '%s'", check->name);
        hr = hr->next;
    }
    bePrintf("\n");
}
void oa_enter_type(SYMBOL *sp)
{
    if (namespaceAndClass[0] && sp->tp->type == bt_enum && !strstr(sp->name, "__anontype"))
    {
        LIST *enumV = beLocalAlloc(sizeof(LIST));
        enumV->data = sp;
        enumV->next = enumList;
        enumList = enumV;
    }
}
static void dumpEnums(void)
{
    int l = strlen(namespaceAndClass);
    if (l)
        namespaceAndClass[l-2] = 0;
    while (enumList)
    {
        SYMBOL *sp = (SYMBOL *)enumList->data;
        if (sp->tp->syms->table[0])
        {
            HASHREC *hr = sp->tp->syms->table[0];
            bePrintf(".class enum nested public auto ansi sealed '%s' {\n", sp->name);
            while (hr)
            {
                SYMBOL *spe = (SYMBOL *)hr->p;
                bePrintf("\t.field public static literal valuetype %s/%s %s = int32(%d)\n",
                        namespaceAndClass, sp->name, spe->name, spe->value.i);
                hr = hr->next;
            }
            bePrintf("\t.field public specialname rtspecialname int32 value__\n");
            bePrintf("}\n");
        }
        enumList = enumList->next;
    }
    if (l)
        namespaceAndClass[l-2] = ':';
}
void dumpTypes()
{
    LIST *lst = typeList;
    LIST **pList = &lst;
    typeList = NULL;

    if (int8_used)
    {
        bePrintf(".class %s private value explicit ansi sealed 'int8[]' {.pack 1 .size 1}\n", namespaceAndClass[0] ? "nested" : "");
    }
    if (int16_used)
    {
        bePrintf(".class %s private value explicit ansi sealed 'int16[]' {.pack 2 .size 1}\n", namespaceAndClass[0] ? "nested" : "");
    }
    if (int32_used)
    {
        bePrintf(".class %s private value explicit ansi sealed 'int32[]' {.pack 4 .size 1}\n", namespaceAndClass[0] ? "nested" : "");
    }
    while (*pList)
    {
        // weed
        LIST **qList = &(*pList)->next;
        TYPE *tp = ((TYPE *)(*pList)->data);
        char z;
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
        if (namespaceAndClass[0])
            bePrintf(".class nested public value explicit auto sequential ansi sealed '");
        else
            bePrintf(".class private value explicit ansi sealed '");
        z = namespaceAndClass[0];
        namespaceAndClass[0] = 0;
        puttype(tp);
        namespaceAndClass[0] = z;
        if (isstructured(tp) && namespaceAndClass[0])
        {
            bePrintf("' {.pack %d .size %d", tp->sp->structAlign, tp->size);
            if (qualifiedStruct(basetype(tp)->sp))
                DumpClassFields(basetype(tp)->sp);
            bePrintf("}\n");
        }
        else
        {
            bePrintf("' {.pack 1 .size %d }\n", tp->size);
        }
        pList = &(*pList)->next;
    }

    typeList = NULL;
    dumpEnums();
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
static void mainLocals(void)
{
    bePrintf("\t.locals (\n");
    bePrintf("\t\t[0] int32 'argc',\n");
    bePrintf("\t\t[1] void * 'argv',\n");
    bePrintf("\t\t[2] void * 'environ',\n");
    bePrintf("\t\t[3] void * 'newmode'\n");
    bePrintf("\t)\n");
}
static void mainInit(void)
{
    bePrintf("\tcall void *'__pctype_func'()\n");
    bePrintf("\tstsfld void * %s_pctype\n", namespaceAndClass);
    bePrintf("\tcall void *'__iob_func'()\n");
    bePrintf("\tdup\n");
    bePrintf("\tstsfld void * %s__stdin\n", namespaceAndClass);
    bePrintf("\tdup\n");
    bePrintf("\tldc.i4 32\n");
    bePrintf("\tadd\n");
    bePrintf("\tstsfld void * %s__stdout\n", namespaceAndClass);
    bePrintf("\tldc.i4 64\n");
    bePrintf("\tadd\n");
    bePrintf("\tstsfld void * %s__stderr\n", namespaceAndClass);
}
void oa_end_generation(void)
{
    SYMBOL *start = NULL, *end = NULL, *mainsp;
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

    if ((start || prm_targettype == DLL) && namespaceAndClass[0])
    {
        bePrintf(".method private hidebysig specialname rtspecialname static void  .cctor() cil managed {\n");
        if (prm_targettype == DLL)
        {
            mainLocals();
            bePrintf("\t.maxstack 5\n\n");
            mainInit();
        }
        else
        {
            bePrintf("\t.maxstack 1\n\n");
        }
        if (start)
            bePrintf("\tcall void %s%s()\n", namespaceAndClass, start->name);
        bePrintf("\tret\n");
        bePrintf("}\n");

    }
    if (prm_targettype != DLL)
    {
        bePrintf(".method private hidebysig static void '$Main'() cil managed {\n");
        bePrintf("\t.entrypoint\n");
        mainLocals();
        bePrintf("\t.maxstack 5\n\n");
        mainInit();
        if (start && !namespaceAndClass[0])
            bePrintf("\tcall void %s%s()\n", namespaceAndClass, start->name);
        bePrintf("\tldloca 'argc'\n");
        bePrintf("\tldloca 'argv'\n");
        bePrintf("\tldloca 'environ'\n");
        bePrintf("\tldc.i4  0\n");
        bePrintf("\tldloca 'newmode'\n");
        bePrintf("\tcall void __getmainargs(void *, void *, void *, int32, void *);\n");
        bePrintf("\tldloc 'argc'\n");
        bePrintf("\tldloc 'argv'\n");
        mainsp = gsearch("main");
        if (mainsp)
        {
            mainsp = (SYMBOL *)mainsp->tp->syms->table[0]->p;
            if ( isvoid(basetype(mainsp->tp)->btp))
                if (namespaceAndClass[0])
                    bePrintf("\tcall void %smain(int32, void *)\n", namespaceAndClass);
                else
                    bePrintf("\tcall void 'main'(int32, void *)\n");
            else
                if (namespaceAndClass[0])
                    bePrintf("\tcall int32 %smain(int32, void *)\n", namespaceAndClass);
                else
                    bePrintf("\tcall int32 'main'(int32, void *)\n");
        }
        else
            if (namespaceAndClass[0])
                bePrintf("\tcall int32 %smain(int32, void *)\n", namespaceAndClass);
            else
                bePrintf("\tcall int32 'main'(int32, void *)\n");
        if (end)
            bePrintf("\tcall void %s%s()\n", namespaceAndClass, end->name);
        if (mainsp)
            if ( isvoid(basetype(mainsp->tp)->btp))
               bePrintf("\tldc.i4 0\n");
        bePrintf("\tcall void exit(int32)\n");
        bePrintf("\tret\n");
        bePrintf("}\n");
    }
    dumpTypes();    
    if (namespaceAndClass[0])
    {
        bePrintf("}\n");
        bePrintf("}\n");
    }
    bePrintf(".method private hidebysig static void * __GetErrno() cil managed {\n");
    bePrintf("\t.maxstack 1\n\n");
    bePrintf("\tcall void * '_errno'()\n");
    bePrintf("\tret\n");
    bePrintf("}\n");

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
