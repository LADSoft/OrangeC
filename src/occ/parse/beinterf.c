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
 * C configuration when we are using ICODE code generator output
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "compiler.h"
 
extern int packdata[], packlevel;

TYPE stdvoid =
{
    bt_void, 0
} ;
TYPE stdany =
{
    bt_any, 1
} ;
TYPE stdauto =
{
    bt_auto, 0
} ;
TYPE stdfunc =
{
    bt_func, 0, &stdvoid
};
TYPE stdpointer = 
{
    bt_pointer, 4, &stdvoid
};
TYPE std__string =
{
    bt___string, 4
};
TYPE std__object =
{
    bt___object, 4
};
TYPE stdnullpointer =
{
    bt_pointer, 4, &stdvoid
};
TYPE stdfloatcomplex =
{
    bt_float_complex, 8
};
TYPE stddoublecomplex =
{
    bt_double_complex, 16
};
TYPE stdlongdoublecomplex =
{
    bt_long_double_complex, 24
};
TYPE stdfloat = 
{
    bt_float, 4,
};
TYPE stdfloatimaginary = 
{
    bt_float_imaginary, 4, 
};
TYPE stddouble = 
{
    bt_double, 8
};
TYPE stddoubleimaginary = 
{
    bt_double_imaginary, 8
};
TYPE stdlongdoubleimaginary = 
{
    bt_long_double_imaginary, 12
};
TYPE stdlonglong = 
{
    bt_long_long, 8
};
TYPE stdunsigned = 
{
    bt_unsigned, 4
};
TYPE stdunative =
{
    bt_unative, 4
};
TYPE stdunsignedlong =
{
    bt_unsigned_long, 4,
};
TYPE stdunsignedlonglong = 
{
    bt_unsigned_long_long, 8
};
TYPE stdconst = 
{
    bt_int, 4,
};
TYPE stdchar = 
{
    bt_char, 1,
};
TYPE stdchar16t = 
{
    bt_char16_t, 2,
};
TYPE stdchar16tptr =
{
    bt_pointer, 0, &stdchar16t
};
TYPE stdchar32t = 
{
    bt_char32_t, 4,
};
TYPE stdchar32tptr =
{
    bt_pointer, 0, &stdchar32t
};
TYPE stdsignedchar = 
{
    bt_char, 1,
};
TYPE stdunsignedchar = 
{
    bt_unsigned_char, 1,
};
TYPE stdshort = 
{
    bt_short, 2,
};
TYPE stdunsignedshort = 
{
    bt_unsigned_short, 2,
};
TYPE std__func__nc = 
{
    bt_pointer, 4, &stdchar
};
static TYPE std__func__c = 
{
    bt_const, 4, &stdchar
};
TYPE std__func__ = 
{
    bt_pointer, 4, &std__func__c
};
TYPE stdstring = 
{
    bt_pointer, 4, &stdchar
};
TYPE stdint = 
{
    bt_int, 4
};
TYPE stdinative =
{
    bt_inative, 4
};
TYPE stdlong =
{
    bt_long, 4
};
TYPE stdlongdouble = 
{
    bt_long_double, 10
};
TYPE stdbool = 
{
    bt_bool, 1
};
TYPE stdwidechar = 
{
    bt_wchar_t, 0
};
TYPE stdwcharptr =
{
    bt_pointer, 0, &stdwidechar
};
TYPE stdcharptr =
{
    bt_pointer, 0, &stdchar
};

extern ARCH_ASM assemblerInterface[];
extern COMPILER_PARAMS cparams;

ARCH_ASM *chosenAssembler;
ARCH_DEBUG *chosenDebugger;

static int recurseNode(EXPRESSION *node, EXPRESSION **type, EXPRESSION **bits)
{
            switch(node->type)
            {
                case en_tempref:
                    *type = node;
                    return 0;
                case en_bits:
                    *bits = node;
                    return recurseNode(node->left, type, bits);
                case en_absolute:
                case en_auto:
                case en_global:
                case en_pc:
                case en_threadlocal:
                    *type = node;
                    return 0;
                case en_add:
                case en_arrayadd:
                case en_structadd:
                    return(recurseNode(node->left, type, bits)
                            + recurseNode(node->right, type, bits));
                default:
                    if (isintconst(node))
                        return node->v.i;
                    if (isfloatconst(node) 
                        || isimaginaryconst(node)
                        || iscomplexconst(node))
                    {
                        *type = node;
                        return 0;
                    }
                    diag("recurseNode: unknown node");
                    break;
            }
    return 0;
}

