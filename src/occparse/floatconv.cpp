/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 * 
 */

#include <cstdio>
#include <malloc.h>
#include <cstring>
#include <climits>
#include "compiler.h"
#include "Floating.h"
#include "config.h"
#include "ifloatconv.h"
#include "floatconv.h"
namespace Parser
{
/*-------------------------------------------------------------------------*/
FPF refloat(EXPRESSION* node)
{
    FPF rv;
    FPF temp;
    switch (node->type)
    {
        case ExpressionNode::c_i_:
            rv = Optimizer::CastToFloat(ISZ_LDOUBLE, Optimizer::IntToFloat(&temp, -ISZ_UINT, node->v.i));
            break;
        case ExpressionNode::c_l_:
            rv = Optimizer::CastToFloat(ISZ_LDOUBLE, Optimizer::IntToFloat(&temp, -ISZ_ULONG, node->v.i));
            break;
        case ExpressionNode::c_ui_:
            rv = Optimizer::CastToFloat(ISZ_LDOUBLE, Optimizer::IntToFloat(&temp, ISZ_UINT, node->v.i));
            break;
        case ExpressionNode::c_ul_:
            rv = Optimizer::CastToFloat(ISZ_LDOUBLE, Optimizer::IntToFloat(&temp, ISZ_ULONG, node->v.i));
            break;
        case ExpressionNode::c_c_:
            rv = Optimizer::CastToFloat(ISZ_LDOUBLE, Optimizer::IntToFloat(&temp, -ISZ_UCHAR, node->v.i));
            break;
        case ExpressionNode::c_uc_:
            rv = Optimizer::CastToFloat(ISZ_LDOUBLE, Optimizer::IntToFloat(&temp, ISZ_UCHAR, node->v.i));
            break;
        case ExpressionNode::c_u16_:
            rv = Optimizer::CastToFloat(ISZ_LDOUBLE, Optimizer::IntToFloat(&temp, ISZ_U16, node->v.i));
            break;
        case ExpressionNode::c_u32_:
            rv = Optimizer::CastToFloat(ISZ_LDOUBLE, Optimizer::IntToFloat(&temp, ISZ_U32, node->v.i));
            break;
        case ExpressionNode::c_wc_:
            rv = Optimizer::CastToFloat(ISZ_LDOUBLE, Optimizer::IntToFloat(&temp, ISZ_WCHAR, node->v.i));
            break;
        case ExpressionNode::c_bool_:
            rv = Optimizer::CastToFloat(ISZ_LDOUBLE, Optimizer::IntToFloat(&temp, ISZ_BOOLEAN, node->v.i));
            break;
        case ExpressionNode::c_ull_:
            rv = Optimizer::CastToFloat(ISZ_LDOUBLE, Optimizer::IntToFloat(&temp, ISZ_ULONGLONG, node->v.i));
            break;
        case ExpressionNode::c_ll_:
            rv = Optimizer::CastToFloat(ISZ_LDOUBLE, Optimizer::IntToFloat(&temp, -ISZ_ULONGLONG, node->v.i));
            break;
        case ExpressionNode::c_f_:
        case ExpressionNode::c_fi_:
            rv = Optimizer::CastToFloat(ISZ_FLOAT, node->v.f);
            break;
        case ExpressionNode::c_d_:
        case ExpressionNode::c_di_:
            rv = Optimizer::CastToFloat(ISZ_DOUBLE, node->v.f);
            break;

        case ExpressionNode::c_ld_:
        case ExpressionNode::c_ldi_:
            rv = Optimizer::CastToFloat(ISZ_LDOUBLE, node->v.f);
            break;
        case ExpressionNode::c_fc_:
            rv = Optimizer::CastToFloat(ISZ_FLOAT, &node->v.c->i);
            rv = Optimizer::CastToFloat(ISZ_FLOAT, &node->v.c->r);
            break;

        case ExpressionNode::c_dc_:
            rv = Optimizer::CastToFloat(ISZ_DOUBLE, &node->v.c->i);
            rv = Optimizer::CastToFloat(ISZ_DOUBLE, &node->v.c->r);
            break;
        case ExpressionNode::c_ldc_:
            rv = Optimizer::CastToFloat(ISZ_LDOUBLE, &node->v.c->i);
            rv = Optimizer::CastToFloat(ISZ_LDOUBLE, &node->v.c->r);
            break;

        default:
            rv = 0;
            break;
    }
    return rv;
}

/*-------------------------------------------------------------------------*/
unsigned char* make_bitint(int bits, unsigned char* array)
{ 
    bits += Optimizer::chosenAssembler->arch->bitintunderlying - 1;
    bits /= Optimizer::chosenAssembler->arch->bitintunderlying;
    bits *= Optimizer::chosenAssembler->arch->bitintunderlying;
    bits /= CHAR_BIT;
    unsigned char* rv = new unsigned char[bits];
    memcpy(rv, array, bits);
    return rv;
}
unsigned char* make_bitint(int bits, long long val)
{
    int origbits = bits;
    bits += Optimizer::chosenAssembler->arch->bitintunderlying - 1;
    bits /= Optimizer::chosenAssembler->arch->bitintunderlying / CHAR_BIT;
    bits *= Optimizer::chosenAssembler->arch->bitintunderlying / CHAR_BIT;
    bits /= CHAR_BIT;
    unsigned char* rv = new unsigned char[bits];
    if (val < 0)
        memset(rv, 0xff, bits);
    else
        memset(rv, 0, bits);
    memcpy(rv, &val, bits > sizeof(val) ? sizeof(val) : bits);
    return rv;
}

unsigned char* rebitint(EXPRESSION* xref, EXPRESSION* cref) 
{
    int xsz = (xref->v.b.bits + Optimizer::chosenAssembler->arch->bitintunderlying - 1);
    xsz /= Optimizer::chosenAssembler->arch->bitintunderlying;
    xsz *= Optimizer::chosenAssembler->arch->bitintunderlying;    
    xsz /= CHAR_BIT;
    switch (cref->type)
    {
        case ExpressionNode::c_bitint_:
        case ExpressionNode::c_ubitint_:
        {
            int csz = (cref->v.b.bits + Optimizer::chosenAssembler->arch->bitintunderlying - 1);
            csz /= Optimizer::chosenAssembler->arch->bitintunderlying;
            csz *= Optimizer::chosenAssembler->arch->bitintunderlying;
            csz /= CHAR_BIT;
            if (xref->v.b.bits < cref->v.b.bits)
            {
                return make_bitint(xref->v.b.bits, cref->v.b.value);
            }
            else
            {
                unsigned char* rv = new unsigned char[xsz];
                int fill = 0;
                if (cref->type == ExpressionNode::c_bitint_)
                {
                    int byte = (cref->v.b.bits-1) / 8;
                    int bit = (cref->v.b.bits-1) % 8;
                    if (cref->v.b.value[byte] & (1 << bit))
                        fill = 0xff;
                }
                memset(rv, fill, xsz);
                memcpy(rv, cref->v.b.value, csz);
                return rv;
            }
        }
            break;
        default:
        {
            int csz = sizeof(unsigned long long);
            unsigned long long aa = reint(cref);
            unsigned char* rv = new unsigned char[xsz];
            int fill = 0;
            switch (cref->type)
            {
                case ExpressionNode::c_l_:
                case ExpressionNode::c_i_:
                case ExpressionNode::c_c_:
                case ExpressionNode::c_ll_:
                case ExpressionNode::c_f_:
                case ExpressionNode::c_d_:
                case ExpressionNode::c_ld_:
                case ExpressionNode::c_fc_:
                case ExpressionNode::c_dc_:
                case ExpressionNode::c_ldc_:
                case ExpressionNode::c_fi_:
                case ExpressionNode::c_di_:
                case ExpressionNode::c_ldi_:
                {   
                    if ((aa) & ( 1ULL << (csz * CHAR_BIT - 1) ))
                        fill = 0xff;
                }
                    break;                
            }
            memset(rv, fill, xsz);
            memcpy(rv, &aa, csz < xsz ? csz : xsz);
            return rv;
        }
    }
}
static unsigned long long LoadBitInt(EXPRESSION* node)
{
    int sz = (node->v.b.bits + Optimizer::chosenAssembler->arch->bitintunderlying - 1) / CHAR_BIT;
    unsigned long long a = 0;
    for (int i = 0; i < sz && i < sizeof(a); i++)
    {
        a |= node->v.b.value[i] << (i * 8);
    }
    return a;
}
unsigned long long reint(EXPRESSION* node)
{
    unsigned long long rv = 0;
    if (node->left && node->right)
        node->unionoffset = node->left->unionoffset | node->right->unionoffset;
    else if (node->left)
        node->unionoffset = node->left->unionoffset;
    switch (node->type)
    {
        case ExpressionNode::c_l_:
            rv = Optimizer::CastToInt(-ISZ_ULONG, node->v.i);
            break;
        case ExpressionNode::c_i_:
            rv = Optimizer::CastToInt(-ISZ_UINT, node->v.i);
            break;
        case ExpressionNode::c_ul_:
            rv = Optimizer::CastToInt(ISZ_ULONG, node->v.i);
            break;
        case ExpressionNode::c_ui_:
            rv = Optimizer::CastToInt(ISZ_UINT, node->v.i);
            break;
        case ExpressionNode::c_c_:
            rv = Optimizer::CastToInt(-ISZ_UCHAR, node->v.i);
            break;
        case ExpressionNode::c_uc_:
            rv = Optimizer::CastToInt(ISZ_UCHAR, node->v.i);
            break;
        case ExpressionNode::c_wc_:
            rv = Optimizer::CastToInt(ISZ_WCHAR, node->v.i);
            break;
        case ExpressionNode::c_u16_:
            rv = Optimizer::CastToInt(ISZ_U16, node->v.i);
            break;
        case ExpressionNode::c_u32_:
            rv = Optimizer::CastToInt(ISZ_U32, node->v.i);
            break;
        case ExpressionNode::c_bool_:
            rv = Optimizer::CastToInt(ISZ_BOOLEAN, node->v.i);
            break;
        case ExpressionNode::c_ull_:
            rv = Optimizer::CastToInt(ISZ_ULONGLONG, node->v.i);
            break;
        case ExpressionNode::c_ll_:
            rv = Optimizer::CastToInt(-ISZ_ULONGLONG, node->v.i);
            break;
        case ExpressionNode::c_f_:
            rv = Optimizer::CastToInt(-ISZ_ULONGLONG, (long long)*node->v.f);
            break;
        case ExpressionNode::c_d_:
            rv = Optimizer::CastToInt(-ISZ_ULONGLONG, (long long)*node->v.f);
            break;

        case ExpressionNode::c_ld_:
            rv = Optimizer::CastToInt(-ISZ_ULONGLONG, (long long)*node->v.f);
            break;
        case ExpressionNode::c_fc_:
            rv = Optimizer::CastToInt(-ISZ_ULONGLONG, (long long)node->v.c->r);
            break;
        case ExpressionNode::c_dc_:
            rv = Optimizer::CastToInt(-ISZ_ULONGLONG, (long long)node->v.c->r);
            break;

        case ExpressionNode::c_ldc_:
            rv = Optimizer::CastToInt(-ISZ_ULONGLONG, (long long)node->v.c->r);
            break;
        case ExpressionNode::c_fi_:
        case ExpressionNode::c_di_:
        case ExpressionNode::c_ldi_:
            rv = (long long)0;
            break;
        case ExpressionNode::c_bitint_:
            rv = LoadBitInt(node);
            if (rv & (1ULL << (node->bits-1)))
            {
                long long mask = ~(( 1ULL << node->v.b.bits) - 1);
                rv |= mask;
            }
            else
            {
                long long mask = ( 1ULL << node->v.b.bits) - 1;
                rv &= mask;
            }
            break;
        case ExpressionNode::c_ubitint_:
            {
                rv = LoadBitInt(node);
                long long mask = ( 1ULL << node->v.b.bits) - 1;
                rv &= mask;
            }
            break;
        default:
            break;
    }
    return rv;
}
/*
FPF dorefloat(SimpleExpression* node)
{
    FPF rv;
    FPF temp;
    switch (node->type)
    {
    case Optimizer::se_ui:
        rv = Optimizer::CastToFloat(ISZ_LDOUBLE, Optimizer::IntToFloat(&temp, ISZ_ULONGLONG, node->i));
        break;
    case Optimizer::se_i:
        rv = Optimizer::CastToFloat(ISZ_LDOUBLE, Optimizer::IntToFloat(&temp, -ISZ_ULONGLONG, node->i));
        break;
    case ExpressionNode::c_f_:
    case ExpressionNode::c_fi_:
        rv = Optimizer::CastToFloat(ISZ_FLOAT, &node->f);
        break;
    case ExpressionNode::c_d_:
    case ExpressionNode::c_di_:
        rv = Optimizer::CastToFloat(ISZ_DOUBLE, &node->f);
        break;

    case ExpressionNode::c_ld_:
    case ExpressionNode::c_ldi_:
        rv = Optimizer::CastToFloat(ISZ_LDOUBLE, &node->f);
        break;
    case ExpressionNode::c_fc_:
        rv = Optimizer::CastToFloat(ISZ_FLOAT, &node->c.i);
        rv = Optimizer::CastToFloat(ISZ_FLOAT, &node->c.r);
        break;

    case ExpressionNode::c_dc_:
        rv = Optimizer::CastToFloat(ISZ_DOUBLE, &node->c.i);
        rv = Optimizer::CastToFloat(ISZ_DOUBLE, &node->c.r);
        break;
    case ExpressionNode::c_ldc_:
        rv = Optimizer::CastToFloat(ISZ_LDOUBLE, &node->c.i);
        rv = Optimizer::CastToFloat(ISZ_LDOUBLE, &node->c.r);
        break;

    default:
        rv = 0;
        break;
    }
    return rv;
}
*/
}  // namespace Parser