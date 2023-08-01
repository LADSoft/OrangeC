/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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
 */

#include <cstdio>
#include <malloc.h>
#include <cstring>
#include <climits>
#include "compiler.h"
#include "Floating.h"
#include "config.h"
#include "ifloatconv.h"

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