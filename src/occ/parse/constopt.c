/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binareby forms, 
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
 * this module combines constants at compile time.  It is used e.g. 
 * for evaluating static assignments, but an initial folding of variable
 * expressions is done as well to slightly improve the code generation
 */
#include <stdio.h>
#include <string.h>
#include <float.h>
#include "compiler.h"

extern int stdpragmas;
extern ARCH_ASM *chosenAssembler;
extern TYPE stdvoid;
extern BOOLEAN initializingGlobalVar;
extern int total_errors;
extern STRUCTSYM *structSyms;
extern int templateNestingCount;

static EXPRESSION *asidehead,  **asidetail;
ULLONG_TYPE reint(EXPRESSION *node);
LLONG_TYPE mod_mask(int i);

static unsigned LLONG_TYPE shifts[sizeof(LLONG_TYPE)*8] ;
static EXPRESSION *functionnesting[100];
static int functionnestingcount = 0;
void constoptinit(void)
{
    int i;
    for (i=0; i < sizeof(LLONG_TYPE) * 8; i++)
        shifts[i] = ((ULLONG_TYPE)1) << i;
}
static int optimizerfloatconst(EXPRESSION *en)
{
    if ((en->pragmas &STD_PRAGMA_FENV) && !initializingGlobalVar)
        return FALSE;

    return isfloatconst(en) || isimaginaryconst(en) || iscomplexconst(en);
}

/*-------------------------------------------------------------------------*/

static int isoptconst(EXPRESSION *en)
{
    if (!en)
        return FALSE;
    return isintconst(en) || optimizerfloatconst(en);
}

/*-------------------------------------------------------------------------*/

static int maxinttype(EXPRESSION *ep1, EXPRESSION *ep2)
{
    int type1 = ep1->type;
    int type2 = ep2->type;
    if (type1 == en_c_ull || type2 == en_c_ull)
        return en_c_ull;
    if (type1 == en_c_ll || type2 == en_c_ll)
        return en_c_ll;
    if (type1 == en_c_ul || type2 == en_c_ul)
        return en_c_ul;
    if (type1 == en_c_l || type2 == en_c_l)
        return en_c_l;
    if (type1 == en_c_u32 || type2 == en_c_u32)
        return en_c_u32;
    if (type1 == en_c_ui || type2 == en_c_ui)
        return en_c_ui;
    if (type1 == en_c_i || type2 == en_c_i)
        return en_c_i;
    if (type1 == en_c_u16 || type2 == en_c_u16)
        return en_c_u16;
    if (type1 == en_c_wc || type2 == en_c_wc)
        return en_c_wc;
    if (type1 == en_c_uc || type2 == en_c_uc)
        return en_c_uc;
    if (type1 == en_c_c || type2 == en_c_c)
        return en_c_c;
    return en_c_bool;
}

/*-------------------------------------------------------------------------*/

static int isunsignedexpr(EXPRESSION *ep1)
{
    switch (ep1->type)
    {
        case en_c_ul:
        case en_c_ui:
        case en_c_ull:
            return TRUE;
        default:
            return FALSE;
    }
}

/*-------------------------------------------------------------------------*/

static int maxfloattype(EXPRESSION *ep1, EXPRESSION *ep2)
{
    int type1 = ep1->type;
    int type2 = ep2->type;
    if (type1 == en_c_ld || type2 == en_c_ld || type1 == en_c_ldi ||
        type2 == en_c_ldi)
        return en_c_ld;
    if (type1 == en_c_d || type2 == en_c_d || type1 == en_c_di ||
        type2 == en_c_di)
        return en_c_d;
    return en_c_f;
}

/*-------------------------------------------------------------------------*/

static int maximaginarytype(EXPRESSION *ep1, EXPRESSION *ep2)
{
    int type1 = ep1->type;
    int type2 = ep2->type;
    if (type1 == en_c_ld || type2 == en_c_ld || type1 == en_c_ldi ||
        type2 == en_c_ldi)
        return en_c_ldi;
    if (type1 == en_c_d || type2 == en_c_d || type1 == en_c_di ||
        type2 == en_c_di)
        return en_c_di;
    return en_c_fi;
}

/*-------------------------------------------------------------------------*/

static int maxcomplextype(EXPRESSION *ep1, EXPRESSION *ep2)
{
    int type1 = ep1->type;
    int type2 = ep2->type;
    if (type1 == en_c_ld || type2 == en_c_ld || type1 == en_c_ldi ||
        type2 == en_c_ldi || type1 == en_c_ldc ||
        type2 == en_c_ldc)
        return en_c_ldc;
    if (type1 == en_c_d || type2 == en_c_d || type1 == en_c_di ||
        type2 == en_c_di || type1 == en_c_dc || type2 == en_c_dc)
        return en_c_dc;
    return en_c_fc;
}
static BOOLEAN hasFloats(EXPRESSION *node)
/*
 * Go through a node and see if it will be promoted to type FLOAT
 */
{
    if (!node)
        return 0;
    switch (node->type)
    {
        case en_x_f:
        case en_x_d:
        case en_x_ld:
        case en_x_fc:
        case en_x_dc:
        case en_x_ldc:
        case en_x_fi:
        case en_x_di:
        case en_x_ldi:
        case en_l_f:
        case en_l_d:
        case en_l_ld:
        case en_l_fc:
        case en_l_dc:
        case en_l_ldc:
        case en_l_fi:
        case en_l_di:
        case en_l_ldi:
        case en_c_f:
        case en_c_d:
        case en_c_ld:
        case en_c_fc:
        case en_c_dc:
        case en_c_ldc:
        case en_c_fi:
        case en_c_di:
        case en_c_ldi:
            return 1;
        default:
            return 0;
        case en_assign:
        case en_autoinc:
        case en_autodec:
        case en_add:
        case en_sub:
        case en_arrayadd:
        case en_arraymul:
        case en_arraydiv:
        case en_structadd:
        case en_umul:
        case en_udiv:
        case en_umod:
        case en_mul:
        case en_div:
        case en_mod:
        case en_and:
        case en_or:
        case en_xor:
        case en_ursh:
        case en_rshd:
        case en_lsh:
        case en_arraylsh:
        case en_rsh:
        case en_land:
        case en_lor:
        case en_void:
        case en_voidnz:
            return (hasFloats(node->left) || hasFloats(node->right));
        case en_cond:
            return hasFloats(node->right);
        case en_argnopush:
        case en_not_lvalue:
        case en_thisref:
        case en_literalclass:
        case en_lvalue:
            return hasFloats(node->left);
    }
    return (0);
}
/*-------------------------------------------------------------------------*/

static int getmode(EXPRESSION *ep1, EXPRESSION *ep2)
/*
 * get the constant mode of a pair of nodes
 * 0 = Neither node is a constant
 * 1 = icon,icon
 * 2 = icon,rcon
 * 3 = rcon,icon
 * 4 = rcon,rcon
 * 5 = icon,nothing
 * 6 = rcon,nothing
 * 7 = nothing,icon
 * 8 = nothing,rcon
 * 9 = rcon, ricon
 * 10 = ricon, rcon
 * 11 = ricon, ricon
 * 12 = ricon, nothing
 * 13 = nothing, ricon
 * 14 = ricon, icon
 * 15 = icon, ricon
 * 16 = ricon, cmplxcon
 * 17 = cmplxcon, ricon
 * 18 = rcon, cmplxcon
 * 19 = cmplxcon, rcon
 * 20 = cmplxcon, cmplxcon
 * 21 = cmplxcon, nothing
 * 22 = nothing, cmplxcon
 * 23 = cmplxcon, icon
 * 24 = icon, cmplxcon
 */
{
    int mode = 0;
    if (!initializingGlobalVar && (ep1->pragmas &STD_PRAGMA_FENV) && (isfloatconst(ep1) ||
        isimaginaryconst(ep1) || iscomplexconst(ep1)))
        return 0;
    if (!initializingGlobalVar && ep2 && (ep2->pragmas &STD_PRAGMA_FENV) && (isfloatconst(ep2) ||
        isimaginaryconst(ep2) || iscomplexconst(ep2)))
        return 0;
    if (isintconst(ep1))
        if (ep2)
        {
            if (isintconst(ep2))
                mode = 1;
            else if (isfloatconst(ep2))
            {
                    mode = 2;
            }
            else if (isimaginaryconst(ep2))
                    mode = 15;
            else if (iscomplexconst(ep2))
                    mode = 24;
            else
                mode = 5;
        }
        else
            mode = 5;
    else if (isfloatconst(ep1))
        if (ep2)
        {
            if (isintconst(ep2))
                    mode = 3;
            else if (isfloatconst(ep2))
                    mode = 4;
            else if (isimaginaryconst(ep2))
                    mode = 9;
            else if (iscomplexconst(ep2))
                    mode = 18;
            else
                mode = 6;
        }
        else
            mode = 6;
    else if (isimaginaryconst(ep1))
        if (ep2)
        {
            if (isfloatconst(ep2))
                    mode = 10;
            else if (isimaginaryconst(ep2))
                    mode = 11;
            else if (iscomplexconst(ep2))
                    mode = 16;
            else if (isintconst(ep2))
                    mode = 14;
            else
                mode = 12;
        } else
            mode = 12;
    else if (iscomplexconst(ep1))
        if (ep2)
        {
            if (isfloatconst(ep2))
                    mode = 17;
            else if (isimaginaryconst(ep2))
                    mode = 19;
            else if (iscomplexconst(ep2))
                    mode = 20;
            else if (isintconst(ep2))
                    mode = 23;
            else
                mode = 21;
        } else
            mode = 21;
    else if (ep2)
    {
        if (isintconst(ep2))
            mode = 7;
        else if (ep2->type == en_c_d || ep2->type == en_c_ld || ep2
            ->type == en_c_f)
                mode = 8;
        else if (ep2->type == en_c_di || ep2->type == en_c_fi ||
            ep2->type == en_c_ldi)
                mode = 13;
        else if (ep2->type == en_c_dc || ep2->type == en_c_fc ||
            ep2->type == en_c_ldc)
                mode = 22;
    }    
    return (mode);
}
LLONG_TYPE MaxOut(enum e_bt size, LLONG_TYPE value)
{
    int bits;
    int plus, minus;
    switch (size)
    {
        case bt_char:
            if (cparams.prm_charisunsigned)
                bits = 8;
            else
                bits = 7;
            break;
        case bt_signed_char:
            bits = 7;
            break;
        case bt_unsigned_char:
            bits = 8;
            break;
        case bt_short:
            bits = getSize(bt_short) * 8 - 1;
            break;
        case bt_unsigned_short:
            bits = getSize(bt_short) * 8;
            break;
        default:
            return value;			
    }
    plus = mod_mask(bits);
    minus = - plus;
    if (value < minus)
        value = minus;
    if (value > plus)
        value = plus;
    return value;
}
/*-------------------------------------------------------------------------*/
ULLONG_TYPE CastToInt(int size, LLONG_TYPE value)
{
    int bits;
    switch(size) {
        default:
            return value ;
        case ISZ_U16:
            bits = getSize(bt_char16_t) * 8;
            break;
        case ISZ_U32:
            bits = getSize(bt_char32_t) * 8;
            break;
        case ISZ_BOOLEAN:
            bits = 1;
            break ;
        case -ISZ_UCHAR:
        case ISZ_UCHAR:
            bits = 8;
            break;
        case -ISZ_USHORT:
        case ISZ_USHORT:
            bits = getSize(bt_short) * 8;
            break;
        case -ISZ_UINT:
        case ISZ_UINT:
            bits = getSize(bt_int) * 8;
            break ;
        case -ISZ_ULONG:
        case ISZ_ULONG:
            bits = getSize(bt_long) * 8;
            break ;
        case -ISZ_ULONGLONG:
        case ISZ_ULONGLONG:
            bits = getSize(bt_long_long) * 8;
            break ;
    }
    value &= mod_mask(bits);
    if (size < 0)
        if (value & (((ULLONG_TYPE)1) << (bits - 1)))
            value |= ~mod_mask(bits);
    return value;
}
FPF CastToFloat(int size, FPF *value)
{
    switch(size) {
        case ISZ_FLOAT:
        case ISZ_IFLOAT:
            if (chosenAssembler->arch->flt_float)
            {
                ARCH_FLOAT *flt = chosenAssembler->arch->flt_float;
                FPFTruncate(value, flt->mantissa_bits, flt->exp_max, flt->exp_min);
            }
            else
                diag("CastToFloat: architecture characteristics for 'float' not set");
            break;
        case ISZ_DOUBLE:
        case ISZ_IDOUBLE:
            if (chosenAssembler->arch->flt_dbl)
            {
                ARCH_FLOAT *flt = chosenAssembler->arch->flt_dbl;
                FPFTruncate(value, flt->mantissa_bits, flt->exp_max, flt->exp_min);
            }
            else
                diag("CastToFloat: architecture characteristics for 'double' not set");
            break;
        case ISZ_LDOUBLE:
        case ISZ_ILDOUBLE:
            if (chosenAssembler->arch->flt_ldbl)
            {
                ARCH_FLOAT *flt = chosenAssembler->arch->flt_ldbl;
                FPFTruncate(value, flt->mantissa_bits, flt->exp_max, flt->exp_min);
            }
            else
                diag("CastToFloat: architecture characteristics for 'long double' not set");
            break ;
    }
    return *value;
}
FPF *IntToFloat(FPF* temp, int size, LLONG_TYPE value)
{
    LLONG_TYPE t = CastToInt(size,value);
    if (size < 0)
        LongLongToFPF(temp,t);
    else
        UnsignedLongLongToFPF(temp,t);
    return temp;
}
FPF refloat(EXPRESSION *node)
{
    FPF rv;
    FPF temp;
    switch (node->type)
    {
        case en_c_i:
            rv = CastToFloat(ISZ_LDOUBLE,IntToFloat(&temp,-ISZ_UINT,node->v.i));
            break;
        case en_c_l:
            rv = CastToFloat(ISZ_LDOUBLE,IntToFloat(&temp,-ISZ_ULONG,node->v.i));
            break;
        case en_c_ui:
            rv = CastToFloat(ISZ_LDOUBLE,IntToFloat(&temp,ISZ_UINT,node->v.i));
            break;
        case en_c_ul:
            rv = CastToFloat(ISZ_LDOUBLE,IntToFloat(&temp,ISZ_ULONG,node->v.i));
            break;
        case en_c_c:
            rv = CastToFloat(ISZ_LDOUBLE,IntToFloat(&temp,-ISZ_UCHAR,node->v.i));
            break;
        case en_c_uc:
            rv = CastToFloat(ISZ_LDOUBLE,IntToFloat(&temp,ISZ_UCHAR,node->v.i));
            break;
        case en_c_u16:
            rv = CastToFloat(ISZ_LDOUBLE,IntToFloat(&temp,ISZ_U16,node->v.i));
            break;
        case en_c_u32:
            rv = CastToFloat(ISZ_LDOUBLE,IntToFloat(&temp,ISZ_U32,node->v.i));
            break;
        case en_c_wc:
            rv = CastToFloat(ISZ_LDOUBLE,IntToFloat(&temp,ISZ_WCHAR,node->v.i));
            break;
        case en_c_bool:
            rv = CastToFloat(ISZ_LDOUBLE,IntToFloat(&temp,ISZ_BOOLEAN,node->v.i));
            break;
        case en_c_ull:
            rv = CastToFloat(ISZ_LDOUBLE,IntToFloat(&temp,ISZ_ULONGLONG,node->v.i));
            break;
        case en_c_ll:
            rv = CastToFloat(ISZ_LDOUBLE,IntToFloat(&temp,-ISZ_ULONGLONG,node->v.i));
            break;
        case en_c_f:
        case en_c_fi:
            rv = CastToFloat(ISZ_FLOAT,&node->v.f);
            break;
        case en_c_d:
        case en_c_di:
            rv = CastToFloat(ISZ_DOUBLE,&node->v.f);
            break;

        case en_c_ld:
        case en_c_ldi:
            rv = CastToFloat(ISZ_LDOUBLE,&node->v.f);
            break;
        default:
            break;
    }
    return rv;
}

