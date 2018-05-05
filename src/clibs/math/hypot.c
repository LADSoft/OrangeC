/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
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

#include <math.h>
long double hypotl(long double x, long double y)
{
    #pragma STDC FP_CONTRACT OFF
    long double result, logbw ;
    int ilogbw = 0;
    if (x == 0.f && y == 0.f)
        return 0.f;
    logbw = logbl(fmaxl(fabsl(x), fabsl(y)));
    if (isfinite(logbw)) {
        ilogbw = (int)logbw;
        x = scalbnl(x, -ilogbw);
        y = scalbnl(y, -ilogbw);
    }
    result = sqrtl(x * x + y * y);

    result = scalbnl(result, ilogbw);

    return result ;
}
float hypotf(float x, float y)
{
    return hypotl(x,y);
}
double hypot(double x, double y)
{
    return hypotl(x,y);
}