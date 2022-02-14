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

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include <share.h>
#include "libp.h"

FILE* _RTL_FUNC freopen(const char* restrict name, const char* restrict mode, FILE* restrict stream)
{
    char buf[265];
    if (stream->token != FILTOK)
    {
        errno = _dos_errno = ENOENT;
        return 0;
    }
    if (!name)
        strcpy(buf, stream->extended->name);
    else
        strcpy(buf, name);
    free(stream->extended->name);
    __basefclose(stream, 0);
    stream->flags &= ~(_F_READ | _F_WRIT | _F_APPEND | _F_EOF | _F_ERR | _F_XEOF | _F_VBUF | _F_IN | _F_OUT);
    if (!__basefopen(buf, mode, stream, -1, SH_COMPAT))
    {
        return 0;
    }
    return stream;
}