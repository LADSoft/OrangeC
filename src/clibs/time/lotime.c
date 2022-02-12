/* Software License Agreement
 * 
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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
 *     As a special exception, if other files instantiate templates or
 *     use macros or inline functions from this file, or you compile
 *     this file and link it with other works to produce a work based
 *     on this file, this file does not by itself cause the resulting
 *     work to be covered by the GNU General Public License. However
 *     the source code for this file must still be made available in
 *     accordance with section (3) of the GNU General Public License.
 *     
 *     This exception does not invalidate any other reasons why a work
 *     based on this file might be covered by the GNU General Public
 *     License.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

struct tm* _RTL_FUNC localtime(const time_t* time)
{
    struct tm* t;
    time_t t1 = *time;
    if (t1 & 0x80000000)
        return NULL;
    tzset();
    t1 -= _timezone;
    // the *2 is because gmtime adjusts for DST...
    t1 += _daylight * 60 * 60 * 2;
    if (t1 > 0x7fffffff)
        return NULL;
    t = gmtime(&t1);
    t->tm_isdst = _daylight;
    return t;
}
