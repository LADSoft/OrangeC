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

#include <stdlib.h>

void* _RTL_FUNC bsearch(const void* key, const void* base, size_t num, size_t width,
                        int (*compare)(const void* elem1, const void* elem2))
{
    int bottom = -1;
    int top = num;
    int l;

    if (!num)
        return 0;

    while (top - bottom > 1)
    {
        int mid = (top + bottom) / 2;
        l = (*compare)(key, (char*)base + mid * width);

        if (l > 0)
            bottom = mid;
        else
            top = mid;
    }
    if (top == num)
        return 0;
    l = (*compare)(key, (char*)base + top * width);
    if (l)
        return 0;

    return (char*)base + top * width;
}
