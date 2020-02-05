/* Software License Agreement
 * 
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <limits.h>
#include "compiler.h"
#include "Floating.h"

extern ARCH_ASM* chosenAssembler;


/*-------------------------------------------------------------------------*/
FPF refloat(EXPRESSION* node)
{
    FPF rv;
    FPF temp;
    switch (node->type)
    {
    case en_c_i:
        rv = CastToFloat(ISZ_LDOUBLE, IntToFloat(&temp, -ISZ_UINT, node->v.i));
        break;
    case en_c_l:
        rv = CastToFloat(ISZ_LDOUBLE, IntToFloat(&temp, -ISZ_ULONG, node->v.i));
        break;
    case en_c_ui:
        rv = CastToFloat(ISZ_LDOUBLE, IntToFloat(&temp, ISZ_UINT, node->v.i));
        break;
    case en_c_ul:
        rv = CastToFloat(ISZ_LDOUBLE, IntToFloat(&temp, ISZ_ULONG, node->v.i));
        break;
    case en_c_c:
        rv = CastToFloat(ISZ_LDOUBLE, IntToFloat(&temp, -ISZ_UCHAR, node->v.i));
        break;
    case en_c_uc:
        rv = CastToFloat(ISZ_LDOUBLE, IntToFloat(&temp, ISZ_UCHAR, node->v.i));
        break;
    case en_c_u16:
        rv = CastToFloat(ISZ_LDOUBLE, IntToFloat(&temp, ISZ_U16, node->v.i));
        break;
    case en_c_u32:
        rv = CastToFloat(ISZ_LDOUBLE, IntToFloat(&temp, ISZ_U32, node->v.i));
        break;
    case en_c_wc:
        rv = CastToFloat(ISZ_LDOUBLE, IntToFloat(&temp, ISZ_WCHAR, node->v.i));
        break;
    case en_c_bool:
        rv = CastToFloat(ISZ_LDOUBLE, IntToFloat(&temp, ISZ_BOOLEAN, node->v.i));
        break;
    case en_c_ull:
        rv = CastToFloat(ISZ_LDOUBLE, IntToFloat(&temp, ISZ_ULONGLONG, node->v.i));
        break;
    case en_c_ll:
        rv = CastToFloat(ISZ_LDOUBLE, IntToFloat(&temp, -ISZ_ULONGLONG, node->v.i));
        break;
    case en_c_f:
    case en_c_fi:
        rv = CastToFloat(ISZ_FLOAT, node->v.f);
        break;
    case en_c_d:
    case en_c_di:
        rv = CastToFloat(ISZ_DOUBLE, node->v.f);
        break;

    case en_c_ld:
    case en_c_ldi:
        rv = CastToFloat(ISZ_LDOUBLE, node->v.f);
        break;
    case en_c_fc:
        rv = CastToFloat(ISZ_FLOAT, &node->v.c->i);
        rv = CastToFloat(ISZ_FLOAT, &node->v.c->r);
        break;

    case en_c_dc:
        rv = CastToFloat(ISZ_DOUBLE, &node->v.c->i);
        rv = CastToFloat(ISZ_DOUBLE, &node->v.c->r);
        break;
    case en_c_ldc:
        rv = CastToFloat(ISZ_LDOUBLE, &node->v.c->i);
        rv = CastToFloat(ISZ_LDOUBLE, &node->v.c->r);
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
        rv = CastToInt(-ISZ_ULONGLONG, (long long)*node->v.f);
        break;
    case en_c_d:
        rv = CastToInt(-ISZ_ULONGLONG, (long long)*node->v.f);
        break;

    case en_c_ld:
        rv = CastToInt(-ISZ_ULONGLONG, (long long)*node->v.f);
        break;
    case en_c_fc:
        rv = CastToInt(-ISZ_ULONGLONG, (long long)node->v.c->r);
        break;
    case en_c_dc:
        rv = CastToInt(-ISZ_ULONGLONG, (long long)node->v.c->r);
        break;

    case en_c_ldc:
        rv = CastToInt(-ISZ_ULONGLONG, (long long)node->v.c->r);
        break;
    case en_c_fi:
    case en_c_di:
    case en_c_ldi:
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
    case se_ui:
        rv = CastToFloat(ISZ_LDOUBLE, IntToFloat(&temp, ISZ_ULONGLONG, node->i));
        break;
    case se_i:
        rv = CastToFloat(ISZ_LDOUBLE, IntToFloat(&temp, -ISZ_ULONGLONG, node->i));
        break;
    case en_c_f:
    case en_c_fi:
        rv = CastToFloat(ISZ_FLOAT, &node->f);
        break;
    case en_c_d:
    case en_c_di:
        rv = CastToFloat(ISZ_DOUBLE, &node->f);
        break;

    case en_c_ld:
    case en_c_ldi:
        rv = CastToFloat(ISZ_LDOUBLE, &node->f);
        break;
    case en_c_fc:
        rv = CastToFloat(ISZ_FLOAT, &node->c.i);
        rv = CastToFloat(ISZ_FLOAT, &node->c.r);
        break;

    case en_c_dc:
        rv = CastToFloat(ISZ_DOUBLE, &node->c.i);
        rv = CastToFloat(ISZ_DOUBLE, &node->c.r);
        break;
    case en_c_ldc:
        rv = CastToFloat(ISZ_LDOUBLE, &node->c.i);
        rv = CastToFloat(ISZ_LDOUBLE, &node->c.r);
        break;

    default:
        rv = 0;
        break;
    }
    return rv;
}
*/