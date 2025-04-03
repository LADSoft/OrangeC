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

#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

__time_t_64 _RTL_FUNC _time64(__time_t_64* tptr)
{
    __time_t_64 val;
    struct tm t2;
    __ll_gettime(&t2);
    val = _mktime64(&t2);
    // correct for mktime
    val -= _daylight * 60 * 60;
    if (tptr)
        *tptr = val;
    return val;
}
__time_t_32 _RTL_FUNC _time32(__time_t_32* tptr)
{
    time_t x = _time64(0);
    if (tptr)
        (*tptr) = x;
    return x;
}
