/* Software License Agreement
 * 
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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
#include <windows.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"
#include <sys\stat.h>

int __ll_chsize(int handle, int size)
{
    int old = __ll_getpos(handle);
    char buf[256];
    int len;
    int rv;
    if (old == -1)
        return old;
    len = __ll_seek(handle, 0, SEEK_END);
    if (len == -1)
        return len;
    len = __ll_getpos(handle);
    if (len == -1)
        return len;
    if (len != size)
    {
        if (len < size)
        {
            memset(buf, 0, 256);
            len = size - len;
            while (len >= 256)
            {
                rv = __ll_write(handle, buf, 256);
                if (rv == -1 || rv != 256)
                    return -1;
                len -= 256;
            }
            if (len)
            {
                rv = __ll_write(handle, buf, len);
                if (rv == -1 || rv != len)
                    return -1;
            }
        }
        else
        {
            if (__ll_seek(handle, size, SEEK_SET) == -1)
                return -1;
            if (!SetEndOfFile((HANDLE)handle))
            {
                errno = GetLastError();
                return -1;
            }
        }
    }
    return __ll_seek(handle, old, SEEK_SET);
}