BE_IMODEDATA *beArgType(IMODE *in)
{
    BE_IMODEDATA *rv = (BE_IMODEDATA *)Alloc(sizeof(BE_IMODEDATA));
    EXPRESSION *node = in->offset, *bits = 0;
    int ofs;
    rv->size = in->size;
    rv->u.sym.startBit = BIT_NO_BITS;
    rv->m = in;
    switch(in->mode)
    {
        case i_none:
        default:
            rv->mode = bee_unknown;
            break;
        case i_ind:
            rv->ind = TRUE;
        case i_immed:
            if (in->mode == i_immed)
                rv->immed = TRUE;
        case i_direct:
            if (in->retval)
            {
                rv->mode = bee_rv;
                break;
            }
            ofs  = recurseNode(in->offset, &node, &bits);
            switch(node->type)
            {
                case en_tempref:
                    rv->mode = bee_temp;
                    rv->u.sym.sp = node->v.sp;
                    rv->u.tempRegNum = node->v.sp->offset;
                    break;
                case en_auto:
                case en_absolute:
                    rv->u.sym.localOffset = node->v.sp->offset;
                case en_global:
                case en_pc:
                    rv->u.sym.symOffset = ofs;
                    rv->u.sym.sp = node->v.sp ;
                    rv->u.sym.name = node->v.sp->decoratedName;
                    if (bits)
                    {
                        rv->u.sym.startBit = bits->startbit;
                        rv->u.sym.bits = bits->bits;
                    }
                    switch(node->type)
                    {
                        case en_auto:
                            rv->mode = bee_local;
                            break;
                        case en_absolute:
                            rv->mode = bee_abs;
                            break;
                        case en_global:
                            rv->mode = bee_global_data;
                            break;
                        case en_pc:
                            rv->mode = bee_global_pc;
                            break;
                        default:
                            break;
                    }
                    break ;
                case en_label:
                    rv->mode = bee_label;
                    rv->u.labelNum = node->v.sp->label;
                    break ;
                case en_labcon:
                    rv->mode = bee_label;
                    rv->u.labelNum = node->v.i;
                    break ;
                case en_threadlocal:
                    rv->mode = bee_threadlocal;
                    break;
                default:
                    if (isintconst(node))
                    {
                        rv->mode = bee_icon;
                        rv->u.i = ofs;
                        break;
                    }
                    if (isfloatconst(node))
                    {
                        rv->mode = bee_float;
                        rv->u.f.r = &node->v.f;
                        break ;
                    }
                    if (isimaginaryconst(node))
                    {
                        rv->mode = bee_imaginary;
                        rv->u.f.r = &node->v.f;
                        break ;
                    }
                    if (iscomplexconst(node))
                    {
                        rv->mode = bee_complex;
                        rv->u.f.r = &node->v.c.r;
                        rv->u.f.i = &node->v.c.i;
                    }
                    break ;
            }
            break;
    }
    return rv;
}
int sizeFromISZ(int isz)
{
    ARCH_SIZING *p = chosenAssembler->arch->type_sizes;
    switch (isz)
    {
        case ISZ_U16:
            return 2;
        case ISZ_U32:
            return 4;
        case ISZ_BIT:
            return 0;
        case ISZ_UCHAR:
        case -ISZ_UCHAR:
            return p->a_char;
        case ISZ_BOOLEAN:
            return p->a_bool;
        case ISZ_USHORT:
        case -ISZ_USHORT:
            return p->a_short;
/*        case ISZ_:*/
/*            return p->a_wchar_t;*/
        case ISZ_ULONG:
        case -ISZ_ULONG:
            return p->a_long;
        case ISZ_ULONGLONG:
        case -ISZ_ULONGLONG:
            return p->a_longlong;
        case ISZ_UINT:
        case -ISZ_UINT:
        case ISZ_UNATIVE:
        case -ISZ_UNATIVE:
            return p->a_int;
/*        case ISZ_ENUM:*/
/*            return p->a_enum;*/
        case ISZ_ADDR:
        case ISZ_STRING:
        case ISZ_OBJECT:
            return p->a_addr;
        case ISZ_SEG:
            return p->a_farseg;
        case ISZ_FARPTR:
            return p->a_farptr;
        case ISZ_FLOAT:
        case ISZ_IFLOAT:
            return p->a_float;
        case ISZ_DOUBLE:
        case ISZ_IDOUBLE:
            return p->a_double;
        case ISZ_LDOUBLE:
        case ISZ_ILDOUBLE:
            return p->a_longdouble;
        case ISZ_CFLOAT:
            return (p->a_float + p->a_fcomplexpad) * 2;
        case ISZ_CDOUBLE:
            return (p->a_double + p->a_rcomplexpad) * 2;
        case ISZ_CLDOUBLE:
            return (p->a_longdouble + p->a_lrcomplexpad) * 2;
        default:
            return 1;
    }
}
int needsAtomicLockFromType(TYPE *tp)
{
    ARCH_SIZING *p = chosenAssembler->arch->type_needsLock;
    switch (basetype(tp)->type)
    {
        case bt_char16_t:
            return 0;
        case bt_char32_t:
            return 0;
        case bt_bit:
        case bt_void:
            return 0;
        case bt_char:
        case bt_unsigned_char:
            return p->a_char;
        case bt_bool:
            return p->a_bool;
        case bt_short:
        case bt_unsigned_short:
            return p->a_short;
        case bt_wchar_t:
            return p->a_wchar_t;
        case bt_long:
        case bt_unsigned_long:
            return p->a_long;
        case bt_long_long:
        case bt_unsigned_long_long:
            return p->a_longlong;
        case bt_int:
        case bt_unsigned:
        case bt_inative:
        case bt_unative:
            return p->a_int;
        case bt_enum:
            return p->a_enum;
        case bt_pointer:
            return p->a_addr;
        case bt_any:
            return p->a_addr;
        case bt_seg:
            return p->a_farseg;
        case bt_far:
            return p->a_farptr;
        case bt_memberptr:
            return p->a_memberptr;
        case bt_float:
        case bt_float_imaginary:
            return p->a_float;
        case bt_double:
        case bt_double_imaginary:
            return p->a_double;
        case bt_long_double:
        case bt_long_double_imaginary:
            return p->a_longdouble;
        case bt_float_complex:
            return 1;
        case bt_double_complex:
            return 1;
        case bt_long_double_complex:
            return 1;
        case bt_class:
        case bt_struct:
        case bt_union:
        default:
            return 1;
    }
}
static int basesize(ARCH_SIZING *p, TYPE *tp)
{
    tp = basetype(tp);
    switch (tp->type)
    {
        case bt_char16_t:
            return 2;
        case bt_char32_t:
            return 4;
        case bt_bit:
        case bt_void:
            return 0;
        case bt_char:
        case bt_unsigned_char:
        case bt_signed_char:
            return p->a_char;
        case bt_bool:
            return p->a_bool;
        case bt_short:
        case bt_unsigned_short:
            return p->a_short;
        case bt_wchar_t:
            return p->a_wchar_t;
        case bt_long:
        case bt_unsigned_long:
            return p->a_long;
        case bt_long_long:
        case bt_unsigned_long_long:
            return p->a_longlong;
        case bt_int:
        case bt_unsigned:
        case bt_inative:
        case bt_unative:
            return p->a_int;
        case bt_enum:
            return p->a_enum;
        case bt_pointer:
            if (tp->array && !tp->vla)
                return basesize(p, tp->btp);
            else
                return p->a_addr;
        case bt_any:
            return p->a_addr;
        case bt_seg:
            return p->a_farseg;
        case bt_far:
            return p->a_farptr;
        case bt_memberptr:
            return p->a_memberptr;
        case bt_float:
        case bt_float_imaginary:
            return p->a_float;
        case bt_double:
        case bt_double_imaginary:
            return p->a_double;
        case bt_long_double:
        case bt_long_double_imaginary:
            return p->a_longdouble;
        case bt_float_complex:
            return (p->a_float + p->a_fcomplexpad) * 2;
        case bt_double_complex:
            return (p->a_double + p->a_rcomplexpad) * 2;
        case bt_long_double_complex:
            return (p->a_longdouble + p->a_lrcomplexpad) * 2;
        case bt___string:
            return p->a_addr;
        case bt___object:
            return p->a_addr;
        case bt_class:
        case bt_struct:
        case bt_union:
            if (p->a_struct)
                return p->a_struct;
            else if (tp->alignment)
                return tp->alignment;
            else
                return tp->sp->structAlign?  tp->sp->structAlign : 1;
        default:
/*            diag("basesize: unknown type");*/
            return 1;
    }
}
int getSize(enum e_bt type)
{
    TYPE tp ;
    memset(&tp, 0, sizeof(tp));
    tp.type = type; /* other fields don't matter, we never call this for structured types*/
    return basesize(chosenAssembler->arch->type_sizes, &tp);    
}
int getBaseAlign(enum e_bt type)
{
    TYPE tp ;
    if (type == bt_auto)
        type = bt_struct;
    tp.type = type; /* other fields don't matter, we never call this for structured types*/
    tp.array = tp.vla = FALSE;
    tp.rootType = &tp;
    return basesize(chosenAssembler->arch->type_align, &tp);    
}
long getautoval(long val)
{
    if (chosenAssembler->arch->spgrowsup)
        return -val;
    else
        return val;
}
int funcvaluesize(int val)
{
    if (chosenAssembler->arch->param_offs)
        return(chosenAssembler->arch->param_offs(val));
    return 0;
}
int alignment(int sc, TYPE *tp)
{
    (void)sc;
    return basesize(chosenAssembler->arch->type_align, tp);
}
int getAlign(int sc, TYPE *tp)
{
    int align = basesize(chosenAssembler->arch->type_align, tp);
    if (sc != sc_auto)
        if (packdata[packlevel] < align)
            align = packdata[packlevel];
    if (chosenAssembler->arch->align)
        align = chosenAssembler->arch->align(align);
    return align;
}
char *getUsageText(void)
{
    return chosenAssembler->usage_text;
}
char *lookupRegName(int regnum)
{
    if (regnum < chosenAssembler->arch->registerCount)
        return chosenAssembler->arch->regNames[regnum].name;
    return "???";
}
KEYWORD *GetProcKeywords(void)
{
    return chosenAssembler->keywords;
}
int init_backend(int *argc ,char **argv)
{
    char assembler[100], debugger[100];
    int i,rv;
    assembler[0] = debugger[0] = 0;
    cparams.prm_asmfile = cparams.prm_compileonly = FALSE;
    for (i=0; i < *argc; i++)
        if (!strncmp(argv[i],"/S",2)) {
            char *p = argv[i]+2,*q = assembler;
            cparams.prm_asmfile = TRUE;
            if (argv[i][1] == 'S')
                cparams.prm_compileonly = TRUE;
            while (*p && !isspace(*p) && *p != ';')
                *q++ = *p++;
            *q = 0;
            if (*p == ';') {
                q = debugger;
                p++;
                while (*p && !isspace(*p))
                    *q++ = *p++;
                *q = 0;
            }
            memcpy(argv + i, argv + i + 1, sizeof(char **) * (*argc - i + 1));
            (*argc)--;
            break;
        }
    if (assembler[0]) {
        ARCH_ASM *a = assemblerInterface ;
        while (a->name) {
            if (!strcmp(a->name, assembler)) {
                chosenAssembler = a;
                break ;
            }
            a++;
                
        }
        if (!a->name)
            fatal("Chosen assembler format '%s' not found", assembler);
    } else {
        chosenAssembler = assemblerInterface;
    }
    if (!chosenAssembler->objext)
        cparams.prm_asmfile = TRUE;
    if (debugger[0]) {
        ARCH_DEBUG *d = chosenAssembler->debug;
        if (d) {
            while (d->name) {
                if (!strcmp(d->name, debugger)) {
                    chosenDebugger = d;
                    break ;
                }
                d++;
            }
        }
        if (!d || !d->name) {
            fatal("Chosen debugger format '%s' not found", debugger);
        }
    } else
        chosenDebugger = chosenAssembler->debug;
    rv = !chosenAssembler->init || (*chosenAssembler->init)(&cparams, chosenAssembler, chosenDebugger);
    if (rv ) {
        stdpointer.size = getSize(bt_pointer);
        stdnullpointer.size = getSize(bt_pointer);
        stdnullpointer.nullptrType = TRUE;
        stdfloatimaginary.size = stdfloat.size = getSize(bt_float);
        stddouble.size = stddoubleimaginary.size = getSize(bt_double);
        stdlongdouble.size = stdlongdoubleimaginary.size = getSize(bt_long_double);
        stdfloatcomplex.size = getSize(bt_float_complex);
        stddoublecomplex.size = getSize(bt_float_complex);
        stdlongdoublecomplex.size = getSize(bt_float_complex);
        stdunsignedlonglong.size = stdlonglong.size = getSize(bt_long_long);
        stdunsignedlong.size = stdlong.size = getSize(bt_long);
        stdconst.size = stdunsigned.size = stdint.size = getSize(bt_unsigned);        
        stdstring.size = getSize(bt_pointer);
        std__string.size = getSize(bt_pointer);
        std__object.size = getSize(bt_pointer);
        stdchar.size = getSize(bt_char);
        stdsignedchar.size = getSize(bt_unsigned_char);
        stdunsignedchar.size = getSize(bt_unsigned_char);
        stdshort.size = getSize(bt_short);
        stdunsignedshort.size = getSize(bt_unsigned_short);
        stdcharptr.size = getSize(bt_pointer);
        std__func__.size = getSize(bt_pointer);
        std__func__nc.size = getSize(bt_pointer);
        std__func__nc.array = TRUE;
        stdbool.size = getSize(bt_bool);
        stdwidechar.size = getSize(bt_wchar_t);
        stdwcharptr.size = getSize(bt_pointer);
        stdchar16tptr.size = getSize(bt_pointer);
        stdchar32tptr.size = getSize(bt_pointer);

        stdpointer.rootType = &stdpointer;
        stdnullpointer.rootType = &stdnullpointer;
        stdfloatimaginary.rootType = &stdfloatimaginary;
        stdfloat.rootType = &stdfloat;
        stddouble.rootType = &stddouble;
        stddoubleimaginary.rootType = &stddoubleimaginary;;
        stdlongdouble.rootType = &stdlongdouble;
        stdlongdoubleimaginary.rootType = &stdlongdoubleimaginary;;
        stdfloatcomplex.rootType = &stdfloatcomplex;
        stddoublecomplex.rootType = &stddoublecomplex;
        stdlongdoublecomplex.rootType = &stdlongdoublecomplex;
        stdunsignedlonglong.rootType = &stdunsignedlonglong;
        stdlonglong.rootType = &stdlonglong;
        stdunsignedlong.rootType = &stdunsignedlong;
        stdlong.rootType = &stdlong;
        stdconst.rootType = &stdconst;
        stdunsigned.rootType = &stdunsigned;
        stdint.rootType = &stdint;        
        stdunative.rootType = &stdunative;
        stdinative.rootType = &stdinative;
        stdstring.rootType = &stdstring;
        stdchar.rootType = &stdchar;
        stdsignedchar.rootType = &stdsignedchar;
        stdunsignedchar.rootType = &stdunsignedchar;
        stdshort.rootType = &stdshort;
        stdunsignedshort.rootType = &stdunsignedshort;
        stdcharptr.rootType = &stdcharptr;
        std__func__.rootType = &std__func__;
        std__func__nc.rootType = &std__func__nc;
        stdbool.rootType = &stdbool;
        stdwidechar.rootType = &stdwidechar;
        stdwcharptr.rootType = &stdwcharptr;
        stdchar16tptr.rootType = &stdchar16tptr;
        stdchar32tptr.rootType = &stdchar32tptr;

    }
    return rv ;
}