/*-------------------------------------------------------------------------*/

ULLONG_TYPE reint(EXPRESSION *node)
{
    ULLONG_TYPE rv;
    if (node->left && node->right)
        node->unionoffset = node->left->unionoffset | node->right->unionoffset;
    else if (node->left)
        node->unionoffset = node->left->unionoffset;
    switch (node->type)
    {
        case en_c_l:
            rv = CastToInt(-ISZ_ULONG, node->v.i);
            break;
        case en_c_i:
            rv = CastToInt(-ISZ_UINT, node->v.i);
            break;
        case en_c_ul:
            rv = CastToInt(ISZ_ULONG, node->v.i);
            break;
        case en_c_ui:
            rv = CastToInt(ISZ_UINT, node->v.i);
            break;
        case en_c_c:
            rv = CastToInt(-ISZ_UCHAR, node->v.i);
            break;
        case en_c_uc:
            rv = CastToInt(ISZ_UCHAR, node->v.i);
            break;
        case en_c_wc:
            rv = CastToInt(ISZ_WCHAR, node->v.i);
            break;
        case en_c_u16:
            rv = CastToInt(ISZ_U16, node->v.i);
            break;
        case en_c_u32:
            rv = CastToInt(ISZ_U32, node->v.i);
            break;
        case en_c_bool:
            rv = CastToInt(ISZ_BOOLEAN, node->v.i);
            break;
        case en_c_ull:
            rv = CastToInt(ISZ_ULONGLONG, node->v.i);
            break;
        case en_c_ll:
            rv = CastToInt(-ISZ_ULONGLONG, node->v.i);
            break;
        case en_c_f:
            rv = CastToInt(-ISZ_ULONGLONG, FPFToLongLong(&node->v.f));
            break;
        case en_c_d:
            rv = CastToInt(-ISZ_ULONGLONG, FPFToLongLong(&node->v.f));
            break;

        case en_c_ld:
            rv = CastToInt(-ISZ_ULONGLONG, FPFToLongLong(&node->v.f));
            break;
        case en_c_fi:
        case en_c_di:
        case en_c_ldi:
            rv = (LLONG_TYPE)0;
            break;
        default:
            break;
    }
    return rv;
}
/*-------------------------------------------------------------------------*/

