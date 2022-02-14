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

#include <errno.h>
#include <wchar.h>

size_t wcsnrtombs(char* restrict dst, const wchar_t** restrict src, size_t nms, size_t len, mbstate_t* restrict p)
{
    wchar_t wc;
    size_t used = 0;
    const wchar_t* r = *src;
    (void)p;

    while (used < len && nms > 0)
    {
        wc = *r++;
        nms--;
        if (wc == L'\0')
        {
            if (dst != NULL)
                *dst = '\0';
            *src = NULL;
            return used;
        }
        else if (wc < 0x100)
        {
            if (dst != NULL)
                *dst++ = (char)wc;
            used++;
        }
        else
        {
            errno = EILSEQ;
            if (dst != NULL)
                *dst = '\0';
            *src = r - 1;
            return -1;
        }
    }

    *src = r;
    return used;
}
