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
#include "ioptimizer.h"
#include "beinterfdefs.h"
#include "config.h"
#include "OptUtils.h"

namespace Optimizer
{
unsigned long long CastToInt(int size, long long value)
{
    int bits;
    switch (size)
    {
        case ISZ_BOOLEAN:
            bits = 1;
            break;
        case -ISZ_UCHAR:
        case ISZ_UCHAR:
            bits = 8;
            break;
        default:
            return value;
        case ISZ_U16:
        case ISZ_U32:
        case -ISZ_USHORT:
        case ISZ_USHORT:
        case -ISZ_UINT:
        case ISZ_UINT:
        case -ISZ_UNATIVE:
        case ISZ_UNATIVE:
        case -ISZ_ULONG:
        case ISZ_ULONG:
        case -ISZ_ULONGLONG:
        case ISZ_ULONGLONG:
        case ISZ_WCHAR:
            bits = sizeFromISZ(size) * 8;
            break;
    }
    value &= mod_mask(bits);
    if (size < 0)
        if (value & (((unsigned long long)1) << (bits - 1)))
            value |= ~mod_mask(bits);
    return value;
}
FPF* IntToFloat(FPF* temp, int size, long long value)
{
    long long t = CastToInt(size, value);
    if (size < 0)
        *temp = (long long)t;
    else
        *temp = (unsigned long long)t;
    return temp;
}

FPF CastToFloat(int size, FPF* value)
{
    switch (size)
    {
        case ISZ_FLOAT:
        case ISZ_IFLOAT:
            if (Optimizer::chosenAssembler->arch->flt_float)
            {
                ARCH_FLOAT* flt = Optimizer::chosenAssembler->arch->flt_float;
                value->Truncate(flt->mantissa_bits, flt->exp_max, flt->exp_min);
            }
            //        else
            //            diag("CastToFloat: architecture characteristics for 'float' not set");
            break;
        case ISZ_DOUBLE:
        case ISZ_IDOUBLE:
            if (Optimizer::chosenAssembler->arch->flt_dbl)
            {
                ARCH_FLOAT* flt = Optimizer::chosenAssembler->arch->flt_dbl;
                value->Truncate(flt->mantissa_bits, flt->exp_max, flt->exp_min);
            }
            //        else
            //            diag("CastToFloat: architecture characteristics for 'double' not set");
            break;
        case ISZ_LDOUBLE:
        case ISZ_ILDOUBLE:
            if (Optimizer::chosenAssembler->arch->flt_ldbl)
            {
                ARCH_FLOAT* flt = Optimizer::chosenAssembler->arch->flt_ldbl;
                value->Truncate(flt->mantissa_bits, flt->exp_max, flt->exp_min);
            }
            //        else
            //            diag("CastToFloat: architecture characteristics for 'long double' not set");
            break;
    }
    return *value;
}
FPF dorefloat(Optimizer::SimpleExpression* node)
{
    FPF rv;
    FPF temp;
    switch (node->type)
    {
        case Optimizer::se_ui:
            rv = CastToFloat(ISZ_LDOUBLE, IntToFloat(&temp, ISZ_ULONGLONG, node->i));
            break;
        case Optimizer::se_i:
            rv = CastToFloat(ISZ_LDOUBLE, IntToFloat(&temp, -ISZ_ULONGLONG, node->i));
            break;
        case Optimizer::se_f:
        case Optimizer::se_fi:

            rv = CastToFloat(ISZ_FLOAT, &node->f);
            break;
        case Optimizer::se_fc:
            rv = CastToFloat(ISZ_FLOAT, &node->c.i);
            rv = CastToFloat(ISZ_FLOAT, &node->c.r);
            break;
        default:
            rv = 0;
            break;
    }
    return rv;
}
}  // namespace Optimizer