void dooper(EXPRESSION **node, int mode)
/*
 *      dooper will execute a constant operation in a node and
 *      modify the node to be the result of the operation.
 *			It will also cast integers to floating point values when
 *			necessary
 */
{
    EXPRESSION *ep,  *ep1,  *ep2;
    FPF temp;
    ep =  *node;
    ep1 = ep->left;
    ep2 = ep->right;
    if (mode == 5)
    {
        if (hasFloats(ep2))
        {
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&ep1->v.f, ep1->v.i);
                else
                    LongLongToFPF(&ep1->v.f, ep1->v.i);
            ep1->type = en_c_d;
            refloat(ep1);
        }
        return ;
    }
    else if (mode == 7)
    {
        if (hasFloats(ep1))
        {
                if (isunsignedexpr(ep2))
                    UnsignedLongLongToFPF(&ep2->v.f, ep2->v.i);
                else
                    LongLongToFPF(&ep2->v.f, ep2->v.i);
            ep2->type = en_c_d;
            refloat(ep2);
        }
        return ;
    }
    else if (mode == 6 || mode == 12 || mode == 13 || mode == 21 || mode == 22)
        return ;
    else
    switch (ep->type)
    {
        case en_structadd:
            if (ep->right->type == en_structelem)
                break;
        case en_add:
        case en_arrayadd:
            switch (mode)
            {
            case 1:
                ep->type = maxinttype(ep1, ep2);
                ep->v.i = ep1->v.i + ep2->v.i;
                ep->v.i = reint(ep);
//				if (ep->unionoffset)
//					printf("1");
//				if (ep1->unionoffset)
//					printf("2");
//				if (ep2->unionoffset)
//					printf("3");
//				printf(";");
                break;
            case 2:
                ep->type = maxfloattype(ep1, ep2);
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&temp, ep1->v.i);
                else
                    LongLongToFPF(&temp, ep1->v.i);
                AddSubFPF(0,&temp, &ep2->v.f, &ep->v.f);
                refloat(ep);
                break;
            case 3:
                ep->type = maxfloattype(ep1, ep2);
                if (isunsignedexpr(ep2))
                    UnsignedLongLongToFPF(&temp, ep2->v.i);
                else
                    LongLongToFPF(&temp, ep2->v.i);
                AddSubFPF(0,&ep1->v.f, &temp, &ep->v.f);
                refloat(ep);
                break;
            case 4:
                ep->type = maxfloattype(ep1, ep2);
                AddSubFPF(0,&ep1->v.f, &ep2->v.f, &ep->v.f);
                refloat(ep);
                break;
            case 9:
                ep->type = maxcomplextype(ep1,ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                ep->v.c.r = ep1->v.f ;
                ep->v.c.i = ep2->v.f;
                refloat(ep);
                break ;
            case 10:
                ep->type = maxcomplextype(ep1,ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                ep->v.c.r = ep2->v.f ;
                ep->v.c.i = ep1->v.f;
                refloat(ep);
                break ;
            case 11:
                ep->type = maximaginarytype(ep1,ep2);
                AddSubFPF(0,&ep1->v.f, &ep2->v.f, &ep->v.f);
                refloat(ep);
                break ;
            case 14:
                ep->type = maxcomplextype(ep1,ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                LongLongToFPF(&ep->v.c.r, ep2->v.i);
                ep->v.c.i = ep1->v.f ;
                refloat(ep);
                break;
            case 15:
                ep->type = maxcomplextype(ep1,ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                LongLongToFPF(&ep->v.c.r, ep1->v.i);
                ep->v.c.i = ep2->v.f ;
                refloat(ep);
                break;
            case 16:
                ep->type = maxcomplextype(ep1,ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                ep->v.c.r = ep2->v.c.r ;
                AddSubFPF(0,&ep1->v.f, &ep2->v.c.i, &ep->v.c.i);
                refloat(ep);
                break;
            case 17:
                ep->type = maxcomplextype(ep1,ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                ep->v.c.r = ep1->v.c.r ;
                AddSubFPF(0,&ep2->v.f, &ep1->v.c.i, &ep->v.c.i);
                refloat(ep);
                break;
            case 18:
                ep->type = maxcomplextype(ep1,ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                AddSubFPF(0,&ep1->v.f, &ep2->v.c.r, &ep->v.c.r);
                ep->v.c.i = ep2->v.c.i;
                refloat(ep);
                break;
            case 19:
                ep->type = maxcomplextype(ep1,ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                AddSubFPF(0,&ep2->v.f, &ep1->v.c.r, &ep->v.c.r);
                ep->v.c.i = ep1->v.c.i;
                refloat(ep);
                break;
            case 20:
                ep->type = maxcomplextype(ep1,ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                AddSubFPF(0,&ep1->v.c.r, &ep2->v.c.r, &ep->v.c.r);
                AddSubFPF(0,&ep1->v.c.i, &ep2->v.c.i, &ep->v.c.i);
                refloat(ep);
                break;
            case 23:
                ep->type = maxcomplextype(ep1,ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                if (isunsignedexpr(ep2))
                    UnsignedLongLongToFPF(&temp, ep2->v.i);
                else
                    LongLongToFPF(&temp, ep2->v.i);
                AddSubFPF(0,&ep1->v.c.r, &temp, &ep->v.c.r);
                ep->v.c.i = ep1->v.c.i ;
                refloat(ep);
                break;
            case 24:
                ep->type = maxcomplextype(ep1,ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&temp, ep1->v.i);
                else
                    LongLongToFPF(&temp, ep1->v.i);
                AddSubFPF(0,&ep2->v.c.r, &temp, &ep->v.c.r);
                ep->v.c.i = ep2->v.c.i ;
                refloat(ep);
                break;
            default:
                break;
            }
            break;
        case en_sub:
            switch (mode)
            {
            case 1:
                ep->type = maxinttype(ep1, ep2);
                ep->v.i = ep1->v.i - ep2->v.i;
                ep->v.i = reint(ep);
                break;
            case 2:
                ep->type = maxfloattype(ep1, ep2);
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&temp, ep1->v.i);
                else
                    LongLongToFPF(&temp, ep1->v.i);
                AddSubFPF(1,&temp, &ep2->v.f, &ep->v.f);
                refloat(ep);
                break;
            case 3:
                ep->type = maxfloattype(ep1, ep2);
                if (isunsignedexpr(ep2))
                    UnsignedLongLongToFPF(&temp, ep2->v.i);
                else
                    LongLongToFPF(&temp, ep2->v.i);
                AddSubFPF(1,&ep1->v.f, &temp, &ep->v.f);
                refloat(ep);
                break;
            case 4:
                ep->type = maxfloattype(ep1, ep2);
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&temp, ep1->v.i);
                else
                    LongLongToFPF(&temp, ep1->v.i);
                AddSubFPF(1,&ep1->v.f, &ep2->v.f, &ep->v.f);
                refloat(ep);
                break;
            case 9:
                ep->type = maxcomplextype(ep1,ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                ep->v.c.r = ep1->v.f ;
                ep->v.c.i = ep2->v.f;
                ep->v.c.i.sign ^= 1;
                refloat(ep);
                break ;
            case 10:
                ep->type = maxcomplextype(ep1,ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                ep->v.c.r = ep2->v.f ;
                ep->v.c.r.sign ^= 1;
                ep->v.c.i = ep1->v.f;
                refloat(ep);
                break ;
            case 11:
                ep->type = maximaginarytype(ep1,ep2);
                AddSubFPF(1,&ep1->v.f, &ep2->v.f, &ep->v.f);
                refloat(ep);
                break ;
            case 14:
                ep->type = maxcomplextype(ep1,ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                if (isunsignedexpr(ep2))
                    UnsignedLongLongToFPF(&ep->v.c.r, ep2->v.i);
                else
                    LongLongToFPF(&ep->v.c.r, ep2->v.i);
                ep->v.c.i = ep1->v.f ;
                refloat(ep);
                break;
            case 15:
                ep->type = maxcomplextype(ep1,ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&ep->v.c.r, ep1->v.i);
                else
                    LongLongToFPF(&ep->v.c.r, ep1->v.i);
                ep->v.c.i = ep2->v.f ;
                ep->v.c.i.sign ^= 1;
                refloat(ep);
                break;
            case 16:
                ep->type = maxcomplextype(ep1,ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                ep->v.c.r = ep2->v.c.r ;
                ep->v.c.r.sign ^= 1;
                AddSubFPF(1,&ep1->v.f, &ep2->v.c.i, & ep->v.c.i);
                refloat(ep);
                break;
            case 17:
                ep->type = maxcomplextype(ep1,ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                ep->v.c.r = ep1->v.c.r ;
                AddSubFPF(1,&ep1->v.c.i, &ep2->v.f, & ep->v.c.i);
                refloat(ep);
                break;
            case 18:
                ep->type = maxcomplextype(ep1,ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                AddSubFPF(1,&ep1->v.f, &ep2->v.c.r, & ep->v.c.r);
                ep->v.c.i = ep2->v.c.i;
                ep->v.c.i.sign ^= 1;
                refloat(ep);
                break;
            case 19:
                ep->type = maxcomplextype(ep1,ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                AddSubFPF(1,&ep1->v.c.r, &ep2->v.f, & ep->v.c.r);
                ep->v.c.i = ep1->v.c.i;
                refloat(ep);
                break;
            case 20:
                ep->type = maxcomplextype(ep1,ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                AddSubFPF(1,&ep1->v.c.r, &ep2->v.c.r, & ep->v.c.r);
                AddSubFPF(1,&ep1->v.c.i, &ep2->v.c.i, & ep->v.c.i);
                refloat(ep);
                break;
            case 23:
                ep->type = maxcomplextype(ep1,ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                if (isunsignedexpr(ep2))
                    UnsignedLongLongToFPF(&temp, ep2->v.i);
                else
                    LongLongToFPF(&temp, ep2->v.i);
                AddSubFPF(1,&ep1->v.c.r, &temp, & ep->v.c.r);
                ep->v.c.i = ep1->v.c.i ;
                refloat(ep);
                break;
            case 24:
                ep->type = maxcomplextype(ep1,ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&temp, ep1->v.i);
                else
                    LongLongToFPF(&temp, ep1->v.i);
                AddSubFPF(1,&temp, &ep2->v.c.r, & ep->v.c.r);
                ep->v.c.i = ep2->v.c.i ;
                ep->v.c.i.sign ^= 1;
                refloat(ep);
                break;
            default:
                break;
            }
            break;
        case en_arraymul:
        case en_umul:
        case en_mul:
            switch (mode)
            {
            case 1:
                ep->type = maxinttype(ep1, ep2);
                ep->v.i = ep1->v.i *ep2->v.i;
                ep->v.i = reint(ep);
                break;
            case 2:
                ep->type = maxfloattype(ep1, ep2);
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&temp, ep1->v.i);
                else
                    LongLongToFPF(&temp, ep1->v.i);
                MultiplyFPF(&temp, & ep2->v.f, &ep->v.f);
                refloat(ep);
                break;
            case 3:
                ep->type = maxfloattype(ep1, ep2);
                if (isunsignedexpr(ep2))
                    UnsignedLongLongToFPF(&temp, ep2->v.i);
                else
                    LongLongToFPF(&temp, ep2->v.i);
                MultiplyFPF(&ep1->v.f, &temp, &ep->v.f);
                refloat(ep);
                break;
            case 4:
                ep->type = maxfloattype(ep1, ep2);
                MultiplyFPF(&ep1->v.f, &ep2->v.f, &ep->v.f);
                refloat(ep);
                break;
            case 9:
                ep->type = maximaginarytype(ep1, ep2);
                MultiplyFPF(&ep1->v.f, &ep2->v.f, &ep->v.f);
                refloat(ep);
                break;
            case 10:
                ep->type = maximaginarytype(ep1, ep2);
                MultiplyFPF(&ep1->v.f, &ep2->v.f, &ep->v.f);
                refloat(ep);
                break;
            case 11:
                ep->type = maxfloattype(ep1, ep2);
                MultiplyFPF(&ep1->v.f, &ep2->v.f, &ep->v.f);
                ep->v.f.sign ^= 1;
                refloat(ep);
                break;
            case 14:
                ep->type = maximaginarytype(ep1, ep2);
                if (isunsignedexpr(ep2))
                    UnsignedLongLongToFPF(&temp, ep2->v.i);
                else
                    LongLongToFPF(&temp, ep2->v.i);
                MultiplyFPF(&ep1->v.f, &temp, &ep->v.f);
                refloat(ep);
                break;
            case 15:
                ep->type = maximaginarytype(ep1, ep2);
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&temp, ep1->v.i);
                else
                    LongLongToFPF(&temp, ep1->v.i);
                MultiplyFPF(&temp, &ep2->v.f, &ep->v.f);
                refloat(ep);
                break;
            case 16:
                ep->type = maxcomplextype(ep1, ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                MultiplyFPF(&ep1->v.f, &ep2->v.c.i, &ep->v.c.r);
                ep->v.c.r.sign ^= 1;
                MultiplyFPF(&ep1->v.f, &ep2->v.c.r, &ep->v.c.i);
                refloat(ep);
                break;
            case 17:
                ep->type = maxcomplextype(ep1, ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                MultiplyFPF(&ep2->v.f, &ep1->v.c.i, &ep->v.c.r);
                ep->v.c.r.sign ^= 1;
                MultiplyFPF(&ep2->v.f, &ep1->v.c.r, &ep->v.c.i);
                refloat(ep);
                break;
            case 18:
                ep->type = maxcomplextype(ep1, ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                MultiplyFPF(&ep1->v.f, &ep2->v.c.r, &ep->v.c.r);
                MultiplyFPF(&ep1->v.f, &ep2->v.c.i, &ep->v.c.i);
                refloat(ep);
                break;
            case 19:
                ep->type = maxcomplextype(ep1, ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                MultiplyFPF(&ep2->v.f, &ep1->v.c.r, &ep->v.c.r);
                MultiplyFPF(&ep2->v.f, &ep1->v.c.i, &ep->v.c.i);
                refloat(ep);
                break;
            case 20:
                if (ep1->v.c.r.type == IFPF_IS_ZERO)
                {
                    ep->type = maxcomplextype(ep1, ep2);
                    ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                        && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                            STD_PRAGMA_CXLIMITED : 0;
                    MultiplyFPF(&ep2->v.c.r, &ep1->v.c.i, &ep->v.c.i);
                    MultiplyFPF(&ep2->v.c.i, &ep1->v.c.i, &ep->v.c.r);
                    ep->v.c.r.sign ^= 1;
                }
                else if (ep1->v.c.i.type == IFPF_IS_ZERO)
                {
                    ep->type = maxcomplextype(ep1, ep2);
                    ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                        && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                            STD_PRAGMA_CXLIMITED : 0;
                    MultiplyFPF(&ep2->v.c.r, &ep1->v.c.r, &ep->v.c.r);
                    MultiplyFPF(&ep2->v.c.i, &ep1->v.c.r, &ep->v.c.i);
                }
                else if (ep2->v.c.r.type == IFPF_IS_ZERO)
                {
                    ep->type = maxcomplextype(ep1, ep2);
                    ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                        && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                            STD_PRAGMA_CXLIMITED : 0;
                    MultiplyFPF(&ep1->v.c.r, &ep2->v.c.i, &ep->v.c.i);
                    MultiplyFPF(&ep1->v.c.i, &ep2->v.c.i, &ep->v.c.r);
                    ep->v.c.r.sign ^= 1;
                }
                else if (ep2->v.c.i.type == IFPF_IS_ZERO)
                {
                    ep->type = maxcomplextype(ep1, ep2);
                    ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                        && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                            STD_PRAGMA_CXLIMITED : 0;
                    MultiplyFPF(&ep1->v.c.r, &ep2->v.c.r, &ep->v.c.r);
                    MultiplyFPF(&ep1->v.c.i, &ep2->v.c.r, &ep->v.c.i);
                }
                refloat(ep);
                break ;
            case 24:
                ep->type = maxcomplextype(ep1, ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&temp, ep1->v.i);
                else
                    LongLongToFPF(&temp, ep1->v.i);
                MultiplyFPF(&temp, &ep2->v.c.r, &ep->v.c.r);
                MultiplyFPF(&temp, &ep2->v.c.i, &ep->v.c.i);
                refloat(ep);
                break;
            default:
                break;
            }
            break;
        case en_arraydiv:
        case en_div:
        case en_udiv:
            switch (mode)
            {
            case 1:
                if (ep2->v.i)
                {
                    ep->type = maxinttype(ep1, ep2);
                    if (isunsignedexpr(ep1) || isunsignedexpr(ep2))
                        ep->v.i = (ULLONG_TYPE)ep1->v.i / (ULLONG_TYPE)ep2->v.i;
                    else
                        ep->v.i = ep1->v.i / ep2->v.i;
                    
                    ep->v.i = reint(ep);
                }
                break;
            case 2: 
                ep->type = maxfloattype(ep1, ep2);
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&temp, ep1->v.i);
                else
                    LongLongToFPF(&temp, ep1->v.i);
                DivideFPF(&temp, &ep2->v.f, &ep->v.f);
                refloat(ep);
                break;
            case 3:
                ep->type = maxfloattype(ep1, ep2);
                if (isunsignedexpr(ep2)) 
                    UnsignedLongLongToFPF(&temp, ep2->v.i);
                else
                    LongLongToFPF(&temp, ep2->v.i);
                DivideFPF(&ep1->v.f, &temp, &ep->v.f);
                refloat(ep);
                break;
            case 4:
                ep->type = maxfloattype(ep1, ep2);
                DivideFPF(&ep1->v.f, &ep2->v.f, &ep->v.f);
                refloat(ep);
                break;
            case 8: // convert to a multiply
            {
                FPF temp;
                UnsignedLongLongToFPF(&temp, 1);
                DivideFPF(&temp, &ep2->v.f, &temp);
                ep2->v.f = temp;
                ep->type = en_mul;
            }
                break;
            case 9:
                ep->type = maximaginarytype(ep1, ep2);
                DivideFPF(&ep1->v.f, &ep2->v.f, &ep->v.f);
                ep->v.f.sign ^= 1;
                refloat(ep);
                break;
            case 10:
                ep->type = maximaginarytype(ep1, ep2);
                DivideFPF(&ep1->v.f, &ep2->v.f, &ep->v.f);
                refloat(ep);
                break;
            case 11:
                ep->type = maximaginarytype(ep1, ep2);
                DivideFPF(&ep1->v.f, &ep2->v.f, &ep->v.f);
                refloat(ep);
                break;
            case 14:
                ep->type = maximaginarytype(ep1, ep2);
                if (isunsignedexpr(ep2))
                    UnsignedLongLongToFPF(&temp, ep2->v.i);
                else
                    LongLongToFPF(&temp, ep2->v.i);
                DivideFPF(&ep1->v.f, &temp, &ep->v.f);
                refloat(ep);
                break;
            case 15:
                ep->type = maximaginarytype(ep1, ep2);
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&temp, ep1->v.i);
                else
                    LongLongToFPF(&temp, ep1->v.i);
                DivideFPF(&temp, &ep2->v.f, &ep->v.f);
                ep->v.f.sign ^= 1;
                refloat(ep);
                break;
            case 17:
                ep->type = maxcomplextype(ep1, ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                DivideFPF(&ep1->v.c.i, &ep2->v.f, &ep->v.c.r);
                DivideFPF(&ep1->v.c.r, &ep2->v.f, &ep->v.c.i);
                ep->v.c.i.sign ^= 1;
                refloat(ep);
                break;
            case 19:
                ep->type = maxcomplextype(ep1, ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                DivideFPF(&ep1->v.c.i, &ep2->v.f, &ep->v.c.r);
                DivideFPF(&ep1->v.c.r, &ep2->v.f, &ep->v.c.i);
                refloat(ep);
                break;
            case 20:
                if (ep2->v.c.r.type == IFPF_IS_ZERO)
                {
                    ep->type = maxcomplextype(ep1, ep2);
                    ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                        && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                            STD_PRAGMA_CXLIMITED : 0;
                    DivideFPF(&ep1->v.c.r, &ep2->v.c.i, &ep->v.c.i);
                    DivideFPF(&ep1->v.c.i, &ep2->v.c.i, &ep->v.c.r);
                    ep->v.c.i.sign ^= 1;
                }
                else if (ep2->v.c.i.type == IFPF_IS_ZERO)
                {
                    ep->type = maxcomplextype(ep1, ep2);
                    ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                        && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                            STD_PRAGMA_CXLIMITED : 0;
                    DivideFPF(&ep1->v.c.r, &ep2->v.c.r, &ep->v.c.r);
                    DivideFPF(&ep1->v.c.i, &ep2->v.c.r, &ep->v.c.i);
                }
                refloat(ep);
                break ;
            case 23:
                ep->type = maxcomplextype(ep1, ep2);
                ep->pragmas |= ((ep1->pragmas &STD_PRAGMA_CXLIMITED)
                    && (ep2->pragmas & STD_PRAGMA_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                if (isunsignedexpr(ep2))
                    UnsignedLongLongToFPF(&temp, ep2->v.i);
                else
                    LongLongToFPF(&temp, ep2->v.i);
                DivideFPF(&ep1->v.c.r, &temp, &ep->v.c.r);
                DivideFPF(&ep1->v.c.i, &temp, &ep->v.c.i);
                refloat(ep);
                break;
            default:
                break;
            }
            break;
        case en_mod:
        case en_umod:
            if (ep2->v.i)
            {
                ep->type = maxinttype(ep1, ep2);
    
                if (isunsignedexpr(ep1) || isunsignedexpr(ep2))
                    ep->v.i = (ULLONG_TYPE)ep1->v.i % (ULLONG_TYPE)ep2->v.i;
                else
                    ep->v.i = ep1->v.i % ep2->v.i;
                ep->v.i = reint(ep);
            }
            break;
        case en_arraylsh:
        case en_lsh:
            ep->type = ep1->type; /* maxinttype(ep1, ep2);*/
            ep->v.i = ep1->v.i << ep2->v.i;
            ep->v.i = reint(ep);
            break;
        case en_ursh:
            ep->type = ep1->type; /* maxinttype(ep1, ep2);*/
            ep1->v.i = reint(ep1);
            ep->v.i = ((ULLONG_TYPE)ep1->v.i) >> ep2->v.i;
            ep->v.i = reint(ep);
            break;
        case en_rsh:
            ep->type = ep1->type; /* maxinttype(ep1, ep2);*/
            ep->v.i = ep1->v.i >> ep2->v.i;
            ep->v.i = reint(ep);
            break;
        case en_and:
            ep->type = maxinttype(ep1, ep2);
            ep->v.i = ep1->v.i &ep2->v.i;
            ep->v.i = reint(ep);
            break;
        case en_or:
            ep->type = maxinttype(ep1, ep2);
            ep->v.i = ep1->v.i | ep2->v.i;
            ep->v.i = reint(ep);
            break;
        case en_xor:
            ep->type = maxinttype(ep1, ep2);
            ep->v.i = ep1->v.i ^ ep2->v.i;
            ep->v.i = reint(ep);
            break;
        default:
            break;
    }
}

int pwrof2(LLONG_TYPE i)
/*
 *      return which power of two i is or -1.
 */
{
    if (i > 1) 
    {
        int top = sizeof(shifts)/sizeof(LLONG_TYPE);
        int bottom =  - 1;
        while (top - bottom > 1)
        {
            int mid = (top + bottom) / 2;
            if (i < shifts[mid])
            {
                top = mid;
            }
            else
            {
                bottom = mid;
            }
        }
        if (bottom <= 0) /* ignore the oth power*/
            return -1;
        if (i == shifts[bottom])
            return bottom;
    }
    return -1 ;
}

/*-------------------------------------------------------------------------*/

LLONG_TYPE mod_mask(int i)
/*
 *      make a mod mask for a power of two.
 */
{
    if (i >= sizeof(shifts)/sizeof(LLONG_TYPE))
        return (LLONG_TYPE)-1;
    return shifts[i] - 1;		
}

/*-------------------------------------------------------------------------*/

void addaside(EXPRESSION *node)
{
    *asidetail = exprNode(en_void, node, 0);
    asidetail = &(*asidetail)->right;
}

/*-------------------------------------------------------------------------*/

int opt0(EXPRESSION **node)
/*
 *      opt0 - delete useless expressions and combine constants.
 *
 *      opt0 will delete expressions such as x + 0, x - 0, x * 0,
 *      x * 1, 0 / x, x / 1, x mod 0, etc from the tree pointed to
 *      by node and combine obvious constant operations. It cannot
 *      combine name and label constants but will combine icon type
 *      nodes.
 *
 * en_arrayadd is not optimized to keep from optimizing struct members
 * into regs later...  just en: en_arrayadd is now equiv to en_add
 * again.  I have fixed the optimize struct thing another way...
 * leaving en_arrayadd in in case we need it later.
 */
{
    EXPRESSION *ep;
    LLONG_TYPE val;
    int sc;
    int rv = FALSE;
    int mode;
    FPF dval;
    int negtype = en_uminus;

    ep =  *node;

    if (ep == 0)
        return FALSE ;
    switch (ep->type)
    {
        case en_l_sp:
        case en_l_fp:
        case en_l_bool:
        case en_x_bool:
        case en_l_bit:
        case en_x_bit:
        case en_l_wc:
        case en_l_u16:
        case en_l_u32:
        case en_l_c:
        case en_l_s:
             /* optimize unary node */
        case en_l_uc:
        case en_l_us:
             /* optimize unary node */
        case en_l_p:
        case en_l_ref:
        case en_l_i:
        case en_l_ui:
        case en_l_l:
        case en_l_ul:
        case en_l_ll:
        case en_l_ull:
        case en_l_f:
        case en_l_d:
        case en_l_ld:
        case en_l_fi:
        case en_l_di:
        case en_l_ldi:
        case en_l_fc:
        case en_l_dc:
        case en_l_ldc:
        case en_x_wc:
        case en_x_c:
        case en_x_uc:
        case en_x_u16:
        case en_x_u32:
        case en_x_s:
        case en_x_us:
        case en_x_l:
        case en_x_ul:
        case en_x_i:
        case en_x_ui:
        case en_x_ll:
        case en_x_ull:
        case en_x_f:
        case en_x_d:
        case en_x_p:
        case en_x_ld:
        case en_x_fp:
        case en_x_sp:
        case en_x_dc:
        case en_x_fc:
        case en_x_ldc:
        case en_x_di:
        case en_x_fi:
        case en_x_ldi:
        case en_shiftby:
        case en_bits:
        case en_literalclass:
            rv |= opt0(&((*node)->left));
            return rv ;
        case en_compl:
            rv |= opt0(&(ep->left));
            if (isintconst(ep->left))
            {
                rv = TRUE;
                ep->type = ep->left->type;
                ep->v.i = ~ep->left->v.i;
                ep->v.i = reint(ep);
                ep->unionoffset = ep->left->unionoffset;
            }
            return rv ;

        case en_uminus:
            rv |= opt0(&(ep->left));
            if (isintconst(ep->left))
            {
                *node = intNode(en_c_i, -ep->left->v.i);
                rv = TRUE;
            }
            else if (isfloatconst(ep->left))
            {
                *node = intNode(ep->left->type, 0);
                (*node)->v.f = ep->left->v.f;
                (*node)->v.f.sign ^= 1;                
                rv = TRUE;
            }
            else if (ep->left->type == en_c_d || ep->left->type ==
                en_c_f || ep->left->type == en_c_ld ||
                ep->left->type == en_c_di || ep->left->type ==
                en_c_fi || ep->left->type == en_c_ldi)
            {
                rv = TRUE;
                ep->type = ep->left->type;
                ep->v.f =  ep->left->v.f;
                ep->v.f.sign ^= 1;
                *node = ep;
            }
            else if (ep->left->type == en_c_dc || ep->left->type ==
                en_c_fc || ep->left->type == en_c_ldc)
            {
                rv = TRUE;
                ep->type = ep->left->type;
                ep->v.c.r =  ep->left->v.c.r;
                ep->v.c.r.sign ^= 1;
                ep->v.c.i =  ep->left->v.c.i;
                ep->v.c.i.sign ^= 1;
                *node = ep;
            }
            return rv ;
        case en_structadd:
        case en_add:
        case en_arrayadd:
        case en_sub:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            if (ep->right->type == en_structelem)
                break;
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
            case 1:
            case 2:
            case 3:
            case 4:
            default:
                rv = TRUE;
                dooper(node, mode);
                break;
            case 5:
                if (ep->left->v.i == 0)
                {
                    if (ep->left->type != en_auto)
                    {
                        if (ep->type == en_sub)
                            *node = exprNode(en_uminus, ep->right, 0);
                        else
                            *node = ep->right;
                        rv = TRUE;
                    }
                }
                else
                {
                    dooper(node, mode);
                    rv = TRUE;
                }
                break;
            case 6:
                if (ep->left->v.f.type == IFPF_IS_ZERO)
                {
                    if (ep->type == en_sub)
                    {
                        ep->right->v.f.sign ^= 1 ;

                    }
                    *node = ep->right;
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            case 7:
                if (ep->right->v.i == 0)
                {
                    if (ep->left->type != en_auto)
                    {
                        *node = ep->left;
                        rv = TRUE;
                    }
                }
                else
                {
                    dooper(node, mode);
                    rv = TRUE;
                }
                break;
            case 8:
                if (ep->right->v.f.type == IFPF_IS_ZERO)
                {
                    *node = ep->left;
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            case 22:
                if (ep->right->v.c.r.type == IFPF_IS_ZERO &&
                    ep->right->v.c.i.type == IFPF_IS_ZERO)
                {
                    *node = ep->left;
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            }
            return rv ;
        case en_mul:
        case en_umul:
        case en_arraymul:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
            default:
                break;
            case 1:
            case 2:
            case 3:
            case 4:
            case 9:
            case 10:
            case 11:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
                dooper(node, mode);
                rv = TRUE;
                break;
            case 5:
                if (!hasFloats(ep->right))
                {
                    val = ep->left->v.i;
                    if (val == 0)
                    {
                        addaside(ep->right);
                        *node = ep->left;
                    }
                    else if (val == 1)
                        *node = ep->right;
                    else if (val ==  - 1)
                        *node = exprNode(negtype, ep->right, 0);
                    else
                    {
                        LLONG_TYPE i = pwrof2(val);
                        if (i != -1)
                        {
                            EXPRESSION *x = ep->left;
                            ep->left = ep->right;
                            ep->right = x;
                            ep->right->v.i = i;
                            rv = TRUE;
                            switch (ep->type)
                            {
                            case en_mul:
                                ep->type = en_lsh;
                                break;
                            case en_umul:
                                ep->type = en_lsh;
                                break;
                            case en_arraymul:
                                ep->type = en_arraylsh;
                                break;
                            default:
                                break;
                            }
                            break;
                        }
                    }
                }
                dooper(node, mode);
                rv = TRUE;
                break;
            case 6:
                dval = ep->left->v.f;
#ifdef XXXXX
                if (dval.type == IFPF_IS_ZERO)
                {
                    addaside(ep->right);
                    *node = ep->left;
                }
                else 
#endif
                if (!dval.sign && ValueIsOne(&dval))
                    *node = ep->right;
                else if (dval.sign && ValueIsOne(&dval))
                    *node = exprNode(negtype, ep->right, 0);
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            case 7:
                if (!hasFloats(ep->left))
                {
                    val = ep->right->v.i;
                    if (val == 0)
                    {
                        addaside(ep->left);
                        *node = ep->right;
                    }
                    else if (val == 1)
                    {
                        *node = ep->left;
                    }
                    else if (val ==  - 1)
                    {
                        *node = exprNode(negtype, ep->left, 0);
                    }
                    else
                    {
                        LLONG_TYPE i = pwrof2(val);
                        if (i != -1)
                        {
                            ep->right->v.i = i;
                            rv = TRUE;
                            switch (ep->type)
                            {
                            case en_mul:
                                ep->type = en_lsh;
                                break;
                            case en_umul:
                                ep->type = en_lsh;
                                break;
                            case en_arraymul:
                                ep->type = en_arraylsh;
                                break;
                            default:
                                break;
                            }
                            break;
                        }
                    }
                }
                dooper(node, mode);
                rv = TRUE;
                break;
            case 8:
                dval = ep->right->v.f;
                if (!dval.sign && ValueIsOne(&dval))
                {
                    *node = ep->left;
                }
                else if (dval.sign && ValueIsOne(&dval))
                {
                    *node = exprNode(negtype, ep->left, 0);
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            case 22:
                dval = ep->right->v.c.r;
                if (ep->right->v.c.i.type == IFPF_IS_ZERO)
                {
                    if (!dval.sign && ValueIsOne(&dval))
                        *node = ep->left;
                    else if (dval.sign && ValueIsOne(&dval))
                    {
                        *node = exprNode(negtype, ep->left, 0);
                    }
                    else
                        dooper(node, mode);
                }
            }
            break;
        case en_arraydiv:
        case en_div:
        case en_udiv:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
            case 1:
            case 2:
            case 3:
            case 4:
            case 20:
                dooper(node, mode);
                rv = TRUE;
                break;
            case 5:
                if (ep->left->v.i == 0)
                {
                    addaside(ep->right);
                    *node = ep->left;
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            case 6:
                dooper(node, mode);
                rv = TRUE;
                break;
            case 7:
                if (!hasFloats(ep->left))
                {
                    val = ep->right->v.i;
                    if (val == 1)
                        *node = ep->left;
                    else if (val ==  - 1)
                        *node = exprNode(negtype, ep->left, 0);
                }
                dooper(node, mode);
                rv = TRUE;
                break;
            case 8:
                dval = ep->right->v.f;
                if (!dval.sign && ValueIsOne(&dval))
                    *node = ep->left;
                if (dval.sign && ValueIsOne(&dval))
                    *node = exprNode(negtype, ep->left, 0);
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
              case 22:
                dval = ep->right->v.c.r;
                if (ep->right->v.c.i.type == IFPF_IS_ZERO)
                {
                    if (!dval.sign && ValueIsOne(&dval))
                        *node = ep->left;
                    else if (dval.sign && ValueIsOne(&dval))
                    {
                        *node = exprNode(negtype, ep->left, 0);
                    }
                    else
                        dooper(node, mode);
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            }
            break;
        case en_mod:
        case en_umod:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
            default:
                break;
            case 7:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 8:
                dooper(node, mode);
                rv = TRUE;
                break;
            }
            break;
        case en_and:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
            default:
                break;
            case 1:
                dooper(node, mode);
                rv = TRUE;
                break;
            case 5:
                if (ep->left->v.i == 0)
                {
                    addaside(ep->right);
                    *node = ep->left;
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            case 7:
                if (ep->right->v.i == 0)
                {
                    addaside(ep->left);
                    *node = ep->right;
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            }
            break;
        case en_or:
        case en_xor:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
            default:
                break;
            case 1:
                dooper(node, mode);
                rv = TRUE;
                break;
            case 5:
                if (ep->left->v.i == 0)
                {
                    *node = ep->right;
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            case 7:
                if (ep->right->v.i == 0)
                {
                    *node = ep->left;
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            }
            break;
        case en_rsh:
        case en_lsh:
        case en_arraylsh:
        case en_ursh:
            rv |= opt0(&(ep->left));
            if (isintconst(ep->left) && ep->right->type ==
                en_shiftby)
                if (isintconst(ep->right->left))
                {
                    ep->right = ep->right->left;
                    rv = TRUE;
                }
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
            default:
                break;
            case 1:
                dooper(node, mode);
                rv = TRUE;
                break;
            case 5:
                if (ep->left->v.i == 0)
                {
                    addaside(ep->right);
                    *node = ep->left;
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            case 7:
                if (ep->right->v.i == 0)
                {
                    *node = ep->left;
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            }
            break;
        case en_land:
            rv |= opt0(&(ep->left));
            if (isintconst(ep->left))
            {
                if (!ep->left->v.i)
                {
                    rv = TRUE;
                    *node = intNode(en_c_i, 0);
                    break;
                }
                else 
                {
                    goto join_land;
                }
            }
            else switch(ep->left->type)
            {
                case en_label:
                case en_pc:
                case en_threadlocal:
                case en_global:
                case en_labcon:
                case en_auto:
                {
join_land:
                    rv |= opt0(&(ep->right));
                    if (isintconst(ep->right))
                    {
                        rv = TRUE;
                        *node = intNode(en_c_i, !!ep->right->v.i);
                    }
                    else switch(ep->right->type)
                    {
                        case en_label:
                        case en_pc:
                        case en_threadlocal:
                        case en_global:
                        case en_labcon:
                        case en_auto:
                            /* assumes nothing can be relocated to address 0 */
                            *node = intNode(en_c_i, 1);
                            rv = TRUE;
                            break;
                        default:
                            break;
                    }
                }
                default:
                    rv |= opt0(&(ep->right));
                    break;
            }
            break;
        case en_lor:
            rv |= opt0(&(ep->left));
            if (isintconst(ep->left))
            {
                if (ep->left->v.i)
                {
                    rv = TRUE;
                    *node = intNode(en_c_i, 1);
                    break;
                }
                else 
                {
                    goto join_lor;
                }
            }
            else switch(ep->left->type)
            {
                case en_label:
                case en_pc:
                case en_threadlocal:
                case en_global:
                case en_labcon:
                case en_auto:
                    /* assumes nothing can be relocated to address 0 */
                    *node = intNode(en_c_i, 1);
                    rv = TRUE;
                    break;
                default:
                {
join_lor:
                    rv |= opt0(&(ep->right));
                    if (isintconst(ep->right))
                    {
                        if (ep->right->v.i)
                        {
                            rv = TRUE;
                            *node = intNode(en_c_i, 1);
                        }
                        else if (isintconst(ep->left) && !ep->left->v.i)
                        {
                            rv = TRUE;
                            *node = intNode(en_c_i, 0);
                        }
                    }
                    else switch(ep->right->type)
                    {
                        case en_label:
                        case en_pc:
                        case en_threadlocal:
                        case en_global:
                        case en_labcon:
                        case en_auto:
                            /* assumes nothing can be relocated to address 0 */
                            *node = intNode(en_c_i, 1);
                            rv = TRUE;
                            break;
                        default:
                            break;
                    }
                }
            }
            break;
        case en_not:
            rv |= opt0(&(ep->left));
            if (isintconst(ep->left))
            {
                *node = intNode(en_c_i, (!ep->left->v.i));
                rv = TRUE;
            }
            else if (isfloatconst(ep->left))
            {
                *node = intNode(en_c_i, ep->left->v.f.type == IFPF_IS_ZERO);
                rv = TRUE;
            }
            break;
        case en_eq:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
            case 1:
                *node = intNode(en_c_i, (ep->left->v.i == ep->right
                    ->v.i));
                rv = TRUE;

                break;
            case 4:
                *node = intNode(en_c_i, FPFEQ(&ep->left->v.f, &ep->right->v.f));
                rv = TRUE;
                break;
            default:
                break;
            }
            break;
        case en_ne:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
            case 1:
                *node = intNode(en_c_i, (ep->left->v.i != ep->right
                    ->v.i));
                rv = TRUE;
                break;
            case 4:
                *node = intNode(en_c_i, !FPFEQ(&ep->left->v.f, &ep->right->v.f));
                rv = TRUE;
                break;
            default:
                break;
            }
            break;
        case en_lt:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
            case 1:
                *node = intNode(en_c_i, (ep->left->v.i < ep->right->v.i)
                    );
                rv = TRUE;
                break;
            case 4:
                *node = intNode(en_c_i, !FPFGTE(&ep->left->v.f, &ep->right->v.f));
                rv = TRUE;
                break;
            default:
                break;
            }
            break;
        case en_le:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
            case 1:
                *node = intNode(en_c_i, (ep->left->v.i <= ep->right
                    ->v.i));
                rv = TRUE;
                break;
            case 4:
                *node = intNode(en_c_i, !FPFGT(&ep->left->v.f, &ep->right->v.f));
                rv = TRUE;
                break;
            default:
                break;
            }
            break;
        case en_ugt:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
            case 1:
                *node = intNode(en_c_i, ((ULLONG_TYPE)ep->left->v.i > 
                    (ULLONG_TYPE)ep->right->v.i));
                rv = TRUE;
                break;
            default:
                break;
            }
            break;
        case en_uge:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
            case 1:
                *node = intNode(en_c_i, ((ULLONG_TYPE)ep->left->v.i >= 
                    (ULLONG_TYPE)ep->right->v.i));
                rv = TRUE;
                break;
            default:
                break;
            }
            break;
        case en_ult:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
            case 1:
                *node = intNode(en_c_i, ((ULLONG_TYPE)ep->left->v.i < 
                    (ULLONG_TYPE)ep->right->v.i));
                rv = TRUE;
                break;
            default:
                break;
            }
            break;
        case en_ule:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
            case 1:
                *node = intNode(en_c_i, ((ULLONG_TYPE)ep->left->v.i <= 
                    (ULLONG_TYPE)ep->right->v.i));
                rv = TRUE;
                break;
            default:
                break;
            }
            break;
        case en_gt:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
            case 1:
                *node = intNode(en_c_i, (ep->left->v.i > ep->right->v.i)
                    );
                rv = TRUE;
                break;
            case 4:
                *node = intNode(en_c_i, FPFGT(&ep->left->v.f, &ep->right->v.f));
                rv = TRUE;
                break;
            default:
                break;
            }
            break;
        case en_ge:
            rv |= opt0(&(ep->left));
            rv |= opt0(&(ep->right));
            mode = getmode(ep->left, ep->right);
            switch (mode)
            {
            case 1:
                *node = intNode(en_c_i, (ep->left->v.i >= ep->right
                    ->v.i));
                rv = TRUE;
                break;
            case 4:
                *node = intNode(en_c_i, !FPFGTE(&ep->left->v.f, &ep->right->v.f));
                rv = TRUE;
                break;
            default:
                break;
            }
            break;


        case en_cond:
            opt0(&ep->left);
            if (isoptconst(ep->left)) {
                if (isfloatconst(ep->left)) {
                    if (ep->left->v.f.type != IFPF_IS_ZERO)
                        *node = ep->right->left;
                    else
                        *node = ep->right->right;
                    rv |= opt0(node);
                    break;
                } else if (isintconst(ep->left)) {
                    if (ep->left->v.i)
                        *node = ep->right->left;
                    else
                        *node = ep->right->right;
                    rv |= opt0(node);
                    break;
                }
            }
            /* Fall Through*/
        case en_void:
        case en_intcall:
        case en_voidnz:
        case en_assign:
        case en_blockassign:
        case en_stackblock:
        case en_autoinc:
        case en_autodec:
            rv |= opt0(&(ep->right));
        case en_trapcall:
        case en_substack:
        case en_alloca:
        case en_loadstack:
        case en_savestack:
        case en_blockclear:
        case en_argnopush:
        case en_not_lvalue:
        case en_lvalue:
        case en_thisref:
        case en_funcret:
            rv |= opt0(&(ep->left));
            break;
        case en_func:
            rv |= opt0(&((*node)->v.func->fcall));
            if ((*node)->v.func->thisptr)
                rv |= opt0(&((*node)->v.func->thisptr));
            return rv;
        case en_atomic:
            rv |= opt0(&((*node)->v.ad->flg));
            rv |= opt0(&((*node)->v.ad->memoryOrder1));
            rv |= opt0(&((*node)->v.ad->memoryOrder2));
            rv |= opt0(&((*node)->v.ad->third));
            rv |= opt0(&((*node)->v.ad->address));
            rv |= opt0(&((*node)->v.ad->value));
            return rv;
        case en_sizeofellipse:
            if (!templateNestingCount)
            {
                int n = 0;
                TEMPLATEPARAMLIST *tpl = (*node)->v.templateParam->p->byPack.pack;
                while (tpl)
                    n++, tpl = tpl->next;
                *node = intNode(en_c_i, n);
            }
            break;
        case en_templateselector:
            if (!templateNestingCount)
            {
                TEMPLATESELECTOR *tsl = (*node)->v.templateSelector;
                SYMBOL *ts = tsl->next->sym;
                SYMBOL *sp = ts;
                TEMPLATESELECTOR *find = tsl->next->next;
                if (tsl->next->isTemplate)
                {
                    TEMPLATEPARAMLIST *current = tsl->next->templateParams;
                    sp = GetClassTemplate(ts, current, TRUE);
                }
                if (sp)
                {
                    sp = basetype(PerformDeferredInitialization (sp->tp, NULL))->sp;
                    while (find && sp)
                    {
                        SYMBOL *spo = sp;
                        if (!isstructured(spo->tp))
                            break;
                        
                        sp = search(find->name, spo->tp->syms);
                        if (!sp)
                        {
                            sp = classdata(find->name, spo, NULL, FALSE, FALSE);
                            if (sp == (SYMBOL *)-1)
                                sp = NULL;
                        }
                        find = find->next;
                    }
                    if (!find && sp)
                    {
                        if (sp->storage_class == sc_constant)
                        {
                            optimize_for_constants(&sp->init->exp);
                            *node = sp->init->exp;
                            return TRUE;
                        }
                    }
                }
                return FALSE;                
                
            }
            break;
        case en_templateparam:
            if (!templateNestingCount && (*node)->v.sp->tp->templateParam->p->type == kw_int)
            {
                SYMBOL *sym = (*node)->v.sp;
                TEMPLATEPARAMLIST *found = NULL;
                STRUCTSYM *search = structSyms;
                while (search && !found)
                {
                    if (search->tmpl)
                    {
                        TEMPLATEPARAMLIST *tpl = search->tmpl;
                        while (tpl && !found)
                        {
                            if (tpl->argsym && !strcmp(tpl->argsym->name, sym->name))
                                found = tpl;
                            tpl = tpl->next;
                        }
                    }
                    search = search->next;
                }
                if (found && found->p->type == kw_int)
                {
                    if (found->p->byNonType.val)
                        *node = found->p->byNonType.val;
                }
            }
            break;
        default:
            break;
    }
    return rv;
}

/*-------------------------------------------------------------------------*/

void enswap(EXPRESSION **one, EXPRESSION **two)
{
    EXPRESSION *temp =  *one;
    *one =  *two;
    *two = temp;
}

/*-------------------------------------------------------------------------*/

int fold_const(EXPRESSION *node)
/*
 *      fold_const will remove constant nodes and return the values to
 *      the calling routines.
 */
{
    int rv = FALSE;
    if (node == 0)
        return 0;
    switch (node->type)
    {
        case en_add:
        case en_arrayadd:
        case en_structadd:
            rv |= fold_const(node->left);
            rv |= fold_const(node->right);
            if (node->right->type == en_structelem)
                break;
            if (isoptconst(node->right))
            {
                switch (node->left->type)
                {
                case en_add:
                case en_arrayadd:
                case en_structadd:
                    if (isoptconst(node->left->left))
                    {
                        int type = en_add;
                        if (node->type == en_arrayadd || node->left
                               ->type == en_arrayadd)
                               type = en_arrayadd;
                        if (node->type == en_structadd || node->left
                               ->type == en_structadd)
                               type = en_structadd;
                        node->type = type;
                        node->left->type = en_add;
                        enswap(&node->left->right, &node->right);
                        rv = TRUE;
                    }
                    else if (isoptconst(node->left->right))
                    {
                        int type = en_add;

                        if (node->type == en_arrayadd || node->left
                               ->type == en_arrayadd)
                            type = en_arrayadd;
                        if (node->type == en_structadd || node->left
                               ->type == en_structadd)
                               type = en_structadd;
                        node->type = type;
                        node->left->type = en_add;
                        enswap(&node->left->left, &node->right);
                        rv = TRUE;
                    }
                    break;
                case en_sub:
                    if (isoptconst(node->left->left))
                    {
                        int type = en_add;
                        if (node->type == en_arrayadd)
                            type = en_arrayadd;
                        if (node->type == en_structadd)
                            type = en_structadd;
                        node->type = en_sub;
                        node->left->type = type;
                        enswap(&node->left->right, &node->right);
                        rv = TRUE;
                    }
                    else if (isoptconst(node->left->right))
                    {
                        int type = en_add;
                        if (node->type == en_arrayadd)
                            type = en_arrayadd;
                        if (node->type == en_structadd)
                            type = en_structadd;
                        node->type = type;
                        node->left->type = en_sub;
                        enswap(&node->left->left, &node->right);
                        rv = TRUE;
                    }
                    break;
                default:
                    break;
                }
            }
            else if (isoptconst(node->left))
            {
                switch (node->right->type)
                {
                case en_add:
                case en_arrayadd:
                case en_structadd:
                    if (isoptconst(node->right->left))
                    {
                        int type = en_add;
                        if (node->type == en_arrayadd || node->right
                            ->type == en_arrayadd)
                            type = en_arrayadd;
                        if (node->type == en_structadd || node->right
                            ->type == en_structadd)
                            type = en_structadd;
                        node->type = type;
                        node->right->type = en_add;
                        enswap(&node->right->right, &node->left);
                        rv = TRUE;
                    }
                    else if (isoptconst(node->right->right))
                    {
                        int type = en_add;
                        if (node->type == en_arrayadd || node->right
                            ->type == en_arrayadd)
                            type = en_arrayadd;
                        if (node->type == en_structadd || node->right
                            ->type == en_structadd)
                            type = en_structadd;
                        node->type = type;
                        node->right->type = en_add;
                        enswap(&node->right->left, &node->left);
                        rv = TRUE;
                    }
                    break;
                case en_sub:
                    if (isoptconst(node->right->left))
                    {
                        node->type = en_sub;
                        node->right->type = en_add;
                        enswap(&node->left, &node->right);
                        enswap(&node->left->right, &node->right);
                        rv = TRUE;
                    }
                    else if (isoptconst(node->right->right))
                    {
                        int type = en_add;
                        if (node->type == en_arrayadd)
                            type = en_arrayadd;
                        if (node->type == en_structadd)
                            type = en_structadd;
                        node->type = type;
                        node->right->type = en_sub;
                        enswap(&node->right->left, &node->left);
                        rv = TRUE;
                    }
                    break;
                default:
                    break;
                }
            }
            break;
        case en_sub:
            rv |= fold_const(node->left);
            rv |= fold_const(node->right);
            if (isoptconst(node->right))
            {
                switch (node->left->type)
                {
                case en_add:
                case en_arrayadd:
                case en_structadd:
                    if (isoptconst(node->left->left))
                    {
                        node->type = en_add;
                        node->left->type = en_sub;
                        enswap(&node->left->right, &node->right);
                        rv = TRUE;
                    }
                    else if (isoptconst(node->left->right))
                    {
                        node->type = en_add;
                        node->left->type = en_sub;
                        enswap(&node->left->left, &node->left->right);
                        enswap(&node->left->right, &node->right);
                        rv = TRUE;
                    }
                    break;
                case en_sub:
                    if (isoptconst(node->left->left))
                    {
                        enswap(&node->left->right, &node->right);
                        rv = TRUE;
                    }
                    else if (isoptconst(node->left->right))
                    {
                        enswap(&node->left, &node->right);
                        enswap(&node->right->left, &node->left);
                        node->type = en_add;
                        node->right->left = exprNode(en_uminus, node->right
                            ->left, 0);
                        rv = TRUE;
                    }
                    break;
                default:
                    break;
                }
            }
            else if (isoptconst(node->left))
            {
                switch (node->right->type)
                {
                case en_add:
                case en_arrayadd:
                case en_structadd:
                    if (isoptconst(node->right->left))
                    {
                        node->type = en_sub;
                        node->right->type = en_sub;
                        enswap(&node->left, &node->right);
                        enswap(&node->left->right, &node->right);
                        enswap(&node->left->left, &node->left->right);
                        rv = TRUE;
                    }
                    else if (isoptconst(node->right->right))
                    {
                        node->type = en_sub;
                        node->right->type = en_sub;
                        enswap(&node->left, &node->right);
                        enswap(&node->left->left, &node->right);
                        rv = TRUE;
                    }
                    break;
                case en_sub:
                    if (isoptconst(node->right->left))
                    {
                        enswap(&node->right->right, &node->left);
                        rv = TRUE;
                    }
                    else if (isoptconst(node->right->right))
                    {

                        enswap(&node->left, &node->right);
                        enswap(&node->left->left, &node->right);
                        node->left->right = exprNode(en_uminus, node->left
                            ->right, 0);
                        rv = TRUE;
                    }
                    break;
                default:
                    break;
                }
            }
            break;
        case en_mul:
        case en_umul:
            rv |= fold_const(node->left);
            rv |= fold_const(node->right);
            if (isoptconst(node->right))
            {
                switch (node->left->type)
                {
                case en_mul:
                case en_umul:
                    if (isoptconst(node->left->left))
                    {
                        node->type = en_mul;
                        node->left->type = en_mul;
                        enswap(&node->left->right, &node->right);
                        rv = TRUE;
                    }
                    else if (isoptconst(node->left->right))
                    {
                        node->type = en_mul;
                        node->left->type = en_mul;
                        enswap(&node->left->left, &node->right);
                        rv = TRUE;
                    }
                    break;
                default:
                    break;
                }
            }
            else if (isoptconst(node->left))
            {
                switch (node->right->type)
                {
                case en_mul:
                case en_umul:
                    if (isoptconst(node->right->left))
                    {
                        node->type = en_mul;
                        node->right->type = en_mul;
                        enswap(&node->right->right, &node->left);
                        rv = TRUE;
                    }
                    else if (isoptconst(node->right->right))
                    {
                        node->type = en_mul;
                        node->right->type = en_mul;
                        enswap(&node->right->left, &node->left);
                        rv = TRUE;
                    }
                    break;
                default:
                    break;
                }
            }
            break;
        case en_and:
        case en_or:
        case en_xor:
            rv |= fold_const(node->left);
            rv |= fold_const(node->right);
            if (node->left->type == node->type && isoptconst(node
                ->right))
            {
                if (isoptconst(node->left->left))
                {
                    enswap(&node->left->right, &node->right);
                    rv = TRUE;
                }
                else if (isoptconst(node->left->right))
                {
                    enswap(&node->left->left, &node->right);
                    rv = TRUE;
                }
            }
            else if (node->right->type == node->type && isoptconst
                (node->left))
            {
                if (isoptconst(node->right->left))
                {
                    enswap(&node->right->right, &node->left);
                    rv = TRUE;
                }
                else if (isoptconst(node->right->right))
                {
                    enswap(&node->right->left, &node->left);
                    rv = TRUE;
                }
            }
            break;
        case en_l_sp:
        case en_l_fp:
        case en_l_fi:
        case en_l_di:
        case en_l_ldi:
        case en_l_fc:
        case en_l_dc:
        case en_l_ldc:
        case en_x_fi:
        case en_x_di:
        case en_x_ldi:
        case en_x_fc:
        case en_x_dc:
        case en_x_ldc:
        case en_x_bit:
        case en_l_f:
        case en_l_d:
        case en_l_ld:
        case en_l_ll:
        case en_l_ull:
        case en_l_wc:
        case en_l_u16:
        case en_l_u32:
        case en_l_c:
        case en_l_s:
        case en_l_p:
        case en_l_ref:
        case en_l_i:
        case en_l_ui:
        case en_l_ul:
        case en_l_l:
        case en_l_uc:
        case en_l_us:
        case en_l_bool:
        case en_l_bit:
        case en_uminus:
        case en_compl:
        case en_not:
        case en_x_f:
        case en_x_d:
        case en_x_ld:
        case en_x_wc:
        case en_x_c:
        case en_x_uc:
        case en_x_u16:
        case en_x_u32:
        case en_x_bool:
        case en_x_s:
        case en_x_us:
        case en_x_ll:
        case en_x_ull:
        case en_x_l:
        case en_x_ul:
        case en_x_i:
        case en_x_ui:
        case en_x_p:
        case en_x_fp:
        case en_x_sp:
        case en_trapcall:
        case en_shiftby:
        case en_substack:
        case en_alloca:
        case en_loadstack:
        case en_savestack:
        case en_bits:
        case en_literalclass:
            rv |= fold_const(node->left);
            break;
        case en_cond:
            rv |= fold_const(node->left);
            rv |= fold_const(node->right->left);
            rv |= fold_const(node->right->right);
            break;
        case en_atomic:
            rv |= fold_const(node->v.ad->flg);
            rv |= fold_const(node->v.ad->memoryOrder1);
            rv |= fold_const(node->v.ad->memoryOrder2);
            rv |= fold_const(node->v.ad->address);
            rv |= fold_const(node->v.ad->value);
            rv |= fold_const(node->v.ad->third);
            break;
        case en_ursh:
        case en_rshd:
        case en_assign:
        case en_void:
        case en_voidnz:
        case en_arraymul:
        case en_arraydiv:
        case en_div:
        case en_udiv:
        case en_umod:
        case en_lsh:
        case en_arraylsh:
        case en_rsh:
        case en_mod:
        case en_lor:
        case en_land:
        case en_eq:
        case en_ne:
        case en_gt:
        case en_ge:
        case en_lt:
        case en_le:
        case en_ugt:
        case en_uge:
        case en_ult:
        case en_ule:
        case en_intcall:
        case en_blockassign:
        case en_stackblock:
        case en_autoinc:
        case en_autodec:
        case en_mp_compare:
            rv |= fold_const(node->right);
        case en_blockclear:
        case en_argnopush:
        case en_not_lvalue:
        case en_lvalue:
            rv |= fold_const(node->left);
            break;
        case en_funcret:
            rv |= fold_const(node->left);
            if (node->left->type != en_func && node->left->type != en_funcret)
                *node = *node->left;
            break;
        case en_thisref:
            rv |= fold_const(node->left);
            if (node->left->type != en_func && node->left->type != en_funcret)
                *node = *node->left;
            break;
        case en_func:
            if (node->v.func->sp && node->v.func->sp->constexpression && node->v.func->sp->inlineFunc.stmt)
            {
                int i;
                STATEMENT *stmt = node->v.func->sp->inlineFunc.stmt;
                    while (stmt && stmt->type == st_expr)
                        stmt = stmt->next;
                    if (stmt && stmt->type == st_block && stmt->lower)
                    {
                        STATEMENT *st = stmt->lower;
                        while (st->type == st_varstart)
                            st = st->next;
                        if (st->type == st_block && !st->next)
                        {
                            st = st->lower;
                            while (st->type == st_line || st->type == st_dbgblock)
                                st = st->next;
                            if (st->type == st_expr || st->type == st_return)
                            {
                                if (st->select)
                                {
                                    for (i = 0; i < functionnestingcount; i++)
                                        if (functionnesting[i] == st->select)
                                            break;
                                    if (i >= functionnestingcount)
                                    {
                                        functionnesting[functionnestingcount++] = st->select;
                                        optimize_for_constants(&st->select);
                                        functionnestingcount--;
                                        if (IsConstantExpression(st->select, FALSE))
                                        {
                                            *node = *st->select;
                                            node->noexprerr = TRUE;
                                            rv = TRUE;
                                        }
                                    }
                                }
                            }
                        }
                    }
            }
            if (!rv)
                rv |= fold_const(node->v.func->fcall);
            break;
        case en_mp_as_bool:
            if (node->left->type == en_memberptr)
            {
                node->type = en_c_i;
                node->left = NULL;
                node->v.i = 1;
                rv = TRUE;
            }
            break;
        case en_stmt:
            // constructor thunks
            while (node->v.stmt && node->v.stmt->type == st_expr)
                node->v.stmt = node->v.stmt->next;
            if (node->v.stmt && node->v.stmt->type == st_block)
            {
                STATEMENT *st = node->v.stmt->lower;
                while (st->type == st_varstart)
                    st = st->next;
                if (st->type == st_block && !st->next)
                {
                    st = st->lower;
                    while (st->type == st_line || st->type == st_dbgblock)
                        st = st->next;
                    if (st->type == st_expr || st->type == st_return)
                    {
                        EXPRESSION *exp = st->select;
                        optimize_for_constants(&st->select);
                        if (IsConstantExpression(st->select, TRUE))
                        {
                            *node = *st->select;
                            node->noexprerr = TRUE;
                            rv = TRUE;
                        }
                    }
                }
            }
            break;
        default:
            break;
    }
    return rv;
}


/*

 * remove type casts from constant nodes and change their size
 */
int typedconsts(EXPRESSION *node1)
{
    int rv = FALSE;
    if (!node1)
        return rv;
    switch (node1->type)
    {
        case en_nullptr:
            node1->type = en_c_ui; // change the nullptr to an int
            rv = TRUE;
            break;
        case en_const:
            /* special trap to replace sc_constants */
//            if (basetype(node1->v.sp->tp)->type == bt_long_long || basetype(node1->v.sp->tp)->type == bt_unsigned_long_long)
//                node1->type = en_c_ll;
//            else
//                node1->type = en_c_i;
//            node1->v.i = node1->v.sp->value.i;
            optimize_for_constants(&node1->v.sp->init->exp);
            *node1 = *node1->v.sp->init->exp;
            rv = TRUE;
            break;
        default:
            break;
        case en_compl:
        case en_not:
        case en_uminus:
        case en_argnopush:
        case en_not_lvalue:
        case en_lvalue:

            rv |= typedconsts(node1->left);
            break;
        case en_cond:
            rv |= typedconsts(node1->left);
            rv |= typedconsts(node1->right->left);
            rv |= typedconsts(node1->right->right);
            break;
        case en_atomic:
            rv |= typedconsts(node1->v.ad->flg);
            rv |= typedconsts(node1->v.ad->memoryOrder1);
            rv |= typedconsts(node1->v.ad->memoryOrder2);
            rv |= typedconsts(node1->v.ad->address);
            rv |= typedconsts(node1->v.ad->value);
            rv |= typedconsts(node1->v.ad->third);
            break;
        case en_add:
        case en_arrayadd:
        case en_structadd:
        case en_sub:
        case en_or:
        case en_xor:
        case en_mul:
        case en_arraymul:
        case en_umul:
        case en_lsh:
        case en_arraylsh:
        case en_ursh:
        case en_rsh:
        case en_rshd:
        case en_div:
        case en_arraydiv:
        case en_mod:
        case en_land:
        case en_lor:
        case en_and:
        case en_void:
        case en_voidnz:
        case en_assign:
        case en_blockassign:
        case en_stackblock:
        case en_intcall:
        case en_udiv:
        case en_umod:
        case en_eq:
        case en_ne:
        case en_lt:
        case en_le:
        case en_ugt:
        case en_uge:
        case en_ult:
        case en_ule:
        case en_gt:
        case en_ge:
        case en_autoinc:
        case en_autodec:
        case en_mp_compare:
            rv |= typedconsts(node1->right);
        case en_trapcall:
        case en_shiftby:
        case en_substack:
        case en_alloca:
        case en_loadstack:
        case en_savestack:
        case en_blockclear:
        case en_mp_as_bool:
        case en_thisref:
        case en_literalclass:
        case en_funcret:
            rv |= typedconsts(node1->left);
            break;
        case en_func:
            rv |= typedconsts(node1->v.func->fcall);
            break;
        case en_l_bool:
        case en_l_bit:
        case en_l_wc:
        case en_l_u16:
        case en_l_u32:
        case en_l_c:
        case en_l_s:
        case en_l_sp:
        case en_l_fp:
        case en_l_uc:
        case en_l_us:
        case en_l_p:
        case en_l_ref:
        case en_l_i:
        case en_l_ui:
        case en_l_l:
        case en_l_ul:
        case en_l_f:
        case en_l_d:
        case en_l_ld:
        case en_l_fi:
        case en_l_di:
        case en_l_ldi:
        case en_l_fc:
        case en_l_dc:
        case en_l_ldc:
        case en_l_ull:
        case en_l_ll:
        case en_bits:
            if (node1->left->type == en_global)
            {
                if (node1->left->v.sp->storage_class == sc_constant && isintconst(node1->left->v.sp->init->exp))
                {
                    optimize_for_constants(&node1->v.sp->init->exp);
                    *node1 = *node1->left->v.sp->init->exp;
                    rv = TRUE;
                }
            }
            else
            {
                rv |= typedconsts(node1->left);
            }
            break;
        case en_x_p:
            rv |= typedconsts(node1->left);
            if (isconstaddress(node1->left) || isintconst(node1->left))
            {
                *node1 = *node1->left;
                rv = TRUE;
            }
            break;
        case en_x_ull:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = TRUE;
                node1->v.i = CastToInt(ISZ_ULONGLONG,reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = en_c_ull;
            }
            break;
        case en_x_ll:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = TRUE;
                node1->v.i = CastToInt(-ISZ_ULONGLONG,reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = en_c_ll;
            }
            break;
        case en_x_bit:
        case en_x_bool:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = TRUE;
                if (isfloatconst(node1->left) || isimaginaryconst(node1->left))
                    node1->v.i = node1->left->v.f.type != IFPF_IS_ZERO;
                else if (iscomplexconst(node1->left))
                    node1->v.i = node1->left->v.c.r.type != IFPF_IS_ZERO
                             || node1->left->v.c.i.type != IFPF_IS_ZERO;
                else
                {
                    node1->v.i = CastToInt(ISZ_BOOLEAN,!!reint(node1->left));
                    node1->unionoffset = node1->left->unionoffset;
                }
                node1->type = en_c_bool;
            }
            break;
        case en_x_c:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = TRUE;
                node1->v.i = CastToInt(-ISZ_UCHAR,reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = en_c_c;
            }
            break;
        case en_x_u16:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = TRUE;
                node1->v.i = CastToInt(ISZ_U16,reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = en_c_u16;
            }
            break;
        case en_x_u32:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = TRUE;
                node1->v.i = CastToInt(ISZ_U32,reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = en_c_u32;
            }
            break;
        case en_x_wc:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = TRUE;
                node1->v.i = CastToInt(-ISZ_WCHAR,reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = en_c_wc;
            }
            break;
        case en_x_uc:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = TRUE;
                node1->v.i = CastToInt(ISZ_UCHAR,reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = en_c_uc;
            }
            break;
        case en_x_s:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = TRUE;
                node1->v.i = CastToInt(-ISZ_USHORT,reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = en_c_i;
            }
            break;
        case en_x_us:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = TRUE;
                node1->v.i = CastToInt(ISZ_USHORT,reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = en_c_ui;
            }
            break;
        case en_x_i:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = TRUE;
                node1->v.i = CastToInt(-ISZ_UINT,reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = en_c_i;
            }
            break;
        case en_x_ui:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = TRUE;
                node1->v.i = CastToInt(ISZ_UINT,reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = en_c_ul;
            }
            break;
        case en_x_l:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = TRUE;
                node1->v.i = CastToInt(-ISZ_ULONG,reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = en_c_i;
            }
            break;
        case en_x_ul:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = TRUE;
                node1->v.i = CastToInt(ISZ_ULONG,reint(node1->left));
                node1->unionoffset = node1->left->unionoffset;
                node1->type = en_c_ul;
            }
            break;
/*#ifdef XXXXX */
        case en_x_f:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                FPF temp = refloat(node1->left);
                rv = TRUE;
                node1->v.f = CastToFloat(ISZ_FLOAT, &temp);
                node1->type = en_c_f;
            }
            break;
        case en_x_fi:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                FPF temp = refloat(node1->left);
                rv = TRUE;
                node1->v.f = CastToFloat(ISZ_IFLOAT, &temp);
                node1->type = en_c_fi;
            }
            break;
        case en_x_d:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                FPF temp = refloat(node1->left);
                rv = TRUE;
                node1->v.f = CastToFloat(ISZ_DOUBLE, &temp);
                node1->type = en_c_d;
            }
            break;
        case en_x_di:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                FPF temp = refloat(node1->left);
                rv = TRUE;
                node1->v.f = CastToFloat(ISZ_IDOUBLE, &temp);
                node1->type = en_c_di;
            }
            break;
        case en_x_ld:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                FPF temp = refloat(node1->left);
                rv = TRUE;
                node1->v.f = CastToFloat(ISZ_LDOUBLE, &temp);
                node1->type = en_c_ld;
            }
            break;
        case en_x_ldi:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                FPF temp = refloat(node1->left);
                rv = TRUE;
                node1->v.f = CastToFloat(ISZ_ILDOUBLE, &temp);
                node1->type = en_c_ldi;
            }
            break;
        case en_x_fc:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = TRUE;
                if (isintconst(node1->left) || isfloatconst(node1->left))
                {
                    FPF temp = refloat(node1->left);
                    node1->v.c.r = CastToFloat(ISZ_ILDOUBLE, &temp);
                    SetFPFZero(&node1->v.c.i, 0);
                }
                else if (isimaginaryconst(node1->left))
                {
                    FPF temp ;
                    node1->left->type = en_c_ld;
                    temp = refloat(node1->left);
                    node1->v.c.i = CastToFloat(ISZ_ILDOUBLE, &temp);
                    SetFPFZero(&node1->v.c.r, 0);
                }
                else
                {
                    node1->v.c = node1->left->v.c;
                }
                node1->type = en_c_fc;					
            }
            break;
        case en_x_dc:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = TRUE;
                if (isintconst(node1->left) || isfloatconst(node1->left))
                {
                    FPF temp = refloat(node1->left);
                    node1->v.c.r = CastToFloat(ISZ_ILDOUBLE, &temp);
                    SetFPFZero(&node1->v.c.i, 0);
                }
                else if (isimaginaryconst(node1->left))
                {
                    FPF temp ;
                    node1->left->type = en_c_ld;
                    temp = refloat(node1->left);
                    node1->v.c.i = CastToFloat(ISZ_ILDOUBLE, &temp);
                    SetFPFZero(&node1->v.c.r, 0);
                }
                else
                {
                    node1->v.c = node1->left->v.c;
                }
                node1->type = en_c_dc;
            }
            break;
        case en_x_ldc:
            rv |= typedconsts(node1->left);
            if (node1->left && isoptconst(node1->left))
            {
                rv = TRUE;
                if (isintconst(node1->left) || isfloatconst(node1->left))
                {
                    FPF temp = refloat(node1->left);
                    node1->v.c.r = CastToFloat(ISZ_ILDOUBLE, &temp);
                    SetFPFZero(&node1->v.c.i, 0);
                }
                else if (isimaginaryconst(node1->left))
                {
                    FPF temp ;
                    node1->left->type = en_c_ld;
                    temp = refloat(node1->left);
                    node1->v.c.i = CastToFloat(ISZ_ILDOUBLE, &temp);
                    SetFPFZero(&node1->v.c.r, 0);
                }
                else
                {
                    node1->v.c = node1->left->v.c;
                }
                node1->type = en_c_ldc;
            }
            break;
/*#endif */
    }
    return rv;
}
static int depth(EXPRESSION *ep)
{
    if (ep == 0)
        return 0;
   switch (ep->type)
    {
        case en_c_bool:
        case en_c_c:
        case en_c_uc:
        case en_c_wc:
        case en_c_u16:
        case en_c_u32:
        case en_c_i:
        case en_c_ui:
        case en_c_l:
        case en_c_ul:
        case en_c_ll:
        case en_c_ull:
        case en_c_f:
        case en_c_d:
        case en_c_ld:
        case en_c_fc:
        case en_c_dc:
        case en_c_ldc:
        case en_c_fi:
        case en_c_di:
        case en_c_ldi:
        case en_global:
        case en_auto:
        case en_pc:
        case en_threadlocal:
        case en_labcon:
        case en_label:
        case en_absolute:
        case en_nullptr:
        case en_atomic:
        case en_structelem:
            return 1;
        case en_funcret:
            return depth(ep->left);
        case en_func:
            
            return 10 + imax(depth(ep->left), depth(ep->right));
        case en_cond:
            return 1 + imax(depth(ep->left), imax(depth(ep->right->left), depth(ep->right->right)));
        default:
            return 1 + imax(depth(ep->left), depth(ep->right));
    }
}
static void rebalance(EXPRESSION *ep)
{
    if (ep == 0)
        return ;
    switch (ep->type)
    {
        case en_c_bool:
        case en_c_c:
        case en_c_uc:
        case en_c_wc:
        case en_c_u16:
        case en_c_u32:
        case en_c_i:
        case en_c_ui:
        case en_c_l:
        case en_c_ul:
        case en_c_ll:
        case en_c_ull:
        case en_c_f:
        case en_c_d:
        case en_c_ld:
        case en_c_fc:
        case en_c_dc:
        case en_c_ldc:
        case en_c_fi:
        case en_c_di:
        case en_c_ldi:
        case en_global:
        case en_auto:
        case en_pc:
        case en_threadlocal:
        case en_labcon:
        case en_label:
        case en_absolute:
        case en_nullptr:
        case en_structelem:
            break;
        case en_add:
        case en_mul:
        case en_umul:
        case en_arraymul:
        case en_and:
        case en_or:
        case en_xor:
        case en_eq:
        case en_ne:
            rebalance(ep->left);
            rebalance(ep->right);
            if (depth(ep->left) < depth(ep->right))
            {
                EXPRESSION *p = ep->left;
                ep->left = ep->right;
                ep->right = p;
            }
            break;
        case en_sub:
            rebalance(ep->left);
            rebalance(ep->right);
            if (depth(ep->left) < depth(ep->right))
            {
                EXPRESSION *p = ep->left;
                ep->left = ep->right;
                ep->right = p;
                ep->type = en_add;
                ep->left = exprNode(en_uminus, ep->left, 0);
            }
            break;
        case en_cond:
            rebalance(ep->left);
            rebalance(ep->right->left);
            rebalance(ep->right->right);
            break;
        case en_atomic:
            rebalance(ep->v.ad->flg);
            rebalance(ep->v.ad->memoryOrder1);
            rebalance(ep->v.ad->memoryOrder1);
            rebalance(ep->v.ad->address);
            rebalance(ep->v.ad->value);
            rebalance(ep->v.ad->third);
            break;
        default:
            rebalance(ep->left);
            rebalance(ep->right);
            break;
    }
}
void optimize_for_constants(EXPRESSION **expr)
{
    int rv = TRUE, count = 8;
    EXPRESSION * oldasidehead = asidehead;
    EXPRESSION ** oldasidetail = asidetail;

    asidehead = 0;
    asidetail = &asidehead;
    while (rv && count--)
        rv = typedconsts(*expr) | fold_const(*expr) | opt0(expr);
    if (asidehead)
    {
        *asidetail =  *expr;
        *expr = asidehead;
    }
    asidehead = oldasidehead;
    asidetail = oldasidetail;
    rebalance(*expr);
}
LEXEME *optimized_expression(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **expr, BOOLEAN commaallowed)
{
    
    if (commaallowed)
        lex = expression(lex, funcsp, atp, tp, expr, 0);
    else
        lex = expression_no_comma(lex, funcsp, atp, tp, expr, NULL, 0);
    if (*tp)
    {
        optimize_for_constants(expr);
    }
    return lex; 
}
