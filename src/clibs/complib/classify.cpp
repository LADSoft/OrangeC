/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

#include <c/math.h>
#include <stdio.h>
int _RTL_FUNC (fpclassify)(float x)
{
    return fpclassify(x);
}

bool _RTL_FUNC (isfinite)(float x)
{
    return isfinite(x);
}
bool _RTL_FUNC (isinf)(float x)
{
    return isinf(x);
}
bool _RTL_FUNC (isnan)(float x)
{
    return isnan(x);
}
bool _RTL_FUNC (isnormal)(float x)
{
    return isnormal(x);
}

bool _RTL_FUNC (isgreater)(float x, float y)
{
    return x > y;
}
bool _RTL_FUNC (isgreaterequal)(float x, float y)
{
    return x >= y;
}
bool _RTL_FUNC (isless)(float x, float y)
{
    return x < y;
}
bool _RTL_FUNC (islessequal)(float x, float y)
{
    return x <= y;
}
bool _RTL_FUNC (islessgreater)(float x, float y)
{
    return x != y;
}
bool _RTL_FUNC (isunordered)(float x, float y)
{
    return isunordered(x, y);
}

int _RTL_FUNC(fpclassify)(double x)
{
    return fpclassify(x);
}

bool _RTL_FUNC(isfinite)(double x)
{
    return isfinite(x);
}
bool _RTL_FUNC(isinf)(double x)
{
    return isinf(x);
}
bool _RTL_FUNC(isnan)(double x)
{
    return isnan(x);
}
bool _RTL_FUNC(isnormal)(double x)
{
    return isnormal(x);
}

bool _RTL_FUNC(isgreater)(double x, double y)
{
    return x > y;
}
bool _RTL_FUNC(isgreaterequal)(double x, double y)
{
    return x >= y;
}
bool _RTL_FUNC(isless)(double x, double y)
{
    return x < y;
}
bool _RTL_FUNC(islessequal)(double x, double y)
{
    return x <= y;
}
bool _RTL_FUNC(islessgreater)(double x, double y)
{
    return x != y;
}
bool _RTL_FUNC(isunordered)(double x, double y)
{
    return isunordered(x, y);
}

int _RTL_FUNC(fpclassify)(long double x)
{
    return fpclassify(x);
}

bool _RTL_FUNC(isfinite)(long double x)
{
    return isfinite(x);
}
bool _RTL_FUNC(isinf)(long double x)
{
    return isinf(x);
}
bool _RTL_FUNC(isnan)(long double x)
{
    return isnan(x);
}
bool _RTL_FUNC(isnormal)(long double x)
{
    return isnormal(x);
}

bool _RTL_FUNC(isgreater)(long double x, long double y)
{
    return x > y;
}
bool _RTL_FUNC(isgreaterequal)(long double x, long double y)
{
    return x >= y;
}
bool _RTL_FUNC(isless)(long double x, long double y)
{
    return x < y;
}
bool _RTL_FUNC(islessequal)(long double x, long double y)
{
    return x <= y;
}
bool _RTL_FUNC(islessgreater)(long double x, long double y)
{
    return x != y;
}
bool _RTL_FUNC(isunordered)(long double x, long double y)
{
    return isunordered(x, y);
}
